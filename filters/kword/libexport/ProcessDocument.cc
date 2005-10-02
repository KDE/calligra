/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (c) 2001 IABG mbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@IABG.de>

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

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Wolf-Michael.Bolle@GMX.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

/*
   Part of the code is based on code licensed under the terms of the
   GNU Library General Public License version 2:
   Copyright 2001 Michael Johnson <mikej@xnet.com>
*/

#include <qdom.h>
#include <qvaluelist.h>

#include <kdebug.h>

#include "KWEFStructures.h"
#include "TagProcessing.h"
#include "ProcessDocument.h"
#include "KWEFKWordLeader.h"


// == KOFFICE DOCUMENT INFORMATION ==

// TODO: verify that all document info is read!

void ProcessTextTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    QString *tagText = (QString *) tagData;

    *tagText = myNode.toElement().text(); // Get the text, also from a CDATA section

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "xml:space" ) );
    ProcessSubtags (myNode, tagProcessingList, leader);

    AllowNoSubtags (myNode, leader);
}

static void ProcessAboutTag ( QDomNode         myNode,
                              void            *tagData,
                              KWEFKWordLeader *leader )
{
    KWEFDocumentInfo *docInfo = (KWEFDocumentInfo *) tagData;

    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "title",    ProcessTextTag, &docInfo->title    ) );
    tagProcessingList.append ( TagProcessing ( "abstract", ProcessTextTag, &docInfo->abstract ) );
    tagProcessingList.append ( TagProcessing ( "keyword",    ProcessTextTag, &docInfo->keywords    ) );
    tagProcessingList.append ( TagProcessing ( "subject", ProcessTextTag, &docInfo->subject ) );
    ProcessSubtags (myNode, tagProcessingList, leader);
}


static void ProcessAuthorTag ( QDomNode         myNode,
                               void            *tagData,
                               KWEFKWordLeader *leader )
{
    KWEFDocumentInfo *docInfo = (KWEFDocumentInfo *) tagData;

    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "full-name",   ProcessTextTag, &docInfo->fullName   ) );
    tagProcessingList.append ( TagProcessing ( "title",       ProcessTextTag, &docInfo->jobTitle   ) );
    tagProcessingList.append ( TagProcessing ( "company",     ProcessTextTag, &docInfo->company    ) );
    tagProcessingList.append ( TagProcessing ( "email",       ProcessTextTag, &docInfo->email      ) );
    tagProcessingList.append ( TagProcessing ( "telephone",   ProcessTextTag, &docInfo->telephone  ) );
    tagProcessingList.append ( TagProcessing ( "telephone-work",   ProcessTextTag, &docInfo->telephonework  ) );
    tagProcessingList.append ( TagProcessing ( "fax",         ProcessTextTag, &docInfo->fax        ) );
    tagProcessingList.append ( TagProcessing ( "country",     ProcessTextTag, &docInfo->country    ) );
    tagProcessingList.append ( TagProcessing ( "postal-code", ProcessTextTag, &docInfo->postalCode ) );
    tagProcessingList.append ( TagProcessing ( "city",        ProcessTextTag, &docInfo->city       ) );
    tagProcessingList.append ( TagProcessing ( "street",      ProcessTextTag, &docInfo->street     ) );
    tagProcessingList.append ( TagProcessing ( "initial",     ProcessTextTag, &docInfo->initial    ) );
    tagProcessingList.append ( TagProcessing ( "position",     ProcessTextTag, &docInfo->position    ) );
    ProcessSubtags (myNode, tagProcessingList, leader);
}


void ProcessDocumentInfoTag ( QDomNode         myNode,
                              void            *,
                              KWEFKWordLeader *leader )
{
    AllowNoAttributes (myNode);

    KWEFDocumentInfo docInfo;

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "log" ) );
    tagProcessingList.append ( TagProcessing ( "author", ProcessAuthorTag, &docInfo ) );
    tagProcessingList.append ( TagProcessing ( "about",  ProcessAboutTag,  &docInfo ) );
    ProcessSubtags (myNode, tagProcessingList, leader);

    leader->doFullDocumentInfo (docInfo);
}


// == KWORD ==

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
// TODO: make this list up-to-date
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


// --------------------------------------------------------------------------------


static void ProcessOneAttrTag ( QDomNode  myNode,
                                QString   attrName,
                                QString   attrType,
                                void     *attrData,
                                KWEFKWordLeader *leader )
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing (attrName, attrType, attrData);
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode, leader);
}


