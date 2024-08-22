/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GapCommand.h"

// KF5
#include <KLocalizedString>

// KoChart
#include "Axis.h"
#include "ChartDebug.h"
#include "ChartShape.h"

namespace KoChart
{

GapCommand::GapCommand(Axis *axis, ChartShape *chart)
    : m_axis(axis)
    , m_chart(chart)
{
    m_oldGapBetweenBars = 0;
    m_newGapBetweenBars = 0;
    m_oldGapBetweenSets = 0;
    m_newGapBetweenSets = 0;
}

GapCommand::~GapCommand() = default;

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
