#ifndef KGRAPH_FACTORY_H
#define KGRAPH_FACTORY_H

#include <klibloader.h>

class KInstance;


class KGraphFactory : public KLibFactory {

    Q_OBJECT

public:
    KGraphFactory(QObject *parent=0, const char *name=0);
    ~KGraphFactory();

    virtual QObject *create(QObject *parent=0, const char *name=0, const char *classname="QObject", const QStringList &args=QStringList());

    static KInstance *global();

private:
    static KInstance *s_global;
};
#endif
