/* This file is part of the KDE project

   Copyright (C) 2008 Casper Boemann <cbr@boemann.dk>

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

#ifndef KPRANIMATIONTOOL_H
#define KPRANIMATIONTOOL_H

#include <KoToolBase.h>

/**
 * The animation tool (associated with the clapperboard icon) is the tool in KPresenter where the user
 * animates shapes and sets up slide transitions.
 */
class KPrAnimationTool : public KoToolBase
{
    Q_OBJECT
public:
    /**
     * Constructor for animation tool
     * @param canvas the canvas this tool will be working for.
     */
    explicit KPrAnimationTool( KoCanvasBase *canvas );
    virtual ~KPrAnimationTool();

public:

    virtual void paint( QPainter &painter, const KoViewConverter &converter );

public slots:
    void activate(bool temporary = false);

public: // Events

    virtual void mousePressEvent( KoPointerEvent *event );
    virtual void mouseMoveEvent( KoPointerEvent *event );
    virtual void mouseReleaseEvent( KoPointerEvent *event );
//    virtual void mouseDoubleClickEvent( KoPointerEvent *event );

//    virtual void keyPressEvent(QKeyEvent *event);

protected:
    QMap<QString, QWidget *> createOptionWidgets();

private:
};

#endif
