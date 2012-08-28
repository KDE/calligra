/* This file is part of the KDE project
 *
 * Copyright (c) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#ifndef KIS_GL2_RENDERTHREAD_H
#define KIS_GL2_RENDERTHREAD_H

#include <QThread>
#include <kis_types.h>

class QRect;
class KisGL2Canvas;

class KisGL2RenderThread : public QThread
{
    Q_OBJECT
public:
    explicit KisGL2RenderThread(KisGL2Canvas *canvas, KisImageWSP image);
    virtual ~KisGL2RenderThread();

    void initialize();

    uint texture() const;

public Q_SLOTS:
    void render();
    void resize(int width, int height);
    void stop();
    void configChanged();

Q_SIGNALS:
    void renderFinished();

protected:
    virtual void run();

private:
    class Private;
    Private * const d;
};

#endif // KIS_GL2_RENDERTHREAD_H
