#include "brushproperty.h"

#include "brushpropertyui.h"
#include "gradientpropertyui.h"
#include "pbpreview.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include <qwidgetstack.h>

#include <klocale.h>
#include <kcolorbutton.h>
#include <kcombobox.h>

BrushProperty::BrushProperty( QWidget *parent, const char *name, const BrushCmd::Brush &brush )
    : QWidget( parent, name )
    , m_brush( brush )
{
    QGridLayout *layout = new QGridLayout( this, 1, 1, 11, 6 );

    m_typeCombo = new KComboBox( this );
    layout->addWidget( m_typeCombo, 0, 1 );
    m_typeCombo->insertItem( i18n( "Brush" ) );
    m_typeCombo->insertItem( i18n( "Gradient" ) );

    QLabel *typeLabel = new QLabel( i18n( "&Type:" ), this );
    layout->addWidget( typeLabel, 0, 0 );
    typeLabel->setBuddy( m_typeCombo );

    m_stack = new QWidgetStack( this );
    layout->addMultiCellWidget( m_stack, 1, 1, 0, 1 );
    connect( m_typeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotTypeChanged( int ) ) );

    m_brushUI = new BrushPropertyUI( m_stack );

    m_brushUI->styleCombo->insertItem( i18n( "No Background Fill" ) );
    m_brushUI->styleCombo->insertItem( i18n( "%1% Fill Pattern" ).arg( 100 ) );
    m_brushUI->styleCombo->insertItem( i18n( "%1% Fill Pattern" ).arg( 94 ) );
    m_brushUI->styleCombo->insertItem( i18n( "%1% Fill Pattern" ).arg( 88 ) );
    m_brushUI->styleCombo->insertItem( i18n( "%1% Fill Pattern" ).arg( 63 ) );
    m_brushUI->styleCombo->insertItem( i18n( "%1% Fill Pattern" ).arg( 50 ) );
    m_brushUI->styleCombo->insertItem( i18n( "%1% Fill Pattern" ).arg( 37 ) );
    m_brushUI->styleCombo->insertItem( i18n( "%1% Fill Pattern" ).arg( 12 ) );
    m_brushUI->styleCombo->insertItem( i18n( "%1% Fill Pattern" ).arg( 6 ) );
    m_brushUI->styleCombo->insertItem( i18n( "Horizontal Lines" ) );
    m_brushUI->styleCombo->insertItem( i18n( "Vertical Lines" ) );
    m_brushUI->styleCombo->insertItem( i18n( "Crossing Lines" ) );
    m_brushUI->styleCombo->insertItem( i18n( "Diagonal Lines ( / )" ) );
    m_brushUI->styleCombo->insertItem( i18n( "Diagonal Lines ( \\ )" ) );
    m_brushUI->styleCombo->insertItem( i18n( "Diagonal Crossing Lines" ) );

    connect( m_brushUI->styleCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotBrushChanged() ) );
    connect( m_brushUI->colorChooser, SIGNAL( changed( const QColor& ) ),
             this, SLOT( slotBrushChanged() ) );

    m_stack->addWidget( m_brushUI, 0 );

    m_gradientUI = new GradientPropertyUI( m_stack );
    m_gradientUI->styleCombo->insertItem( i18n( "Horizontal Gradient" ) );
    m_gradientUI->styleCombo->insertItem( i18n( "Vertical Gradient" ) );
    m_gradientUI->styleCombo->insertItem( i18n( "Diagonal Gradient 1" ) );
    m_gradientUI->styleCombo->insertItem( i18n( "Diagonal Gradient 2" ) );
    m_gradientUI->styleCombo->insertItem( i18n( "Circle Gradient" ) );
    m_gradientUI->styleCombo->insertItem( i18n( "Rectangle Gradient" ) );
    m_gradientUI->styleCombo->insertItem( i18n( "PipeCross Gradient" ) );
    m_gradientUI->styleCombo->insertItem( i18n( "Pyramid Gradient" ) );

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

    m_preview = new PBPreview( this, 0, PBPreview::Brush );
    layout->addMultiCellWidget( m_preview, 2, 2, 0, 1 );

    slotReset();
}


BrushProperty::~BrushProperty()
{
}


FillType BrushProperty::getFillType() const
{
    return (FillType) m_typeCombo->currentItem();
}


