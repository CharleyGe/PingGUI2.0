#ifndef SHOWINFORMATIONMESSAGEBOX_H
#define SHOWINFORMATIONMESSAGEBOX_H

#include <QMessageBox>
#include <QEventLoop>

class showInformaionMessageBox:public QObject
{
    Q_OBJECT

private:
    const QString m_title;
    const QString m_message;

public:
    showInformaionMessageBox(const QString &title, const QString &message);

    static void show(const QString &title, const QString &message);

private:
    void readyShow(void);

private slots:
    void onShow(void);
};

#endif // SHOWINFORMATIONMESSAGEBOX_H
