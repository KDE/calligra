#include <qpainter.h>
#include <kdebug.h>

#include "vpoint.h"
#include "vpath.h"

#include "karbon_part.h"
#include "karbon_view.h"

KarbonPart::KarbonPart( QWidget* parentWidget, const char* widgetName,
    QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
// <test-object> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    VPath* path = new VPath();
    double	x1(0.33333333), y1(0.0),
		x2(100.0), y2(200.0),
		x3(200.0), y3(200.0);
    path->moveTo(x1,y1);
    path->lineTo(x2,y2);
    path->lineTo(x3,y3);
    
    x1 = 250.0; y1 = 50.0;
    x2 = 0.0; y2 = 50.0;
    x3 = 300.0; y3 = 200.0;
    path->curveTo(x1,y1,x2,y2,x3,y3);
    m_objects.append( path );
// </test-object> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}

KarbonPart::~KarbonPart()
{
    for ( VObject* object=m_objects.first(); object!=0L; object=m_objects.next() ) 
    {
	delete( object );
    }    
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
    painter.scale(VPoint::s_fractInvScale,VPoint::s_fractInvScale);

    // paint all objects:
    VObject* obj;
    for ( obj=m_objects.first(); obj!=0L; obj=m_objects.next() ) 
    {
	obj->draw( painter );
    }
}

#include "karbon_part.moc"
