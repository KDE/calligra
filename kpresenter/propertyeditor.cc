#include "propertyeditor.h"

#include "kpobject.h"
#include "kprpage.h"
#include "kpresenter_doc.h"
#include "kpresenter_utils.h"
#include "kprcommand.h"
#include "penstylewidget.h"
#include "brushproperty.h"
#include "rectproperty.h"

#include <klocale.h>

PropertyEditor::PropertyEditor( QWidget *parent, const char *name, KPrPage *page, KPresenterDoc *doc )
    : QTabDialog( parent, name, true )
    , m_page(  page )
    , m_doc( doc )
    , m_objects( page->getSelectedObjects() )
    , m_penProperty( 0 )
    , m_brushProperty( 0 )
    , m_rectProperty( 0 )
{

    setCancelButton( i18n( "&Cancel" ) );
    setOkButton( i18n( "&OK" ) );
    setApplyButton( i18n( "&Apply" ) );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( slotDone() ) );

    setupTabs();
}


PropertyEditor::~PropertyEditor()
{
}


KCommand * PropertyEditor::getCommand()
{
    KMacroCommand *macro = 0;

    if ( m_penProperty )
    {
        int change = m_penProperty->getPenConfigChange();
        if ( change )
        {
            PenCmd::Pen pen( m_penProperty->getPen(),
                             m_penProperty->getLineBegin(),
                             m_penProperty->getLineEnd() );

            PenCmd *cmd = new PenCmd( i18n( "Apply Styles" ), m_objects, pen, m_doc, m_page, change );

            if ( !macro )
            {
                macro = new KMacroCommand( i18n( "Apply Properties" ) );
            }

            macro->addCommand( cmd );
        }
    }

    if ( m_brushProperty )
    {
        int change = m_brushProperty->getBrushPropertyChange();
        if ( change )
        {
            BrushCmd::Brush brush( m_brushProperty->getBrush(),
                                   m_brushProperty->getGColor1(),
                                   m_brushProperty->getGColor2(),
                                   m_brushProperty->getGType(),
                                   m_brushProperty->getFillType(),
                                   m_brushProperty->getGUnbalanced(),
                                   m_brushProperty->getGXFactor(),
                                   m_brushProperty->getGYFactor() );

            BrushCmd *cmd = new BrushCmd( i18n( "Apply Styles" ), m_objects, brush, m_doc, m_page, change );

            if ( !macro )
            {
                macro = new KMacroCommand( i18n( "Apply Properties" ) );
            }

            macro->addCommand( cmd );
        }
    }

    if ( m_rectProperty )
    {
        int change = m_rectProperty->getRectPropertyChange();

        if ( change )
        {
            RectValueCmd::RectValues rectValue( m_rectProperty->getRectValues() );

            RectValueCmd *cmd = new RectValueCmd( i18n( "Apply Styles" ), m_objects, rectValue, m_doc, m_page, change );

            if ( !macro )
            {
                macro = new KMacroCommand( i18n( "Apply Properties" ) );
            }

            macro->addCommand( cmd );
        }
    }

    return macro;
}


void PropertyEditor::setupTabs()
{
    int flags = getPropertyFlags( m_objects );

    if ( flags & PtPen )
        setupTabPen( flags & PtLineEnds );

    if ( flags & PtBrush )
        setupTabBrush();

    if ( flags & PtRectangle )
        setupTabRect();
}


void PropertyEditor::setupTabPen( bool configureLineEnds )
{
    if ( m_penProperty == 0 )
    {
        PenCmd::Pen pen( m_page->getPen( QPen() ),
                         m_page->getLineBegin( L_NORMAL ),
                         m_page->getLineEnd( L_NORMAL ) );

        m_penProperty = new PenStyleWidget( this, 0, pen, configureLineEnds );
        addTab( m_penProperty, i18n( "Out&line" ) );
    }
}


void PropertyEditor::setupTabBrush()
{
    if ( m_brushProperty == 0 )
    {
        BrushCmd::Brush brush( m_page->getBrush( QBrush() ),
                m_page->getGColor1( Qt::black ),
                m_page->getGColor2( Qt::red ),
                m_page->getGType( BCT_GHORZ ),
                m_page->getFillType( FT_BRUSH ),
                m_page->getGUnbalanced( false ),
                m_page->getGXFactor( 100 ),
                m_page->getGYFactor( 100 ) );

        m_brushProperty = new BrushProperty( this, 0, brush );
        addTab( m_brushProperty, i18n( "&Fill" ) );
    }
}


void PropertyEditor::setupTabRect()
{
    if ( m_rectProperty == 0 )
    {
        RectValueCmd::RectValues rectValue;
        rectValue.xRnd = m_page->getRndX( 0 );
        rectValue.yRnd = m_page->getRndY( 0 );
        m_rectProperty = new RectProperty( this, 0, rectValue );
        addTab( m_rectProperty, i18n( "&Rectangle" ) );
    }
}


void PropertyEditor::slotDone()
{
    emit propertiesOk();

    if ( m_penProperty )
        m_penProperty->apply();
    if ( m_brushProperty )
        m_brushProperty->apply();
    if ( m_rectProperty )
        m_rectProperty->apply();
}

#include "propertyeditor.moc"
