/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRVIEW_H
#define KPRVIEW_H

#include <QObject>

#include <KoView.h>
#include <KoZoomHandler.h>

class KoCanvasController;
class KPrCanvas;
class KPrDocument;

class KPrView : public KoView
{
    Q_OBJECT
public:
    KPrView( KPrDocument * document, QWidget * parent = 0 );
    ~KPrView();

    void updateReadWrite( bool readwrite );

    KoViewConverter * viewConverter() { return &m_zoomHandler; }

    KPrCanvas * kprcanvas() { return m_canvas; }

private:    
    void initGUI();
    void initActions();

    KPrDocument * m_doc;
    KPrCanvas * m_canvas;
    KoCanvasController * m_canvasController;
    KoZoomHandler m_zoomHandler;
};

#endif /* KPRVIEW_H */