QBrush BrushProperty::getQBrush() const
{
    QBrush brush;

    switch ( m_brushUI->styleCombo->currentItem() )
    {
        case 0:
            brush.setStyle( NoBrush );
            break;
        case 1:
            brush.setStyle( SolidPattern );
            break;
        case 2:
            brush.setStyle( Dense1Pattern );
            break;
        case 3:
            brush.setStyle( Dense2Pattern );
            break;
        case 4:
            brush.setStyle( Dense3Pattern );
            break;
        case 5:
            brush.setStyle( Dense4Pattern );
            break;
        case 6:
            brush.setStyle( Dense5Pattern );
            break;
        case 7:
            brush.setStyle( Dense6Pattern );
            break;
        case 8:
            brush.setStyle( Dense7Pattern );
            break;
        case 9:
            brush.setStyle( HorPattern );
            break;
        case 10:
            brush.setStyle( VerPattern );
            break;
        case 11:
            brush.setStyle( CrossPattern );
            break;
        case 12:
            brush.setStyle( BDiagPattern );
            break;
        case 13:
            brush.setStyle( FDiagPattern );
            break;
        case 14:
            brush.setStyle( DiagCrossPattern );
            break;
    }

    brush.setColor( m_brushUI->colorChooser->color() );

    return brush;
}


QColor BrushProperty::getGColor1()const
{
    return m_gradientUI->color1Chooser->color();
}


QColor BrushProperty::getGColor2()const
{
    return m_gradientUI->color2Chooser->color();
}


BCType BrushProperty::getGType()const
{
    return (BCType)( m_gradientUI->styleCombo->currentItem() + 1 );
}


bool BrushProperty::getGUnbalanced()const
{
    return m_gradientUI->unbalancedCheckBox->isChecked();
}


int BrushProperty::getGXFactor() const
{
    return m_gradientUI->xSlider->value();
}


int BrushProperty::getGYFactor() const
{
    return m_gradientUI->ySlider->value();
}


int BrushProperty::getBrushPropertyChange() const
{
    int flags = 0;
    bool fillTypeChanged = getFillType() != m_brush.fillType;

    if ( fillTypeChanged )
    {
        flags |= BrushCmd::BrushGradientSelect;
    }

    if ( getFillType() == FT_BRUSH )
    {
        QBrush brush = getQBrush();
        if ( fillTypeChanged || brush.color() != m_brush.brush.color() )
        {
            flags |= BrushCmd::BrushColor;
        }
        if ( fillTypeChanged || brush.color() != m_brush.brush.color() )
        {
            flags |= BrushCmd::BrushStyle;
        }
    }
    else
    {
        if ( fillTypeChanged || getGColor1() != m_brush.gColor1 )
        {
            flags |= BrushCmd::GradientColor1;
        }
        if ( fillTypeChanged || getGColor2() != m_brush.gColor2 )
        {
            flags |= BrushCmd::GradientColor2;
        }
        if ( fillTypeChanged || getGType() != m_brush.gType )
        {
            flags |= BrushCmd::GradientType;
        }
        if ( fillTypeChanged || getGUnbalanced() != m_brush.unbalanced )
        {
            flags |= BrushCmd::GradientBalanced;
        }
        if ( fillTypeChanged || getGXFactor() != m_brush.xfactor )
        {
            flags |= BrushCmd::GradientXFactor;
        }
        if ( fillTypeChanged || getGYFactor() != m_brush.yfactor )
        {
            flags |= BrushCmd::GradientYFactor;
        }
    }
    return flags;
}


BrushCmd::Brush BrushProperty::getBrush() const
{
    BrushCmd::Brush brush( getQBrush(),
                           getGColor1(),
                           getGColor2(),
                           getGType(),
                           getFillType(),
                           getGUnbalanced(),
                           getGXFactor(),
                           getGYFactor() );
    return brush;
}


void BrushProperty::setBrush( BrushCmd::Brush &brush )
{
    m_brush = brush;
    slotReset();
}


void BrushProperty::apply()
{
    int flags = getBrushPropertyChange();

    if ( flags & BrushCmd::BrushGradientSelect )
        m_brush.fillType = getFillType();

    if ( flags & BrushCmd::BrushColor )
        m_brush.brush.setColor( getQBrush().color() );

    if ( flags & BrushCmd::BrushStyle )
        m_brush.brush.setStyle( getQBrush().style() );

    if ( flags & BrushCmd::GradientColor1 )
        m_brush.gColor1 = getGColor1();

    if ( flags & BrushCmd::GradientColor2 )
        m_brush.gColor2 = getGColor2();

    if ( flags & BrushCmd::GradientType )
        m_brush.gType = getGType() ;

    if ( flags & BrushCmd::GradientBalanced )
        m_brush.unbalanced = getGUnbalanced() ;

    if ( flags & BrushCmd::GradientXFactor )
        m_brush.xfactor = getGXFactor() ;

    if ( flags & BrushCmd::GradientYFactor )
        m_brush.yfactor = getGYFactor() ;
}


