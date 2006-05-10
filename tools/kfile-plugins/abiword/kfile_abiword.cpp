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
#include "kfile_abiword.h"

#include <klocale.h>
#include <kgenericfactory.h>
#include <kfilterdev.h>

#include <qdom.h>
#include <QFile>
#include <qdatetime.h>
#include <kdebug.h>

typedef KGenericFactory<AbiwordPlugin> AbiwordFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_abiword, AbiwordFactory( "kfile_abiword" ))

AbiwordPlugin::AbiwordPlugin(QObject *parent,
                       const QStringList &args)

    : KFilePlugin(parent, args)
{
    init();
}

void AbiwordPlugin::init()
{
    KFileMimeTypeInfo* info = addMimeTypeInfo( "application/x-abiword" );

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

bool AbiwordPlugin::readInfo( KFileMetaInfo& info, uint what)
{
    if ( info.path().isEmpty() ) // remote file
        return false;

    //Find the last extension
    QString strExt;
    const int result=info.path().findRev('.');
    if (result>=0)
    {
        strExt=info.path().mid(result);
    }
    QString strMime; // Mime type of the compressor (default: unknown)
    if ((strExt==".gz")||(strExt==".GZ")        //in case of .abw.gz (logical extension)
        ||(strExt==".zabw")||(strExt==".ZABW")) //in case of .zabw (extension used prioritary with AbiWord)
    {
        // Compressed with gzip
        strMime="application/x-gzip";
        kDebug() << "Compression: gzip" << endl;
    }
    else if ((strExt==".bz2")||(strExt==".BZ2") //in case of .abw.bz2 (logical extension)
        ||(strExt==".bzabw")||(strExt==".BZABW")) //in case of .bzabw (extension used prioritary with AbiWord)
    {
        // Compressed with bzip2
        strMime="application/x-bzip2";
        kDebug() << "Compression: bzip2" << endl;
    }

    KFileMetaInfoGroup group = appendGroup(info, "DocumentInfo");
    QIODevice* in = KFilterDev::deviceForFile(info.path(),strMime);
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
    QDomNode summary  = docElem.namedItem("metadata");
    QDomNode m_item = summary.namedItem("m");

    QString author;
    QString title;
    QString abstract;

    while( !m_item.isNull() )
    {
        kDebug()<<" m_item.toElement().text: "<<m_item.toElement().text()<<endl;
        QString key = m_item.toElement().attribute( "key" );
        if ( key.isEmpty() )
            continue;
        else if ( key=="dc.creator" )
            author=m_item.toElement().text();
        else if ( key=="dc.description" )
            abstract=m_item.toElement().text();
        else if ( key=="dc.title" )
            title=m_item.toElement().text();
        else
            kDebug()<<" Other key :"<<key<<endl;
        m_item = m_item.nextSibling();
    }
    appendItem(group, "Author", stringItem( author ));
    appendItem(group, "Title", stringItem( title ));
    appendItem(group, "Abstract", stringItem( abstract ));

    delete in;
    return true;
}

QString AbiwordPlugin::stringItem( const QString &name )
{
    return name.isEmpty() ? i18n("*Unknown*") : name;
}


#include "kfile_abiword.moc"
