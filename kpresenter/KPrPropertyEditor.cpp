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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "KPrPropertyEditor.h"

#include "KPrObject.h"
#include "KPrPage.h"
#include "KPrDocument.h"
#include "KPrUtils.h"
#include "KPrCommand.h"
#include "KPrPenStyleWidget.h"
#include "KPrBrushProperty.h"
#include "KPrRectProperty.h"
#include "KPrPolygonProperty.h"
#include "KPrPieProperty.h"
#include "KPrPictureProperty.h"
#include "KPrTextProperty.h"
#include "KPrObjectProperties.h"

#include <klocale.h>
#include <kstdguiitem.h>

KPrPropertyEditor::KPrPropertyEditor( QWidget *parent, const char *name, KPrPage *page, KPrDocument *doc )
    : Q3TabDialog( parent, name, true )
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
    setCancelButton( KStdGuiItem::cancel().text() );
    setOkButton( KStdGuiItem::ok().text() );
    setApplyButton( KStdGuiItem::apply().text() );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( slotDone() ) );

    m_objectProperties = new KPrObjectProperties( m_objects );

    setupTabs();
}


KPrPropertyEditor::~KPrPropertyEditor()
{
    delete m_objectProperties;
}


KCommand * KPrPropertyEditor::getCommand()
{
    KMacroCommand *macro = 0;

    if ( m_penProperty )
    {
        int change = m_penProperty->getPenConfigChange();
        if ( change )
        {
            KoPenCmd::Pen pen( m_penProperty->getPen() );

            KoPenCmd *cmd = new KoPenCmd( i18n( "Apply Styles" ), m_objects, pen, m_doc, m_page, change );

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
            KPrBrushCmd::Brush brush( m_brushProperty->getBrush() );

            KPrBrushCmd *cmd = new KPrBrushCmd( i18n( "Apply Styles" ), m_objects, brush, m_doc, m_page, change );

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
            KPrRectValueCmd::RectValues rectValue( m_rectProperty->getRectValues() );

            KPrRectValueCmd *cmd = new KPrRectValueCmd( i18n( "Apply Styles" ), m_objects, rectValue, m_doc, m_page, change );

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
            KPrPolygonSettingCmd::PolygonSettings polygonSettings( m_polygonProperty->getPolygonSettings() );

            KPrPolygonSettingCmd *cmd = new KPrPolygonSettingCmd( i18n("Apply Styles"), polygonSettings,
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
            KPrPieValueCmd *cmd = new KPrPieValueCmd( i18n("Apply Styles"), m_pieProperty->getPieValues(),
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
            KPrPictureSettingCmd *cmd = new KPrPictureSettingCmd( i18n("Apply Styles"), m_pictureProperty->getPictureSettings(),
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
            if ( change & KPrTextProperty::ProtectContent )
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

            if ( change & KPrTextProperty::Margins )
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
            KPrGeneralProperty::GeneralValue generalValue( m_generalProperty->getGeneralValue() );

            if ( change & KPrGeneralProperty::Name )
            {
                KCommand *cmd = new KPrNameObjectCommand( i18n( "Name Object" ), generalValue.m_name,
                                                          m_objects.at( 0 ), m_doc );

                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                macro->addCommand( cmd );
            }

            if ( change & KPrGeneralProperty::Protect )
            {
                KCommand *cmd= new KPrGeometryPropertiesCommand( i18n( "Protect Object" ), m_objects,
                                                                 generalValue.m_protect == STATE_ON,
                                                                 KPrGeometryPropertiesCommand::ProtectSize,m_doc );

                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                macro->addCommand( cmd );
            }

            if ( change & KPrGeneralProperty::KeepRatio )
            {
                KCommand *cmd= new KPrGeometryPropertiesCommand( i18n( "Keep Ratio" ), m_objects,
                                                                 generalValue.m_keepRatio == STATE_ON,
                                                                 KPrGeometryPropertiesCommand::KeepRatio,m_doc );
                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                macro->addCommand( cmd );
            }

            if ( change & KPrGeneralProperty::Left
                 || change & KPrGeneralProperty::Top
                 || change & KPrGeneralProperty::Width
                 || change & KPrGeneralProperty::Height )
            {
                if ( !macro )
                {
                    macro = new KMacroCommand( i18n( "Apply Properties" ) );
                }

                Q3PtrListIterator<KPrObject> it( m_objects );
                for ( ; it.current(); ++it )
                {
                    KoRect oldRect = it.current()->getRect();
                    KoRect newRect = oldRect;
                    if ( change & KPrGeneralProperty::Left )
                        newRect.moveTopLeft( KoPoint( generalValue.m_rect.left(), newRect.top() ) );

                    if ( change & KPrGeneralProperty::Top )
                        newRect.moveTopLeft( KoPoint( newRect.left(), generalValue.m_rect.top() ) );

                    if ( change & KPrGeneralProperty::Width )
                        newRect.setWidth( generalValue.m_rect.width() );

                    if ( change & KPrGeneralProperty::Height )
                        newRect.setHeight( generalValue.m_rect.height() );

                    KCommand *cmd = new KPrResizeCmd( i18n( "Change Size" ),
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


void KPrPropertyEditor::setupTabs()
{
    setupTabGeneral();

    int flags = m_objectProperties->getPropertyFlags();

    if ( flags & KPrObjectProperties::PtPen )
        setupTabPen( flags & KPrObjectProperties::PtLineEnds );

    if ( flags & KPrObjectProperties::PtBrush )
        setupTabBrush();

    if ( flags & KPrObjectProperties::PtRectangle )
        setupTabRect();

    if ( flags & KPrObjectProperties::PtPolygon )
        setupTabPolygon();

    if ( flags & KPrObjectProperties::PtPie )
        setupTabPie();

    if ( flags & KPrObjectProperties::PtPicture )
        setupTabPicture();

    if ( flags & KPrObjectProperties::PtText )
        setupTabText();
}


void KPrPropertyEditor::setupTabPen( bool configureLineEnds )
{
    if ( m_penProperty == 0 )
    {
        KoPenCmd::Pen pen( m_objectProperties->getPen() );

        m_penProperty = new KPrPenStyleWidget( this, 0, pen, configureLineEnds );
        addTab( m_penProperty, i18n( "Outl&ine" ) );
    }
}


void KPrPropertyEditor::setupTabBrush()
{
    if ( m_brushProperty == 0 )
    {
        KPrBrushCmd::Brush brush( m_objectProperties->getBrush() );

        m_brushProperty = new KPrBrushProperty( this, 0, brush );
        addTab( m_brushProperty, i18n( "&Fill" ) );
    }
}


void KPrPropertyEditor::setupTabRect()
{
    if ( m_rectProperty == 0 )
    {
        KPrRectValueCmd::RectValues rectValues = m_objectProperties->getRectValues();
        m_rectProperty = new KPrRectProperty( this, 0, rectValues );
        addTab( m_rectProperty, i18n( "&Rectangle" ) );
    }
}


void KPrPropertyEditor::setupTabPolygon()
{
    if ( m_polygonProperty == 0 )
    {
        KPrPolygonSettingCmd::PolygonSettings polygonSettings = m_objectProperties->getPolygonSettings();
        m_polygonProperty = new KPrPolygonProperty( this, 0, polygonSettings );
        addTab( m_polygonProperty, i18n("Polygo&n" ) );
    }
}


void KPrPropertyEditor::setupTabPie()
{
    if ( m_pieProperty == 0 )
    {
        m_pieProperty = new KPrPieProperty( this, 0, m_objectProperties->getPieValues() );
        addTab( m_pieProperty, i18n("&Pie" ) );
    }
}


void KPrPropertyEditor::setupTabPicture()
{
    if ( m_pictureProperty == 0 )
    {
        m_pictureProperty = new KPrPictureProperty( this, 0, m_objectProperties->getPixmap(), m_objectProperties->getPictureSettings() );
        addTab( m_pictureProperty, i18n("Pict&ure" ) );
    }
}


void KPrPropertyEditor::setupTabText()
{
    if ( m_textProperty == 0 )
    {
        m_textProperty = new KPrTextProperty( this, 0, m_objectProperties->getMarginsStruct(),
                                           m_doc->unit(), m_objectProperties->getProtectContent() );
        addTab( m_textProperty, i18n("Te&xt" ) );
    }
}


void KPrPropertyEditor::setupTabGeneral()
{
    if ( m_generalProperty == 0 )
    {
        KPrGeneralProperty::GeneralValue generalValue = getGeneralValue();
        m_generalProperty = new KPrGeneralProperty( this, 0, generalValue, m_doc->unit() );
        addTab( m_generalProperty, i18n( "&General" ) );
    }
}


KPrGeneralProperty::GeneralValue KPrPropertyEditor::getGeneralValue()
{
    KPrGeneralProperty::GeneralValue generalValue;

    if ( m_objects.count() == 1 )
    {
        generalValue.m_name = m_objects.at( 0 )->getObjectName();
    }

    bool protect = false;
    generalValue.m_protect = STATE_OFF;
    bool keepRatio = false;
    generalValue.m_keepRatio = STATE_OFF;

    Q3PtrListIterator<KPrObject> it( m_objects );
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


void KPrPropertyEditor::slotDone()
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

#include "KPrPropertyEditor.moc"
