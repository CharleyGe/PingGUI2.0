#include "innerlineedit.h"

#include <QtWidgets>
#include <QCompleter>

InnerLineEdit::InnerLineEdit(QWidget *parent)
    :QLineEdit(parent)
{
    setTextMargins(0,0,this->height(),0);

    QCompleter *completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->setMaxVisibleItems(11);
    listModel = new QStringListModel(valueList,this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(listModel);


    this->setCompleter(completer);
    connect(this,SIGNAL(editingFinished()),this,SLOT(editComplete()));
}

void InnerLineEdit::setNormal()
{
    setText("");
    //setTextMargins(-5,0,this->height(),0);
    //setStyleSheet
}

void InnerLineEdit::setSearching()
{
    setTextMargins(0,0,this->height(),0);
    //setStyleSheet
}

void InnerLineEdit::addValue(const QString &value)
{
    valueList.append(value);
    listModel->setStringList(valueList);
}

void InnerLineEdit::prepend(const QString &value)
{
    valueList.prepend(value);
    listModel->setStringList(valueList);
}

void InnerLineEdit::addrRemove()
{
    valueList.removeAt(4);
    listModel->setStringList(valueList);
}

void InnerLineEdit::delimiterInsert(const int &position)
{
    qDebug()<<position<<endl;
    valueList.insert(position-1,"-------我是分隔符别点我-------");
    listModel->setStringList(valueList);
}

void InnerLineEdit::focusInEvent(QFocusEvent *event)
{
    emit pressed();
    QLineEdit::focusInEvent(event);
}

void InnerLineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
}

void InnerLineEdit::editComplete()
{
    QString text = this->text();
    if(QString::compare(text,QString(""))!=0){
        bool flag = valueList.contains(text,Qt::CaseInsensitive);
        if(!flag){
            addrCount++;
            if(addrCount==1){
                delimiterInsert(addrCount);
            }
            if(addrCount>5)
            {
                addrRemove();  //删去位置5上的元素
            }
            prepend(text);  //在列表开头添加历史记录
        }
    }
}

void InnerLineEdit::saveHist()
{
    editComplete();
}
