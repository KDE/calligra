#include "kivio_stencil_geometry_panel.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpixmap.h>

#include "tkfloatspinbox.h"
#include "tkunits.h"

static const char* width_xpm[] = {
  "13 11 3 1",
  "  c Gray0",
  ". c #808080",
  "X c None",
  "XXXXXXXXXXXXX",
  "XXXXXXXXXXXXX",
  ".XXXXXXXXXXX.",
  ".XX XXXXX XX.",
  ".X  XXXXX  X.",
  ".           .",
  ".X  XXXXX  X.",
  ".XX XXXXX XX.",
  ".XXXXXXXXXXX.",
  "XXXXXXXXXXXXX",
  "XXXXXXXXXXXXX"
};

static const char* height_xpm[] = {
  "13 11 3 1",
  "  c Gray0",
  ". c #808080",
  "X c None",
  "XXX.......XXX",
  "XXXXXX XXXXXX",
  "XXXXX   XXXXX",
  "XXXX     XXXX",
  "XXXXXX XXXXXX",
  "XXXXXX XXXXXX",
  "XXXXXX XXXXXX",
  "XXXX     XXXX",
  "XXXXX   XXXXX",
  "XXXXXX XXXXXX",
  "XXX.......XXX"
};

static const char* xpos_xpm[] = {
  "13 11 2 1",
  "  c Gray0",
  ". c None",
  ".. .. .......",
  ".. .. .......",
  "...  ........",
  "...  ........",
  ".. .. .......",
  ".. .. .     .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  ".......     ."
};

static const char* ypos_xpm[] = {
  "13 11 2 1",
  "  c Gray0",
  ". c None",
  "... . .......",
  "... . .......",
  "... . .......",
  ".... ........",
  ".... ........",
  "..  ...     .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  ".......     ."
};


KivioStencilGeometryPanel::KivioStencilGeometryPanel(QWidget* parent)
: QWidget(parent,"KivioStencilGeometryPanel")
{
    QGridLayout* grid = new QGridLayout( this, 4, 2, 3, 3 );

    QLabel *lx = new QLabel(this);
    QLabel *ly = new QLabel(this);
    QLabel *lw = new QLabel(this);
    QLabel *lh = new QLabel(this);

    lx->setPixmap( QPixmap((const char **)xpos_xpm) );
    ly->setPixmap( QPixmap((const char **)ypos_xpm) );
    lw->setPixmap( QPixmap((const char **)width_xpm) );
    lh->setPixmap( QPixmap((const char **)height_xpm) );

    m_pX = new TKUFloatSpinBox( this );
    m_pX->setMinValue(-1000.0);
    m_pX->setMaxValue(1000.0);
    m_pX->setLineStep(0.5);
    m_pX->setValue(0.0);

    m_pY = new TKUFloatSpinBox( this );
    m_pY->setMinValue(-1000.0);
    m_pY->setMaxValue(1000.0);
    m_pY->setLineStep(0.5);
    m_pY->setValue(0.0);

    m_pW = new TKUFloatSpinBox( this );
    m_pW->setMinValue(-1000.0);
    m_pW->setMaxValue(1000.0);
    m_pW->setLineStep(0.5);
    m_pW->setValue(0.0);

    m_pH = new TKUFloatSpinBox( this );
    m_pH->setMinValue(-1000.0);
    m_pH->setMaxValue(1000.0);
    m_pH->setLineStep(0.5);
    m_pH->setValue(0.0);

    QObject::connect( m_pX, SIGNAL(valueChanged(float)), this, SLOT(xChange(float)) );
    QObject::connect( m_pY, SIGNAL(valueChanged(float)), this, SLOT(yChange(float)) );
    QObject::connect( m_pW, SIGNAL(valueChanged(float)), this, SLOT(wChange(float)) );
    QObject::connect( m_pH, SIGNAL(valueChanged(float)), this, SLOT(hChange(float)) );

    grid->addWidget( lx, 0, 0 );
    grid->addWidget( m_pX, 0, 1 );

    grid->addWidget( ly, 1, 0 );
    grid->addWidget( m_pY, 1, 1 );

    grid->addWidget( lw, 2, 0 );
    grid->addWidget( m_pW, 2, 1 );

    grid->addWidget( lh, 3, 0 );
    grid->addWidget( m_pH, 3, 1 );
}

KivioStencilGeometryPanel::~KivioStencilGeometryPanel()
{
}

void KivioStencilGeometryPanel::setUnit( int m )
{
    m_pX->setUnit(m);
    m_pY->setUnit(m);
    m_pW->setUnit(m);
    m_pH->setUnit(m);
}

void KivioStencilGeometryPanel::xChange( float d )
{
    emit positionChanged( d, m_pY->value() );
}

void KivioStencilGeometryPanel::yChange( float d )
{
    emit positionChanged( m_pX->value(), d );
}

void KivioStencilGeometryPanel::wChange( float d )
{
    emit sizeChanged( d, m_pH->value() );
}

void KivioStencilGeometryPanel::hChange( float d )
{
    emit sizeChanged( m_pW->value(), d );
}

void KivioStencilGeometryPanel::setPosition( float x, float y )
{
    m_pX->setValue(x);
    m_pY->setValue(y);
}

void KivioStencilGeometryPanel::setSize( float w, float h )
{
    m_pW->setValue(w);
    m_pH->setValue(h);
}
#include "kivio_stencil_geometry_panel.moc"
