#ifndef KIVIO_STENCIL_GEOMETRY_PANEL_H
#define KIVIO_STENCIL_GEOMETRY_PANEL_H

#include <qwidget.h>
#include <KoUnit.h>

class KoPageLayout;
class KoUnitDoubleSpinBox;
class KIntSpinBox;

class KivioStencilGeometryPanel : public QWidget
{
  Q_OBJECT
  protected:
    KoUnitDoubleSpinBox *m_pX, *m_pY, *m_pW, *m_pH;
    KIntSpinBox* m_rotationSBox;
    KoUnit::Unit m_unit;
    bool m_emitSignals;

  public slots:
    void setUnit(KoUnit::Unit);
    void setPosition(double, double);
    void setSize(double, double);
    void setPageLayout(const KoPageLayout& l);
    void setRotation(int d);

    void setEmitSignals(bool e);

  protected slots:
    void xChange(double);
    void yChange(double);
    void wChange(double);
    void hChange(double);
    void rotationChange(int);

  signals:
    void positionChanged(double, double);
    void sizeChanged(double, double);
    void rotationChanged(int);

  public:
    KivioStencilGeometryPanel(QWidget* parent);
    virtual ~KivioStencilGeometryPanel();
};

#endif
