/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

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
#include <QList>
#include <KDebug>
#include <core/kexipartinfo.h>
#include <core/kexipartitem.h>
#include <google/template.h>

#include "DataProvider.h"
#include "HTTPStream.h"

#include "TemplateProvider.h"

namespace KexiWebForms {

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
        // Add tables to left menu
        QString tables;
        QStringList tableNames(gConnection->tableNames());
        qSort(tableNames);
        foreach(QString t, tableNames) {
            tables.append("<li><a href=\"/read/").append(t);
            tables.append("\">").append(t).append("</a></li>");
        }
        afterDict->SetValue("TABLE_LIST", tables.toUtf8().constData());
        
        // Add queries to left menu
        // Step 1: Fill the queryNames stringlist
        // Step 2: Sort it
        // Step 3: Write the links
        QString queries;
        QList<int> queryIds(gConnection->queryIds());
        QMap<QString, QString> queryCaptions;
        for (int i = 0; i < queryIds.size(); ++i) {
            queryCaptions[gConnection->querySchema(queryIds.at(i))->caption()] =
                gConnection->querySchema(queryIds.at(i))->name();
        }
        foreach (QString str, queryCaptions.keys()) {
            queries.append("<li><a href=\"/query/").append(queryCaptions[str]);
            queries.append("\">").append(str).append("</a></li>");
        }
        afterDict->SetValue("QUERY_LIST", queries.toUtf8().constData());
        
        return dict;
    }

    void renderTemplate(google::TemplateDictionary* dict, HTTPStream& stream) {
        std::string output;
        google::Template::GetTemplate(dict->name(), google::DO_NOT_STRIP)->Expand(&output, dict);
        stream << output << webend;
        delete dict;
    }

}
