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

#include "ExportTagProcessing.h"
#include "ExportFilterBase.h"
#include "ExportFilterFullPower.h"

QString ClassExportFilterHtmlFullPower::escapeCssIdentifier(const QString& strText) const
{
    // Reference: section 4.1.3 of the CSS2 recommendation
    // NOTE: when we need to escape, we choose the numerical CSS escape as it is encoding neutral.

    QString strReturn;

    for (uint i=0; i<strText.length(); i++)
    {
        const QChar ch=strText[i];
        if (((ch>='a') && (ch<='z'))
            || ((ch>='A') && (ch<='Z')))
        {
            strReturn+=ch;
        }
        else if (((ch>='0') && (ch<='9'))
            || (ch=='-'))
        {
            if (i=0)
            {
                // A digit or a hyphen is not allowed as first character of an identifier
                //  therefore we must escape it
                strReturn+='\\'; // start escape
                strReturn+=QString::number(ch.unicode(),16);
                strReturn+=' '; // end escape (the space is not part of the following text!)
            }
            else
            {
                strReturn+=ch;
            }
        }
        else if (ch>=QChar(161))
        {
            // Any Unicode character greater or egual to 161 is allowed too, even at start.
            // FIXME: what if the encoding does not support this character? (Would need to be escaped too!)
            strReturn+=ch;
        }
        else
        {
            // We have a non-acceptable character, so escape it!
            strReturn+='\\'; // start escape
            strReturn+=QString::number(ch.unicode(),16);
            strReturn+=' '; // end escape (the space is not part of the following text!)
        }
    }
    return strReturn;
}

void ClassExportFilterHtmlFullPower::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
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
            outputText+="<span style=\"";

            // Font name
            QString fontName = (*paraFormatDataIt).fontName;
            if ( !fontName.isEmpty() )
            {
                outputText+="font-family: ";
                outputText+=fontName; // TODO: add alternative font names
                outputText+="; ";
            }

            // Font style
            outputText+="font-style: ";
            if ( (*paraFormatDataIt).italic )
            {
                outputText+="italic";
            }
            else
            {
                outputText+="normal";
            }
            outputText+="; ";

            outputText+="font-weight: ";
            if ( (*paraFormatDataIt).weight >= 75 )
            {
                outputText+="bold";
            }
            else
            {
                outputText+="normal";
            }
            outputText+="; ";

            const int size=(*paraFormatDataIt).fontSize;
            if (size>0)
            {
                // We use absolute font sizes.
                outputText+="font-size: ";
                outputText+=QString::number(size,10);
                outputText+="pt; ";
            }

            if ( (*paraFormatDataIt).colour.isValid() )
            {
                // Give colour
                outputText+="color: ";
                // QColor::name() does all the job :)
                outputText+=(*paraFormatDataIt).colour.name();
                outputText+="; ";
            }

            outputText+="text-decoration: ";
            if ( (*paraFormatDataIt).underline )
            {
                outputText+="underline";
            }
            else if ( (*paraFormatDataIt).strikeout )
            {
                outputText+="line-through";
            }
            else
            {
                outputText+="none";
            }
            //outputText+="; ";
            outputText+="\">"; // close span opening tag
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
            outputText+="</span>";
        }
    }
}

QString ClassExportFilterHtmlFullPower::getBodyOpeningTagExtraAttributes(void) const
{
    return QString::null;
}

QString ClassExportFilterHtmlFullPower::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    QString strResult;
    switch (typeList)
    {
    case CounterData::STYLE_CUSTOMBULLET: // We cannot keep the custom type/style
    default:
        {
            orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_NONE:
        {
            orderedList=false;
            strResult="<ul style=\"list-style-type:none\">\n";
            break;
        }
    case CounterData::STYLE_CIRCLEBULLET:
        {
            orderedList=false;
            strResult="<ul style=\"list-style-type:circle\">\n";
            break;
        }
    case CounterData::STYLE_SQUAREBULLET:
        {
            orderedList=false;
            strResult="<ul style=\"list-style-type:square\">\n";
            break;
        }
    case CounterData::STYLE_DISCBULLET:
        {
            orderedList=false;
            strResult="<ul style=\"list-style-type:disc\">\n";
            break;
        }
    case CounterData::STYLE_NUM:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:decimal\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_L:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:lower-alpha\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_U:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:upper-alpha\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_L:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:lower-roman\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_U:
        {
            orderedList=true;
            strResult="<ol style=\"list-style-type:upper-roman\">\n";
            break;
        }
    case CounterData::STYLE_CUSTOM:
        {
            // We cannot keep the custom type/style
            orderedList=true;
            strResult="<ol>\n";
            break;
        }
    }
    return strResult;
}

