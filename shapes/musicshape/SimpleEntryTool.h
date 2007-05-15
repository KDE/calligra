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
#ifndef SIMPLEENTRY_TOOL
#define SIMPLEENTRY_TOOL

#include <QPointF>
#include <KoTool.h>
#include "core/Chord.h"

class MusicShape;
class QUndoCommand;
class QAction;

/**
 * Tool that provides functionality to insert/remove notes/rests. Named after Finale's Simple Entry tool.
 */
class SimpleEntryTool : public KoTool
{
    Q_OBJECT
    public:
        explicit SimpleEntryTool( KoCanvasBase* canvas );
        ~SimpleEntryTool();
        
        virtual void paint( QPainter& painter, KoViewConverter& converter );
        
        virtual void mousePressEvent( KoPointerEvent* event ) ;
        virtual void mouseMoveEvent( KoPointerEvent* event );
        virtual void mouseReleaseEvent( KoPointerEvent* event );
        
        void activate (bool temporary=false);
        void deactivate();
        
        void addCommand(QUndoCommand* command);
    protected:
        /*
         * Create default option widget
         */
        virtual QWidget * createOptionWidget();
        
    protected slots:
        void noteLengthChanged(QAction* action);
    private:
        MusicShape *m_musicshape;
        QAction *m_actionBreveNote;
        QAction *m_actionWholeNote;
        QAction *m_actionHalfNote;
        QAction *m_actionQuarterNote;
        QAction *m_actionNote8;
        QAction *m_actionNote16;
        QAction *m_actionNote32;
        QAction *m_actionNote64;
        QAction *m_actionNote128;
        MusicCore::Chord::Duration m_duration;
        QPointF m_point;
};

#endif
