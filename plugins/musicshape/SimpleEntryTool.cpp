/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SimpleEntryTool.h"

#include <QAction>
#include <QActionGroup>
#include <QBuffer>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QPainter>
#include <QUrl>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <KLocalizedString>

#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include "MusicCursor.h"
#include "MusicDebug.h"
#include "MusicShape.h"
#include "Renderer.h"

#include "dialogs/SimpleEntryWidget.h"

#include "actions/AccidentalAction.h"
#include "actions/DotsAction.h"
#include "actions/EraserAction.h"
#include "actions/KeySignatureAction.h"
#include "actions/NoteEntryAction.h"
#include "actions/RemoveBarAction.h"
#include "actions/SelectionAction.h"
#include "actions/SetClefAction.h"
#include "actions/TiedNoteAction.h"
#include "actions/TimeSignatureAction.h"

#include "commands/AddBarsCommand.h"
#include "commands/AddNoteCommand.h"
#include "commands/CreateChordCommand.h"

#include "core/Bar.h"
#include "core/Clef.h"
#include "core/KeySignature.h"
#include "core/MusicXmlReader.h"
#include "core/MusicXmlWriter.h"
#include "core/Note.h"
#include "core/Part.h"
#include "core/Sheet.h"
#include "core/Staff.h"
#include "core/StaffSystem.h"
#include "core/Voice.h"
#include "core/VoiceBar.h"

#include <algorithm>

using namespace MusicCore;

