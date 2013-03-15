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


// Own
#include "OdtTraverserBackend.h"

// Calligra
#include <KoXmlReader.h>

// Odftraverse library
#include "OdfParser.h"


class OdtTraverserBackend::Private
{
 public:
    Private();
    ~Private();

    bool dummy;                 // We don't have any actual content in this class yet.
};

OdtTraverserBackend::Private::Private()
{
}

OdtTraverserBackend::Private::~Private()
{
}


// ----------------------------------------------------------------
//                 class OdtTraverserBackend


OdtTraverserBackend::OdtTraverserBackend(OdfTraverserContext *context)
    : d(new OdtTraverserBackend::Private)
{
    Q_UNUSED(context)
}

OdtTraverserBackend::~OdtTraverserBackend()
{
}


void OdtTraverserBackend::beginTraversal(OdfTraverserContext *context)
{
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTraversal(OdfTraverserContext *context)
{
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginInsideElementsTag(KoXmlElement &element,
                                                 OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endInsideElementsTag(KoXmlElement &element,
                                               OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagP(KoXmlElement &element,
                                    OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagP(KoXmlElement &element,
                                  OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagH(KoXmlElement &element,
                                    OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagH(KoXmlElement &element,
                                  OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagSpan(KoXmlElement &element,
                                       OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagSpan(KoXmlElement &element,
                                     OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagS(KoXmlElement &element,
                                    OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagS(KoXmlElement &element,
                                  OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagTab(KoXmlElement &element,
                                      OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagTab(KoXmlElement &element,
                                    OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagLineBreak(KoXmlElement &element,
                                            OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagLineBreak(KoXmlElement &element,
                                          OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagPageBreak(KoXmlElement &element,
                                            OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagPageBreak(KoXmlElement &element,
                                          OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagA(KoXmlElement &element,
                                    OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagA(KoXmlElement &element,
                                  OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginCharacterData(KoXmlNode &node,
                                             OdfTraverserContext *context)
{
    Q_UNUSED(node);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endCharacterData(KoXmlNode &node,
                                           OdfTraverserContext *context)
{
    Q_UNUSED(node);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagList(KoXmlElement &element,
                                       OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagList(KoXmlElement &element,
                                     OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagTable(KoXmlElement &element,
                                        OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagTable(KoXmlElement &element,
                                      OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagTableRow(KoXmlElement &element,
                                           OdfTraverserContext *context,
                                           OdtTraverser::TableCellType type)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(type);
}

void OdtTraverserBackend::endTagTableRow(KoXmlElement &element,
                                         OdfTraverserContext *context,
                                         OdtTraverser::TableCellType type)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(type);
}


// ----------------------------------------------------------------
//                             Frames


void OdtTraverserBackend::beginTagFrame(KoXmlElement &element,
                                        OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagFrame(KoXmlElement &element,
                                      OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginEmbeddedFormula(const QString &href,
                                               OdfTraverserContext *context)
{
    Q_UNUSED(href);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endEmbeddedFormula(const QString &href,
                                             OdfTraverserContext *context)
{
    Q_UNUSED(href);
    Q_UNUSED(context);
}


// ----------------------------------------------------------------


void OdtTraverserBackend::beginTagTableOfContent(KoXmlElement &element,
                                                 OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagTableOfContent(KoXmlElement &element,
                                               OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagTableOfContentBody(KoXmlElement &element,
                                                     OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagTableOfContentBody(KoXmlElement &element,
                                                   OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagBookMark(KoXmlElement &element,
                                           OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagBookMark(KoXmlElement &element,
                                         OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagBookMarkStart(KoXmlElement &element,
                                                OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagBookMarkStart(KoXmlElement &element,
                                              OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagBookMarkEnd(KoXmlElement &element,
                                              OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagBookMarkEnd(KoXmlElement &element,
                                            OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginUnknownTags(KoXmlElement &element,
                                           OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endUnknownTags(KoXmlElement &element,
                                         OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagNote(KoXmlElement &element,
                                       OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagNote(KoXmlElement &element,
                                     OdfTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

