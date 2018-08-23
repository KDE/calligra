/* This file is part of the KDE project
 * Copyright 2017 Dag Andersen <danders@get2net.dk>
 * Copyright 2012 Brijesh Patel <brijesh3105@gmail.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ChartTextShapeCommand.h"

// KF5
#include <klocalizedstring.h>

// Calligra
#include "KoShape.h"
#include "KoShapeMoveCommand.h"
#include "KoShapeSizeCommand.h"

// KoChart
#include "ChartShape.h"
#include "ChartLayout.h"
#include "PlotArea.h"
#include "ChartDebug.h"

using namespace KoChart;

ChartTextShapeCommand::ChartTextShapeCommand(KoShape* textShape, ChartShape *chart, bool isVisible, KUndo2Command *parent)
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

ChartTextShapeCommand::~ChartTextShapeCommand()
{
}

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
