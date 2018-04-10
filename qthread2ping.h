#ifndef QTHREAD2PING_H
#define QTHREAD2PING_H
#include <QThread>
#include <pinger.h>

class QThread2ping:public QThread
{
    Q_OBJECT

public:
    QThread2ping(QObject* par);
    ~QThread2ping();
    void run();

signals:
    void testSignal(int);

private:
    Pinger *p;
};

#endif // QTHREAD2PING_H
