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

#include <docbookexport.h>
#include <docbookexport.moc>
#include <kdebug.h>
#include <qdom.h>


DocBookExport::DocBookExport ( KoFilter    *parent,
                               const char  *name    ) : KoFilter (parent, name)
{
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
    QDomNode childNode;

    for ( childNode = parentNode.firstChild (); !childNode.isNull (); childNode = childNode.nextSibling () )
    {
//      if ( childNode.isElement () )   // doesn't work!!!
        if ( childNode.nodeType () == QDomNode::ElementNode )
        {
            bool found = false;

            QValueList<TagProcessing>::Iterator  tagProcessingIt;

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
//                      kdError (30502) << "<para>ignoring " << childNode.nodeName ()
//                                      << " tag in " << parentNode.nodeName () << "!</para>" << endl;
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
    QString outputText;
    QValueList<TagProcessing> tagProcessingList;
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
    QDomNamedNodeMap myAttribs ( myNode.toElement ().attributes () );

    int i, n;
    n = myAttribs.length ();

    for ( i = 0; i < n; i++ )
    {
        QDomAttr myAttrib ( myAttribs.item (i).toAttr () );

        if ( !myAttrib.isNull () )
        {
            bool found = false;

            QValueList<AttrProcessing>::Iterator attrProcessingIt;

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
//                      kdError(30502) << "<para>ignoring " << myNode.tagName << " attribute "
//                                     << (*attrProcessingIt).name << "!</para>" << endl;
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
    QValueList<AttrProcessing> attrProcessingList;
    ProcessAttributes (myNode, attrProcessingList);
}


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
// DOC
//   FRAMESETS
//     FRAMESET
//       PARAGRAPH
//          TEXT - Text Element
//          FORMATS
//            FORMAT id=1 pos= len=
//              FONT name=
//              ITALIC value=1
//          LAYOUT
//            NAME value=


void ProcessLayoutNameTag ( QDomNode   myNode,
                            void      *tagData,
                            QString   &         )
{
    QString *layout = (QString *) tagData;

    *layout = "";
    QValueList<AttrProcessing> attrProcessingList;
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
    QString *layout = (QString *) tagData;

    AllowNoAttributes (myNode);

    *layout = "";
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "NAME",      ProcessLayoutNameTag, (void *) layout ) );
    tagProcessingList.append ( TagProcessing ( "FOLLOWING", NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "COUNTER",   NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "FORMAT",    NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "TABULATOR", NULL,                 NULL            ) );
    tagProcessingList.append ( TagProcessing ( "FLOW",      NULL,                 NULL            ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessItalicTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &         )
{
    bool *italic = (bool *) tagData;

    *italic = false;

    int value (-1);
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "value", "int", (void *) &value ) );
    ProcessAttributes (myNode, attrProcessingList);

    switch ( value )
    {
        case 1:
            *italic = true;
            break;

        case -1:
            kdError(30502) << "Bad attributes in ITALIC tag!" << endl;
            break;

        default:
            kdError(30502) << "Unexpected ITALIC attribute value value " << value << "!" << endl;
    }

    AllowNoSubtags (myNode);
}


void ProcessFontTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         )
{
    QString *fontName = (QString *) tagData;

    *fontName = "";
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.clear ();
    attrProcessingList.append ( AttrProcessing ( "name", "QString", (void *) fontName ) );
    ProcessAttributes (myNode, attrProcessingList);

    if ( *fontName == "" )
    {
        kdError(30502) << "Bad font name!" << endl;
    }

    AllowNoSubtags (myNode);
}


// FormatData is a container for data retreived from the FORMAT tag
// and its subtags to be used in the PARAGRAPH tag.

class FormatData
{
    public:
        FormatData ()
        {}

        FormatData ( int     p,
                     int     l,
                     QString f,
                     bool    i  ) : pos (p), len (l), fontName (f), italic (i)
        {}

        int     pos;
        int     len;
        QString fontName;
        bool    italic;
};


void ProcessFormatTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &outputText )
{
    QValueList<FormatData> *formatDataList = (QValueList<FormatData> *) tagData;

    FormatData formatData (-1, -1, "", false);

    int formatId = -1;
    QValueList<AttrProcessing> attrProcessingList;
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

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "SIZE",      NULL,             NULL                          ) );
    tagProcessingList.append ( TagProcessing ( "WEIGHT",    NULL,             NULL                          ) );
    tagProcessingList.append ( TagProcessing ( "UNDERLINE", NULL,             NULL                          ) );
    tagProcessingList.append ( TagProcessing ( "FONT",      ProcessFontTag,   (void *) &formatData.fontName ) );
    tagProcessingList.append ( TagProcessing ( "ITALIC",    ProcessItalicTag, (void *) &formatData.italic   ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);

    (*formatDataList).append (formatData);
}


