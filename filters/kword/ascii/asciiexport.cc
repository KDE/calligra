/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>

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


ASCIIExport::ASCIIExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}


// The class TagProcessing and the two functions ProcessSubtags () and
// AllowNoSubtags () allow for easing parsing of subtags in the
// current tag. If don't expect any subtags you call AllowNoSubtags ().
// Otherwise you create a list of TagProcessing elements and pass that
// to ProcessSubtags () which will go through all subtags it can find,
// call the corresponding processing function, and do all the
// necessary error handling.


class TagProcessing
{
    public:
        TagProcessing ()
        {}

        TagProcessing (QString  n,
                       void     (*p)(QDomNode, void *, QString &),
                       void    *d) : name (n), processor (p), data (d)
        {}

        QString  name;
        void     (*processor)(QDomNode, void *, QString &);
        void    *data;
};


void ProcessSubtags ( QDomNode                    parentNode,
                      QValueList<TagProcessing>  &tagProcessingList,
                      QString                    &outputText         )
{
    QValueList<TagProcessing>::Iterator  tagProcessingIt;
    bool                                 found;
    QDomNode                             childNode;

    for ( childNode = parentNode.firstChild (); !childNode.isNull (); childNode = childNode.nextSibling () )
    {
//      if ( childNode.isElement () )   // doesn't work!!!
        if ( childNode.nodeType () == QDomNode::ElementNode )
        {
            found = false;

            for ( tagProcessingIt = tagProcessingList.begin ();
                  !found && tagProcessingIt != tagProcessingList.end ();
                  tagProcessingIt++ )
            {
                if ( childNode.nodeName () == (*tagProcessingIt).name )
                {
                    found = true;

                    if ( (*tagProcessingIt).processor != NULL )
                    {
                        ((*tagProcessingIt).processor) ( childNode, (*tagProcessingIt).data, outputText );
                    }
                    else
                    {
//                      outputText += "<para>ignoring " + childNode.nodeName () +
//                                    " tag in " + parentNode.nodeName () + "!</para>\n";
                    }
                }
            }

            if ( !found )
            {
                kdError(30502) << "Unexpected tag " << childNode.nodeName ()
                               << " in " << parentNode.nodeName () << "!" << endl;
            }
        }
    }
}


void AllowNoSubtags ( QDomNode  myNode )
{
    QValueList<TagProcessing>  tagProcessingList;
    QString                    outputText;

    tagProcessingList.clear ();
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


// The class AttrProcessing and the two functions ProcessAttributes ()
// and AllowNoSubtags () allow for easing parsing of the current tag's
// attributes. If don't expect any attributes you call AllowNoAttributes ().
// Otherwise you create a list of AttrProcessing elements and pass
// that to ProcessAttributes () which will go through all attributes
// it can find, retreive the value in the datatype defined, and do all
// the necessary error handling.


class AttrProcessing
{
    public:
        AttrProcessing ()
        {}

        AttrProcessing ( QString   n,
                         QString   t,
                         void     *d  ) : name (n), type (t), data (d)
        {}

        QString   name;
        QString   type;
        void     *data;
};


void ProcessAttributes ( QDomNode                     myNode,
                         QValueList<AttrProcessing>  &attrProcessingList )
{
    QValueList<AttrProcessing>::Iterator attrProcessingIt;
    bool                                 found;
    int                                  i, n;

    QDomNamedNodeMap myAttribs;
    QDomAttr         myAttrib;

    myAttribs = myNode.toElement ().attributes ();
    n = myAttribs.length ();

    for ( i = 0; i < n; i++ )
    {
        myAttrib = myAttribs.item (i).toAttr ();

        if ( !myAttrib.isNull () )
        {
            found = false;

            for ( attrProcessingIt = attrProcessingList.begin ();
                  !found && attrProcessingIt != attrProcessingList.end ();
                  attrProcessingIt++ )
            {
                if ( myAttrib.name () == (*attrProcessingIt).name )
                {
                    found = true;

                    if ( (*attrProcessingIt).data != NULL )
                    {
                        if ( (*attrProcessingIt).type == "QString" )
                        {
                            *((QString *) (*attrProcessingIt).data) = myAttrib.value ();
                        }
                        else if ( (*attrProcessingIt).type == "int" )
                        {
                            *((int *) (*attrProcessingIt).data) = myAttrib.value ().toInt ();
                        }
                        else
                        {
                            kdError(30502) << "Unexpected data type " << (*attrProcessingIt).type << " in " <<
                                           myNode.nodeName () << " attribute " << (*attrProcessingIt).name
                                           << "!" << endl;
                        }
                    }
                    else
                    {
//                      outputText += "<para>ignoring " + myNode.tagName + " attribute " +
//                                    (*attrProcessingIt).name + "!</para>\n";
                    }
                }
            }

            if ( !found )
            {
                kdError(30502) << "Unexpected attribute " << myAttrib.name () << " in " <<
                                  myNode.nodeName () << "!" << endl;
            }
        }
    }
}


void AllowNoAttributes ( QDomNode  myNode )
{
    QValueList<AttrProcessing>  attrProcessingList;

    ProcessAttributes (myNode, attrProcessingList);
}


// Every tag has its own processing function. All of those functions
// have the same parameters since the functions are passed to
// ProcessSubtags throuch the TagProcessing class.  The top level
// function is ProcessDocTag and can be called with the node returned
// by QDomDocument::documentElement (). The tagData argument can be
// used to either pass variables down to the subtags or to allow
// subtags to return values. Currently implemented is processing for
// the following tags and attributes:
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


void ProcessLayoutNameTag ( QDomNode   myNode,
                            void      *tagData,
                            QString   &         )
{
    QValueList<AttrProcessing>   attrProcessingList;
    QString                     *layout;

    layout = (QString *) tagData;

    *layout = "";

    attrProcessingList.clear ();
    attrProcessingList.append ( AttrProcessing ( "value", "QString", (void *) layout ) );

    ProcessAttributes (myNode, attrProcessingList);

    if ( *layout == "" )
    {
        *layout = "Standard";

        kdError(30502) << "Bad layout name value!" << endl;
    }

    AllowNoSubtags (myNode);
}


void ProcessLayoutTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &outputText )
{
    QValueList<TagProcessing>   tagProcessingList;
    QString                    *layout;

    layout = (QString *) tagData;

    AllowNoAttributes (myNode);

    tagProcessingList.clear ();
    tagProcessingList.append ( TagProcessing ( "NAME",      ProcessLayoutNameTag, (void *) layout ) );
    tagProcessingList.append ( TagProcessing ( "FOLLOWING", NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "COUNTER",   NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "FORMAT",    NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "TABULATOR", NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "FLOW",      NULL,                 NULL            ) );

    ProcessSubtags (myNode, tagProcessingList, outputText);
}


