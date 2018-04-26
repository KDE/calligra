/* This file is part of the KDE project
   Copyright 2018 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "GapCommand.h"

// KF5
#include <klocalizedstring.h>

// KoChart
#include "Axis.h"
#include "ChartShape.h"
#include "ChartDebug.h"


namespace KoChart {


GapCommand::GapCommand(Axis *axis, ChartShape *chart)
    : m_axis(axis)
    , m_chart(chart)
{
    m_oldGapBetweenBars = 0;
    m_newGapBetweenBars = 0;
    m_oldGapBetweenSets = 0;
    m_newGapBetweenSets = 0;
}

GapCommand::~GapCommand()
{
}

void GapCommand::redo()
{
    if (m_oldGapBetweenBars != m_newGapBetweenBars) {
        m_axis->setGapBetweenBars(m_newGapBetweenBars);
    }
    if (m_oldGapBetweenSets != m_newGapBetweenSets) {
        m_axis->setGapBetweenSets(m_newGapBetweenSets);
    }
    m_chart->update();
}

void GapCommand::undo()
{
    if (m_oldGapBetweenBars != m_newGapBetweenBars) {
        m_axis->setGapBetweenBars(m_oldGapBetweenBars);
    }
    if (m_oldGapBetweenSets != m_newGapBetweenSets) {
        m_axis->setGapBetweenSets(m_oldGapBetweenSets);
    }
    m_chart->update();
}

void GapCommand::setGapBetweenBars(int percent)
{
    m_oldGapBetweenBars = m_axis->gapBetweenBars();
    m_newGapBetweenBars = percent;
    setText(kundo2_i18n("Set Gap Between Bars"));
}

void GapCommand::setGapBetweenSets(int percent)
{
    m_oldGapBetweenSets = m_axis->gapBetweenSets();
    m_newGapBetweenSets = percent;
    setText(kundo2_i18n("Set Gap Between Sets"));
}

}
