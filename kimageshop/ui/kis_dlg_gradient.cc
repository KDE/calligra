#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include <qwidget.h>

#include <klocale.h>

#include "kis_doc.h"
#include "kis_dlg_gradient.h"
#include "integerwidget.h"
#include "blendchooser.h"

// X11 headers
#undef Below
#undef Above

GradientDialog::GradientDialog( KisDoc *_doc, 
    QWidget *_parent, const char *_name )
    : KFloatingDialog( _parent, _name )
    , m_pDoc( _doc )
{
  setCaption( i18n( "Gradients" ) );

  QWidget *area = new QWidget( this, "GradientDialogArea" );
  setBaseWidget( area );

  QGridLayout *layout = new QGridLayout( area, 7, 2, 5 );

  IntegerWidget *opacity = new IntegerWidget( 0, 100, area );
  opacity->setValue( 100 );
  opacity->setTickmarks( QSlider::Below );
  opacity->setTickInterval( 10 );
  layout->addWidget( opacity, 0, 1 );

  QLabel *lblOpacity = new QLabel( opacity, i18n( "Opacity" ), area );
  layout->addWidget( lblOpacity, 0, 0 );

  IntegerWidget *offset = new IntegerWidget( 0, 100, area, "offset" );
  offset->setTickmarks( QSlider::Below );
  offset->setTickInterval( 10 );
  layout->addWidget( offset, 1, 1 );

  QLabel *lblOffset = new QLabel( offset, i18n( "Offset" ), area );
  layout->addWidget( lblOffset, 1, 0 );

  BlendChooser *mode = new BlendChooser( area );
  layout->addWidget( mode, 2, 1 );

  QLabel *lblMode = new QLabel( mode, i18n( "Mode" ), area );
  layout->addWidget( lblMode, 2, 0 );

  QComboBox *blend = new QComboBox( false, area );
  blend->insertItem( i18n( "FG to BG (RGB)" ) );
  blend->insertItem( i18n( "FG to BG (HSV)" ) );
  blend->insertItem( i18n( "FG to Transparent" ) );
  blend->insertItem( i18n( "Custom (from editor)" ) );
  layout->addWidget( blend, 3, 1 );

  QLabel *lblBlend = new QLabel( blend, i18n( "Blend" ), area );
  layout->addWidget( lblBlend, 3, 0 );

  QComboBox *gradient = new QComboBox( false, area );
  gradient->insertItem( i18n( "Linear" ) );
  gradient->insertItem( i18n( "Bi-Linear" ) );
  gradient->insertItem( i18n( "Radial" ) );
  gradient->insertItem( i18n( "Square" ) );
  gradient->insertItem( i18n( "Conical (symmetric)" ) );
  gradient->insertItem( i18n( "Conical (assymmetric)" ) );
  gradient->insertItem( i18n( "Shapeburst (angular)" ) );
  gradient->insertItem( i18n( "Shapeburst (spherical)" ) );
  gradient->insertItem( i18n( "Shapeburst (dimpled)" ) );
  layout->addWidget( gradient, 4, 1 );

  QLabel *lblGradient = new QLabel( gradient, i18n( "Gradient" ), area );
  layout->addWidget( lblGradient, 4, 0 );

  QComboBox *repeat = new QComboBox( false, area );
  repeat->insertItem( i18n( "None" ) );
  repeat->insertItem( i18n( "Sawtooth wave" ) );
  repeat->insertItem( i18n( "Triangular wave" ) );
  layout->addWidget( repeat, 5, 1 );

  QLabel *lblRepeat= new QLabel( repeat, i18n( "Repeat" ), area );
  layout->addWidget( lblRepeat, 5, 0 );

  layout->setColStretch( 1, 1 );
  layout->setRowStretch( 6, 1 );
  
  opacity->setMinimumWidth( gradient->sizeHint().width() );
}

GradientDialog::~GradientDialog()
{
}
 
#include "kis_dlg_gradient.moc"
