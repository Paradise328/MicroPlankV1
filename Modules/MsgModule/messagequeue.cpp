#include "messagequeue.h"

MessageQueue::MessageQueue(QObject *parent): ThreadSafeQueue<Message_Inner_T>(parent)
{}


void MessageQueue::SendMessage(const Message_Inner_T &msg)
{

    Message_Inner_T *item = new Message_Inner_T;
    item->Recver = msg.Recver;
    item->Sender = msg.Sender;
    item->Request = msg.Request;
    push_back(*item);
}

Message_Inner_T MessageQueue::GetMessage(void)
{
    auto item = dequeue();
    return item;
}
