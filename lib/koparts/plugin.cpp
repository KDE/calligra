#include "plugin.h"

Plugin::Plugin( QObject* parent, const char* name )
    : QObject( parent, name ), m_collection( this )
{
}

Plugin::~Plugin()
{
}

QAction* Plugin::action( const char* name )
{
    return m_collection.action( name );
}

QActionCollection* Plugin::actionCollection()
{
    return &m_collection;
}

#include "plugin.moc"
