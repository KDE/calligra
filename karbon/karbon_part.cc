#include <qpainter.h>
#include <kdebug.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "vpoint.h"

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
KarbonPart::paintContent( QPainter& painter, const QRect& /*rect*/,
    bool /*transparent*/ )
{
//    painter.scale(VPoint::s_fractInvScale,VPoint::s_fractInvScale);
    painter.scale(2.0,2.0);
    painter.setPen( Qt::black );
//    painter.drawLine(0,0,100*VPoint::s_fractScale,100*VPoint::s_fractScale);
    painter.drawLine(100,100,200,200);

    // paint all objects:
    VObject* obj;
    for ( obj=m_objects.first(); obj!=0L; obj=m_objects.next() ) 
    {
	obj->draw( painter );
    }
}

#include "karbon_part.moc"
