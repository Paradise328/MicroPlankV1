#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QReadWriteLock>
#include <QHash>
#include <QWaitCondition>
#include <QDebug>

template<typename T>
class ThreadSafeQueue : public QObject
{
public:
    explicit ThreadSafeQueue(QObject *parent = nullptr);

    void push_back(const T &value);
    void push_front(const T &value);
    T dequeue(void);
    void clear(void);
    int length(void);
    bool isEmpty();

private:
    QQueue<T>   m_Queue;
  //  QReadWriteLock m_ReadWriteMutex;
    QMutex m_Mutex;
    QWaitCondition m_Cond;

};

//对于模板类，以下这种函数的具体实现必须全部写在h文件里，或
//直接全部写在类里面，不能写在CPP里面
template<typename T>
ThreadSafeQueue<T>::ThreadSafeQueue(QObject *parent) : QObject(parent)
{
    m_Queue.clear();
}

template<typename T>
void ThreadSafeQueue<T>:: push_back(const T &value)
{
    m_Mutex.lock();
    m_Queue.push_back(value);
    m_Mutex.unlock();
    m_Cond.wakeOne();
}

template<typename T>
void ThreadSafeQueue<T>:: push_front(const T &value)
{
    m_Mutex.lock();
    m_Queue.push_front(value);
    m_Mutex.unlock();
    m_Cond.wakeOne();
}

template<typename T>
T ThreadSafeQueue<T>:: dequeue(void)
{
   T valueTemp;
   m_Mutex.lock();
   while(m_Queue.length()==0){
        m_Cond.wait(&m_Mutex);
   }
   valueTemp = m_Queue.dequeue();

   m_Mutex.unlock();
   return valueTemp;
}

template<typename T>
void ThreadSafeQueue<T>::clear(void)
{
    m_Mutex.lock();
    m_Queue.clear();
    m_Mutex.unlock();
}

template<typename T>
int ThreadSafeQueue<T>::length()
{
    m_Mutex.lock();
    int i_temp=m_Queue.length();
    m_Mutex.unlock();
    return i_temp;
}

template<typename T>
bool ThreadSafeQueue<T>::isEmpty()
{
    m_Mutex.lock();
    bool b_temp=m_Queue.isEmpty();
    m_Mutex.unlock();
    return b_temp;
}



#endif // THREADSAFEQUEUE_H
