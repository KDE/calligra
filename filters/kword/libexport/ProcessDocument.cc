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

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include <qdom.h>
#include <qvaluelist.h>

#include <kdebug.h>

#include "KWEFStructures.h"
#include "TagProcessing.h"
#include "KWEFBaseClass.h"
#include "ProcessDocument.h"

// Every tag has its own processing function. All of those functions
// have the same parameters since the functions are passed to
// ProcessSubtags throuch the TagProcessing class.  The top level
// function is ProcessDocTag and can be called with the node returned
// by QDomDocument::documentElement (). The tagData argument can be
// used to either pass variables down to the subtags or to allow
// subtags to return values. As a bare minimum the tag processing
// functions must handle the tag's attributes and the tag's subtags
// (which it can choose to ignore). Currently implemented is
// processing for the following tags and attributes:
//
// TODO: make this list again up-to-date
//
// DOC
//   FRAMESETS
//     FRAMESET
//       PARAGRAPH
//          TEXT - Text Element
//          FORMATS
//            FORMAT id=1 pos= len=
//          LAYOUT
//            NAME value=


static void ProcessLayoutNameTag ( QDomNode myNode, void *tagData, QString &, KWEFBaseClass* )
{
    LayoutData *layout = (LayoutData *) tagData;

    layout->styleName = "";
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "value", "QString", (void *) &layout->styleName ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ( layout->styleName == "" )
    {
        layout->styleName = "Standard";
        kdError(30503) << "Bad layout name value!" << endl;
    }

    AllowNoSubtags (myNode);
}

static void ProcessLayoutFlowTag ( QDomNode myNode, void *tagData, QString &, KWEFBaseClass* )
{
    LayoutData *layout = (LayoutData *) tagData;

    layout->alignment = "";
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "align", "QString", (void *) &layout->alignment ) );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode);
}

static void ProcessLayoutTabulatorTag ( QDomNode myNode, void *tagData, QString &, KWEFBaseClass*)
{
    // WARNING: This is exactly the format AbiWord needs for defining its tabulators
    // TODO: make this function independant of the AbiWord export filter

    LayoutData *layout = (LayoutData *) tagData;

    double ptPos;
    int type;

    QValueList<AttrProcessing> attrProcessingList;

    attrProcessingList.append ( AttrProcessing ( "ptpos", "double",(void *)&ptPos ) );
    attrProcessingList.append ( AttrProcessing ( "type", "int", (void *) &type ) );
    ProcessAttributes (myNode, attrProcessingList);
    if(layout->tabulator.isEmpty())
        layout->tabulator=QString::number(ptPos);
    else
        layout->tabulator+=","+QString::number(ptPos);
    layout->tabulator+="pt";

    switch(type)
    {
    case 0:
        layout->tabulator+="/L0";
        break;
    case 1:
        layout->tabulator+="/C0";
        break;
    case 2:
        layout->tabulator+="/R0";
        break;
    case 3:
        layout->tabulator+="/D0";
        break;
    default:
        layout->tabulator+="/L0";
        break;
    }
    AllowNoSubtags (myNode);
}

static void ProcessLayoutOffsetTag( QDomNode myNode, void *tagData, QString &, KWEFBaseClass*)
{
    LayoutData *layout = (LayoutData *) tagData;
    QValueList<AttrProcessing> attrProcessingList;

    attrProcessingList.append ( AttrProcessing ("after" , "double", (void *)&layout->marginBottom ) );
    attrProcessingList.append ( AttrProcessing ("before" , "double", (void *)&layout->marginTop ) );

    ProcessAttributes (myNode, attrProcessingList);
    AllowNoSubtags (myNode);
}

static void ProcessLayoutLineSpacingTag( QDomNode myNode, void *tagData, QString &, KWEFBaseClass*)
{
    LayoutData *layout = (LayoutData *) tagData;
    QValueList<AttrProcessing> attrProcessingList;
    QString line;
    attrProcessingList.append ( AttrProcessing ("value" , "QString", (void *)&line ) );
    ProcessAttributes (myNode, attrProcessingList);

    if (line=="oneandhalf")
        layout->lineSpacingType=15;
    else if (line=="double")
        layout->lineSpacingType=20;
    else
    {
        const double size=line.toDouble();
        if (size>=1.0)
        {
            // We have a valid size
            layout->lineSpacingType=0; // set to custom
            layout->lineSpacing=size;
        }
        else
        {
            layout->lineSpacingType=10; // set to 1 line
        }
    }
    AllowNoSubtags (myNode);
}

