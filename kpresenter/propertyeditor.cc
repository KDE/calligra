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
            PenCmd::Pen pen( m_penProperty->getPen() );

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
            BrushCmd::Brush brush( m_brushProperty->getBrush() );

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
                    KoRect newRect = oldRect;
                    if ( change & GeneralProperty::Left )
                        newRect.moveTopLeft( KoPoint( generalValue.m_rect.left(), newRect.top() ) );

                    if ( change & GeneralProperty::Top )
                        newRect.moveTopLeft( KoPoint( newRect.left(), generalValue.m_rect.top() ) );

                    if ( change & GeneralProperty::Width )
                        newRect.setWidth( generalValue.m_rect.width() );

                    if ( change & GeneralProperty::Height )
                        newRect.setHeight( generalValue.m_rect.height() );

                    KCommand *cmd = new ResizeCmd( i18n( "Change Size" ),
                                                   newRect.topLeft() - oldRect.topLeft(),
                                                   newRect.size() - oldRect.size(),
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
        PenCmd::Pen pen( m_objectProperties->getPen() );

        m_penProperty = new PenStyleWidget( this, 0, pen, configureLineEnds );
        addTab( m_penProperty, i18n( "Outl&ine" ) );
    }
}


void PropertyEditor::setupTabBrush()
{
    if ( m_brushProperty == 0 )
    {
        BrushCmd::Brush brush( m_objectProperties->getBrush() );

        m_brushProperty = new BrushProperty( this, 0, brush );
        addTab( m_brushProperty, i18n( "&Fill" ) );
    }
}


void PropertyEditor::setupTabRect()
{
    if ( m_rectProperty == 0 )
    {
        RectValueCmd::RectValues rectValues = m_objectProperties->getRectValues();
        m_rectProperty = new RectProperty( this, 0, rectValues );
        addTab( m_rectProperty, i18n( "&Rectangle" ) );
    }
}


void PropertyEditor::setupTabPolygon()
{
    if ( m_polygonProperty == 0 )
    {
        PolygonSettingCmd::PolygonSettings polygonSettings = m_objectProperties->getPolygonSettings();
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
