#include <qpainter.h>
#include <kdebug.h>

#include "karbon_part.h"
#include "karbon_view.h"

KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName,
    QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
}

bool
KarbonPart::initDoc()
{
    // If nothing is loaded, do initialize here
    return true;
}

KoView*
KarbonPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KarbonView( this, parent, name );
}

bool
KarbonPart::loadXML( QIODevice*, const QDomDocument& )
{
    // TODO load the document from the QDomDocument
    return true;
}

QDomDocument
KarbonPart::saveXML()
{
    // TODO save the document into a QDomDocument
    return QDomDocument();
}


void
KarbonPart::paintContent( QPainter& /* painter*/, const QRect& /*rect*/,
    bool /*transparent*/ )
{
    kdDebug(310000) << "**** warning: part.paintContent" << endl;
}

#include "karbon_part.moc"
