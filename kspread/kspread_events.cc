#include "kspread_events.h"

Event::Event()
    : QEvent( QEvent::User )
{
}

Event::~Event()
{
}

// ----------------------------------------------------

KSpreadSelectionChanged::KSpreadSelectionChanged( const QRect& rect, const QString& table )
    : Event()
{
    m_rect = rect;
    m_table = table;
}

KSpreadSelectionChanged::~KSpreadSelectionChanged()
{
}

bool KSpreadSelectionChanged::test( QEvent* e )
{
    if ( e->type() != QEvent::User )
	return FALSE;
    
    return ( strcmp( ((Event*)e)->eventName(), "KSpread/View/SelectionChanged" ) == 0 );
}
