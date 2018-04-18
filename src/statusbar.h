#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QObject>
#include <QLabel>

class StatusBar : public QObject
{
    Q_OBJECT
public:
    StatusBar(QWidget *parent=nullptr);
    void SetText(int index,QString text);
    void Clear();
private:
    QList<QLabel*> labels;
};

#endif // STATUSBAR_H
