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

//KOffice includes
#include <KoUnit.h>

const QString Filterkpr2odf::createPageStyle( const KoXmlElement& page )
{
    KoGenStyle style( KoGenStyle::StyleDrawingPage, "drawing-page" );
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
        style.addProperty( "draw:fill", "none" );
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
            style.addAttribute( "draw:fill", "gradient" );
            style.addProperty( "draw:fill-gradient-name", createGradientStyle( page ) );
        }
    }
    else
    {
        //it's a picture instead
        QString pictureName = getPictureNameFromKey( page.namedItem( "BACKPICTUREKEY" ).toElement() );
        KoXmlElement backView = page.namedItem( "BACKVIEW" ).toElement();
        style.addProperty( "draw:fill", "bitmap" );

        //The image is specified by a draw:fill-image style in draw:fill-image-name
        KoGenStyle drawFillImage( KoGenStyle::StyleFillImage );

        //default values
        drawFillImage.addAttribute( "xlink:href", "#Picture/" + m_pictures[ pictureName ] );
        drawFillImage.addAttribute( "xlink:type", "simple" );
        drawFillImage.addAttribute( "xlink:show", "embed" );
        drawFillImage.addAttribute( "xlink:actuate", "onLoad" );
        QString repeat;
        if( backView.isNull() )
        {
            //the picture is just centered
            repeat = "no-repeat";
        }
        else if( backView.attribute( "value" ) == "0"  )
        {
            //the picture is stretched
            repeat = "stretch";
        }
        else if( backView.attribute( "value" ) == "2" )
        {
            //picture is in mosaic
            repeat = "repeat";
        }
        style.addProperty( "style:repeat", repeat );

        style.addProperty( "draw:fill-name", m_styles.lookup( drawFillImage ) );
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
        int effect = pageEffect.attribute( "value", "0" ).toInt();
        switch( effect )
        {
        case -1:
            effectName = "random";
            break;
        case 0:
            effectName = "none";
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
            effectName = "open";
            break;
        case 7:
            effectName = "interlocking-horizontal-left";
            break;
        case 8:
            effectName = "interlocking-horizontal-right";
            break;
        case 9:
            effectName = "interlocking-horizontal-right";
            break;
        case 10:
            effectName = "interlocking-vertical-bottom";
            break;
        case 11:
            effectName = "spiralin-left";
            break;
        case 12:
            effectName = "fly-away";
            break;
        case 13:
            effectName = "horizontal-stripes";
            break;
        case 14:
            effectName = "vertical-stripes";
            break;
        case 15:
            effectName = "fade-to-center";
            break;
        case 16:
            effectName = "fade-from-center";
            break;
        case 17:
            effectName = "horizontal-checkerboard";
            break;
        case 18:
            effectName = "vertical-checkerboard";
            break;
        case 19:
            effectName = "fade-from-top";
            break;
        case 20:
            effectName = "uncover-to-bottom";
            break;
        case 21:
            effectName = "fade-from-bottom";
            break;
        case 22:
            effectName = "uncover-to-top";
            break;
        case 23:
            effectName = "fade-from-right";
            break;
        case 24:
            effectName = "uncover-to-left";
            break;
        case 25:
            effectName = "fade-from-left";
            break;
        case 26:
            effectName = "uncover-to-right";
            break;
        case 27:
            effectName = "fade-from-lowerright";
            break;
        case 28:
            effectName = "uncover-to-upperleft";
            break;
        case 29:
            effectName = "fade-from-upperrigh";
            break;
        case 30:
            effectName = "uncover-to-lowerleft";
            break;
        case 31:
            effectName = "fade-from-lowerleft";
            break;
        case 32:
            effectName = "uncover-to-upperright";
            break;
        case 33:
            effectName = "fade-from-upperleft";
            break;
        case 34:
            effectName = "fade-from-lowerleft";
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
        case 40:
            effectName = "melt";
            break;
        }//switch efect
        style.addProperty( "presentation:transition-style", effectName );
    }//if pageEfect is null

    return m_styles.lookup( style, "dp" );
}

const QString Filterkpr2odf::createGradientStyle( const KoXmlElement& gradientElement )
{
    KoGenStyle style( KoGenStyle::StyleGradient, "drawing-page" );
    style.setAutoStyleInStylesDotXml( true );

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

const QString Filterkpr2odf::createPageLayout()
{
    //Create the page-layout that is: paper, header and footer sizes
    KoXmlElement paper( m_mainDoc.namedItem( "DOC" ).namedItem( "HEADER" ).toElement() );
    KoXmlElement paperBorders( paper.namedItem( "PAPERBORDERS" ).toElement() );

    //page-layout-properties
    KoGenStyle style( KoGenStyle::StylePageLayout );
    style.setAutoStyleInStylesDotXml( true );

    style.addProperty( "fo:margin-top", QString( "%1cm" ).arg( KoUnit::toCentimeter( paperBorders.attribute( "ptTop" , "0" ).toFloat() ) ) );
    style.addProperty( "fo:margin-bottom", QString( "%1cm" ).arg( KoUnit::toCentimeter( paperBorders.attribute( "ptBottom" , "0" ).toFloat() ) ) );
    style.addProperty( "fo:margin-left", QString( "%1cm" ).arg( KoUnit::toCentimeter( paperBorders.attribute( "ptLeft" , "0" ).toFloat() ) ) );
    style.addProperty( "fo:margin-right", QString( "%1cm" ).arg( KoUnit::toCentimeter( paperBorders.attribute( "ptRight" , "0" ).toFloat() ) ) );
    style.addProperty( "fo:page-width", QString( "%1cm" ).arg( KoUnit::toCentimeter( paper.attribute( "ptWidth" , "0" ).toFloat() ) ) );
    style.addProperty( "fo:page-height", QString( "%1cm" ).arg( KoUnit::toCentimeter( paper.attribute( "ptHeight" , "0" ).toFloat() ) ) );
    style.addProperty( "fo:print-orientation", "landscape" );//FIXME: why?

    //NOTE: header-style and footer-style are not present because in KPresenter they are treated as text boxes

    return m_styles.lookup( style, "pm" );
}

const QString Filterkpr2odf::createMasterPageStyle()
{
    KoXmlElement header( m_mainDoc.namedItem( "DOC" ).namedItem( "HEADER" ).toElement() );
    KoXmlElement footer( m_mainDoc.namedItem( "DOC" ).namedItem( "FOOTER" ).toElement() );

    KoGenStyle style( KoGenStyle::StyleMaster, "" );
    style.addAttribute( "style:page-layout-name", createPageLayout() );

    return m_styles.lookup( style, "Default" );
}

