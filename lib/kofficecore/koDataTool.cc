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
#include <koDocument.h>
#include <kinstance.h>
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

QString KoDataToolInfo::iconName() const
{
    if ( !m_service )
        return QString::null;
    return m_service->icon();
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

    return QStringList::split( ',', m_service->comment() );
}

KoDataTool* KoDataToolInfo::createTool( QObject* parent, const char* name )
{
    createTool( 0L, parent, name );
}

KoDataTool* KoDataToolInfo::createTool( KoDocument* part, QObject* parent, const char* name ) const
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
    KoDataTool * tool = static_cast<KoDataTool *>(obj);
    tool->setPart( part );
    return tool;
}

KService::Ptr KoDataToolInfo::service() const
{
    return m_service;
}

QValueList<KoDataToolInfo> KoDataToolInfo::query( const QString& datatype, const QString& mimetype )
{
    return query( datatype, mimetype, 0L );
}

QValueList<KoDataToolInfo> KoDataToolInfo::query( const QString& datatype, const QString& mimetype, KoDocument * part )
{
    QValueList<KoDataToolInfo> lst;

    QString constr;

    if ( !datatype.isEmpty() )
    {
        constr = QString::fromLatin1( "DataType == '%1'" ).arg( datatype );
    }
    if ( !mimetype.isEmpty() )
    {
        QString tmp = QString::fromLatin1( "'%1' in DataMimeTypes" ).arg( mimetype );
        if ( constr.isEmpty() )
            constr = tmp;
        else
            constr = constr + " and " + tmp;
    }
/* Bug in KTrader ? Test with HEAD-kdelibs!
    if ( part )
    {
        QString tmp = QString::fromLatin1( "not ('%1' in ExcludeFrom)" ).arg( part->instance()->instanceName() );
        if ( constr.isEmpty() )
            constr = tmp;
        else
            constr = constr + " and " + tmp;
    } */

    // Query the trader
    //kdDebug() << "KoDataToolInfo::query " << constr << endl;
    KTrader::OfferList offers = KTrader::self()->query( "KoDataTool", constr );

    KTrader::OfferList::ConstIterator it = offers.begin();
    for( ; it != offers.end(); ++it )
    {
        // Temporary replacement for the non-working trader query above
        if ( !part || !(*it)->property("ExcludeFrom").toStringList()
             .contains( part->instance()->instanceName() ) )
            lst.append( KoDataToolInfo( *it ) );
        else
            kdDebug() << (*it)->entryPath() << " excluded." << endl;
    }

    return lst;
}

bool KoDataToolInfo::isValid() const
{
    return( m_service );
}

/*************************************************
 *
 * KoDataToolAction
 *
 *************************************************/
KoDataToolAction::KoDataToolAction( const QString & text, const KoDataToolInfo & info, const QString & command,
                                    QObject * parent, const char * name )
    : KAction( text, info.iconName(), 0, parent, name ),
      m_command( command ),
      m_info( info )
{
}

void KoDataToolAction::slotActivated()
{
    emit toolActivated( m_info, m_command );
}

QList<KAction> KoDataToolAction::dataToolActionList( const QValueList<KoDataToolInfo> & tools, const QObject *receiver, const char* slot )
{
    QList<KAction> actionList;
    if ( tools.isEmpty() )
        return actionList;

    actionList.append( new KActionSeparator() );
    QValueList<KoDataToolInfo>::ConstIterator entry = tools.begin();
    for( ; entry != tools.end(); ++entry )
    {
        QStringList userCommands = (*entry).userCommands();
        QStringList commands = (*entry).commands();
        ASSERT(!commands.isEmpty());
        if ( commands.count() != userCommands.count() )
            kdWarning() << "KoDataTool desktop file error (" << (*entry).service()->entryPath()
                        << "). " << commands.count() << " commands and "
                        << userCommands.count() << " descriptions." << endl;
        QStringList::ConstIterator uit = userCommands.begin();
        QStringList::ConstIterator cit = commands.begin();
        for (; uit != userCommands.end() && cit != commands.end(); ++uit, ++cit )
        {
            //kdDebug() << "creating action " << *uit << " " << *cit << endl;
            KoDataToolAction * action = new KoDataToolAction( *uit, *entry, *cit );
            connect( action, SIGNAL( toolActivated( const KoDataToolInfo &, const QString & ) ),
                     receiver, slot );
            actionList.append( action );
        }
    }

    return actionList;
}

/*************************************************
 *
 * KoDataTool
 *
 *************************************************/

KoDataTool::KoDataTool( QObject* parent, const char* name )
    : QObject( parent, name ), m_part( 0L )
{
}

KInstance* KoDataTool::instance() const
{
   return m_part ? m_part->instance() : 0L;
}

#include <koDataTool.moc>