SimpleEntryTool::SimpleEntryTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_musicshape(nullptr)
    , m_voice(0)
    , m_selectionStart(-1)
    , m_cursor(nullptr)
{
    QActionGroup *actionGroup = new QActionGroup(this);
    connect(actionGroup, &QActionGroup::triggered, this, &SimpleEntryTool::activeActionChanged);

    QAction *importAction = new QAction(koIcon("document-import"), i18n("Import"), this);
    addAction("import", importAction);
    connect(importAction, &QAction::triggered, this, &SimpleEntryTool::importSheet);

    QAction *exportAction = new QAction(koIcon("document-export"), i18n("Export"), this);
    addAction("export", exportAction);
    connect(exportAction, &QAction::triggered, this, &SimpleEntryTool::exportSheet);

    QAction *addBars = new QAction(koIcon("list-add"), i18n("Add measures"), this);
    addAction("add_bars", addBars);
    connect(addBars, &QAction::triggered, this, &SimpleEntryTool::addBars);

    AbstractMusicAction *actionBreveNote = new NoteEntryAction(BreveNote, false, this);
    actionBreveNote->setShortcut(Qt::Key_9);
    addAction("note_breve", actionBreveNote);
    actionGroup->addAction(actionBreveNote);

    AbstractMusicAction *actionWholeNote = new NoteEntryAction(WholeNote, false, this);
    actionWholeNote->setShortcut(Qt::Key_8);
    addAction("note_whole", actionWholeNote);
    actionGroup->addAction(actionWholeNote);

    AbstractMusicAction *actionHalfNote = new NoteEntryAction(HalfNote, false, this);
    actionHalfNote->setShortcut(Qt::Key_7);
    addAction("note_half", actionHalfNote);
    actionGroup->addAction(actionHalfNote);

    AbstractMusicAction *actionQuarterNote = new NoteEntryAction(QuarterNote, false, this);
    actionQuarterNote->setShortcut(Qt::Key_6);
    addAction("note_quarter", actionQuarterNote);
    actionGroup->addAction(actionQuarterNote);

    AbstractMusicAction *actionNote8 = new NoteEntryAction(EighthNote, false, this);
    actionNote8->setShortcut(Qt::Key_5);
    addAction("note_eighth", actionNote8);
    actionGroup->addAction(actionNote8);

    AbstractMusicAction *actionNote16 = new NoteEntryAction(SixteenthNote, false, this);
    actionNote16->setShortcut(Qt::Key_4);
    addAction("note_16th", actionNote16);
    actionGroup->addAction(actionNote16);

    AbstractMusicAction *actionNote32 = new NoteEntryAction(ThirtySecondNote, false, this);
    actionNote32->setShortcut(Qt::Key_3);
    addAction("note_32nd", actionNote32);
    actionGroup->addAction(actionNote32);

    AbstractMusicAction *actionNote64 = new NoteEntryAction(SixtyFourthNote, false, this);
    actionNote64->setShortcut(Qt::Key_2);
    addAction("note_64th", actionNote64);
    actionGroup->addAction(actionNote64);

    AbstractMusicAction *actionNote128 = new NoteEntryAction(HundredTwentyEighthNote, false, this);
    actionNote128->setShortcut(Qt::Key_1);
    addAction("note_128th", actionNote128);
    actionGroup->addAction(actionNote128);

    AbstractMusicAction *actionBreveRest = new NoteEntryAction(BreveNote, true, this);
    addAction("rest_breve", actionBreveRest);
    actionGroup->addAction(actionBreveRest);

    AbstractMusicAction *actionWholeRest = new NoteEntryAction(WholeNote, true, this);
    addAction("rest_whole", actionWholeRest);
    actionGroup->addAction(actionWholeRest);

    AbstractMusicAction *actionHalfRest = new NoteEntryAction(HalfNote, true, this);
    addAction("rest_half", actionHalfRest);
    actionGroup->addAction(actionHalfRest);

    AbstractMusicAction *actionQuarterRest = new NoteEntryAction(QuarterNote, true, this);
    addAction("rest_quarter", actionQuarterRest);
    actionGroup->addAction(actionQuarterRest);

    AbstractMusicAction *actionRest8 = new NoteEntryAction(EighthNote, true, this);
    addAction("rest_eighth", actionRest8);
    actionGroup->addAction(actionRest8);

    AbstractMusicAction *actionRest16 = new NoteEntryAction(SixteenthNote, true, this);
    addAction("rest_16th", actionRest16);
    actionGroup->addAction(actionRest16);

    AbstractMusicAction *actionRest32 = new NoteEntryAction(ThirtySecondNote, true, this);
    addAction("rest_32nd", actionRest32);
    actionGroup->addAction(actionRest32);

    AbstractMusicAction *actionRest64 = new NoteEntryAction(SixtyFourthNote, true, this);
    addAction("rest_64th", actionRest64);
    actionGroup->addAction(actionRest64);

    AbstractMusicAction *actionRest128 = new NoteEntryAction(HundredTwentyEighthNote, true, this);
    addAction("rest_128th", actionRest128);
    actionGroup->addAction(actionRest128);

    AbstractMusicAction *action;
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

    action = new EraserAction(this);
    addAction("eraser", action);
    actionGroup->addAction(action);

    action = new DotsAction(this);
    addAction("dots", action);
    actionGroup->addAction(action);

    action = new TiedNoteAction(this);
    addAction("tiednote", action);
    actionGroup->addAction(action);

    action = new SelectionAction(this);
    addAction("select", action);
    actionGroup->addAction(action);

    actionQuarterNote->setChecked(true);
    m_activeAction = actionQuarterNote;

    QMenu *clefMenu = new QMenu();
    clefMenu->addAction(action = new SetClefAction(Clef::Trebble, 2, 0, this));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    clefMenu->addAction(action = new SetClefAction(Clef::Bass, 4, 0, this));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    clefMenu->addAction(action = new SetClefAction(Clef::Alto, 3, 0, this));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    clefMenu->addAction(action = new SetClefAction(Clef::Tenor, 4, 0, this));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    clefMenu->addAction(action = new SetClefAction(Clef::Soprano, 1, 0, this));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    m_menus.append(clefMenu);

    QList<QAction *> contextMenu;

    QAction *clefAction = new QAction(i18n("Clef"), this);
    clefAction->setMenu(clefMenu);
    contextMenu.append(clefAction);

    QMenu *tsMenu = new QMenu();
    tsMenu->addAction(action = new TimeSignatureAction(this, 2, 2));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 2, 4));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 3, 4));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 4, 4));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 5, 4));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 3, 8));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 5, 8));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 6, 8));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 7, 8));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 9, 8));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    tsMenu->addAction(action = new TimeSignatureAction(this, 12, 8));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    m_menus.append(tsMenu);

    QAction *timeSigAction = new QAction(i18n("Time signature"), this);
    timeSigAction->setMenu(tsMenu);
    contextMenu.append(timeSigAction);

    QMenu *ksMenu = new QMenu();
    ksMenu->addAction(action = new KeySignatureAction(this, 0));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addSeparator();
    ksMenu->addAction(action = new KeySignatureAction(this, 1));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addAction(action = new KeySignatureAction(this, 2));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addAction(action = new KeySignatureAction(this, 3));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addAction(action = new KeySignatureAction(this, 4));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addSeparator();
    ksMenu->addAction(action = new KeySignatureAction(this, -1));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addAction(action = new KeySignatureAction(this, -2));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addAction(action = new KeySignatureAction(this, -3));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addAction(action = new KeySignatureAction(this, -4));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    ksMenu->addSeparator();
    ksMenu->addAction(action = new KeySignatureAction(this));
    connect(action, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    m_menus.append(ksMenu);

    QAction *keySigAction = new QAction(i18n("Key signature"), this);
    keySigAction->setMenu(ksMenu);
    contextMenu.append(keySigAction);

    QAction *removeBarAction = new RemoveBarAction(this);
    connect(removeBarAction, &QAction::triggered, this, &SimpleEntryTool::actionTriggered);
    contextMenu.append(removeBarAction);

    setPopupActionList(contextMenu);
}

