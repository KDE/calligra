#ifndef CALLIGRALIBS_KOCOLORSET_H
#define CALLIGRALIBS_KOCOLORSET_H

#include <KoResource.h>
#include <KoColor.h>
#include <QObject>

struct KoColorSetEntry
{
    KoColor color;
    QString name;
    bool operator==(const KoColorSetEntry& rhs) const { return name == rhs.name; }
};

class KoColorSet : public QObject, public KoResource
{
public:
    KoColorSet(const QString &filename = QString()) : QObject(), KoResource(filename) {}
    void add(const KoColorSetEntry &) {}
    void remove(const KoColorSetEntry &) {}
    KoColorSetEntry getColor(quint32 index) { return KoColorSetEntry(); }
    qint32 nColors() { return 0; }
};

#endif