void ProcessFormatsTag ( QDomNode   myNode,
                         void      *tagData,
                         QString   &outputText )
{
    QValueList<FormatData> *formatDataList = (QValueList<FormatData> *) tagData;

    AllowNoAttributes (myNode);

    (*formatDataList).clear ();
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FORMAT", ProcessFormatTag, (void *) formatDataList ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessTextTag ( QDomNode    myNode,
                      void       *tagData,
                      QString    &         )
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


struct DocData
{
    bool article;
    bool head1;
    bool head2;
    bool head3;
    bool bulletList;
    bool enumeratedList;
    bool alphabeticalList;
};


// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

void ProcessParagraphData ( QString                 &paraText,
                            QValueList<FormatData>  &paraFormatDataList,
                            QString                  tag,
                            QString                 &outputText          )
{
    outputText += "<" + tag + ">";

    if ( paraText.length () > 0 )
    {
        QValueList<FormatData>::Iterator  paraFormatDataIt;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            bool fixedFont = false;

            if ( (*paraFormatDataIt).fontName == "courier" )
            {
                fixedFont = true;
            }

            if ( (*paraFormatDataIt).italic )
            {
                outputText += "<EMPHASIS>";
            }

            if ( fixedFont )
            {
                outputText += "<LITERAL>";
            }

            outputText += paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len )
                                  .replace ( QRegExp ( "&" ), "&amp;" )
                                  .replace ( QRegExp ( "<" ), "&lt;"  )
                                  .replace ( QRegExp ( ">" ), "&gt;"  );

            if ( fixedFont )
            {
                outputText += "</LITERAL>";
            }

            if ( (*paraFormatDataIt).italic )
            {
                outputText += "</EMPHASIS>";
            }
        }
    }

    outputText += "</" + tag + ">\n";
}


void CloseItemizedList ( DocData  &docData,
                         QString  &outputText )
{
    if ( docData.bulletList )
    {
        outputText += "</ITEMIZEDLIST> <!-- End of Bullet List -->\n";
        docData.bulletList = false;
    }
}


void CloseEnumeratedList ( DocData  &docData,
                           QString  &outputText )
{
    if ( docData.enumeratedList )
    {
        outputText += "</ORDEREDLIST> <!-- End of Enumerated List -->\n";
        docData.enumeratedList = false;
    }
}


void CloseAlphabeticalList ( DocData  &docData,
                             QString  &outputText )
{
    if ( docData.alphabeticalList )
    {
        outputText += "</ORDEREDLIST> <!-- End of Alphabetical List -->\n";
        docData.alphabeticalList = false;
    }
}


void CloseLists ( DocData &docData,
                  QString &outputText )
{
    CloseItemizedList ( docData, outputText );
    CloseEnumeratedList ( docData, outputText );
    CloseAlphabeticalList ( docData, outputText );
}


void CloseHead3 ( DocData  &docData,
                  QString  &outputText )
{
    CloseLists ( docData, outputText );

    if ( docData.head3 )
    {
        outputText += "</SECTION> <!-- End of Head 3 -->\n";
        docData.head3 = false;
    }
}


void CloseHead2 ( DocData  &docData,
                  QString  &outputText )
{
    CloseHead3 ( docData, outputText );

    if ( docData.head2 )
    {
        outputText += "</SECTION> <!-- End of Head 2 -->\n";
        docData.head2 = false;
    }
}


void CloseHead1AndArticle ( DocData  &docData,
                            QString  &outputText )
{
    CloseHead2 ( docData, outputText );

    if ( docData.article )
    {
        outputText += "</ARTICLE>\n";
        docData.article = false;
    }

    if ( docData.head1 )
    {
        outputText += "</CHAPTER> <!-- End of Head 1 -->\n";
        docData.head1 = false;
    }
}


void OpenArticleUnlessHead1 ( DocData  &docData,
                              QString  &outputText )
{
    if ( !docData.head1 && !docData.article )
    {
        outputText += "<ARTICLE> <!-- Begin of Article -->\n";
        docData.article = true;
    }
}


