/* This file is part of the KDE project

   Copyright (C) 2012-2013 Inge Wallin            <inge@lysator.liu.se>
   Copyright (C) 2012      Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef ODTTRAVERSER_H
#define ODTTRAVERSER_H

// Qt
#include <QHash>
#include <QString>

// Calligra
#include <KoXmlReader.h>

// this library
#include "odftraverse_export.h"


class QSizeF;

class KoXmlWriter;
class KoStore;

class OdtTraverserBackend;
class OdfTraverserContext;


/** @brief Traverse the XML tree of the content of an ODT file.
 *
 * The OdtTraverser is used to traverse the contents of an ODT file.
 * For every XML element that it comes across it will call a specific
 * function in a backend class: @see OdtTraverserBackend. 
 *
 * Before the traversing process is started the ODT file will be
 * analyzed to collect some data that may be needed during the
 * traversal: metadata, manifest and styles (note: styles is not yet
 * implemented). This data is stored in the so called context, which
 * is kept in an instance of the OdfTraverserContext class.
 *
 * The context will be passed around to the backend in every call to a
 * backend callback function.
 *
 * In addition to the pre-analyzed data from the ODT file, the context
 * can be used to keep track of data that is used in the backend
 * processing such as internal links, lists of embedded data such as
 * pictures.
 */
class ODFTRAVERSE_EXPORT OdtTraverser
{
 public:
    OdtTraverser();
    ~OdtTraverser();

    bool traverseContent(OdtTraverserBackend *backend,
                         OdfTraverserContext *context);

    // Used when traversing table cells in headers and normal tables respectively.
    enum TableCellType {
        TableDataType,
        TableHeaderType,
    };

 protected:
    // All handleTag*() are named after the tag in the ODF that they handle.

    // Generic stuff
    //
    // FIXME: Should be called handleGeneralTextContent()
    //
    // FIXME: There should be one general handler for block level and
    //        one for paragraph level.
    void handleInsideElementsTag(KoXmlElement &element);

    // Basic text stuff.
    void handleTagP(KoXmlElement &element);
    void handleTagH(KoXmlElement &element);
    void handleTagSpan(KoXmlElement &element);
    void handleTagS(KoXmlElement &element);
    void handleTagTab(KoXmlElement &element);
    void handleTagLineBreak(KoXmlElement &element);
    void handleTagPageBreak(KoXmlElement &element);
    void handleTagA(KoXmlElement &element);
    void handleCharacterData(KoXmlNode &node);

    // Slightly more advanced text stuff.
    void handleTagList(KoXmlElement &element);
    void handleTagTable(KoXmlElement &element);
    void handleTagTableRow(KoXmlElement &element, TableCellType type = TableDataType);

    // Frames
    void handleTagFrame(KoXmlElement &element);
    void handleEmbeddedFormula(const QString &href);
    void copyXmlElement(const KoXmlElement &el, KoXmlWriter &writer,
                        QHash<QString, QString> &unknownNamespaces);

    // Other block level text tags
    void handleTagTableOfContent(KoXmlElement &element);
    void handleTagTableOfContentBody(KoXmlElement &element);
//--
    void handleTagBookMark(KoXmlElement &element);
    void handleTagBookMarkStart(KoXmlElement &element);
    void handleTagBookMarkEnd(KoXmlElement &element);

    void handleTagNote(KoXmlElement &element);// footnotes and endnotes

    // FIXME: text:annotation

    // Called for all tags that are not explicitly recognized. 
    void handleUnknownTags(KoXmlElement &element);

 private:
    OdtTraverserBackend  *m_backend;
    OdfTraverserContext  *m_context;

#if 0  // This data is now store in the context.
       // FIXME: Remove when we have fixed all the handleTag*() functions.

    // A list of images and their sizes. This list is collected during
    // the conversion and returned from traverseContent() using an
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

    // Format: QHash< QString id, QString video source>
    QHash<QString, QString> m_mediaFilesList;
    int m_mediaId;
#endif
};

#endif // ODTTRAVERSER_H