static void ProcessColorAttrTag ( QDomNode myNode, void *tagData, KWEFKWordLeader * )
{
    QColor *attrValue = (QColor *) tagData;

    int red, green, blue;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "red",   red   );
    attrProcessingList << AttrProcessing ( "green", green );
    attrProcessingList << AttrProcessing ( "blue",  blue  );
    ProcessAttributes (myNode, attrProcessingList);

    attrValue->setRgb (red, green, blue);
}


static void ProcessBoolIntAttrTag ( QDomNode  myNode,
                                    QString   attrName,
                                    void     *attrData,
                                    KWEFKWordLeader *leader )
{
    ProcessOneAttrTag (myNode, attrName, "bool", attrData, leader);
}


// --------------------------------------------------------------------------------


static void ProcessIntValueTag (QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    ProcessOneAttrTag (myNode, "value", "int", tagData, leader);
}


static void ProcessBoolIntValueTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    ProcessBoolIntAttrTag (myNode, "value", tagData, leader);
}


static void ProcessStringValueTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    ProcessOneAttrTag (myNode, "value", "QString", tagData, leader);
}

static void ProcessStringNameTag (QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    ProcessOneAttrTag (myNode, "name", "QString", tagData, leader);
}


// --------------------------------------------------------------------------------


static void ProcessOldLayoutChildTag (QDomNode myNode, void *tagData, KWEFKWordLeader* /*leader*/)
{
    QValueList<AttrProcessing> attrProcessingList;

    double* d = (double*) ( tagData );
    *d = 0.0; // Put a sensible default

    attrProcessingList
        << AttrProcessing ( "pt", *d )
        << AttrProcessing ( "inch" )
        << AttrProcessing ( "mm" )
        ;
    ProcessAttributes (myNode, attrProcessingList);
}

static void ProcessUnderlineTag (QDomNode myNode, void *tagData, KWEFKWordLeader* /*leader*/ )
{
    TextFormatting* text=(TextFormatting*) tagData;
    QString str,style;
    QString strColor;

    text->underlineWord = false;

    QValueList<AttrProcessing> attrProcessingList;

    attrProcessingList
        << AttrProcessing ( "value",   str )
        << AttrProcessing ( "styleline", style )
        << AttrProcessing ( "wordbyword", text->underlineWord )
        << AttrProcessing ( "underlinecolor",   strColor )
        ;
    ProcessAttributes (myNode, attrProcessingList);

    str=str.stripWhiteSpace();
    text->underlineValue=str;
    if ( (str=="0") || (str.isEmpty()) )
    {
        text->underline=false;
    }
    else
    {
        // We assume that anything else is underlined
        text->underline=true;
        text->underlineStyle = style;
        text->underlineColor.setNamedColor(strColor);
    }
}

static void ProcessStrikeoutTag (QDomNode myNode, void *tagData, KWEFKWordLeader* /*leader*/ )
{
    TextFormatting* text=(TextFormatting*) tagData;
    QString type, linestyle;

    text->strikeoutWord = false;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ("value" , type );
    attrProcessingList << AttrProcessing ("styleline" , linestyle );
    attrProcessingList << AttrProcessing ( "wordbyword", text->strikeoutWord );
    ProcessAttributes (myNode, attrProcessingList);

    if( type.isEmpty() || ( type == "0" ) )
        text->strikeout = false;
    else
    {
        text->strikeout = true;
        text->strikeoutType = type;
        text->strikeoutLineStyle = linestyle;
        if( text->strikeoutType == "1" )
            text->strikeoutType = "single";
        if( text->strikeoutLineStyle.isEmpty() )
            text->strikeoutLineStyle = "solid";
    }
}


void ProcessAnchorTag ( QDomNode       myNode,
                        void          *tagData,
                        KWEFKWordLeader *leader )
{
    QString *instance = (QString *) tagData;

    QString type;
    *instance = QString::null;
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "type",     type    )
                       << AttrProcessing ( "instance", "QString", instance );
    ProcessAttributes (myNode, attrProcessingList);

    if ( type != "frameset" )
    {
        kdWarning (30508) << "Unknown ANCHOR type " << type << "!" << endl;
    }

    if ( (*instance).isEmpty () )
    {
        kdWarning (30508) << "Bad ANCHOR instance name!" << endl;
    }

    AllowNoSubtags (myNode, leader);
}


