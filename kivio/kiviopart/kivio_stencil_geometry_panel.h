#ifndef KIVIO_STENCIL_GEOMETRY_PANEL_H
#define KIVIO_STENCIL_GEOMETRY_PANEL_H

#include <qwidget.h>

class TKUFloatSpinBox;

class KivioStencilGeometryPanel : public QWidget
{ Q_OBJECT
protected:
    TKUFloatSpinBox *m_pX, *m_pY, *m_pW, *m_pH;

public slots:
    void setUnit(int);
    void setPosition(float, float);
    void setSize(float, float);

protected slots:
    void xChange(float);
    void yChange(float);
    void wChange(float);
    void hChange(float);

signals:
    void positionChanged(float, float);
    void sizeChanged(float, float);

public:
    KivioStencilGeometryPanel(QWidget* parent);
    virtual ~KivioStencilGeometryPanel();
};

#endif