SimpleEntryTool::~SimpleEntryTool()
{
    qDeleteAll(m_menus);
}

void SimpleEntryTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);
    foreach (KoShape *shape, shapes) {
        m_musicshape = dynamic_cast<MusicShape *>(shape);
        if (m_musicshape) {
            // TODO get the cursor that was used the last time for that sheet from some map
            m_cursor = new MusicCursor(m_musicshape->sheet(), m_musicshape->sheet());
            break;
        }
    }
    if (!m_musicshape) {
        Q_EMIT done();
        return;
    }
    useCursor(Qt::ArrowCursor);
}

void SimpleEntryTool::deactivate()
{
    // debugMusic<<"SimpleEntryTool::deactivate";
    m_musicshape = nullptr;
    m_cursor = nullptr;
}

void SimpleEntryTool::paint(QPainter &painter, const KoViewConverter &viewConverter)
{
    Sheet *sheet = m_musicshape->sheet();
    int firstSystem = m_musicshape->firstSystem();
    int lastSystem = m_musicshape->lastSystem();
    int firstBar = sheet->staffSystem(firstSystem)->firstBar();
    int lastBar = INT_MAX;
    if (lastSystem < sheet->staffSystemCount() - 1) {
        lastBar = sheet->staffSystem(lastSystem + 1)->firstBar() - 1;
    }

    // somehow check for selections
    if (m_selectionStart >= 0) {
        // find first shape
        MusicShape *shape = m_musicshape;
        while (shape->predecessor())
            shape = shape->predecessor();

        // now loop over all shapes
        while (shape) {
            painter.save();
            painter.setTransform(shape->absoluteTransformation(&viewConverter) * painter.transform());
            KoShape::applyConversion(painter, viewConverter);
            painter.setClipRect(QRectF(QPointF(0, 0), shape->size()), Qt::IntersectClip);

            for (int b = qMax(shape->firstBar(), m_selectionStart); b <= m_selectionEnd && b < sheet->barCount() && b <= shape->lastBar(); b++) {
                Bar *bar = sheet->bar(b);
                bool selectedStaff = false;
                for (int p = 0; p < sheet->partCount(); p++) {
                    Part *part = sheet->part(p);
                    for (int s = 0; s < part->staffCount(); s++) {
                        Staff *staff = part->staff(s);
                        if (staff == m_selectionStaffStart) {
                            selectedStaff = true;
                        }
                        if (selectedStaff) {
                            QPointF p1 = bar->position() + QPointF(0, staff->top());
                            QPointF p2 = QPointF(p1.x() + bar->size(), p1.y() + (staff->lineCount() - 1) * staff->lineSpacing());
                            painter.setBrush(QBrush(Qt::yellow));
                            painter.setPen(Qt::NoPen);
                            painter.drawRect(QRectF(p1, p2));
                        }
                        if (staff == m_selectionStaffEnd) {
                            selectedStaff = false;
                        }
                    }
                }
            }
            for (int p = 0; p < sheet->partCount(); p++) {
                Part *part = sheet->part(p);
                shape->renderer()->renderPart(painter, part, qMax(shape->firstBar(), m_selectionStart), qMin(shape->lastBar(), m_selectionEnd), Qt::black);
            }
            shape = shape->successor();
            painter.restore();
        }
    }

    painter.setTransform(m_musicshape->absoluteTransformation(&viewConverter) * painter.transform());
    KoShape::applyConversion(painter, viewConverter);
    painter.setClipRect(QRectF(QPointF(0, 0), m_musicshape->size()), Qt::IntersectClip);

    if (m_activeAction->isVoiceAware()) {
        for (int i = 0; i < sheet->partCount(); i++) {
            Part *p = sheet->part(i);
            if (p->voiceCount() > m_voice) {
                m_musicshape->renderer()->renderVoice(painter, p->voice(m_voice), firstBar, lastBar, Qt::red);
            }
        }
    }

    // draw cursor
    if (m_cursor) {
        m_activeAction->renderKeyboardPreview(painter, *m_cursor);
    }

    m_activeAction->renderPreview(painter, m_point);
}