static void ProcessLinkTag (QDomNode myNode, void *tagData, KWEFKWordLeader *)
{
    VariableData *variable = (VariableData *) tagData;

    QString linkName, hrefName;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("linkName", linkName) );
    attrProcessingList.append ( AttrProcessing ("hrefName", hrefName) );
    ProcessAttributes (myNode, attrProcessingList);

    variable->setLink(linkName, hrefName);
}


static void ProcessPgNumTag (QDomNode myNode, void *tagData, KWEFKWordLeader *)
{
    VariableData *variable = (VariableData *) tagData;

    QString subtype, value;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("subtype", subtype) );
    attrProcessingList.append ( AttrProcessing ("value",   value  ) );
    ProcessAttributes (myNode, attrProcessingList);

    variable->setPgNum(subtype, value);
}


static void ProcessTypeTag (QDomNode myNode, void *tagData, KWEFKWordLeader *)
{
    VariableData *variable = (VariableData *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("key",  variable->m_key ) );
    attrProcessingList.append ( AttrProcessing ("text", variable->m_text) );
    attrProcessingList.append ( AttrProcessing ("type", variable->m_type) );
    ProcessAttributes (myNode, attrProcessingList);
}

static void ProcessFieldTag (QDomNode myNode, void *tagData, KWEFKWordLeader *)
{
    VariableData *variable = (VariableData *) tagData;
    int subtype;
    QString name, value;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ("subtype", subtype) );
    attrProcessingList.append ( AttrProcessing ("value", value ) );
    ProcessAttributes (myNode, attrProcessingList);

    switch( subtype )
    {
    case  0:  name = "fileName"; break;
    case  1:  name = "dirName"; break;
    case  2:  name = "authorName"; break;
    case  3:  name = "authorEmail"; break;
    case  4:  name = "authorCompany"; break;
    case 10:  name = "docTitle"; break;
    case 11:  name = "docAbstract"; break;
    case 16:  name = "authorInitial"; break;
    default: break;
    }

    if(!name.isEmpty())
        variable->setField(name, value);
}

static void ProcessFootnoteTag (QDomNode myNode, void *tagData, KWEFKWordLeader *leader)
{
    VariableData *variable = (VariableData *) tagData;
    QString frameset, value, numberingtype, notetype;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList
        << AttrProcessing ( "value", value )
        << AttrProcessing ( "numberingtype", numberingtype )
        << AttrProcessing ( "frameset", frameset )
        << AttrProcessing ( "notetype", notetype )
        ;
    ProcessAttributes (myNode, attrProcessingList);

    // search for frameset in the footnoteList
    for(unsigned i=0;i<leader->footnoteList.count();i++)
    {
       if( leader->footnoteList[i].frameName == frameset )
       {
           variable->setFootnote( notetype, numberingtype, value, &leader->footnoteList[i].para );
           break;
       }
    }
}

static void ProcessNoteTag (QDomNode myNode, void *tagData, KWEFKWordLeader *leader)
{
    VariableData *variable = (VariableData *) tagData;

    QString note;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList
        << AttrProcessing ( "note", note )
        ;
    ProcessAttributes (myNode, attrProcessingList);

    // set it even if note is empty
    variable->setGenericData( "note", note );
}

// ### TODO: some files have not a <VARIABLE> tag but its supposed children are directly children of <FORMAT id="4">
static void ProcessVariableTag (QDomNode myNode, void* tagData, KWEFKWordLeader* leader)
{
    VariableData *variable = (VariableData *) tagData;

    QValueList<TagProcessing> tagProcessingList;
    // "TYPE|PGNUM|DATE|TIME|CUSTOM|SERIALLETTER|FIELD|LINK|NOTE"
    tagProcessingList
        << TagProcessing ( "TYPE",          ProcessTypeTag,         variable )
        << TagProcessing ( "PGNUM",         ProcessPgNumTag,        variable )
        << TagProcessing ( "DATE" )
        << TagProcessing ( "TIME" )
        << TagProcessing ( "CUSTOM" )
        << TagProcessing ( "SERIALLETTER" )
        << TagProcessing ( "FIELD",         ProcessFieldTag,        variable )
        << TagProcessing ( "LINK",          ProcessLinkTag,         variable )
        << TagProcessing ( "NOTE",          ProcessNoteTag,         variable )
        << TagProcessing ( "FOOTNOTE",      ProcessFootnoteTag,     variable )
        ;
    ProcessSubtags (myNode, tagProcessingList, leader);
}

