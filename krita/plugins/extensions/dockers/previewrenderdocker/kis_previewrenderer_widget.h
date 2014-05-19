#ifndef KISPREVIEWRENDERERWIDGET_H
#define KISPREVIEWRENDERERWIDGET_H

#include <QGLWidget>

class KisPreviewRendererWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit KisPreviewRendererWidget(QWidget *parent = 0);

signals:

public slots:
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

};

#endif // KISPREVIEWRENDERERWIDGET_H
