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

#include <KoPathTool.h>
#include <QMap>

class KoPathShape;
class KoSelection;
class KPrAnimateMotion;

/**
 * The animation tool (associated with the clapperboard icon) is the tool in KPresenter where the user
 * animates shapes and sets up slide transitions.
 */
class KPrAnimationTool : public KoPathTool
{
    Q_OBJECT
public:
    /**
     * Constructor for animation tool
     * @param canvas the canvas this tool will be working for.
     */
    explicit KPrAnimationTool( KoCanvasBase *canvas );
    virtual ~KPrAnimationTool();

    virtual void paint( QPainter &painter, const KoViewConverter &converter );

    virtual void mousePressEvent( KoPointerEvent *event );

    void repaintDecorations();

public slots:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    virtual void deactivate();

private slots:
    void saveMotionPath();
    void reloadMotionPaths();
    void verifyMotionPathChanged(KoShape *shape);

protected:
    QRectF handlesSize();

    virtual QList<QWidget *> createOptionWidgets();

    void loadMotionPathShapes();

    virtual void addPathShape(KoPathShape *pathShape);

    virtual void paintPath(KoPathShape& pathShape, QPainter &painter, const KoViewConverter &converter);

    QPair<qreal, qreal> getScaleCorrection();

private:
    QList<KoPathShape *>m_motionPaths;
    KoPathShape *m_currentMotionPathSelected;
    QMap<KoPathShape *, KPrAnimateMotion *> m_pathMap;
    QMap<KoPathShape *, KoShape *> m_shapesMap;
    bool m_reloadMotionPaths;
};

#endif
