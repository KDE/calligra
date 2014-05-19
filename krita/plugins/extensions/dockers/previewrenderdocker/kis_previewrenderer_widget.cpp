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