static void ProcessLineBreakingTag ( QDomNode myNode, void *tagData, QString &, KWEFBaseClass*)
{
    LayoutData *layout = (LayoutData *) tagData;

    QString strBefore, strAfter;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "linesTogether", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "hardFrameBreak", "QString", (void *) &strBefore) );
    attrProcessingList.append ( AttrProcessing ( "hardFrameBreakAfter", "QString", (void *) &strAfter) );
    ProcessAttributes (myNode, attrProcessingList);

    layout->pageBreakBefore=(strBefore=="true");
    layout->pageBreakAfter =(strAfter =="true");

    AllowNoSubtags (myNode);
}

static void ProcessCounterTag ( QDomNode myNode, void *tagData, QString &, KWEFBaseClass* )
{
    CounterData *counter = (CounterData *) tagData;
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "numberingtype", "int", (void *) &counter->numbering ) );
    attrProcessingList.append ( AttrProcessing ( "type", "int", (void *) &counter->style ) );
    attrProcessingList.append ( AttrProcessing ( "depth", "int", (void *) &counter->depth ) );
    attrProcessingList.append ( AttrProcessing ( "start", "int", (void *) &counter->start ) );
    attrProcessingList.append ( AttrProcessing ( "lefttext", "QString", (void *) &counter->lefttext ) );
    attrProcessingList.append ( AttrProcessing ( "righttext", "QString", (void *) &counter->righttext ) );
    attrProcessingList.append ( AttrProcessing ( "bullet", "int", (void *) &counter->customCharacter ) );
    attrProcessingList.append ( AttrProcessing ( "bulletfont", "QString", (void *) &counter->customFont ) );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode);
}


// FORMAT's subtags

static void ProcessItalicTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int value=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->italic=(value!=0);
}

static void ProcessUnderlineTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int value=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->underline=(value!=0);
}

static void ProcessStrikeOutTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int value=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->strikeout=(value!=0);
}

static void ProcessWeightTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int weight=50;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&weight) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->weight=weight;
}

static void ProcessSizeTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int size=-1;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&size) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->fontSize=size;
}

static void ProcessFontTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass* )
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    QString fontName;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("name", "QString", (void *)&fontName) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->fontName=fontName;
}

static void ProcessColorTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int red,green,blue;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("red"   , "int", (void *)&red   ) );
    attrProcessingList.append ( AttrProcessing ("green" , "int", (void *)&green ) );
    attrProcessingList.append ( AttrProcessing ("blue"  , "int", (void *)&blue  ) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->colour.setRgb(red, green, blue);
}

static void ProcessTextBackGroundColorTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int red,green,blue;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("red"   , "int", (void *)&red   ) );
    attrProcessingList.append ( AttrProcessing ("green" , "int", (void *)&green ) );
    attrProcessingList.append ( AttrProcessing ("blue"  , "int", (void *)&blue  ) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->textbackgroundColour.setRgb(red, green, blue);
}

static void ProcessVertAlignTag (QDomNode myNode, void* formatDataPtr , QString&, KWEFBaseClass*)
{
    FormatData* formatData = (FormatData*) formatDataPtr;

    int value=-1;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("value", "int", (void *)&value) );
    ProcessAttributes (myNode, attrProcessingList);

    formatData->verticalAlignment=value;
}

