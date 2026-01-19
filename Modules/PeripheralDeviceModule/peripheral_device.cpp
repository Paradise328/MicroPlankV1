#include "peripheral_device.h"
#include <QTcpServer>

/*
 * https://blog.csdn.net/qq_42590121/article/details/116269269
 */


/**
  * @brief 无参数构造函数
  * @param None
  * @retval None
  */
Peripheral_Device::Peripheral_Device()
{
    this->ConnSta=DISCONNECTED;
    this->start();
}

/**
  * @brief 带参数的构造函数
  * @param ip和端口
  * @retval None
  */
Peripheral_Device::Peripheral_Device(QString ip, quint16 port)
{
    qRegisterMetaType<QByteArray>("QByteArray");
    qRegisterMetaType<QByteArray>("QByteArray&");
//    this->GetData=nullptr;
    this->ConnSta=DISCONNECTED;
    this->Ip=ip;
    this->Port=port;

    connect(this,&Peripheral_Device::FlushConns,this,&Peripheral_Device::OnFlushConns);

    //建立一个连接
    QTcpSocket *m_sockettemp = nullptr;
    m_sockettemp = new QTcpSocket(this);
    this->connect(m_sockettemp,&QTcpSocket::readyRead,this,&Peripheral_Device::on_Data_In);

    this->connect(m_sockettemp,&QTcpSocket::disconnected,this,&Peripheral_Device::on_Disconnected);

    QObject::connect(this, SIGNAL(Resend(QByteArray&)), this, SLOT(onResend(QByteArray&)));

    QHostAddress hostarrd(this->Ip);

    m_sockettemp->connectToHost(hostarrd,this->Port);

    m_sockettemp->waitForConnected(5000);

    if(m_sockettemp->isOpen())
    {
        ConList.append(m_sockettemp);

        this->m_socket = ConList[0];
        this->m_socketport = this->m_socket->peerPort();
        this->ConnSta = CONNECTED;
    }else{
        LOG(INFO)<<"open error ip:"<<ip.toStdString();
    }

    //冗余的连接用定时器来做
    this->ConnTimer=new QTimer;
    connect(this->ConnTimer,&QTimer::timeout,this,&Peripheral_Device::ConnTimer_Callback);
    this->ConnTimer->start(3000);

    //断线重连定时器
    this->ReconnTimer = new QTimer;
    connect(this->ReconnTimer,&QTimer::timeout,this,&Peripheral_Device::ReconnTimer_Callback);

    this->start();
}

/**
  * @brief 用来做冗余连接的定时器
  * @param None
  * @retval None
  */
void Peripheral_Device::ConnTimer_Callback(void)
{
    int ConCount = this->ConList.length();
    if(ConCount >= CON_COUNT_MAX){this->ConnTimer->stop();return;}

    QTcpSocket *m_sockettemp = nullptr;
    m_sockettemp = new QTcpSocket();
    this->connect(m_sockettemp, &QTcpSocket::readyRead, this, &Peripheral_Device::on_Data_In);
    this->connect(m_sockettemp, &QTcpSocket::disconnected, this, &Peripheral_Device::on_Disconnected);
    QObject::connect(this, SIGNAL(Resend(QByteArray&)), this, SLOT(onResend(QByteArray&)));

    m_sockettemp->setSocketOption(QAbstractSocket::KeepAliveOption, true);
    QHostAddress hostarrd(this->Ip);
    m_sockettemp->connectToHost(hostarrd, this->Port+ConCount);
    m_sockettemp->waitForConnected(1000);

    if(m_sockettemp->isOpen())
    {
        ConList.append(m_sockettemp);
        if(this->ConnSta == DISCONNECTED )
        {
            this->m_socket = m_sockettemp;
            // qDebug()<<"this->m_socket=m_sockettemp;";
        }
        this->ConnSta = CONNECTED;
    }
}

/**
  * @brief 重连定时器槽函数
  * @param None
  * @retval None
  */
