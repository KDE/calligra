/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>
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

#include <asciiexport.h>
#include <asciiexport.moc>
#include <kdebug.h>
#include <qdom.h>

#include <KWEFStructures.h>
#include <TagProcessing.h>
#include <KWEFBaseClass.h>
#include <ProcessDocument.h>

ASCIIExport::ASCIIExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

void ProcessParagraphData ( QString                 &paraText,
                            ValueListFormatData     &paraFormatDataList,
                            QString                 &outputText,
                            KWEFBaseClass           * )
{
    if ( paraText.length () > 0 )
    {
        CreateMissingFormatData(paraText,paraFormatDataList);

        ValueListFormatData::Iterator  paraFormatDataIt;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            outputText += paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
        }
    }

    outputText += "\n";
}


void ProcessParagraphTag ( QDomNode      myNode,
                           void          *,
                           QString       &outputText,
                           KWEFBaseClass *exportFilter )
{
    AllowNoAttributes (myNode);

    QString paraText;
    ValueListFormatData paraFormatDataList;
    LayoutData layout;

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,    (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag, (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,  (void *) &layout         ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

    QString paraLayout=layout.styleName;

    if ( paraLayout == "Head 1" )
    {
        outputText += "###################################\n";
        outputText += "# ";
        ProcessParagraphData ( paraText, paraFormatDataList, outputText, exportFilter );
        outputText += "###################################\n";
    }
    else if ( paraLayout == "Head 2" )
    {
        outputText += "#### ";
        ProcessParagraphData ( paraText, paraFormatDataList, outputText, exportFilter );
    }
    else if ( paraLayout == "Head 3" )
    {
        outputText += "## ";
        ProcessParagraphData ( paraText, paraFormatDataList, outputText, exportFilter );
    }
    else if ( paraLayout == "Bullet List" )
    {
        outputText += "o ";
        ProcessParagraphData ( paraText, paraFormatDataList, outputText, exportFilter );
    }
    else if ( paraLayout == "Enumerated List" )
    {
        outputText += "1. ";   // less than perfect
        ProcessParagraphData ( paraText, paraFormatDataList, outputText, exportFilter );
    }
    else if ( paraLayout == "Alphabetical List" )
    {
        outputText += "a) ";   // less than perfect
        ProcessParagraphData ( paraText, paraFormatDataList, outputText, exportFilter );
    }
    else
    {
        if ( paraLayout != "Standard" )
        {
            kdError(30502) << "Unknown layout " + paraLayout + "!" << endl;
        }

        ProcessParagraphData ( paraText, paraFormatDataList, outputText, exportFilter );
    }
}


void ProcessFramesetTag ( QDomNode      myNode,
                          void          *,
                          QString       &outputText,
                          KWEFBaseClass *exportFilter )
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "frameType", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "frameInfo", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "removable", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "visible",   "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "name",      "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAME",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "PARAGRAPH", ProcessParagraphTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}


void ProcessFramesetsTag ( QDomNode      myNode,
                           void          *,
                           QString       &outputText,
                           KWEFBaseClass *exportFilter )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}


void ProcessDocTag ( QDomNode       myNode,
                     void           *,
                     QString        &outputText,
                     KWEFBaseClass  *exportFilter )
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "editor",        "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "mime",          "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "syntaxVersion", "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "PAPER",       NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "ATTRIBUTES",  NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FOOTNOTEMGR", NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "STYLES",      NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "PIXMAPS",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "SERIALL",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FRAMESETS",   ProcessFramesetsTag, NULL ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}


bool ASCIIExport::filter(const QString  &filenameIn,
                         const QString  &filenameOut,
                         const QString  &from,
                         const QString  &to,
                         const QString  &         )
{
    if ( to != "text/plain" || from != "application/x-kword" )
    {
        return false;
    }

    KoStore koStoreIn (filenameIn, KoStore::Read);

    if ( !koStoreIn.open ( "root" ) )
    {
        koStoreIn.close ();

        kdError(30502) << "Unable to open input file!" << endl;
        return false;
    }

    QByteArray byteArrayIn = koStoreIn.read ( koStoreIn.size () );
    koStoreIn.close ();

    QDomDocument qDomDocumentIn;
    qDomDocumentIn.setContent (byteArrayIn);

    QDomNode docNode = qDomDocumentIn.documentElement ();

    QString stringBufOut;

    ProcessDocTag (docNode, NULL, stringBufOut, NULL);

    QFile fileOut (filenameOut);

    if ( !fileOut.open (IO_WriteOnly) )
    {
        fileOut.close ();

        kdError(30502) << "Unable to open output file!" << endl;
        return false;
    }

    fileOut.writeBlock ( (const char *) stringBufOut.local8Bit (), stringBufOut.length () );
    fileOut.close ();

    return true;
}

