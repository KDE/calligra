#ifndef FAKE_KICON_H
#define FAKE_KICON_H

#include <QIcon>

class KIcon : public QIcon
{
public:
    KIcon(const QString &name = QString()) : QIcon() {}
    KIcon(const QIcon &icon) : QIcon(icon) {}
};

#endif
