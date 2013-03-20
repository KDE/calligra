/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin            <inge@lysator.liu.se>

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

#ifndef ODTTRAVERSERBACKEND_H
#define ODTTRAVERSERBACKEND_H

// Calligra
#include <KoXmlReader.h>
#include <KoFilter.h>

// this library
#include "odftraverse_export.h"
#include "OdtTraverser.h"


class QByteArray;
class QSizeF;
class QStringList;
class KoStore;
class OdfTraverserContext;


/** @brief A default backend for the OdtTraverser class.
 *
 * This class defines an interface and the default behaviour for the
 * backend to the ODT traverser (@see OdtTraverser). When the
 * traverser is called upon to traverse a certain XML tree, there will
 * be two parameters to the root traverse function: a pointer to a
 * backend object and a pointer to a context object.
 *
 * The traverser will traverse the XML tree and for every tag it comes
 * across it will call a specific function in the backend and every
 * call will use the pointer to the context object. 
 *
 * Each supported XML tag has a corresponding callback function. This
 * callback function will be called twice: once when the tag is first
 * encountered anc once when the tag is closed.  This means that an
 * element with no child elements will be called twice in succession.
 *
 * The callback function will also receive as parameter a reference to
 * the XML element in question so that the callback can examine the
 * attributes. The third parameter is an enum indicating whether the
 * element is an open tag or a close tag.
 *
 * This class defines a virtual function for every supported
 * element. It also implements a default behaviour for every element
 * which is to ignore the parameters and do nothing.
 *
 * When this class is used e.g. in a filter it is recommended to
 * inherit this class and only reimplement those functions that are
 * actually needed.
 */
class ODFTRAVERSE_EXPORT OdtTraverserBackend
{
 public:
    /// Tells whether a callback function is called on an open tag or a close tag.
    enum BeginEndTag {
        BeginTag,
        EndTag
    };

    explicit OdtTraverserBackend(OdfTraverserContext *context);
    virtual ~OdtTraverserBackend();

    // Called before and after the actual traversal.
    virtual void beginEndTraversal(OdfTraverserContext *context, BeginEndTag beginEnd);

    // Called when there is a document level element with children.
    virtual void insideElementsTag(KoXmlElement &element,
                                   OdfTraverserContext *context, BeginEndTag beginEnd);

    // Text tags
    virtual void tagP(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);
    virtual void tagH(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);
    virtual void tagSpan(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);
    virtual void tagS(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);
    virtual void tagTab(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);
    virtual void tagLineBreak(KoXmlElement &element, OdfTraverserContext *context,
                              BeginEndTag beginEnd);
    virtual void tagPageBreak(KoXmlElement &element, OdfTraverserContext *context,
                              BeginEndTag beginEnd);
    virtual void tagA(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);
    virtual void characterData(KoXmlNode &node, OdfTraverserContext *context,
                               BeginEndTag beginEnd);

    virtual void tagList(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);

    virtual void tagTable(KoXmlElement &element, OdfTraverserContext *context,
                          BeginEndTag beginEnd);
    virtual void tagTableRow(KoXmlElement &element, OdfTraverserContext *context,
                             OdtTraverser::TableCellType type,
                             BeginEndTag beginEnd);

    virtual void tagFrame(KoXmlElement &element, OdfTraverserContext *context,
                          BeginEndTag beginEnd);
    virtual void beginEmbeddedFormula(const QString &href, OdfTraverserContext *context,
                                      BeginEndTag beginEnd);

    virtual void tagTableOfContent(KoXmlElement &element, OdfTraverserContext *context,
                                   BeginEndTag beginEnd);
    virtual void tagTableOfContentBody(KoXmlElement &element, OdfTraverserContext *context,
                                       BeginEndTag beginEnd);

    virtual void tagBookMark(KoXmlElement &element, OdfTraverserContext *context,
                             BeginEndTag beginEnd);
    virtual void tagBookMarkStart(KoXmlElement &element, OdfTraverserContext *context,
                                  BeginEndTag beginEnd);
    virtual void tagBookMarkEnd(KoXmlElement &element, OdfTraverserContext *context,
                                BeginEndTag beginEnd);

    virtual void tagNote(KoXmlElement &element, OdfTraverserContext *context,
                         BeginEndTag beginEnd);

    virtual void unknownTags(KoXmlElement &element, OdfTraverserContext *context,
                             BeginEndTag beginEnd);

 private:
    class Private;
    Private * const d;
};


#endif // ODTTRAVERSERBACKEND_H
