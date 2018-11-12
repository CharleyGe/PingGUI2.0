#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <pinger.h>
#include <QThread>
#include <QCheckBox>
#include <QSpinBox>
#include <QStatusBar>
#include <QIcon>
#include <QComboBox>
#include <QListWidget>
#include <QtNetwork>
#include "addressitem.h"

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QCheckBox;
class QSpinBox;
class QStatusBar;
class QComboBox;
class QListWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

signals:
    //void searchTextChanged(QString);

private:
    QGroupBox *hostBox;
    QGroupBox *Options;
    QPushButton *pingButton;
    QPushButton *stopButton;
    QLabel *ipaddrLabel;
    QLabel *timeOut;
    QLabel *bufferSize;
    QLabel *Echo;
    QLabel *ttl;
    QLineEdit *ipaddr;
    QTableWidget *tableWidget;
    Pinger *ping;
    int row_count;
    QString pingAddr;
    int packSize=0;
    QCheckBox *resolvhostName;
    QSpinBox *timeOut_spin;
    QSpinBox *bufferSize_spin;
    QSpinBox *echoRequest_spin;
    QSpinBox *ttl_spin;
    QStatusBar *statusBar;
    QStatusBar *statusBarLeft;
    QStatusBar *statusBarMid;
    QIcon icon;
    //QLineEdit的私有数据如下
    //InnerLineEdit *lineEdit;
    //第二种方案以combox实现搜索框便于删除历史记录带有Qcompleter补全机制
    QComboBox *address_combo_box;
    QListWidget *list_widget;
    QStringList addresses;  //QCompleter和QcomboBox下拉菜单中元素列表
    QStringListModel *listModel;  //QCompleter使用

private slots:
    void pingAction();  //开始ping
    void pingStop();  //结束ping
    void onQthreadFinished();
    void showData(const QString &str);
    void showDataTime(const QString &timestamp);
    void showHostName(const QString &hostNameinUI);
    void showBytes(const int &bytesinUI);
    void showTTL(const QString &ttlData);
    void showErrMsg(const QString &errMsg);
    void updateStatusBarMid(const int &sent, const int &rcv);
    void updateStatusBarLeft(const int &pingTimes);
    void pingCanceled();
    void onThreadFinish();
    void btnEnabled();
    //lineEdit的信号槽如下
    //void textChange(QString text);
    //void editClicked();
    //void btnClicked();
    void updateStatusAddrInvalid();
    //QCombobox的信号槽如下
    void showAddress_slot(QString &SAS);
    void removeAddress_slot(QString &RAS);
    void saveHist();  //记录历史
    void addValue(const QString &value);  //往Qcompleterlist中添加新的条目
    void deleteValue(QString &value);
};

#endif // WIDGET_H
