/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "KPrBrushProperty.h"

#include "brushpropertyui.h"
#include "gradientpropertyui.h"
#include "KPrPBPreview.h"

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <qslider.h>

#include <q3widgetstack.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>

#include <klocale.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kvbox.h>

KPrBrushProperty::KPrBrushProperty( QWidget *parent, const char *name, const KPrBrushCmd::Brush &brush )
    : QWidget( parent, name )
    , m_brush( brush )
{
    Q3GridLayout *layout = new Q3GridLayout( this, 1, 1, KDialog::marginHint(), KDialog::spacingHint() );

    m_typeCombo = new KComboBox( this );
    layout->addWidget( m_typeCombo, 0, 1 );
    m_typeCombo->addItem( i18n( "Single Color" ) );
    m_typeCombo->addItem( i18n( "Gradient" ) );
    m_typeCombo->addItem( i18n( "Transparent" ) );
    m_typeCombo->setWhatsThis( i18n( "You can choose between Single Color, Gradient or Transparent as the type." ) );

    QLabel *typeLabel = new QLabel( i18n( "&Type:" ), this );
    layout->addWidget( typeLabel, 0, 0 );
    typeLabel->setBuddy( m_typeCombo );

    m_stack = new Q3WidgetStack( this );
    layout->addMultiCellWidget( m_stack, 1, 1, 0, 1 );
    connect( m_typeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotTypeChanged( int ) ) );

    m_brushUI = new BrushPropertyUI( m_stack );

    m_brushUI->styleCombo->insertItem( 100, i18n( "%1% Fill Pattern" ) );
    m_brushUI->styleCombo->insertItem( 94, i18n( "%1% Fill Pattern" ) );
    m_brushUI->styleCombo->insertItem( 88, i18n( "%1% Fill Pattern" ) );
    m_brushUI->styleCombo->insertItem( 63, i18n( "%1% Fill Pattern" ) );
    m_brushUI->styleCombo->insertItem( 50, i18n( "%1% Fill Pattern" ) );
    m_brushUI->styleCombo->insertItem( 37, i18n( "%1% Fill Pattern" ) );
    m_brushUI->styleCombo->insertItem( 12, i18n( "%1% Fill Pattern" ) );
    m_brushUI->styleCombo->insertItem( 6, i18n( "%1% Fill Pattern" ) );
    m_brushUI->styleCombo->addItem( i18n( "Horizontal Lines" ) );
    m_brushUI->styleCombo->addItem( i18n( "Vertical Lines" ) );
    m_brushUI->styleCombo->addItem( i18n( "Crossing Lines" ) );
    m_brushUI->styleCombo->addItem( i18n( "Diagonal Lines ( / )" ) );
    m_brushUI->styleCombo->addItem( i18n( "Diagonal Lines ( \\ )" ) );
    m_brushUI->styleCombo->addItem( i18n( "Diagonal Crossing Lines" ) );

    m_preview_color = new KPrPBPreview( m_brushUI->previewPanel, KPrPBPreview::Brush );
    Q3HBoxLayout *hbox = new Q3HBoxLayout( m_brushUI->previewPanel );
    m_preview_color->setWhatsThis( i18n( "This displays a preview of your choices." ) );
    hbox->addWidget(m_preview_color);

    connect( m_brushUI->styleCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotBrushChanged() ) );
    connect( m_brushUI->colorChooser, SIGNAL( changed( const QColor& ) ),
             this, SLOT( slotBrushChanged() ) );

    m_stack->addWidget( m_brushUI, 0 );

    m_gradientUI = new GradientPropertyUI( m_stack );
    m_gradientUI->styleCombo->addItem( i18n( "Horizontal" ) );
    m_gradientUI->styleCombo->addItem( i18n( "Vertical" ) );
    m_gradientUI->styleCombo->addItem( i18n( "Diagonal 1" ) );
    m_gradientUI->styleCombo->addItem( i18n( "Diagonal 2" ) );
    m_gradientUI->styleCombo->addItem( i18n( "Circle" ) );
    m_gradientUI->styleCombo->addItem( i18n( "Rectangle" ) );
    m_gradientUI->styleCombo->addItem( i18n( "PipeCross" ) );
    m_gradientUI->styleCombo->addItem( i18n( "Pyramid" ) );

    m_preview_gradient = new KPrPBPreview( m_gradientUI->previewPanel, KPrPBPreview::Gradient );
    hbox = new Q3HBoxLayout( m_gradientUI->previewPanel );
    hbox->addWidget(m_preview_gradient);

    connect( m_gradientUI->styleCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotBackColorTypeChanged() ) );
    connect( m_gradientUI->color1Chooser, SIGNAL( changed( const QColor& ) ),
             this, SLOT( slotColor1Changed() ) );
    connect( m_gradientUI->color2Chooser, SIGNAL( changed( const QColor& ) ),
             this, SLOT( slotColor2Changed() ) );
    connect( m_gradientUI->unbalancedCheckBox, SIGNAL( clicked() ),
             this, SLOT( slotUnbalancedChanged() ) );
    connect( m_gradientUI->xSlider, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotXFactorChanged() ) );
    connect( m_gradientUI->ySlider, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotYFactorChanged() ) );

    m_stack->addWidget( m_gradientUI, 1 );
    m_stack->addWidget( new Q3Frame(this), 2 ); // the transparent case

    slotReset();
}


