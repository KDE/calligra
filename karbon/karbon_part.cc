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
	// create a layer. we need at least one:
	m_layers.append( new VLayer() );


// <test-object> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	VPath* path = new VPath();
	double x1, y1, x2, y2;

	x1 = 200.0; y1 = 100.0;
	path->moveTo(x1,y1);

	x1 = 300.0; y1 = 100.0;
	x2 = 300.0; y2 = 200.0;
	path->arcTo(x1,y1,x2,y2,100.0);
	x1 = 300.0; y1 = 300.0;
	x2 = 200.0; y2 = 300.0;
	path->arcTo(x1,y1,x2,y2,100.0);
	x1 = 100.0; y1 = 300.0;
	x2 = 100.0; y2 = 200.0;
	path->arcTo(x1,y1,x2,y2,100.0);
	x1 = 100.0; y1 = 100.0;
	x2 = 200.0; y2 = 100.0;
	path->arcTo(x1,y1,x2,y2,100.0);
	path->close();
	m_layers.last()->m_objects.append( path );


	path = new VPath();
	x1 = 200.0; y1 = 100.0;
	path->moveTo(x1,y1);

	x1 = 300.0; y1 = 100.0;
	x2 = 300.0; y2 = 200.0;
	path->arcTo(x1,y1,x2,y2,100.0);
	x1 = 300.0; y1 = 300.0;
	x2 = 200.0; y2 = 300.0;
	path->arcTo(x1,y1,x2,y2,100.0);
	x1 = 100.0; y1 = 300.0;
	x2 = 100.0; y2 = 200.0;
	path->arcTo(x1,y1,x2,y2,100.0);
	x1 = 100.0; y1 = 100.0;
	x2 = 200.0; y2 = 100.0;
	path->arcTo(x1,y1,x2,y2,100.0);

	path->shear( 0.5, 0.0 );
	path->rotate( -45.0 );
	path->scale( 0.5, 0.5 );
	path->translate( 100.0, 100.0 );

	m_layers.last()->m_objects.append( path );

// </test-object> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}

KarbonPart::~KarbonPart()
{
	for ( VLayer* layer=m_layers.first(); layer!=0L; layer=m_layers.next() )
	{
		delete( layer );
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
KarbonPart::paintContent( QPainter& /*p*/, const QRect& /*rect*/,
	bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
	kdDebug() << "part->paintContent()" << endl;
}

#include "karbon_part.moc"
