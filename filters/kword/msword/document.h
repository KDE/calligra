/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <handlers.h>
#include <functor.h>

#include <qstring.h>
#include <qdom.h>
#include <qobject.h>

#include <queue>
#include <string>

namespace wvWare {
    class Parser;
}
class KWordReplacementHandler;
class KWordTextHandler;

class Document : public QObject, public wvWare::SubDocumentHandler
{
    Q_OBJECT
public:
    Document( const std::string& fileName, QDomDocument& mainDocument, QDomDocument &documentInfo, QDomElement& framesetsElement );
    virtual ~Document();

    bool hasParser() const { return m_parser != 0L; }
    bool bodyFound() const { return m_bodyFound; }

    virtual void startBody();
    virtual void endBody();

    virtual void startHeader( wvWare::HeaderData::Type type );
    virtual void endHeader();

    virtual void startFootnote();
    virtual void endFootnote();

    bool parse();

    void processSubDocQueue();

    void finishDocument();

    typedef const wvWare::FunctorBase* FunctorPtr;
    struct SubDocument
    {
        SubDocument( FunctorPtr ptr, int d ) : functorPtr(ptr), data(d) {}
        ~SubDocument() {}
        FunctorPtr functorPtr;
        int data;
    };
    bool hasSubDocument() const;

protected slots:
    // Connected to the KWordTextHandler only when parsing the body
    void slotFirstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> );

    // Add to our parsing queue, for headers, footers, footnotes, text boxes etc.
    // Note that a header functor will parse ALL the header/footers (of the section)
    void slotSubDocFound( const wvWare::FunctorBase* functor, int data );

private:
    void processStyles();
    void processAssociatedStrings();
    void createInitialFrame( QDomElement& parentFramesetElem, int top, int bottom, bool autoExtend );

    QDomDocument& m_mainDocument;
    QDomDocument& m_documentInfo;
    QDomElement& m_framesetsElement;
    KWordReplacementHandler* m_replacementHandler;
    KWordTextHandler* m_textHandler;
    wvWare::SharedPtr<wvWare::Parser> m_parser;
    std::queue<SubDocument> m_subdocQueue;
    unsigned char m_headerFooters; // a mask of HeaderData::Type bits
    bool m_bodyFound;
    int m_footNoteNumber; // number of footnote _framesets_ written out
    int m_endNoteNumber; // number of endnote _framesets_ written out
};

#endif // DOCUMENT_H
