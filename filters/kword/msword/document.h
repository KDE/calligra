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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "tablehandler.h"

#include <wv2/handlers.h>
#include <wv2/functor.h>

#include <qstring.h>
#include <qdom.h>
#include <qobject.h>
#include <qstringlist.h>

#include <queue>
#include <string>

class KoStoreDevice;
class KoSize;

namespace wvWare {
    class Parser;
    namespace Word97 {
        class BRC;
    }
}
class KoRect;
class KoFilterChain;
class KWordReplacementHandler;
class KWordTableHandler;
class KWordPictureHandler;
class KWordTextHandler;

class Document : public QObject, public wvWare::SubDocumentHandler
{
    Q_OBJECT
public:
    Document( const std::string& fileName, QDomDocument& mainDocument, QDomDocument &documentInfo, QDomElement& framesetsElement, KoFilterChain* chain );
    virtual ~Document();

    bool hasParser() const { return m_parser != 0L; }
    bool bodyFound() const { return m_bodyFound; }

    virtual void bodyStart();
    virtual void bodyEnd();

    virtual void headerStart( wvWare::HeaderData::Type type );
    virtual void headerEnd();

    virtual void footnoteStart();
    virtual void footnoteEnd();

    bool parse();

    void processSubDocQueue();

    void finishDocument();

    typedef const wvWare::FunctorBase* FunctorPtr;
    struct SubDocument
    {
        SubDocument( FunctorPtr ptr, int d, const QString& n, const QString& extra )
            : functorPtr(ptr), data(d), name(n), extraName(extra) {}
        ~SubDocument() {}
        FunctorPtr functorPtr;
        int data;
        QString name;
        QString extraName;
    };

    // Called by PictureHandler
    KoStoreDevice* createPictureFrameSet( const KoSize& size );

protected slots:
    // Connected to the KWordTextHandler only when parsing the body
    void slotFirstSectionFound( wvWare::SharedPtr<const wvWare::Word97::SEP> );

    // Add to our parsing queue, for headers, footers, footnotes, text boxes etc.
    // Note that a header functor will parse ALL the header/footers (of the section)
    void slotSubDocFound( const wvWare::FunctorBase* functor, int data );

    // Add to our parsing queue, for tables
    void slotTableFound( const KWord::Table& table );

    // Write out the frameset and add the key to the PICTURES tag
    void slotPictureFound( const QString& frameName, const QString& pictureName, const wvWare::FunctorBase* );

    // Similar to footnoteStart/footnoteEnd but for cells.
    // This is connected to KWordTableHandler
    void slotTableCellStart( int row, int column, int rowSize, int columnSize, const KoRect& cellRect, const QString& tableName, const wvWare::Word97::BRC& brcTop, const wvWare::Word97::BRC& brcBottom, const wvWare::Word97::BRC& brcLeft, const wvWare::Word97::BRC& brcRight, const wvWare::Word97::SHD& shd );
    void slotTableCellEnd();

private:
    void processStyles();
    void processAssociatedStrings();
    enum NewFrameBehavior { Reconnect=0, NoFollowup=1, Copy=2 };
    QDomElement createInitialFrame( QDomElement& parentFramesetElem, double left, double right, double top, double bottom, bool autoExtend, NewFrameBehavior nfb );
    void generateFrameBorder( QDomElement& frameElementOut, const wvWare::Word97::BRC& brcTop, const wvWare::Word97::BRC& brcBottom, const wvWare::Word97::BRC& brcLeft, const wvWare::Word97::BRC& brcRight, const wvWare::Word97::SHD& shd );

    QDomDocument& m_mainDocument;
    QDomDocument& m_documentInfo;
    QDomElement& m_framesetsElement;
    KWordReplacementHandler* m_replacementHandler;
    KWordTableHandler* m_tableHandler;
    KWordPictureHandler* m_pictureHandler;
    KWordTextHandler* m_textHandler;
    KoFilterChain* m_chain;
    wvWare::SharedPtr<wvWare::Parser> m_parser;
    std::queue<SubDocument> m_subdocQueue;
    std::queue<KWord::Table> m_tableQueue;
    QStringList m_pictureList; // for <PICTURES>
    unsigned char m_headerFooters; // a mask of HeaderData::Type bits
    bool m_bodyFound;
    int m_footNoteNumber; // number of footnote _framesets_ written out
    int m_endNoteNumber; // number of endnote _framesets_ written out
};

#endif // DOCUMENT_H
