#include "plugin.h"

Plugin::Plugin( QObject* parent, const char* name )
    : QObject( parent, name ), m_collection( this )
{
}

Plugin::~Plugin()
{
}

KAction* Plugin::action( const char* name )
{
    return m_collection.action( name );
}

KActionCollection* Plugin::actionCollection()
{
    return &m_collection;
}

#include "plugin.moc"
