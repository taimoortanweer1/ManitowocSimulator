#include "controller_impl.h"
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <QProcess>
#include <QTimer>
#include <QTime>
#include <QThread>
#include <unistd.h>
#include "PavMain2/presentationcontroller.hpp"
#include "PavMain2/musiccontroller.hpp"
#include "presentationcontroller_impl.h"
#include "PavMain2/voicecontroller.hpp"
#include "gm_mqtt.h"

//MQTT Config
//const QString MQTT_BROKER_IP = QLatin1String("192.168.1.21");
const QString MQTT_BROKER_IP = QLatin1String("127.0.0.1");
const int MQTT_BROKER_PORT = 1883;

const QMap<QByteArray, QString> USER_THEME_MAP = {
    { "MaryBarra", QStringLiteral("Mary") },
    { "SteveCarlisle", QStringLiteral("Steve") },
    { "MikeSimcoe", QStringLiteral("Mike") },
    { "AndrewSmith", QStringLiteral("Andrew") },
    { "BryanNesbitt", QStringLiteral("Bryan") },
    { "MarkReuss", QStringLiteral("Mark") }
};
const QString FALLBACK_THEME = QStringLiteral("DefaultTheme");

ControllerImpl::ControllerImpl(QObject *parent):
    PavMain2::Controller(parent)
  , m_mqttManager(new IcsMqttManager(this))
  , m_tripTimer(new QTimer(this))
  , m_backgroundTimer(new QTimer(this))
{
    m_tripTimer->setInterval(1000);
    connect(m_tripTimer, &QTimer::timeout,
            this, [this]() -> void {
                if (tripProgress() < tripProgressMax()) {
                    if (tripProgress() < sectionProgressMax()) {
                        setTripProgress(tripProgress() + 1);
                    }
                } else {
                    m_tripTimer->stop();
                }
            });

    connect(this, &Controller::sectionProgressMinChanged, this, [this]() -> void {
                setTripProgress(sectionProgressMin());
                startTripTimer();
            });

    m_backgroundTimer->setInterval(20000);
    m_backgroundTimer->setSingleShot(true);
    connect(m_backgroundTimer, &QTimer::timeout,
            this, [this]() -> void {
                emit Controller::nextBackground();
    });

    connect(m_mqttManager, &IcsMqttManager::messageArrived,
            this, [this](const QString &topic, const QByteArray &payload) -> void {
        if (topic.compare(MQTT_PRESENTATION_STATES_TOPIC) == 0) {
            PavMain2::PresentationController *pc = nullptr;

            pc = getContextObject<PavMain2::PresentationController>();
            pc->showLoopOff();
            if (pc) {
                if (payload == MQTT_PRESENTATION_STATE_IDLE) {
                    pc->idle();
                    PavMain2::Controller *c = getContextObject<Controller>();
                    c->setScreenState(QLatin1String("3"));
                } else if (payload == MQTT_PRESENTATION_STATE_WELCOME) {
                    pc->welcome();
                } else if (payload == MQTT_PRESENTATION_STATE_ENTER_VEHICLE) {
                    pc->enterVehicle();
                } else if (payload == MQTT_PRESENTATION_STATE_START_RIDE) {
                    pc->startRide();
                } else if (payload == MQTT_PRESENTATION_STATE_RELAX_TRANSITION) {
                    pc->relaxTransition();
                } else if (payload == MQTT_PRESENTATION_STATE_RELAX_FIRE) {
                    pc->relaxFire();
                } else if (payload == MQTT_PRESENTATION_STATE_MUSIC) {
                    pc->socialMusic();
                } else if (payload == MQTT_PRESENTATION_STATE_QANDA) {
                    pc->qAndA();
                    PavMain2::VoiceController *vc = nullptr;
                    vc = getContextObject<PavMain2::VoiceController>();
                    vc->startListeningAnimation();

                } else if (payload == MQTT_PRESENTATION_STATE_RELAXING) {
                    pc->relaxing();

                } else if (payload == MQTT_PRESENTATION_STATE_PICKUP_TRANSITION) {
                    pc->pickupTransition();
                } else if (payload == MQTT_PRESENTATION_STATE_SOCIAL_DRINKS) {
                    pc->socialDrinks();
                } else if (payload == MQTT_PRESENTATION_STATE_PHOTO_1) {
                    pc->groupPhoto();
                } else if (payload == MQTT_PRESENTATION_STATE_PHOTO_2) {
                    pc->photoShare();
                } else if (payload == MQTT_PRESENTATION_STATE_EXIT) {
                    pc->exitVehicle();
                } else if (payload == MQTT_PRESENTATION_STATE_APPROACH) {
                    pc->approachArrival();
                }
            }
        } else if (topic.compare(MQTT_GESTURE_TOPIC) == 0) {
            qDebug() << "Gesture Topic";
            if(payload == MQTT_GESTURE_SWIPE_RIGHT) {
                qWarning() << Q_FUNC_INFO << "Gesture Right Received";
                emit gestureMoveForward();

            } else if(payload == MQTT_GESTURE_SWIPE_LEFT) {
                emit gestureMoveForward();
            } else if(payload == MQTT_GESTURE_SWIPE_UP) {
                emit gestureMoveForward();
            } else if(payload == MQTT_GESTURE_SWIPE_DOWN) {
                emit gestureSwipeDown();
            } else if(payload == MQTT_GESTURE_TAP) {
                emit gestureMoveForward();
            }

        } else if( topic.compare(MQTT_SHUTDOWN_TOPIC) == 0) {
            qDebug() << "Trying to shutdown!!!";
            QProcess* process = new QProcess();
            process->start(QLatin1String("/home/pav/shutdown.sh"));
        } else if (topic.compare(MQTT_SET_USER_TOPIC) == 0) {
            context()->activateTheme(USER_THEME_MAP.value(payload, FALLBACK_THEME));
            qWarning() << "Setting user name to: " << USER_THEME_MAP.value(payload, QString::fromLatin1(""));
            PavMain2::VoiceController * vc = getContextObject<PavMain2::VoiceController>();
            vc->setUserName(USER_THEME_MAP.value(payload, QString::fromLatin1("")));
        }else if (topic.compare(MQTT_SHOW_LOOP_TOPIC) == 0) {
            PavMain2::PresentationController *pc = nullptr;

            pc = getContextObject<PavMain2::PresentationController>();
            qWarning() << "Setting show loop" << payload;
            if(payload == MQTT_ON)
            {
                pc->showLoopOn();
            } else if (payload == MQTT_OFF) {
                pc->showLoopOff();
            }

        }else if (topic.compare(MQTT_SPEACH_RESPONSE_TOPIC) == 0) {
            auto vc = getContextObject<PavMain2::VoiceController>();
            vc->playTextResponse(QString::fromLatin1(payload));
        }else if (topic.compare(MQTT_TAKE_PICTURE_TOPIC) == 0) {
            emit selfieRequest();
            qDebug() << "Taking Selfie";
        }else if (topic.compare(MQTT_WHERE_TOPIC) == 0) {
            auto vc = getContextObject<PavMain2::VoiceController>();
            vc->receivedWhereCmd();
        }else if (topic.compare(MQTT_APPROACH_TOPIC) == 0) {
            auto vc = getContextObject<PavMain2::VoiceController>();
            vc->receivedApproachCmd();
        }else if (topic.compare(MQTT_UPLOAD_TOPIC) == 0) {
            emit uploadRequested();
        }else if (topic.compare(MQTT_CAPTION_TOPIC) == 0) {
            emit captionRequested();
        }else if (topic.compare(MQTT_MIC_MUTE_TOPIC) == 0) {
            if(payload == MQTT_ON)
            {
                auto vc = getContextObject<PavMain2::VoiceController>();
                vc->stopListeningAnimation();
            }
        }else if (topic.compare(MQTT_MUSIC_TOPIC) == 0) {
            PavMain2::MusicController *mc = getContextObject<PavMain2::MusicController>();
            if(payload == MQTT_PLAY_MUSIC)
            {
                mc->setMediaPlayerActive(true);
            } else if(payload == MQTT_STOP_MUSIC) {
                mc->setMediaPlayerActive(false);
            }
        }

    });


    m_mqttManager->setBroker(MQTT_BROKER_IP, MQTT_BROKER_PORT);
    m_mqttManager->setClientId(QLatin1String("testApp"));
    connect(m_mqttManager, &IcsMqttManager::connectionStateChanged,
            this, [this](int status) -> void {
        if (status == IcsMqttManager::ConnectionStates::Connected) {
            m_mqttManager->subscribeToTopic(MQTT_PRESENTATION_STATES_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_GESTURE_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_SET_USER_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_SHOW_LOOP_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_SPEACH_RESPONSE_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_TAKE_PICTURE_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_WHERE_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_UPLOAD_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_CAPTION_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_MIC_MUTE_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_MUSIC_TOPIC);
            m_mqttManager->subscribeToTopic(MQTT_APPROACH_TOPIC);
        }
    });

    m_mqttManager->connectToBroker();

    connect(this ,&Controller::screenStateChanged,[=](){
        qWarning() << Q_FUNC_INFO << MQTT_MAIN_SCREEN_TOPIC << this->screenState().toLatin1() ;
        this->m_mqttManager->sendMessage( MQTT_MAIN_SCREEN_TOPIC,  this->screenState().toLatin1() );
    });
    connect(this, &Controller::scentStateChanged, [=](){
        qWarning() << Q_FUNC_INFO << MQTT_SCENTS_TOPIC << " Scent state = " << this->scentState();
        this->m_mqttManager->sendMessage( MQTT_SCENTS_TOPIC, QString::number(this->scentState()).toLatin1());
    });
}