static void AppendTagProcessingFormatOne(QValueList<TagProcessing>& tagProcessingList, FormatData& formatData)
{
    tagProcessingList
        << TagProcessing ( "COLOR",               ProcessColorAttrTag,    &formatData.text.fgColor           )
        << TagProcessing ( "FONT",                ProcessStringNameTag,   &formatData.text.fontName          )
        << TagProcessing ( "SIZE",                ProcessIntValueTag,     &formatData.text.fontSize          )
        << TagProcessing ( "WEIGHT",              ProcessIntValueTag,     &formatData.text.weight            )
        << TagProcessing ( "ITALIC",              ProcessBoolIntValueTag, &formatData.text.italic            )
        << TagProcessing ( "UNDERLINE",           ProcessUnderlineTag,    &formatData.text                   )
        << TagProcessing ( "STRIKEOUT",           ProcessStrikeoutTag,    &formatData.text                   )
        << TagProcessing ( "VERTALIGN",           ProcessIntValueTag,     &formatData.text.verticalAlignment )
        << TagProcessing ( "SHADOW" )
        << TagProcessing ( "FONTATTRIBUTE",       ProcessStringValueTag,  &formatData.text.fontAttribute     )
        << TagProcessing ( "LANGUAGE",            ProcessStringValueTag,  &formatData.text.language          )
        << TagProcessing ( "ANCHOR" )
        << TagProcessing ( "IMAGE" )
        << TagProcessing ( "PICTURE" )
        << TagProcessing ( "VARIABLE" )
        << TagProcessing ( "TEXTBACKGROUNDCOLOR", ProcessColorAttrTag,    &formatData.text.bgColor           )
        << TagProcessing ( "OFFSETFROMBASELINE" )
        << TagProcessing ( "CHARSET" ) // Old KWord documents or KWord's RTF import filter
        ;

    if ( formatData.text.language == "xx" )
    {
        // The text language should have been named "x-test" or so to follow the specification
        // As it does not, we need to convert it.
        formatData.text.language = "en_US";
    }
}


static void SubProcessFormatOneTag(QDomNode myNode,
    ValueListFormatData *formatDataList, int formatPos, int formatLen,
    KWEFKWordLeader *leader)
{
    if ( formatPos == -1 || formatLen == -1 )
    {
        // We have no position and no length defined
        // It can happen in a child of <STYLE>, just put secure values
        formatPos=0;
        formatLen=0;
        kdDebug (30508) << "Missing formatting! Style? "
                        << myNode.nodeName()
                        << " = " << myNode.nodeValue()
                        << endl;

        // In the old syntax (KWord 0.8), the comment would be displayed for each paragraph, so do not show it.
        if ( ! leader->m_oldSyntax )
            kdDebug (30508) << "Missing formatting for <FORMAT> (style or syntax version 1 ?)" << endl;
    }

    FormatData formatData(1, formatPos, formatLen);
    QValueList<TagProcessing> tagProcessingList;
    AppendTagProcessingFormatOne(tagProcessingList,formatData);
    ProcessSubtags (myNode, tagProcessingList, leader);

    (*formatDataList) << formatData;
}


static void SubProcessFormatTwoTag(QDomNode myNode,
    ValueListFormatData *formatDataList, int formatPos, int formatLen,
    KWEFKWordLeader *leader)
{
    if ( (formatPos == -1) )
    {
        // We have no position defined
        kdWarning(30508) << "Missing text image position!" << endl;
        return;
    }
    // In KWord 0.8, the len attribute was not defined
    if (formatLen == -1)
        formatLen = 1;

    FormatData formatData(2, formatPos, formatLen);
    QValueList<TagProcessing> tagProcessingList;

    QString fileName; // KWord 0.8
    KoPictureKey key; // Re-saved by KWord 1.2 or KWord 1.3
    tagProcessingList.append(TagProcessing( "FILENAME", ProcessStringValueTag, &fileName));
    tagProcessingList.append(TagProcessing( "PICTURE",  ProcessImageTag, &key ));
    ProcessSubtags (myNode, tagProcessingList, leader);

    if ( !fileName.isEmpty() )
    {
        kdDebug(30508) << "KWord 0.8 text image: " << fileName << endl;
        key = KoPictureKey( fileName );
    }
    else
    {
        kdDebug(30508) << "KWord 1.2/1.3 text image: " << key.toString() << endl;
    }

    formatData.frameAnchor.key = key;
    formatData.frameAnchor.picture.key = key;

    (*formatDataList) << formatData;
}


