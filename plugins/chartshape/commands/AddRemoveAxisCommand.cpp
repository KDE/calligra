/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2017 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AddRemoveAxisCommand.h"

// KF5
#include <KLocalizedString>

// Calligra
#include "KoShape.h"
#include "KoShapeManager.h"
#include "KoShapeMoveCommand.h"
#include "KoShapeSizeCommand.h"

// KoChart
#include "Axis.h"
#include "ChartDebug.h"
#include "ChartLayout.h"
#include "ChartShape.h"
#include "ChartTextShapeCommand.h"
#include "PlotArea.h"

using namespace KoChart;

AddRemoveAxisCommand::AddRemoveAxisCommand(Axis *axis, ChartShape *chart, bool add, KoShapeManager *shapeManager, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_axis(axis)
    , m_chart(chart)
    , m_add(add)
    , mine(add)
    , m_shapeManager(shapeManager)
{
    if (add) {
        setText(kundo2_i18n("Add Axis"));
        initAdd();
    } else {
        setText(kundo2_i18n("Remove Axis"));
        initRemove();
    }
}

AddRemoveAxisCommand::~AddRemoveAxisCommand()
{
    if (mine) {
        m_chart->layout()->remove(m_axis->title()); // to be safe; this should be handled elsewhere
        delete m_axis;
    }
}

void AddRemoveAxisCommand::redo()
{
    mine = !mine;
    if (m_add) {
        m_axis->plotArea()->addAxis(m_axis);
        m_axis->updateKChartAxisPosition();
        m_shapeManager->addShape(m_axis->title(), KoShapeManager::AddWithoutRepaint);
        m_axis->plotArea()->addTitleToLayout();
    } else {
        //         m_shapeManager->remove(m_axis->title());
        m_axis->plotArea()->takeAxis(m_axis);
    }
    KUndo2Command::redo();
    m_axis->plotArea()->plotAreaUpdate();
    m_chart->update();
    m_chart->relayout();
}

void AddRemoveAxisCommand::undo()
{
    mine = !mine;
    if (m_add) {
        m_axis->plotArea()->takeAxis(m_axis);
        //         m_shapeManager->remove(m_axis->title());
    } else {
        m_axis->plotArea()->addAxis(m_axis);
        m_axis->updateKChartAxisPosition();
        m_shapeManager->addShape(m_axis->title(), KoShapeManager::AddWithoutRepaint);
        m_axis->plotArea()->addTitleToLayout();
    }
    KUndo2Command::undo();
    m_axis->plotArea()->plotAreaUpdate();
    m_chart->update();
    m_chart->relayout();
}

void AddRemoveAxisCommand::initAdd()
{
}

void AddRemoveAxisCommand::initRemove()
{
}
