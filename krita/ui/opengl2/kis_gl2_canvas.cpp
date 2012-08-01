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

#include "kis_gl2_canvas.h"

#include <QGLWidget>

#include <GL/gl.h>

#include <kis_config.h>
#include <kis_config_notifier.h>
#include <kis_image.h>

#include "kis_gl2_tilemanager.h"

class KisGL2Canvas::Private
{
public:
    KisGL2TileManager *tileManager;
};

KisGL2Canvas::KisGL2Canvas(KisCanvas2 *canvas, KisCoordinatesConverter *coordinatesConverter, QWidget *parent)
    : QGLWidget(parent), KisCanvasWidgetBase(canvas, coordinatesConverter), d(new Private)
{
    d->tileManager = new KisGL2TileManager(this);
}

KisGL2Canvas::~KisGL2Canvas()
{
    delete d;
}

QWidget* KisGL2Canvas::widget()
{
    return this;
}

bool KisGL2Canvas::callFocusNextPrevChild(bool next)
{
    return focusNextPrevChild(next);
}

void KisGL2Canvas::initialize(KisImageWSP image)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), SLOT(configChanged()));
    configChanged();

    d->tileManager->initialize(image);
}

void KisGL2Canvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Draw the background

    //Draw the tiles
}

void KisGL2Canvas::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void KisGL2Canvas::configChanged()
{
    const KisConfig cfg;
    QColor clearColor = cfg.canvasBorderColor();

    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0);
}
