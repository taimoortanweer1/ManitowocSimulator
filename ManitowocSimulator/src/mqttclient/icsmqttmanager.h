#ifndef ICSMQTTMANAGER_H
#define ICSMQTTMANAGER_H

#include <QObject>
#include <QUrl>
#include <QHash>

extern "C" {
#include "MQTTAsync.h"
}

void mqttOnConnect(void *context, MQTTAsync_successData *response);
void mqttOnConnectFailure(void *context, MQTTAsync_failureData *response);
void mqttOnDisconnect(void *context, MQTTAsync_successData *response);
void mqttOnConnectionLost(void *context, char *cause);

int mqttOnMessageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message);
void mqttOnSubscribed(void *context, MQTTAsync_successData *response);
void mqttOnSubscriptionFailure(void *context, MQTTAsync_failureData *response);
void mqttOnMessageSent(void *context, MQTTAsync_successData *response);
void mqttOnMessageDelivered(void *context, int token);
void mqttOnMessageSendFailure(void *context, MQTTAsync_failureData *response);


class IcsMqttMessage {
public:
    IcsMqttMessage() : token(-1), payload(nullptr) {}
    IcsMqttMessage(const QString &topic, const QByteArray &payload,
                   MQTTAsync_responseOptions responseOptions, int qos, int retain);
    IcsMqttMessage(const QString &topic, QByteArray &&payload,
                   MQTTAsync_responseOptions responseOptions, int qos, int retain);
    int token;
    QString topic;
    QByteArray payload;
    MQTTAsync_responseOptions responseOptions;
    MQTTAsync_message message;
};


class IcsMqttManager : public QObject
{
    Q_OBJECT
public:
    explicit IcsMqttManager(QObject *parent = nullptr);
    ~IcsMqttManager();

    enum ConnectionStates {
        Unknown = 0,
        Connected,
        Disconnected,
        Failed
    };

    void setBroker(const QString &host, int port);
    void setBroker(const QUrl &broker);
    void setClientId(const QString &id);

    bool connectToBroker();
    bool sendMessage(const QString &topic, const QByteArray &payload, int qos = 1, int retain = 0);
    bool sendMessage(const QString &topic, QByteArray &&payload, int qos = 1, int retain = 0);
    bool subscribeToTopic(const QString &topic);

public slots:
    void _connectedToBroker(MQTTAsync_successData *response);
    void _connectionToBrokerFailed(MQTTAsync_failureData *response);
    void _disconnectedFromBroker(QString cause);
    void _messageArrived(char *topicName, int topicLen, MQTTAsync_message *message);
    void _messageSent(int response);
    void _messageSendFailure(MQTTAsync_failureData *response);
    void _subscribedToTopic(MQTTAsync_successData *response);
    void _subscribedToTopicFailure(MQTTAsync_failureData *response);

signals:
    void connectionStateChanged(int state);
    void messageArrived(const QString &topicName, const QByteArray &payload);
    void messageSent(const QString &topicName);
    void messageSendFailed(const QString &topicName);
    void subscribedToTopic(const QString &topicName);
    void subscribeToTopicFailed(const QString &topicName);

private:
    QUrl m_brokerUrl;
    QString m_clientId;
    ConnectionStates m_connectionState;

    MQTTAsync m_mqttClient;
    MQTTAsync_connectOptions m_mqttConnectOptions;
    MQTTAsync_responseOptions m_mqttSubResponseOptions;
    MQTTAsync_responseOptions m_mqttMsgResponseOptions;


    QHash<int, IcsMqttMessage> m_outgoingMessageQueue; // identified by MQTTAsync_token (int)
    QHash<int,QString> m_topicSubscriptionQueue;
    QStringList m_subscribedTopics;

    void cleanUpToken(int token);

};

#endif // ICSMQTTMANAGER_H