static void SubProcessFormatThreeTag(QDomNode myNode,
    ValueListFormatData *formatDataList, int formatPos, int /*formatLen*/,
    KWEFKWordLeader *leader)
{
    if ( (formatPos == -1) ) // formatLen is never there but is 1.
    {
        // We have no position and no length defined
        kdWarning(30508) << "Missing variable formatting!" << endl;
        return;
    }
    AllowNoSubtags (myNode, leader);

    const FormatData formatData(3, formatPos, 1);
    (*formatDataList) << formatData;
}

static void SubProcessFormatFourTag(QDomNode myNode,
    ValueListFormatData *formatDataList, int formatPos, int formatLen,
    KWEFKWordLeader *leader)
{
    if ( (formatPos == -1) || (formatLen == -1) )
    {
        // We have no position and no length defined
        kdWarning(30508) << "Missing variable formatting!" << endl;
        return;
    }
    FormatData formatData(4, formatPos, formatLen);
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append(TagProcessing("VARIABLE",   ProcessVariableTag, &formatData.variable));
    // As variables can have a formating too, we have to process formating
    AppendTagProcessingFormatOne(tagProcessingList,formatData);
    ProcessSubtags (myNode, tagProcessingList, leader);

    (*formatDataList) << formatData;
}


static void SubProcessFormatSixTag(QDomNode myNode,
    ValueListFormatData *formatDataList, int formatPos, int formatLen,
    KWEFKWordLeader *leader)
{
    if ( formatPos != -1 && formatLen != -1 )
    {
        QString instance;

        QValueList<TagProcessing> tagProcessingList;
        // TODO: We can have all layout information as in regular texts
        //       They simply apply to the table frames
        //       FONT is just the first that we've come across so far
        tagProcessingList << TagProcessing ( "FONT" )
                            << TagProcessing ( "ANCHOR", ProcessAnchorTag, &instance );
        ProcessSubtags (myNode, tagProcessingList, leader);
#if 0
        kdDebug (30508) << "DEBUG: Adding frame anchor " << instance << endl;
#endif

        (*formatDataList) << FormatData ( formatPos, formatLen, FrameAnchor (instance) );
    }
    else
    {
        kdWarning (30508) << "Missing or bad anchor formatting!" << endl;
    }
}

static void ProcessFormatTag (QDomNode myNode, void *tagData, KWEFKWordLeader *leader)
{
    ValueListFormatData *formatDataList = (ValueListFormatData *) tagData;
    int formatId  = -1;
    int formatPos = -1;
    int formatLen = -1;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "id",  formatId  );
    attrProcessingList << AttrProcessing ( "pos", formatPos );
    attrProcessingList << AttrProcessing ( "len", formatLen );
    ProcessAttributes (myNode, attrProcessingList);

    if ( ( formatId == -1 ) && ( leader->m_oldSyntax ) )
    {
        formatId = 1; // KWord 0.8 did not define it in <LAYOUT>
    }

    switch ( formatId )
    {
    case 1: // regular texts
        {
            SubProcessFormatOneTag(myNode, formatDataList, formatPos, formatLen, leader);
            break;
        }
    case 2: // text image (KWord 0.8)
        {
            SubProcessFormatTwoTag(myNode, formatDataList, formatPos, formatLen, leader);
            break;
        }
    case 3: // KWord 0.8 tabulator
        {
            SubProcessFormatThreeTag(myNode, formatDataList, formatPos, formatLen, leader);
            break;
        }
    case 4: // variables
        {
            SubProcessFormatFourTag(myNode, formatDataList, formatPos, formatLen, leader);
            break;
        }
    case 6: // anchors
        {
            SubProcessFormatSixTag(myNode, formatDataList, formatPos, formatLen, leader);
            break;
        }
    case -1:
        {
            kdWarning (30508) << "FORMAT attribute id value not set!" << endl;
            AllowNoSubtags (myNode, leader);
            break;
        }
    case 5: // KWord 0.8 footnote
    default:
            kdWarning(30508) << "Unexpected FORMAT attribute id value " << formatId << endl;
            AllowNoSubtags (myNode, leader);
    }

}


void ProcessFormatsTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    ValueListFormatData *formatDataList = (ValueListFormatData *) tagData;

    AllowNoAttributes (myNode);

    (*formatDataList).clear ();
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "FORMAT", ProcessFormatTag, formatDataList );
    ProcessSubtags (myNode, tagProcessingList, leader);
}


