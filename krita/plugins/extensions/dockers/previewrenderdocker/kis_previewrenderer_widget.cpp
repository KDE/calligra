/*
 *  Copyright (c) 2014 Spencer Brown <sbrown655@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <opengl/kis_opengl.h>

#ifdef HAVE_OPENGL
#include "kis_previewrenderer_widget.h"

#include <QGLWidget>

KisPreviewRendererWidget::KisPreviewRendererWidget(QWidget *parent) :
    QGLWidget(parent, KisOpenGL::sharedContextWidget())
{

}

void KisPreviewRendererWidget::initializeGL()
{

}

void KisPreviewRendererWidget::resizeGL(int w, int h)
{

}

void KisPreviewRendererWidget::paintGL()
{
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

#endif // HAVE_OPENGL