void ProcessParagraphTag ( QDomNode   myNode,
                           void      *tagData,
                           QString   &outputText )
{
    DocData *docData = (DocData *) tagData;

    AllowNoAttributes (myNode);

    QString paraText;
    QValueList<FormatData> paraFormatDataList;
    QString paraLayout;
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "TEXT",    ProcessTextTag,    (void *) &paraText           ) );
    tagProcessingList.append ( TagProcessing ( "FORMATS", ProcessFormatsTag, (void *) &paraFormatDataList ) );
    tagProcessingList.append ( TagProcessing ( "LAYOUT",  ProcessLayoutTag,  (void *) &paraLayout         ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);

    if ( paraLayout == "Head 1" )
    {
        CloseHead1AndArticle ( *docData, outputText );

        outputText += "<CHAPTER> <!-- Begin of Head 1 -->\n";
        (*docData).head1 = true;

        ProcessParagraphData ( paraText, paraFormatDataList, "TITLE", outputText );
    }
    else if ( paraLayout == "Head 2" )
    {
        CloseHead2 ( *docData, outputText );

        outputText += "<SECTION> <!-- Begin of Head 2 -->\n";
        (*docData).head2 = true;

        ProcessParagraphData ( paraText, paraFormatDataList, "TITLE", outputText );
    }
    else if ( paraLayout == "Head 3" )
    {
        CloseHead3 ( *docData, outputText );

        outputText += "<SECTION> <!-- Begin of Head 3 -->\n";
        (*docData).head3 = true;

        ProcessParagraphData ( paraText, paraFormatDataList, "TITLE", outputText );
    }
    else if ( paraLayout == "Bullet List" )
    {
        CloseEnumeratedList ( *docData, outputText );
        CloseAlphabeticalList ( *docData, outputText );

        OpenArticleUnlessHead1 ( *docData, outputText );

        if ( !(*docData).bulletList )
        {
            outputText += "<ITEMIZEDLIST> <!-- Begin of Bullet List -->\n";
            (*docData).bulletList = true;
        }

        outputText += "<LISTITEM>\n";
        ProcessParagraphData ( paraText, paraFormatDataList, "PARA", outputText );
        outputText += "</LISTITEM>\n";
    }
    else if ( paraLayout == "Enumerated List" )
    {
        CloseItemizedList ( *docData, outputText );
        CloseAlphabeticalList ( *docData, outputText );

        OpenArticleUnlessHead1 ( *docData, outputText );

        if ( !(*docData).enumeratedList )
        {
            outputText += "<ORDEREDLIST NUMERATION=\"Arabic\"> <!-- Begin of Enumerated List -->\n";
            (*docData).enumeratedList = true;
        }

        outputText += "<LISTITEM>\n";
        ProcessParagraphData ( paraText, paraFormatDataList, "PARA", outputText );
        outputText += "</LISTITEM>\n";
    }
    else if ( paraLayout == "Alphabetical List" )
    {
        CloseItemizedList ( *docData, outputText );
        CloseEnumeratedList ( *docData, outputText );

        OpenArticleUnlessHead1 ( *docData, outputText );

        if ( !(*docData).alphabeticalList )
        {
            outputText += "<ORDEREDLIST NUMERATION=\"Loweralpha\"> <!-- Begin of Alphabetical List -->\n";
            (*docData).alphabeticalList = true;
        }

        outputText += "<LISTITEM>\n";
        ProcessParagraphData ( paraText, paraFormatDataList, "PARA", outputText );
        outputText += "</LISTITEM>\n";
    }
    else
    {
        CloseLists ( *docData, outputText );

        OpenArticleUnlessHead1 ( *docData, outputText );

        if ( paraLayout != "Standard" )
        {
            kdError(30502) << "Unknown layout " + paraLayout + "!" << endl;
        }

        ProcessParagraphData ( paraText, paraFormatDataList, "PARA", outputText );
    }
}


