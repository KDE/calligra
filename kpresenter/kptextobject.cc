/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kptextobject.h"
#include "kpgradient.h"
#include "resizecmd.h"

#include <qwidget.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include <qapplication.h>

#include <klocale.h>
#include <kdebug.h>

#include "kpresenter_view.h"
#include "kpresenter_doc.h"
#include <iostream>
using namespace std;

/******************************************************************/
/* Class: KPTextObject                                            */
/******************************************************************/

/*================ default constructor ===========================*/
KPTextObject::KPTextObject(  KPresenterDoc *doc )
    : KPObject(), ktextobject( doc, this, 0, "" )
{
    ktextobject.hide();
    brush = Qt::NoBrush;
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    gColor1 = Qt::red;
    gColor2 = Qt::green;
    drawEditRect = true;
    drawEmpty = true;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;
}

/*================================================================*/
//KPTextObject &KPTextObject::operator=( const KPTextObject & )
//{
//    return *this;
//}

/*======================= set size ===============================*/
void KPTextObject::setSize( int _width, int _height )
{
//     if ( QSize( _width, _height ) == ext )
//      return;

    KPObject::setSize( _width, _height );
    if ( move )
        return;
    ktextobject.resize( ext );
    // hack, somehow se should rather get rid of the delayed resize in KTextEdit
    QApplication::sendPostedEvents( &ktextobject, QEvent::Resize );
    // WTH ? This creates the ficlker when going fullscreen ! (DF)
    // qApp->processEvents();

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*======================= set size ===============================*/
void KPTextObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move )
        return;
    ktextobject.resize( ext );
    QApplication::sendPostedEvents( &ktextobject, QEvent::Resize );
    //qApp->processEvents(); // hack, somehow se should rather get rid of the delayed resize in KTextEdit

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*================================================================*/
void KPTextObject::setFillType( FillType _fillType )
{
    fillType = _fillType;

    if ( fillType == FT_BRUSH && gradient )
    {
        delete gradient;
        gradient = 0;
    }
    if ( fillType == FT_GRADIENT && !gradient )
        gradient = new KPGradient( gColor1, gColor2, gType, getSize(), unbalanced, xfactor, yfactor );
}

/*========================= save =================================*/
void KPTextObject::save( QTextStream& out )
{
    out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
    out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
    out << indent << "<SHADOW distance=\"" << shadowDistance << "\" direction=\""
        << static_cast<int>( shadowDirection ) << "\" red=\"" << shadowColor.red() << "\" green=\"" << shadowColor.green()
        << "\" blue=\"" << shadowColor.blue() << "\"/>" << endl;
    out << indent << "<EFFECTS effect=\"" << static_cast<int>( effect ) << "\" effect2=\""
        << static_cast<int>( effect2 ) << "\"/>" << endl;
    out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
    out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
    out << indent << "<FILLTYPE value=\"" << static_cast<int>( fillType ) << "\"/>" << endl;
    out << indent << "<GRADIENT red1=\"" << gColor1.red() << "\" green1=\"" << gColor1.green()
        << "\" blue1=\"" << gColor1.blue() << "\" red2=\"" << gColor2.red() << "\" green2=\""
        << gColor2.green() << "\" blue2=\"" << gColor2.blue() << "\" type=\""
        << static_cast<int>( gType ) << "\" unbalanced=\"" << (uint)unbalanced << "\" xfactor=\"" << xfactor
        << "\" yfactor=\"" << yfactor << "\"/>" << endl;
    out << indent << "<PEN red=\"" << pen.color().red() << "\" green=\"" << pen.color().green()
        << "\" blue=\"" << pen.color().blue() << "\" width=\"" << pen.width()
        << "\" style=\"" << static_cast<int>( pen.style() ) << "\"/>" << endl;
    out << indent << "<BRUSH red=\"" << brush.color().red() << "\" green=\"" << brush.color().green()
        << "\" blue=\"" << brush.color().blue() << "\" style=\"" << static_cast<int>( brush.style() ) << "\"/>" << endl;
    out << indent << "<DISAPPEAR effect=\"" << static_cast<int>( effect3 ) << "\" doit=\"" << static_cast<int>( disappear )
        << "\" num=\"" << disappearNum << "\"/>" << endl;
    saveKTextObject( out );
}

