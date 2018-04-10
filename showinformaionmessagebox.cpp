#include "showinformationmessagebox.h"
#include <QTimer>
#include <QApplication>

showInformaionMessageBox::showInformaionMessageBox(const QString &title, const QString &message):
    m_title(title),
    m_message(message)
{

}

void showInformaionMessageBox::show(const QString &title, const QString &message)
{
    QEventLoop eventLoop;
    auto messageBox = new showInformaionMessageBox(title, message);
    connect(messageBox, SIGNAL(destroyed()), &eventLoop, SLOT(quit()));
    messageBox->readyShow();
    eventLoop.exec();
}

void showInformaionMessageBox::readyShow(void)
{
    this->moveToThread(qApp->thread());
    QTimer::singleShot(0,this,SLOT(onShow()));
}

void showInformaionMessageBox::onShow(void)
{
    QMessageBox::information(NULL, m_title, m_message);
    this->deleteLater();
}