void Peripheral_Device::ReconnTimer_Callback(void)
{
    qint16 porttemp=0;
    if(ReconnPortList.isEmpty()==false){
        porttemp=ReconnPortList[0];
    }
    else{
        porttemp=this->Port;
    }
    if(porttemp<this->Port)porttemp=this->Port;

    QTcpSocket *m_sockettemp=nullptr;
    m_sockettemp=new QTcpSocket();
    this->connect(m_sockettemp,&QTcpSocket::readyRead,this,&Peripheral_Device::on_Data_In);
  //  this->connect(m_sockettemp,&QTcpSocket::connected,this,&Peripheral_Device::on_Connected);
    this->connect(m_sockettemp,&QTcpSocket::disconnected,this,&Peripheral_Device::on_Disconnected);
    QObject::connect(this, SIGNAL(Resend(QByteArray&)), this, SLOT(onResend(QByteArray&)));

    QHostAddress hostarrd(this->Ip);

    m_sockettemp->connectToHost(hostarrd,porttemp);
    m_sockettemp->waitForConnected(1000);//这句话会跳到connected slot ，所以下面几句话是在跳到connected solt后执行的


    if(m_sockettemp->isOpen())
    {
      ConList.append(m_sockettemp);

      if(ReconnPortList.isEmpty()==false){
          ReconnPortList.removeAt(0);
      }
      if(this->ConnSta==DISCONNECTED )
      {
          this->m_socket=m_sockettemp;
          // qDebug()<<"this->m_socket=m_sockettemp;";
      }
      this->ConnSta = CONNECTED;
    }

    if(ReconnPortList.length()==0)this->ReconnTimer->stop();

}

/**
  * @brief 使用当前在用的连接发送数据
  * @param 需要发送的数据首地址和长度
  * @retval SEND_SUCCESS：发送成功；SEND_ERROR：发送失败
  */
eSendReturn Peripheral_Device::Send_Data(uint8_t data[],qint32 len)
{
    if(this->ConnSta!=CONNECTED) return SEND_ERROR;

    QByteArray datab=QByteArray((const char*)data,len);
    int datalen=datab.length();
    int mlen=0;
    if(m_socket==nullptr)qDebug()<<"Send_Data m_socket==nullptr";
Send1:
    mlen=(int)m_socket->write(datab);
    if(mlen==datalen)
    {
        m_socket->waitForBytesWritten(1000);
        return SEND_SUCCESS;
    }
    else if(mlen==-1)
    {
        if(this->Change_Usable_Con())goto Send1;
        else{
            // qDebug()<<"Send 1 All Connection Lost";
            this->ReconnTimer->start(1500);
            return SEND_ERROR;/*或者加异常处理代码*/
        }
        qDebug()<<"send error,mlen="<<mlen;
//        return SEND_ERROR;
    }
    else if(mlen<datalen)
    {
        if(this->Change_Usable_Con())goto Send1;
        else{
            // qDebug()<<"Send 2 All Connection Lost";
            this->ReconnTimer->start(1500);
            return SEND_ERROR;/*或者加异常处理代码*/
        }
        m_socket->waitForBytesWritten(1000);
        // qDebug()<<"send abort,mlen="<<mlen;
//        return SEND_ERROR;
    }
    return SEND_ERROR;

}

/**
  * @brief 使用当前在用的连接发送数据
  * @param 需要发送的数据
  * @retval SEND_SUCCESS：发送成功；SEND_ERROR：发送失败
  */
eSendReturn Peripheral_Device::Send_Data(QByteArray data_qba)
{
    if(this->ConnSta!=CONNECTED) return SEND_ERROR;

    int datalen=data_qba.length();
    int mlen=0;
    if(m_socket==nullptr)qDebug()<<"Send_Data m_socket==nullptr";
Send2:
    mlen=(int)m_socket->write(data_qba);
    if(mlen==datalen)
    {
        m_socket->waitForBytesWritten(1000);
        // qDebug()<<"send success";
        return SEND_SUCCESS;
    }
    else if(mlen == -1)
    {
        if(this->Change_Usable_Con())goto Send2;
        else{
            // qDebug()<<"Send 3 All Connection Lost";
            this->ReconnTimer->start(1500);
            return SEND_ERROR;/* 或者加异常处理代码 */
        }
        std::cout << "send error,mlen=" << mlen << std::endl;
    }
    else if(mlen < datalen)
    {
        if(this->Change_Usable_Con())goto Send2;
        else{
            // qDebug()<<"Send 4 All Connection Lost";
            this->ReconnTimer->start(1500);
            return SEND_ERROR;/*或者加异常处理代码*/
        }
        m_socket->waitForBytesWritten(1000);
        std::cout << "send abort, mlen = " << mlen << std::endl;
    }
    return SEND_ERROR;

}

