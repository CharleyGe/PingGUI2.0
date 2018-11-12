#include "widget.h"
#include <QBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QStringList>
#include <QHeaderView>
#include <QSizePolicy>
#include <QIcon>
#include <QListWidgetItem>
#include <QCompleter>
#include <QDebug>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowIcon(QIcon("mainIcon.png"));
    hostBox = new QGroupBox(tr("Host/Address"));
    Options = new QGroupBox(tr("Options"));

    pingButton = new QPushButton(tr("Ping"));
    stopButton = new QPushButton(tr("Stop"));
    pingButton->setFixedSize(100,30);
    stopButton->setFixedSize(100,30);
    stopButton->setEnabled(false);  //stop按钮不可用

    pingButton->setFocus();
    pingButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    pingButton->setShortcut(Qt::Key_Enter);   //按enter激活
    pingButton->setShortcut(Qt::Key_Return);

    ipaddrLabel = new QLabel(tr("IP Address:"));
    timeOut = new QLabel(tr("Timeout(in ms):"));
    bufferSize = new QLabel(tr("Buffer Size:"));
    Echo = new QLabel(tr("Echo Requests:"));
    ttl = new QLabel(tr("Time To Live:"));

    //ipaddr = new QLineEdit;
    //ipaddr->setText("www.weclassroom.com");
    //lineEdit = new InnerLineEdit;

    //lineEdit->setText("www.weclassroom.com");
    //关联lineEdit的信号槽
    //connect(lineEdit,SIGNAL(textChanged(QString)),this,SLOT(textChange(QString)));
    //connect(lineEdit,SIGNAL(pressed()),this,SLOT(editClicked()));

    //this->setFocus();
    //lineEdit->addValue("www.weclassroom.com");
    //lineEdit->addValue("scribbleapi0001.weclassroom.com");
    //lineEdit->addValue("api.weclassroom.com");
    //lineEdit->addValue("im.weclassroom.com");
    //lineEdit->addValue("client.weclassroom.com");


    //第二种方案以QCombox+QLineEdit组合实现搜索框
    //历史记录
    //记录提示
    //可以对记录进行删除操作
    address_combo_box = new QComboBox();
    address_combo_box->setFixedWidth(200);

    ipaddr = new QLineEdit;
    ipaddr->setFixedWidth(180);

    //建议网址
    addresses<<"www.weclassroom.com"<<"scribbleapi0001.weclassroom.com"<<"api.weclassroom.com"<<"im.weclassroom.com"<<"client.weclassroom.com";


    list_widget = new QListWidget();

    address_combo_box->setModel(list_widget->model());
    address_combo_box->setView(list_widget);

    //提示补齐到QLineEdit上
    QCompleter *completer = new QCompleter(ipaddr);
    listModel = new QStringListModel(addresses,ipaddr);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(listModel);
    ipaddr->setCompleter(completer);

    for(int i=0; i<addresses.size();++i){
        addressItem *address_item = new addressItem();
        address_item->setAddressName(addresses.at(i));
        connect(address_item, SIGNAL(showAddress(QString&)), this, SLOT(showAddress_slot(QString&)));
        connect(address_item, SIGNAL(removeAddressSig(QString&)), this, SLOT(removeAddress_slot(QString&)));  //历史列表更新
        connect(address_item, SIGNAL(removeAddressSig(QString&)), this, SLOT(deleteValue(QString&)));   //提示列表更新
        QListWidgetItem *list_item = new QListWidgetItem(list_widget);
        list_widget->setItemWidget(list_item, address_item);
    }

    timeOut_spin = new QSpinBox(this);
    timeOut_spin->setRange(100,10000);
    timeOut_spin->setSingleStep(100);
    timeOut_spin->setValue(1000);

    bufferSize_spin = new QSpinBox(this);
    bufferSize_spin->setRange(32,252);
    bufferSize_spin->setSingleStep(8);
    bufferSize_spin->setValue(32);

    echoRequest_spin = new QSpinBox(this);
    echoRequest_spin->setRange(5,65500);
    echoRequest_spin->setSingleStep(5);
    echoRequest_spin->setValue(10);

    ttl_spin = new QSpinBox(this);
    ttl_spin->setRange(10, 255);
    ttl_spin->setSingleStep(1);
    ttl_spin->setValue(128);

    QGridLayout *glayout = new QGridLayout;
    glayout->addWidget(timeOut, 0, 0, 1, 2);
    glayout->addWidget(timeOut_spin, 0, 2, 1, 1);
    glayout->addWidget(bufferSize, 0, 3, 1, 2);
    glayout->addWidget(bufferSize_spin, 0, 5, 1, 1);
    glayout->addWidget(Echo, 1, 0, 1, 2);
    glayout->addWidget(echoRequest_spin, 1, 2, 1, 1);
    glayout->addWidget(ttl, 1, 3, 1, 2);
    glayout->addWidget(ttl_spin, 1, 5, 1, 1);

    Options->setLayout(glayout);  //放入GroupBox中

    //左侧状态栏
    statusBarLeft = new QStatusBar();
    statusBarLeft->showMessage(tr("Ping Requests: 0"));
    statusBarLeft->setSizeGripEnabled(FALSE);

    //中间状态栏
    statusBarMid = new QStatusBar();
    statusBarMid->setSizeGripEnabled(FALSE);
    statusBarMid->resize(10,5);

    //版权状态栏
    statusBar = new QStatusBar();
    QLabel *permanent = new QLabel(this);
    permanent->setFrameStyle(QFrame::Box|QFrame::Sunken);
    permanent->setText(tr("Charley's Ping Soft"));
    statusBar->addPermanentWidget(permanent);
    statusBar->setSizeGripEnabled(FALSE);

    //状态栏水平放置
    QHBoxLayout *statusBox = new QHBoxLayout;
    statusBox->addWidget(statusBarLeft,1);
    statusBox->addWidget(statusBarMid,2);
    statusBox->addWidget(statusBar,1);

    //QHBoxLayout *hbox1=new QHBoxLayout;
    //hbox1->addWidget(ipaddrLabel);
    //hbox1->addWidget(ipaddr);
    //hbox1->addWidget(lineEdit);  //第一中方案以LineEdit实现搜索框
    //hbox1->addWidget(address_combo_box);
    //hbox1->addWidget(ipaddr,0,);

    //hack lineEdit+Qcompleter+QCobBox实现提示+历史记录
    QGridLayout *hbox1 = new QGridLayout;
    hbox1->addWidget(ipaddrLabel,0,0,1,1);
    hbox1->addWidget(address_combo_box,0,1,1,1);
    hbox1->addWidget(ipaddr,0,1,1,1);

    resolvhostName = new QCheckBox(tr("Resolve IP address to hostname"),this);

    QVBoxLayout *vbox1 = new QVBoxLayout;
    vbox1->addLayout(hbox1);
    vbox1->addWidget(resolvhostName);

    hostBox->setLayout(vbox1);  //加入GroupBox中



    QVBoxLayout *btonBox= new QVBoxLayout;
    btonBox->addWidget(pingButton);
    btonBox->addWidget(stopButton);
    btonBox->setSizeConstraint(QLayout::SetFixedSize);


    QHBoxLayout *hbox=new QHBoxLayout;
    hbox->addWidget(hostBox);
    hbox->addWidget(Options);
    hbox->addLayout(btonBox);

    //创建一个tableWidget对象
    tableWidget = new QTableWidget;
    tableWidget->setColumnCount(7);   //设置列数
    tableWidget->horizontalHeader()->setSectionsClickable(FALSE);
    //设置表头内容
    QStringList header;
    header<<tr("IP Address")<<tr("Host Name ")<<tr("Bytes")<<tr("Time")<<tr("TTL")<<tr("Error Message/Code")<<tr(" ");
    tableWidget->setHorizontalHeaderLabels(header);

    //设置表头字体加粗
    QFont font=tableWidget->horizontalHeader()->font();
    font.setBold(true);
    tableWidget->horizontalHeader()->setFont(font);

    tableWidget->horizontalHeader()->setStretchLastSection(true);//设置充满表宽度
    tableWidget->verticalHeader()->setDefaultSectionSize(20);//设置行高
    tableWidget->setFrameShape(QFrame::NoFrame);  //设置无边框
    tableWidget->setShowGrid(false);  //设置不现实格子线
    tableWidget->verticalHeader()->setVisible(false);  //设置垂直头不可见
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  //一次只能选择一行不可多选
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置不可编辑
    tableWidget->horizontalHeader()->resizeSection(0, 100);//设置表头第一列的宽度为100
    tableWidget->horizontalHeader()->resizeSection(1, 150);//设置表头第二列的宽度为150
    tableWidget->horizontalHeader()->resizeSection(2, 50); //设置表头第三列的宽度为50
    tableWidget->horizontalHeader()->resizeSection(3, 50); //四列50
    tableWidget->horizontalHeader()->resizeSection(4, 60); //五列50
    tableWidget->horizontalHeader()->resizeSection(5, 150);//六列
    tableWidget->horizontalHeader()->setFixedHeight(25);  //设置表头的高度
    tableWidget->setStyleSheet("selection-background-color:lightblue;"); // 设置选中背景色
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置表头背景色
    tableWidget->setFocusPolicy(Qt::NoFocus);  //去除焦点
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);  //禁止表头移动

    QVBoxLayout *mainLayout=new QVBoxLayout;
    mainLayout->addLayout(hbox);
    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(statusBox);
    setLayout(mainLayout);

    connect(pingButton,SIGNAL(clicked()),this,SLOT(pingAction()));
    connect(pingButton,SIGNAL(clicked()),this,SLOT(saveHist()));  //记住历史
    connect(stopButton,SIGNAL(clicked()),this,SLOT(pingStop()));

    //全局线程的创建
   /* ping = new Pinger();
    connect(ping, &Pinger::sendData, this, &Widget::showData);
    connect(ping, &Pinger::sendTime, this, &Widget::showDataTime);
    connect(ping, &Pinger::sendHostName, this, &Widget::showHostName);
    connect(ping, &Pinger::sendBytes, this, &Widget::showBytes);
    connect(ping, &QThread::finished, this, &Widget::onThreadFinish);
    //connect(ping, SIGNAL(finished()), ping, SLOT(deleteLater()));   //线程结束后执行析构函数销毁线程
    connect(ping, &Pinger::sendTTL, this, &Widget::showTTL);
    connect(ping, &Pinger::sendErrMsg, this, &Widget::showErrMsg);
    connect(ping, &Pinger::updatePingRequests, this, &Widget::updateStatusBarLeft);
    connect(ping, &Pinger::sendResultData, this, &Widget::updateStatusBarMid);
    connect(ping, &Pinger::pingCanceled, this, &Widget::pingCanceled);*/
}

