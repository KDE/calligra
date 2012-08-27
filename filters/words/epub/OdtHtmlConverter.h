 /* This file is part of the KDE project
   Copyright (C) 2012 Inge Wallin            <inge@lysator.liu.se>
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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

#ifndef ODTHTMLCONVERTER_H
#define ODTHTMLCONVERTER_H

#include <QHash>
#include <QString>
#include <QList>

#include <KoXmlReader.h>
#include <KoFilter.h>

class QByteArray;
class QSizeF;
class QStringList;
class KoXmlWriter;
class KoStore;
class EpubFile;

struct StyleInfo {
    StyleInfo();

    QString family;
    QString parent;
    bool isDefaultStyle;
    bool hasBreakBefore;
    bool inUse;
    QHash<QString, QString> attributes;
};


class OdtHtmlConverter
{
 public:
    OdtHtmlConverter();
    ~OdtHtmlConverter();

    /** Parse all styles in the store, convert them to CSS styles and return info about them.
     *
     * This function opens contents.xml and styles.xml and parses the
     * character and paragraph properties of them. It also records the
     * parent name, wether it has the fo:break-before="page" attribute and
     * wether it is in use in the contents.
     *
     * @param odfStore the store where content.xml and styles.xml can be found.
     * @param styles the out parameter where the styles are returned. This
     * is a QHash with the style internal style name as index (not the
     * printed name) and a StyleInfo pointer as value
     *
     * @return KoFilter::OK if the parsing was successful
     * @return other if the parsing was not successful
     */
 
    KoFilter::ConversionStatus convertStyles(KoStore *odfStore,
                                             QHash<QString, StyleInfo*> &styles);
    KoFilter::ConversionStatus convertContent(KoStore *odfStore, QHash<QString, QString> &metaData,
                                              EpubFile *epub, QHash<QString, StyleInfo*> &styles,
                                              QHash<QString, QSizeF> &imagesSrcList);

 private:

    // Handle a collection of styles from either content.xml or styles.xml
    void collectStyles(KoXmlNode &stylesNode, QHash<QString, StyleInfo*> &styles);
    void collectStyleAttributes(KoXmlElement &propertiesElement, StyleInfo *styleInfo);

    // Helper functions to create the html contents.
    void beginHtmlFile(QHash<QString, QString> &metaData);
    void endHtmlFile();
    void createHtmlHead(KoXmlWriter *writer, QHash<QString, QString> &metaData);


    // All handleTag*() are named after the tag in the ODF that they handle.
    void handleCharacterData(KoXmlNode &node, KoXmlWriter *htmlWriter,
                             QHash<QString, StyleInfo*> &styles);

    void handleTagTable(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                        QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

    void handleTagFrame(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                        QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

    void handleTagP(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                    QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

    void handleTagSpan(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                       QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

    void handleTagPageBreak(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                            QHash<QString, StyleInfo*> &styles);

    void handleTagH(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                    QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

    void handleTagList(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                       QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

    void handleTagA(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                    QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

    void handleTagTab(KoXmlWriter *htmlWriter);
    void handleTagTableOfContent(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                 QHash<QString, StyleInfo*> &styles,
                                 QHash<QString, QSizeF> &imagesSrcList);

    void handleTagTableOfContentBody(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                     QHash<QString, StyleInfo*> &styles,
                                     QHash<QString, QSizeF> &imagesSrcList);

    void handleTagLineBreak(KoXmlWriter *htmlWriter);
    void handleTagBookMark(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagBookMarkStart(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagBookMarkEnd(KoXmlWriter *htmlWriter);


    void handleUnknownTags(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                           QHash<QString, StyleInfo*> &styles,
                           QHash<QString, QSizeF> &imagesSrcList);

    void handleInsideElementsTag(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter,
                                 QHash<QString, StyleInfo*> &styles,
                                 QHash<QString, QSizeF> &imagesSrcList);

    void handleTagNote(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);

    void writeFootNotes(KoXmlWriter *htmlWriter,
                        QHash<QString, StyleInfo*> &styles, QHash<QString, QSizeF> &imagesSrcList);

    void writeEndNotes(KoXmlWriter *htmlWriter, QHash<QString, StyleInfo*> &styles,
                       QHash<QString, QSizeF> &imagesSrcList);

    /** Before start parsing go inside content.xml and collect links id from
     * book-mark-start tag and save its id in hash as key for its value save
     * the current chapter (as we are looking forbook-mark-start tag, we identify
     * page break before too, and we have an id that goes up when i see page break
     * so we now we are in which file or chapter and this id is the value of hash
     * and at the end when we want to write html file, when we see an id, find it in hash
     * and set it instead < a  href = hash.value(key) + #key />
     */
    void collectInternalLinksInfo(KoXmlElement &currentElement,
                                  QHash<QString, StyleInfo*> &styles, int &chapter);

 private:
    // Some variables used while creating the HTML contents.
    QByteArray   m_htmlContent;
    QBuffer     *m_outBuf;
    KoXmlWriter *m_htmlWriter;

    // The number of the current chapter during the conversion.
    int m_currentChapter;

    // Internal links have to be done in a two pass fashion.

    // The first pass just quickly steps through the content and
    // collects the anchors in linksInfo. The second pass is the
    // actual conversion where linksInfo is used to create the
    // links. The reason we have to do it like this is that the
    // contents is split up into chapters and we have to know when we
    // write the href which file (chapter) the anchor is in.
    //
    // The format is QHash<name, filename>
    // where
    //    name      is a unique name for this anchor
    //    filename  is the filename where it occurs. example: "chapter3.xhtml"
    //
    QHash<QString, QString> m_linksInfo;

    // Footnotes are collected while parsing a chapter and written at
    // the end of a chapter. Endnotes are collected while parsing all
    // chapters and are written into its own file at the end of the book.
    //
    // The format is QHash<id, noteElement>
    // where
    //    id          is a unique name for this footnote
    //    noteElement is the KoXmlElement that contains the body of the note
    //
    QHash<QString, KoXmlElement> m_footNotes;
    QHash<QString, KoXmlElement> m_endNotes;
};

#endif // ODTHTMLCONVERTER_H
