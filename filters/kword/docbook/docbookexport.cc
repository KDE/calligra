/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>
   Copyright (C) 2001 Nicolas GOUTTE

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
#include <qregexp.h>
#include <qdom.h>

#include <KWEFStructures.h>
#include <KWEFUtil.h>
#include <TagProcessing.h>
#include <KWEFBaseClass.h>
#include <ProcessDocument.h>

DocBookExport::DocBookExport ( KoFilter    *parent,
                               const char  *name    ) : KoFilter (parent, name)
{
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
                            ValueListFormatData  &paraFormatDataList,
                            QString                  tag,
                            QString                 &outputText )
{
    outputText += "<" + tag + ">";

    if ( paraText.length () > 0 )
    {
        CreateMissingFormatData(paraText,paraFormatDataList);

        ValueListFormatData::Iterator  paraFormatDataIt;

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

            outputText += EscapeXmlText(paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len ));

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
                           QString   &outputText,
                            KWEFBaseClass           *exportFilter )
{
    DocData *docData = (DocData *) tagData;

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
                          QString   &outputText,
                            KWEFBaseClass           *exportFilter )
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
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}


void ProcessFramesetsTag ( QDomNode   myNode,
                           void      *tagData,
                           QString   &outputText,
                            KWEFBaseClass           *exportFilter  )
{
    AllowNoAttributes (myNode);

    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "FRAMESET", ProcessFramesetTag, tagData ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}


void ProcessDocTag ( QDomNode   myNode,
                     void      *,
                     QString   &outputText,
                            KWEFBaseClass           *exportFilter )
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
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

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
                       QString   &outputText,
                            KWEFBaseClass           *exportFilter )
{
    BookInfo *bookInfo = (BookInfo *) tagData;

    AllowNoAttributes (myNode);

    (*bookInfo).title    = "";
    (*bookInfo).abstract = "";
    QValueList<TagProcessing> tagProcessingList;
    tagProcessingList.append ( TagProcessing ( "title",    ProcessTextTag, (void *) &(*bookInfo).title    ) );
    tagProcessingList.append ( TagProcessing ( "abstract", ProcessTextTag, (void *) &(*bookInfo).abstract ) );
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}


void ProcessAuthorTag ( QDomNode   myNode,
                        void      *tagData,
                        QString   &outputText,
                            KWEFBaseClass           *exportFilter )
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
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);
}


void ProcessInfoData ( QString tagName,
                       QString tagText,
                       QString &outputText)
{
    if ( tagText != "" )
    {
        outputText += "<" + tagName + ">" + tagText + "</" + tagName + ">\n";
    }
}


void ProcessDocumentInfoTag ( QDomNode   myNode,
                              void      *,
                              QString   &outputText,
                            KWEFBaseClass           *exportFilter )
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
    ProcessSubtags (myNode, tagProcessingList, outputText, exportFilter);

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


bool DocBookExport::filter ( const QString  &filenameIn,
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

    ProcessDocumentInfoTag (docNode, NULL, stringBufOut, NULL);
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

    ProcessDocTag (docNode, NULL, stringBufOut, NULL);


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
