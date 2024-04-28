/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeTextAnchorCommand.h"
#include <KLocalizedString>

ChangeTextAnchorCommand::ChangeTextAnchorCommand(ArtisticTextShape *shape, ArtisticTextShape::TextAnchor anchor)
    : m_shape(shape)
    , m_anchor(anchor)
{
    setText(kundo2_i18n("Change text anchor"));
}

void ChangeTextAnchorCommand::undo()
{
    if (m_shape) {
        m_shape->setTextAnchor(m_oldAnchor);
    }
}

void ChangeTextAnchorCommand::redo()
{
    if (m_shape) {
        m_oldAnchor = m_shape->textAnchor();
        m_shape->setTextAnchor(m_anchor);
    }
}
