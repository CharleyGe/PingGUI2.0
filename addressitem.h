#ifndef ADDRESSITEM_H
#define ADDRESSITEM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class addressItem : public QWidget
{
    Q_OBJECT
public:
    explicit addressItem(QWidget *parent = 0);
    ~addressItem();
    void setAddressName(QString address_text);
    QString getAddressName();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void removeAddressSig(QString &RA);
    void showAddress(QString &SA);

private:
    bool mouse_press;
    QLabel *address_name;
    QPushButton *delete_button;

public slots:
    void removeAddress();
};

#endif // ADDRESSITEM_H
