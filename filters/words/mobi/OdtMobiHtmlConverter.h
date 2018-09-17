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

#ifndef ODTMOBIHTMLCONVERTER_H
#define ODTMOBIHTMLCONVERTER_H

#include <QHash>
#include <QString>
#include <QList>

#include <KoXmlReader.h>
#include <KoFilter.h>

class QByteArray;
class QSizeF;
class KoXmlWriter;
class KoStore;
class FileCollector;


struct StyleInfo {
    StyleInfo();

    QString family;
    QString parent;
    bool isDefaultStyle;

    int  defaultOutlineLevel;
    bool shouldBreakChapter;
    bool inUse;

    QHash<QString, QString> attributes;
};


class OdtMobiHtmlConverter
{
 public:
    struct ConversionOptions {
        bool  stylesInCssFile;  // True if the css should go into a separate file
        bool  doBreakIntoChapters; // True if the output should be broken into chapters.
        bool  useMobiConventions;  // True if Mobi is using the convention.
                                   // to handle img tag and for handle indention
                                   // and do not write meta and link tag in html head.
    };

    OdtMobiHtmlConverter();
    ~OdtMobiHtmlConverter();

    KoFilter::ConversionStatus convertContent(KoStore *odfStore, QHash<QString, QString> &metaData,
                                              ConversionOptions *options,
                                              FileCollector *collector,
                                              // Out parameters:
                                              QHash<QString, QSizeF> &images);

 private:

    // Helper functions to create the html contents.
    void beginHtmlFile(QHash<QString, QString> &metaData);
    void endHtmlFile();
    void createHtmlHead(KoXmlWriter *writer, QHash<QString, QString> &metaData);


    // All handleTag*() are named after the tag in the ODF that they handle.
    void handleInsideElementsTag(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagP(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagH(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagSpan(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleCharacterData(KoXmlNode &node, KoXmlWriter *htmlWriter);

    void handleTagTable(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);

    void handleTagA(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);

    void handleTagPageBreak(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagList(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);

    void handleTagFrame(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);

    void handleTagTab(KoXmlWriter *htmlWriter);
    void handleTagTableOfContent(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagTableOfContentBody(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);

    void handleTagLineBreak(KoXmlWriter *htmlWriter);
    void handleTagBookMark(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagBookMarkStart(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagBookMarkEnd(KoXmlWriter *htmlWriter);

    void handleUnknownTags(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);
    void handleTagNote(KoXmlElement &nodeElement, KoXmlWriter *htmlWriter);

    void collectInternalLinksInfo(KoXmlElement &currentElement, int &chapter);

    void writeFootNotes(KoXmlWriter *htmlWriter);
    void writeEndNotes(KoXmlWriter *htmlWriter);

    // Elements like font for color or element span for bgcolor, element u for underline, b, i
    void openFontOptionsElement(KoXmlWriter *htmlWriter, StyleInfo *styleInfo);
    void closeFontOptionsElement(KoXmlWriter *htmlWriter);
    //

    KoFilter::ConversionStatus collectStyles(KoStore *odfStore, QHash<QString, StyleInfo*> &styles);
    void collectStyleSet(KoXmlNode &stylesNode, QHash<QString, StyleInfo*> &styles);
    void collectStyleAttributes(KoXmlElement &propertiesElement, StyleInfo *styleInfo);

    void fixStyleTree(QHash<QString, StyleInfo*> &styles);

    KoFilter::ConversionStatus createCSS(QHash<QString, StyleInfo*> &styles,
                                         QByteArray &cssContent);
    void flattenStyles(QHash<QString, StyleInfo*> &styles);
    void flattenStyle(const QString &styleName, QHash<QString, StyleInfo*> &styles,
                      QSet<QString> &doneStyles);

    // Write boomarks position in its reference.
    void generateMobiInternalLinks();

 private:
    FileCollector *m_collector;

    // Some variables used while creating the HTML contents.
    QByteArray   m_cssContent;
    QByteArray   m_htmlContent;
    QBuffer     *m_outBuf;
    KoXmlWriter *m_htmlWriter;

    // Options for the conversion process
    // FIXME: This should go into an Options struct together with some
    //        others from FileConversion.h.
    const ConversionOptions *m_options;

    QHash<QString, StyleInfo*> m_styles;

    // The number of the current chapter during the conversion.
    int m_currentChapter;

    // A list of images and their sizes. This list is collected during
    // the conversion and returned from convertContent() using an
    // outparameter.
    //
    // The format is QHash<name, size>
    // where
    //    name   is the name of the picture inside the ODT file
    //    size   is the size in points.
    //
    QHash<QString, QSizeF>  m_images;

    // Internal links have to be done in a two pass fashion.
    //
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

    // specific values for mobi.

    // The format is QHash<QString source, int index>
    // In mobi we save images with index and as we can have repeated
    // images we need a hash.
    // e.g img tag format in mobi <img recindex="0004">
    QHash<QString, int> m_imagesIndex;

    int m_imgIndex;

    // In mobi we do not need indentation.
    bool m_doIndent;

    bool m_optionsTag;
    bool m_boldTag;
    bool m_underlineTag;
    bool m_italicTag;
    bool m_spanTag;
    bool m_fontColorTag;
    //

    // The format is QHash <reference position in file, reference id>
    QMap <qint64, QString> m_refrencesList;
    // The format is QHash <bookmark poistion in file, bookmark id>
    QMap <QString, qint64> m_bookMarksList;

};

#endif // ODTMOBIHTMLCONVERTER_H
