#ifndef KIVIO_STENCIL_GEOMETRY_PANEL_H
#define KIVIO_STENCIL_GEOMETRY_PANEL_H

#include <qwidget.h>
#include <koUnit.h>

class KoUnitDoubleSpinBox;

class KivioStencilGeometryPanel : public QWidget
{ Q_OBJECT
protected:
    KoUnitDoubleSpinBox *m_pX, *m_pY, *m_pW, *m_pH;
    KoUnit::Unit m_unit;

public slots:
    void setUnit(KoUnit::Unit);
    void setPosition(double, double);
    void setSize(double, double);

protected slots:
    void xChange(double);
    void yChange(double);
    void wChange(double);
    void hChange(double);

signals:
    void positionChanged(double, double);
    void sizeChanged(double, double);

public:
    KivioStencilGeometryPanel(QWidget* parent);
    virtual ~KivioStencilGeometryPanel();
};

#endif

