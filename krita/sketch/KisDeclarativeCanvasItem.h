/*
*  Copyright (c) 2012 KO GmbH. Contact: Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KISDECLARATIVECANVAS_H
#define KISDECLARATIVECANVAS_H

#include <QDeclarativeItem>
#include <KoCanvasBase.h>

class KisCanvas2;


/**
 * @brief The KisDeclarativeCanvas class is a declarative item that wraps a Krita canvas widget,
 * either an opengl widget (in which case it will share the framebuffer) or a qpainter canvas widget,
 * in which case all best are off.
 */
class KisDeclarativeCanvasItem : public QDeclarativeItem //, public KoCanvasBase
{
    Q_OBJECT
public:
    explicit KisDeclarativeCanvasItem(KisCanvas2 *canvas, QDeclarativeItem *parent = 0);

//    virtual void KoCanvasBase::gridSize(qreal*, qreal*) const;
//    virtual bool KoCanvasBase::snapToGrid() const;
//    virtual void KoCanvasBase::setCursor(const QCursor&);
//    virtual void KoCanvasBase::addCommand(KUndo2Command*);
//    virtual KoShapeManager* KoCanvasBase::shapeManager() const;
//    virtual void KoCanvasBase::updateCanvas(const QRectF&);
//    virtual KoToolProxy* KoCanvasBase::toolProxy() const;
//    virtual KoViewConverter* KoCanvasBase::viewConverter() const;
//    virtual QWidget* KoCanvasBase::canvasWidget();
//    virtual const QWidget* KoCanvasBase::canvasWidget() const;
//    virtual KoUnit KoCanvasBase::unit() const;
//    virtual void KoCanvasBase::updateInputMethodInfo();

signals:

public slots:
private:

    KisCanvas2 *m_canvas;
};

#endif // KISDECLARATIVECANVAS_H