void BrushProperty::setQBrush( const QBrush &brush )
{
    switch ( brush.style() )
    {
        case NoBrush:
            m_brushUI->styleCombo->setCurrentItem( 0 );
            break;
        case SolidPattern:
            m_brushUI->styleCombo->setCurrentItem( 1 );
            break;
        case Dense1Pattern:
            m_brushUI->styleCombo->setCurrentItem( 2 );
            break;
        case Dense2Pattern:
            m_brushUI->styleCombo->setCurrentItem( 3 );
            break;
        case Dense3Pattern:
            m_brushUI->styleCombo->setCurrentItem( 4 );
            break;
        case Dense4Pattern:
            m_brushUI->styleCombo->setCurrentItem( 5 );
            break;
        case Dense5Pattern:
            m_brushUI->styleCombo->setCurrentItem( 6 );
            break;
        case Dense6Pattern:
            m_brushUI->styleCombo->setCurrentItem( 7 );
            break;
        case Dense7Pattern:
            m_brushUI->styleCombo->setCurrentItem( 8 );
            break;
        case HorPattern:
            m_brushUI->styleCombo->setCurrentItem( 9 );
            break;
        case VerPattern:
            m_brushUI->styleCombo->setCurrentItem( 10 );
            break;
        case CrossPattern:
            m_brushUI->styleCombo->setCurrentItem( 11 );
            break;
        case BDiagPattern:
            m_brushUI->styleCombo->setCurrentItem( 12 );
            break;
        case FDiagPattern:
            m_brushUI->styleCombo->setCurrentItem( 13 );
            break;
        case DiagCrossPattern:
            m_brushUI->styleCombo->setCurrentItem( 14 );
            break;
        case CustomPattern:
            break;
    }

    m_brushUI->colorChooser->setColor( brush.color() );
    m_preview->setBrush( brush );
}


void BrushProperty::setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
                                    bool _unbalanced, int _xfactor, int _yfactor )
{
    m_gradientUI->styleCombo->setCurrentItem( (int) _t - 1 );
    m_gradientUI->color1Chooser->setColor( _c1 );
    m_gradientUI->color2Chooser->setColor( _c2 );
    m_gradientUI->unbalancedCheckBox->setChecked( _unbalanced );
    setUnbalancedEnabled( _unbalanced );
    m_gradientUI->xSlider->setValue( _xfactor );
    m_gradientUI->ySlider->setValue( _yfactor );
    m_preview->setGradient( _c1, _c2, _t, _unbalanced, _xfactor, _yfactor );
}


void BrushProperty::setUnbalancedEnabled( bool state )
{
    m_gradientUI->xFactorLabel->setEnabled( state );
    m_gradientUI->xSlider->setEnabled( state );
    m_gradientUI->yFactorLabel->setEnabled( state );
    m_gradientUI->ySlider->setEnabled( state );
}


void BrushProperty::slotReset()
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
    m_typeCombo->setCurrentItem( (int)m_brush.fillType );
    slotTypeChanged( (int)m_brush.fillType );
}


void BrushProperty::slotTypeChanged( int pos )
{
    m_stack->raiseWidget( pos );

    // Brush
    if ( pos == 0 )
    {
        m_preview->setPaintType( PBPreview::Brush );
    }
    // Gradient
    else
    {
        m_preview->setPaintType( PBPreview::Gradient );
    }
}


void BrushProperty::slotBrushChanged()
{
    m_preview->setBrush( getQBrush() );
}


void BrushProperty::slotColor1Changed()
{
    m_preview->setColor1( getGColor1() );
}


void BrushProperty::slotColor2Changed()
{
    m_preview->setColor2( getGColor2() );
}


void BrushProperty::slotBackColorTypeChanged()
{
    m_preview->setBackColorType( getGType() );
}


void BrushProperty::slotUnbalancedChanged()
{
    bool state = getGUnbalanced();
    setUnbalancedEnabled( state );
    m_preview->setUnbalanced( state );
}


void BrushProperty::slotXFactorChanged()
{
    m_preview->setXFactor( getGXFactor() );
}


void BrushProperty::slotYFactorChanged()
{
    m_preview->setYFactor( getGYFactor() );
}

#include "brushproperty.moc"
