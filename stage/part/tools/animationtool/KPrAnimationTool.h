/* This file is part of the KDE project

   Copyright (C) 2008 C. Boemann <cbo@boemann.dk>

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

#include <KoCreatePathTool.h>
#include <QMap>
#include <animations/KPrShapeAnimation.h>

/**
 * The animation tool (associated with the clapperboard icon) is the tool in KPresenter where the user
 * animates shapes and sets up slide transitions.
 */
class KPrAnimationTool : public KoCreatePathTool
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
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);

public: // Events

    virtual void mousePressEvent( KoPointerEvent *event );
    //virtual void mouseMoveEvent( KoPointerEvent *event );
    //virtual void mouseReleaseEvent( KoPointerEvent *event );
//    virtual void mouseDoubleClickEvent( KoPointerEvent *event );

//    virtual void keyPressEvent(QKeyEvent *event);
    void repaintDecorations();

protected:
    QRectF handlesSize();

    virtual QList<QWidget *> createOptionWidgets();

    void loadMotionPathShapes();

    virtual void addPathShape(KoPathShape *pathShape);

    virtual void paintPath(KoPathShape& pathShape, QPainter &painter, const KoViewConverter &converter);

private:

    QList<KoPathShape *>m_motionPaths;
    QMap<KPrShapeAnimation *, KoPathShape *> m_motionP;
    KPrShapeAnimation *currentAnimation;
    KoPathShape *currentPath;
};

#endif
