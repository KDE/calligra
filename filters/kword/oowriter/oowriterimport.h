/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2003 David Faure <faure@kde.org>

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

#ifndef OoWriter_IMPORT_H__
#define OoWriter_IMPORT_H__

#include <KoFilter.h>

#include <q3dict.h>
#include <qdom.h>
//Added by qt3to4:
#include <QByteArray>
#include <QVector>
#include <KoStyleStack.h>
#include <KoXmlReader.h>
#include <liststylestack.h>

class KZip;

class OoWriterImport : public KoFilter
{
    Q_OBJECT
public:
    OoWriterImport(QObject * parent, const QStringList &);
    virtual ~OoWriterImport();

    virtual KoFilter::ConversionStatus convert(QByteArray const & from, QByteArray const & to);

private:
    void prepareDocument(QDomDocument& mainDocument, QDomElement& framesetsElem);
    void finishDocumentContent(QDomDocument& mainDocument);
    void writePageLayout(QDomDocument& mainDocument, const QString& masterPageName);
    void parseList(QDomDocument& doc, const KoXmlElement& list, QDomElement& currentFramesetElement);
    bool pushListLevelStyle(const QString& listStyleName, int level);
    bool pushListLevelStyle(const QString& listStyleName, KoXmlElement& fullListStyle, int level);
    void applyListStyle(QDomDocument& doc, QDomElement& layoutElement, const KoXmlElement& paragraph);
    void writeCounter(QDomDocument& doc, QDomElement& layoutElement, bool heading, int level, bool ordered);
    QDomElement parseParagraph(QDomDocument& doc, const KoXmlElement& paragraph);
    void parseSpanOrSimilar(QDomDocument& doc, const KoXmlElement& parent, QDomElement& kwordParagraph, QDomElement& kwordFormats, QString& paragraphText, uint& pos);
    // Reads from m_styleStack, writes the text properties to parentElement
    void writeFormat(QDomDocument& doc, QDomElement& parentElement, int id, int pos, int length);
    // Reads from m_styleStack, writes the paragraph properties to layoutElement
    void writeLayout(QDomDocument& doc, QDomElement& layoutElement);
    enum NewFrameBehavior { Reconnect = 0, NoFollowup = 1, Copy = 2 };
    QDomElement createInitialFrame(QDomElement& parentFramesetElem, double left, double right, double top, double bottom, bool autoExtend, NewFrameBehavior nfb);
    void createStyles(QDomDocument &doc);
    void createDocumentInfo(QDomDocument &docinfo);
    void createDocumentContent(QDomDocument &doccontent, QDomElement& mainFramesetElement);
    void parseBodyOrSimilar(QDomDocument &doc, const KoXmlElement& parent, QDomElement& currentFramesetElement);
    KoFilter::ConversionStatus loadAndParse(const QString& filename, KoXmlDocument& doc);
    KoFilter::ConversionStatus openFile();
    bool createStyleMap(const KoXmlDocument & styles, QDomDocument& doc);
    void insertStyles(const KoXmlElement& element, QDomDocument& doc);
    void importDateTimeStyle(const KoXmlElement& parent);
    void fillStyleStack(const KoXmlElement& object, const char* nsURI, const QString& attrName);
    void addStyles(const KoXmlElement* style);
    void importFootnotesConfiguration(QDomDocument& doc, const KoXmlElement& elem, bool endnote);
    void importFootnote(QDomDocument& doc, const KoXmlElement& object, QDomElement& formats, uint pos, const QString& tagName);
    QString appendPicture(QDomDocument& doc, const KoXmlElement& object);
    QString appendTextBox(QDomDocument& doc, const KoXmlElement& object);
    void appendTOC(QDomDocument& doc, const KoXmlElement& toc);
    void importFrame(QDomElement& frameElementOut, const KoXmlElement& object, bool isText);
    void importCommonFrameProperties(QDomElement& frameElementOut);
    void importHeaderFooter(QDomDocument& doc, const KoXmlElement& headerFooter, bool isHeader, KoXmlElement& style);
    void anchorFrameset(QDomDocument& doc, QDomElement& formats, uint pos, const QString& frameName);
    void appendField(QDomDocument& doc, QDomElement& outputFormats, const KoXmlElement& object, uint pos);
    void appendKWordVariable(QDomDocument& doc, QDomElement& formats, const KoXmlElement& object, uint pos,
                             const QString& key, int type, QDomElement& child);
    void appendBookmark(QDomDocument& doc, int paragId, int pos, const QString& name);
    void appendBookmark(QDomDocument& doc, int paragId, int pos, int endParagId, int endPos, const QString& name);
    void parseTable(QDomDocument &doc, const KoXmlElement& parent, QDomElement& currentFramesetElement);
    void parseInsideOfTable(QDomDocument &doc, const KoXmlElement& parent, QDomElement& currentFramesetElement,
                            const QString& tableName, const QVector<double> & columnLefts, uint& row, uint& column);
    static QString kWordStyleName(const QString& ooStyleName);

    KoXmlDocument   m_content;
    KoXmlDocument   m_meta;
    KoXmlDocument   m_settings;
    KoXmlDocument   m_stylesDoc;

    Q3Dict<KoXmlElement>   m_styles;
    Q3Dict<KoXmlElement>   m_masterPages;
    Q3Dict<KoXmlElement>   m_listStyles;

    KoStyleStack m_styleStack;
    KoXmlElement m_defaultStyle;
    ListStyleStack m_listStyleStack;
    KoXmlElement m_outlineStyle;
    bool m_insideOrderedList;
    bool m_nextItemIsListItem; // only the first elem inside list-item is numbered
    bool m_hasTOC;
    bool m_hasHeader;
    bool m_hasFooter;
    int m_restartNumbering;
    QString m_currentListStyleName;
    QString m_currentMasterPage;
    QDomElement m_currentFrameset; // set by parseBodyOrSimilar

    struct BookmarkStart {
        BookmarkStart() {} // for stupid QValueList
        BookmarkStart(const QString&s, int par, int ind)
                : frameSetName(s), paragId(par), pos(ind) {}
        QString frameSetName;
        int paragId;
        int pos;
    };
    typedef QMap<QString, BookmarkStart> BookmarkStartsMap;
    BookmarkStartsMap m_bookmarkStarts;

    typedef QMap<QString, QString> DataFormatsMap;
    DataFormatsMap m_dateTimeFormats; // maybe generalize to include number formats.

    uint m_pictureNumber; // Number of the picture (increment *before* use)
    KZip* m_zip; // Input KZip file
};

#endif