void ProcessFramesetTag ( QDomNode   myNode,
                          void      *tagData,
                          QString   &outputText )
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "frameType", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "frameInfo", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "removable", "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "visible",   "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "name",      "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAME",     NULL,                NULL    ) );
    tagProcessingList.append ( TagProcessing ( "PARAGRAPH", ProcessParagraphTag, tagData ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessFramesetsTag ( QDomNode   myNode,
                           void      *tagData,
                           QString   &outputText  )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, tagData ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessDocTag ( QDomNode   myNode,
                     void      *,
                     QString   &outputText )
{
    QValueList<AttrProcessing> attrProcessingList;
    attrProcessingList.append ( AttrProcessing ( "editor",        "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "mime",          "", NULL ) );
    attrProcessingList.append ( AttrProcessing ( "syntaxVersion", "", NULL ) );
    ProcessAttributes (myNode, attrProcessingList);

    DocData docData;
    docData.article          = false;
    docData.head1            = false;
    docData.head2            = false;
    docData.head3            = false;
    docData.bulletList       = false;
    docData.enumeratedList   = false;
    docData.alphabeticalList = false;
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "PAPER",       NULL,                NULL              ) );
    tagProcessingList.append ( TagProcessing ( "ATTRIBUTES",  NULL,                NULL              ) );
    tagProcessingList.append ( TagProcessing ( "FOOTNOTEMGR", NULL,                NULL              ) );
    tagProcessingList.append ( TagProcessing ( "STYLES",      NULL,                NULL              ) );
    tagProcessingList.append ( TagProcessing ( "PIXMAPS",     NULL,                NULL              ) );
    tagProcessingList.append ( TagProcessing ( "SERIALL",     NULL,                NULL              ) );
    tagProcessingList.append ( TagProcessing ( "FRAMESETS",   ProcessFramesetsTag, (void *) &docData ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);

    CloseHead1AndArticle (docData, outputText);
}


struct BookInfo
{
   QString title;
   QString abstract;
   QString fullName;
   QString jobTitle;
   QString company;
   QString email;
   QString telephone;
   QString fax;
   QString country;
   QString postalCode;
   QString city;
   QString street;
};


void ProcessAboutTag ( QDomNode   myNode,
                       void      *tagData,
                       QString   &outputText )
{
    BookInfo *bookInfo = (BookInfo *) tagData;

    AllowNoAttributes (myNode);

    (*bookInfo).title    = "";
    (*bookInfo).abstract = "";
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "title",    ProcessTextTag, (void *) &(*bookInfo).title    ) );
    tagProcessingList.append ( TagProcessing ( "abstract", ProcessTextTag, (void *) &(*bookInfo).abstract ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessAuthorTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &outputText )
{
    BookInfo *bookInfo = (BookInfo *) tagData;

    AllowNoAttributes (myNode);

    (*bookInfo).fullName   = "";
    (*bookInfo).jobTitle   = "";
    (*bookInfo).company    = "";
    (*bookInfo).email      = "";
    (*bookInfo).telephone  = "";
    (*bookInfo).fax        = "";
    (*bookInfo).country    = "";
    (*bookInfo).postalCode = "";
    (*bookInfo).city       = "";
    (*bookInfo).street     = "";
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "full-name",   ProcessTextTag, (void *) &(*bookInfo).fullName   ) );
    tagProcessingList.append ( TagProcessing ( "title",       ProcessTextTag, (void *) &(*bookInfo).jobTitle   ) );
    tagProcessingList.append ( TagProcessing ( "company",     ProcessTextTag, (void *) &(*bookInfo).company    ) );
    tagProcessingList.append ( TagProcessing ( "email",       ProcessTextTag, (void *) &(*bookInfo).email      ) );
    tagProcessingList.append ( TagProcessing ( "telephone",   ProcessTextTag, (void *) &(*bookInfo).telephone  ) );
    tagProcessingList.append ( TagProcessing ( "fax",         ProcessTextTag, (void *) &(*bookInfo).fax        ) );
    tagProcessingList.append ( TagProcessing ( "country",     ProcessTextTag, (void *) &(*bookInfo).country    ) );
    tagProcessingList.append ( TagProcessing ( "postal-code", ProcessTextTag, (void *) &(*bookInfo).postalCode ) );
    tagProcessingList.append ( TagProcessing ( "city",        ProcessTextTag, (void *) &(*bookInfo).city       ) );
    tagProcessingList.append ( TagProcessing ( "street",      ProcessTextTag, (void *) &(*bookInfo).street     ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);
}


void ProcessInfoData ( QString tagName,
                       QString tagText,
                       QString &outputText )
{
    if ( tagText != "" )
    {
        outputText += "<" + tagName + ">" + tagText + "</" + tagName + ">\n";
    }
}


void ProcessDocumentInfoTag ( QDomNode   myNode,
                              void      *,
                              QString   &outputText )
{
    AllowNoAttributes (myNode);

    BookInfo bookInfo;
    bookInfo.title      = "";
    bookInfo.abstract   = "";
    bookInfo.fullName   = "";
    bookInfo.jobTitle   = "";
    bookInfo.company    = "";
    bookInfo.email      = "";
    bookInfo.telephone  = "";
    bookInfo.fax        = "";
    bookInfo.country    = "";
    bookInfo.postalCode = "";
    bookInfo.city       = "";
    bookInfo.street     = "";
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "log",    NULL,             NULL               ) );
    tagProcessingList.append ( TagProcessing ( "author", ProcessAuthorTag, (void *) &bookInfo ) );
    tagProcessingList.append ( TagProcessing ( "about",  ProcessAboutTag,  (void *) &bookInfo ) );
    ProcessSubtags (myNode, tagProcessingList, outputText);

    QString bookInfoText;
    QString abstractText;
    QString authorText;
    QString affiliationText;
    QString addressText;

    ProcessInfoData ( "TITLE",       bookInfo.title,      bookInfoText    );
    ProcessInfoData ( "PARA",        bookInfo.abstract,   abstractText    );
    ProcessInfoData ( "SURNAME",     bookInfo.fullName,   authorText      );
    ProcessInfoData ( "JOBTITLE",    bookInfo.jobTitle,   affiliationText );
    ProcessInfoData ( "ORGNAME",     bookInfo.company,    affiliationText );
    ProcessInfoData ( "STREET",      bookInfo.street,     addressText     );
    ProcessInfoData ( "CITY",        bookInfo.city,       addressText     );
    ProcessInfoData ( "POSTCODE",    bookInfo.postalCode, addressText     );
    ProcessInfoData ( "COUNTRY",     bookInfo.country,    addressText     );
    ProcessInfoData ( "EMAIL",       bookInfo.email,      addressText     );
    ProcessInfoData ( "PHONE",       bookInfo.telephone,  addressText     );
    ProcessInfoData ( "FAX",         bookInfo.fax,        addressText     );

    ProcessInfoData ( "ADDRESS",     addressText,         affiliationText );
    ProcessInfoData ( "AFFILIATION", affiliationText,     authorText      );
    ProcessInfoData ( "ABSTRACT",    abstractText,        bookInfoText    );
    ProcessInfoData ( "AUTHOR",      authorText,          bookInfoText    );
    ProcessInfoData ( "BOOKINFO",    bookInfoText,        outputText      );
}


