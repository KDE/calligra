#include "kformula_factory.h"
#include "kformula_doc.h"

#include <kinstance.h>


#include <kfiledialog.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kimgio.h>
#include <klocale.h>
#include <kaboutdata.h>

#include <qstringlist.h>


KInstance* KFormulaFactory::s_global = 0;

extern "C"
{
    void* init_libkformulapart()
    {
	return new KFormulaFactory;
    }
};


KAboutData* KFormulaFactory::aboutData()
{
      KAboutData *aboutData= new KAboutData( "kformula","KFormula",
        "0.2", "KDE formulaeditor", KAboutData::License_GPL,
        "(c) 1998-2000, Andrea Rizzi");
      aboutData->addAuthor("Andrea Rizzi",0, "rizzi@kde.org");
      return aboutData;
}


KFormulaFactory::KFormulaFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    s_global = new KInstance( "kformula" );
}

KFormulaFactory::~KFormulaFactory()
{
    delete s_global;
}

QObject* KFormulaFactory::create( QObject* parent, const char* name, const char* /*classname*/, const QStringList & )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KFormulaFactory: parent does not inherit KoDocument");
	return 0;
    }
    KFormulaDoc *doc = new KFormulaDoc( (KoDocument*)parent, name );
    emit objectCreated(doc);
    return doc;
}

KInstance* KFormulaFactory::global()
{
    return s_global;
}

#include "kformula_factory.moc"
