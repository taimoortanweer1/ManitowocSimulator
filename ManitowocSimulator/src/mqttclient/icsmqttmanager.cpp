#include "icsmqttmanager.h"
#if ! defined QT_NO_DEBUG_OUTPUT
//#define QT_NO_DEBUG_OUPUT
#endif
#include <QDebug>

#include <QTimer>


IcsMqttMessage::IcsMqttMessage(const QString &topic, const QByteArray &payload,
                               MQTTAsync_responseOptions responseOptions, int qos, int retain) :
    token(-1),
    topic(topic),
    payload(payload),
    responseOptions(responseOptions),
    message(MQTTAsync_message_initializer)
{
    message.payload = this->payload.data();
    message.payloadlen = this->payload.length();
    message.qos = qos;
    message.retained = retain; // What is this?
}

IcsMqttMessage::IcsMqttMessage(const QString &topic, QByteArray &&payload,
                               MQTTAsync_responseOptions responseOptions, int qos, int retain) :
    token(-1),
    topic(topic),
    payload(payload),
    responseOptions(responseOptions),
    message(MQTTAsync_message_initializer)
{
    message.payload = this->payload.data();
    message.payloadlen = this->payload.length();
    message.qos = qos;
    message.retained = retain; // What is this?
}

IcsMqttManager::IcsMqttManager(QObject *parent) : QObject(parent),
    m_connectionState(Disconnected),
    m_mqttClient(new MQTTAsync),
    m_mqttConnectOptions(MQTTAsync_connectOptions_initializer),
    m_mqttSubResponseOptions(MQTTAsync_responseOptions_initializer),
    m_mqttMsgResponseOptions(MQTTAsync_responseOptions_initializer)
{
    m_mqttConnectOptions.onSuccess = mqttOnConnect;
    m_mqttConnectOptions.onFailure = mqttOnConnectFailure;
    m_mqttConnectOptions.context = this;

    m_mqttSubResponseOptions.onSuccess = mqttOnSubscribed;
    m_mqttSubResponseOptions.onFailure = mqttOnSubscriptionFailure;
    m_mqttSubResponseOptions.context = this;

    m_mqttMsgResponseOptions.onSuccess = NULL;
    m_mqttMsgResponseOptions.onFailure = mqttOnMessageSendFailure;
    m_mqttMsgResponseOptions.context = this;
}

IcsMqttManager::~IcsMqttManager()
{
    MQTTAsync_destroy(&m_mqttClient);
}

void IcsMqttManager::setBroker(const QString &host, int port)
{
    qWarning() << Q_FUNC_INFO <<  "trying to set broker to " << QStringLiteral("tcp://%1:%2").arg(host).arg(port);
    setBroker(QUrl(QStringLiteral("tcp://%1:%2").arg(host).arg(port)));
}

void IcsMqttManager::setBroker(const QUrl &broker)
{
    m_brokerUrl = broker;
}

void IcsMqttManager::setClientId(const QString &id)
{
    m_clientId = id;
}

