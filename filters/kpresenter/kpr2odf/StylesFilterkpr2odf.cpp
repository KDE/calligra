/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

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
   Boston, MA  02110-1301  USA.
*/

//Qt's includes
#include <QTime>

const QString Filterkpr2odf::createPageStyle( const KoXmlElement& page )
{
    KoGenStyle style = KoGenStyle::KoGenStyle( KoGenStyle::StyleDrawingPage, "drawing-page" );
    KoXmlElement backMaster = page.namedItem( "BACKMASTER" ).toElement();
    if( !backMaster.isNull() ) {
        style.addProperty( "presentation:background-visible", backMaster.attribute( "presentation:displayBackground", "1" ) == "1" );
        style.addProperty( "presentation:background-objects-visible", backMaster.attribute( "displayMasterPageObject", "1" ) == "1" );
    }
    else { //if BACKMASTER is not found we assume it's true
        style.addProperty( "presentation:background-visible", true );
        style.addProperty( "presentation:background-objects-visible", true );
    }
    if( !page.hasChildNodes() ) {
        //we check if this is an empty page
        return m_styles->lookup( style, "dp" );
    }
    //it is not an empty page let's keep loading
    KoXmlElement backType = page.namedItem( "BACKTYPE" ).toElement();
    if( backType.isNull() || backType.attribute( "value" ) == "0" )
    {
        //it's some form of color, plain or a gradient
        KoXmlElement bcType = page.namedItem( "BCTYPE" ).toElement();
        if( backType.isNull() || backType.attribute( "value" ) == "0" )
        {
            //background is a plain color
            style.addAttribute( "draw:fill", "solid" );
            style.addAttribute( "draw:fill-color", page.namedItem( "BACKCOLOR1" ).toElement().attribute("color") );
        }
        else
        {
            //TODO: background is a gradient
            style.addAttribute( "draw:fill-gradient-name", createGradientStyle( page ) );
        }
    }
    else
    {
        //it's a picture instead
        KoXmlElement backtype = page.namedItem( "BACKTYPE").toElement();
        KoXmlElement backPictureKey = page.namedItem( "BACKPICTUREKEY" ).toElement();
    }

    //Add the duration of the page effect
    KoXmlElement pageDuration = page.namedItem( "PGTIMER" ).toElement();
    if( !pageDuration.isNull() )
    {
        QTime time;
        time = time.addSecs( pageDuration.attribute( "timer", "0" ).toInt() );
        style.addAttribute( "presentation:duration", time.toString("'P'hh'H'mm'M'ss'S'") );
        style.addAttribute( "presentation:transition-type", "automatic" );
    }

    //Add the page effect
    KoXmlElement pageEffect = page.namedItem( "PGEFFECT" ).toElement();
    if ( !pageEffect.isNull() )
    {
        QString effectName;
        //TODO: fill this list! damn nothing in th dtd
        int effect = pageEffect.attribute( "value", "0" ).toInt();
        switch( effect )
        {
        case -1:
            effectName = "random";
            break;
        case 1:
            effectName = "close-vertical";
            break;
        case 2:
            effectName = "close-horizontal";
            break;
        case 3:
            effectName = "fade-to-center";
            break;
        case 4:
            effectName = "open-vertical";
            break;
        case 5:
            effectName = "open-horizontal";
            break;
        case 6:
            effectName = "fade-from-center";
            break;
        case 11:
            effectName = "spiralin-left";
            break;
        case 19:
            effectName = "fade-from-top";
            break;
        case 21:
            effectName = "fade-from-bottom";
            break;
        case 22:
            effectName = "roll-from-bottom";
            break;
        case 24:
            effectName = "roll-from-right";
            break;
        case 35:
            effectName = "dissolve";
            break;
        case 36:
            effectName = "fade-from-lowerright";
            break;
        case 37:
            effectName = "fade-from-upperright";
            break;
        case 38:
            effectName = "fade-from-lowerleft";
            break;
        case 39:
            effectName = "fade-from-upperleft";
            break;
        }//switch efect
        style.addAttribute( "presentation:transition-style", effectName );
    }//if pageEfect is null

    return m_styles->lookup( style, "dp" );
}

const QString Filterkpr2odf::createGradientStyle( const KoXmlElement& page )
{
}
