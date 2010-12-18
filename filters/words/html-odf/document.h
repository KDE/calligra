/* This file is part of the KOffice project
   Copyright (C) 2010 Pramod S G <pramod.xyle@gmail.com>

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

#include "generated/leinputstream.h"

#include <QString>
#include <qdom.h>
#include <QObject>
#include <QStringList>
#include <QRectF>
#include <queue>
#include <string>
#include <QList>
#include <QBuffer>
#include <QDomElement>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoStore.h>

class KoStoreDevice;

namespace kword
{
class Parser;
class BRC;
}
class KoFilterChain;
class KWordTableHandler;
class KWordPictureHandler;
class KWordTextHandler;
class KWordGraphicsHandler;

class Document : public QObject
{
    Q_OBJECT
public:
    Document();
    virtual ~Document();

    KWordTextHandler *textHandler() const {
        return m_textHandler;
    }


    bool bodyFound() const {
        return m_bodyFound;
    }

    //virtual void bodyStart();
    virtual void bodyEnd();

    virtual void headersMask(QList<bool> mask);

    virtual void footnoteStart();
    virtual void footnoteEnd();

    virtual void annotationStart();
    virtual void annotationEnd();

    bool parse();

    void processSubDocQueue();

    void finishDocument();

    QString masterPageName(void) const { return m_masterPageName_list.first(); }
    void set_writeMasterPageName(bool val) { m_writeMasterPageName = val; }
    bool writeMasterPageName(void) const { return m_writeMasterPageName; }
    bool omittMasterPage(void) const { return m_omittMasterPage; }
    bool writingHeader(void) const { return m_writingHeader; }
    KoXmlWriter* headerWriter(void) const { return m_headerWriter; }


    bool headersChanged(void) const;

    QString lineNumbersStyleName() const { return m_lineNumbersStyleName; }

private:
    void processStyles();
    void processAssociatedStrings();
    enum NewFrameBehavior { Reconnect = 0, NoFollowup = 1, Copy = 2 };

    KWordTextHandler*        m_textHandler;
    KWordTableHandler*       m_tableHandler;
    KWordGraphicsHandler*    m_graphicsHandler;

    KoFilterChain* m_chain;
    bool m_bodyFound;

    int m_footNoteNumber;
    int m_endNoteNumber;

    // Helpers to generate the various parts of an ODF file.
    KoXmlWriter* m_bodyWriter;      //for writing to the body of content.xml
    KoGenStyles* m_mainStyles;      //for collecting styles
    KoXmlWriter* m_metaWriter;      //for writing to meta.xml
    KoXmlWriter* m_headerWriter;    //for header/footer writing in styles.xml


    int m_headerCount; //to have a unique name for element we're putting into an masterPageStyle
    bool m_writingHeader; //flag for headers/footers, where we write the actual text to styles.xml
    bool m_evenOpen;  //processing an even header/footer
    bool m_firstOpen; //processing a first page header/footer
    QBuffer* m_buffer; //for odd and first page header/footer tags
    QBuffer* m_bufferEven; //for even header/footer tags
    QList<bool> m_hasHeader_list; //does master-page/page-layout require a header element
    QList<bool> m_hasFooter_list; //does master-page/page-layout require a footer element

    //information about section's empty/nonempty header/footer stories
    QList<bool> m_headersMask;

    QStringList m_masterPageName_list; //master-page names
    QStringList m_pageLayoutName_list; //page-layout names
    bool m_writeMasterPageName; //whether to write the master-page name into a paragraph/table
    bool m_omittMasterPage; //whether master-page style for current section has been omitted

    int m_initialFootnoteNumber;
    int m_initialEndnoteNumber;

    QString m_lineNumbersStyleName;

};

#endif // DOCUMENT_H