KPrBrushProperty::~KPrBrushProperty()
{
}


FillType KPrBrushProperty::getFillType() const
{
    int selected = m_typeCombo->currentItem();
    if(selected == 2)
        selected = 0;
    return (FillType) selected;
}


QBrush KPrBrushProperty::getQBrush() const
{
    QBrush brush;

    switch ( m_brushUI->styleCombo->currentItem() )
    {
        case 0:
            brush.setStyle( Qt::SolidPattern );
            break;
        case 1:
            brush.setStyle( Qt::Dense1Pattern );
            break;
        case 2:
            brush.setStyle( Qt::Dense2Pattern );
            break;
        case 3:
            brush.setStyle( Qt::Dense3Pattern );
            break;
        case 4:
            brush.setStyle( Qt::Dense4Pattern );
            break;
        case 5:
            brush.setStyle( Qt::Dense5Pattern );
            break;
        case 6:
            brush.setStyle( Qt::Dense6Pattern );
            break;
        case 7:
            brush.setStyle( Qt::Dense7Pattern );
            break;
        case 8:
            brush.setStyle( Qt::HorPattern );
            break;
        case 9:
            brush.setStyle( Qt::VerPattern );
            break;
        case 10:
            brush.setStyle( Qt::CrossPattern );
            break;
        case 11:
            brush.setStyle( Qt::BDiagPattern );
            break;
        case 12:
            brush.setStyle( Qt::FDiagPattern );
            break;
        case 13:
            brush.setStyle( Qt::DiagCrossPattern );
            break;
    }
    if( m_typeCombo->currentItem() == 2)
        brush.setStyle( Qt::NoBrush );

    brush.setColor( m_brushUI->colorChooser->color() );

    return brush;
}


QColor KPrBrushProperty::getGColor1()const
{
    return m_gradientUI->color1Chooser->color();
}


QColor KPrBrushProperty::getGColor2()const
{
    return m_gradientUI->color2Chooser->color();
}


BCType KPrBrushProperty::getGType()const
{
    return (BCType)( m_gradientUI->styleCombo->currentItem() +1 );
}


bool KPrBrushProperty::getGUnbalanced()const
{
    return m_gradientUI->unbalancedCheckBox->isChecked();
}


int KPrBrushProperty::getGXFactor() const
{
    return m_gradientUI->xSlider->value();
}


int KPrBrushProperty::getGYFactor() const
{
    return m_gradientUI->ySlider->value();
}


