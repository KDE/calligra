#include "kivio_stencil_geometry_panel.h"

#include <QLayout>
#include <QLabel>
#include <qpixmap.h>

#include <knuminput.h>
#include <klocale.h>
#include <kdialog.h>

#include <KoPageLayout.h>
#include <KoUnitWidgets.h>

KivioStencilGeometryPanel::KivioStencilGeometryPanel(QWidget* parent)
: QWidget(parent,"KivioStencilGeometryPanel")
{
  QGridLayout* grid = new QGridLayout(this, 4, 2, 0, 2);

  QLabel* lx = new QLabel(i18n("X:"), this);
  QLabel* ly = new QLabel(i18n("Y:"), this);
  QLabel* lw = new QLabel(i18n("Width:"), this);
  QLabel* lh = new QLabel(i18n("Height:"), this);
//   QLabel *rotationLbl = new QLabel(this);

  m_pX = new KoUnitDoubleSpinBox(this, 0.0, 1000.0, 0.5, 0.0);
  m_pY = new KoUnitDoubleSpinBox(this, 0.0, 1000.0, 0.5, 0.0);
  m_pW = new KoUnitDoubleSpinBox(this, 0.0, 1000.0, 0.5, 0.0);
  m_pH = new KoUnitDoubleSpinBox(this, 0.0, 1000.0, 0.5, 0.0);
//   m_rotationSBox = new KIntSpinBox(-360, 360, 1, 0, 10, this);
//   m_rotationSBox->hide();

  connect(m_pX, SIGNAL(valueChanged(double)), SLOT(xChange(double)));
  connect(m_pY, SIGNAL(valueChanged(double)), SLOT(yChange(double)));
  connect(m_pW, SIGNAL(valueChanged(double)), SLOT(wChange(double)));
  connect(m_pH, SIGNAL(valueChanged(double)), SLOT(hChange(double)));
//  connect(m_rotationSBox, SIGNAL(valueChanged(int)), SLOT(rotationChange(int)));

  grid->addWidget(lx, 0, 0);
  grid->addWidget(m_pX, 1, 0);

  grid->addWidget(ly, 0, 1);
  grid->addWidget(m_pY, 1, 1);

  grid->addWidget(lw, 2, 0);
  grid->addWidget(m_pW, 3, 0);

  grid->addWidget(lh, 2, 1);
  grid->addWidget(m_pH, 3, 1);

  grid->setRowStretch(4, 10);

//   grid->addWidget(rotationLbl, 4, 0);
//   grid->addWidget(m_rotationSBox, 4, 1);

  m_unit = KoUnit::U_PT;
  m_emitSignals = true;
}

KivioStencilGeometryPanel::~KivioStencilGeometryPanel()
{
}

void KivioStencilGeometryPanel::setUnit( KoUnit::Unit m )
{
  bool oldEmitSignals = m_emitSignals;
  m_emitSignals = false;
  m_pX->setUnit(m);
  m_pY->setUnit(m);
  m_pW->setUnit(m);
  m_pH->setUnit(m);
  m_unit = m;
  m_emitSignals = oldEmitSignals;
}

void KivioStencilGeometryPanel::xChange( double d )
{
  if(m_emitSignals) {
    emit positionChanged( KoUnit::fromUserValue(d, m_unit), m_pY->value() );
  }
}

void KivioStencilGeometryPanel::yChange( double d )
{
  if(m_emitSignals) {
    emit positionChanged( m_pX->value(), KoUnit::fromUserValue(d, m_unit) );
  }
}

void KivioStencilGeometryPanel::wChange( double d )
{
  if(m_emitSignals) {
    emit sizeChanged( KoUnit::fromUserValue(d, m_unit), m_pH->value() );
  }
}

void KivioStencilGeometryPanel::hChange( double d )
{
  if(m_emitSignals) {
    emit sizeChanged( m_pW->value(), KoUnit::fromUserValue(d, m_unit) );
  }
}

void KivioStencilGeometryPanel::rotationChange(int d)
{
  if(m_emitSignals) {
    emit rotationChanged(d);
  }
}

void KivioStencilGeometryPanel::setPosition( double x, double y )
{
  m_pX->changeValue(x);
  m_pY->changeValue(y);
}

void KivioStencilGeometryPanel::setSize( double w, double h )
{
  m_pW->changeValue(w);
  m_pH->changeValue(h);
}

void KivioStencilGeometryPanel::setPageLayout(const KoPageLayout& l)
{
  m_pX->setMaxValue(l.ptWidth);
  m_pY->setMaxValue(l.ptHeight);
}

void KivioStencilGeometryPanel::setRotation(int /*d*/)
{
//   m_rotationSBox->setValue(d);
}

void KivioStencilGeometryPanel::setEmitSignals(bool e)
{
  m_emitSignals = e;
}

#include "kivio_stencil_geometry_panel.moc"