/**
  * @brief 设置有信息收到的回调函数
  * @param 回调函数的函数指针
  * @retval None
  */
void Peripheral_Device::Datain_CallBack(QByteArray ba)
{

}

/**
  * @brief 有信息收到的槽函数，在里面调用getdata回调函数
  * @param None
  * @retval None
  */
void Peripheral_Device::on_Data_In(void)
{

    if(this->ConnSta!=CONNECTED) return;
    if(m_socket->canReadLine()>0)
    {  
       // qDebug()<<"datain";
        this->DataRecin+=m_socket->read(100);//用readline速度很慢，运行越久越慢
        int len=this->DataRecin.length();
        if(this->DataRecin.at(len-2)==0x0D)
        {
            emit NewDataIn(this->DataRecin);
            this->DataRecin.clear();
        }
    }

}
/**
  * @brief 拆数据包，收到的协议帧转为COMMU_FRAME类型
  * @param data：收到的协议帧，cf COMMU_FRAME类型参数的引用
  * @retval 成功或失败：Depack_SUCCESS：成功；Depack_ERROR：失败
  */
eDepackReturn Peripheral_Device::Depack_Frame(QByteArray data,COMMU_FRAME &cf)
{
    uint32_t CrCtemp=0;
    COMMU_FRAME cftemp;
    memset(&cftemp,0,sizeof(COMMU_FRAME));
    memcpy(&cftemp,data.data(),8);//从开头到ID结束
    cftemp.size=data[8];

    uint8_t datatemp=data.at(9);
    cftemp.sender=static_cast<eDeviceType>(datatemp);

    datatemp=data.at(10);
    cftemp.recever=(eDeviceType)datatemp;

    cftemp.payload.size=data[11];

    uint8_t dtemp=data[12];
    switch (dtemp)
    {
        case  0xf0:cftemp.payload.type=CMD_KEEP_ALIVE;break;
        case  0xf1:cftemp.payload.type=CMD_TEST;break;
        case  0xf2:cftemp.payload.type=CMD_CTRL_WRITE;break;
        case  0xf3:cftemp.payload.type=CMD_READ_DATA;break;
        case  0xf4:cftemp.payload.type=CMD_UPLOAD_DATA;break;
        case  0xf5:cftemp.payload.type=CMD_RES_CMD;break;
        default:{ LOG(ERROR) << "switch (dtemp) err!";return Depack_ERROR;}
    }
    cftemp.payload.argscounter=data[13];
    memcpy(cftemp.payload.args,data.data()+14,cftemp.payload.argscounter);
    memcpy(&(cftemp.crc),data.data()+data.length()-6,6);

    if(cftemp.preamble!=PREAMBLE){LOG(ERROR) << "cftemp.preamble!=PREAMBLE err!";return Depack_ERROR;}
    if(cftemp.sender!=this->type){LOG(ERROR) << "cftemp.sender!=this->type err!";return Depack_ERROR;}
    if(cftemp.recever!=DEV_HOST){LOG(ERROR) << "cftemp.recever!=DEV_HOST err!";return Depack_ERROR;}

    CrCtemp=CalcCrc16((uint8_t *)data.data(),cftemp.size-6/*data.length()-6*/);
    if(cftemp.crc!=CrCtemp){LOG(ERROR) << "cftemp.crc!=CrCtemp err!";return Depack_ERROR;}

    cf=cftemp;

    return Depack_SUCCESS;
}

/**
  * @brief 封装数据包，COMMU_FRAME类型转为协议帧
  * @param cf：COMMU_FRAME类型参数，datap：封装好的QByteArray类型数据，
  * @retval 封装好的数据帧长度
  */
