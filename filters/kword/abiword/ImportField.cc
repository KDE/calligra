/* This file is part of the KDE project
   Copyright (C) 2002 Nicolas GOUTTE <goutte@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qstring.h>
#include <qdom.h>

#include <klocale.h>

#include "ImportField.h"

QString getFootnoteFramesetName(const QString& id)
{
    return i18nc("Frameset name","Footnote %1",id);
}

static void InsertTimeVariable(QDomDocument& mainDocument,
    QDomElement& variableElement, QString strKey)
{
    QDomElement typeElement=mainDocument.createElement("TYPE");
    typeElement.setAttribute("key",strKey);
    typeElement.setAttribute("type",2); // Time
    typeElement.setAttribute("text","-"); // Dummy, we let KWord do the work!
    variableElement.appendChild(typeElement); //Append to <VARIABLE>
    QDomElement timeElement=mainDocument.createElement("TIME");
    // We cannot calculate the time, so default to midnight
    timeElement.setAttribute("hour",0);
    timeElement.setAttribute("minute",0);
    timeElement.setAttribute("second",0);
    timeElement.setAttribute("fix",0); // AbiWord's <field> is never fixed
    variableElement.appendChild(timeElement); //Append to <VARIABLE>
}

static void InsertDateVariable(QDomDocument& mainDocument,
    QDomElement& variableElement, QString strKey)
{
    QDomElement typeElement=mainDocument.createElement("TYPE");
    typeElement.setAttribute("key",strKey);
    typeElement.setAttribute("type",0); // date
    typeElement.setAttribute("text","-"); // Just a dummy, KWord will do the work
    variableElement.appendChild(typeElement); //Append to <VARIABLE>
    QDomElement dateElement=mainDocument.createElement("DATE");
    // As we have no idea about the current date, use the *nix epoch 1970-01-01
    dateElement.setAttribute("year",1970);
    dateElement.setAttribute("month",1);
    dateElement.setAttribute("day",1);
    dateElement.setAttribute("fix",0);  // AbiWord's <field> is never fixed
    variableElement.appendChild(dateElement); //Append to <VARIABLE>
}

static bool ProcessTimeField(QDomDocument& mainDocument,
    QDomElement& variableElement, QString strType)
// strType: AbiWord's type
{
    if (strType=="time")
    {
        InsertTimeVariable(mainDocument, variableElement, "TIMELocale");
    }
    else if (strType=="time_miltime")
    {
        // AbiWord's military time is just the standard 24h time (with seconds)
        InsertTimeVariable(mainDocument, variableElement, "TIMEhh:mm:ss");
    }
    else if (strType=="time_ampm")
    {
        InsertTimeVariable(mainDocument, variableElement, "TIMEam");
    }
    else
    {
        // time_zone: not supported due to KWord
        // time_epoch: not supported due to KWord (%Z)
        return false;
    }
    return true;
}

static bool ProcessDateField(QDomDocument& mainDocument,
    QDomElement& variableElement, QString strType)
// strType: AbiWord's type
// Help for the % formats:
//   man date
//  or
//   info date
{
    if (strType=="date")
    {
        InsertDateVariable(mainDocument, variableElement, "DATE0dddd mmmm dd, yyyy");
    }
    else if (strType=="date_mmddyy")
    {
        InsertDateVariable(mainDocument, variableElement, "DATE0mm/dd/yy");
    }
    else if (strType=="date_ddmmyy")
    {
        InsertDateVariable(mainDocument, variableElement, "DATE0dd/mm/yy");
    }
    else if (strType=="date_mdy")
    {
        InsertDateVariable(mainDocument, variableElement, "DATE0mmmm dd, yyyy");
    }
    else if (strType=="date_mthdy")
    {
        InsertDateVariable(mainDocument, variableElement, "DATE0mmm dd, yyyy");
    }
    else if (strType=="date_dfl")
    {   // Should be %c, but we cannot do time zones, so for now: Locale, no time! (TODO)
        InsertDateVariable(mainDocument, variableElement, "DATE0Locale");
    }
    else if (strType=="date_ntdfl")
    {
        InsertDateVariable(mainDocument, variableElement, "DATE0Locale");
    }
    else if (strType=="date_wkday")
    {
        InsertDateVariable(mainDocument, variableElement, "DATE0dddd");
    }
    else
    {
        // date_doy: not supported (%j)
        return false;
    }
    return true;
}

bool ProcessField(QDomDocument& mainDocument,
    QDomElement& variableElement, QString strType, const QXmlAttributes& attributes)
{
    // In AbiWord:
    //   field names are in the file: src/text/fmt/xp/fp_Fields.h
    //   field contents are in the file: src/text/fmt/xp/fp_Run.cpp

    bool done=false;
    if (strType.startsWith("time"))
    {
        done=ProcessTimeField(mainDocument, variableElement, strType);
    }
    else if (strType.startsWith("date"))
    {
        done=ProcessDateField(mainDocument, variableElement, strType);
    }
    else if ((strType=="page_number")||(strType=="page_count"))
    {
        QDomElement typeElement=mainDocument.createElement("TYPE");
        typeElement.setAttribute("key","NUMBER");
        typeElement.setAttribute("type",4); // page number/count
        typeElement.setAttribute("text",1); // We cannot count the pages, so give a default value
        variableElement.appendChild(typeElement); //Append to <VARIABLE>
        QDomElement pgnumElement=mainDocument.createElement("PGNUM");
        pgnumElement.setAttribute("subtype",(strType=="page_count")?1:0);
        pgnumElement.setAttribute("value",1);
        variableElement.appendChild(pgnumElement); //Append to <VARIABLE>
        done=true;
    }
    else if (strType=="file_name")
    {
        QDomElement typeElement=mainDocument.createElement("TYPE");
        typeElement.setAttribute("key","STRING");
        typeElement.setAttribute("type",8);
        typeElement.setAttribute("text","?"); // TODO: do we need this information right now?
        variableElement.appendChild(typeElement); //Append to <VARIABLE>
        QDomElement fieldElement=mainDocument.createElement("FIELD");
        fieldElement.setAttribute("subtype",0);
        fieldElement.setAttribute("value","?"); // Should be the same as the text attribute
        variableElement.appendChild(fieldElement); //Append to <VARIABLE>
        done=true;
    }
    else if (strType=="endnote_ref")
    {
        QDomElement typeElement=mainDocument.createElement("TYPE");
        typeElement.setAttribute("key","STRING");
        typeElement.setAttribute("type",11);
        typeElement.setAttribute("text","?"); // ### TODO: do we need this information right now?
        variableElement.appendChild(typeElement); //Append to <VARIABLE>
        QDomElement element=mainDocument.createElement("FOOTNOTE");
        element.setAttribute("numberingtype","auto"); // ### TODO: support other types
        element.setAttribute("notetype","footnote");
        QString reference(attributes.value("endnote-id").trimmed());
        element.setAttribute("frameset", getFootnoteFramesetName(reference)); // ### TODO: better name
        element.setAttribute("value","?"); // Should be the same as the text attribute
        variableElement.appendChild(element); //Append to <VARIABLE>
        done=true;
    }
    
    // Not supported:
    //  app_ver
    //  app_id
    //  app_options
    //  app_target
    //  app_compiledate
    //  app_compiletime
    //  list_label
    //  word_count
    //  char_count
    //  line_count
    //  para_count
    //  nbsp_count
    //  page_ref
    // ...

    return done;
}
