// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#include <qstring.h>

#include "ExportFilterBase.h"
#include "ExportFilterSpartan.h"


QString ClassExportFilterHtmlSpartan::getDocType(void) const
{
    // We are STRICT
    return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">";
}

QString ClassExportFilterXHtmlSpartan::getDocType(void) const
{
    // We are STRICT
    return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"DTD/xhtml1-strict.dtd\">";
}

QString ClassExportFilterHtmlSpartan::getBodyOpeningTagExtraAttributes(void) const
{
    return QString::null;
}

void ClassExportFilterHtmlSpartan::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
{
    if (! paraText.isEmpty() )
    {

        CreateMissingFormatData(paraText,paraFormatDataList);

        ValueListFormatData::Iterator  paraFormatDataIt;  //Warning: cannot use "->" with it!!

        QString partialText;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            //Retrieve text
            partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );

            if ((*paraFormatDataIt).missing)
            {   //Format is not issued from KWord. Therefore is only the layout
                // So it is only the text
                if (outputText==" ")
                {//Just a space as text. Therefore we must use a non-breaking space.
                    outputText += "&nbsp;";
                }
                else
                {
                    //Code all possible predefined HTML entities
                    outputText += escapeText(partialText);
                }
                continue; // And back to the loop
            }

            // TODO: first and last characters of partialText should not be a space (white space problems!)
            // TODO: replace multiples spaces in non-breaking spaces!
            // Opening elements
            // <sub> and <sup> are not really considered being formatting by HTML, so we keep them here!
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sub>"; //Subscript
            }
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="<sup>"; //Superscript
            }

            // The text
            if (outputText==" ")
            {//Just a space as text. Therefore we must use a non-breaking space.
                outputText += "&nbsp;";
            }
            else
            {
                //Code all possible predefined HTML entities
                outputText += escapeText(partialText);
            }

            // Closing elements
            if ( 2==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sup>"; //Superscript
            }
            if ( 1==(*paraFormatDataIt).verticalAlignment )
            {
                outputText+="</sub>"; //Subscript
            }
        }
    }
}

QString ClassExportFilterHtmlSpartan::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    QString strResult;
    switch (typeList)
    {
    case CounterData::STYLE_NONE:
    default:
        {
            orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_NUM:
    case CounterData::STYLE_ALPHAB_L:
    case CounterData::STYLE_ALPHAB_U:
    case CounterData::STYLE_ROM_NUM_L:
    case CounterData::STYLE_ROM_NUM_U:
    case CounterData::STYLE_CUSTOM:
        {
            orderedList=true;
            strResult="<ol>\n";
            break;
        }
    }
    return strResult;
}

QString ClassExportFilterHtmlSpartan::getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData&)
{
    QString strElement;
    strElement+='<';
    strElement+=strTag;
    strElement+='>';
    strElement+=strParagraphText;
    strElement+="</";
    strElement+=strTag;
    strElement+=">\n";
    return strElement;
}
