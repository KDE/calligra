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

#include <qiodevice.h>
#include <qtextstream.h>
#include <qregexp.h>

#include <kdebug.h>

#include <KWEFStructures.h>
#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include <docbookexport.h>
#include <docbookexport.moc>

// TODO: this filter needs a new kdebug number (it's the ACSII one!)

class DocBookWorker : public KWEFBaseWorker
{
public:
    DocBookWorker(void) : m_ioDevice(NULL), m_streamOut(NULL) { }
    virtual ~DocBookWorker(void) { }
public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void); // Close file in normal conditions
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList);
    virtual bool doFullDocumentInfo(const KWEFDocumentInfo& docInfo);
private:
    void ProcessParagraphData (const QString& paraText, ValueListFormatData& paraFormatDataList, const QString& tag);
    void CloseItemizedList (void);
    void CloseEnumeratedList (void);
    void CloseAlphabeticalList (void);
    void CloseLists (void);
    void CloseHead3 (void);
    void CloseHead2 (void);
    void CloseHead1AndArticle (void);
    void OpenArticleUnlessHead1 (void);
private:
    QIODevice* m_ioDevice;
    QTextStream* m_streamOut;
    QString m_strDocumentInfo;
    bool m_article;
    bool m_head1;
    bool m_head2;
    bool m_head3;
    bool m_bulletList;
    bool m_enumeratedList;
    bool m_alphabeticalList;
};

bool DocBookWorker::doOpenFile(const QString& filenameOut, const QString& to)
{
    m_ioDevice=new QFile(filenameOut);

    if (!m_ioDevice)
    {
        kdError(30502) << "No output file! Aborting!" << endl;
        return false;
    }

    if ( !m_ioDevice->open (IO_WriteOnly) )
    {
        kdError(30502) << "Unable to open output file!" << endl;
        return false;
    }

    m_streamOut=new QTextStream(m_ioDevice);
    if (!m_ioDevice)
    {
        kdError(30502) << "Could not create output stream! Aborting!" << endl;
        m_ioDevice->close();
        return false;
    }

    // TODO: ask the user for the encoding!
    m_streamOut->setEncoding( QTextStream::Locale );
    return true;
}

bool DocBookWorker::doCloseFile(void)
{
    if (m_ioDevice)
        m_ioDevice->close();
    return (m_ioDevice);
}

bool DocBookWorker::doOpenDocument(void)
{
    m_article          = false;
    m_head1            = false;
    m_head2            = false;
    m_head3            = false;
    m_bulletList       = false;
    m_enumeratedList   = false;
    m_alphabeticalList = false;
    // NOTE: in the original filter DOCTYPE and PUBLIC were lower-case
    //   however in SGML they must be upper-case
    *m_streamOut << "<!DOCTYPE book PUBLIC \"-//OASIS//DTD DocBook V3.1//EN\">\n";
    *m_streamOut << "<BOOK>\n";
    *m_streamOut << m_strDocumentInfo;
    return true;
}

bool DocBookWorker::doCloseDocument(void)
{
    CloseHead1AndArticle();
    *m_streamOut << "</BOOK>\n";
    return true;
}

// ProcessParagraphData () mangles the pure text through the
// formatting information stored in the FormatData list and prints it
// out to the export file.

void DocBookWorker::ProcessParagraphData ( const QString& paraText,
    ValueListFormatData& paraFormatDataList, const QString& tag)
{
    *m_streamOut << "<" + tag + ">";

    if ( paraText.length () > 0 )
    {

        ValueListFormatData::Iterator  paraFormatDataIt;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            bool fixedFont = false;

            if ( (*paraFormatDataIt).fontName.contains("ourier")) // Courier or courier
            {
                fixedFont = true;
            }

            if ( (*paraFormatDataIt).italic )
            {
                *m_streamOut << "<EMPHASIS>";
            }

            if ( fixedFont )
            {
                *m_streamOut << "<LITERAL>";
            }

            *m_streamOut << EscapeXmlText(paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len ));

            if ( fixedFont )
            {
                *m_streamOut << "</LITERAL>";
            }

            if ( (*paraFormatDataIt).italic )
            {
                *m_streamOut << "</EMPHASIS>";
            }
        }
    }

    *m_streamOut << "</" + tag + ">\n";
}


