#include <kinstance.h>

#include <kgraph_part.h>
#include <kgraph_factory.h>


extern "C" {
    void* init_libkgraph() {
        return new KGraphFactory;
    }
};

KInstance *KGraphFactory::s_global=0;

KGraphFactory::KGraphFactory(QObject *parent, const char *name)
                             : KLibFactory(parent, name) {
}

KGraphFactory::~KGraphFactory() {
    if (s_global)
        delete s_global;
}

QObject *KGraphFactory::create(QObject *parent, const char *name, const char *classname, const QStringList &) {

/*
    if (parent && !parent->inherits("KoDocument")) {
        qDebug("ExampleFactory: parent does not inherit KoDocument");
        return 0;
    }
*/

    bool bWantKoDocument=(strcmp(classname, "KofficeDocument")==0);

    KGraphPart *part = new KGraphPart(parent, name, !bWantKoDocument);

    if (!bWantKoDocument)
        part->setReadWrite(false);

    emit objectCreated(part);
    return part;
}

KInstance *KGraphFactory::global() {
    if (!s_global)
        s_global=new KInstance("kgraph");
    return s_global;
}
#include <kgraph_factory.moc>