// --------------------------------------------------------------------------------


static void ProcessCounterTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    CounterData *counter = (CounterData *) tagData;

    // Be extra careful with data declared as enum (it could be very different from an int)
    int counterStyle = counter->style;
    int counterNumbering = counter->numbering;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "type",            counterStyle );
    attrProcessingList << AttrProcessing ( "depth",           counter->depth           );
    attrProcessingList << AttrProcessing ( "bullet",          counter->customCharacter );
    attrProcessingList << AttrProcessing ( "start",           counter->start           );
    attrProcessingList << AttrProcessing ( "numberingtype",   counterNumbering );
    attrProcessingList << AttrProcessing ( "lefttext",        counter->lefttext        );
    attrProcessingList << AttrProcessing ( "righttext",       counter->righttext       );
    attrProcessingList << AttrProcessing ( "bulletfont",      counter->customFont      );
    attrProcessingList << AttrProcessing ( "customdef"  );
    attrProcessingList << AttrProcessing ( "text",            counter->text            );
    attrProcessingList << AttrProcessing ( "display-levels" );
    attrProcessingList << AttrProcessing ( "align" );
    ProcessAttributes (myNode, attrProcessingList);

    counter->style = CounterData::Style( counterStyle );
    counter->numbering = CounterData::Numbering( counterNumbering );

    AllowNoSubtags (myNode, leader);
}


static void ProcessLayoutTabulatorTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    TabulatorList* tabulatorList = (TabulatorList*) tagData;

    TabulatorData tabulator;

    QValueList<AttrProcessing> attrProcessingList;

    attrProcessingList
        << AttrProcessing ( "ptpos",   tabulator.m_ptpos   )
        << AttrProcessing ( "type",    tabulator.m_type    )
        << AttrProcessing ( "filling", tabulator.m_filling )
        << AttrProcessing ( "width",   tabulator.m_width   )
        << AttrProcessing ( "alignchar" )
        ;

    if ( leader->m_oldSyntax )
    {
        // Avoid too many warning
        attrProcessingList
            << AttrProcessing ( "mmpos" )
            << AttrProcessing ( "inchpos" ) // Never ever use it, as this value is mostly wrong (e.g. 1.1009e+15)
        ;
    }

    ProcessAttributes (myNode, attrProcessingList);
    tabulatorList->append(tabulator);

    AllowNoSubtags (myNode, leader);
}


static void ProcessIndentsTag (QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    LayoutData *layout = (LayoutData *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ("first" , layout->indentFirst );
    attrProcessingList << AttrProcessing ("left"  , layout->indentLeft  );
    attrProcessingList << AttrProcessing ("right" , layout->indentRight );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode, leader);
}


static void ProcessLayoutOffsetTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    LayoutData *layout = (LayoutData *) tagData;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ("after" ,  layout->marginBottom );
    attrProcessingList << AttrProcessing ("before" , layout->marginTop    );
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode, leader);
}


static void ProcessLineBreakingTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    LayoutData *layout = (LayoutData *) tagData;

    QString strBefore, strAfter;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "linesTogether",       layout->keepLinesTogether  );
    attrProcessingList << AttrProcessing ( "hardFrameBreak",      layout->pageBreakBefore );
    attrProcessingList << AttrProcessing ( "hardFrameBreakAfter", layout->pageBreakAfter  );
    attrProcessingList << AttrProcessing ( "keepWithNext" ); // RTF import filter
    ProcessAttributes (myNode, attrProcessingList);

    AllowNoSubtags (myNode, leader);
}


static void ProcessShadowTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader)
{
    LayoutData *layout = (LayoutData *) tagData;

    int red=0;
    int green=0;
    int blue=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "distance", layout->shadowDistance );
    attrProcessingList << AttrProcessing ( "direction", layout->shadowDirection );
    attrProcessingList << AttrProcessing ( "red",      red   );
    attrProcessingList << AttrProcessing ( "green",    green );
    attrProcessingList << AttrProcessing ( "blue",     blue  );
    ProcessAttributes (myNode, attrProcessingList);

    layout->shadowColor.setRgb(red,green,blue);

    AllowNoSubtags (myNode, leader);
}

