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


class ODFTRAVERSE_EXPORT OdtTraverserBackend
{
 public:
    enum BeginEndTag {
        BeginTag,
        EndTag
    };

    OdtTraverserBackend(OdfTraverserContext *context);
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
