/* This file is part of the KDE project
   Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef kwgui_h
#define kwgui_h

#include <khbox.h>
#include <KoUnit.h>

class KWView;
class KWDocStruct;
class KWCanvas;
class KWDocument;

class KoRuler;
class KoCanvasController;
class KoTabChooser;

class QSplitter;

/******************************************************************/
/* Class: KWGUI                                                   */
/******************************************************************/

class KWGUI : public KHBox
{
    Q_OBJECT

public:
    KWGUI( const QString& viewMode, QWidget *parent, KWView *view );

    void showGUI();

    KWView *getView()const { return m_view; }
    KWCanvas *canvasWidget()const { return m_canvas; }
    KoRuler *getVertRuler()const { return m_vertRuler; }
    KoRuler *getHorzRuler()const { return m_horRuler; }
    KoTabChooser *getTabChooser()const { return m_tabChooser; }
    KWDocStruct *getDocStruct()const { return m_docStruct; }
    int visibleWidth() const;
    int visibleHeight() const;

public slots:
    void reorganize();

protected slots:
    void unitChanged( KoUnit::Unit );

protected:
    friend class KWView;
    void resizeEvent( QResizeEvent *e );

    // A pointer to the view.
    KWView          *m_view;

    // The left side
    KWDocStruct     *m_docStruct;

    // The right side
    QWidget         *m_right;	// The layout widget.

    KoTabChooser    *m_tabChooser;
    KoRuler         *m_vertRuler;
    KoRuler         *m_horRuler;
    KWCanvas        *m_canvas;

    QSplitter       *m_panner;
    KoCanvasController    *m_canvasView;
};

#endif
