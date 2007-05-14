/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.kruiselbrink@student.tue.nl>
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
#include <QPainter>
#include <QGridLayout>
#include <QToolButton>
#include <QTabWidget>

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <KFileDialog>

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "MusicShape.h"

#include "SimpleEntryTool.h"
#include "SimpleEntryTool.moc"

#include "dialogs/SimpleEntryWidget.h"

SimpleEntryTool::SimpleEntryTool( KoCanvasBase* canvas )
    : KoTool( canvas ),
    m_musicshape(0)
{
    QActionGroup* noteGroup = new QActionGroup(this);
    
    m_actionBreveNote = new QAction(KIcon("music-note-breve"), i18n("Double whole note"), this);
    addAction("note_breve", m_actionBreveNote);
    m_actionBreveNote->setCheckable(true);
    noteGroup->addAction(m_actionBreveNote);
    
    m_actionWholeNote = new QAction(KIcon("music-note-whole"), i18n("Whole note"), this);
    addAction("note_whole", m_actionWholeNote);
    m_actionWholeNote->setCheckable(true);
    noteGroup->addAction(m_actionWholeNote);
    
    m_actionHalfNote = new QAction(KIcon("music-note-half"), i18n("Half note"), this);
    addAction("note_half", m_actionHalfNote);
    m_actionHalfNote->setCheckable(true);
    noteGroup->addAction(m_actionHalfNote);
    
    m_actionQuarterNote = new QAction(KIcon("music-note-quarter"), i18n("Quarter note"), this);
    addAction("note_quarter", m_actionQuarterNote);
    m_actionQuarterNote->setCheckable(true);
    noteGroup->addAction(m_actionQuarterNote);
    
    m_actionNote8 = new QAction(KIcon("music-note-eighth"), i18n("Eighth note"), this);
    addAction("note_eighth", m_actionNote8);
    m_actionNote8->setCheckable(true);
    noteGroup->addAction(m_actionNote8);
    
    m_actionNote16 = new QAction(KIcon("music-note-16th"), i18n("16th note"), this);
    addAction("note_16th", m_actionNote16);
    m_actionNote16->setCheckable(true);
    noteGroup->addAction(m_actionNote16);
    
    m_actionNote32 = new QAction(KIcon("music-note-32nd"), i18n("32nd note"), this);
    addAction("note_32nd", m_actionNote32);
    m_actionNote32->setCheckable(true);
    noteGroup->addAction(m_actionNote32);
    
    m_actionNote64 = new QAction(KIcon("music-note-64th"), i18n("64th note"), this);
    addAction("note_64th", m_actionNote64);
    m_actionNote64->setCheckable(true);
    noteGroup->addAction(m_actionNote64);
    
    m_actionNote128 = new QAction(KIcon("music-note-128th"), i18n("128th note"), this);
    addAction("note_128th", m_actionNote128);
    m_actionNote128->setCheckable(true);
    noteGroup->addAction(m_actionNote128);

    
    m_actionQuarterNote->setChecked(true);
}

SimpleEntryTool::~SimpleEntryTool()
{
}

void SimpleEntryTool::activate (bool temporary)
{
    Q_UNUSED( temporary );
    kDebug() << k_funcinfo << endl;
    
    KoSelection* selection = m_canvas->shapeManager()->selection();
    foreach ( KoShape* shape, selection->selectedShapes() )
    {
        m_musicshape = dynamic_cast<MusicShape*>( shape );
        if ( m_musicshape )
            break;
    }
    if ( !m_musicshape )
    {
        emit sigDone();
        return;
    }
    useCursor( Qt::ArrowCursor, true );
}

void SimpleEntryTool::deactivate()
{
    kDebug()<<"SimpleEntryTool::deactivate\n";
    m_musicshape = 0;
}

void SimpleEntryTool::paint( QPainter& painter, KoViewConverter& viewConverter )
{
    Q_UNUSED( viewConverter );
}

void SimpleEntryTool::mousePressEvent( KoPointerEvent* )
{
}

void SimpleEntryTool::mouseMoveEvent( KoPointerEvent* )
{
}

void SimpleEntryTool::mouseReleaseEvent( KoPointerEvent* )
{
}

void SimpleEntryTool::addCommand(QUndoCommand* command)
{
    m_canvas->addCommand(command);
}


QWidget * SimpleEntryTool::createOptionWidget()
{
    SimpleEntryWidget* widget = new SimpleEntryWidget(this);
    
    return widget;
}

