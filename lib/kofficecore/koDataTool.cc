/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpixmap.h>
#include <qfile.h>
#include <koDataTool.h>
#include <ktrader.h>

#include <kglobal.h>
#include <kstddirs.h>

#include <klibloader.h>
#include <kdebug.h>

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

    if ( !!m_service && !m_service->serviceTypes().contains( "KoDataTool" ) )
    {
        kdDebug(30003) << "The service " << m_service->name().latin1()
                       << " does not feature the service type KoDataTool" << endl;
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

    return m_service->property( "DataType" ).toString();
}

QStringList KoDataToolInfo::mimeTypes() const
{
    if ( !m_service )
        return QStringList();

    return m_service->property( "DataMimeTypes" ).toStringList();
}

bool KoDataToolInfo::isReadOnly() const
{
    if ( !m_service )
        return TRUE;

    return m_service->property( "ReadOnly" ).toBool();
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

    return m_service->property( "Commands" ).toStringList();
}

QStringList KoDataToolInfo::userCommands() const
{
    if ( !m_service )
        return QString::null;

    return m_service->property( "CommandsI18N" ).toStringList();
}

KoDataTool* KoDataToolInfo::createTool( QObject* parent, const char* name )
{
    if ( !m_service )
        return 0;

    KLibFactory* factory = KLibLoader::self()->factory( QFile::encodeName(m_service->library()) );

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
    KTrader *trader = KTrader::self();
    KTrader::OfferList offers = trader->query( "KoDataTool", constr );

    KTrader::OfferList::ConstIterator it = offers.begin();
    for( ; it != offers.end(); ++it )
        lst.append( KoDataToolInfo( *it ) );

    return lst;
}

bool KoDataToolInfo::isValid() const
{
    return( m_service );
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

#include <koDataTool.moc>
