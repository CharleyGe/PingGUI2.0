#include "widget.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QDate>

void customMessageHandler(QtMsgType type, const QMessageLogContext &,const QString &msg)
{
    QString txt;
    QString tim = QDate::currentDate().toString("yyyy-MM-dd ")+QTime::currentTime().toString("hh:mm:ss")+QString("-");
    switch(type){
    case QtDebugMsg:
        txt = tim + QString("Debug: %1\n").arg(msg);
        break;

    case QtWarningMsg:
        txt = tim + QString("Warning: %1").arg(msg);
        break;

    case QtCriticalMsg:
        txt = tim + QString("Critical: %1").arg(msg);
        break;

    case QtFatalMsg:
        txt = tim + QString("Fatal:%1").arg(msg);
        //abort();
    }

    QFile outFile("debuglog.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts<<txt<<"\r\n"<<endl;
    if(QtFatalMsg == type){
        abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("../myPing/Ping_zh_CN.qm");
    if(QLocale::system().name()=="zh_CN")
        a.installTranslator(&translator);
    Widget w;
    w.setFixedSize(720,400);  //禁止改变窗口大小
    w.show();

    qDebug("----------debug message start ----------");

    return a.exec();
}
