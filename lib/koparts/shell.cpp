#include "shell.h"
#include "part.h"
#include "view.h"
#include "plugin.h"

#include <qdom.h>
#include <qmenubar.h>
#include <qaction.h>
#include <qapplication.h>
#include <qfile.h>
#include <qstatusbar.h>
#include <qobjectlist.h>

Shell::Shell( QWidget* /*parent*/, const char* name )
    : KTMainWindow( name ), m_collection( this )
{
    m_toolbars.setAutoDelete( TRUE );
    m_rootPart = 0;
    m_activeView = 0;
    m_activePart = 0;
    m_policy = TriState;
    m_selectedView = 0;
    m_selectedPart = 0;
    m_statusBar = 0;
    m_bDoPartActivation = TRUE;

    resize( 700, 480 );

    qApp->installEventFilter( this );
}

Shell::~Shell()
{
}

void Shell::initShell()
{
    QString shellconf = configFile();
    QDomDocument shell;
    shell.setContent( shellconf );
    QDomElement s = shell.documentElement();
	
    QDomElement f = s.namedItem("MenuBar").toElement();
    createMenuBar( f, QDomElement() );
}

void Shell::setSelectionPolicy( SelectionPolicy p )
{
    m_policy = p;
}

Shell::SelectionPolicy Shell::selectionPolicy()
{
    return m_policy;
}

KStatusBar *Shell::createStatusBar()
{
  if ( m_statusBar )
    delete m_statusBar;

  m_statusBar = new KStatusBar( this );

  setStatusBar( m_statusBar );

  return m_statusBar;
}

KToolBar *Shell::viewToolBar( const char *name )
{
  return (KToolBar *)child( name, "KToolBar" );
}

KMenuBar *Shell::viewMenuBar( const char *name )
{
  return (KMenuBar *)child( name, "KMenuBar" );
}

Part* Shell::rootPart()
{
    return m_rootPart;
}

View* Shell::rootView()
{
    return (View *)view();
}

void Shell::setRootPart( Part* part )
{
    if ( part == m_rootPart )
	return;

    View* oldview = rootView();
	
    m_rootPart = part;

    if ( part )
    {
	View* view = part->createView( this );
	setView( view );
	view->show();
	
	setActiveView( view );
    }
    else
	setActiveView( 0 );

    if ( oldview )
	delete oldview;
}

View* Shell::activeView()
{
    return m_activeView;
}

Part* Shell::activePart()
{
    return m_activePart;
}

View* Shell::selectedView()
{
    return m_selectedView;
}

Part* Shell::selectedPart()
{
    return m_selectedPart;
}

void Shell::setSelectedView( View* view, Part* part )
{
    ASSERT( view || !part );
    ASSERT( m_policy == TriState );

    if ( m_selectedView == view && m_selectedPart == part )
	return;

    Part* old_part = m_selectedPart;
    View* old_view = m_selectedView;

    if ( view && !part )
	part = view->part();
    m_selectedView = view;
    m_selectedPart = part;

    if ( old_view )
    {
	ViewSelectEvent event( old_part, FALSE );
	qApp->sendEvent( old_view, &event );
    }

    if ( m_selectedView )
    {
	ViewSelectEvent event( m_selectedPart, TRUE );
	qApp->sendEvent( m_selectedView, &event );
    }
}

void Shell::setActiveView( View* view, Part* part )
{
    if ( m_activeView == view && m_activePart == part )
	return;

    Part* old_part = m_selectedPart;
    View* old_view = m_selectedView;

    m_selectedView = 0;
    m_selectedPart = 0;

    if ( old_view && old_part &&
	 ( old_view != view || old_part != part ) )
    {
	ViewSelectEvent event( old_part, FALSE );
	qApp->sendEvent( old_view, &event );
    }

    old_part = m_activePart;
    old_view = m_activeView;

    if ( view && !part )
	part = view->part();
    m_activeView = view;
    m_activePart = part;

    if ( old_view )
    {
	ViewActivateEvent event( old_part, FALSE );
	qApp->sendEvent( old_view, &event );
        m_toolbars.clear();
    }

    const QObjectList *children = menuBar()->children();
    if ( children )
    {
      QObjectListIt it( *children );
      for (; it.current(); ++it )
        if ( it.current()->inherits( "QAction" ) )
	  ((QAction *)it.current())->unplug( menuBar() );
    }

    menuBar()->clear();

    if ( m_statusBar )
    {
      delete m_statusBar;
      m_statusBar = 0;
      setStatusBar( 0 );
    }

    if ( m_activeView )
    {
	QString shellconf = configFile();
	QDomDocument shell;
	shell.setContent( shellconf );
	QDomElement s = shell.documentElement();

	QString config = view->part()->config();
	QDomDocument document;
	document.setContent( config );

	QDomElement d = document.documentElement();
	QDomElement e;
	if ( m_activeView->part() == m_activePart )
	    e = d.namedItem("MenuBar").toElement();
	else
	    e = d.namedItem("InPlaceEditing").namedItem("MenuBar").toElement();	
	QDomElement f = s.namedItem("MenuBar").toElement();
	createMenuBar( f, e );

	if ( m_activeView->part() == m_activePart )
	    e = d.namedItem("ToolBars").toElement();
	else
	    e = d.namedItem("InPlaceEditing").namedItem("ToolBars").toElement();
	createToolBars( e );

	ViewActivateEvent event( m_activePart, TRUE );
	qApp->sendEvent( m_activeView, &event );
    }
    else
    {
	QString shellconf = configFile();
	QDomDocument shell;
	shell.setContent( shellconf );
	QDomElement s = shell.documentElement();

	QDomElement f = s.namedItem("MenuBar").toElement();
	createMenuBar( f, QDomElement() );
    }
}