int KPrBrushProperty::getBrushPropertyChange() const
{
    int flags = 0;
    bool fillTypeChanged = getFillType() != m_brush.fillType;

    if ( getFillType() == FT_BRUSH )
    {
        QBrush brush = getQBrush();
        if ( fillTypeChanged || brush.color() != m_brush.brush.color() )
        {
            flags |= KPrBrushCmd::BrushColor;
        }
        if ( fillTypeChanged || brush.style() != m_brush.brush.style() )
        {
            flags |= KPrBrushCmd::BrushStyle;
        }
        if ( fillTypeChanged )
        {
            flags |= KPrBrushCmd::BrushGradientSelect;
        }
    }
    else
    {
        if ( fillTypeChanged || getGColor1() != m_brush.gColor1 )
        {
            flags |= KPrBrushCmd::GradientColor1;
        }
        if ( fillTypeChanged || getGColor2() != m_brush.gColor2 )
        {
            flags |= KPrBrushCmd::GradientColor2;
        }
        if ( fillTypeChanged || getGType() != m_brush.gType )
        {
            flags |= KPrBrushCmd::GradientType;
        }
        if ( fillTypeChanged || getGUnbalanced() != m_brush.unbalanced )
        {
            flags |= KPrBrushCmd::GradientBalanced;
        }
        if ( fillTypeChanged || getGXFactor() != m_brush.xfactor )
        {
            flags |= KPrBrushCmd::GradientXFactor;
        }
        if ( fillTypeChanged || getGYFactor() != m_brush.yfactor )
        {
            flags |= KPrBrushCmd::GradientYFactor;
        }
        if ( fillTypeChanged )
        {
            flags |= KPrBrushCmd::BrushGradientSelect;
        }
    }
    return flags;
}


KPrBrushCmd::Brush KPrBrushProperty::getBrush() const
{
    KPrBrushCmd::Brush brush( getQBrush(),
                           getGColor1(),
                           getGColor2(),
                           getGType(),
                           getFillType(),
                           getGUnbalanced(),
                           getGXFactor(),
                           getGYFactor() );
    return brush;
}


void KPrBrushProperty::setBrush( KPrBrushCmd::Brush &brush )
{
    m_brush = brush;
    slotReset();
}


void KPrBrushProperty::apply()
{
    int flags = getBrushPropertyChange();

    if ( flags & KPrBrushCmd::BrushGradientSelect )
        m_brush.fillType = getFillType();

    if ( flags & KPrBrushCmd::BrushColor )
        m_brush.brush.setColor( getQBrush().color() );

    if ( flags & KPrBrushCmd::BrushStyle )
        m_brush.brush.setStyle( getQBrush().style() );

    if ( flags & KPrBrushCmd::GradientColor1 )
        m_brush.gColor1 = getGColor1();

    if ( flags & KPrBrushCmd::GradientColor2 )
        m_brush.gColor2 = getGColor2();

    if ( flags & KPrBrushCmd::GradientType )
        m_brush.gType = getGType() ;

    if ( flags & KPrBrushCmd::GradientBalanced )
        m_brush.unbalanced = getGUnbalanced() ;

    if ( flags & KPrBrushCmd::GradientXFactor )
        m_brush.xfactor = getGXFactor() ;

    if ( flags & KPrBrushCmd::GradientYFactor )
        m_brush.yfactor = getGYFactor() ;
}


void KPrBrushProperty::setQBrush( const QBrush &brush )
{
    switch ( brush.style() )
    {
			case Qt::NoBrush:
            // TODO
            break;
			case Qt::SolidPattern:
            m_brushUI->styleCombo->setCurrentItem( 0 );
            break;
			case Qt::Dense1Pattern:
            m_brushUI->styleCombo->setCurrentItem( 1 );
            break;
			case Qt::Dense2Pattern:
            m_brushUI->styleCombo->setCurrentItem( 2 );
            break;
			case Qt::Dense3Pattern:
            m_brushUI->styleCombo->setCurrentItem( 3 );
            break;
			case Qt::Dense4Pattern:
            m_brushUI->styleCombo->setCurrentItem( 4 );
            break;
			case Qt::Dense5Pattern:
            m_brushUI->styleCombo->setCurrentItem( 5 );
            break;
			case Qt::Dense6Pattern:
            m_brushUI->styleCombo->setCurrentItem( 6 );
            break;
			case Qt::Dense7Pattern:
            m_brushUI->styleCombo->setCurrentItem( 7 );
            break;
			case Qt::HorPattern:
            m_brushUI->styleCombo->setCurrentItem( 8 );
            break;
			case Qt::VerPattern:
            m_brushUI->styleCombo->setCurrentItem( 9 );
            break;
			case Qt::CrossPattern:
            m_brushUI->styleCombo->setCurrentItem( 10 );
            break;
			case Qt::BDiagPattern:
            m_brushUI->styleCombo->setCurrentItem( 11 );
            break;
			case Qt::FDiagPattern:
            m_brushUI->styleCombo->setCurrentItem( 12 );
            break;
			case Qt::DiagCrossPattern:
            m_brushUI->styleCombo->setCurrentItem( 13 );
            break;
			case Qt::CustomPattern:
            break;
    }

    m_brushUI->colorChooser->setColor( brush.color() );
    m_preview_color->setBrush( brush );
}