// FormatData is a container for data retreived from the FORMAT tag
// and its subtags to be used in the PARAGRAPH tag.

struct FormatData
{
        int                    pos;
        int                    len;
};


void ProcessFormatTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &outputText )
{
    QValueList<AttrProcessing>   attrProcessingList;
    QValueList<TagProcessing>    tagProcessingList;
    QValueList<FormatData>      *formatDataList;
    FormatData                   formatData;
    int                          formatId;

    formatDataList = (QValueList<FormatData> *) tagData;

    formatId = -1;
    formatData.pos = -1;
    formatData.len = -1;

    attrProcessingList.clear ();
    attrProcessingList.append ( AttrProcessing ( "id",  "int", (void *) &formatId       ) );
    attrProcessingList.append ( AttrProcessing ( "pos", "int", (void *) &formatData.pos ) );
    attrProcessingList.append ( AttrProcessing ( "len", "int", (void *) &formatData.len ) );

    ProcessAttributes (myNode, attrProcessingList);

    if ( formatId != 1 )
    {
        kdError(30502) << "Unexpected FORMAT attribute id value " << formatId << "!" << endl;
    }

    if ( formatData.pos == -1 || formatData.len == -1 )
    {
        formatData.pos = 0;
        formatData.len = 0;

        kdError(30502) << "Missing formatting!" << endl;
    }

    tagProcessingList.clear ();
    tagProcessingList.append ( TagProcessing ( "SIZE",      NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "WEIGHT",    NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "UNDERLINE", NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "FONT",      NULL, NULL ) );
    tagProcessingList.append ( TagProcessing ( "ITALIC",    NULL, NULL ) );

    ProcessSubtags (myNode, tagProcessingList, outputText);

    (*formatDataList).append (formatData);
}


void ProcessFormatsTag ( QDomNode   myNode,
                         void      *tagData,
                         QString   &outputText )
{
    QValueList<TagProcessing>   tagProcessingList;
    QValueList<FormatData>     *formatDataList;

    formatDataList = (QValueList<FormatData> *) tagData;

    AllowNoAttributes (myNode);

    (*formatDataList).clear ();

    tagProcessingList.clear ();
    tagProcessingList.append ( TagProcessing ( "FORMAT", ProcessFormatTag, (void *) formatDataList ) );

    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessTextTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         )
{
    QDomText   tagDomText;
    QString   *tagText;

    tagText = (QString *) tagData;

    tagDomText = myNode.firstChild ().toText ();

    if ( !tagDomText.isNull () )
    {
        *tagText = tagDomText.data ();
    }
    else
    {
        *tagText = "";
    }

    AllowNoAttributes (myNode);

    AllowNoSubtags (myNode);
}


// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

