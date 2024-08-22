/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
 * SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChartTextShapeCommand.h"

// KF5
#include <KLocalizedString>

// Calligra
#include "KoShape.h"
#include "KoShapeMoveCommand.h"
#include "KoShapeSizeCommand.h"

// KoChart
#include "ChartDebug.h"
#include "ChartLayout.h"
#include "ChartShape.h"
#include "PlotArea.h"

using namespace KoChart;

ChartTextShapeCommand::ChartTextShapeCommand(KoShape *textShape, ChartShape *chart, bool isVisible, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_textShape(textShape)
    , m_chart(chart)
    , m_oldIsVisible(textShape->isVisible())
    , m_newIsVisible(isVisible)
{
    init();

    if (m_newIsVisible) {
        setText(kundo2_i18n("Show Text Shape"));
    } else {
        setText(kundo2_i18n("Hide Text Shape"));
    }
}

ChartTextShapeCommand::~ChartTextShapeCommand() = default;

void ChartTextShapeCommand::redo()
{
    KUndo2Command::redo();
    if (m_newRotation != m_oldRotation) {
        m_textShape->rotate(-m_oldRotation);
        m_textShape->rotate(m_newRotation);
    }
    if (m_oldIsVisible != m_newIsVisible) {
        m_textShape->setVisible(m_newIsVisible); // after redo()
    }
    m_chart->update();
    m_chart->relayout();
}

void ChartTextShapeCommand::undo()
{
    KUndo2Command::undo();
    if (m_newRotation != m_oldRotation) {
        m_textShape->rotate(-m_newRotation);
        m_textShape->rotate(m_oldRotation);
    }
    if (m_oldIsVisible != m_newIsVisible) {
        m_textShape->setVisible(m_oldIsVisible); // after undo()
    }
    m_chart->update();
    m_chart->relayout();
}

void ChartTextShapeCommand::init()
{
    m_newRotation = m_oldRotation = m_textShape->rotation();
}

void ChartTextShapeCommand::setRotation(int angle)
{
    // Do not need a text here as rotation will only be done when textshape is made visible in some way
    m_newRotation = angle;
}
