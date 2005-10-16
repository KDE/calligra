/* This file is part of the KDE project
   Copyright (C) 2005, Gary Cramblitt <garycramblitt@comcast.net>

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

/*
    Note: This code is not yet integrated into KOffice.  To test, add the following
    line to koffice/lib/kofficecore/koMainWindow.cc
        new KPanelKbdSizer(this, "mw-panelSizer");
    just below the line
        setCentralWidget( d->m_splitter );
    in KoMainWindow constructor.  Then "make install" in koffice/lib/kofficecore.
*/

#ifndef __KPANELKBDSIZER_H__
#define __KPANELKBDSIZER_H__

// Qt includes.
#include <qobject.h>

// KOffice includes.
#include <koffice_export.h>

class KPanelKbdSizerPrivate;
class QWidgetList;
class KMainWindow;

class KOFFICECORE_EXPORT KPanelKbdSizer : public QObject
{
    // TODO: A .moc isn't really needed right now, but see TODO in eventFilter method.
    // Q_OBJECT

    public:
        KPanelKbdSizer(KMainWindow* parent = 0, const char* name = 0);
        virtual ~KPanelKbdSizer();

    protected:
        bool eventFilter( QObject *o, QEvent *e );

        // Retrieves a list of all Splitter and DockArea widgets in the application.
        QWidgetList* getAllPanels();
        // Advances to the next Panel handle.  If not currently in resizing mode,
        // turns it on.
        void nextHandle();
        // Moves to the previous Panel handle.  If not currently in resizing mode,
        // turns it on.
        void prevHandle();
        // Exits Sizing mode.
        void exitSizing();
        // Moves panel handle based on key pressed.
        void resizePanel(int key, int state);
        // Displays the sizer icon.
        void showIcon();
        // Hides the sizer icon.
        void hideIcon();

    private:
        KPanelKbdSizerPrivate* d;
};

#endif              // __KPANELKBDSIZER_H__
