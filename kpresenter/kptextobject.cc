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

#include <kptextobject.h>
#include <kpgradient.h>
#include <resizecmd.h>

#include <qwidget.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include <qapplication.h>

#include <klocale.h>
#include <kdebug.h>

#include <kpresenter_view.h>
#include <kpresenter_doc.h>
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
QDomDocumentFragment KPTextObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KPObject::save(doc);
    fragment.appendChild(KPObject::createValueElement("FILLTYPE", static_cast<int>(fillType), doc));
    fragment.appendChild(KPObject::createGradientElement("GRADIENT", gColor1, gColor2, static_cast<int>(gType),
                                                         unbalanced, xfactor, yfactor, doc));
    fragment.appendChild(KPObject::createPenElement("PEN", pen, doc));
    fragment.appendChild(KPObject::createBrushElement("BRUSH", brush, doc));
    fragment.appendChild(saveKTextObject( doc ));
    return fragment;
}

/*========================== load ================================*/
void KPTextObject::load(const QDomElement &element)
{
    KPObject::load(element);

    QDomElement e=element.namedItem("PEN").toElement();
    if(!e.isNull())
        setPen(KPObject::toPen(e));
    e=element.namedItem("BRUSH").toElement();
    if(!e.isNull())
        setBrush(KPObject::toBrush(e));
    e=element.namedItem("FILLTYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        setFillType(static_cast<FillType>(tmp));
    }
    e=element.namedItem("GRADIENT").toElement();
    if(!e.isNull()) {
        KPObject::toGradient(e, gColor1, gColor2, gType, unbalanced, xfactor, yfactor);
        if(gradient)
            gradient->init(gColor1, gColor2, gType, unbalanced, xfactor, yfactor);
    }
    e=element.namedItem("TEXTOBJ").toElement();
    if(!e.isNull()) {
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
QDomElement KPTextObject::saveKTextObject( QDomDocument& doc )
{
    KTextEditParag *parag = ktextobject.document()->firstParag();
    KTextEditDocument::TextSettings textSettings = ktextobject.document()->textSettings();
    QDomElement textobj=doc.createElement("TEXTOBJ");
    textobj.setAttribute("lineSpacing", ktextobject.document()->lineSpacing());
    textobj.setAttribute("paragSpacing", ktextobject.document()->paragSpacing());
    textobj.setAttribute("margin", ktextobject.document()->margin());
    textobj.setAttribute("bulletType1", (int)textSettings.bulletType[0]);
    textobj.setAttribute("bulletType2", (int)textSettings.bulletType[1]);
    textobj.setAttribute("bulletType3", (int)textSettings.bulletType[2]);
    textobj.setAttribute("bulletType4", (int)textSettings.bulletType[3]);
    textobj.setAttribute("bulletColor1", textSettings.bulletColor[0].name());
    textobj.setAttribute("bulletColor2", textSettings.bulletColor[1].name());
    textobj.setAttribute("bulletColor3", textSettings.bulletColor[2].name());
    textobj.setAttribute("bulletColor4", textSettings.bulletColor[3].name());
    // ### fix this loop (Werner)
    while ( parag ) {
        QDomElement paragraph=doc.createElement("P");
        paragraph.setAttribute("align", parag->alignment());
        paragraph.setAttribute("type", (int)parag->type());
        paragraph.setAttribute("depth", parag->listDepth());
        KTextEditFormat *lastFormat = 0;
        QString tmpText, tmpFamily, tmpColor;
        int tmpPointSize=10;
        unsigned int tmpBold=false, tmpItalic=false, tmpUnderline=false;
        for ( int i = 0; i < parag->length(); ++i ) {
            KTextEditString::Char *c = parag->at( i );
            if ( !lastFormat || c->format->key() != lastFormat->key() ) {
                if ( lastFormat )
                    paragraph.appendChild(saveHelper(tmpText, tmpFamily, tmpColor, tmpPointSize,
                                                     tmpBold, tmpItalic, tmpUnderline, doc));
                lastFormat = c->format;
                tmpText="";
                tmpFamily=lastFormat->font().family();
                tmpPointSize=lastFormat->font().pointSize();
                tmpBold=static_cast<unsigned int>(lastFormat->font().bold());
                tmpItalic=static_cast<unsigned int>(lastFormat->font().italic());
                tmpUnderline=static_cast<unsigned int>(lastFormat->font().underline());
                tmpColor=lastFormat->color().name();
            }
            tmpText+=c->c;
        }
        if ( lastFormat ) {
            paragraph.appendChild(saveHelper(tmpText, tmpFamily, tmpColor, tmpPointSize,
                                             tmpBold, tmpItalic, tmpUnderline, doc));
        }
        textobj.appendChild(paragraph);
        parag = parag->next();
    }
    return textobj;
}

QDomElement KPTextObject::saveHelper(const QString &tmpText, const QString &tmpFamily, const QString &tmpColor,
                                     int tmpPointSize, unsigned int tmpBold, unsigned int tmpItalic,
                                     unsigned int tmpUnderline, QDomDocument &doc) {
    QDomElement element=doc.createElement("TEXT");
    element.setAttribute("family", tmpFamily);
    element.setAttribute("pointSize", tmpPointSize);
    element.setAttribute("bold", tmpBold);
    element.setAttribute("italic", tmpItalic);
    element.setAttribute("underline", tmpUnderline);
    element.setAttribute("color", tmpColor);
    if(tmpText.stripWhiteSpace().isEmpty())
        // working around a bug in QDom
        element.setAttribute("whitespace", tmpText.length());
    element.appendChild(doc.createTextNode(tmpText));
    return element;
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
                    if(n.hasAttribute("whitespace")) {
                        int ws=n.attribute("whitespace").toInt();
                        txt.fill(' ', ws);
                    }
                    n=n.nextSibling().toElement();
                    if ( txt.isEmpty() )
                        txt = ' ';
                    if ( !txt.isEmpty() ) {
                        if ( ( !txt[txt.length()-1].isSpace()  && n.isNull() ) )
                            txt+=' ';
                        lastParag->append( txt );
                        lastParag->setFormat( i, txt.length(), fm, TRUE, KTextEditFormat::Format );
                        i += txt.length();
                    }
                }
                else
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