void ControllerImpl::selfieImagePulled(QString path)
{
    path.prepend(qApp->applicationDirPath() + QString::fromLatin1("/"));
    path.prepend(QString::fromLatin1("file://"));
    setSelfieImagePath(path);
}
void ControllerImpl::setCaptureCameraFeed(OpenCVCamView *camera)
{
    if (m_captureCameraFeed != camera) {
        m_captureCameraFeed = camera;
        connect(m_captureCameraFeed, &OpenCVCamView::newImagePulled, this, &ControllerImpl::selfieImagePulled);
        emit captureCameraFeedChanged();
    }
}

void ControllerImpl::setTripProgress(int tripProgress)
{
    Controller::setTripProgress(tripProgress);
    setTimeTillDestination(QTime(0, 0).addSecs(tripProgressMax() - tripProgress).toString(QStringLiteral("mm:ss")));
}

void ControllerImpl::startTripTimer()
{
    m_tripTimer->start();
}

void ControllerImpl::takeSelfie()
{
    setSelfieCountDownActive(false);
    if (m_captureCameraFeed) {
        qDebug() << "Taking selfie";
        QString imageName = QString::fromLatin1("selfie_")
                + QDateTime::currentDateTime().toString() + QString::fromLatin1(".png");
        m_captureCameraFeed->pullCameraImage(imageName);
    }
}

