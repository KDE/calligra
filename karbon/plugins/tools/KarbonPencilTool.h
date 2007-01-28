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

#ifndef _KARBONPENCILTOOL_H_
#define _KARBONPENCILTOOL_H_

#include <KoTool.h>

class KoPathShape;

class KarbonPencilTool : public KoTool
{
    Q_OBJECT
public:
    explicit KarbonPencilTool(KoCanvasBase *canvas);
    ~KarbonPencilTool();

    void paint( QPainter &painter, KoViewConverter &converter );
    void repaintDecorations();

    void mousePressEvent( KoPointerEvent *event ) ;
    void mouseMoveEvent( KoPointerEvent *event );
    void mouseReleaseEvent( KoPointerEvent *event );
    void keyPressEvent(QKeyEvent *event);

    void activate (bool temporary=false);
    void deactivate();

protected:
    virtual QWidget * createOptionWidget();

private slots:
    virtual void resourceChanged( KoCanvasResource::EnumCanvasResource key, const QVariant & res );
    void selectMode( int mode );
    void setOptimize( int state );
    void setDelta( double delta );
private:

    double lineAngle( const QPointF &p1, const QPointF &p2 );
    void addPoint( const QPointF & point );
    void finish( bool closePath );

    enum PencilMode { ModeRaw, ModeCurve, ModeStraight };

    PencilMode m_mode;
    bool m_optimizeRaw;
    bool m_optimizeCurve;
    double m_combineAngle;
    double m_fittingError;
    bool m_close;

    QList<QPointF> m_points; // the raw points

    KoPathShape * m_shape;
};

#endif // _KARBONPENCILTOOL_H_