uint8_t Peripheral_Device::Packet_Frame(QByteArray &datap,COMMU_FRAME cftemp)
{
    uint8_t framelen=cftemp.size;

    datap.append((const char *)(&(cftemp.preamble)),4);
    datap.append((const char *)(&(cftemp.ID)),4);
    datap.append((const char *)(&(cftemp.size)),1);
    datap.append((const char *)(&(cftemp.sender)),1);
    datap.append((const char *)(&(cftemp.recever)),1);

    datap.append((const char *)(&(cftemp.payload.size)),1);
    datap.append((const char *)(&(cftemp.payload.type)),1);
    datap.append((const char *)(&(cftemp.payload.argscounter)),1);
    datap.append((const char *)(cftemp.payload.args),cftemp.payload.argscounter);

    cftemp.crc=CalcCrc16((uint8_t *)datap.data(),framelen-6);//CRC本身和LF不参与计算
    //直接拿cftemp强转来算CRC会有问题，可能是大小端问题或对齐问题，或别的原因

    cftemp.LF=LINEFEED;
    datap.append((const char *)(&(cftemp.crc)),6);

    return framelen;
}


/**
  * @brief 刷新冗余连接槽函数，发送保活数据帧，由FlushConns信号触发
  * @param None
  * @retval None
  */
void Peripheral_Device::OnFlushConns(void)
{
    if(ConList.length()<=1)return;

    for(int i=1;i<ConList.length();i++)
    {
        auto item=ConList[i];

        uint8_t payloadsize=3;
        uint8_t framelen=17+payloadsize;
        COMMU_FRAME cf;
        memset(&cf,0,sizeof (COMMU_FRAME));
        cf.preamble=PREAMBLE;
        cf.ID=this->GetRandNum();
        cf.size=framelen;
        cf.sender=DEV_HOST;
        cf.recever=this->type;

        cf.payload.type=CMD_KEEP_ALIVE;
        cf.payload.size=payloadsize;
        cf.payload.argscounter=0;
        QByteArray dataarr;

        this->Packet_Frame(dataarr,cf);
        try
        {
          item->write(dataarr);
        }
        catch (std::exception e)
        {
           qWarning()<<"FlushConns err:"<<e.what();
        }
    }
}

/**
  * @brief 获取32位随机数，用于消息ID
  * @param None
  * @retval 32位随机数
  */
uint32_t Peripheral_Device::GetRandNum(void)
{
    // qsrand(QTime(0,0,0,0).msecsTo(QTime::currentTime()));
    // uint32_t randtemp=qrand();
    uint32_t randtemp=QRandomGenerator::global()->generate();

    // qsrand(QTime(0,0,0,0).msecsTo(QTime::currentTime()));
    randtemp<<=16;
    randtemp|=QRandomGenerator::global()->generate();
    return randtemp;
}

/**
  * @brief 把COMMU_FRAME类型数据封装并发送
  * @param cftemp：COMMU_FRAME类型数据；ReSendTimes：失败后重发次数；NeedAck:是否需要回执
  * @retval SEND_SUCCESS：发送成功；SEND_ERROR：发送失败
  */
eSendReturn Peripheral_Device::Send_Frame(COMMU_FRAME cftemp,uint8_t ReSendTimes,bool NeedAck)
{
    if(this->ConnSta!=CONNECTED) return SEND_ERROR;
    QByteArray DataQBA;
    this->Packet_Frame(DataQBA,cftemp);
    if(NeedAck)
    {
        Res_Sta RStemp;
        RStemp.ReSendLeft=ReSendTimes;
        RStemp.WaitTime=0;
        RStemp.Frame=DataQBA;
        this->Qhash_Res.insert(cftemp.ID,RStemp);
    }
    this->Send_Data(DataQBA);
    return SEND_SUCCESS;
}


/**
  * @brief 当前连接已不可用，切换成另一个可用的连接
  * @param None
  * @retval true：成功；false：失败
  */
