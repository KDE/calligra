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
            style.addProperty( "draw:fill-color", page.namedItem( "BACKCOLOR1" ).toElement().attribute( "color" ) );
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
        drawFillImage.addAttribute( "xlink:href", "Pictures/" + m_pictures[ pictureName ] );
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

        style.addProperty( "draw:fill-image-name", m_styles.lookup( drawFillImage, "picture" ) );
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
    if( !pageEffect.isNull() )
    {
        QString effectName( "none" );//we default to none
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

        //Add the sound
        KoXmlElement soundEffect = page.namedItem( "PGSOUNDEFFECT" ).toElement();
        if( !soundEffect.isNull() && soundEffect.attribute( "soundEffect" ) != "0" )
        {
            //As this is a "complex" tag we add it "manually"
            //TODO: check if we can use StyleChildElement instead
            QBuffer buffer;
            buffer.open( IO_WriteOnly );
            KoXmlWriter elementWriter( &buffer );
            elementWriter.startElement( "presentation:sound" );
            elementWriter.addAttribute( "xlink:href", "Sounds/" + m_sounds[ soundEffect.attribute( "soundFileName" ) ] );
            elementWriter.addAttribute( "xlink:type", "simple" );
            elementWriter.addAttribute( "xlink:show", "new" );
            elementWriter.addAttribute( "xlink:actuate", "onRequest");
            elementWriter.endElement();

            QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
            style.addChildElement( "presentationSound", elementContents );
        }
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
        KoXmlElement backColor1( gradientElement.namedItem( "BACKCOLOR1" ).toElement() );
        if( !backColor1.isNull() )
            style.addAttribute( "draw:start-color", backColor1.attribute( "color" ) );

        KoXmlElement backColor2( gradientElement.namedItem( "BACKCOLOR2" ).toElement() );
        if( !backColor2.isNull() )
            style.addAttribute( "draw:end-color", backColor2.attribute( "color" ) );

        KoXmlElement bcType( gradientElement.namedItem( "BCTYPE" ).toElement() );
        if( !bcType.isNull() )
            type = bcType.attribute( "value" ).toInt();

        KoXmlElement bGradient = gradientElement.namedItem( "BGRADIENT" ).toElement();
        if( !bGradient.isNull() )
        {
            if( bGradient.attribute( "unbalanced" ) == "0" )
            {
                style.addAttribute( "draw:cx", "50%" );
                style.addAttribute( "draw:cy", "50%" );
            }
            else
            {
                int cx = bGradient.attribute( "xfactor" ).toInt();
                int cy = bGradient.attribute( "yfactor" ).toInt();
                //FIXME: find out if this is still applicable and why is the calculus like that
                style.addAttribute( "draw:cx", QString( "%1%" ).arg( cx / 4 + 50 ) );
                style.addAttribute( "draw:cy", QString( "%1%" ).arg( cy / 4 + 50 ) );
            }
        }
    }
    else
    {
        style.addAttribute( "draw:start-color", gradientElement.attribute( "color1", "" ) );
        style.addAttribute( "draw:end-color", gradientElement.attribute( "color2", "" ) );
        type = gradientElement.attribute( "type" ).toInt();
        if( gradientElement.hasAttribute( "unbalanced" ) )
        {
            if( gradientElement.attribute( "unbalanced" ) == "0" )
            {
                style.addAttribute( "draw:cx", "50%" );
                style.addAttribute( "draw:cy", "50%" );
            }
            else
            {
                int cx = gradientElement.attribute( "xfactor" ).toInt();
                int cy = gradientElement.attribute( "yfactor" ).toInt();
                //FIXME: find out if this is still applicable and why is the calculus like that
                style.addAttribute( "draw:cx", QString( "%1%" ).arg( cx / 4 + 50 ) );
                style.addAttribute( "draw:cy", QString( "%1%" ).arg( cy / 4 + 50 ) );
            }
        }
    }

    //export the type of the gradient
    QString typeString;
    QString angle;
    switch( type )
    {
    case 1:
        typeString = "linear";
        angle = "0";
        break;
    case 2:
        typeString = "linear";
        angle = "900";
        break;
    case 3:
        typeString = "linear";
        angle = "450";
        break;
    case 4:
        typeString = "linear";
        angle = "135";
        break;
    case 5:
        typeString = "radial";
        angle = "0";
        break;
    case 6:
        typeString = "square";
        angle = "0";
        break;
    case 7:
        typeString = "axial";
        angle = "0";
        break;
    }//switch type
    style.addAttribute( "draw:style", typeString );
    style.addAttribute( "draw:angle", angle );

    return m_styles.lookup( style, "gr" );
}

