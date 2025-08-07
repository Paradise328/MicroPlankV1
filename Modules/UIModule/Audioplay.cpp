#include "Audioplay.h"

void AudioPlay::playMotionScaling(const int& t_teleOpScalingLevel){

    switch(t_teleOpScalingLevel){
    case 1:
        LOG(INFO) << "Playing sound: scaling factor 20";
        m_sounds["1"].play();
        while(m_sounds["1"].getStatus() == sf::Sound::Playing){
            sf::sleep(sf::milliseconds(50));
        }
        break;
    case 2:
        LOG(INFO) << "Playing sound: scaling factor 12";
        m_sounds["2"].play();
        while(m_sounds["2"].getStatus() == sf::Sound::Playing){
            sf::sleep(sf::milliseconds(50));
        }
        break;
    case 3:
        LOG(INFO) << "Playing sound: scaling factor 10";
        m_sounds["3"].play();
        while(m_sounds["3"].getStatus() == sf::Sound::Playing){
            sf::sleep(sf::milliseconds(50));
        }
        break;

    case 4:
        LOG(INFO) << "Playing sound: scaling factor 7";
        m_sounds["4"].play();
        while(m_sounds["4"].getStatus() == sf::Sound::Playing){
            sf::sleep(sf::milliseconds(50));
        }
        break;
    case 5:
        LOG(INFO) << "Playing sound: scaling factor 7";
        m_sounds["4"].play();
        while(m_sounds["4"].getStatus() == sf::Sound::Playing){
            sf::sleep(sf::milliseconds(50));
        }
        break;
    default:
        LOG(WARNING) << "WARNING: invalid velocity level";
        break;
    }
}

void AudioPlay::playEnableStatus(const int& t_enableStatus)
{
    if(t_enableStatus == 1){
        LOG(INFO) << "Playing sound for enable";
        m_sounds["enable"].play();
        while(m_sounds["enable"].getStatus() == sf::Sound::Playing){
            sf::sleep(sf::milliseconds(50));
        }
    }
}

void AudioPlay::SendInnerMsg(Module_Inner_E recever,int Action,QString arg)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender=Module_Inner_E::Audio;
    msgTemp.Recver=recever;
    msgTemp.Request.insert(Action,arg);
    m_messagePool.SendMessage(msgTemp);
}

void AudioPlay::SendInnerMsg(Module_Inner_E recever,int Action,QList<QString> arglist)
{
    Message_Inner_T msgTemp;
    msgTemp.Sender = Module_Inner_E::Audio;
    msgTemp.Recver = recever;
    for(int i=0;i<arglist.length();i++)
    {
        msgTemp.Request.insert(Action,arglist[i]);
    }
    m_messagePool.SendMessage(msgTemp);
}

void AudioPlay::dealWithMsg()
{
    Message_Inner_T msg;
    while(1)
    {
        m_MsgGottenRWLock.lockForRead();
        if(m_MsgGottenQueue.isEmpty()==false){
            msg=m_MsgGottenQueue.dequeue();
        }else{
            m_MsgGottenRWLock.unlock();
            break;
        }
        m_MsgGottenRWLock.unlock();
        if(msg.Recver!=Module_Inner_E::Audio&&msg.Recver!=Module_Inner_E::MultipleModules)
            return;

        QHash<int,QString>::ConstIterator i=msg.Request.constBegin();
        while(i!=msg.Request.constEnd())
        {
            switch (i.key())
            {
            case static_cast<int>(Voice_Action::VOICE_RecvMasterData):
            {

                QStringList slist=i.value().split(":");
                if(slist[0] == "speedCur")
                {
                    auto speed = (slist[1].toInt());
                    // playMotionScaling(speed);
                }
                if(slist[0] == "enableBoth")
                {
                    // playEnableStatus(1);
                }
                break;
            }
            case static_cast<int>(Voice_Action::VOCIE_SelfCheck):
            {
                break;
            }
            default:break;
            }
            i++;
        }
    }

}

void AudioPlay::GetAmMsg(Message_Inner_T msg)
{
    m_MsgGottenRWLock.lockForWrite();
    m_MsgGottenQueue.enqueue(msg);
    m_MsgGottenRWLock.unlock();
    emit DealMsgSignal();
}
