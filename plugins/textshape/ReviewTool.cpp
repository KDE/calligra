/* This file is part of the KDE project
 * Copyright (C) 2009-2010 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
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

#include "ReviewTool.h"
#include <KoToolBase.h>

#include <KoCanvasBase.h>
#include <KoTextLayoutRootArea.h>
#include <KoChangeTracker.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoTextDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextEditor.h>
#include <KoTextShapeData.h>
#include <KoViewConverter.h>
#include <KoGlobal.h>

#include "TextShape.h"

#include "commands/AcceptChangeCommand.h"
#include "commands/RejectChangeCommand.h"
#include "commands/ShowChangesCommand.h"
#include "dialogs/TrackedChangeModel.h"
#include "dialogs/TrackedChangeManager.h"
#include "dialogs/AcceptRejectChangeDialog.h"
#include "dialogs/ChangeConfigureDialog.h"

#include <KLocale>
#include <KAction>
#include <KUser>

ReviewTool::ReviewTool(KoCanvasBase* canvas): TextTool(canvas),
    m_textEditor(0),
    m_textShapeData(0),
    m_canvas(canvas),
    m_textShape(0),
    m_model(0),
    m_trackedChangeManager(0),
    m_changesTreeView(0)
{
    createActions();
}

ReviewTool::~ReviewTool()
{
}

void ReviewTool::createActions()
{
    KAction *action;
    action = new KAction(i18n("Tracked change manager"), this);
    action->setShortcut(Qt::ALT + Qt::CTRL + Qt::Key_T);
    addAction("show_changeManager", action);
    connect(action, SIGNAL(triggered()), this, SLOT(showTrackedChangeManager()));

    m_actionShowChanges = new KAction(i18n("Show Changes"), this);
    m_actionShowChanges->setCheckable(true);
    addAction("edit_show_changes", m_actionShowChanges);
    connect(m_actionShowChanges, SIGNAL(triggered(bool)), this, SLOT(toggleShowChanges(bool)));

    m_actionRecordChanges = new KAction(i18n("Record Changes"), this);
    m_actionRecordChanges->setCheckable(true);
    addAction("edit_record_changes", m_actionRecordChanges);
    connect(m_actionRecordChanges, SIGNAL(triggered(bool)), this, SLOT(toggleRecordChanges(bool)));
}

void ReviewTool::mouseReleaseEvent(KoPointerEvent* event)
{
    TextTool::mouseReleaseEvent(event);
}
void ReviewTool::activate(KoToolBase::ToolActivation toolActivation, const QSet< KoShape* >& shapes)
{
    TextTool::activate(toolActivation, shapes);
}
void ReviewTool::deactivate()
{
    TextTool::deactivate();
}
void ReviewTool::mouseMoveEvent(KoPointerEvent* event)
{
    TextTool::mouseMoveEvent(event);
}
void ReviewTool::mousePressEvent(KoPointerEvent* event)
{
    TextTool::mousePressEvent(event);
}
void ReviewTool::keyPressEvent(QKeyEvent* event)
{
    TextTool::keyPressEvent(event);
}
void ReviewTool::paint(QPainter& painter, const KoViewConverter& converter)
{
    TextTool::paint(painter,converter);
}

void ReviewTool::showTrackedChangeManager()
{
    Q_ASSERT(m_model);
    m_trackedChangeManager = new TrackedChangeManager();
    m_trackedChangeManager->setModel(m_model);
    connect(m_trackedChangeManager, SIGNAL(currentChanged(QModelIndex)), this, SLOT(selectedChangeChanged(QModelIndex)));
    m_trackedChangeManager->show();
    //    view.setModel(&model);
    //    view.setWindowTitle("testTracked");
    //    view.show();
    //    TrackedChangeManager *dia = new TrackedChangeManager(m_textShapeData->document());
    //    dia->show();
}


void ReviewTool::toggleShowChanges(bool on)//TODO transfer this in KoTextEditor
{
    m_actionShowChanges->setChecked(on);
    ShowChangesCommand *command = new ShowChangesCommand(on, m_textShapeData->document(), this->canvas());
    connect(command, SIGNAL(toggledShowChange(bool)), m_actionShowChanges, SLOT(setChecked(bool)));
    m_textEditor->addCommand(command);
}

void ReviewTool::toggleRecordChanges(bool on)
{
    m_actionRecordChanges->setChecked(on);
    if (m_changeTracker)
        m_changeTracker->setRecordChanges(on);
}