const QString Filterkpr2odf::createPageLayout()
{
    //Create the page-layout that is: paper, header and footer sizes
    KoXmlElement paper( m_mainDoc.namedItem( "DOC" ).namedItem( "PAPER" ).toElement() );
    KoXmlElement paperBorders( paper.namedItem( "PAPERBORDERS" ).toElement() );

    //page-layout-properties
    KoGenStyle style( KoGenStyle::StylePageLayout );
    style.setAutoStyleInStylesDotXml( true );

    if( paperBorders.hasAttribute( "ptTop" ) )
    {
        style.addProperty( "fo:margin-top", QString( "%1cm" ).arg( KoUnit::toCentimeter( paperBorders.attribute( "ptTop" ).toFloat() ) ) );
    }
    if( paperBorders.hasAttribute( "ptBottom" ) )
    {
        style.addProperty( "fo:margin-bottom", QString( "%1cm" ).arg( KoUnit::toCentimeter( paperBorders.attribute( "ptBottom" ).toFloat() ) ) );
    }
    if( paperBorders.hasAttribute( "ptLeft" ) )
    {
        style.addProperty( "fo:margin-left", QString( "%1cm" ).arg( KoUnit::toCentimeter( paperBorders.attribute( "ptLeft" ).toFloat() ) ) );
    }
    if( paperBorders.hasAttribute( "ptRight" ) )
    {
        style.addProperty( "fo:margin-right", QString( "%1cm" ).arg( KoUnit::toCentimeter( paperBorders.attribute( "ptRight" ).toFloat() ) ) );
    }
    if( paper.hasAttribute( "ptWidth" ) )
    {
        style.addProperty( "fo:page-width", QString( "%1cm" ).arg( KoUnit::toCentimeter( paper.attribute( "ptWidth" ).toFloat() ) ) );
    }
    if( paper.hasAttribute( "ptHeight" ) )
    {
        style.addProperty( "fo:page-height", QString( "%1cm" ).arg( KoUnit::toCentimeter( paper.attribute( "ptHeight" ).toFloat() ) ) );
    }
    style.addProperty( "fo:print-orientation", "landscape" );

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
//TODO: load the tags still missing
const QString Filterkpr2odf::createGraphicStyle( const KoXmlElement& element )
{
    //A graphic style is wiely used by a broad type of objects, hence can have many different properties
    KoGenStyle style( KoGenStyle::StyleGraphicAuto, "graphic" );
    KoXmlElement textObject( element.namedItem( "TEXTOBJ" ).toElement() );
    if( !textObject.isNull() )
    {
        if( textObject.hasAttribute( "verticalAlign" ) )
        {
            QString textAligment = textObject.attribute( "verticalAlign" );
            if( textAligment == "center" )//all the other values are valid except center that is called middle in ODF
                textAligment = "middle";
            style.addProperty( "draw:textarea-vertical-align", textAligment );
        }
        if( textObject.hasAttribute( "bleftpt" ) )
        {
             style.addProperty( "fo:padding-left", QString( "%1pt" ).arg( textObject.attribute( "bleftpt" ) ) );
        }
        if( textObject.hasAttribute( "bbottompt" ) )
        {
            style.addProperty( "fo:padding-bottom",QString( "%1pt" ).arg( textObject.attribute( "bbottompt" ) ) );
        }
        if( textObject.hasAttribute( "btoppt" ) )
        {
            style.addProperty( "fo:padding-top", QString( "%1pt" ).arg( textObject.attribute( "btoppt" ) ) );
        }
        if( textObject.hasAttribute( "brightpt" ) )
        {
            style.addProperty( "fo:padding-right", QString( "%1pt" ).arg( textObject.attribute( "brightpt" ) ) );
        }
    }

    KoXmlElement pen( element.namedItem( "PEN" ).toElement() );
    if( !pen.isNull() )
    {
        style.addProperty( "svg:stroke-width", QString("%1cm").arg( KoUnit::toCentimeter( pen.attribute( "width" ).toDouble() ) ) );
        style.addProperty( "svg:stroke-color", pen.attribute( "color" ) );

        QString stroke;
        int strokeStyle = pen.attribute( "style" ).toInt();
        if( strokeStyle == 1 )
        {
            stroke = "solid";
        }
        else if( strokeStyle >= 2 && strokeStyle <= 5 )
        {
            stroke = "dash";
            style.addProperty( "draw:stroke-dash", createStrokeDashStyle( strokeStyle ) );
        }
        else
        {
            stroke = "none";
        }
        style.addProperty( "draw:stroke", stroke );
    }

    //We now define what's the object filled with, we "default" to a brush if both attributes are present
    KoXmlElement brush( element.namedItem( "BRUSH" ).toElement() );
    KoXmlElement gradient( element.namedItem( "GRADIENT" ).toElement() );
    QString fill;
    if( !brush.isNull() )
    {
        QString fillColor( brush.attribute( "color" ) );

        int brushStyle = brush.attribute( "style" ).toInt();
        if( brushStyle == 1 )
        {
            fill = "solid";
            style.addProperty( "draw:fill-color", fillColor );
        }
        else if( brushStyle >= 9 && brushStyle <= 14 )
        {
            fill = "hatch";
            style.addProperty( "draw:fill-hatch-name",  createHatchStyle( brushStyle, fillColor ) );
        }
        else if( brushStyle >= 2 && brushStyle <= 8 )
        {
            //in KPR files transparency was used in ODF opacity is used instead calculated as 100-transparency
            int opacity;
            if( brushStyle == 2 )
                opacity = 6;
            else if( brushStyle == 3 )
                opacity = 12;
            else if( brushStyle == 4 )
                opacity = 37;
            else if( brushStyle == 5 )
                opacity = 50;
            else if( brushStyle == 6 )
                opacity = 63;
            else if( brushStyle == 7 )
                opacity = 88;
            else if( brushStyle == 8 )
                opacity = 94;
            //Opacity is a gradient, in this case an uniform one
            style.addProperty( "draw:opacity", createOpacityGradientStyle( opacity ) );
        }
    }
    else if( !gradient.isNull() )
    {
        fill = "gradient";
        style.addProperty( "draw:fill-gradient-name", createGradientStyle( gradient ) );
    }
    else
    {
        fill = "none";
    }

    if( !fill.isNull() ) //don't save ""
    {
        style.addProperty( "draw:fill", fill );
    }

    KoXmlElement lineBegin = element.namedItem( "LINEBEGIN" ).toElement();
    if( !lineBegin.isNull() )
    {
        style.addProperty( "draw:marker-start-width", "0.25cm" );

        int markerStyle = lineBegin.attribute( "value" ).toInt();
        style.addProperty( "draw:marker-start", createMarkerStyle( markerStyle ) );
    }

    KoXmlElement lineEnd = element.namedItem( "LINEEND" ).toElement();
    if( !lineEnd.isNull() )
    {
        style.addProperty( "draw:marker-end-width", "0.25cm" );

        int markerStyle = lineEnd.attribute( "value" ).toInt();
        style.addProperty( "draw:marker-end", createMarkerStyle( markerStyle ) );
    }

    KoXmlElement shadow = element.namedItem( "SHADOW" ).toElement();
    if( !shadow.isNull() )
    {
        style.addProperty( "draw:shadow", "visible" );
        style.addProperty( "draw:shadow-color", shadow.attribute( "color" ) );

        QString shadowOffsetX;
        QString shadowOffsetY;
        int direction = shadow.attribute( "direction" ).toInt();
        QString distance = QString( "%1cm" ).arg( KoUnit::toCentimeter( shadow.attribute( "distance" ).toDouble() ) );
        switch( direction )
        {
        case 1: //Left Up
            shadowOffsetX = '-' + distance;
            shadowOffsetY = '-' + distance;
            break;
        case 2: //Up
            shadowOffsetX = "0cm";
            shadowOffsetY = '-' + distance;
            break;
        case 3: //Right Up
            shadowOffsetX = distance;
            shadowOffsetY = '-' + distance;
            break;
        case 4: //Right
            shadowOffsetX = distance;
            shadowOffsetY = "0cm";
            break;
        case 5: //Right Bottom
            shadowOffsetX = distance;
            shadowOffsetY = distance;
            break;
        case 6: //Bottom
            shadowOffsetX = "0cm";
            shadowOffsetY = distance;
            break;
        case 7: //Left Bottom
            shadowOffsetX = '-' + distance;
            shadowOffsetY = distance;
            break;
        case 8: //Left
            shadowOffsetX = '-' + distance;
            shadowOffsetY = "0cm";
            break;
        }
        style.addProperty( "draw:shadow-offset-x", shadowOffsetX );
        style.addProperty( "draw:shadow-offset-y", shadowOffsetY );
    }
//     if( m_name != "standard" )
//         style.setAttribute( "style:parent-style-name", "standard" ); FIXME: what do we do?

    return m_styles.lookup( style, "gr" );
}

const QString Filterkpr2odf::createOpacityGradientStyle( int opacity )
{
    //Opacity wasn't a gradient in KPR so we go from and to the same value
    KoGenStyle style( KoGenStyle::StyleOpacity );
    QString opacityString = QString("%1%").arg( opacity );
    style.addAttribute( "draw:start", opacityString );
    style.addAttribute( "draw:end", opacityString );
    return m_styles.lookup( style, "op" );
}

const QString Filterkpr2odf::createMarkerStyle( int markerType )
{
    KoGenStyle style( KoGenStyle::StyleMarker );

    QString displayName;
    QString viewBox;
    QString d;

    switch( markerType )
    {
    case 0: //Normal
        //nothing
        break;
    case 1:
        displayName = "Arrow";
        viewBox = "0 0 20 30";
        d = "m10 0-10 30h20z";
        break;
    case 2:
        displayName = "Square";
        viewBox = "0 0 10 10";
        d = "m0 0h10v10h-10z";
        break;
    case 3:
        displayName = "Circle";
        viewBox = "0 0 1131 1131";
        d = "m462 1118-102-29-102-51-93-72-72-93-51-102-29-102-13-105 13-102 29-106 51-102 72-89 93-72 102-50 102-34 106-9 101 9 106 34 98 50 93 72 72 89 51 102 29 106 13 102-13 105-29 102-51 102-72 93-93 72-98 51-106 29-101 13z";
        break;
    case 4:
        displayName = "Line Arrow";
        viewBox = "0 0 1122 2243";
        d = "m0 2108v17 17l12 42 30 34 38 21 43 4 29-8 30-21 25-26 13-34 343-1532 339 1520 13 42 29 34 39 21 42 4 42-12 34-30 21-42v-39-12l-4 4-440-1998-9-42-25-39-38-25-43-8-42 8-38 25-26 39-8 42z";
        break;
    case 5:
        displayName = "Dimension Lines";
        viewBox = "0 0 836 110";
        d = "m0 0h278 278 280v36 36 38h-278-278-280v-36-36z";
        break;
    case 6:
        displayName = "Doble Arrow";
        viewBox = "0 0 1131 1918";//FIXME: same as Double line arrow, not sure if it's ok, nothing in KPresenter1.6
        d = "m737 1131h394l-564-1131-567 1131h398l-398 787h1131z";
        break;
    case 7:
        displayName = "Double Line Arrow";
        viewBox = "0 0 1131 1918";
        d = "m0 11h312 312h122z";
        break;
    }//switch markerType

    style.addAttribute( "draw:display-name", displayName );
    style.addAttribute( "draw:viewBox", viewBox );
    style.addAttribute( "draw:d", d );

    return m_styles.lookup( style, "mks" );;
}

const QString Filterkpr2odf::createStrokeDashStyle( int strokeStyle )
{
    KoGenStyle style( KoGenStyle::StyleStrokeDash );

    //"Containment" strings, filled according to the type of the strokeStyle
    QString displayName;
    QString styleString;
    QString dots1;
    QString dots1_length;
    QString dots2;
    QString dots2_length;
    QString distance;

    switch( strokeStyle )
    {
    case 0:
    case 1:
    {
        //"Empty style"
        return m_styles.lookup( style, "sds" );;
    }
    case 2:
        displayName = "Fine Dashed";
        styleString = "rect";
        dots1 = "1";
        dots1_length = "0.508cm";
        dots2 = "1";
        dots2_length = "0.508cm";
        distance = "0.508cm";
        break;
    case 3:
        displayName = "Fine Dotted";
        styleString = "rect";
        dots1 = "1";
        distance = "0.257cm";
        break;
    case 4:
        displayName = "Ultrafine 1 Dot 1 Dash";
        styleString = "rect";
        dots1 = "1";
        dots1_length = "0.051cm";
        dots2 = "1";
        dots2_length = "0.254cm";
        distance = "0.127cm";
        break;
    case 5:
        displayName = "2 Dots 1 Dash";
        styleString = "rect";
        dots1 = "2";
        dots2 = "1";
        dots2_length = "0.203cm";
        distance = "0.203cm";
        break;
    }//switch strokeStyle

    //Not all the strings are filled always so in oder to not
    //flood the style with unneeded "", we check if it was written
    style.addAttribute( "draw:style", styleString );
    style.addAttribute( "draw:dots1", dots1 );
    style.addAttribute( "draw:distance", distance );
    if( !dots1_length.isNull() )
    {
        style.addAttribute( "draw:dots1-length", dots1_length );
    }
    if( !dots2.isNull() )
    {
        style.addAttribute( "draw:dots2", dots2 );
    }
    if( !dots2_length.isNull() )
    {
        style.addAttribute( "draw:dots2-length", dots2_length );
    }

    return m_styles.lookup( style, "sds" );
}

const QString Filterkpr2odf::createHatchStyle( int brushStyle, QString fillColor )
{
    KoGenStyle style( KoGenStyle::StyleHatch );

    //"Contaimnet" strings
    QString displayName;
    QString styleString;
    QString distance;
    QString rotation;

    //Other numbers aren't needed because those aren't a hatch style
    switch ( brushStyle )
    {
    case 9:
        displayName = fillColor + " 0 Degrees";
        styleString = "single";
        distance = "0.102cm";
        rotation = "0";
        break;
    case 10:
        displayName = fillColor + " 90 Degrees";
        styleString = "single";
        distance = "0.102cm";
        rotation = "900";
        break;
    case 11:
        displayName = fillColor + " Crossed 0 Degrees";
        styleString = "double";
        distance = "0.076cm";
        rotation = "900";
        break;
    case 12:
        displayName = fillColor + " 45 Degrees";
        styleString = "single";
        distance = "0.102cm";
        rotation = "450";
        break;
    case 13:
        displayName = fillColor + " -45 Degrees";
        styleString = "single";
        distance = "0.102cm";
        rotation = "3150";
        break;
    case 14:
        displayName = fillColor + " Crossed 45 Degrees";
        styleString = "double";
        distance = "0.076cm";
        rotation = "450";
        break;
    }

    style.addAttribute( "draw:display-name", displayName );
    style.addAttribute( "draw:style", styleString );
    style.addAttribute( "draw:color", fillColor );
    style.addAttribute( "draw:distance", distance );
    style.addAttribute( "draw:rotation", rotation );

    return m_styles.lookup( style, "hs" );
}