KToolBar* Shell::createToolBar( const char* name )
{
    bool honor_mode = (QString(name) == "mainToolBar") ? true : false;
    KToolBar* bar = new KToolBar( this, name, -1, honor_mode );
    m_toolbars.append( bar );

    addToolBar( bar );

    return bar;
}

void Shell::createToolBars( const QDomElement& element )
{
    setUpdatesEnabled( FALSE );
    QDomElement e = element.firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	if ( e.tagName() == "ToolBar" )
        {
	    KToolBar* bar = createToolBar( e.attribute("name") );
	    if ( !e.attribute( "fullWidth" ).isEmpty() &&
		 e.attribute( "fullWidth" ).lower() == "true" )
		bar->setFullWidth( TRUE );
	    else
		bar->setFullWidth( FALSE );
	    if ( !e.attribute( "position" ).isEmpty() ) {
		if ( e.attribute( "position" ).lower() == "top" )
		    bar->setBarPos( KToolBar::Top );
		else if ( e.attribute( "position" ).lower() == "left" )
		    bar->setBarPos( KToolBar::Left );
		else if ( e.attribute( "position" ).lower() == "right" )
		    bar->setBarPos( KToolBar::Right );
		else if ( e.attribute( "position" ).lower() == "bottom" )
		    bar->setBarPos( KToolBar::Bottom );
		else if ( e.attribute( "position" ).lower() == "floating" )
		    bar->setBarPos( KToolBar::Floating );
		else if ( e.attribute( "position" ).lower() == "flat" )
		    bar->setBarPos( KToolBar::Flat );
	    }
		
	    QDomElement f = e.firstChild().toElement();
	    for( ; !f.isNull(); f = f.nextSibling().toElement() )
	    {
		if ( f.tagName() == "Action" )
	        {
		    QAction* a = action( f.attribute("name") );
		    if ( a )
		      a->plug( bar );
		}
		else if ( f.tagName() == "PluginAction" )
                {
		    Plugin* plugin = m_activeView->plugin( f.attribute("plugin") );
		    if ( plugin )
		    {
			QAction* a = plugin->action( f.attribute("name") );
			if ( a )
			    a->plug( bar );
			else
			    qDebug("Shell: Unknown plugin action %s", f.attribute("name").latin1() );
		    }
		    else
			qDebug("Shell: Unknown plugin %s", f.attribute("plugin").latin1() );
		}
		else if ( f.tagName() == "Separator" )
	        {
		    bar->insertSeparator();
		}

	    }

	    bar->show();
	}
    }
    setUpdatesEnabled( TRUE );
}

