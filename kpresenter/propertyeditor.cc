// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include "propertyeditor.h"

#include "kpobject.h"
#include "kprpage.h"
#include "kpresenter_doc.h"
#include "kpresenter_utils.h"
#include "kprcommand.h"
#include "penstylewidget.h"
#include "brushproperty.h"
#include "rectproperty.h"
#include "polygonproperty.h"
#include "pieproperty.h"
#include "pictureproperty.h"
#include "textproperty.h"
#include "kpobjectproperties.h"

#include <klocale.h>

PropertyEditor::PropertyEditor( QWidget *parent, const char *name, KPrPage *page, KPresenterDoc *doc )
    : QTabDialog( parent, name, true )
    , m_page( page )
    , m_doc( doc )
    , m_objects( page->getSelectedObjects() )
    , m_penProperty( 0 )
    , m_brushProperty( 0 )
    , m_rectProperty( 0 )
    , m_polygonProperty( 0 )
    , m_pieProperty( 0 )
    , m_pictureProperty( 0 )
    , m_textProperty( 0 )
    , m_generalProperty( 0 )
    , m_objectProperties( 0 )
{
    setCancelButton( i18n( "&Cancel" ) );
    setOkButton( i18n( "&OK" ) );
    setApplyButton( i18n( "&Apply" ) );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( slotDone() ) );

    m_objectProperties = new KPObjectProperties( m_objects );

    setupTabs();
}


PropertyEditor::~PropertyEditor()
{
    delete m_objectProperties;
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

    if ( m_polygonProperty )
    {
        int change = m_polygonProperty->getPolygonPropertyChange();

        if ( change )
        {
            PolygonSettingCmd::PolygonSettings polygonSettings( m_polygonProperty->getPolygonSettings() );

            PolygonSettingCmd *cmd = new PolygonSettingCmd( i18n("Apply Styles"), polygonSettings,
                                                            m_objects, m_doc, m_page, change );

            if ( !macro )
            {
                macro = new KMacroCommand( i18n( "Apply Properties" ) );
            }

            macro->addCommand( cmd );
        }
    }

    if ( m_pieProperty )
    {
        int change = m_pieProperty->getPiePropertyChange();

        if ( change )
        {
            PieValueCmd *cmd = new PieValueCmd( i18n("Apply Styles"), m_pieProperty->getPieValues(),
                                                m_objects, m_doc, m_page, change );
            if ( !macro )
            {
                macro = new KMacroCommand( i18n( "Apply Properties" ) );
            }

            macro->addCommand( cmd );
        }
    }

    if ( m_pictureProperty )
    {
        int change = m_pictureProperty->getPicturePropertyChange();

        if ( change )
        {
            PictureSettingCmd *cmd = new PictureSettingCmd( i18n("Apply Styles"), m_pictureProperty->getPictureSettings(),
                                                            m_objects, m_doc, m_page, change );
            if ( !macro )
            {
                macro = new KMacroCommand( i18n( "Apply Properties" ) );
            }

            macro->addCommand( cmd );
        }
    }

    if ( m_textProperty )
    {
        int change = m_textProperty->getTextPropertyChange();

        if ( change )
        {
            if ( change & TextProperty::ProtectContent )
            {
                KPrProtectContentCommand * cmd = new KPrProtectContentCommand( i18n( "Apply Styles" ), m_objects,
                                                                               m_textProperty->getProtectContent(),
                                                                               m_doc );
                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                macro->addCommand( cmd );
            }

            if ( change & TextProperty::Margins )
            {
                KPrChangeMarginCommand *cmd = new KPrChangeMarginCommand( i18n( "Apply Styles" ), m_objects,
                                                                          m_textProperty->getMarginsStruct(),
                                                                          m_doc, m_page );

                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                macro->addCommand( cmd );
            }
        }
    }

    if ( m_generalProperty )
    {
        int change = m_generalProperty->getGeneralPropertyChange();

        if ( change )
        {
            GeneralProperty::GeneralValue generalValue( m_generalProperty->getGeneralValue() );

            if ( change & GeneralProperty::Name )
            {
                KCommand *cmd = new KPrNameObjectCommand( i18n( "Name Object" ), generalValue.m_name,
                                                          m_objects.at( 0 ), m_doc );

                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                macro->addCommand( cmd );
            }

            if ( change & GeneralProperty::Protect )
            {
                KCommand *cmd= new KPrGeometryPropertiesCommand( i18n( "Protect Object" ), m_objects,
                                                                 generalValue.m_protect == STATE_ON,
                                                                 KPrGeometryPropertiesCommand::ProtectSize );

                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                macro->addCommand( cmd );
            }

            if ( change & GeneralProperty::KeepRatio )
            {
                KCommand *cmd= new KPrGeometryPropertiesCommand( i18n( "Keep Ratio" ), m_objects,
                                                                 generalValue.m_keepRatio == STATE_ON,
                                                                 KPrGeometryPropertiesCommand::KeepRatio );
                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                macro->addCommand( cmd );
            }

            if ( change & GeneralProperty::Left
                 || change & GeneralProperty::Top
                 || change & GeneralProperty::Width
                 || change & GeneralProperty::Height )
            {
                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                QPtrListIterator<KPObject> it( m_objects );
                for ( ; it.current(); ++it )
                {
                    KoRect oldRect = it.current()->getRect();
                    if ( ! ( change & GeneralProperty::Left ) )
                        generalValue.m_rect.moveTopLeft( KoPoint( oldRect.left(), generalValue.m_rect.top() ) );

                    if ( ! ( change & GeneralProperty::Top ) )
                        generalValue.m_rect.moveTopLeft( KoPoint( generalValue.m_rect.left(), oldRect.top() ) );

                    if ( ! ( change & GeneralProperty::Width ) )
                        generalValue.m_rect.setWidth( oldRect.width() );

                    if ( ! ( change & GeneralProperty::Height ) )
                        generalValue.m_rect.setHeight( oldRect.height() );

                    KCommand *cmd = new ResizeCmd( i18n( "Change Size" ),
                                                   generalValue.m_rect.topLeft() - oldRect.topLeft(),
                                                   generalValue.m_rect.size() - oldRect.size(),
                                                   it.current(), m_doc );

                    macro->addCommand( cmd );
                }
            }
        }
    }

    return macro;
}


