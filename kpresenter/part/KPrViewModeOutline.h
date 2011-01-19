/* This file is part of the KDE project
* Copyright (C) 2010 Ludovic Delfau <ludovicdelfau@gmail.com>
* Copyright (C) 2010 Julien Desgats <julien.desgats@gmail.com>
* Copyright (C) 2010-2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
* Copyright (C) 2010-2011 Benjamin Port <port.benjamin@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRVIEWMODEOUTLINE_H
#define KPRVIEWMODEOUTLINE_H

#include <KoPAView.h>
#include <KoPACanvas.h>
#include <KoPAViewMode.h>

#include "pagelayout/KPrPlaceholders.h"

#include <QTextEdit>
#include <QTextTableCell>

class KPrOutlineEditor;
class QKeyEvent;
class QTextFrame;

enum PlaceholderType;
/**
* @brief View for outline mode.
*/
class KPrViewModeOutline : public KoPAViewMode {

    Q_OBJECT;

    public:
        struct FrameData {
            QTextDocument * textDocument;
            int             numSlide;
            PlaceholderType type;
        };

        KPrViewModeOutline( KoPAView * view, KoPACanvas * canvas );
        virtual ~KPrViewModeOutline();

        void paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect);
        void paintEvent( KoPACanvas * canvas, QPaintEvent* event );
        void tabletEvent( QTabletEvent *event, const QPointF &point );
        void mousePressEvent( QMouseEvent *event, const QPointF &point );
        void mouseDoubleClickEvent( QMouseEvent *event, const QPointF &point );
        void mouseMoveEvent( QMouseEvent *event, const QPointF &point );
        void mouseReleaseEvent( QMouseEvent *event, const QPointF &point );
        void keyPressEvent( QKeyEvent *event );
        void keyReleaseEvent( QKeyEvent *event );
        void wheelEvent( QWheelEvent * event, const QPointF &point );

        void activate(KoPAViewMode *previousViewMode);
        void deactivate();

        void enableSync();
        void disableSync();
        /**
        * (Un)indents current line or selection.
        * @param  indent  true to indent, false to unindent
        * @return true if items has been (un)indented, false otherwise
        */
        bool indent(bool indentation = true);
        QTextFrame * currentFrame();
        void deleteSlide();
        void addSlide();
        void placeholderSwitch();
        /**
        * Try to put editor's cursor on the frame that corresponds to given description.
        * @param  slide    slide number
        * @param  type     shape's class
        * @param  atBegin  if true cursot will be on first frame position, otherwise on last position
        */

        /**
        * Fills the editor with presentation data
        */
        void setCursorTo(int slide, PlaceholderType type, bool atBegin=true);
        void populate();
        QMap<QTextFrame*, FrameData> link();
    public slots:
        virtual void updateActivePage( KoPAPageBase * page );
        /**
        * @brief Synchronize the editor with shapes.
        * @see QTextDocument::contentsChange
        */
        void synchronize(int position, int charsRemoved, int charsAdded);

        void slotSelectionChanged();
        /**
        * Inserts the shape's document into text editor
        */
        void insertText(QTextDocument* sourceShape, QTextFrame* destDocument, QTextCharFormat *charFormat);

    private:
        /**
        * @brief The outline editor.
        */
        KPrOutlineEditor * m_editor;


        QTextFrameFormat m_titleFrameFormat;
        QTextFrameFormat m_defaultFrameFormat;
        QTextCharFormat  m_titleCharFormat;
        QTextCharFormat  m_defaultCharFormat;
        QTextListFormat  m_listFormat;


        /**
        * @brief Link between frame in the editor and document in the shape.
        */
        QMap<QTextFrame*, FrameData> m_link;

};

#endif // KPRVIEWMODEOUTLINE_H
