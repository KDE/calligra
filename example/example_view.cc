#include "example_view.h"
#include "example_factory.h"
#include "example_part.h"

#include <qpainter.h>
#include <qiconset.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>

ExampleView::ExampleView( ExamplePart* part, QWidget* parent, const char* name )
    : KoView( part, parent, name )
{
    setInstance( ExampleFactory::global() );
    setXMLFile( "example.rc" );
    KStdAction::cut(this, SLOT( cut() ), actionCollection(), "cut" );
    // Note: Prefer KStdAction::* to any custom action if possible.
    //m_cut = new KAction( i18n("&Cut"), "editcut", 0, this, SLOT( cut() ),
    //			 actionCollection(), "cut");
}

void ExampleView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    koDocument()->paintEverything( painter, ev->rect(), FALSE, this );

    painter.end();
}

void ExampleView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
#warning TODO
#endif
}

void ExampleView::cut()
{
    kdDebug(31000) << "ExampleView::cut(): CUT called" << endl;
}

#include "example_view.moc"