void PropertyEditor::setupTabs()
{
    int flags = m_objectProperties->getPropertyFlags();

    if ( flags & KPObjectProperties::PtPen )
        setupTabPen( flags & KPObjectProperties::PtLineEnds );

    if ( flags & KPObjectProperties::PtBrush )
        setupTabBrush();

    if ( flags & KPObjectProperties::PtRectangle )
        setupTabRect();

    if ( flags & KPObjectProperties::PtPolygon )
        setupTabPolygon();

    if ( flags & KPObjectProperties::PtPie )
        setupTabPie();

    if ( flags & KPObjectProperties::PtPicture )
        setupTabPicture();

    if ( flags & KPObjectProperties::PtText )
        setupTabText();

    setupTabGeneral();
}


void PropertyEditor::setupTabPen( bool configureLineEnds )
{
    if ( m_penProperty == 0 )
    {
        PenCmd::Pen pen( m_page->getPen( QPen() ),
                         m_page->getLineBegin( L_NORMAL ),
                         m_page->getLineEnd( L_NORMAL ) );

        m_penProperty = new PenStyleWidget( this, 0, pen, configureLineEnds );
        addTab( m_penProperty, i18n( "Outl&ine" ) );
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


void PropertyEditor::setupTabPolygon()
{
    if ( m_polygonProperty == 0 )
    {
        PolygonSettingCmd::PolygonSettings polygonSettings;
        polygonSettings.checkConcavePolygon = m_page->getCheckConcavePolygon( false );
        polygonSettings.cornersValue = m_page->getCornersValue( 3 );
        polygonSettings.sharpnessValue = m_page->getSharpnessValue( 0 );
        m_polygonProperty = new PolygonProperty( this, 0, polygonSettings );
        addTab( m_polygonProperty, i18n("Polygo&n" ) );
    }
}


void PropertyEditor::setupTabPie()
{
    if ( m_pieProperty == 0 )
    {
        m_pieProperty = new PieProperty( this, 0, m_objectProperties->getPieValues() );
        addTab( m_pieProperty, i18n("&Pie" ) );
    }
}


void PropertyEditor::setupTabPicture()
{
    if ( m_pictureProperty == 0 )
    {
        m_pictureProperty = new PictureProperty( this, 0, m_objectProperties->getPixmap(), m_objectProperties->getPictureSettings() );
        addTab( m_pictureProperty, i18n("Pict&ure" ) );
    }
}


void PropertyEditor::setupTabText()
{
    if ( m_textProperty == 0 )
    {
        m_textProperty = new TextProperty( this, 0, m_objectProperties->getMarginsStruct(),
                                           m_doc->getUnit(), m_objectProperties->getProtectContent() );
        addTab( m_textProperty, i18n("Te&xt" ) );
    }
}


void PropertyEditor::setupTabGeneral()
{
    if ( m_generalProperty == 0 )
    {
        GeneralProperty::GeneralValue generalValue = getGeneralValue();
        m_generalProperty = new GeneralProperty( this, 0, generalValue, m_doc->getUnit() );
        addTab( m_generalProperty, i18n( "&General" ) );
    }
}


GeneralProperty::GeneralValue PropertyEditor::getGeneralValue()
{
    GeneralProperty::GeneralValue generalValue;

    if ( m_objects.count() == 1 )
    {
        generalValue.m_name = m_objects.at( 0 )->getObjectName();
    }

    bool protect = false;
    generalValue.m_protect = STATE_OFF;
    bool keepRatio = false;
    generalValue.m_keepRatio = STATE_OFF;

    QPtrListIterator<KPObject> it( m_objects );
    if ( it.current() )
    {
        protect = it.current()->isProtect();
        generalValue.m_protect = protect ? STATE_ON : STATE_OFF;
        keepRatio = it.current()->isKeepRatio();
        generalValue.m_keepRatio = keepRatio ? STATE_ON : STATE_OFF;
        generalValue.m_rect = it.current()->getRect();
        ++it;
    }

    for ( ; it.current(); ++it )
    {
        if ( protect != it.current()->isProtect() )
        {
            generalValue.m_protect = STATE_UNDEF;
            if ( generalValue.m_keepRatio == STATE_UNDEF )
            {
                break;
            }
        }

        if ( keepRatio != it.current()->isKeepRatio() )
        {
            generalValue.m_keepRatio = STATE_UNDEF;
            if ( generalValue.m_protect == STATE_UNDEF )
            {
                break;
            }
        }
    }

    return generalValue;
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
    if ( m_polygonProperty )
        m_polygonProperty->apply();
    if ( m_pieProperty )
        m_pieProperty->apply();
    if ( m_pictureProperty )
        m_pictureProperty->apply();
    if ( m_textProperty )
        m_textProperty->apply();
    if ( m_generalProperty )
        m_generalProperty->apply();
}

#include "propertyeditor.moc"