bool Peripheral_Device::Change_Usable_Con()
{
     this->ConnSta = DISCONNECTED;
     try
     {
       this->m_socket->deleteLater();
     }
     catch (std::exception e)
     {
        qWarning()<<"Change_Usable_Con err:"<<e.what();
     }

    if(ConList.isEmpty())return false;
    ConList.removeAt(0);
    if(ConList.isEmpty())return false;
    this->m_socket=ConList[0];
    this->m_socketport=this->m_socket->peerPort();

    {
//====切换连接后发送一帧通信测试帧，来告知接收端当前用哪个端口
        uint8_t payloadsize=3;
        uint8_t framelen=17+payloadsize;
        COMMU_FRAME cf;
        memset(&cf,0,sizeof (COMMU_FRAME));
        cf.preamble=PREAMBLE;
        cf.ID=this->GetRandNum();
        cf.size=framelen;
        cf.sender=DEV_HOST;
        cf.recever=this->type;

        cf.payload.type=CMD_TEST;
        cf.payload.size=payloadsize;
        cf.payload.argscounter=0;
        QByteArray dataarr;

        this->Packet_Frame(dataarr,cf);
        try
        {
           this->m_socket->write(dataarr);
        }
        catch (std::exception e)
        {
           qWarning()<<"FlushConns err:"<<e.what();
        }
    }

    return true;
}

/**
  * @brief 线程运行函数
  * @param None
  * @retval None
  */
void Peripheral_Device::run()
{
    static uint32_t TimerCount=0;
    while(1)
    {
       if(this->Qhash_Res.isEmpty()==false)//维护回执队列，需要时重发
       {
           QHash<uint32_t,Res_Sta>::const_iterator it = Qhash_Res.constBegin();
           while (it != Qhash_Res.constEnd())
           {
               uint32_t key=it.key();Res_Sta value=it.value();

               if(value.WaitTime>=5)
               {
                  value.WaitTime=0;
                  if(value.ReSendLeft>0)
                  {
                      value.ReSendLeft--;
                      emit Resend(value.Frame);
                  }
                  else
                  {
                    qDebug()<<"Communication ERROR";
                  }
               }
               else
               {
                   value.WaitTime++;
               }
               this->Qhash_Res.insert(key,value);
               it++;
           }
       }
//       auto Qtime_next=QTime::currentTime().addMSecs(10);
//       while(QTime::currentTime()<Qtime_next)
//           QCoreApplication::processEvents(QEventLoop::AllEvents,10);
       this->DynamicsDelay(10);
       TimerCount++;
       if(TimerCount%1000==0)//60S应用层主动保活
       {
           emit this->FlushConns();
       }
    }
}

/**
  * @brief 重发槽函数，用Resend信号启动
  * @param dat需要重发数据的引用
  * @retval None
  */
void Peripheral_Device::onResend(QByteArray& dat)
{
    std::cout << "Resend!!!" << std::endl;
    Send_Data(dat);
}

/**
  * @brief 连接成功槽函数，每个连接都会响应
  * @param None
  * @retval None
  */
void Peripheral_Device::on_Connected(void)
{
   // this->ConnSta = CONNECTED;
    //  qDebug()<<"Connected！！！";
}

/**
  * @brief 连接断开槽函数，每个连接都会响应
  * @param None
  * @retval None
  */
void Peripheral_Device::on_Disconnected(void)
{
    // qDebug() << "DisConnected！！！";
    this->ConnSta = DISCONNECTED;
    if(this->m_socketport!=0)
    // qDebug() << "lose:this->m_socketport=" << this->m_socketport<<" this->ip="<<this->Ip;
    this->ReconnPortList.append(this->m_socketport);
    if(this-> Change_Usable_Con() == false)
    {
        this->ConnSta = DISCONNECTED;
       // qDebug()<<"All Connection Lost"<<this->Ip;
        /*或者加异常处理代码*/
    }
    else
    {
        this->ConnSta = CONNECTED;
    }
   this->ReconnTimer->start(1500);
}

void Peripheral_Device::DynamicsDelay(int nms)
{
    auto Qtime_next = QTime::currentTime().addMSecs(nms);
    while(QTime::currentTime() < Qtime_next)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents,10);
    }
}
