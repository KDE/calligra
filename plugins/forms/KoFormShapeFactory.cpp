/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFormShapeFactory.h"

#include "KoFormShape.h"

#include <KLocalizedString>
#include <KoXmlNS.h>

using namespace Qt::StringLiterals;

KoFormShapeFactory::KoFormShapeFactory()
    : KoShapeFactoryBase(u"FormShape"_s, i18n("Form Control"))
{
    setToolTip(i18n("OpenDocument form control"));
    setLoadingPriority(10);
    setXmlElements({qMakePair(QString(KoXmlNS::draw), QStringList{u"control"_s})});
}

KoShape *KoFormShapeFactory::createDefaultShape(KoDocumentResourceManager *) const
{
    return new KoFormShape();
}

bool KoFormShapeFactory::supports(const KoXmlElement &element, KoShapeLoadingContext &) const
{
    return element.localName() == "control"_L1 && element.namespaceURI() == KoXmlNS::draw && !element.attributeNS(KoXmlNS::draw, u"control"_s).isEmpty();
}
