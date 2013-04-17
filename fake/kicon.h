#ifndef FAKE_KICON_H
#define FAKE_KICON_H

#include <QIcon>
#include "kofake_export.h"
class KOFAKE_EXPORT KIcon : public QIcon
{
public:
    KIcon(const QString &name = QString()) : QIcon() {}
    KIcon(const QIcon &icon) : QIcon(icon) {}
};

#endif
