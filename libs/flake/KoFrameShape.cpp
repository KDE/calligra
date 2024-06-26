/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoFrameShape.h"

#include <FlakeDebug.h>
#include <KoXmlReader.h>

class Q_DECL_HIDDEN KoFrameShape::Private
{
public:
    Private(const QString &ns, const QString &tag)
        : ns(ns)
        , tag(tag)
    {
    }

    const QString ns;
    const QString tag;
};

KoFrameShape::KoFrameShape(const QString &ns, const QString &tag)
    : d(new Private(ns, tag))
{
}

KoFrameShape::~KoFrameShape()
{
    delete d;
}

bool KoFrameShape::loadOdfFrame(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    const KoXmlElement &frameElement(KoXml::namedItemNS(element, d->ns, d->tag));
    if (frameElement.isNull()) {
        errorFlake << "frame element" << d->tag << "not found";
        return false;
    }

    return loadOdfFrameElement(frameElement, context);
}
