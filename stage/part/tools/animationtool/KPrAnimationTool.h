/* This file is part of the KDE project

   Copyright (C) 2008 C. Boemann <cbo@boemann.dk>
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

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
class KoShapeManager;
class KPrAnimateMotion;
class KPrShapeAnimationDocker;

/**
 * The animation tool (associated with the clapperboard icon) is the tool in Stage where the user
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
    explicit KPrAnimationTool(KoCanvasBase *canvas);
    ~KPrAnimationTool() override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;

    void mousePressEvent(KoPointerEvent *event) override;

    void repaintDecorations() override;

public Q_SLOTS:
    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) override;
    void deactivate() override;

private Q_SLOTS:

    /**
      * @brief Reload motion path shapes (For all shapes with motion paths on current page)
      */
    void reloadMotionPaths();

    /**
      * @brief Verify if a motion path shape has changed (to update the canvas)
      */
    void verifyMotionPathChanged(KoShape *shape);

protected:
    QRectF handlesSize();

    QList<QPointer<QWidget> > createOptionWidgets() override;

    /**
      * @brief Load motion path shapes (For all shapes with motion paths on current page)
      */
    void initMotionPathShapes();

    /**
      * @brief Add motion path shape to the motion path shapes manager
      */
    virtual void addPathShape(KoPathShape *pathShape);

    /**
      * @brief Helper method to get the current page size
      */
    QSizeF getPageSize();

    /**
      * @brief remove motion paths of motion path manager and helper maps
      */
    void cleanMotionPathManager();

private:
    KoPathShape *m_currentMotionPathSelected;
    QMap<KoPathShape *, KPrAnimateMotion *> m_animateMotionMap;
    QMap<KoPathShape *, KoShape *> m_shapesMap;
    KoShapeManager *m_pathShapeManager;
    bool m_initializeTool;
    KPrShapeAnimationDocker *m_shapeAnimationWidget;
};

#endif
