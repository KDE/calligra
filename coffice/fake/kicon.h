#ifndef FAKE_KICON_H
#define FAKE_KICON_H

#include <QIcon>

#define koIconWanted(x,y) KIcon()

class KIcon : public QIcon
{
public:
    KIcon(const QString &name = QString()) : QIcon() {}
    KIcon(const QIcon &icon) : QIcon(icon) {}
};

#endif
