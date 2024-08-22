/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
   SPDX-FileCopyrightText: 2012 Brijesh Patel <brijesh3105@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "LegendCommand.h"

// KF5
#include <KLocalizedString>

// KoChart
#include "ChartDebug.h"
#include "ChartLayout.h"
#include "ChartShape.h"
#include "Legend.h"

using namespace KoChart;
using namespace KChart;

LegendCommand::LegendCommand(KoChart::Legend *legend)
    : m_legend(legend)
{
    QObject *l = qobject_cast<QObject *>(legend); // legend is both KoShape and QObject, both with parent() method
    m_chart = dynamic_cast<ChartShape *>(l->parent());
    Q_ASSERT(m_chart);

    m_newFont = legend->font();
    m_newTitle = legend->title();
    m_newFontSize = legend->fontSize();
    m_newExpansion = legend->expansion();
}

LegendCommand::~LegendCommand() = default;

void LegendCommand::redo()
{
    // save the old type
    m_oldTitle = m_legend->title();
    m_oldFont = m_legend->font();
    m_oldFontSize = m_legend->fontSize();
    m_oldExpansion = m_legend->expansion();
    if (m_oldTitle == m_newTitle && m_oldFont == m_newFont && m_oldFontSize == m_newFontSize && m_oldExpansion == m_newExpansion)
        return;

    // Actually do the work
    m_legend->setTitle(m_newTitle);
    m_legend->setFont(m_newFont);
    m_legend->setFontSize(m_newFontSize);
    m_legend->setExpansion(m_newExpansion);

    m_legend->update();
}

void LegendCommand::undo()
{
    if (m_oldTitle == m_newTitle && m_oldFont == m_newFont && m_oldFontSize == m_newFontSize && m_oldExpansion == m_newExpansion)
        return;

    m_legend->setTitle(m_oldTitle);
    m_legend->setFont(m_oldFont);
    m_legend->setFontSize(m_oldFontSize);
    m_legend->setExpansion(m_oldExpansion);

    m_legend->update();
}

void LegendCommand::setLegendTitle(const QString &title)
{
    m_newTitle = title;

    setText(kundo2_i18n("Set Legend Title"));
}

void LegendCommand::setLegendFont(const QFont &font)
{
    m_newFont = font;
    m_newFontSize = font.pointSize();

    setText(kundo2_i18n("Set Legend Font"));
}

void LegendCommand::setLegendFontSize(int size)
{
    m_newFontSize = size;

    setText(kundo2_i18n("Set Legend Font size"));
}

void LegendCommand::setLegendExpansion(LegendExpansion expansion)
{
    m_newExpansion = expansion;

    setText(kundo2_i18n("Set Legend Orientation"));
}
