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
#include "ExportFilterDirect.h"

QString ClassExportFilterHtmlTransitional::getDocType(void) const
{
    // We are TRANSITIONAL, as we want to use tags like <FONT>, <U> and explicit colours.
    return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">";
}

QString ClassExportFilterXHtmlTransitional::getDocType(void) const
{
    // We are TRANSITIONAL, as we want to use tags like <FONT>, <U> and explicit colours.
    // Note "html" is lower-case in XHTML, while "DOCTYPE" and "!PUBLIC" are upper-case!
    return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"DTD/xhtml1-transitional.dtd\">";
}

QString ClassExportFilterHtmlTransitional::getBodyOpeningTagExtraAttributes(void) const
{
    // Define the background colour as white!
    return " bgcolor=\"#FFFFFF\""; // Leading space is important!
}

void ClassExportFilterHtmlTransitional::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
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
            {   // Format is not issued from KWord.
                //  So it is only the text (no additional formating.)
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
            // TODO: replace multiple spaces into non-breaking spaces!
            // Opening elements

            QString fontName = (*paraFormatDataIt).fontName;

            // Will a potential <font> tag have attributes?
            QString fontAttributes;

            if ( !fontName.isEmpty() )
            {
                fontAttributes+=" face=\"";
                fontAttributes+=fontName; // TODO: add alternative font names
                fontAttributes+="\"";
            }
            // Give the font size relatively (be kind with people with impered vision)
            // TODO: option to give absolute font sizes
            int size=(*paraFormatDataIt).fontSize;
            // 12pt is considered the normal size // TODO: relative to layout!
            if (size>0)
            {
                size /= 4;
                size -= 3;
                //if (size<-4) size=-4; // Cannot be triggered
                if (size>4) size=4;
                if (size)
                {
                    fontAttributes+=" size=\""; // in XML numbers must be quoted!
                    if (size>0)
                    {
                        fontAttributes+="+";
                    }
                    fontAttributes+=QString::number(size,10);
                    fontAttributes+="\"";
                }
            }
            if ( (*paraFormatDataIt).colour.isValid() )
            {
                // Give colour
                fontAttributes+=" color=\"";
                // QColor::name() does all the job :)
                fontAttributes+=(*paraFormatDataIt).colour.name();
                fontAttributes+="\"";
            }

            if (!fontAttributes.isEmpty())
            {
                // We have font attributes, so we must have a <font> element
                outputText+="<font";
                outputText+=fontAttributes;
                outputText+=">";
            }

            if ( (*paraFormatDataIt).weight >= 75 )
            {
                outputText+="<b>";
            }
            if ( (*paraFormatDataIt).italic )
            {
                outputText+="<i>";
            }
            if ( (*paraFormatDataIt).underline )
            {
                outputText+="<u>";
            }
            if ( (*paraFormatDataIt).strikeout )
            {
                outputText+="<s>";
            }
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
            if ( (*paraFormatDataIt).strikeout )
            {
                outputText+="</s>";
            }
            if ( (*paraFormatDataIt).underline )
            {
                outputText+="</u>";
            }
            if ( (*paraFormatDataIt).italic )
            {
                outputText+="</i>";
            }
            if ( (*paraFormatDataIt).weight >= 75 )
            {
                outputText+="</b>";
            }

            if (!fontAttributes.isEmpty())
            {
                outputText+="</font>";
            }
        }
    }
}

