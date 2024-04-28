/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "EllipseShapeConfigCommand.h"
#include <KLocalizedString>

EllipseShapeConfigCommand::EllipseShapeConfigCommand(EllipseShape *ellipse,
                                                     EllipseShape::EllipseType type,
                                                     qreal startAngle,
                                                     qreal endAngle,
                                                     KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_ellipse(ellipse)
    , m_newType(type)
    , m_newStartAngle(startAngle)
    , m_newEndAngle(endAngle)
{
    Q_ASSERT(m_ellipse);

    setText(kundo2_i18n("Change ellipse"));

    m_oldType = m_ellipse->type();
    m_oldStartAngle = m_ellipse->startAngle();
    m_oldEndAngle = m_ellipse->endAngle();
}

void EllipseShapeConfigCommand::redo()
{
    KUndo2Command::redo();

    m_ellipse->update();

    if (m_oldType != m_newType)
        m_ellipse->setType(m_newType);
    if (m_oldStartAngle != m_newStartAngle)
        m_ellipse->setStartAngle(m_newStartAngle);
    if (m_oldEndAngle != m_newEndAngle)
        m_ellipse->setEndAngle(m_newEndAngle);

    m_ellipse->update();
}

void EllipseShapeConfigCommand::undo()
{
    KUndo2Command::undo();

    m_ellipse->update();

    if (m_oldType != m_newType)
        m_ellipse->setType(m_oldType);
    if (m_oldStartAngle != m_newStartAngle)
        m_ellipse->setStartAngle(m_oldStartAngle);
    if (m_oldEndAngle != m_newEndAngle)
        m_ellipse->setEndAngle(m_oldEndAngle);

    m_ellipse->update();
}