QString ClassExportFilterHtmlFullPower::getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout)
{
    QString strElement;
    strElement+='<';
    strElement+=strTag;

    // Opening elements
    strElement+=" style=\"";

    // We do not set "left" explicitly, since KWord cannot do bi-di
    if (( layout.alignment== "right") || (layout.alignment=="center") || (layout.alignment=="justify"))
    {
        strElement+=QString("text-align:%1;").arg(layout.alignment);
    }

    if ( layout.indentLeft!=0.0 )
    {
        strElement+=QString("margin-left:%1pt;").arg(layout.indentLeft);
    }

    if ( layout.indentRight!=0.0 )
    {
        strElement+=QString("margin-right:%1pt;").arg(layout.indentRight);
    }

    if ( layout.indentFirst!=0.0 )
    {
        strElement+=QString("text-indent:%1pt;").arg(layout.indentFirst);
    }

    // Font name
    QString fontName = layout.formatData.fontName;
    if ( !fontName.isEmpty() )
    {
        strElement+="font-family: ";
        strElement+=fontName; // TODO: add alternative font names
        strElement+="; ";
    }

    // Font style
    strElement+="font-style: ";
    if ( layout.formatData.italic )
    {
        strElement+="italic";
    }
    else
    {
        strElement+="normal";
    }
    strElement+="; ";

    strElement+="font-weight: ";
    if ( layout.formatData.weight >= 75 )
    {
        strElement+="bold";
    }
    else
    {
        strElement+="normal";
    }
    strElement+="; ";

    const int size=layout.formatData.fontSize;
    if (size>0)
    {
        // We use absolute font sizes.
        strElement+="font-size: ";
        strElement+=QString::number(size,10);
        strElement+="pt; ";
    }

    if ( layout.formatData.colour.isValid() )
    {
        // Give colour
        strElement+="color: ";
        // QColor::name() does all the job :)
        strElement+=layout.formatData.colour.name();
        strElement+="; ";
    }

    strElement+="text-decoration: ";
    if ( layout.formatData.underline )
    {
        strElement+="underline";
    }
    else if ( layout.formatData.strikeout )
    {
        strElement+="line-through";
    }
    else
    {
        strElement+="none";
    }
    //strElement+="; ";
    strElement+="\">"; // close opening tag

    if ( 1==layout.formatData.verticalAlignment )
    {
        strElement+="<sub>"; //Subscript
    }
    if ( 2==layout.formatData.verticalAlignment )
    {
        strElement+="<sup>"; //Superscript
    }

    // The text
    strElement+=strParagraphText;
    // Closing elements

    if ( 2==layout.formatData.verticalAlignment )
    {
        strElement+="</sup>"; //Superscript
    }
    if ( 1==layout.formatData.verticalAlignment )
    {
        strElement+="</sub>"; //Subscript
    }

    strElement+="</";
    strElement+=strTag;
    strElement+=">\n";
    return strElement;
}

QString ClassExportFilterHtmlFullPower::processDocTagStylesOnly(QDomElement myNode)
{
    QString strReturn;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "editor",        "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "mime",          "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "syntaxVersion", "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    strReturn+="<style type=\"text/css\">\n";
    if (!isXML())
    {
        // Put the style under comments to increase the compatibility with old browsers
        // However in XHTML 1.0, you cannot put the style definition into HTML comments
        strReturn+="<!--\n";
    }
    strReturn+="BODY { background-color: #FFFFFF }\n";

    // We are only interested in <STYLES> for now!
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "STYLES",      NULL,                NULL ) );
    ProcessSubtags (myNode, tagProcessingList, strReturn, this);

    if (!isXML())
    {
        strReturn+="-->\n";
    }
    strReturn+="</style>\n";

    return strReturn;
}

