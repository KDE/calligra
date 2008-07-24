/* This file is part of the KDE project
   Copyright (C) 2008 Lorenzo Villani <lvillani@binaryhelix.net>
   Copyright (C) 2008 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <string>

#include <QtAlgorithms>
#include <QTextDocument>
#include <QList>
#include <KDebug>

#include <core/kexipartinfo.h>
#include <core/kexipartitem.h>
#include <google/template.h>

#include <pion/net/HTTPResponseWriter.hpp>

#include "model/Database.h"
#include "DataProvider.h"

#include "TemplateProvider.h"

using namespace pion::net;

namespace KexiWebForms {

    static bool caseInsensitiveLessThan(const QString &s1, const QString &s2) {
        return s1.toLower() < s2.toLower();
    }

    // Adds object list to the menu.
    // Step 1: Fill the queryNames stringlist
    // Step 2: Sort it
    // Step 3: Write the links
    static void addList(google::TemplateDictionary* dict, KexiDB::ObjectTypes objectType, const char* uri, const char* keyName)
    {
        KexiWebForms::Model::Database db;
        QHash<QString, QString> oNames(db.getNames(objectType));
        QStringList captions(oNames.uniqueKeys());
        qSort(captions.begin(), captions.end(), caseInsensitiveLessThan);
        
        QString HTML;
        foreach (const QString& caption, captions) {
            QStringList names(oNames.values(caption));
            foreach (const QString& name, names) {
                HTML.append(QString::fromLatin1("\t<li><a href=\"/%1/%2\">%3</a></li>\n").arg(uri).arg(name).arg(caption));
            }
        }
        
        dict->SetValue(keyName, HTML.toUtf8().constData());
        
        
        /*QList<int> objectIds(gConnection->objectIds( objectType ));
        QMap<QString, QString> objectNamesForCaptions;
        foreach (const int id, objectIds) {
            KexiDB::SchemaData schema;
            tristate res = gConnection->loadObjectSchemaData( id, schema );
            if (res != true)
                continue;
            objectNamesForCaptions.insertMulti( 
                schema.captionOrName(), schema.name() ); //insertMulti() because there can be many objects with the same caption
        }
        QStringList objectCaptionsSorted( objectNamesForCaptions.uniqueKeys() );
        kDebug() << objectCaptionsSorted;
        qSort(objectCaptionsSorted.begin(), objectCaptionsSorted.end(), caseInsensitiveLessThan);
        kDebug() << objectCaptionsSorted;
        const QString itemString( QString::fromLatin1("<li><a href=\"/") + objectTypeName + QString::fromLatin1("/%1\">%2</a></li>\n") );
        QString result;
        foreach (const QString& caption, objectCaptionsSorted) {
            QStringList names( objectNamesForCaptions.values( caption ) );
            qSort(names); // extra sort :)
            kDebug() << names;
            foreach (const QString& name, names) {
                kDebug() << name << caption;
                result.append( itemString.arg(name).arg(Qt::escape(caption)) );
            }
        }
        dict->SetValue(keyName, result.toUtf8().constData());*/
    }

    google::TemplateDictionary* initTemplate(const char* filename) {
        google::TemplateDictionary* dict = new google::TemplateDictionary(filename);
        dict->SetFilename(filename);
        // Add header template
        google::TemplateDictionary* beforeDict = dict->AddIncludeDictionary("beforecontent");
        beforeDict->SetFilename("beforecontent.tpl");
        beforeDict->SetValue("TITLE", gProjectData->infoString(false).toUtf8().constData());

        // Add footer template (-- note, this includes the left menu with the standard template)
        google::TemplateDictionary* afterDict = dict->AddIncludeDictionary("aftercontent");
        afterDict->SetFilename("aftercontent.tpl");

        // Add objects to the left menu
        //KexiWebForms::Model::Database db;
        /*QHash<QString, QString> tableNames(db.getNames(KexiDB::TableObjectType));
        QHash<QString, QString> queryNames(db.getNames(KexiDB::QueryObjectType));
        //addList(tableNames);
        //addList(queryNames);
        QStringList tableCaptions(tableNames.uniqueKeys());
        QStringList queryCaptions(queryNames.uniqueKeys());
        qSort(tableCaptions.begin(), tableCaptions.end(), caseInsensitiveLessThan);
        qSort(queryCaptions.begin(), queryCaptions.end(), caseInsensitiveLessThan);

        QString tablesHTML;
        foreach (const QString& caption, tableCaptions) {
            QStringList names(tableNames.values(caption));
            foreach (const QString& name, names) {
                tablesHTML.append(QString::fromLatin1("\t<li><a href=\"/read/%1\">%2</a></li>\n").arg(name).arg(caption));
            }
        }

        QString queriesHTML;
        foreach (const QString& caption, queryCaptions) {
            QStringList names(queryNames.values(caption));
            foreach (const QString& name, names) {
                queriesHTML.append(QString::fromLatin1("<li><a href=\"/query/%1\">%2</a></li>").arg(name).arg(caption));
            }
            }
        
        kDebug() << "TABLES HTML: " << tablesHTML << endl;
        afterDict->SetValue("TABLE_LIST", tablesHTML.toLatin1().constData());
        afterDict->SetValue("QUERY_LIST", queriesHTML.toLatin1().constData());*/
        
        //qSort(
        //foreach (
        addList(afterDict, KexiDB::TableObjectType, "read", "TABLE_LIST");
        addList(afterDict, KexiDB::QueryObjectType, "query", "QUERY_LIST");
        return dict;
    }

    void renderTemplate(google::TemplateDictionary* dict, pion::net::HTTPResponseWriterPtr writer) {
        std::string output;
        google::Template::GetTemplate(dict->name(), google::DO_NOT_STRIP)->Expand(&output, dict);
        writer->writeNoCopy(output);
        writer->writeNoCopy(HTTPTypes::STRING_CRLF);
        writer->writeNoCopy(HTTPTypes::STRING_CRLF);
        writer->send();
    }

}