void ProcessParagraphData ( QString                 &paraText,
                            QValueList<FormatData>  &paraFormatDataList,
                            QString                 &outputText          )
{
    QValueList<FormatData>::Iterator  paraFormatDataIt;

    if ( paraText.length () > 0 )
    {
        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            outputText += paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
        }
    }

    outputText += "\n";
}


void ProcessParagraphTag ( QDomNode   myNode,
                           void      *,
                           QString   &outputText )
{
    QValueList<TagProcessing>         tagProcessingList;
    QString                           paraText;
    QValueList<FormatData>            paraFormatDataList;
    QString                           paraLayout;

    AllowNoAttributes (myNode);

    paraText = "";
    paraFormatDataList.clear ();
    paraLayout = "";

    tagProcessingList.clear ();
    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,    (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag, (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,  (void *) &paraLayout         ) );

    ProcessSubtags (myNode, tagProcessingList, outputText);

    if ( paraLayout == "Head 1" )
    {
        outputText += "###################################\n";
        outputText += "# ";
        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
        outputText += "###################################\n";
    }
    else if ( paraLayout == "Head 2" )
    {
        outputText += "#### ";
        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    }
    else if ( paraLayout == "Head 3" )
    {
        outputText += "## ";
        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    }
    else if ( paraLayout == "Bullet List" )
    {
        outputText += "o ";
        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    }
    else if ( paraLayout == "Enumerated List" )
    {
        outputText += "1. ";   // less than perfect
        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    }
    else if ( paraLayout == "Alphabetical List" )
    {
        outputText += "a) ";   // less than perfect
        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    }
    else
    {
        if ( paraLayout != "Standard" )
        {
            kdError(30502) << "Unknown layout " + paraLayout + "!" << endl;
        }

        ProcessParagraphData ( paraText, paraFormatDataList, outputText );
    }
}


void ProcessFramesetTag ( QDomNode   myNode,
                          void      *,
                          QString   &outputText )
{
    QValueList<AttrProcessing>  attrProcessingList;
    QValueList<TagProcessing>   tagProcessingList;

    attrProcessingList.clear ();
    attrProcessingList.append ( AttrProcessing ( "frameType", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "frameInfo", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "removable", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "visible",   "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "name",      "", NULL ) );

    ProcessAttributes (myNode, attrProcessingList);

    tagProcessingList.clear ();
    tagProcessingList.append ( TagProcessing ( "FRAME",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "PARAGRAPH", ProcessParagraphTag, NULL ) );

    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessFramesetsTag ( QDomNode   myNode,
                           void      *,
                           QString   &outputText  )
{
    QValueList<TagProcessing>  tagProcessingList;

    AllowNoAttributes (myNode);

    tagProcessingList.clear ();
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, NULL ) );

    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessDocTag ( QDomNode   myNode,
                     void      *,
                     QString   &outputText )
{
    QValueList<AttrProcessing>  attrProcessingList;
    QValueList<TagProcessing>   tagProcessingList;

    attrProcessingList.clear ();
    attrProcessingList.append ( AttrProcessing ( "editor",        "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "mime",          "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "syntaxVersion", "", NULL ) );

    ProcessAttributes (myNode, attrProcessingList);

    tagProcessingList.clear ();
    tagProcessingList.append ( TagProcessing ( "PAPER",       NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "ATTRIBUTES",  NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FOOTNOTEMGR", NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "STYLES",      NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "PIXMAPS",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "SERIALL",     NULL,                NULL ) );
    tagProcessingList.append ( TagProcessing ( "FRAMESETS",   ProcessFramesetsTag, NULL ) );

    ProcessSubtags (myNode, tagProcessingList, outputText);
}


const bool ASCIIExport::filter(const QString  &filenameIn,
                               const QString  &filenameOut,
                               const QString  &from,
                               const QString  &to,
                               const QString  &         )
{
    KoStore                    koStoreIn (filenameIn, KoStore::Read);
    QByteArray                 byteArrayIn;
    QString                    stringBufIn;
    QDomDocument               qDomDocumentIn;
    QDomNode                   docNode;
    QString                    stringBufOut;
    QFile                      fileOut (filenameOut);


    if ( to != "text/plain" || from != "application/x-kword" )
    {
        return false;
    }

    if ( !koStoreIn.open ( "root" ) )
    {
        koStoreIn.close ();

        kdError(30502) << "Unable to open input file!" << endl;
        return false;
    }

    // read the whole file
    byteArrayIn = koStoreIn.read (koStoreIn.size());
    koStoreIn.close ();

    stringBufIn = QString::fromUtf8 ( (const char *) byteArrayIn, byteArrayIn.size () );

    qDomDocumentIn.setContent (stringBufIn);
    docNode = qDomDocumentIn.documentElement ();

    stringBufOut = "";

    ProcessDocTag (docNode, NULL, stringBufOut);

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