void Widget::pingAction()
{
     ping = new Pinger();  //开启一个局部线程
     connect(ping, &Pinger::sendData, this, &Widget::showData);
     connect(ping, &Pinger::sendTime, this, &Widget::showDataTime);
     connect(ping, &Pinger::sendHostName, this, &Widget::showHostName);
     connect(ping, &Pinger::sendBytes, this, &Widget::showBytes);
     connect(ping, &QThread::finished, this, &Widget::onThreadFinish);
     connect(ping, SIGNAL(finished()), ping, SLOT(deleteLater()));   //线程结束后执行析构函数销毁线程
     connect(ping, &Pinger::sendTTL, this, &Widget::showTTL);
     connect(ping, &Pinger::sendErrMsg, this, &Widget::showErrMsg);
     connect(ping, &Pinger::updatePingRequests, this, &Widget::updateStatusBarLeft);
     connect(ping, &Pinger::sendResultData, this, &Widget::updateStatusBarMid);
     connect(ping, &Pinger::pingCanceled, this, &Widget::pingCanceled);
     connect(ping, &Pinger::btnEnable, this, &Widget::btnEnabled);  //线程结束开启ping按钮
     connect(ping, &Pinger::addrInvalidUpdateStatus, this, &Widget::updateStatusAddrInvalid);
    //
    if(resolvhostName->isChecked())
        this->ping->setResolveHostName(TRUE);
    else
        this->ping->setResolveHostName(FALSE);

    tableWidget->setRowCount(0);  //连行也清除掉
    pingAddr = ipaddr->text();
    //pingAddr = lineEdit->text();
    //pingAddr = address_combo_box->currentText();
    packSize = bufferSize_spin->value();
    this->ping->setPingAddr(pingAddr);//把要ping 的地址传递给子线程
    this->ping->setPackSize(packSize);  //把发送的数据包大小传递给pinger类
    this->ping->setTimeOut(timeOut_spin->value());  //把超时数值传递给pinger类
    this->ping->setPingTimes(echoRequest_spin->value()); //把ping的次数传递给pinger
    this->ping->setTTL(ttl_spin->value());  //把ttl值传递给pinger类
    pingButton->setEnabled(false);//ping按钮不可用
    stopButton->setEnabled(true); //stop按钮可用
    stopButton->setShortcut(QKeySequence::InsertParagraphSeparator);//设置Enter快捷键
    stopButton->setShortcut(Qt::Key_Enter);
    stopButton->setShortcut(Qt::Key_Return);


    statusBarMid->showMessage(tr("Trying to Ping Address ")+pingAddr+"...");
    //lineEdit->saveHist();  //保存历史

    ping->start();
    statusBarLeft->showMessage("Ping Requests: 0");
    qDebug()<<"Local PC-name"<<QHostInfo::localHostName()<<endl;
    qDebug()<<"Trying to ping address "<<pingAddr<<endl;
    qDebug()<<"ping has started..."<<endl;
}

