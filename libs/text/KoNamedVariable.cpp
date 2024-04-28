/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoNamedVariable.h"

#include "KoInlineTextObjectManager.h"

// Include Q_UNUSED classes, for building on Windows
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>

KoNamedVariable::KoNamedVariable(Property key, const QString &name)
    : KoVariable(true)
    , m_name(name)
    , m_key(key)
{
}

void KoNamedVariable::propertyChanged(Property property, const QVariant &value)
{
    if (property == m_key)
        setValue(qvariant_cast<QString>(value));
}

void KoNamedVariable::setup()
{
    setValue(manager()->stringProperty(m_key));
}

bool KoNamedVariable::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    // TODO
    return false;
}

void KoNamedVariable::saveOdf(KoShapeSavingContext &context)
{
    Q_UNUSED(context);
    // TODO
}