const bool DocBookExport::filter ( const QString  &filenameIn,
                                   const QString  &filenameOut,
                                   const QString  &from,
                                   const QString  &to,
                                   const QString  &             )
{
    if ( to != "text/sgml" || from != "application/x-kword" )
    {
        return false;
    }

    QString stringBufOut;

    stringBufOut += "<!doctype book public \"-//OASIS//DTD DocBook V3.1//EN\">\n";

    stringBufOut += "<BOOK>\n";

    KoStore koStoreIn (filenameIn, KoStore::Read);


    {
    if ( !koStoreIn.open ( "documentinfo.xml" ) )
    {
        koStoreIn.close ();

        kdError(30502) << "Unable to open input file!" << endl;
        return false;
    }

    QByteArray byteArrayIn = koStoreIn.read ( koStoreIn.size () );
    koStoreIn.close ();

    QString stringBufIn = QString::fromUtf8 ( (const char *) byteArrayIn, byteArrayIn.size () );

    QDomDocument qDomDocumentIn;
    qDomDocumentIn.setContent (stringBufIn);

    QDomNode docNode = qDomDocumentIn.documentElement ();

    ProcessDocumentInfoTag (docNode, NULL, stringBufOut);
    }


    if ( !koStoreIn.open ( "root" ) )
    {
        koStoreIn.close ();

        kdError(30502) << "Unable to open input file!" << endl;
        return false;
    }

    QByteArray byteArrayIn = koStoreIn.read ( koStoreIn.size () );
    koStoreIn.close ();

    QString stringBufIn = QString::fromUtf8 ( (const char *) byteArrayIn, byteArrayIn.size () );

    QDomDocument qDomDocumentIn;
    qDomDocumentIn.setContent (stringBufIn);

    QDomNode docNode = qDomDocumentIn.documentElement ();

    ProcessDocTag (docNode, NULL, stringBufOut);


    stringBufOut += "</BOOK>\n";


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
