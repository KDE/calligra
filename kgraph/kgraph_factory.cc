#include <kinstance.h>
#include <klocale.h>
#include <kaboutdata.h>

#include <kgraph_part.h>
#include <kgraph_factory.h>


extern "C" {
    void* init_libkgraph() {
        return new KGraphFactory;
    }
};

static const char *description=I18N_NOOP("KGraph - Graphs for Engineers");
static const char *version="0.1";

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

    bool bWantKoDocument=(strcmp(classname, "KoDocument")==0);

    KGraphPart *part = new KGraphPart(parent, name, !bWantKoDocument);

    if (!bWantKoDocument)
        part->setReadWrite(false);

    emit objectCreated(part);
    return part;
}

KAboutData* KGraphFactory::aboutData() {
        KAboutData *aboutData=new KAboutData("kgraph", I18N_NOOP("KGraph"),
                                             version, description, KAboutData::License_GPL,
                                             "(c) 2000, Werner Trobin"); 
        aboutData->addAuthor("Werner Trobin", 0, "wtrobin@carinthia.com");     
        return aboutData;
}

KInstance *KGraphFactory::global() {
    if (!s_global) {
        s_global=new KInstance(aboutData());
    }
    return s_global;
}
#include <kgraph_factory.moc>
