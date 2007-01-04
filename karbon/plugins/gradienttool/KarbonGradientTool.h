/* This file is part of the KDE project
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
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
#include <KoInteractionStrategy.h>

class KoShape;
class QUndoCommand;
class QLinearGradient;
class QRadialGradient;
class QConicalGradient;

class KarbonGradientTool : public KoTool
{
    Q_OBJECT
public:
    explicit KarbonGradientTool(KoCanvasBase *canvas);
    ~KarbonGradientTool();

    void paint( QPainter &painter, KoViewConverter &converter );

    void mousePressEvent( KoPointerEvent *event ) ;
    void mouseMoveEvent( KoPointerEvent *event );
    void mouseReleaseEvent( KoPointerEvent *event );

    void activate (bool temporary=false);
    void deactivate();

private:
    class GradientStrategy
    {
    public:
        GradientStrategy( KoShape *shape );
        virtual ~GradientStrategy() {};
        virtual void paint( QPainter &painter, KoViewConverter &converter ) = 0;
        virtual bool selectHandle( const QPointF &mousePos );
        KoShape * shape() { return m_shape; }
        void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers);
        void setEditing( bool on );
        bool isEditing() { return m_editing; }
        QUndoCommand * createCommand();
    protected:
        void paintHandle( QPainter &painter, const QPointF &position );
        bool mouseInsideHandle( const QPointF &mousePos, const QPointF &handlePos );
        virtual QBrush background() = 0;
        KoShape *m_shape;
        int m_selectedHandle;
        QBrush m_oldBackground;
        QBrush m_newBackground;
        QList<QPointF> m_handles;
    private:
        bool m_editing;
    };

    class LinearGradientStrategy : public GradientStrategy
    {
    public:
        LinearGradientStrategy( KoShape *shape, const QLinearGradient *gradient );
        virtual void paint( QPainter &painter, KoViewConverter &converter );
    private:
        virtual QBrush background();
        enum Handles { start, stop };
    };

    class RadialGradientStrategy : public GradientStrategy
    {
    public:
        RadialGradientStrategy( KoShape *shape, const QRadialGradient *gradient );
        virtual void paint( QPainter &painter, KoViewConverter &converter );
    private:
        virtual QBrush background();
        enum Handles { center, focal, radius };
    };

    class ConicalGradientStrategy : public GradientStrategy
    {
    public:
        ConicalGradientStrategy( KoShape *shape, const QConicalGradient *gradient );
        virtual void paint( QPainter &painter, KoViewConverter &converter );
    private:
        virtual QBrush background();
        enum Handles { center, direction };
    };

    QList<GradientStrategy*> m_gradients;
    GradientStrategy* m_currentStrategy;
};

#endif // _KARBONGRADIENTTOOL_H_
