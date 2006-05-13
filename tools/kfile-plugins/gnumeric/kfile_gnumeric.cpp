/* This file is part of the KDE project
 * Copyright (C) 2005 Laurent Montel <montel@kde.org>
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
#include "kfile_gnumeric.h"

#include <klocale.h>
#include <kgenericfactory.h>
#include <kfilterdev.h>

#include <qdom.h>
#include <QFile>
#include <QDateTime>
#include <kdebug.h>

typedef KGenericFactory<GnumericPlugin> GnumericFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_gnumeric, GnumericFactory( "kfile_gnumeric" ))

GnumericPlugin::GnumericPlugin(QObject *parent,
                       const QStringList &args)

    : KFilePlugin(parent, args)
{
    init();
}

void GnumericPlugin::init()
{
    KFileMimeTypeInfo* info = addMimeTypeInfo( "application/x-gnumeric" );

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

bool GnumericPlugin::readInfo( KFileMetaInfo& info, uint what)
{
    if ( info.path().isEmpty() ) // remote file
        return false;

    KFileMetaInfoGroup group = appendGroup(info, "DocumentInfo");
    QIODevice* in = KFilterDev::deviceForFile(info.path(),"application/x-gzip");
    if ( !in )
    {
        kError() << "Cannot create device for uncompressing! Aborting!" << endl;
        return false;
    }

    if (!in->open(QIODevice::ReadOnly))
    {
        kError() << "Cannot open file for uncompressing! Aborting!" << endl;
        delete in;
        return false;
    }
    QDomDocument doc;
    doc.setContent( in );
    in->close();
    QDomElement docElem = doc.documentElement();
    QDomNode summary  = docElem.namedItem("gmr:Summary");
    QDomNode gmr_item = summary.namedItem("gmr:Item");
    QString author;
    QString title;
    QString abstract;

    while( !gmr_item.isNull() )
    {
        QDomNode gmr_name  = gmr_item.namedItem("gmr:name");
        QDomNode gmr_value = gmr_item.namedItem("gmr:val-string");
        if (gmr_name.toElement().text() == "title")
        {
            title=gmr_value.toElement().text();
        }
        else if (gmr_name.toElement().text() == "author")
        {
            author=gmr_value.toElement().text();
        }
        else if (gmr_name.toElement().text() == "comments")
        {
            abstract=gmr_value.toElement().text();
        }
        gmr_item = gmr_item.nextSibling();
    }
    appendItem(group, "Author", stringItem( author ));
    appendItem(group, "Title", stringItem( title ));
    appendItem(group, "Abstract", stringItem( abstract ));

    delete in;
    return true;
}

QString GnumericPlugin::stringItem( const QString &name )
{
    return name.isEmpty() ? i18n("*Unknown*") : name;
}


#include "kfile_gnumeric.moc"
