/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeOdfSaveHelper.h"
#include "KoDragOdfSaveHelper_p.h"

#include <KoOdf.h>
#include <KoShape.h>
#include <KoXmlWriter.h>

#include <algorithm>

class KoShapeOdfSaveHelperPrivate : public KoDragOdfSaveHelperPrivate
{
public:
    KoShapeOdfSaveHelperPrivate(const QList<KoShape *> &shapes)
        : shapes(shapes)
    {
    }

    QList<KoShape *> shapes;
};

KoShapeOdfSaveHelper::KoShapeOdfSaveHelper(const QList<KoShape *> &shapes)
    : KoDragOdfSaveHelper(*(new KoShapeOdfSaveHelperPrivate(shapes)))
{
}

bool KoShapeOdfSaveHelper::writeBody()
{
    Q_D(KoShapeOdfSaveHelper);
    d->context->addOption(KoShapeSavingContext::DrawId);

    KoXmlWriter &bodyWriter = d->context->xmlWriter();
    bodyWriter.startElement("office:body");
    bodyWriter.startElement(KoOdf::bodyContentElement(KoOdf::Text, true));

    std::sort(d->shapes.begin(), d->shapes.end(), KoShape::compareShapeZIndex);
    foreach (KoShape *shape, d->shapes) {
        shape->saveOdf(*d->context);
    }

    bodyWriter.endElement(); // office:element
    bodyWriter.endElement(); // office:body

    return true;
}
