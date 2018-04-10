#ifndef INNERLINEEDIT_H
#define INNERLINEEDIT_H

#include <QLineEdit>
#include <QStringListModel>

class InnerLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit InnerLineEdit(QWidget *parent=0);
    void setNormal();
    void setSearching();
    void addValue(const QString &value);
    void saveHist();
    void prepend(const QString &value);
    void addrRemove();
    void delimiterInsert(const int &position);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:
    void pressed();

private slots:
    void editComplete();

private:
    QStringList valueList;
    QStringListModel *listModel;
    int addrCount=0;
};

#endif // INNERLINEEDIT_H