static void ProcessSingleFormatTag (QDomNode myNode, void *tagData, QString &, KWEFBaseClass* exportFilter)
{
    FormatData *formatData = (FormatData*) tagData;

    int formatId (-1);

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "id",  "int", (void *) &formatId       ) );
    attrProcessingList.append ( AttrProcessing ( "pos", "int", (void *) &formatData->pos ) );
    attrProcessingList.append ( AttrProcessing ( "len", "int", (void *) &formatData->len ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ( formatId != 1 )
    {
        kdError(30503) << "Unexpected FORMAT attribute id value " << formatId << "!" << endl;
    }

    if ( formatData->pos == -1 || formatData->len == -1 )
    {
        formatData->pos = 0;
        formatData->len = 0;

        kdError(30503) << "Missing formatting!" << endl;
    }

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "ITALIC",    ProcessItalicTag,   (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "UNDERLINE", ProcessUnderlineTag,(void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "STRIKEOUT", ProcessStrikeOutTag,(void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "WEIGHT",    ProcessWeightTag,   (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "SIZE",      ProcessSizeTag,     (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "FONT",      ProcessFontTag,     (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "COLOR",     ProcessColorTag,    (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "VERTALIGN", ProcessVertAlignTag,(void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "TEXTBACKGROUNDCOLOR", ProcessTextBackGroundColorTag, (void*) formatData ) );
    tagProcessingList.append ( TagProcessing ( "LINK",      NULL,               NULL));

    QString strDummy;

    ProcessSubtags (myNode, tagProcessingList, strDummy, exportFilter);

}

static void ProcessIndentsTag (QDomNode myNode, void* tagData , QString&, KWEFBaseClass*)
{
    LayoutData *layout = (LayoutData *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("first" , "double", (void *)&layout->indentFirst ) );
    attrProcessingList.append ( AttrProcessing ("left"  , "double", (void *)&layout->indentLeft  ) );
    attrProcessingList.append ( AttrProcessing ("right" , "double", (void *)&layout->indentRight ) );
    ProcessAttributes (myNode, attrProcessingList);
}

void ProcessLayoutTag ( QDomNode myNode, void *tagData, QString &outputText, KWEFBaseClass* exportFilter )
// Processes <LAYOUT> and <STYLE>
{
    kdDebug(-1) << "Entering ProcessLayoutTag" << endl;

    LayoutData *layout = (LayoutData *) tagData;

    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "NAME",        ProcessLayoutNameTag,     (void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "FOLLOWING",   NULL,                     NULL            ) );
    tagProcessingList.append ( TagProcessing ( "COUNTER",     ProcessCounterTag,        (void*) &layout->counter ) );
    tagProcessingList.append ( TagProcessing ( "FORMAT",      ProcessSingleFormatTag,   (void*) &layout->formatData ) );
    tagProcessingList.append ( TagProcessing ( "TABULATOR",   ProcessLayoutTabulatorTag,(void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "FLOW",        ProcessLayoutFlowTag,     (void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "INDENTS",     ProcessIndentsTag,        (void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "OFFSETS",     ProcessLayoutOffsetTag,   (void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "LINESPACING", ProcessLayoutLineSpacingTag,(void*) layout ) );
    tagProcessingList.append ( TagProcessing ( "PAGEBREAKING",ProcessLineBreakingTag,   (void*) layout  ) );
    tagProcessingList.append ( TagProcessing ( "LEFTBORDER",    NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "RIGHTBORDER",   NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "TOPBORDER",     NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "BOTTOMBORDER",  NULL, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

    kdDebug(-1) << "Exiting ProcessLayoutTag" << endl;
}

static void ProcessFormatTag (QDomNode myNode, void *tagData, QString & outputText, KWEFBaseClass* exportFilter)
{
    // To use in <FORMATS> elements

    ValueListFormatData *formatDataList = (ValueListFormatData *) tagData;

    FormatData formatData (-1,-1);

    ProcessSingleFormatTag(myNode,(void*) &formatData, outputText, exportFilter);

    formatDataList->append (formatData);
}


void ProcessFormatsTag ( QDomNode myNode, void *tagData, QString &outputText, KWEFBaseClass* exportFilter )
{
    ValueListFormatData *formatDataList = (ValueListFormatData *) tagData;

    AllowNoAttributes (myNode);

    (*formatDataList).clear ();
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FORMAT", ProcessFormatTag, (void *) formatDataList ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}

void ProcessTextTag ( QDomNode myNode, void *tagData, QString &, KWEFBaseClass*)
{
    QString *tagText = (QString *) tagData;

    QDomText myText ( myNode.firstChild ().toText () );

    if ( !myText.isNull () )
    {
        *tagText = myText.data ();
    }
    else
    {
        *tagText = "";
    }

    AllowNoAttributes (myNode);

    AllowNoSubtags (myNode);
}