static void ProcessAnyBorderTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    BorderData *border =  static_cast <BorderData*> (tagData);

    int red=0;
    int green=0;
    int blue=0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "red",   red   );
    attrProcessingList << AttrProcessing ( "green", green );
    attrProcessingList << AttrProcessing ( "blue",  blue  );
    attrProcessingList << AttrProcessing ( "style", border->style );
    attrProcessingList << AttrProcessing ( "width", border->width );
    ProcessAttributes (myNode, attrProcessingList);

    border->color.setRgb(red,green,blue);

    AllowNoSubtags (myNode, leader);
}

static void ProcessFollowingTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    ProcessOneAttrTag (myNode, "name", "QString", tagData, leader);
}

static void ProcessLinespacingTag (QDomNode myNode, void *tagData, KWEFKWordLeader* /*leader*/ )
{
    LayoutData *layout = (LayoutData *) tagData;
    QString oldValue, spacingType;
    double spacingValue;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ("value" , oldValue );
    attrProcessingList << AttrProcessing ("type" , spacingType );
    attrProcessingList << AttrProcessing ("spacingvalue"  , spacingValue  );
    ProcessAttributes (myNode, attrProcessingList);

    // KWord pre-1.2 uses only the "value" attribute (stored in oldValue)
    // while 1.2 uses mainly "type" and "spacingvalue", while keeping "value" for compatibility

    if ( spacingType.isEmpty() )
    {
        // for old format
        if( oldValue == "oneandhalf" )
            layout->lineSpacingType = LayoutData::LS_ONEANDHALF;
        else if ( oldValue == "double" )
            layout->lineSpacingType = LayoutData::LS_DOUBLE;
        else
        {
            bool ok = false;
            const double size = oldValue.toDouble( &ok );
            if ( ok && ( size >= 0.0 ) ) // 0 is allowed but negative values are not
            {
                // We have a valid size
                layout->lineSpacingType = LayoutData::LS_CUSTOM; // set to custom
                layout->lineSpacing     = size;
            }
            else
               layout->lineSpacingType = LayoutData::LS_SINGLE; // assume single linespace
         }
    }
    else
    {
        // for new format
        if( spacingType == "oneandhalf" )
            layout->lineSpacingType = LayoutData::LS_ONEANDHALF;
        else if ( spacingType == "double" )
            layout->lineSpacingType = LayoutData::LS_DOUBLE;
        else if ( spacingType == "custom" )
            layout->lineSpacingType = LayoutData::LS_CUSTOM;
        else if ( spacingType == "atleast" )
            layout->lineSpacingType = LayoutData::LS_ATLEAST;
        else if ( spacingType == "multiple" )
            layout->lineSpacingType = LayoutData::LS_MULTIPLE;
        else if ( spacingType == "fixed" )
            layout->lineSpacingType = LayoutData::LS_FIXED;
        else
             layout->lineSpacingType = LayoutData::LS_SINGLE; // assume single linespace
        layout->lineSpacing = spacingValue;
    }
}

static void ProcessLineSpaceTag (QDomNode myNode, void *tagData, KWEFKWordLeader* /*leader*/ )
{
    // <LINESPACE> is an old tag, of before syntax 1
    LayoutData *layout = (LayoutData *) tagData;
    double spacingValue = 0.0;

    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "pt", spacingValue );
    attrProcessingList << AttrProcessing ( "mm" );
    attrProcessingList << AttrProcessing ( "inch" );
    ProcessAttributes (myNode, attrProcessingList);

    layout->lineSpacingType = LayoutData::LS_CUSTOM; // set to custom
    layout->lineSpacing     = spacingValue;
}

static void ProcessFlowTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
{
    LayoutData *layout = (LayoutData *) tagData;

    QString oldAlign, normalAlign;

    QValueList<AttrProcessing> attrProcessingList;
    if ( leader->m_oldSyntax )
    {
        // KWord 0.8
        attrProcessingList << AttrProcessing ( "value", oldAlign ); // KWord 0.8
    }
    // New syntax and some files from syntax 1
    attrProcessingList << AttrProcessing ( "align", normalAlign );
    attrProcessingList << AttrProcessing ( "dir" ); // ### TODO
    ProcessAttributes (myNode, attrProcessingList);

    if ( leader->m_oldSyntax && normalAlign.isEmpty() )
    {
        if ( oldAlign.isEmpty() )
        {
            layout->alignment = "left"; // KWord 0.8 did not support right-to-left
        }
        else
        {
            const int align = oldAlign.toInt();
            if ( ( align < 0 ) || ( align > 3) )
            {
                kdWarning(30508) << "KWord 0.8 flow unknown: " << oldAlign << endl;
                layout->alignment = "left"; // Unknown, so assume left
            }
            else
            {
                const char* flows[]={"left", "right", "center", "justify" };
                layout->alignment = flows[ align ];
            }
        }
        kdDebug(30508) << "KWord 0.8 flow: " << oldAlign << " corrected: " << layout->alignment << endl;
    }
    else
    {
        layout->alignment = normalAlign;
    }

}


