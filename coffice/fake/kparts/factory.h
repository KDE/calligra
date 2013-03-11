#ifndef FAKE_KPARTS_FACTORY_H
#define FAKE_KPARTS_FACTORY_H

#include <kparts/part.h>
#include <QObject>

namespace KParts {

    class Factory : public QObject
    {
    public:
        Factory(QObject *parent = 0, const char *name = 0) : QObject(parent) {}
    };

}

#endif
 
