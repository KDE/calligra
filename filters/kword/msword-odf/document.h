/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "tablehandler.h"

#include <wv2/src/handlers.h>
#include <wv2/src/functor.h>

#include <QString>
#include <qdom.h>
#include <QObject>
#include <QStringList>
#include <QRectF>
#include <queue>
#include <string>
#include <QBuffer>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoStore.h>

class KoStoreDevice;

namespace wvWare
{
class Parser;
namespace Word97 {
class BRC;
}
}
class KoFilterChain;
class KWordReplacementHandler;
class KWordTableHandler;
class KWordPictureHandler;
class KWordTextHandler;

class Document : public QObject, public wvWare::SubDocumentHandler
{
    Q_OBJECT
public:
    Document(const std::string& fileName, KoFilterChain* chain, KoXmlWriter* bodyWriter,
             KoGenStyles* mainStyles, KoXmlWriter* metaWriter, KoStore* store, KoXmlWriter* manifestWriter);
    virtual ~Document();

    KWordTextHandler *textHandler() const {
        return m_textHandler;
    }

    bool hasParser() const {
        return m_parser != 0L;
    }
    bool bodyFound() const {
        return m_bodyFound;
    }

    virtual void bodyStart();
    virtual void bodyEnd();

    virtual void headerStart(wvWare::HeaderData::Type type);
    virtual void headerEnd();

    virtual void footnoteStart();
    virtual void footnoteEnd();

    bool parse();

    void processSubDocQueue();

    void finishDocument();

    typedef const wvWare::FunctorBase* FunctorPtr;
    struct SubDocument {
        SubDocument(FunctorPtr ptr, int d, const QString& n, const QString& extra)
                : functorPtr(ptr), data(d), name(n), extraName(extra) {}
        ~SubDocument() {}
        FunctorPtr functorPtr;
        int data;
        QString name;
        QString extraName;
    };


public slots:
    // Connected to the KWordTextHandler only when parsing the body
    void slotSectionFound(wvWare::SharedPtr<const wvWare::Word97::SEP>);

    void slotSectionEnd(wvWare::SharedPtr<const wvWare::Word97::SEP>);

    // Add to our parsing queue, for headers, footers, footnotes, text boxes etc.
    // Note that a header functor will parse ALL the header/footers (of the section)
    void slotSubDocFound(const wvWare::FunctorBase* functor, int data);

    void slotFootnoteFound(const wvWare::FunctorBase* functor, int data);

    void slotHeadersFound(const wvWare::FunctorBase* functor, int data);

    void slotTableFound(KWord::Table* table);

    // Write out the frameset and add the key to the PICTURES tag
    void slotPictureFound(const QString& frameName, const QString& pictureName, KoXmlWriter* writer,
                          const wvWare::FunctorBase*);

    // Similar to footnoteStart/footnoteEnd but for cells.
    // This is connected to KWordTableHandler
    //void slotTableCellStart( int row, int column, int rowSize, int columnSize, const QRectF& cellRect, const QString& tableName, const wvWare::Word97::BRC& brcTop, const wvWare::Word97::BRC& brcBottom, const wvWare::Word97::BRC& brcLeft, const wvWare::Word97::BRC& brcRight, const wvWare::Word97::SHD& shd );
    //void slotTableCellEnd();

private:
    void processStyles();
    void processAssociatedStrings();
    enum NewFrameBehavior { Reconnect = 0, NoFollowup = 1, Copy = 2 };
    void generateFrameBorder(QDomElement& frameElementOut, const wvWare::Word97::BRC& brcTop, const wvWare::Word97::BRC& brcBottom, const wvWare::Word97::BRC& brcLeft, const wvWare::Word97::BRC& brcRight, const wvWare::Word97::SHD& shd);

    // Handlers for different data types in the document.
    KWordTextHandler*        m_textHandler;
    KWordTableHandler*       m_tableHandler;
    KWordReplacementHandler* m_replacementHandler;
    KWordPictureHandler*     m_pictureHandler;

    KoFilterChain* m_chain;
    wvWare::SharedPtr<wvWare::Parser> m_parser;
    std::queue<SubDocument> m_subdocQueue;
    std::queue<KWord::Table> m_tableQueue;
    QStringList m_pictureList; // for <PICTURES>
    unsigned char m_headerFooters; // a mask of HeaderData::Type bits
    bool m_bodyFound;
    bool m_evenOpen; //we're processing an even header or footer
    bool m_oddOpen; //we're processing an odd header or footer
    int m_footNoteNumber; // number of footnote _framesets_ written out
    int m_endNoteNumber; // number of endnote _framesets_ written out

    // Helpers to generate the various parts of an ODF file.
    KoXmlWriter* m_bodyWriter;      //for writing to the body of content.xml
    KoGenStyles* m_mainStyles;      //for collecting styles
    KoXmlWriter* m_metaWriter;      //for writing to meta.xml
    KoGenStyle*  m_masterStyle;     //for header/footer stuff, at least
    KoGenStyle*  m_pageLayoutStyle; //page layout style
    KoXmlWriter* m_writer;          //for header/footer tags

    bool m_hasHeader;
    bool m_hasFooter;
    QBuffer* m_buffer; //for header/footer tags
    QBuffer* m_bufferEven; //for even header/footer tags
    int m_headerCount; //just so we have a unique name for the element we're putting in m_masterStyle
    QString m_masterStyleName; //need to know what the master style name is so we can write it
};

#endif // DOCUMENT_H
