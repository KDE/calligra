/* This file is part of the KDE project
 * Copyright (C) 2002 Simon MacMullen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <config.h>
#include "kfile_koffice.h"

#include <klocale.h>
#include <kgenericfactory.h>
#include <KoStore.h>
#include <KoStoreDevice.h>

#include <qdom.h>
#include <QFile>
#include <QDateTime>

typedef KGenericFactory<KOfficePlugin> KOfficeFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_koffice, KOfficeFactory( "kfile_koffice" ))

KOfficePlugin::KOfficePlugin(QObject *parent,
                       const QStringList &args)

    : KFilePlugin(parent, args)
{
    makeMimeTypeInfo( "application/x-kword" );
    makeMimeTypeInfo( "application/x-kpresenter" );
    makeMimeTypeInfo( "application/x-kspread" );
    makeMimeTypeInfo( "application/x-karbon" );
    makeMimeTypeInfo( "application/x-kontour" );
    makeMimeTypeInfo( "application/x-kchart" );
    makeMimeTypeInfo( "application/x-kivio" );
    makeMimeTypeInfo( "application/x-krita" );
    makeMimeTypeInfo( "application/x-kformula" );

    /*makeMimeTypeInfo( "application/vnd.kde.kword" );
    makeMimeTypeInfo( "application/vnd.kde.kpresenter" );
    makeMimeTypeInfo( "application/vnd.kde.kspread" );
    makeMimeTypeInfo( "application/vnd.kde.karbon" );
    makeMimeTypeInfo( "application/vnd.kde.kontour" );*/
}

void KOfficePlugin::makeMimeTypeInfo(const QString& mimeType)
{
    KFileMimeTypeInfo* info = addMimeTypeInfo( mimeType );

    KFileMimeTypeInfo::GroupInfo* group = 0L;

    group = addGroupInfo(info, "DocumentInfo", i18n("Document Information"));

    KFileMimeTypeInfo::ItemInfo* item;

    item = addItemInfo(group, "Author", i18n("Author"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Author);
    item = addItemInfo(group, "Title", i18n("Title"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Name);
    item = addItemInfo(group, "Abstract", i18n("Abstract"), QVariant::String);
    setHint(item, KFileMimeTypeInfo::Description);
}

bool KOfficePlugin::readInfo( KFileMetaInfo& info, uint what)
{
    if ( info.path().isEmpty() ) // remote file
        return false;

    KFileMetaInfoGroup group = appendGroup(info, "DocumentInfo");

    KoStore* store = KoStore::createStore(info.path(), KoStore::Read);
    if ( store && store->open( QString("documentinfo.xml") ) )
    {
        KoStoreDevice dev( store );
        QDomDocument doc;
        doc.setContent( &dev );

        QDomNode authorNode = doc.namedItem("document-info").namedItem("author");
        QDomNode aboutNode = doc.namedItem("document-info").namedItem("about");

        QString author = stringFromNode(authorNode, "full-name");
        QString title = stringFromNode(aboutNode, "title");
        QString abstract = stringFromNode(aboutNode, "abstract");

        appendItem(group, "Author", author);
        appendItem(group, "Title", title);
        appendItem(group, "Abstract", abstract);

        store->close();
        delete store;
        return true;
    }
    delete store;
    return false;
}

QString KOfficePlugin::stringFromNode(const QDomNode &node, const QString &name)
{
    QString value = node.namedItem(name).toElement().text();
    return value.isEmpty() ? i18n("*Unknown*") : value;
}


#include "kfile_koffice.moc"
