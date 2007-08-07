/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.Kruisselbrink@student.tue.nl>
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
#include <QPair>
#include <QInputDialog>

#include <kdebug.h>
#include <klocale.h>
#include <kicon.h>
#include <KFileDialog>

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

#include "MusicShape.h"
#include "Renderer.h"

#include "SimpleEntryTool.h"
#include "SimpleEntryTool.moc"

#include "dialogs/SimpleEntryWidget.h"

#include "actions/NoteEntryAction.h"
#include "actions/AccidentalAction.h"

#include "commands/AddBarsCommand.h"

#include "core/Sheet.h"
#include "core/Part.h"
#include "core/Staff.h"
#include "core/Bar.h"
#include "core/Voice.h"
#include "core/VoiceBar.h"
#include "core/Clef.h"
#include "core/StaffSystem.h"

using namespace MusicCore;

SimpleEntryTool::SimpleEntryTool( KoCanvasBase* canvas )
    : KoTool( canvas ),
    m_musicshape(0),
    m_voice(0)
{
    QActionGroup* actionGroup = new QActionGroup(this);
    connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(activeActionChanged(QAction*)));

    QAction* addBars = new QAction(KIcon("edit-add"), i18n("Add measures"), this);
    addAction("add_bars", addBars);
    connect(addBars, SIGNAL(triggered()), this, SLOT(addBars()));

    AbstractMusicAction* actionBreveNote = new NoteEntryAction(Chord::Breve, false, this);
    addAction("note_breve", actionBreveNote);
    actionGroup->addAction(actionBreveNote);

    AbstractMusicAction* actionWholeNote = new NoteEntryAction(Chord::Whole, false, this);
    addAction("note_whole", actionWholeNote);
    actionGroup->addAction(actionWholeNote);

    AbstractMusicAction* actionHalfNote = new NoteEntryAction(Chord::Half, false, this);
    addAction("note_half", actionHalfNote);
    actionGroup->addAction(actionHalfNote);

    AbstractMusicAction* actionQuarterNote = new NoteEntryAction(Chord::Quarter, false, this);
    addAction("note_quarter", actionQuarterNote);
    actionGroup->addAction(actionQuarterNote);

    AbstractMusicAction* actionNote8 = new NoteEntryAction(Chord::Eighth, false, this);
    addAction("note_eighth", actionNote8);
    actionGroup->addAction(actionNote8);

    AbstractMusicAction* actionNote16 = new NoteEntryAction(Chord::Sixteenth, false, this);
    addAction("note_16th", actionNote16);
    actionGroup->addAction(actionNote16);

    AbstractMusicAction* actionNote32 = new NoteEntryAction(Chord::ThirtySecond, false, this);
    addAction("note_32nd", actionNote32);
    actionGroup->addAction(actionNote32);

    AbstractMusicAction* actionNote64 = new NoteEntryAction(Chord::SixtyFourth, false, this);
    addAction("note_64th", actionNote64);
    actionGroup->addAction(actionNote64);

    AbstractMusicAction* actionNote128 = new NoteEntryAction(Chord::HundredTwentyEighth, false, this);
    addAction("note_128th", actionNote128);
    actionGroup->addAction(actionNote128);

    AbstractMusicAction* actionBreveRest = new NoteEntryAction(Chord::Breve, true, this);
    addAction("rest_breve", actionBreveRest);
    actionGroup->addAction(actionBreveRest);

    AbstractMusicAction* actionWholeRest = new NoteEntryAction(Chord::Whole, true, this);
    addAction("rest_whole", actionWholeRest);
    actionGroup->addAction(actionWholeRest);

    AbstractMusicAction* actionHalfRest = new NoteEntryAction(Chord::Half, true, this);
    addAction("rest_half", actionHalfRest);
    actionGroup->addAction(actionHalfRest);

    AbstractMusicAction* actionQuarterRest = new NoteEntryAction(Chord::Quarter, true, this);
    addAction("rest_quarter", actionQuarterRest);
    actionGroup->addAction(actionQuarterRest);

    AbstractMusicAction* actionRest8 = new NoteEntryAction(Chord::Eighth, true, this);
    addAction("rest_eighth", actionRest8);
    actionGroup->addAction(actionRest8);

    AbstractMusicAction* actionRest16 = new NoteEntryAction(Chord::Sixteenth, true, this);
    addAction("rest_16th", actionRest16);
    actionGroup->addAction(actionRest16);

    AbstractMusicAction* actionRest32 = new NoteEntryAction(Chord::ThirtySecond, true, this);
    addAction("rest_32nd", actionRest32);
    actionGroup->addAction(actionRest32);

    AbstractMusicAction* actionRest64 = new NoteEntryAction(Chord::SixtyFourth, true, this);
    addAction("rest_64th", actionRest64);
    actionGroup->addAction(actionRest64);

    AbstractMusicAction* actionRest128 = new NoteEntryAction(Chord::HundredTwentyEighth, true, this);
    addAction("rest_128th", actionRest128);
    actionGroup->addAction(actionRest128);

    AbstractMusicAction* action;
    action = new AccidentalAction(-2, this);
    addAction("accidental_doubleflat", action);
    actionGroup->addAction(action);

    action = new AccidentalAction(-1, this);
    addAction("accidental_flat", action);
    actionGroup->addAction(action);

    action = new AccidentalAction(0, this);
    addAction("accidental_natural", action);
    actionGroup->addAction(action);

    action = new AccidentalAction(1, this);
    addAction("accidental_sharp", action);
    actionGroup->addAction(action);

    action = new AccidentalAction(2, this);
    addAction("accidental_doublesharp", action);
    actionGroup->addAction(action);

    actionQuarterNote->setChecked(true);
    m_activeAction = actionQuarterNote;
}

