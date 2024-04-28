/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CommentShapeFactory.h"
#include "CommentShape.h"

#include <KLocalizedString>
#include <KoDocumentBase.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

CommentShapeFactory::CommentShapeFactory()
    : KoShapeFactoryBase(COMMENTSHAPEID, i18n("Comment"))
{
    setXmlElementNames(KoXmlNS::officeooo, QStringList("annotation"));
    setHidden(true);
}

CommentShapeFactory::~CommentShapeFactory()
{
}

KoShape *CommentShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    return new CommentShape(documentResources);
}

bool CommentShapeFactory::supports(const KoXmlElement &element, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return element.localName() == "annotation" && element.namespaceURI() == KoXmlNS::officeooo; // TODO change accordingly
}