void Widget::pingStop()
{
    statusBarMid->showMessage(tr("Stopping...Please wait!"));
    if(this->ping->isRunning())
    {
        this->ping->stopImmediately();  //结束线程
    }
    return;
}

void Widget::onQthreadFinished()
{

    qDebug()<<"pingThread finish"<<endl;
}

void Widget::showData(const QString &str)
{
   row_count = tableWidget->rowCount();
   tableWidget->insertRow(row_count);
   QTableWidgetItem *item = new QTableWidgetItem();
   item->setText(str);
   item->setTextAlignment(0x0001);
   tableWidget->setItem(row_count, 0, item);
}

void Widget::showDataTime(const QString &timestamp)
{
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setText(QString(timestamp+" ms"));
    item->setTextAlignment(0x0004);
    tableWidget->setItem(row_count, 3, item);
}

void Widget::showHostName(const QString &hostNameinUI)
{
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setText(hostNameinUI);
    item->setTextAlignment(0x0001);
    tableWidget->setItem(row_count, 1, item);
}

void Widget::showBytes(const int &bytesinUI)
{
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setText(QString("%1").arg(bytesinUI));
    item->setTextAlignment(0x0004);
    tableWidget->setItem(row_count, 2, item);
}

void Widget::showTTL(const QString &ttlData)
{
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setText(ttlData);
    item->setTextAlignment(0x0004);
    tableWidget->setItem(row_count, 4, item);
}