void Shell::createMenuBar( const QDomElement& shell, const QDomElement& part )
{
    setUpdatesEnabled( FALSE );
    QStringList names;

    QDomElement e = shell.firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	names.append( e.attribute("name") );
    }

    e = shell.firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	if  ( e.tagName() == "Menu" )
        {
	    QDomElement p;
	    QDomElement f = part.firstChild().toElement();
	    for( ; !f.isNull(); f = f.nextSibling().toElement() )
		if ( f.attribute("name") == e.attribute("name") )
		    p = f;

	    QPopupMenu* menu = createMenu( e, p );
	    QDomElement n = e.namedItem("text").toElement();
	    QString name = n.text();
	
	    menuBar()->insertItem( name, menu );
	}
	else if ( e.tagName() == "Part" )
        {
	    QDomElement f = part.firstChild().toElement();
	    for( ; !f.isNull(); f = f.nextSibling().toElement() )
		if ( !names.contains( f.attribute("name") ) )
	        {
		  if ( f.tagName() == "Menu" )
		  {
		    QPopupMenu* menu = createMenu( QDomElement(), f );
		    QDomElement n = f.namedItem("text").toElement();
		    QString name = n.text();
	
		    menuBar()->insertItem( name, menu );
		  }
		  else if ( f.tagName() == "Action" )
		  {
		    QAction *a = action( f.attribute( "name" ) );
		    if ( a )
		    {
		      //Simon: now we're getting *rude*...
		
		      QMenuBar *qbar = (QMenuBar *)menuBar()->child( 0L, "QMenuBar" );
		
		      if ( qbar )
		       a->plug( qbar );
		    }
		  }
		}
    	}
    }
    setUpdatesEnabled( TRUE );
}

QPopupMenu* Shell::createMenu( const QDomElement& shell, const QDomElement& part )
{
    bool shellmenu = TRUE;
    QDomElement m = shell;
    if ( m.isNull() )
    {
	m = part;
	shellmenu = FALSE;
    }

    bool first = TRUE;
    QPopupMenu* menu = new QPopupMenu( this );

    QDomElement e = m.firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	if ( e.tagName() == "Menu" )
        {
	    first = FALSE;
	    QPopupMenu* menu2;
	    if ( shellmenu )
		menu2 = createMenu( e, QDomElement() );
	    else
		menu2 = createMenu( QDomElement(), e );
	    QDomElement n = e.namedItem("text").toElement();
	    QString name = n.text();
	    menu->insertItem( name, menu2 );
	}
	else if ( e.tagName() == "Action" )
        {
	    first = FALSE;
	    QAction* a = action( e.attribute("name"), shellmenu );
	    if ( a )
		a->plug( menu );
	    else
		qDebug("Shell: Unknown action %s", e.attribute("name").latin1() );
	}
	else if ( e.tagName() == "PluginAction" )
        {
	    first = FALSE;
	    Plugin* plugin = m_activeView->plugin( e.attribute("plugin") );
	    if ( plugin )
	    {
		QAction* a = plugin->action( e.attribute("name") );
		if ( a )
		    a->plug( menu );
		else
		    qDebug("Shell: Unknown plugin action %s", e.attribute("name").latin1() );
	    }
	    else
		qDebug("Shell: Unknown plugin %s", e.attribute("plugin").latin1() );
	}
	else if ( e.tagName() == "Separator" )
        {
	    first = FALSE;
	    menu->insertSeparator();
	}
	else if ( e.tagName() == "Part" )
        {	
	    bool empty = TRUE;
	
	    QDomElement f = part.firstChild().toElement();
	    for( ; !f.isNull(); f = f.nextSibling().toElement() )
	    {
		if ( f.tagName() == "Menu" )
	        {
		    if ( !first && empty )
			menu->insertSeparator();
		    empty = FALSE;
		
		    QPopupMenu* menu2;
		    if ( shellmenu )
			menu2 = createMenu( f, QDomElement() );
		    else
			menu2 = createMenu( QDomElement(), f );
		    QDomElement n = f.namedItem("text").toElement();
		    QString name = n.text();
		    menu->insertItem( name, menu2 );
		}
		else if ( f.tagName() == "Action" )
	        {
		    if ( !first && empty )
			menu->insertSeparator();
		    empty = FALSE;
				
		    QAction* a = action( f.attribute("name") );
		    if ( a )
			a->plug( menu );
		    else
			qDebug("Shell: Unknown action %s", f.attribute("name").latin1() );
		}
		else if ( e.tagName() == "PluginAction" )
	        {
		    if ( !first && empty )
			menu->insertSeparator();
		    empty = FALSE;

		    Plugin* plugin = m_activeView->plugin( e.attribute("plugin") );
		    if ( plugin )
	            {
			QAction* a = plugin->action( e.attribute("name") );
			if ( a )
			    a->plug( menu );
			else
			    qDebug("Shell: Unknown plugin action %s", e.attribute("name").latin1() );
		    }
		    else
			qDebug("Shell: Unknown plugin %s", e.attribute("plugin").latin1() );
		}
		else if ( f.tagName() == "Separator" )
	        {
		    empty = FALSE;
		    menu->insertSeparator();
		}
	    }
	
	    if ( !e.nextSibling().isNull() && !empty )
		menu->insertSeparator();
	}
    }

    return menu;
}