void ProcessLayoutTag ( QDomNode myNode, void *tagData, KWEFKWordLeader *leader )
// Processes <LAYOUT> and <STYLE>
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "outline" ); // Only in <STYLE>
    ProcessAttributes (myNode, attrProcessingList);

    LayoutData *layout = (LayoutData *) tagData;

    ValueListFormatData formatDataList;

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "NAME",         ProcessStringValueTag,       &layout->styleName           );
    tagProcessingList << TagProcessing ( "FOLLOWING",    ProcessFollowingTag,         &layout->styleFollowing      );
    tagProcessingList << TagProcessing ( "FLOW",         ProcessFlowTag,              layout );
    tagProcessingList << TagProcessing ( "INDENTS",      ProcessIndentsTag,           layout              );
    tagProcessingList << TagProcessing ( "OFFSETS",      ProcessLayoutOffsetTag,      layout              );
    tagProcessingList << TagProcessing ( "LINESPACING",  ProcessLinespacingTag,       layout              );
    tagProcessingList << TagProcessing ( "PAGEBREAKING", ProcessLineBreakingTag,      layout              );
    tagProcessingList << TagProcessing ( "LEFTBORDER",   ProcessAnyBorderTag,         &layout->leftBorder          );
    tagProcessingList << TagProcessing ( "RIGHTBORDER",  ProcessAnyBorderTag,         &layout->rightBorder         );
    tagProcessingList << TagProcessing ( "TOPBORDER",    ProcessAnyBorderTag,         &layout->topBorder           );
    tagProcessingList << TagProcessing ( "BOTTOMBORDER", ProcessAnyBorderTag,         &layout->bottomBorder        );
    tagProcessingList << TagProcessing ( "COUNTER",      ProcessCounterTag,           &layout->counter    );
    tagProcessingList << TagProcessing ( "FORMAT",       ProcessFormatTag,            &formatDataList     );
    tagProcessingList << TagProcessing ( "TABULATOR",    ProcessLayoutTabulatorTag,   &layout->tabulatorList       );
    tagProcessingList << TagProcessing ( "SHADOW",       ProcessShadowTag,            layout                       );

    if ( leader->m_oldSyntax )
    {
        layout->indentLeft = 0.0; // ### TODO: needed or not?
        tagProcessingList
            << TagProcessing ( "OHEAD",  ProcessOldLayoutChildTag, &layout->marginTop )
            << TagProcessing ( "OFOOT",  ProcessOldLayoutChildTag, &layout->marginBottom )
            << TagProcessing ( "ILEFT",  ProcessOldLayoutChildTag, &layout->indentLeft )
            << TagProcessing ( "IFIRST", ProcessOldLayoutChildTag, &layout->indentFirst )
            << TagProcessing ( "LINESPACE", ProcessLineSpaceTag,   layout )
            ;
    }

    ProcessSubtags (myNode, tagProcessingList, leader);


    if ( formatDataList.isEmpty () )
    {
        kdWarning (30508) << "No FORMAT tag within LAYOUT/STYLE!" << endl;
    }
    else
    {
        layout->formatData = formatDataList.first ();

        if ( formatDataList.count () > 1 )
        {
            kdWarning (30508) << "More than one FORMAT tag within LAYOUT/STYLE!" << endl;
        }
    }

    if ( layout->styleName.isEmpty () )
    {
        layout->styleName = "Standard";
        kdWarning (30508) << "Empty layout name!" << endl;
    }

}


static void ProcessImageKeyTag ( QDomNode myNode,
    void *tagData, KWEFKWordLeader *)
{
    KoPictureKey *key = (KoPictureKey*) tagData;

    // Let KoPicture do the loading
    key->loadAttributes(myNode.toElement());
}

void ProcessImageTag ( QDomNode myNode,
    void *tagData, KWEFKWordLeader *leader )
{ // <PICTURE>
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList << AttrProcessing ( "keepAspectRatio" );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList << TagProcessing ( "KEY", ProcessImageKeyTag, tagData );
    ProcessSubtags (myNode, tagProcessingList, leader);
}