bool IcsMqttManager::connectToBroker()
{
    if(!m_brokerUrl.isValid() || m_clientId.isEmpty())
    {
        qWarning() << Q_FUNC_INFO << "Broker URL and client id must be set before connecting";
        return false;
    }
    int rc = MQTTAsync_create(&m_mqttClient, m_brokerUrl.toString().toLatin1(),
                              m_clientId.toLatin1(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if(rc != MQTTASYNC_SUCCESS)
    {
        qWarning() << Q_FUNC_INFO << "Failed to create client";
        return false;
    }

    rc = MQTTAsync_setCallbacks(m_mqttClient, this, mqttOnConnectionLost, mqttOnMessageArrived, mqttOnMessageDelivered);
    if(rc != MQTTASYNC_SUCCESS)
    {
        qWarning() << Q_FUNC_INFO << "Failed to set callbacks" << rc;
        return false;
    }

    rc = MQTTAsync_connect(m_mqttClient, &m_mqttConnectOptions);
    if(rc != MQTTASYNC_SUCCESS)
    {
        QString reason;
        switch(rc)
        {
        case 1:
            reason = QLatin1String("Connection refused: Unacceptable protocol version");
            break;
        case 2:
            reason = QLatin1String("Connection refused: Identifier rejected");
            break;
        case 3:
            reason = QLatin1String("Connection refused: Server unavailable");
            break;
        case 4:
            reason = QLatin1String("Connection refused: Bad user name or password");
            break;
        case 5:
            reason = QLatin1String("Connection refused: Not authorized");
            break;
        default:
            reason = QLatin1String("Unknown error");
        }
        qWarning() << Q_FUNC_INFO << "Failed to connect to host" << rc << reason;
        return false;
    }

    qWarning() << Q_FUNC_INFO << "Attempting to connect to" << m_brokerUrl.toString();
    return true;
}

bool IcsMqttManager::sendMessage(const QString &topic, const QByteArray &payload, int qos, int retain)
{
    IcsMqttMessage msg = {topic, payload, m_mqttMsgResponseOptions, qos, retain};

    int rc = MQTTAsync_sendMessage(m_mqttClient, msg.topic.toLatin1(), &(msg.message), &(msg.responseOptions));

    if(rc != MQTTASYNC_SUCCESS)
    {
        qWarning() << Q_FUNC_INFO << "Message send failed";
        return false;
    }

    msg.token = msg.responseOptions.token;
    m_outgoingMessageQueue.insert(msg.token, msg);
   /* qDebug() << Q_FUNC_INFO << "Sending message with token" << msg.token << "to Topic: " << topic << "and payload len of"
             << msg.payload.length() << "and payload:" << msg.payload;
             */

    return true;
}

bool IcsMqttManager::sendMessage(const QString &topic, QByteArray &&payload, int qos, int retain)
{
    IcsMqttMessage msg = {topic, std::move(payload), m_mqttMsgResponseOptions, qos, retain};

    int rc = MQTTAsync_sendMessage(m_mqttClient, msg.topic.toLatin1(), &(msg.message), &(msg.responseOptions));

    if(rc != MQTTASYNC_SUCCESS)
    {
        qWarning() << Q_FUNC_INFO << "Message send failed";
        return false;
    }

    msg.token = msg.responseOptions.token;
    m_outgoingMessageQueue.insert(msg.token, msg);
   /* qDebug() << Q_FUNC_INFO << "Sending message with token" << msg.token << "to Topic: " << topic << "and payload len of"
             << msg.payload.length() << "and payload:" << msg.payload; */

    return true;
}

bool IcsMqttManager::subscribeToTopic(const QString &topic)
{
    MQTTAsync_responseOptions opts = m_mqttSubResponseOptions;
    int rc = MQTTAsync_subscribe(m_mqttClient, topic.toLatin1(), 1, &opts);
    if(rc != MQTTASYNC_SUCCESS)
    {
        qWarning() << Q_FUNC_INFO << "Subscription failure for topic" << topic;
        return false;
    }
    qWarning() << Q_FUNC_INFO << "Subscribing to" << topic;
    m_topicSubscriptionQueue.insert(opts.token, topic);
    return true;
}

void IcsMqttManager::_connectedToBroker(MQTTAsync_successData *response)
{
    qWarning() << Q_FUNC_INFO << "Connection succeeded:" << response->token;
    m_connectionState = Connected;
    emit connectionStateChanged(m_connectionState);
}

void IcsMqttManager::_connectionToBrokerFailed(MQTTAsync_failureData *response)
{
    qWarning() << Q_FUNC_INFO << "Connection failed";
    if(response)
        qWarning() << Q_FUNC_INFO << "Reason:" << response->code << response->message;
    m_connectionState = Failed;
    emit connectionStateChanged(m_connectionState);
}

void IcsMqttManager::_disconnectedFromBroker(QString cause)
{
    qDebug() << Q_FUNC_INFO << "Disconnected from broker:" << cause;
    m_connectionState = Disconnected;
    emit connectionStateChanged(m_connectionState);
}

void IcsMqttManager::_messageArrived(char *topicName, int topicLen, MQTTAsync_message *message)
{
    QByteArray data = QByteArray(static_cast<char*>(message->payload), message->payloadlen);
    qDebug() << Q_FUNC_INFO << topicName << topicLen << data.length() << data;

    emit messageArrived(QLatin1String(topicName), data);
}

void IcsMqttManager::_messageSent(int token)
{
   // qDebug() << Q_FUNC_INFO << token;
    if(m_outgoingMessageQueue.contains(token))
    {
        emit messageSent(m_outgoingMessageQueue.value(token).topic);
        cleanUpToken(token);
    }
}

void IcsMqttManager::_messageSendFailure(MQTTAsync_failureData *response)
{
    if(!response)
        return;
    qWarning() << Q_FUNC_INFO << "Token:" << response->token << "Message:" << response->message;
    if(m_outgoingMessageQueue.contains(response->token))
    {
        emit messageSendFailed(m_outgoingMessageQueue.value(response->token).topic);
        cleanUpToken(response->token);
    }
}

void IcsMqttManager::_subscribedToTopic(MQTTAsync_successData *response)
{
    if(!response)
        return;
    if(m_topicSubscriptionQueue.contains(response->token))
    {
        qDebug() << Q_FUNC_INFO << "subscribed to" << m_topicSubscriptionQueue.value(response->token);
        m_subscribedTopics.append(m_topicSubscriptionQueue.value(response->token));
        emit subscribedToTopic(m_topicSubscriptionQueue.value(response->token));
        m_topicSubscriptionQueue.remove(response->token);
    }
}

void IcsMqttManager::_subscribedToTopicFailure(MQTTAsync_failureData *response)
{
    if(!response)
        return;
    if(m_topicSubscriptionQueue.contains(response->token))
    {
        qWarning() << Q_FUNC_INFO << "Topic:" << m_topicSubscriptionQueue.value(response->token) << "Token:"
                 << response->token << "Message:" << response->message;
        emit subscribeToTopicFailed(m_topicSubscriptionQueue.value(response->token));
        m_topicSubscriptionQueue.remove(response->token);
    }
}

void IcsMqttManager::cleanUpToken(int token)
{
    if(m_outgoingMessageQueue.contains(token))
    {
        m_outgoingMessageQueue.take(token);
    } else
        qWarning() << Q_FUNC_INFO << "Warning: attempted to delete data for non-existant token";
}


void mqttOnConnectionLost(void *context, char *cause)
{
    qWarning() << Q_FUNC_INFO << context << cause;
    if(context)
        static_cast<IcsMqttManager*>(context)->_disconnectedFromBroker(QLatin1String(cause));
}

int mqttOnMessageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    if(context)
        static_cast<IcsMqttManager*>(context)->_messageArrived(topicName, topicLen, message);
    return 1; // Must return 1 to tell mqtt that we handled the message
}

void mqttOnConnect(void *context, MQTTAsync_successData *response)
{
    if(context)
        static_cast<IcsMqttManager*>(context)->_connectedToBroker(response);
}

void mqttOnConnectFailure(void *context, MQTTAsync_failureData *response)
{
    if(context)
        static_cast<IcsMqttManager*>(context)->_connectionToBrokerFailed(response);
}

// Either mqttOnMessageSent or mqttOnMessageDelivered will be called, depending on the QOS
void mqttOnMessageSent(void *context, MQTTAsync_successData *response)
{
    if(context && response)
        static_cast<IcsMqttManager*>(context)->_messageSent(response->token);
}

void mqttOnMessageSendFailure(void *context, MQTTAsync_failureData *response)
{
    if(context)
        static_cast<IcsMqttManager*>(context)->_messageSendFailure(response);
}

void mqttOnMessageDelivered(void *context, int token)
{
    if(context)
        static_cast<IcsMqttManager*>(context)->_messageSent(token);
}

void mqttOnSubscribed(void *context, MQTTAsync_successData *response)
{
    if(context)
        static_cast<IcsMqttManager*>(context)->_subscribedToTopic(response);
}

void mqttOnSubscriptionFailure(void *context, MQTTAsync_failureData *response)
{
    if(context)
        static_cast<IcsMqttManager*>(context)->_subscribedToTopicFailure(response);
}
