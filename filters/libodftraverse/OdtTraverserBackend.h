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
 * Boston, MA 02110-1301, USA.
*/

#ifndef ODTTRAVERSERBACKEND_H
#define ODTTRAVERSERBACKEND_H

// Calligra
#include <KoXmlReader.h>

class QByteArray;
class QSizeF;
class QStringList;
class KoStore;

class OdtTraverserBackend;


// FIXME: Should be moved to the traverser .h file?  Or a common one?
enum TableCellType {
    TableDataType,
    TableHeaderType,
};

// A base class for anything you want to send into the backend. 
class OdtTraverserContext {
};


class OdtTraverserBackend
{
 public:
    OdtTraverserBackend(OdtTraverserContext *context);
    virtual ~OdtTraverserBackend();

    virtual void beginTraversal(OdtTraverserContext *context);
    virtual void endTraversal(OdtTraverserContext *context);

    virtual void beginInsideElementsTag(KoXmlElement &element,
                                        OdtTraverserContext *context);
    virtual void endInsideElementsTag(KoXmlElement &element,
                                      OdtTraverserContext *context);
    virtual void beginTagP(KoXmlElement &element,
                           OdtTraverserContext *context);
    virtual void endTagP(KoXmlElement &element,
                         OdtTraverserContext *context);
    virtual void beginTagH(KoXmlElement &element,
                           OdtTraverserContext *context);
    virtual void endTagH(KoXmlElement &element,
                         OdtTraverserContext *context);
    virtual void beginTagSpan(KoXmlElement &element,
                              OdtTraverserContext *context);
    virtual void endTagSpan(KoXmlElement &element,
                            OdtTraverserContext *context);
    virtual void beginTagS(KoXmlElement &element,
                           OdtTraverserContext *context);
    virtual void endTagS(KoXmlElement &element,
                         OdtTraverserContext *context);
    virtual void beginCharacterData(KoXmlNode &node,
                                    OdtTraverserContext *context);
    virtual void endCharacterData(KoXmlNode &node,
                                  OdtTraverserContext *context);

    virtual void beginTagTable(KoXmlElement &element,
                               OdtTraverserContext *context);
    virtual void endTagTable(KoXmlElement &element,
                             OdtTraverserContext *context);
    virtual void beginTagTableRow(KoXmlElement &element,
                                   OdtTraverserContext *context,
                                   TableCellType type = TableDataType);
    virtual void endTagTableRow(KoXmlElement &element,
                                OdtTraverserContext *context);

    virtual void beginTagA(KoXmlElement &element,
                           OdtTraverserContext *context);
    virtual void endTagA(KoXmlElement &element,
                         OdtTraverserContext *context);

    virtual void beginTagPageBreak(KoXmlElement &element,
                                   OdtTraverserContext *context);
    virtual void endTagPageBreak(KoXmlElement &element,
                                 OdtTraverserContext *context);
    virtual void beginTagList(KoXmlElement &element,
                              OdtTraverserContext *context);
    virtual void endTagList(KoXmlElement &element,
                            OdtTraverserContext *context);

    virtual void beginTagFrame(KoXmlElement &element,
                               OdtTraverserContext *context);
    virtual void endTagFrame(KoXmlElement &element,
                             OdtTraverserContext *context);
    virtual void beginEmbeddedFormula(const QString &href,
                                      OdtTraverserContext *context);
    virtual void endEmbeddedFormula(const QString &href,
                                    OdtTraverserContext *context);

    virtual void beginTagTab(KoXmlElement &element,
                             OdtTraverserContext *context);
    virtual void endTagTab(KoXmlElement &element,
                           OdtTraverserContext *context);
    virtual void beginTagTableOfContent(KoXmlElement &element,
                                        OdtTraverserContext *context);
    virtual void endTagTableOfContent(KoXmlElement &element,
                                      OdtTraverserContext *context);
    virtual void beginTagTableOfContentBody(KoXmlElement &element,
                                            OdtTraverserContext *context);
    virtual void endTagTableOfContentBody(KoXmlElement &element,
                                          OdtTraverserContext *context);

    virtual void beginTagLineBreak(OdtTraverserContext *context);
    virtual void endTagLineBreak(OdtTraverserContext *context);
    virtual void beginTagBookMark(KoXmlElement &element,
                                  OdtTraverserContext *context);
    virtual void endTagBookMark(KoXmlElement &element,
                                OdtTraverserContext *context);
    virtual void beginTagBookMarkStart(KoXmlElement &element,
                                       OdtTraverserContext *context);
    virtual void endTagBookMarkStart(KoXmlElement &element,
                                     OdtTraverserContext *context);
    virtual void beginTagBookMarkEnd(KoXmlElement &element,
                                     OdtTraverserContext *context);
    virtual void endTagBookMarkEnd(KoXmlElement &element,
                                   OdtTraverserContext *context);

    virtual void beginUnknownTags(KoXmlElement &element,
                                  OdtTraverserContext *context);
    virtual void endUnknownTags(KoXmlElement &element,
                                OdtTraverserContext *context);
    virtual void beginTagNote(KoXmlElement &element,
                              OdtTraverserContext *context);
    virtual void endTagNote(KoXmlElement &element,
                            OdtTraverserContext *context);
};

#endif // ODTTRAVERSERBACKEND_H
