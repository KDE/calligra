#ifndef KOBALLOON_H
#define KOBALLOON_H

#include <QWidget>

class KoBalloon : public QWidget
{
    Q_OBJECT
public:
    explicit KoBalloon(QString content, QWidget *parent = 0);

private:
    QString m_content;

};

#endif // KOBALLOON_H