/*========================== load ================================*/
void KPTextObject::load( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        // orig
        if ( name == "ORIG" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "x" )
                    orig.setX( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "y" )
                    orig.setY( ( *it ).m_strValue.toInt() );
            }
        }

        // disappear
        else if ( name == "DISAPPEAR" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "effect" )
                    effect3 = ( Effect3 )( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "doit" )
                    disappear = ( bool )( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "num" )
                    disappearNum = ( *it ).m_strValue.toInt();
            }
        }

        // size
        else if ( name == "SIZE" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "width" )
                    ext.setWidth( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "height" )
                    ext.setHeight( ( *it ).m_strValue.toInt() );
            }
        }

        // shadow
        else if ( name == "SHADOW" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "distance" )
                    shadowDistance = ( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "direction" )
                    shadowDirection = ( ShadowDirection )( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "red" )
                    shadowColor.setRgb( ( *it ).m_strValue.toInt(),
                                        shadowColor.green(), shadowColor.blue() );
                if ( ( *it ).m_strName == "green" )
                    shadowColor.setRgb( shadowColor.red(), ( *it ).m_strValue.toInt(),
                                        shadowColor.blue() );
                if ( ( *it ).m_strName == "blue" )
                    shadowColor.setRgb( shadowColor.red(), shadowColor.green(),
                                        ( *it ).m_strValue.toInt() );
            }
        }

        // effects
        else if ( name == "EFFECTS" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "effect" )
                    effect = ( Effect )( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "effect2" )
                    effect2 = ( Effect2 )( *it ).m_strValue.toInt();
            }
        }

        // angle
        else if ( name == "ANGLE" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    angle = ( *it ).m_strValue.toDouble();
            }
        }

        // presNum
        else if ( name == "PRESNUM" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    presNum = ( *it ).m_strValue.toInt();
            }
        }

        // KTextObject
        else if ( name == "TEXTOBJ" ) {
            QDomElement e = parser.currentElement();
            ktextobject.document()->setLineSpacing( e.attribute( "lineSpacing" ).toInt() );
            ktextobject.document()->setParagSpacing( e.attribute( "paragSpacing" ).toInt() );
            ktextobject.document()->setMargin( e.attribute( "margin" ).toInt() );
            KTextEditDocument::TextSettings settings = ktextobject.document()->textSettings();
            settings.bulletColor[0] = QColor( e.attribute( "bulletColor1", Qt::black.name() ) );
            settings.bulletColor[1] = QColor( e.attribute( "bulletColor2", Qt::black.name() ) );
            settings.bulletColor[2] = QColor( e.attribute( "bulletColor3", Qt::black.name() ) );
            settings.bulletColor[3] = QColor( e.attribute( "bulletColor4", Qt::black.name() ) );
            settings.bulletType[0] = (KTextEditDocument::Bullet)e.attribute( "bulletType1", 0 ).toInt();
            settings.bulletType[1] = (KTextEditDocument::Bullet)e.attribute( "bulletType2", 0 ).toInt();
            settings.bulletType[2] = (KTextEditDocument::Bullet)e.attribute( "bulletType3", 0 ).toInt();
            settings.bulletType[3] = (KTextEditDocument::Bullet)e.attribute( "bulletType4", 0 ).toInt();
            ktextobject.document()->setTextSettings( settings );
            QString type = e.attribute( "objType" );
            int t = -1;
            if ( !type.isEmpty() ) {
                if ( type == "1" )
                    t = KTextEdit::EnumList;
                if ( type == "2" )
                    t = KTextEdit::BulletList;
            }

            loadKTextObject( e, t );
        }

        // pen
        else if ( name == "PEN" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "red" )
                    pen.setColor( QColor( ( *it ).m_strValue.toInt(), pen.color().green(), pen.color().blue() ) );
                if ( ( *it ).m_strName == "green" )
                    pen.setColor( QColor( pen.color().red(), ( *it ).m_strValue.toInt(), pen.color().blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    pen.setColor( QColor( pen.color().red(), pen.color().green(), ( *it ).m_strValue.toInt() ) );
                if ( ( *it ).m_strName == "width" )
                    pen.setWidth( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "style" )
                    pen.setStyle( ( Qt::PenStyle )( *it ).m_strValue.toInt() );
            }
            setPen( pen );
        }

        // brush
        else if ( name == "BRUSH" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "red" )
                    brush.setColor( QColor( ( *it ).m_strValue.toInt(), brush.color().green(), brush.color().blue() ) );
                if ( ( *it ).m_strName == "green" )
                    brush.setColor( QColor( brush.color().red(), ( *it ).m_strValue.toInt(), brush.color().blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    brush.setColor( QColor( brush.color().red(), brush.color().green(), ( *it ).m_strValue.toInt() ) );
                if ( ( *it ).m_strName == "style" )
                    brush.setStyle( ( Qt::BrushStyle )( *it ).m_strValue.toInt() );
            }
            setBrush( brush );
        }

        // fillType
        else if ( name == "FILLTYPE" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    fillType = static_cast<FillType>( ( *it ).m_strValue.toInt() );
            }
            setFillType( fillType );
        }

        // gradient
        else if ( name == "GRADIENT" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "red1" )
                    gColor1 = QColor( ( *it ).m_strValue.toInt(), gColor1.green(), gColor1.blue() );
                if ( ( *it ).m_strName == "green1" )
                    gColor1 = QColor( gColor1.red(), ( *it ).m_strValue.toInt(), gColor1.blue() );
                if ( ( *it ).m_strName == "blue1" )
                    gColor1 = QColor( gColor1.red(), gColor1.green(), ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "red2" )
                    gColor2 = QColor( ( *it ).m_strValue.toInt(), gColor2.green(), gColor2.blue() );
                if ( ( *it ).m_strName == "green2" )
                    gColor2 = QColor( gColor2.red(), ( *it ).m_strValue.toInt(), gColor2.blue() );
                if ( ( *it ).m_strName == "blue2" )
                    gColor2 = QColor( gColor2.red(), gColor2.green(), ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "type" )
                    gType = static_cast<BCType>( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "unbalanced" )
                    unbalanced = static_cast<bool>( ( *it ).m_strValue.toInt() );
                if ( ( *it ).m_strName == "xfactor" )
                    xfactor = ( *it ).m_strValue.toInt();
                if ( ( *it ).m_strName == "yfactor" )
                    yfactor = ( *it ).m_strValue.toInt();
            }
            setGColor1( gColor1 );
            setGColor2( gColor2 );
            setGType( gType );
            setGUnbalanced( unbalanced );
            setGXFactor( xfactor );
            setGYFactor( yfactor );
        }

        else
            kdError() << "Unknown tag '" << tag << "' in TEXT_OBJECT" << endl;

        if ( !parser.close( tag ) ) {
            kdError() << "ERR: Closing Child" << endl;
            return;
        }
    }
    setSize( ext.width(), ext.height() );
}