void Widget::showErrMsg(const QString &errMsg)
{
    qDebug()<<"showErrMsg has called."<<endl;
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setText(errMsg);
    item->setTextAlignment(0x0004);
    tableWidget->setItem(row_count, 5, item);
}

void Widget::updateStatusBarLeft(const int &pingTimes)
{
    statusBarLeft->showMessage(tr("Ping Requests: %1").arg(pingTimes));
}

void Widget::updateStatusBarMid(const int &sent, const int &rcv)
{
    statusBarMid->showMessage(tr("Packets: Sent=%1,Received=%2,Lost=%3(%4%loss)").arg(sent).arg(rcv).arg(sent-rcv).arg((sent-rcv)*100/sent));
}

void Widget::pingCanceled()
{
    statusBarLeft->showMessage(tr("Ping Canceled!"));
}

void Widget::onThreadFinish()
{
    qDebug()<<"pingThread finish"<<endl;
    stopButton->setEnabled(false); //stop按钮不可用
    pingButton->setEnabled(true);  //ping按钮可用
}

Widget::~Widget()
{
    qDebug() <<"start destory widget";
    qDebug() <<"----------debug message finished----------";
}

void Widget::addValue(const QString &value)
{
    addresses.append(value);
    listModel->setStringList(addresses);
}

