#include "addressitem.h"
#include <QtWidgets>
#include <QPixmap>

addressItem::addressItem(QWidget *parent) : QWidget(parent)
{
    mouse_press = false;
    address_name = new QLabel();
    delete_button = new QPushButton();
    QPixmap image(":/new/prefix1/img/close.png");
    delete_button->setIcon(image);
    delete_button->setFixedSize(image.size());
    connect(delete_button, SIGNAL(clicked()), this, SLOT(removeAddress()));

    QHBoxLayout *main_layout = new QHBoxLayout();
    main_layout->addWidget(address_name);
    main_layout->addStretch();
    main_layout->addWidget(delete_button);
    //main_layout->setContentsMargins(5,5,5,5);
    main_layout->setContentsMargins(2,2,2,2);
    this->setLayout(main_layout);
}

addressItem::~addressItem()
{

}

void addressItem::setAddressName(QString address_text)
{
    address_name->setText(address_text);
}

QString addressItem::getAddressName()
{
    QString address_name_text = address_name->text();
    return address_name_text;
}

void addressItem::removeAddress()
{
    QString address_name_text = address_name->text();
    emit removeAddressSig(address_name_text);
}

void addressItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        mouse_press = true;
    }
}

void addressItem::mouseReleaseEvent(QMouseEvent *event)
{
    if(mouse_press)
    {
        emit showAddress(address_name->text());
        mouse_press = false;
    }
}