void ControllerImpl::startBackgroundTimer()
{
    m_backgroundTimer->start();
}

void ControllerImpl::uploadSelfie()
{
    QTimer::singleShot(1500, this, [this]() -> void {setUploadAnimationActive(true);});
}

void ControllerImpl::startVaporFire()
{
    qWarning() << Q_FUNC_INFO << "m_mqttManager->sendMessage("<<MQTT_VAPORFIRE_TOPIC<<", \"1\");";
    m_mqttManager->sendMessage(MQTT_VAPORFIRE_TOPIC, "1");
}

void ControllerImpl::stopVaporFire()
{
    qWarning() << Q_FUNC_INFO << "m_mqttManager->sendMessage("<<MQTT_VAPORFIRE_TOPIC<<", \"0\");";
    m_mqttManager->sendMessage(MQTT_VAPORFIRE_TOPIC, "0");
}

void ControllerImpl::turnOnSpa()
{
    m_mqttManager->sendMessage(MQTT_LED_SPA_TOPIC, "0");
}

void ControllerImpl::openMinibar()
{
    m_mqttManager->sendMessage(MQTT_MINIBAR_TOPIC, MQTT_OPEN);
}

void ControllerImpl::closeMinibar()
{
    m_mqttManager->sendMessage(MQTT_MINIBAR_TOPIC, MQTT_CLOSE);
}

void ControllerImpl::openArmrest()
{
    m_mqttManager->sendMessage(MQTT_ARMRESET_SCREEN_TOPIC, MQTT_OPEN);
}

void ControllerImpl::closeArmrest()
{
    m_mqttManager->sendMessage(MQTT_ARMRESET_SCREEN_TOPIC, MQTT_CLOSE);
}

void ControllerImpl::startListening()
{
    m_mqttManager->sendMessage(MQTT_MIC_MUTE_TOPIC, MQTT_OFF);
}

void ControllerImpl::stopListening()
{
    m_mqttManager->sendMessage(MQTT_MIC_MUTE_TOPIC, MQTT_ON);
    PavMain2::VoiceController *vc = getContextObject<PavMain2::VoiceController>();
    vc->stopListeningAnimation();
}

void ControllerImpl::startDelayTimer(int delayMs)
{
    QTimer::singleShot(delayMs, [this]() {
        emit delayTimerFinished();
    });
}