void DocBookWorker::CloseItemizedList (void)
{
    if ( m_bulletList )
    {
        *m_streamOut << "</ITEMIZEDLIST> <!-- End of Bullet List -->\n";
        m_bulletList = false;
    }
}


void DocBookWorker::CloseEnumeratedList (void)
{
    if ( m_enumeratedList )
    {
        *m_streamOut << "</ORDEREDLIST> <!-- End of Enumerated List -->\n";
        m_enumeratedList = false;
    }
}


void DocBookWorker::CloseAlphabeticalList (void)
{
    if ( m_alphabeticalList )
    {
        *m_streamOut << "</ORDEREDLIST> <!-- End of Alphabetical List -->\n";
        m_alphabeticalList = false;
    }
}


void DocBookWorker::CloseLists (void)
{
    DocBookWorker::CloseItemizedList ();
    DocBookWorker::CloseEnumeratedList ();
    DocBookWorker::CloseAlphabeticalList ();
}


void DocBookWorker::CloseHead3 (void)
{
    CloseLists();

    if ( m_head3 )
    {
        *m_streamOut << "</SECTION> <!-- End of Head 3 -->\n";
        m_head3 = false;
    }
}


void DocBookWorker::CloseHead2 (void)
{
    CloseHead3();

    if ( m_head2 )
    {
        *m_streamOut << "</SECTION> <!-- End of Head 2 -->\n";
        m_head2 = false;
    }
}


void DocBookWorker::CloseHead1AndArticle (void)
{
    CloseHead2();

    if ( m_article )
    {
        *m_streamOut << "</ARTICLE>\n";
        m_article = false;
    }

    if ( m_head1 )
    {
        *m_streamOut << "</CHAPTER> <!-- End of Head 1 -->\n";
        m_head1 = false;
    }
}


void DocBookWorker::OpenArticleUnlessHead1 (void)
{
    if ( !m_head1 && !m_article )
    {
        *m_streamOut << "<ARTICLE> <!-- Begin of Article -->\n";
        m_article = true;
    }
}


