#include "KSpreadMapIface.h"

#include "kspread_map.h"
#include "kspread_table.h"
#include "kspread_doc.h"

#include <kapp.h>
#include <dcopclient.h>
#include <kdebug.h>

KSpreadMapIface::KSpreadMapIface( KSpreadMap* map )
    : DCOPObject( map )
{
    m_map = map;
}

DCOPRef KSpreadMapIface::table( const QString& name )
{
    KSpreadTable* t = m_map->findTable( name );
    if ( !t )
        return DCOPRef();

    return DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
}

DCOPRef KSpreadMapIface::tableByIndex( int index )
{
    KSpreadTable* t = m_map->tableList().at( index );
    if ( !t )
    {
        kdDebug(36001) << "+++++ No table found at index " << index << endl;
        return DCOPRef();
    }

    kdDebug(36001) << "+++++++ Returning table " << t->QObject::name() << endl;

    return DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
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
        t.append( DCOPRef( kapp->dcopClient()->appId(), it.current()->dcopObject()->objId() ) );

    return t;
}

DCOPRef KSpreadMapIface::insertTable( const QString& name )
{
    if ( m_map->findTable( name ) )
        return table( name );

    KSpreadTable* t = new KSpreadTable( m_map, name );
    t->setTableName( name );
    m_map->doc()->addTable( t );

    return table( name );
}

bool KSpreadMapIface::processDynamic(const QCString &fun, const QByteArray &/*data*/,
                                     QCString& replyType, QByteArray &replyData)
{
    // Does the name follow the pattern "foobar()" ?
    uint len = fun.length();
    if ( len < 3 )
        return FALSE;

    if ( fun[ len - 1 ] != ')' || fun[ len - 2 ] != '(' )
        return FALSE;

    KSpreadTable* t = m_map->findTable( fun.left( len - 2 ).data() );
    if ( !t )
        return FALSE;

    replyType = "DCOPRef";
    QDataStream out( replyData, IO_WriteOnly );
    out << DCOPRef( kapp->dcopClient()->appId(), t->dcopObject()->objId() );
    return TRUE;
}
