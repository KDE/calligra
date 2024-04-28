/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLEENTRY_TOOL
#define SIMPLEENTRY_TOOL

#include "core/Chord.h"
#include <KoToolBase.h>
#include <QPointF>

class MusicShape;
class KUndo2Command;
class AbstractMusicAction;
class MusicCursor;
class QMenu;

namespace MusicCore
{
class Staff;
}

/**
 * Tool that provides functionality to insert/remove notes/rests. Named after Finale's Simple Entry tool.
 */
class SimpleEntryTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit SimpleEntryTool(KoCanvasBase *canvas);
    ~SimpleEntryTool() override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;

    void addCommand(KUndo2Command *command);

    MusicShape *shape();
    int voice();

    void setSelection(int startBar, int endBar, MusicCore::Staff *startStaff, MusicCore::Staff *endStaff);

protected:
    QWidget *createOptionWidget() override;
protected Q_SLOTS:
    void activeActionChanged(QAction *action);
    void voiceChanged(int voice);
    void addBars();
    void actionTriggered();
    void importSheet();
    void exportSheet();

private:
    MusicShape *m_musicshape;
    AbstractMusicAction *m_activeAction;
    QPointF m_point;
    int m_voice;

    MusicCore::Staff *m_contextMenuStaff;
    int m_contextMenuBar;
    QPointF m_contextMenuPoint;

    int m_selectionStart, m_selectionEnd;
    MusicCore::Staff *m_selectionStaffStart, *m_selectionStaffEnd;

    MusicCursor *m_cursor;
    QList<QMenu *> m_menus;
};

#endif