bool DocBookWorker::doFullParagraph(QString& paraText, LayoutData& layout, ValueListFormatData& paraFormatDataList)
{
    QString paraLayout=layout.styleName;

    if ( paraLayout == "Head 1" )
    {
        CloseHead1AndArticle ();

        *m_streamOut << "<CHAPTER> <!-- Begin of Head 1 -->\n";
        m_head1 = true;

        ProcessParagraphData ( paraText, paraFormatDataList, "TITLE");
    }
    else if ( paraLayout == "Head 2" )
    {
        CloseHead2 ();

        *m_streamOut << "<SECTION> <!-- Begin of Head 2 -->\n";
        m_head2 = true;

        ProcessParagraphData ( paraText, paraFormatDataList, "TITLE");
    }
    else if ( paraLayout == "Head 3" )
    {
        CloseHead3 ();

        *m_streamOut << "<SECTION> <!-- Begin of Head 3 -->\n";
        m_head3 = true;

        ProcessParagraphData ( paraText, paraFormatDataList, "TITLE");
    }
    else if ( paraLayout == "Bullet List" )
    {
        CloseEnumeratedList ();
        CloseAlphabeticalList ();

        OpenArticleUnlessHead1 ();

        if ( !m_bulletList )
        {
            *m_streamOut << "<ITEMIZEDLIST> <!-- Begin of Bullet List -->\n";
            m_bulletList = true;
        }

        *m_streamOut << "<LISTITEM>\n";
        ProcessParagraphData ( paraText, paraFormatDataList, "PARA");
        *m_streamOut << "</LISTITEM>\n";
    }
    else if ( paraLayout == "Enumerated List" )
    {
        CloseItemizedList ();
        CloseAlphabeticalList ();

        OpenArticleUnlessHead1 ();

        if ( !m_enumeratedList )
        {
            *m_streamOut << "<ORDEREDLIST NUMERATION=\"Arabic\"> <!-- Begin of Enumerated List -->\n";
            m_enumeratedList = true;
        }

        *m_streamOut << "<LISTITEM>\n";
        ProcessParagraphData ( paraText, paraFormatDataList, "PARA");
        *m_streamOut << "</LISTITEM>\n";
    }
    else if ( paraLayout == "Alphabetical List" )
    {
        CloseItemizedList ();
        CloseEnumeratedList ();

        OpenArticleUnlessHead1 ();

        if ( !m_alphabeticalList )
        {
            *m_streamOut << "<ORDEREDLIST NUMERATION=\"Loweralpha\"> <!-- Begin of Alphabetical List -->\n";
            m_alphabeticalList = true;
        }

        *m_streamOut << "<LISTITEM>\n";
        ProcessParagraphData ( paraText, paraFormatDataList, "PARA");
        *m_streamOut << "</LISTITEM>\n";
    }
    else
    {
        CloseLists ();

        OpenArticleUnlessHead1 ();

        if ( paraLayout != "Standard" )
        {
            kdError(30502) << "Unknown layout " + paraLayout + "!" << endl;
        }

        ProcessParagraphData ( paraText, paraFormatDataList, "PARA");
    }
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


bool DocBookWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{
    QString bookInfoText;
    QString abstractText;
    QString authorText;
    QString affiliationText;
    QString addressText;

    ProcessInfoData ( "TITLE",       docInfo.title,      bookInfoText    );
    ProcessInfoData ( "PARA",        docInfo.abstract,   abstractText    );
    ProcessInfoData ( "SURNAME",     docInfo.fullName,   authorText      );
    ProcessInfoData ( "JOBTITLE",    docInfo.jobTitle,   affiliationText );
    ProcessInfoData ( "ORGNAME",     docInfo.company,    affiliationText );
    ProcessInfoData ( "STREET",      docInfo.street,     addressText     );
    ProcessInfoData ( "CITY",        docInfo.city,       addressText     );
    ProcessInfoData ( "POSTCODE",    docInfo.postalCode, addressText     );
    ProcessInfoData ( "COUNTRY",     docInfo.country,    addressText     );
    ProcessInfoData ( "EMAIL",       docInfo.email,      addressText     );
    ProcessInfoData ( "PHONE",       docInfo.telephone,  addressText     );
    ProcessInfoData ( "FAX",         docInfo.fax,        addressText     );

    ProcessInfoData ( "ADDRESS",     addressText,         affiliationText );
    ProcessInfoData ( "AFFILIATION", affiliationText,     authorText      );
    ProcessInfoData ( "ABSTRACT",    abstractText,        bookInfoText    );
    ProcessInfoData ( "AUTHOR",      authorText,          bookInfoText    );
    ProcessInfoData ( "BOOKINFO",    bookInfoText,        m_strDocumentInfo );

    return true;
}


DocBookExport::DocBookExport ( KoFilter    *parent,
                               const char  *name    ) : KoFilter (parent, name)
{
}

bool DocBookExport::filter ( const QString  &filenameIn,
                             const QString  &filenameOut,
                             const QString  &from,
                             const QString  &to,
                             const QString  &param )
{
    if ( to != "text/sgml" || from != "application/x-kword" )
    {
        return false;
    }

    DocBookWorker* worker=new DocBookWorker();

    if (!worker)
    {
        kdError(30502) << "Cannot create Worker! Aborting!" << endl;
        return false;
    }

    KWEFKWordLeader* leader=new KWEFKWordLeader(worker);

    if (!leader)
    {
        kdError(30502) << "Cannot create Worker! Aborting!" << endl;
        delete worker;
        return false;
    }

    bool flag=leader->filter(filenameIn,filenameOut,from,to,param);

    delete leader;
    delete worker;

    return flag;
}
