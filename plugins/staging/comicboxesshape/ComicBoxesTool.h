/*
 *  Copyright (c) 2010 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _COMICBOXESTOOL_H_
#define _COMICBOXESTOOL_H_

#include <KoToolBase.h>

class ComicBoxesShape;
class KoShape;

class ComicBoxesTool : public KoToolBase
{
    Q_OBJECT
  public:
    explicit ComicBoxesTool(KoCanvasBase *canvas);
    ~ComicBoxesTool();
    
    /// reimplemented
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) ;

    /// reimplemented
    virtual void paint( QPainter &painter, const KoViewConverter &converter );

    /// reimplemented
    virtual void mousePressEvent( KoPointerEvent *event );
    /// reimplemented
    virtual void mouseMoveEvent( KoPointerEvent *event );
    /// reimplemented
    virtual void mouseReleaseEvent( KoPointerEvent *event );
  protected:
    virtual QMap<QString, QWidget *> createOptionWidgets();

private:
    QRectF currentDraggingRect() const;
    
  private:
    ComicBoxesShape* m_currentShape;
    bool m_dragging;
    QPointF m_currentStartingPoint;
    QPointF m_currentPoint;
};

#endif
