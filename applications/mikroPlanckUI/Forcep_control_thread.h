#ifndef FORCEP_CONTROL_THREAD_H
#define FORCEP_CONTROL_THREAD_H

#include <QThread>


class Forcep_Control_Thread : public QThread
{
    Q_OBJECT
public:
    explicit Forcep_Control_Thread(QObject *parent = nullptr);
    void run();

    void onPageChange(int p);

signals:
    void  get_Pos();

private:
    bool InOpreationPage=false;
};

#endif // FORCEP_CONTROL_THREAD_H
