#include <kglobal.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kinstance.h>
#include <kaboutdata.h>

#include "kimage_factory.h"
#include "kimage_doc.h"

static const char* description=I18N_NOOP("KOffice Image Viewer");
static const char* version="0.1";

extern "C"
{
    void* init_libkimage()
    {
	return new KImageFactory;
    }
};

KInstance* KImageFactory::s_global = 0;

KImageFactory::KImageFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
}

KImageFactory::~KImageFactory()
{
  if ( s_global ) 
    delete s_global;
}

QObject* KImageFactory::create( QObject* parent, const char* name, const char*classname, const QStringList & )
{
/*
   if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KImageFactory: parent does not inherit KoDocument");
	return 0;
    }
*/

    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KImageDocument *doc = new KImageDocument( parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      doc->setReadWrite( false );

    emit objectCreated(doc);
    return doc;
}

KAboutData* KImageFactory::aboutData()
{
     KAboutData *aboutData= new KAboutData( "kimage", I18N_NOOP("KImage"),
                                            version, description, KAboutData::License_GPL,
                                            "(c) 1998-2000, Michael Koch");
     aboutData->addAuthor("Michael Koch",0, "koch@kde.org");
     return aboutData;
}

KInstance* KImageFactory::global()
{
   if ( !s_global )
   {
     s_global = new KInstance(aboutData());
   }
   return s_global;
}

#include "kimage_factory.moc"
