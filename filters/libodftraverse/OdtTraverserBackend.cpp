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


// ----------------------------------------------------------------
//             class OdtTraverserBackend::Private


class OdtTraverserBackend::Private
{
 public:
    Private();
    ~Private();

    bool dummy;                 // We don't have any actual content in
                                // this class yet but it's still
                                // needed for forward binary compatibility.
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
    delete d;
}


void OdtTraverserBackend::beginEndTraversal(OdfTraverserContext *context, BeginEndTag beginEnd)
{
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}


void OdtTraverserBackend::insideElementsTag(KoXmlElement &element, OdfTraverserContext *context,
                                            BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}


void OdtTraverserBackend::tagP(KoXmlElement &element, OdfTraverserContext *context,
                               BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagH(KoXmlElement &element, OdfTraverserContext *context,
                               BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagSpan(KoXmlElement &element, OdfTraverserContext *context,
                                  BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagS(KoXmlElement &element, OdfTraverserContext *context,
                               BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagTab(KoXmlElement &element, OdfTraverserContext *context,
                                 BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagLineBreak(KoXmlElement &element, OdfTraverserContext *context,
                                       BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagPageBreak(KoXmlElement &element, OdfTraverserContext *context,
                                       BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagA(KoXmlElement &element, OdfTraverserContext *context,
                               BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::characterData(KoXmlNode &node, OdfTraverserContext *context,
                                        BeginEndTag beginEnd)
{
    Q_UNUSED(node);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}


void OdtTraverserBackend::tagList(KoXmlElement &element, OdfTraverserContext *context,
                                  BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}


void OdtTraverserBackend::tagTable(KoXmlElement &element, OdfTraverserContext *context,
                                   BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagTableRow(KoXmlElement &element, OdfTraverserContext *context,
                                      OdtTraverser::TableCellType type, BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(type);
    Q_UNUSED(beginEnd);
}


// ----------------------------------------------------------------
//                             Frames


void OdtTraverserBackend::tagFrame(KoXmlElement &element, OdfTraverserContext *context,
                                   BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::beginEmbeddedFormula(const QString &href, OdfTraverserContext *context,
                                               BeginEndTag beginEnd)
{
    Q_UNUSED(href);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}


// ----------------------------------------------------------------


void OdtTraverserBackend::tagTableOfContent(KoXmlElement &element, OdfTraverserContext *context,
                                            BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}


void OdtTraverserBackend::tagTableOfContentBody(KoXmlElement &element, OdfTraverserContext *context,
                                                BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}


void OdtTraverserBackend::tagBookMark(KoXmlElement &element, OdfTraverserContext *context,
                                      BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagBookMarkStart(KoXmlElement &element, OdfTraverserContext *context,
                                           BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::tagBookMarkEnd(KoXmlElement &element, OdfTraverserContext *context,
                                         BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}


void OdtTraverserBackend::tagNote(KoXmlElement &element, OdfTraverserContext *context,
                                       BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}

void OdtTraverserBackend::unknownTags(KoXmlElement &element, OdfTraverserContext *context,
                                      BeginEndTag beginEnd)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(beginEnd);
}
