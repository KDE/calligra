#ifndef FAKE_KNOTIFICATION_H
#define FAKE_KNOTIFICATION_H

#include <QObject>

class KNotification : public QObject
{
    Q_OBJECT
public:
    KNotification(const QString &name = QString()) : QObject() { setObjectName(name.toUtf8()); }
    void setText(const QString &text) {}
    void addContext(const QString &key, const QString &value) {}
public Q_SLOTS:
    void sendEvent() { deleteLater(); }
};

#endif
 
