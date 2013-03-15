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
    OdtTraverserBackend(OdfTraverserContext *context);
    virtual ~OdtTraverserBackend();

    // Called before and after the actual traversal.
    virtual void beginTraversal(OdfTraverserContext *context);
    virtual void endTraversal(OdfTraverserContext *context);

    // Called when there is a document level element with children.
    virtual void beginInsideElementsTag(KoXmlElement &element,
                                        OdfTraverserContext *context);
    virtual void endInsideElementsTag(KoXmlElement &element,
                                      OdfTraverserContext *context);

    // Text tags
    virtual void beginTagP(KoXmlElement &element,
                           OdfTraverserContext *context);
    virtual void endTagP(KoXmlElement &element,
                         OdfTraverserContext *context);
    virtual void beginTagH(KoXmlElement &element,
                           OdfTraverserContext *context);
    virtual void endTagH(KoXmlElement &element,
                         OdfTraverserContext *context);
    virtual void beginTagSpan(KoXmlElement &element,
                              OdfTraverserContext *context);
    virtual void endTagSpan(KoXmlElement &element,
                            OdfTraverserContext *context);
    virtual void beginTagS(KoXmlElement &element,
                           OdfTraverserContext *context);
    virtual void endTagS(KoXmlElement &element,
                         OdfTraverserContext *context);
    virtual void beginTagTab(KoXmlElement &element,
                             OdfTraverserContext *context);
    virtual void endTagTab(KoXmlElement &element,
                           OdfTraverserContext *context);
    virtual void beginTagLineBreak(KoXmlElement &element,
                                   OdfTraverserContext *context);
    virtual void endTagLineBreak(KoXmlElement &element,
                                 OdfTraverserContext *context);
    virtual void beginTagPageBreak(KoXmlElement &element,
                                   OdfTraverserContext *context);
    virtual void endTagPageBreak(KoXmlElement &element,
                                 OdfTraverserContext *context);
    virtual void beginTagA(KoXmlElement &element,
                           OdfTraverserContext *context);
    virtual void endTagA(KoXmlElement &element,
                         OdfTraverserContext *context);
    virtual void beginCharacterData(KoXmlNode &node,
                                    OdfTraverserContext *context);
    virtual void endCharacterData(KoXmlNode &node,
                                  OdfTraverserContext *context);

    virtual void beginTagList(KoXmlElement &element,
                              OdfTraverserContext *context);
    virtual void endTagList(KoXmlElement &element,
                            OdfTraverserContext *context);

    virtual void beginTagTable(KoXmlElement &element,
                               OdfTraverserContext *context);
    virtual void endTagTable(KoXmlElement &element,
                             OdfTraverserContext *context);
    virtual void beginTagTableRow(KoXmlElement &element,
                                   OdfTraverserContext *context,
                                   OdtTraverser::TableCellType type = OdtTraverser::TableDataType);
    virtual void endTagTableRow(KoXmlElement &element,
                                OdfTraverserContext *context,
                                OdtTraverser::TableCellType type = OdtTraverser::TableDataType);

    virtual void beginTagFrame(KoXmlElement &element,
                               OdfTraverserContext *context);
    virtual void endTagFrame(KoXmlElement &element,
                             OdfTraverserContext *context);
    virtual void beginEmbeddedFormula(const QString &href,
                                      OdfTraverserContext *context);
    virtual void endEmbeddedFormula(const QString &href,
                                    OdfTraverserContext *context);

    virtual void beginTagTableOfContent(KoXmlElement &element,
                                        OdfTraverserContext *context);
    virtual void endTagTableOfContent(KoXmlElement &element,
                                      OdfTraverserContext *context);
    virtual void beginTagTableOfContentBody(KoXmlElement &element,
                                            OdfTraverserContext *context);
    virtual void endTagTableOfContentBody(KoXmlElement &element,
                                          OdfTraverserContext *context);
//--

    virtual void beginTagBookMark(KoXmlElement &element,
                                  OdfTraverserContext *context);
    virtual void endTagBookMark(KoXmlElement &element,
                                OdfTraverserContext *context);
    virtual void beginTagBookMarkStart(KoXmlElement &element,
                                       OdfTraverserContext *context);
    virtual void endTagBookMarkStart(KoXmlElement &element,
                                     OdfTraverserContext *context);
    virtual void beginTagBookMarkEnd(KoXmlElement &element,
                                     OdfTraverserContext *context);
    virtual void endTagBookMarkEnd(KoXmlElement &element,
                                   OdfTraverserContext *context);

    virtual void beginUnknownTags(KoXmlElement &element,
                                  OdfTraverserContext *context);
    virtual void endUnknownTags(KoXmlElement &element,
                                OdfTraverserContext *context);
    virtual void beginTagNote(KoXmlElement &element,
                              OdfTraverserContext *context);
    virtual void endTagNote(KoXmlElement &element,
                            OdfTraverserContext *context);

 private:
    class Private;
    Private * const d;
};


#endif // ODTTRAVERSERBACKEND_H
