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


class OdtTraverserContext::Private
{
public:
    Private(KoStore *store);
    ~Private();

    KoStore *store;

    // This data is used for conversion while traversing the content tree.
    // It's created from the store that is given to us at construction time.
    QHash<QString, QString>    metadata;
    QHash<QString, QString>    manifest;
    QHash<QString, StyleInfo*> styles;

    // This data is created during the traversal and can be used after
    // it is finished.
    QHash<QString, QSizeF>   images;
    QHash<QString, QString>  mediaFiles;
};


OdtTraverserContext::Private::Private(KoStore *store)
{
    this->store = store;
}

OdtTraverserContext::Private::~Private()
{
    // FIXME: Maybe we should delete the store here...  or not?
    //        The question is: who really owns it?
}


QHash<QString, QString> OdtTraverserContext::metadata() const
{
    return d->metadata;
}

    QHash<QString, QString> OdtTraverserContext::manifest() const
{
    return d->manifest;
}

    QHash<QString, StyleInfo*> OdtTraverserContext::styles() const
{
    return d->styles;
}


QHash<QString, QSizeF> OdtTraverserContext::images() const
{
    return d->images;
}

QHash<QString, QString> OdtTraverserContext::mediaFiles() const
{
    return d->mediaFiles;
}


// ----------------------------------------------------------------


OdtTraverserBackend::OdtTraverserBackend(OdtTraverserContext *context)
{
    Q_UNUSED(context)
}

OdtTraverserBackend::~OdtTraverserBackend()
{
}


void OdtTraverserBackend::beginTraversal(OdtTraverserContext *context)
{
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTraversal(OdtTraverserContext *context)
{
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginInsideElementsTag(KoXmlElement &element,
                                                 OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endInsideElementsTag(KoXmlElement &element,
                                               OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagP(KoXmlElement &element,
                                    OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagP(KoXmlElement &element,
                                  OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagH(KoXmlElement &element,
                                    OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagH(KoXmlElement &element,
                                  OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagSpan(KoXmlElement &element,
                                       OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagSpan(KoXmlElement &element,
                                     OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagS(KoXmlElement &element,
                                    OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagS(KoXmlElement &element,
                                  OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginCharacterData(KoXmlNode &node,
                                             OdtTraverserContext *context)
{
    Q_UNUSED(node);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endCharacterData(KoXmlNode &node,
                                           OdtTraverserContext *context)
{
    Q_UNUSED(node);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagTable(KoXmlElement &element,
                                        OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagTable(KoXmlElement &element,
                                      OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagTableRow(KoXmlElement &element,
                                           OdtTraverserContext *context,
                                           TableCellType type)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    Q_UNUSED(type);
}

void OdtTraverserBackend::endTagTableRow(KoXmlElement &element,
                                           OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagA(KoXmlElement &element,
                                    OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagA(KoXmlElement &element,
                                  OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagPageBreak(KoXmlElement &element,
                                            OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagPageBreak(KoXmlElement &element,
                                          OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagList(KoXmlElement &element,
                                       OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagList(KoXmlElement &element,
                                     OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagFrame(KoXmlElement &element,
                                        OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagFrame(KoXmlElement &element,
                                      OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginEmbeddedFormula(const QString &href,
                                               OdtTraverserContext *context)
{
    Q_UNUSED(href);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endEmbeddedFormula(const QString &href,
                                             OdtTraverserContext *context)
{
    Q_UNUSED(href);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagTab(KoXmlElement &element,
                                      OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagTab(KoXmlElement &element,
                                    OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagTableOfContent(KoXmlElement &element,
                                                 OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagTableOfContent(KoXmlElement &element,
                                               OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagTableOfContentBody(KoXmlElement &element,
                                                     OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagTableOfContentBody(KoXmlElement &element,
                                                   OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginTagLineBreak(OdtTraverserContext *context)
{
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagLineBreak(OdtTraverserContext *context)
{
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagBookMark(KoXmlElement &element,
                                           OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagBookMark(KoXmlElement &element,
                                         OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagBookMarkStart(KoXmlElement &element,
                                                OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagBookMarkStart(KoXmlElement &element,
                                              OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagBookMarkEnd(KoXmlElement &element,
                                              OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagBookMarkEnd(KoXmlElement &element,
                                            OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}


void OdtTraverserBackend::beginUnknownTags(KoXmlElement &element,
                                           OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endUnknownTags(KoXmlElement &element,
                                         OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::beginTagNote(KoXmlElement &element,
                                       OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

void OdtTraverserBackend::endTagNote(KoXmlElement &element,
                                     OdtTraverserContext *context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
}

