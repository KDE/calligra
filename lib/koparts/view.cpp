#include "view.h"
#include "part.h"
#include "frame.h"

#include <klibloader.h>

ViewChild::ViewChild( PartChild* child, Frame* frame )
{
    m_child = child;
    m_frame = frame;

    if ( frame )
	connect( frame, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
    else if ( child )
	connect( child, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
}

ViewChild::~ViewChild()
{
    if ( m_frame )
	delete m_frame;
}

PartChild* ViewChild::partChild()
{
    return m_child;
}

Frame* ViewChild::frame()
{
    return m_frame;
}

void ViewChild::slotDestroyed()
{
    if ( sender() == m_frame )
	m_frame = 0;
    else if ( sender() == m_child )
	m_child = 0;
}

// -----------------------------------------------------

View::View( Part* part, QWidget* parent, const char* name )
    : QWidget( parent, name ), m_collection( this )
{
    m_part = part;
    m_scaleX = 1.0;
    m_scaleY = 1.0;

    setFocusPolicy( StrongFocus );
}

View::~View()
{
}

Part* View::part()
{
    return m_part;
}

Shell* View::shell()
{
    QObject* o = parent();
    while( o )
	if ( o->inherits("Shell") )
	    return (Shell*)o;
	else
	    o = o->parent();

    return 0;
}

QAction* View::action( const char* name )
{
    return m_collection.action( name );
}

QActionCollection* View::actionCollection()
{
    return &m_collection;
}

int View::leftBorder() const
{
    return 0;
}

int View::rightBorder() const
{
    return 0;
}

int View::topBorder() const
{
    return 0;
}

int View::bottomBorder() const
{
    return 0;
}

void View::setScaling( double x, double y )
{
    m_scaleX = x;
    m_scaleY = y;
    update();
}

double View::xScaling() const
{
    return m_scaleX;
}

double View::yScaling() const
{
    return m_scaleY;
}

Part* View::hitTest( const QPoint& )
{
    return part();
}

void View::customEvent( QCustomEvent* ev )
{
    if ( ev->type() == QEvent::User + 291173 )
	viewActivateEvent( (ViewActivateEvent*)ev );
    else if ( ev->type() == QEvent::User + 291174 )
	viewSelectEvent( (ViewSelectEvent*)ev );
}

void View::viewSelectEvent( ViewSelectEvent* ev )
{
    if ( ev->part() == part() )
	emit selected( ev->selected() );
}

void View::viewActivateEvent( ViewActivateEvent* ev )
{
    if ( ev->part() == part() )
	emit activated( ev->activated() );
}

QWidget* View::canvas()
{
    return this;
}

Plugin* View::plugin( const char* libname )
{
    QObject* ch = child( libname, "Plugin" );
    if ( ch )
	return (Plugin*)ch;

    KLibLoader* loader = KLibLoader::self();
    if ( !loader )
    {
	qDebug("View: No library loader installed");
	return 0;
    }
    
    KLibFactory* f = loader->factory( libname );
    if ( !f )
    {
	qDebug("View: Could not initialize library");
	return 0;
    }
    QObject* obj = f->create( this, libname, "Plugin" );
    if ( !obj->inherits("Plugin" ) )
    {
	qDebug("The library does not feature an object of class Plugin");
	delete obj;
	return 0;
    }

    return (Plugin*)obj;
}

bool View::doubleClickActivation() const
{
    return FALSE;
}

int View::canvasXOffset() const
{
    return 0;
}

int View::canvasYOffset() const
{
    return 0;
}

// --------------------------------------------

ViewActivateEvent::ViewActivateEvent( Part* part, bool active )
    : QEvent( (QEvent::Type) ( QEvent::User + 291173 ) )
{
    m_active = active;
    m_part = part;
}

bool ViewActivateEvent::activated() const
{
    return m_active;
}

bool ViewActivateEvent::deactivated() const
{
    return !m_active;
}

Part* ViewActivateEvent::part()
{
    return m_part;
}

// --------------------------------------------

ViewSelectEvent::ViewSelectEvent( Part* part, bool sel )
    : QEvent( (QEvent::Type) ( QEvent::User + 291174 ) )
{
    m_selected = sel;
    m_part = part;
}

bool ViewSelectEvent::selected() const
{
    return m_selected;
}

Part* ViewSelectEvent::part()
{
    return m_part;
}

#include "view.moc"
