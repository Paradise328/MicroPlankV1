#ifndef AUDIOPLAY_H
#define AUDIOPLAY_H

#include <unordered_map>
#include <SFML/Audio.hpp>
#include <iostream>
#include <future>
#include <thread>
#include <unistd.h>
#include "../LoggerModule/easylogging++.h"
#include "../MsgModule/messagequeue.h"

class AudioPlay: public QObject
{
    Q_OBJECT
public:
    AudioPlay() = delete;
    AudioPlay(const std::unordered_map<std::string, std::string>& audioMap, MessageQueue&  messagePool):
        m_audioMap(audioMap),
        m_messagePool(messagePool)
    {
        connect(this, &AudioPlay::DealMsgSignal, this, &AudioPlay::dealWithMsg);
    }

    void GetAmMsg(Message_Inner_T msg);

signals:
    void DealMsgSignal();

private:

    std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;

    std::unordered_map<std::string, sf::Sound>       m_sounds;

    const std::unordered_map<std::string, std::string> m_audioMap;

    void playMotionScaling(const int& t_teleOpScalingLevel);

    void playEnableStatus(const int& t_enableStatus);

    /*MessageQueue relative function*/
    MessageQueue            &m_messagePool;
    QQueue<Message_Inner_T> m_MsgGottenQueue;
    QReadWriteLock          m_MsgGottenRWLock;
    void                    SendInnerMsg(Module_Inner_E recever,int Action,QString arg);
    void                    SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist);
    void                    dealWithMsg();

};

#endif // AUDIOPLAY_H
