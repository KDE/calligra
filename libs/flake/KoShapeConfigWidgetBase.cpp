/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShapeConfigWidgetBase.h"

KoShapeConfigWidgetBase::KoShapeConfigWidgetBase()
    : m_resourceManager(nullptr)
{
}

KoShapeConfigWidgetBase::~KoShapeConfigWidgetBase() = default;

void KoShapeConfigWidgetBase::setUnit(const KoUnit &)
{
}

void KoShapeConfigWidgetBase::setResourceManager(KoCanvasResourceManager *rm)
{
    m_resourceManager = rm;
}

bool KoShapeConfigWidgetBase::showOnShapeCreate()
{
    return false;
}

bool KoShapeConfigWidgetBase::showOnShapeSelect()
{
    return true;
}

KUndo2Command *KoShapeConfigWidgetBase::createCommand()
{
    return nullptr;
}
