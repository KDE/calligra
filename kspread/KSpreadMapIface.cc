#include "KSpreadMapIface.h"

#include "kspread_map.h"
#include "kspread_table.h"

KSpreadMapIface::KSpreadMapIface( KSpreadMap* map )
    : DCOPObject( map )
{
    m_map = map;
}

DCOPRef KSpreadMapIface::table( const QString& name )
{
    KSpreadTable* t = m_map->findTable( name );
    if ( !t )
	return DCOPRef( 0 );
    
    return DCOPRef( t->dcopObject() );
}

DCOPRef KSpreadMapIface::table( int index )
{
    KSpreadTable* t = m_map->tableList().at( index );
    if ( !t )
    {
	qDebug("+++++ No table found at index %i", index );
	return DCOPRef( 0 );
    }
    
    qDebug("+++++++ Returning table %s", t->QObject::name() );
    
    return DCOPRef( t->dcopObject() );
}

int KSpreadMapIface::tableCount() const
{
    return m_map->count();
}

QStringList KSpreadMapIface::tableNames() const
{
    QStringList names;

    QList<KSpreadTable>& lst = m_map->tableList();
    QListIterator<KSpreadTable> it( lst );
    for( ; it.current(); ++it )
	names.append( it.current()->name() );

    return names;
}

QValueList<DCOPRef> KSpreadMapIface::tables()
{
    QValueList<DCOPRef> t;

    QList<KSpreadTable>& lst = m_map->tableList();
    QListIterator<KSpreadTable> it( lst );
    for( ; it.current(); ++it )
	t.append( DCOPRef( it.current()->dcopObject() ) );

    return t;
}

