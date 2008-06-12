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
#include <QHash>

const QString Filterkpr2odf::createPageStyle( const KoXmlElement& page )
{
    KoGenStyle style = KoGenStyle::KoGenStyle( KoGenStyle::StyleDrawingPage, "drawing-page" );
    KoXmlElement backMaster = page.namedItem( "BACKMASTER" ).toElement();
    if( !backMaster.isNull() )
    {
        style.addProperty( "presentation:background-visible", backMaster.attribute( "presentation:displayBackground", "1" ) == "1" );
        style.addProperty( "presentation:background-objects-visible", backMaster.attribute( "displayMasterPageObject", "1" ) == "1" );
    }
    else
    {
        //if BACKMASTER is not found we assume it's true
        style.addProperty( "presentation:background-visible", true );
        style.addProperty( "presentation:background-objects-visible", true );
    }
    if( !page.hasChildNodes() )
    {
        //we check if this is an empty page
        return m_styles.lookup( style, "dp" );
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
            style.addProperty( "draw:fill", "solid" );
            style.addProperty( "draw:fill-color", page.namedItem( "BACKCOLOR1" ).toElement().attribute("color") );
        }
        else
        {
            //background is a gradient
            style.addProperty( "draw:fill-gradient-name", createGradientStyle( page ) );
        }
    }
    else
    {
        //it's a picture instead
        QString pictureName = getPictureNameFromKey( page.namedItem( "BACKPICTUREKEY" ).toElement() );
        KoXmlElement backView = page.namedItem( "BACKVIEW" ).toElement();
        //FIXME:how do we represent this?
        if( backView.isNull() )
        {
            //the picture is just centered
        }
        else if( backView.attribute( "value" ) == "0"  )
        {
            //the picture is scaled
        }
        else if( backView.attribute( "value" ) == "2" )
        {
            //picture is in mosaic
        }
    }

    //Add the duration of the page effect
    KoXmlElement pageDuration = page.namedItem( "PGTIMER" ).toElement();
    if( !pageDuration.isNull() )
    {
        QTime time;
        time = time.addSecs( pageDuration.attribute( "timer", "0" ).toInt() );
        style.addProperty( "presentation:duration", time.toString("'P'hh'H'mm'M'ss'S'") );
        style.addProperty( "presentation:transition-type", "automatic" );
    }

    //Add the page effect
    KoXmlElement pageEffect = page.namedItem( "PGEFFECT" ).toElement();
    if ( !pageEffect.isNull() )
    {
        QString effectName;
        //TODO: fill this list! damn nothing in the dtd
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
        style.addProperty( "presentation:transition-style", effectName );
    }//if pageEfect is null

    return m_styles.lookup( style, "dp" );
}

const QString Filterkpr2odf::createGradientStyle( const KoXmlElement& gradientElement )
{
    KoGenStyle style = KoGenStyle::KoGenStyle( KoGenStyle::StyleGradient, "drawing-page" ) ;
    //KPresenter didn't allow to customize those attributes
    style.addAttribute( "draw:start-intensity", "100%" );
    style.addAttribute( "draw:end-intensity", "100%" );
    style.addAttribute( "draw:border", "0%" );
    //Check wether the gradient belongs to a page or to an object
    int type = 1;//we default to 1
    if( gradientElement.nodeName() == "PAGE" )
    {
        KoXmlElement backColor1 = gradientElement.namedItem( "BACKCOLOR1" ).toElement();
        KoXmlElement backColor2 = gradientElement.namedItem( "BACKCOLOR2" ).toElement();
        KoXmlElement bcType = gradientElement.namedItem( "BCTYPE" ).toElement();
        KoXmlElement bGradient = gradientElement.namedItem( "BGRADIENT" ).toElement();

        if( !backColor1.isNull() )
            style.addProperty( "draw:start-color", backColor1.attribute( "color" ) );
        if( !backColor2.isNull() )
            style.addProperty( "draw:start-color", backColor2.attribute( "color" ) );
        if( !bcType.isNull() )
            type = bcType.attribute( "value" ).toInt();
//         if ( !bGradient.isNull() )
//         {
//             if ( bGradient.attribute( "unbalanced" ) == "0" )
//             {
//                 gradient.setAttribute( "draw:cx", m_cx );
//                 gradient.setAttribute( "draw:cy", m_cy );
//                 m_cx = "50%";
//                 m_cy = "50%";
//             }
//             else
//             {
//                 int cx = bGradient.attribute( "xfactor" ).toInt();
//                 int cy = bGradient.attribute( "yfactor" ).toInt();
//                 m_cx = QString( "%1%" ).arg( cx / 4 + 50 );
//                 m_cy = QString( "%1%" ).arg( cy / 4 + 50 );
//             }
//         }
    }
    else
    {

    }

    //export the type of the gradient
//     switch ( type )
//     {
//     case 1:
//         m_style = "linear";
//         m_angle = "0";
//         break;
//     case 2:
//         m_style = "linear";
//         m_angle = "900";
//         break;
//     case 3:
//         m_style = "linear";
//         m_angle = "450";
//         break;
//     case 4:
//         m_style = "linear";
//         m_angle = "135";
//         break;
//     case 5:
//         m_style = "radial";
//         m_angle = "0";
//         break;
//     case 6:
//         m_style = "square";
//         m_angle = "0";
//         break;
//     case 7:
//         m_style = "axial";
//         m_angle = "0";
//         break;
//     }//switch type
}
