#include "qlayout.h"
#include "qlabel.h"
#include "qslider.h"
#include "qwidget.h"

#include "klocale.h"

#include "gradientdlg.h"
#include "kimageshop_doc.h"
#include "integerwidget.h"

GradientDialog::GradientDialog( KImageShopDoc *_doc, QWidget *_parent, const char *_name )
  : KFloatingDialog( _parent, _name )
  , m_pDoc( _doc )
{
  setCaption( i18n( "Gradients" ) );

  QWidget *area = new QWidget( this, "GradientDialogArea" );
  setBaseWidget( area );

  QGridLayout *layout = new QGridLayout( area, 7, 2 );

  QLabel *label;

  IntegerWidget *opacity = new IntegerWidget( 0, 100, this );
  opacity->setBackgroundColor( red );
#warning "TODO: Can someone make setTickmarks working."
#warning "      It doesn't compile for me. I don't know why. (Michael)"
//opacity_aaa->setTickmarks( QSlider::Below );
  opacity->setTickInterval( 10 );
  layout->addWidget( opacity, 0, 1 );

  label = new QLabel( opacity, i18n( "Opacity" ), area );
  label->setBackgroundColor( blue );
  layout->addWidget( label, 0, 0 );

  IntegerWidget *offset = new IntegerWidget( 0, 100, this, "offset" );
  offset->setBackgroundColor( green );
//offset->setTickmarks( QSlider::Below );
  offset->setTickInterval( 10 );
  layout->addWidget( offset, 1, 1 );

  label = new QLabel( offset, i18n( "Offset" ), area );
  label->setBackgroundColor( yellow );
  layout->addWidget( label, 1, 0 );

  QComboBox *mode = new QComboBox( false, area );
  mode->insertItem( i18n( "Normal" ) );
  mode->insertItem( i18n( "Dissolve" ) );
  mode->insertItem( i18n( "Behind" ) );
  mode->insertItem( i18n( "Multiply" ) );
  mode->insertItem( i18n( "Screen" ) );
  mode->insertItem( i18n( "Overlay" ) );
  mode->insertItem( i18n( "Difference" ) );
  mode->insertItem( i18n( "Addition" ) );
  mode->insertItem( i18n( "Subtract" ) );
  mode->insertItem( i18n( "Darken only" ) );
  mode->insertItem( i18n( "Lighten only" ) );
  mode->insertItem( i18n( "Hue" ) );
  mode->insertItem( i18n( "Saturation" ) );
  mode->insertItem( i18n( "Color" ) );
  mode->insertItem( i18n( "Value" ) );
  layout->addWidget( mode, 2, 1 );

  label = new QLabel( mode, i18n( "Mode" ), area );
  layout->addWidget( label, 2, 0 );

  QComboBox *blend = new QComboBox( false, area );
  blend->insertItem( i18n( "FG to BG (RGB)" ) );
  blend->insertItem( i18n( "FG to BG (HSV)" ) );
  blend->insertItem( i18n( "FG to Transparent" ) );
  blend->insertItem( i18n( "Custom (from editor)" ) );
  layout->addWidget( blend, 3, 1 );

  label = new QLabel( blend, i18n( "Blend" ), area );
  layout->addWidget( label, 3, 0 );

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

  label = new QLabel( gradient, i18n( "Gradient" ), area );
  layout->addWidget( label, 4, 0 );

  QComboBox *repeat = new QComboBox( false, area );
  repeat->insertItem( i18n( "None" ) );
  repeat->insertItem( i18n( "Sawtooth wave" ) );
  repeat->insertItem( i18n( "Triangular wave" ) );
  layout->addWidget( repeat, 5, 1 );

  label = new QLabel( repeat, i18n( "Repeat" ), area );
  layout->addWidget( label, 5, 0 );

  layout->setColStretch( 0, 0 );

  layout->setRowStretch( 0, 0 );
  layout->setRowStretch( 1, 0 );
  layout->setRowStretch( 2, 0 );
  layout->setRowStretch( 3, 0 );
}

GradientDialog::~GradientDialog()
{
}

#include "gradientdlg.moc"

























