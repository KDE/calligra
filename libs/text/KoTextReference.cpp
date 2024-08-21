/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextReference.h"

#include "KoInlineTextObjectManager.h"
#include "KoTextLocator.h"

// Include Q_UNSUSED classes, for building on Windows
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>

KoTextReference::KoTextReference(int indexId)
    : KoVariable()
    , m_indexId(indexId)
{
}

KoTextReference::~KoTextReference()
{
    KoTextLocator *loc = locator();
    if (loc)
        loc->removeListener(this);
}

void KoTextReference::variableMoved(const QTextDocument *document, int posInDocument)
{
    Q_UNUSED(document);
    Q_UNUSED(posInDocument);
    Q_ASSERT(manager());
    KoTextLocator *loc = locator();
    if (loc)
        setValue(QString::number(loc->pageNumber()));
    else
        setValue("NOREF"); // anything smarter to point to a broken reference?
}

void KoTextReference::setup()
{
    locator()->addListener(this);
    variableMoved(nullptr, 0);
}

KoTextLocator *KoTextReference::locator()
{
    return dynamic_cast<KoTextLocator *>(manager()->inlineTextObject(m_indexId));
}

bool KoTextReference::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    // TODO
    return false;
}

void KoTextReference::saveOdf(KoShapeSavingContext &context)
{
    Q_UNUSED(context);
    // TODO
}
