/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#ifndef _KARBONGRADIENTTOOL_H_
#define _KARBONGRADIENTTOOL_H_

#include <KoTool.h>

class GradientStrategy;

/**
 * A tool for editing gradient backgrounds of shapes.
 * The gradients can be edited by moving gradient
 * handles directly on the canvas.
 */
class KarbonGradientTool : public KoTool
{
    Q_OBJECT
public:
    explicit KarbonGradientTool(KoCanvasBase *canvas);
    ~KarbonGradientTool();

    void paint( QPainter &painter, KoViewConverter &converter );
    void repaintDecorations();

    void mousePressEvent( KoPointerEvent *event ) ;
    void mouseMoveEvent( KoPointerEvent *event );
    void mouseReleaseEvent( KoPointerEvent *event );
    void keyPressEvent(QKeyEvent *event);

    void activate (bool temporary=false);
    void deactivate();

private slots:
    virtual void resourceChanged( KoCanvasResource::EnumCanvasResource key, const QVariant & res );

private:

    QList<GradientStrategy*> m_gradients; ///< the list of editing strategies, one for each shape
    GradientStrategy* m_currentStrategy;  ///< the current editing strategy
};

#endif // _KARBONGRADIENTTOOL_H_