void KPrBrushProperty::setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
                                    bool _unbalanced, int _xfactor, int _yfactor )
{
    m_gradientUI->styleCombo->setCurrentItem( (int) _t - 1 );
    m_gradientUI->color1Chooser->setColor( _c1 );
    m_gradientUI->color2Chooser->setColor( _c2 );
    m_gradientUI->unbalancedCheckBox->setChecked( _unbalanced );
    setUnbalancedEnabled( _unbalanced );
    m_gradientUI->xSlider->setValue( _xfactor );
    m_gradientUI->ySlider->setValue( _yfactor );
    m_preview_gradient->setGradient( _c1, _c2, _t, _unbalanced, _xfactor, _yfactor );
}


void KPrBrushProperty::setUnbalancedEnabled( bool state )
{
    m_gradientUI->xFactorLabel->setEnabled( state );
    m_gradientUI->xSlider->setEnabled( state );
    m_gradientUI->yFactorLabel->setEnabled( state );
    m_gradientUI->ySlider->setEnabled( state );
}


void KPrBrushProperty::slotReset()
{
    if ( m_brush.gType == BCT_PLAIN )
        m_brush.gType = BCT_GHORZ;
    setQBrush( m_brush.brush );
    setGradient( m_brush.gColor1,
                 m_brush.gColor2,
                 m_brush.gType,
                 m_brush.unbalanced,
                 m_brush.xfactor,
                 m_brush.yfactor );

    int panelIndex;
    if( m_brush.fillType == FT_BRUSH && m_brush.brush.style() == Qt::NoBrush )
        panelIndex = 2;
    else
        panelIndex = (int)m_brush.fillType;

    m_typeCombo->setCurrentItem( panelIndex );
    slotTypeChanged( panelIndex );
}


void KPrBrushProperty::slotTypeChanged( int pos )
{
    m_stack->raiseWidget( pos );
    slotBrushChanged();
}


void KPrBrushProperty::slotBrushChanged()
{
    m_preview_color->setBrush( getQBrush() );
}


void KPrBrushProperty::slotColor1Changed()
{
    m_preview_gradient->setColor1( getGColor1() );
}


void KPrBrushProperty::slotColor2Changed()
{
    m_preview_gradient->setColor2( getGColor2() );
}


void KPrBrushProperty::slotBackColorTypeChanged()
{
    BCType type = getGType();
    m_preview_gradient->setBackColorType( type );
    m_gradientUI->xSlider->setEnabled( type != 1 );
    m_gradientUI->ySlider->setEnabled( type != 2 );
}


void KPrBrushProperty::slotUnbalancedChanged()
{
    bool state = getGUnbalanced();
    setUnbalancedEnabled( state );
    m_preview_gradient->setUnbalanced( state );

    slotBackColorTypeChanged(); // make sure the sliders enabled-ness is up-to-date
}


void KPrBrushProperty::slotXFactorChanged()
{
    m_preview_gradient->setXFactor( getGXFactor() );
}


void KPrBrushProperty::slotYFactorChanged()
{
    m_preview_gradient->setYFactor( getGYFactor() );
}

#include "KPrBrushProperty.moc"