class PublicWidget : public QWidget
{
public:
    WFlags hack() const { return QWidget::getWFlags(); }
};

bool Shell::eventFilter( QObject* obj, QEvent* ev )
{
    if ( m_policy == Direct )
    {
	if ( ev->type() == QEvent::FocusIn )
        {
	    qDebug("Focus Event");
	    QObject* o = obj;
	    do
            {
		if ( o->inherits("View") )
	        {
		    View* view = (View*)o;
		    if ( view->topLevelWidget() != this )
			return FALSE;
		    setActiveView( view );
		    return FALSE;
		}
		o = o->parent();
	    } while( o );
	}

	return FALSE;
    }

    if ( m_policy == TriState && ( ev->type() == QEvent::MouseButtonPress ||
				   ( ev->type() == QEvent::MouseButtonDblClick && m_bDoPartActivation ) ) )
    {
	if ( obj->inherits("Frame") )
	    return FALSE;
	
	QMouseEvent* e = (QMouseEvent*)ev;
	if ( e->button() != LeftButton )
	    return FALSE;
	
	// qDebug("Click event ev=%x obj=%x", (int)ev, (int)obj );
	// Iterate over parents to find an instance of class "View"
	QObject* o = obj;
	do
        {
	    // No popup menus and stuff.
	    if ( o->isWidgetType() && ( ((PublicWidget*)o)->hack() & WStyle_Tool ) )
		return FALSE;
	
	    if ( o->inherits("View") )
	    {
		View* _view = (View*)o;
		if ( _view->topLevelWidget() != this )
		    return FALSE;
		// Turn mouse coordinates into coordinates of the document
		QPoint canvaspos( _view->canvas()->mapFromGlobal( e->globalPos() ) );
		canvaspos.rx() -= _view->canvasXOffset();
		canvaspos.ry() -= _view->canvasYOffset();
		Part* part = _view->hitTest( canvaspos );
		if ( !part )
		    return FALSE;

		// Double clicks always activate
		if ( ev->type() == QEvent::MouseButtonDblClick )
	        {
		    // Hmmmm, the view/part is already active, so dont
		    // eat the double click event.
		    if ( _view == m_activeView && part == m_activePart )
			return FALSE;
		
		    setActiveView( _view, part );
		    return TRUE;
		}
		
		// If the view handles selection on its own ....
		if ( _view->doubleClickActivation() )
	        {
		    // Click on the root view/part ?
		    if ( rootPart() == part && (View *)view() == _view )
			setActiveView( _view, part );

		    return FALSE;
		}
		
		//qDebug("Processing mouse click");
		/* if ( view )
		    qDebug("Click view=%x  part=%s", (int)view, part->name() );
		else
    		    qDebug("Click view=%x  part=%x", (int)view, (int)part ); */

		// The view/part is neither selected nor active ?
		if ( ( m_selectedView != _view || m_selectedPart != part ) &&
		     ( m_activeView != _view || m_activePart != part ) )
	        {
		    // Click on the root view/part ?
		    if ( rootPart() == part && (View *)view() == _view )
			// The root view/part is never selected -> activate it
			setActiveView( _view, part );
		    else
			setSelectedView( _view, part );
		
		    // Eat the event
		    return TRUE;
		}
		// Is the view/part selected but not activated ?
		else if ( m_selectedView == _view && m_selectedPart == part )
	        {
		    setActiveView( _view, part );
		
		    // Eat the event
		    return TRUE;
		}
		// Is the view/part already active ?
		else if ( m_activeView == _view && m_activePart == part )
	        {
		    // Clear the selection
		    setSelectedView( 0 );
		
		    // Dont eat the event
		    return FALSE;
		}

		// Dont eat the event
		return FALSE;
	    }
	    o = o->parent();
	} while( o );
    }

    return FALSE;
}

QString Shell::readConfigFile( const QString& filename ) const
{
    QFile file( filename );
    if ( !file.open( IO_ReadOnly ) )
	return QString::null;

    uint size = file.size();
    char* buffer = new char[ size + 1 ];
    file.readBlock( buffer, size );
    buffer[ size ] = 0;
    file.close();

    QString text = QString::fromUtf8( buffer, size );
    delete[] buffer;

    return text;
}

QActionCollection* Shell::actionCollection()
{
    return &m_collection;
}

QAction* Shell::action( const char* name, bool shell )
{
    if ( shell )
	return m_collection.action( name );

    if ( m_activeView->part() == m_activePart )
    {
	QAction* a = m_activeView->action( name );
	if ( a )
	    return a;
    }

    return m_activePart->action( name );
}

#include "shell.moc"
