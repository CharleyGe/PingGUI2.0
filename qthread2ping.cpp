#include "qthread2ping.h"
#include <QDebug>

QThread2ping::QThread2ping(QObject* par):QThread(par)
{
   // p = new Pinger();
}

QThread2ping::~QThread2ping()
{
    qDebug()<<"QThread2ping::~QThread2ping()";
}

void QThread2ping::run()
{
   // p->ping();
    //emit testSignal(123);
}
