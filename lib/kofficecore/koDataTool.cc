#include "koDataTool.h"
#include "koTrader.h"

#include <kglobal.h>
#include <kstddirs.h>

#include <klibloader.h>

/*************************************************
 *
 * KoDataToolInfo
 *
 *************************************************/

KoDataToolInfo::KoDataToolInfo()
{
    m_service = 0;
}

KoDataToolInfo::KoDataToolInfo( const KService::Ptr& service )
{
    m_service = service;

    if ( m_service && !m_service->serviceTypes().contains( "KoDataTool" ) )
    {
	qDebug("The service %s does not feature the service type KoDataTool", m_service->name().latin1() );
	m_service = 0;
    }
}

KoDataToolInfo::KoDataToolInfo( const KoDataToolInfo& info )
{
    m_service = info.service();
}

KoDataToolInfo& KoDataToolInfo::operator= ( const KoDataToolInfo& info )
{
    m_service = info.service();

    return *this;
}

QString KoDataToolInfo::dataType() const
{
    if ( !m_service )
	return QString::null;

    return m_service->property( "DataType" )->stringValue();
}

QStringList KoDataToolInfo::mimeTypes() const
{
    if ( !m_service )
	return QStringList();

    return m_service->property( "DataMimeTypes" )->stringListValue();
}

bool KoDataToolInfo::isReadOnly() const
{
    if ( !m_service )
	return TRUE;

    return m_service->property( "ReadOnly" )->boolValue();
}

QPixmap KoDataToolInfo::icon() const
{
    if ( !m_service )
	return QPixmap();

    QPixmap pix;
    QStringList lst = KGlobal::dirs()->resourceDirs("icon");
    QStringList::ConstIterator it = lst.begin();
    while (!pix.load( *it + "/" + m_service->icon() ) && it != lst.end() )
	it++;

    return pix;
}

QPixmap KoDataToolInfo::miniIcon() const
{
    if ( !m_service )
	return QPixmap();

    QPixmap pix;
    QStringList lst = KGlobal::dirs()->resourceDirs("mini");
    QStringList::ConstIterator it = lst.begin();
    while (!pix.load( *it + "/" + m_service->icon() ) && it != lst.end() )
	it++;

    return pix;
}

QStringList KoDataToolInfo::commands() const
{
    if ( !m_service )
	return QString::null;

    return m_service->property( "Commands" )->stringListValue();
}

QStringList KoDataToolInfo::userCommands() const
{
    if ( !m_service )
	return QString::null;

    return m_service->property( "CommandsI18N" )->stringListValue();
}

KoDataTool* KoDataToolInfo::createTool( QObject* parent, const char* name )
{
    if ( !m_service )
	return 0;

    KLibFactory* factory = KLibLoader::self()->factory( m_service->library() );

    if( !factory )
	return 0;

    QObject* obj = factory->create( parent, name );
    if ( !obj || !obj->inherits( "KoDataTool" ) )
    {
	delete obj;
	return 0;
    }

    return (KoDataTool*)obj;
}

KService::Ptr KoDataToolInfo::service() const
{
    return m_service;
}

QValueList<KoDataToolInfo> KoDataToolInfo::query( const QString& datatype, const QString& mimetype )
{
    QValueList<KoDataToolInfo> lst;

    QString constr;

    if ( !datatype.isEmpty() )
    {
	QString tmp( "DataType == '%1'" );
	tmp = tmp.arg( datatype );
	constr = tmp;
    }
    if ( !mimetype.isEmpty() )
    {
	QString tmp( "'%1' in DataMimeTypes" );
	tmp = tmp.arg( mimetype );
	if ( constr.isEmpty() )
	    constr = tmp;
	else
	    constr = constr + " and " + tmp;
    }

    // Query the trader
    KoTrader *trader = KoTrader::self();
    KoTrader::OfferList offers = trader->query( "KoDataTool", constr );

    KoTrader::OfferList::ConstIterator it = offers.begin();
    for( ; it != offers.end(); ++it )
	lst.append( KoDataToolInfo( *it ) );
	
    return lst;
}

bool KoDataToolInfo::isValid() const
{
    return( m_service != (KService*)0 );
}
			
/*************************************************
 *
 * KoDataTool
 *
 *************************************************/

KoDataTool::KoDataTool( QObject* parent, const char* name )
    : QObject( parent, name )
{
}

#include "koDataTool.moc"