void Widget::saveHist()
{
    //首先判断是否在列表中存在这个网址
    bool canAdd=true;
    int list_count = list_widget->count();
    for(int i=0; i<list_count; i++)
    {
        QListWidgetItem *item = list_widget->item(i);
        addressItem *address_item = (addressItem*)(list_widget->itemWidget(item));

        QString address_name = address_item->getAddressName();

        if(pingAddr == address_name)
        {
            canAdd = false;
            break;
        }
    }
    if(canAdd)
    {
        addressItem *address_item = new addressItem();
        if(pingAddr!=NULL)
        {
            address_item->setAddressName(pingAddr);
            connect(address_item, SIGNAL(showAddress(QString&)), this, SLOT(showAddress_slot(QString&)));
            connect(address_item, SIGNAL(removeAddressSig(QString&)), this, SLOT(removeAddress_slot(QString&)));
            connect(address_item, SIGNAL(removeAddressSig(QString&)), this, SLOT(deleteValue(QString&)));
            QListWidgetItem *list_item = new QListWidgetItem(list_widget);
            list_widget->setItemWidget(list_item, address_item);
        }
    }

    //QCompleter列表中元素更新
    if(QString::compare(pingAddr, QString(""))!=0){
        bool flag = addresses.contains(pingAddr, Qt::CaseInsensitive);
        if(!flag){
            addValue(pingAddr);
        }
    }
}

void Widget::btnEnabled()
{
    stopButton->setEnabled(false);  //stop按钮不可用
    pingButton->setEnabled(true);  //ping按钮可用
}

void Widget::updateStatusAddrInvalid()
{
    statusBarMid->showMessage(tr(""));
}

void Widget::showAddress_slot(QString &SAS)
{
    //address_combo_box->setEditText(SAS);
    ipaddr->setText(SAS);
    address_combo_box->hidePopup();
}

void Widget::removeAddress_slot(QString &RAS)
{
    //address_combo_box->hidePopup();
    int list_count = list_widget->count();
    for(int i=0; i<list_count; i++)
    {
        QListWidgetItem *item = list_widget->item(i);
        addressItem *address_item = (addressItem*)(list_widget->itemWidget(item));

        QString address_name = address_item->getAddressName();

        if(RAS == address_name)
        {
            list_widget->takeItem(i);
            delete item;
            break;
        }
    }
}

void Widget::deleteValue(QString &pingAddr)
{
    if(QString::compare(pingAddr, QString(""))!=0){
        bool flag = addresses.contains(pingAddr, Qt::CaseInsensitive);
        if(flag){
           //删除该字符串
            addresses.removeOne(pingAddr);
            listModel->setStringList(addresses);   //删除元素后对QCompleter列表进行更新
            qDebug()<<"addresses delete one successfully!"<<endl;
        }
    }
}