/*========================= draw =================================*/
void KPTextObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move ) {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    _painter->save();
//    _painter->setClipRect( getBoundingRect( _diffx, _diffy ) );
    setupClipRegion( _painter, getBoundingRect( _diffx, _diffy ) );

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    _painter->setPen( pen );
    _painter->setBrush( brush );

    int penw = pen.width() / 2;

    if ( shadowDistance > 0 ) {
        _painter->save();
        ktextobject.document()->enableDrawAllInOneColor( shadowColor );

        if ( angle == 0 ) {
            int sx = ox;
            int sy = oy;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            _painter->translate( sx, sy );

            if ( specEffects ) {
                switch ( effect2 ) {
                case EF2T_PARA:
                    kdDebug() << "KPTextObject::draw onlyCurrStep=" << onlyCurrStep << " subPresStep=" << subPresStep << endl;
                    drawParags( _painter, ( onlyCurrStep ? subPresStep : 0 ), subPresStep );
                    break;
                default:
                    ktextobject.document()->draw( _painter, ktextobject.colorGroup() );
                }
            } else {
                ktextobject.document()->draw( _painter, ktextobject.colorGroup() );
            }

            ktextobject.document()->disableDrawAllInOneColor();
        } else {
            _painter->translate( ox, oy );

            QRect br = QRect( 0, 0, ow, oh );
            int pw = br.width();
            int ph = br.height();
            QRect rr = br;
            int yPos = -rr.y();
            int xPos = -rr.x();
            br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
            rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

            QWMatrix m, mtx;
            mtx.rotate( angle );
            m.translate( pw / 2, ph / 2 );
            m = mtx * m;

            _painter->setWorldMatrix( m, true );

            int sx = 0;
            int sy = 0;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            _painter->translate( rr.left() + xPos + sx, rr.top() + yPos + sy );
            if ( specEffects ) {
                switch ( effect2 ) {
                case EF2T_PARA:
                    kdDebug() << "KPTextObject::draw onlyCurrStep=" << onlyCurrStep << " subPresStep=" << subPresStep << endl;
                    drawParags( _painter, ( onlyCurrStep ? subPresStep : 0 ), subPresStep );
                    break;
                default:
                    ktextobject.document()->draw( _painter, ktextobject.colorGroup() );
                }
            } else {
                ktextobject.document()->draw( _painter, ktextobject.colorGroup() );
            }
            ktextobject.document()->disableDrawAllInOneColor();
        }
        _painter->restore();
    }

    _painter->translate( ox, oy );

    if ( angle == 0 ) {
        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );
        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect( penw, penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( penw, penw, *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( penw, penw, ow - 2 * penw, oh - 2 * penw );

        if ( specEffects ) {
            switch ( effect2 ) {
            case EF2T_PARA:
                kdDebug() << "KPTextObject::draw onlyCurrStep=" << onlyCurrStep << " subPresStep=" << subPresStep << endl;
                drawParags( _painter, ( onlyCurrStep ? subPresStep : 0 ), subPresStep );
                break;
            default:
                ktextobject.document()->draw( _painter, ktextobject.colorGroup() );
            }
        } else {
            ktextobject.document()->draw( _painter, ktextobject.colorGroup() );
        }
    } else {
        QRect br = QRect( 0, 0, ow, oh );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int yPos = -rr.y();
        int xPos = -rr.x();
        br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m, mtx;
        mtx.rotate( angle );
        m.translate( pw / 2, ph / 2 );
        m = mtx * m;

        _painter->setWorldMatrix( m, true );

        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );

        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect( rr.left() + xPos + penw, rr.top() + yPos + penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( rr.left() + xPos + penw, rr.top() + yPos + penw, *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( rr.left() + xPos + penw, rr.top() + yPos + penw, ow - 2 * penw, oh - 2 * penw );

        _painter->translate( rr.left() + xPos, rr.top() + yPos );

        if ( specEffects ) {
            switch ( effect2 ) {
            case EF2T_PARA:
                kdDebug() << "KPTextObject::draw onlyCurrStep=" << onlyCurrStep << " subPresStep=" << subPresStep << endl;
                drawParags( _painter, ( onlyCurrStep ? subPresStep : 0 ), subPresStep );
                break;
            default:
                ktextobject.document()->draw( _painter, ktextobject.colorGroup() );
            }
        } else {
            ktextobject.document()->draw( _painter, ktextobject.colorGroup() );
        }
    }

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*========================== activate ============================*/
void KPTextObject::activate( QWidget *_widget, int diffx, int diffy )
{
    ktextobject.reparent( _widget, 0, QPoint( orig.x() - diffx, orig.y() - diffy ), false );
    ktextobject.resize( ext.width(), ext.height() );
    ktextobject.show();
    ktextobject.setCursor( Qt::ibeamCursor );
}

/*========================== deactivate ==========================*/
void KPTextObject::deactivate( KPresenterDoc *doc )
{
    recalcPageNum( doc );
    ktextobject.reparent( 0, 0, QPoint( 0, 0 ), false );
    ktextobject.hide();
}

/*========================= zoom =================================*/
void KPTextObject::zoom( float _fakt )
{
    KPObject::zoom( _fakt );
    ktextobject.zoom( _fakt );
}

/*==================== zoom orig =================================*/
void KPTextObject::zoomOrig()
{
    KPObject::zoomOrig();
    ktextobject.unzoom();
}

/*================================================================*/
void KPTextObject::extendObject2Contents( KPresenterView */*view*/ )
{
    QSize s( ktextobject.neededSize() );
    setSize( s.width(), s.height() );
}

/*=========================== save ktextobject ===================*/
void KPTextObject::saveKTextObject( QTextStream& out )
{
    KTextEditParag *parag = ktextobject.document()->firstParag();
    KTextEditDocument::TextSettings textSettings = ktextobject.document()->textSettings();
    out << otag
	<< "<TEXTOBJ lineSpacing=\"" << ktextobject.document()->lineSpacing()
	<< "\" paragSpacing=\"" << ktextobject.document()->paragSpacing()
	<< "\" margin=\"" << ktextobject.document()->margin()
	<< "\" bulletType1=\"" << (int)textSettings.bulletType[0]
	<< "\" bulletType2=\"" << (int)textSettings.bulletType[1]
	<< "\" bulletType3=\"" << (int)textSettings.bulletType[2]
	<< "\" bulletType4=\"" << (int)textSettings.bulletType[3]
	<< "\" bulletColor1=\"" << textSettings.bulletColor[0].name()
	<< "\" bulletColor2=\"" << textSettings.bulletColor[1].name()
	<< "\" bulletColor3=\"" << textSettings.bulletColor[2].name()
	<< "\" bulletColor4=\"" << textSettings.bulletColor[3].name()
	<< "\">" << endl;
    while ( parag ) {
	out << otag << "<P align=\"" << parag->alignment()
	    << "\" type=\"" << (int)parag->type()
	    << "\" depth=\"" << parag->listDepth() << "\">" << endl;
	out << indent;
	KTextEditFormat *lastFormat = 0;
	for ( int i = 0; i < parag->length(); ++i ) {
	    KTextEditString::Char *c = parag->at( i );
	    if ( !lastFormat || c->format->key() != lastFormat->key() ) {
		if ( lastFormat )
		    out << "</TEXT>";
		lastFormat = c->format;
		out << "<TEXT family=\"" <<  lastFormat->font().family()
		    << "\" pointSize=\"" << lastFormat->font().pointSize()
		    << "\" bold=\"" << (uint)lastFormat->font().bold()
		    << "\" italic=\"" << (uint)lastFormat->font().italic()
		    << "\" underline=\"" << (uint)lastFormat->font().underline()
		    << "\" color=\"" << lastFormat->color().name()
		    << "\">";
	    }
	    QChar chr = c->c;
	    if ( chr == '&' )
		out << "&amp;";
	    else if ( chr == '<' )
		out << "&lt;";
	    else if ( chr == '>' )
		out << "&gt;";
	    else
		out << QString(c->c);
	}
	if ( lastFormat )
	    out << "</TEXT>";
	out << endl;

	out << etag << "</P>" << endl;

	parag = parag->next();
    }
    out << etag << "</TEXTOBJ>" << endl;
}

/*====================== load ktextobject ========================*/
void KPTextObject::loadKTextObject( const QDomElement &elem, int type )
{
    QDomElement e = elem.firstChild().toElement();
    KTextEditParag *lastParag = ktextobject.document()->firstParag();
    lastParag->remove( 0, 1 );
    int i = 0;
    KTextEditFormat *fm = 0;
    int listNum = 0;
    KTextEditDocument::TextSettings settings = ktextobject.document()->textSettings();
    int lineSpacing = 0, paragSpacing = 0;
    while ( !e.isNull() ) {
        if ( e.tagName() == "P" ) {
            QDomElement n = e.firstChild().toElement();
            if ( type != -1 )
                lastParag->setType( (KTextEditParag::Type)type );
            else
                lastParag->setType( (KTextEditParag::Type)e.attribute( "type" ).toInt() );
            lastParag->setAlignment( e.attribute( "align" ).toInt() );
            lastParag->setListDepth( e.attribute( "depth" ).toInt() );
            lineSpacing = QMAX( e.attribute( "lineSpacing" ).toInt(), lineSpacing );
            paragSpacing = QMAX( QMAX( e.attribute( "distBefore" ).toInt(), e.attribute( "distAfter" ).toInt() ), paragSpacing );
            while ( !n.isNull() ) {
                if ( n.tagName() == "TEXT" ) {
                    QString family = n.attribute( "family" );
                    int size = n.attribute( "pointSize" ).toInt();
                    bool bold = (bool)n.attribute( "bold" ).toInt();
                    bool italic = (bool)n.attribute( "italic" ).toInt();
                    bool underline = (bool)n.attribute( "underline" ).toInt();
                    QString color = n.attribute( "color" );
                    QFont fn( family );
                    fn.setPointSize( size );
                    fn.setBold( bold );
                    fn.setItalic( italic );
                    fn.setUnderline( underline );
                    QColor col( color );
                    fm = ktextobject.document()->formatCollection()->format( fn, col );
                    QString txt = n.firstChild().toText().data();
                    if ( txt.isEmpty() && lastParag->length() == 0 )
                        txt = ' ';
                    if ( !txt.isEmpty() ) {
                        if ( !txt[txt.length()-1].isSpace() ) {
                            kdWarning() << "Found a TEXT element which doesn't end with a trailing space, adding one." << endl;
                            txt+=' ';
                        }
                        lastParag->append( txt );
                        lastParag->setFormat( i, txt.length(), fm, TRUE, KTextEditFormat::Format );
                        i += txt.length();
                    }
                }
                n = n.nextSibling().toElement();
            }
        } else if ( e.tagName() == "UNSORTEDLISTTYPE" ) {
            if ( listNum < 4 ) {
                QColor c( e.attribute( "red" ).toInt(), e.attribute( "green" ).toInt(), e.attribute( "blue" ).toInt() );
                settings.bulletColor[ listNum++ ] = c;
            }
        }
        e = e.nextSibling().toElement();
        if ( e.isNull() )
            break;
        i = 0;
        if ( !lastParag->length() == 0 )
            lastParag = new KTextEditParag( ktextobject.document(), lastParag, 0 );
    }

    settings.lineSpacing = lineSpacing;
    settings.paragSpacing = QMAX( ktextobject.document()->paragSpacing(), paragSpacing );
    ktextobject.document()->setTextSettings( settings );
    ktextobject.updateCurrentFormat();
}

/*================================================================*/
void KPTextObject::recalcPageNum( KPresenterDoc *doc )
{
    int h = doc->getPageRect( 0, 0, 0 ).height();
    int pgnum = -1;
    for ( unsigned int i = 0; i < doc->getPageNums(); ++i ) {
        if ( (int)orig.y() <= ( (int)i + 1 ) * h ) {
            pgnum = i + 1;
            break;
        }
    }

    if ( pgnum == -1 )
        pgnum = doc->getPageNums();

#if 0
    ktextobject.setPageNum( pgnum );
#endif
}

void KPTextObject::drawParags( QPainter *p, int from, int to )
{
    int i = 0;
    KTextEditParag *parag = ktextobject.document()->firstParag();
    while ( parag ) {
        if ( !parag->isValid() )
            parag->format();

        p->translate( 0, parag->rect().y() );
        if ( i >= from && i <= to )
            parag->paint( *p, ktextobject.colorGroup(), 0, FALSE );
        p->translate( 0, -parag->rect().y() );
        parag = parag->next();

        ++i;
        if ( i > to )
            return;
    }
}