QString ClassExportFilterHtmlTransitional::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    QString strResult;
    switch (typeList)
    {
    case CounterData::STYLE_NONE: // HTML cannot express STYLE_NONE
    case CounterData::STYLE_CUSTOMBULLET: // We cannot keep the custom type/style
    default:
        {
            orderedList=false;
            strResult="<ul>\n";
            break;
        }
    case CounterData::STYLE_CIRCLEBULLET:
        {
            orderedList=false;
            strResult="<ul type=\"circle\">\n";
            break;
        }
    case CounterData::STYLE_SQUAREBULLET:
        {
            orderedList=false;
            strResult="<ul type=\"square\">\n";
            break;
        }
    case CounterData::STYLE_DISCBULLET:
        {
            orderedList=false;
            strResult="<ul type=\"disc\">\n";
            break;
        }
    case CounterData::STYLE_NUM:
        {
            orderedList=true;
            strResult="<ol type=\"1\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_L:
        {
            orderedList=true;
            strResult="<ol type=\"a\">\n";
            break;
        }
    case CounterData::STYLE_ALPHAB_U:
        {
            orderedList=true;
            strResult="<ol type=\"A\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_L:
        {
            orderedList=true;
            strResult="<ol type=\"i\">\n";
            break;
        }
    case CounterData::STYLE_ROM_NUM_U:
        {
            orderedList=true;
            strResult="<ol type=\"I\">\n";
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

QString ClassExportFilterHtmlTransitional::getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout)
{
    QString strAlign;

    if (strTag!="li")
    {
        // We do not set "left" explicitly, since KWord cannot do bi-di
        if (( layout.alignment== "right") || (layout.alignment=="center") || (layout.alignment=="justify"))
        {
            strAlign=QString(" align=\"%1\"").arg(layout.alignment);
        }
    }

    QString strElement;
    strElement+='<';
    strElement+=strTag;
    strElement+=strAlign;
    strElement+='>';

   // Opening elements

    QString fontName = layout.formatData.fontName;

    // Will a potential <font> tag have attributes?
    QString fontAttributes;

    if ( !fontName.isEmpty() )
    {
        fontAttributes+=" face=\"";
        fontAttributes+=fontName; // TODO: add alternative font names
        fontAttributes+="\"";
    }
#if 0
    // FIXME: using "size=+-" make things ugly. So for now, it is deactivated

    // Give the font size relatively (be kind with people with impered vision)
    // TODO: option to give absolute font sizes
    int size=layout.formatData.fontSize;
    // 12pt is considered the normal size // TODO: relative to layout!
    if (size>0)
    {
        size /= 4;
        size -= 3;
        //if (size<-4) size=-4; // Cannot be triggered
        if (size>4) size=4;
        if (size)
        {
            fontAttributes+=" size=\""; // in XML numbers must be quoted!
            if (size>0)
            {
                fontAttributes+="+";
            }
            fontAttributes+=QString::number(size,10);
            fontAttributes+="\"";
        }
    }
#endif
    if ( layout.formatData.colour.isValid() )
    {
        // Give colour
        fontAttributes+=" color=\"";
        // QColor::name() does all the job :)
        fontAttributes+=layout.formatData.colour.name();
        fontAttributes+="\"";
    }

    if (!fontAttributes.isEmpty())
    {
        // We have font attributes, so we must have a <font> element
        strElement+="<font";
        strElement+=fontAttributes;
        strElement+=">";
    }

    // Allow bold only if tag is not a heading!
    const bool bold=((layout.formatData.weight >= 75) & (strTag[0]!='h'));

    if ( bold )
    {
        strElement+="<b>";
    }
    if ( layout.formatData.italic )
    {
        strElement+="<i>";
    }
    if ( layout.formatData.underline )
    {
        strElement+="<u>";
    }
    if ( layout.formatData.strikeout )
    {
        strElement+="<s>";
    }
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
    if ( layout.formatData.strikeout )
    {
        strElement+="</s>";
    }
    if ( layout.formatData.underline )
    {
        strElement+="</u>";
    }
    if ( layout.formatData.italic )
    {
        strElement+="</i>";
    }
    if ( bold )
    {
        strElement+="</b>";
    }

    if (!fontAttributes.isEmpty())
    {
        strElement+="</font>";
    }

    strElement+="</";
    strElement+=strTag;
    strElement+=">\n";
    return strElement;
}
