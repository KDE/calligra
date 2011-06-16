#ifndef KOBALLOON_H
#define KOBALLOON_H

#include <QWidget>
#include <QPainter>

class KoBalloon : public QWidget
{
    Q_OBJECT
public:
    explicit KoBalloon(QString content, int position, QWidget *parent = 0);

    int y() {return m_y;}
    virtual void paintEvent(QPaintEvent *event);

private:
    QString m_content;
    int m_y;

};

#endif // KOBALLOON_H