void SimpleEntryTool::mousePressEvent(KoPointerEvent *event)
{
    if (!m_musicshape->boundingRect().contains(event->point)) {
        QRectF area(event->point, QSizeF(1, 1));
        foreach (KoShape *shape, canvas()->shapeManager()->shapesAt(area, true)) {
            MusicShape *musicshape = dynamic_cast<MusicShape *>(shape);
            if (musicshape) {
                m_musicshape->update();
                m_musicshape = musicshape;
                m_musicshape->update();
                break; // stop looking.
            }
        }
    }

    QPointF p = m_musicshape->absoluteTransformation(nullptr).inverted().map(event->point);
    Sheet *sheet = m_musicshape->sheet();

    p.setY(p.y() + sheet->staffSystem(m_musicshape->firstSystem())->top());

    // debugMusic << "pos:" << p;
    //  find closest staff system
    StaffSystem *system = nullptr;
    for (int i = m_musicshape->firstSystem(); i <= m_musicshape->lastSystem() && i < sheet->staffSystemCount(); i++) {
        StaffSystem *ss = sheet->staffSystem(i);
        // debugMusic << "system" << i << "has top" << ss->top();
        if (ss->top() > p.y())
            break;
        system = ss;
    }

    if (system == nullptr) {
        // debugMusic << "no staff system found";
        return;
    }

    // find closest staff
    Staff *closestStaff = nullptr;
    qreal dist = 1e99;
    qreal yrel = p.y() - system->top();
    for (int prt = 0; prt < sheet->partCount(); prt++) {
        Part *part = sheet->part(prt);
        for (int st = 0; st < part->staffCount(); st++) {
            Staff *staff = part->staff(st);
            qreal top = staff->top();
            qreal bot = staff->top() + (staff->lineCount() - 1) * staff->lineSpacing();
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
    //    debugMusic << "line: " << line;

    Part *part = closestStaff->part();
    for (int i = part->voiceCount(); i <= m_voice; i++) {
        part->addVoice();
    }

    // find correct bar
    Bar *bar = nullptr;
    int barIdx = -1;
    bool inPrefix = false;
    for (int b = system->firstBar(); b < sheet->barCount(); b++) {
        Bar *bb = sheet->bar(b);
        if (bb->position().x() <= p.x() && bb->position().x() + bb->size() >= p.x()) {
            bar = bb;
            barIdx = b;
            break;
        }
        if (bb->prefixPosition().x() <= p.x() && bb->prefixPosition().x() + bb->prefix() >= p.x()) {
            bar = bb;
            barIdx = b;
            inPrefix = true;
            break;
        }
    }

    foreach (QAction *a, popupActionList()) {
        a->setVisible(bar);
    }

    if (!bar)
        return;

    QPointF point;
    if (inPrefix) {
        point = QPointF(p.x() - bar->prefixPosition().x() - bar->prefix(), yrel - closestStaff->top());
    } else {
        point = QPointF(p.x() - bar->position().x(), yrel - closestStaff->top());
    }

    if (event->button() == Qt::RightButton) {
        m_contextMenuStaff = closestStaff;
        m_contextMenuBar = barIdx;
        m_contextMenuPoint = point;
        event->ignore();
    } else {
        m_activeAction->mousePress(closestStaff, barIdx, point);
        event->accept();
    }
}

void SimpleEntryTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (!m_musicshape->boundingRect().contains(event->point)) {
        QRectF area(event->point, QSizeF(1, 1));
        foreach (KoShape *shape, canvas()->shapeManager()->shapesAt(area, true)) {
            MusicShape *musicshape = dynamic_cast<MusicShape *>(shape);
            if (musicshape) {
                if (musicshape->sheet() == m_musicshape->sheet() || !event->buttons()) {
                    m_musicshape->update();
                    m_musicshape = musicshape;
                    m_musicshape->update();
                    break; // stop looking.
                }
            }
        }
    }

    m_point = m_musicshape->absoluteTransformation(nullptr).inverted().map(event->point);
    canvas()->updateCanvas(QRectF(QPointF(event->point.x() - 100, event->point.y() - 100), QSizeF(200, 200)));
    if (event->buttons()) {
        QPointF p = m_musicshape->absoluteTransformation(nullptr).inverted().map(event->point);
        Sheet *sheet = m_musicshape->sheet();

        p.setY(p.y() + sheet->staffSystem(m_musicshape->firstSystem())->top());
        // find closest staff system
        StaffSystem *system = nullptr;
        for (int i = m_musicshape->firstSystem(); i <= m_musicshape->lastSystem() && i < sheet->staffSystemCount(); i++) {
            StaffSystem *ss = sheet->staffSystem(i);
            if (ss->top() > p.y())
                break;
            system = ss;
        }

        if (system == nullptr) {
            return;
        }

        // find closest staff
        Staff *closestStaff = nullptr;
        qreal dist = 1e99;
        qreal yrel = p.y() - system->top();
        for (int prt = 0; prt < sheet->partCount(); prt++) {
            Part *part = sheet->part(prt);
            for (int st = 0; st < part->staffCount(); st++) {
                Staff *staff = part->staff(st);
                qreal top = staff->top();
                qreal bot = staff->top() + (staff->lineCount() - 1) * staff->lineSpacing();
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
        //    debugMusic << "line: " << line;

        Part *part = closestStaff->part();
        for (int i = part->voiceCount(); i <= m_voice; i++) {
            part->addVoice();
        }

        // find correct bar
        Bar *bar = nullptr;
        int barIdx = -1;
        bool inPrefix = false;
        for (int b = system->firstBar(); b < sheet->barCount(); b++) {
            Bar *bb = sheet->bar(b);
            if (bb->position().x() <= p.x() && bb->position().x() + bb->size() >= p.x()) {
                bar = bb;
                barIdx = b;
                break;
            }
            if (bb->prefixPosition().x() <= p.x() && bb->prefixPosition().x() + bb->prefix() >= p.x()) {
                bar = bb;
                barIdx = b;
                inPrefix = true;
                break;
            }
        }

        if (!bar)
            return;

        QPointF point;
        if (inPrefix) {
            point = QPointF(p.x() - bar->prefixPosition().x() - bar->prefix(), yrel - closestStaff->top());
        } else {
            point = QPointF(p.x() - bar->position().x(), yrel - closestStaff->top());
        }

        m_activeAction->mouseMove(closestStaff, barIdx, point);
    }
}

void SimpleEntryTool::mouseReleaseEvent(KoPointerEvent *)
{
}

void SimpleEntryTool::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
    m_activeAction->keyPress(event, *m_cursor);
    if (!event->isAccepted()) {
        event->accept();
        switch (event->key()) {
        case Qt::Key_Left:
            m_cursor->moveLeft();
            m_musicshape->update();
            break;
        case Qt::Key_Right:
            m_cursor->moveRight();
            m_musicshape->update();
            break;
        case Qt::Key_Up:
            m_cursor->moveUp();
            m_musicshape->update();
            break;
        case Qt::Key_Down:
            m_cursor->moveDown();
            m_musicshape->update();
            break;
        default:
            event->ignore();
        }
    }
}

void SimpleEntryTool::addCommand(KUndo2Command *command)
{
    canvas()->addCommand(command);
}

QWidget *SimpleEntryTool::createOptionWidget()
{
    SimpleEntryWidget *widget = new SimpleEntryWidget(this);

    connect(widget, &SimpleEntryWidget::voiceChanged, this, &SimpleEntryTool::voiceChanged);

    return widget;
}

void SimpleEntryTool::activeActionChanged(QAction *action)
{
    bool oldVoiceAware = m_activeAction->isVoiceAware();
    Q_UNUSED(oldVoiceAware);
    m_activeAction = qobject_cast<AbstractMusicAction *>(action);
    // if (m_activeAction->isVoiceAware() != oldVoiceAware) {
    m_musicshape->update();
    //    reinterpret_cast<SimpleEntryWidget*>(optionWidget())->setVoiceListEnabled(m_activeAction->isVoiceAware());
    //}
}

void SimpleEntryTool::voiceChanged(int voice)
{
    m_cursor->setVoice(voice);
    m_voice = voice;
    m_musicshape->update();
}

void SimpleEntryTool::addBars()
{
    bool ok;
    int barCount = QInputDialog::getInt(nullptr, i18n("Add measures"), i18n("Add how many measures?"), 1, 1, 1000, 1, &ok);
    if (!ok)
        return;
    addCommand(new AddBarsCommand(m_musicshape, barCount));
}

void SimpleEntryTool::actionTriggered()
{
    AbstractMusicAction *action = dynamic_cast<AbstractMusicAction *>(sender());
    if (!action)
        return;
    action->mousePress(m_contextMenuStaff, m_contextMenuBar, m_contextMenuPoint);
}

MusicShape *SimpleEntryTool::shape()
{
    return m_musicshape;
}

int SimpleEntryTool::voice()
{
    return m_voice;
}

void SimpleEntryTool::setSelection(int firstBar, int lastBar, Staff *startStaff, Staff *endStaff)
{
    // debugMusic << "firstBar:" << firstBar << "lastBar:" << lastBar;
    m_selectionStart = firstBar;
    m_selectionEnd = lastBar;
    m_selectionStaffStart = startStaff;
    m_selectionStaffEnd = endStaff;
    bool foundEnd = false;
    Sheet *sheet = m_musicshape->sheet();
    for (int p = 0; p < sheet->partCount(); p++) {
        Part *part = sheet->part(p);
        for (int s = 0; s < part->staffCount(); s++) {
            Staff *staff = part->staff(s);
            if (staff == m_selectionStaffStart) {
                if (foundEnd) {
                    std::swap(m_selectionStaffStart, m_selectionStaffEnd);
                }
                break;
            }
            if (staff == m_selectionStaffEnd) {
                foundEnd = true;
            }
        }
    }
    MusicShape *shape = m_musicshape;
    while (shape) {
        shape->update();
        shape = shape->predecessor();
    }
    shape = m_musicshape->successor();
    while (shape) {
        shape->update();
        shape = shape->successor();
    }
}

void SimpleEntryTool::importSheet()
{
    QString file = QFileDialog::getOpenFileName(nullptr, i18nc("@title:window", "Import"), QString(), i18n("MusicXML files (*.xml)"));
    if (file.isEmpty() || file.isNull())
        return;
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    KoXmlDocument doc;
    KoXml::setDocument(doc, &f, true);
    KoXmlElement e = doc.documentElement();
    // debugMusic << e.localName() << e.nodeName();
    Sheet *sheet = MusicXmlReader(nullptr).loadSheet(doc.documentElement());
    if (sheet) {
        m_musicshape->setSheet(sheet, 0);
        m_musicshape->update();
    }
}

void SimpleEntryTool::exportSheet()
{
    QString file = QFileDialog::getSaveFileName(nullptr, i18nc("@title:window", "Export"), QString(), i18n("MusicXML files (*.xml)"));
    if (file.isEmpty() || file.isNull())
        return;

    QBuffer b;
    b.open(QIODevice::ReadWrite);
    KoXmlWriter kw(&b);
    kw.startDocument("score-partwise", "-//Recordare//DTD MusicXML 2.0 Partwise//EN", "http://www.musicxml.org/dtds/partwise.dtd");
    MusicXmlWriter().writeSheet(kw, m_musicshape->sheet(), true);
    kw.endDocument();

    b.seek(0);

    // debugMusic << b.data();
    QFile f(file);
    f.open(QIODevice::WriteOnly);
    QXmlStreamWriter w(&f);

    QXmlStreamReader xml(&b);
    while (!xml.atEnd()) {
        xml.readNext();
        // debugMusic << xml.tokenType() << xml.tokenString();
        // debugMusic << xml.error() << xml.errorString();
        if (xml.isCDATA()) {
            w.writeCDATA(xml.text().toString());
        } else if (xml.isCharacters()) {
            w.writeCharacters(xml.text().toString());
        } else if (xml.isComment()) {
            w.writeComment(xml.text().toString());
        } else if (xml.isDTD()) {
            w.writeDTD(xml.text().toString());
        } else if (xml.isEndDocument()) {
            w.writeEndDocument();
        } else if (xml.isEndElement()) {
            w.writeEndElement();
        } else if (xml.isEntityReference()) {
            w.writeEntityReference(xml.name().toString());
        } else if (xml.isProcessingInstruction()) {
            w.writeProcessingInstruction(xml.processingInstructionTarget().toString(), xml.processingInstructionData().toString());
        } else if (xml.isStartDocument()) {
            w.writeStartDocument();
        } else if (xml.isStartElement()) {
            w.writeStartElement(xml.name().toString());
            QXmlStreamAttributes attr = xml.attributes();
            for (int a = 0; a < attr.count(); a++) {
                w.writeAttribute(attr[a].name().toString(), attr[a].value().toString());
            }
        }
    }
}