SimpleEntryTool::~SimpleEntryTool()
{
}

void SimpleEntryTool::activate (bool temporary)
{
    Q_UNUSED( temporary );
    KoSelection* selection = m_canvas->shapeManager()->selection();
    foreach ( KoShape* shape, selection->selectedShapes() )
    {
        m_musicshape = dynamic_cast<MusicShape*>( shape );
        if ( m_musicshape )
            break;
    }
    if ( !m_musicshape )
    {
        emit done();
        return;
    }
    useCursor( Qt::ArrowCursor, true );
}

void SimpleEntryTool::deactivate()
{
    kDebug()<<"SimpleEntryTool::deactivate";
    m_musicshape = 0;
}

void SimpleEntryTool::paint( QPainter& painter, const KoViewConverter& viewConverter )
{
    painter.setMatrix( painter.matrix() * m_musicshape->transformationMatrix(&viewConverter) );
    KoShape::applyConversion( painter, viewConverter );
    painter.setClipRect(QRectF(QPointF(0, 0), m_musicshape->size()));

    Sheet* sheet = m_musicshape->sheet();
    for (int i = 0; i < sheet->partCount(); i++) {
        Part* p = sheet->part(i);
        if (p->voiceCount() > m_voice) {
            m_musicshape->renderer()->renderVoice(painter, p->voice(m_voice), Qt::red);
        }
    }

    m_activeAction->renderPreview(painter, m_point);
}

void SimpleEntryTool::mousePressEvent( KoPointerEvent* event )
{
    QPointF p = m_musicshape->transformationMatrix(0).inverted().map(event->point);
    Sheet *sheet = m_musicshape->sheet();

    // find closest staff system
    StaffSystem* system = 0;
    for (int i = 0; i < sheet->staffSystemCount(); i++) {
        StaffSystem* ss = sheet->staffSystem(i);
        if (ss->top() > p.y()) break;
        system = ss;
    }

    Q_ASSERT(system);
    if(system == 0)
        return;

    // find closest staff
    Staff* closestStaff = 0;
    double dist = 1e99;
    double yrel = p.y() - system->top();
    for (int prt = 0; prt < sheet->partCount(); prt++) {
        Part* part = sheet->part(prt);
        for (int st = 0; st < part->staffCount(); st++) {
            Staff* staff = part->staff(st);
            double top = staff->top();
            double bot = staff->top() + (staff->lineCount() - 1) * staff->lineSpacing();
            if (fabs(top - yrel) < dist) {
                closestStaff = staff;
                dist = fabs(top - yrel);
            }
            if (fabs(bot - yrel) < dist) {
                closestStaff = staff;
                dist = fabs(bot - yrel);
            }
        }
    }

//    int line = closestStaff->line(yrel - closestStaff->top());
//    kDebug() << "line: " << line << endl;

    Part* part = closestStaff->part();
    for (int i = part->voiceCount(); i <= m_voice; i++) {
        part->addVoice();
    }
    Voice* voice = part->voice(m_voice);

    // find correct bar
    Bar* bar = 0;
    int barIdx = -1;
    for (int b = system->firstBar(); b < sheet->barCount(); b++) {
        Bar* bb = sheet->bar(b);
        if (bb->position().x() <= p.x() && bb->position().x() + bb->size() >= p.x()) {
            bar = bb;
            barIdx = b;
            break;
        }
    }

    if (!bar) return;
/*
    Clef* clef = closestStaff->lastClefChange(barIdx, INT_MAX);

    Chord* c = new Chord(closestStaff, m_duration);
    if (clef) {
        kDebug() <<"clef:" << clef->shape();
        int pitch = clef->lineToPitch(line);
        kDebug() <<"pitch:" << pitch;
        c->addNote(closestStaff, pitch);
    }
    voice->bar(bar)->addElement(c);
    m_musicshape->engrave();
    m_musicshape->repaint();*/
    m_activeAction->mousePress(closestStaff, barIdx, QPointF(p.x() - bar->position().x(), yrel - closestStaff->top()));
}

void SimpleEntryTool::mouseMoveEvent( KoPointerEvent* event )
{
    m_point = m_musicshape->transformationMatrix(0).inverted().map(event->point);
    m_canvas->updateCanvas(QRectF(QPointF(event->point.x() - 100, event->point.y() - 100), QSizeF(200, 200)));
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

    connect(widget, SIGNAL(voiceChanged(int)), this, SLOT(voiceChanged(int)));

    return widget;
}

void SimpleEntryTool::activeActionChanged(QAction* action)
{
    m_activeAction = qobject_cast<AbstractMusicAction*>(action);
}

void SimpleEntryTool::voiceChanged(int voice)
{
    m_voice = voice;
    m_musicshape->repaint();
}

void SimpleEntryTool::addBars()
{
    bool ok;
    int barCount = QInputDialog::getInteger(NULL, "Add measures", "Add how many measures?", 1, 1, 1000, 1, &ok);
    if (!ok) return;
    addCommand(new AddBarsCommand(m_musicshape, barCount));
}

MusicShape* SimpleEntryTool::shape()
{
    return m_musicshape;
}

int SimpleEntryTool::voice()
{
    return m_voice;
}
