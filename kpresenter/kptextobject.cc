/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: ktextobject object                                     */
/******************************************************************/

#include "kptextobject.h"
#include "kpgradient.h"
#include "resizecmd.h"

#include <qwidget.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qwmatrix.h>

#include <klocale.h>

#include "kpresenter_view.h"

/******************************************************************/
/* Class: KPTextObject                                            */
/******************************************************************/

/*================ default constructor ===========================*/
KPTextObject::KPTextObject()
    : KPObject(), ktextobject( 0, "" )
{
    ktextobject.hide();
    ktextobject.setAutoFocus( true );
    brush = Qt::NoBrush;
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    gColor1 = Qt::red;
    gColor2 = Qt::green;
    drawEditRect = true;
    drawEmpty = true;
}

/*======================= set size ===============================*/
void KPTextObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move ) return;
    ktextobject.resize( ext );

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*======================= set size ===============================*/
void KPTextObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move ) return;
    ktextobject.resize( ext );

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
        gradient = new KPGradient( gColor1, gColor2, gType, getSize() );
}

/*========================= save =================================*/
void KPTextObject::save( ostream& out )
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
        << static_cast<int>( gType ) << "\"/>" << endl;
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
void KPTextObject::load( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string tag;
    string name;

    while ( parser.open( 0L, tag ) )
    {
        KOMLParser::parseTag( tag.c_str(), name, lst );

        // orig
        if ( name == "ORIG" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "x" )
                    orig.setX( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "y" )
                    orig.setY( atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        // disappear
        else if ( name == "DISAPPEAR" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "effect" )
                    effect3 = ( Effect3 )atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "doit" )
                    disappear = ( bool )atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "num" )
                    disappearNum = atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // size
        else if ( name == "SIZE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "width" )
                    ext.setWidth( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "height" )
                    ext.setHeight( atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        // shadow
        else if ( name == "SHADOW" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "distance" )
                    shadowDistance = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "direction" )
                    shadowDirection = ( ShadowDirection )atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "red" )
                    shadowColor.setRgb( atoi( ( *it ).m_strValue.c_str() ),
                                        shadowColor.green(), shadowColor.blue() );
                if ( ( *it ).m_strName == "green" )
                    shadowColor.setRgb( shadowColor.red(), atoi( ( *it ).m_strValue.c_str() ),
                                        shadowColor.blue() );
                if ( ( *it ).m_strName == "blue" )
                    shadowColor.setRgb( shadowColor.red(), shadowColor.green(),
                                        atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        // effects
        else if ( name == "EFFECTS" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "effect" )
                    effect = ( Effect )atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "effect2" )
                    effect2 = ( Effect2 )atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // angle
        else if ( name == "ANGLE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    angle = atof( ( *it ).m_strValue.c_str() );
            }
        }

        // presNum
        else if ( name == "PRESNUM" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    presNum = atoi( ( *it ).m_strValue.c_str() );
            }
        }

        // KTextObject
        else if ( name == "TEXTOBJ" )
        {
            ktextobject.clear( false );
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "objType" )
                    ktextobject.setObjType( ( KTextObject::ObjType )atoi( ( *it ).m_strValue.c_str() ) );
                else
                    cerr << "Unknown attrib TEXTOBJ:'" << ( *it ).m_strName << "'" << endl;
            }
            loadKTextObject( parser, lst );
        }

        // pen
        else if ( name == "PEN" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "red" )
                    pen.setColor( QColor( atoi( ( *it ).m_strValue.c_str() ), pen.color().green(), pen.color().blue() ) );
                if ( ( *it ).m_strName == "green" )
                    pen.setColor( QColor( pen.color().red(), atoi( ( *it ).m_strValue.c_str() ), pen.color().blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    pen.setColor( QColor( pen.color().red(), pen.color().green(), atoi( ( *it ).m_strValue.c_str() ) ) );
                if ( ( *it ).m_strName == "width" )
                    pen.setWidth( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "style" )
                    pen.setStyle( ( Qt::PenStyle )atoi( ( *it ).m_strValue.c_str() ) );
            }
            setPen( pen );
        }

        // brush
        else if ( name == "BRUSH" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "red" )
                    brush.setColor( QColor( atoi( ( *it ).m_strValue.c_str() ), brush.color().green(), brush.color().blue() ) );
                if ( ( *it ).m_strName == "green" )
                    brush.setColor( QColor( brush.color().red(), atoi( ( *it ).m_strValue.c_str() ), brush.color().blue() ) );
                if ( ( *it ).m_strName == "blue" )
                    brush.setColor( QColor( brush.color().red(), brush.color().green(), atoi( ( *it ).m_strValue.c_str() ) ) );
                if ( ( *it ).m_strName == "style" )
                    brush.setStyle( ( Qt::BrushStyle )atoi( ( *it ).m_strValue.c_str() ) );
            }
            setBrush( brush );
        }

        // fillType
        else if ( name == "FILLTYPE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    fillType = static_cast<FillType>( atoi( ( *it ).m_strValue.c_str() ) );
            }
            setFillType( fillType );
        }

        // gradient
        else if ( name == "GRADIENT" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "red1" )
                    gColor1 = QColor( atoi( ( *it ).m_strValue.c_str() ), gColor1.green(), gColor1.blue() );
                if ( ( *it ).m_strName == "green1" )
                    gColor1 = QColor( gColor1.red(), atoi( ( *it ).m_strValue.c_str() ), gColor1.blue() );
                if ( ( *it ).m_strName == "blue1" )
                    gColor1 = QColor( gColor1.red(), gColor1.green(), atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "red2" )
                    gColor2 = QColor( atoi( ( *it ).m_strValue.c_str() ), gColor2.green(), gColor2.blue() );
                if ( ( *it ).m_strName == "green2" )
                    gColor2 = QColor( gColor2.red(), atoi( ( *it ).m_strValue.c_str() ), gColor2.blue() );
                if ( ( *it ).m_strName == "blue2" )
                    gColor2 = QColor( gColor2.red(), gColor2.green(), atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "type" )
                    gType = static_cast<BCType>( atoi( ( *it ).m_strValue.c_str() ) );
            }
            setGColor1( gColor1 );
            setGColor2( gColor2 );
            setGType( gType );
        }

        else
            cerr << "Unknown tag '" << tag << "' in TEXT_OBJECT" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            return;
        }
    }
    setSize( ext.width(), ext.height() );

    if ( ktextobject.items() > 0 )
    {
        ktextobject.setFont( ktextobject.itemAt( 0 )->font() );
        ktextobject.setColor( ktextobject.itemAt( 0 )->color() );
    }
}

/*========================= draw =================================*/
void KPTextObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();
    KRect r;

    KRect cr = getBoundingRect( _diffx, _diffy );
    int _x = cr.x(), _y = cr.y(), _w = cr.width(), _h = cr.height();

    _painter->save();
    r = _painter->viewport();

    _painter->setPen( pen );
    _painter->setBrush( brush );

    int penw = pen.width();

    if ( shadowDistance > 0 )
    {
        _painter->save();
        ktextobject.enableDrawAllInOneColor( shadowColor );

        if ( angle == 0 )
        {
            int sx = ox;
            int sy = oy;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            _painter->setViewport( sx, sy, r.width(), r.height() );

            if ( specEffects )
            {
                switch ( effect2 )
                {
                case EF2T_PARA:
                    ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, ( onlyCurrStep ? subPresStep : 0 ), subPresStep, ownClipping, drawEmpty );
                    break;
                default:
                    ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, -1, -1, ownClipping, drawEmpty );
                }
            }
            else
                ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, -1, -1, ownClipping, drawEmpty );

            ktextobject.disableDrawAllInOneColor();
        }
        else
        {
            _painter->setViewport( ox, oy, r.width(), r.height() );

            KRect br = KRect( 0, 0, ow, oh );
            int pw = br.width();
            int ph = br.height();
            KRect rr = br;
            int yPos = -rr.y();
            int xPos = -rr.x();
            br.moveTopLeft( KPoint( -br.width() / 2, -br.height() / 2 ) );
            rr.moveTopLeft( KPoint( -rr.width() / 2, -rr.height() / 2 ) );

            QWMatrix m, mtx;
            mtx.rotate( angle );
            m.translate( pw / 2, ph / 2 );
            m = mtx * m;

            _painter->setWorldMatrix( m );

            int sx = 0;
            int sy = 0;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            _painter->translate( rr.left() + xPos + sx, rr.top() + yPos + sy );

            if ( specEffects )
            {
                switch ( effect2 )
                {
                case EF2T_PARA:
                    ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, ( onlyCurrStep ? subPresStep : 0 ), subPresStep, ownClipping, drawEmpty );
                    break;
                default:
                    ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, -1, -1, ownClipping, drawEmpty );
                }
            }
            else
                ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, -1, -1, ownClipping, drawEmpty );

            ktextobject.disableDrawAllInOneColor();
        }
        _painter->restore();
    }

    _painter->setViewport( ox, oy, r.width(), r.height() );

    if ( angle == 0 )
    {
        _painter->setPen( Qt::NoPen );
        _painter->setBrush( brush );
        if ( fillType == FT_BRUSH || !gradient )
            _painter->drawRect( penw, penw, ext.width() - 2 * penw, ext.height() - 2 * penw );
        else
            _painter->drawPixmap( penw, penw, *gradient->getGradient(), 0, 0, ow - 2 * penw, oh - 2 * penw );

        _painter->setPen( pen );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( penw, penw, ow - 2 * penw, oh - 2 * penw );

        if ( specEffects )
        {
            switch ( effect2 )
            {
            case EF2T_PARA:
                ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, ( onlyCurrStep ? subPresStep : 0 ), subPresStep, ownClipping, drawEmpty );
                break;
            default:
                ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, -1, -1, ownClipping, drawEmpty );
            }
        }
        else
            ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, -1, -1, ownClipping, drawEmpty );
    }
    else
    {
        KRect br = KRect( 0, 0, ow, oh );
        int pw = br.width();
        int ph = br.height();
        KRect rr = br;
        int yPos = -rr.y();
        int xPos = -rr.x();
        br.moveTopLeft( KPoint( -br.width() / 2, -br.height() / 2 ) );
        rr.moveTopLeft( KPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m, mtx;
        mtx.rotate( angle );
        m.translate( pw / 2, ph / 2 );
        m = mtx * m;

        _painter->setWorldMatrix( m );

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

        if ( specEffects )
        {
            switch ( effect2 )
            {
            case EF2T_PARA:
                ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, ( onlyCurrStep ? subPresStep : 0 ), subPresStep, ownClipping, drawEmpty );
                break;
            default:
                ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, -1, -1, ownClipping, drawEmpty );
            }
        }
        else
            ktextobject.draw( *_painter, _x, _y, _w, _h, zoomed, -1, -1, ownClipping, drawEmpty );
    }

    _painter->setViewport( r );
    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*========================== activate ============================*/
void KPTextObject::activate( QWidget *_widget, int diffx, int diffy )
{
    ktextobject.recreate( _widget, 0, KPoint( orig.x() - diffx, orig.y() - diffy ), false );
    ktextobject.resize( ext.width(), ext.height() );
    ktextobject.show();
    ktextobject.setCursor( Qt::ibeamCursor );
    ktextobject.setAutoFocus( true );
}

/*========================== deactivate ==========================*/
void KPTextObject::deactivate()
{
    ktextobject.recreate( 0, 0, KPoint( 0, 0 ), false );
    ktextobject.hide();
}

/*========================= zoom =================================*/
void KPTextObject::zoom( float _fakt )
{
    KPObject::zoom( _fakt );
    //ktextobject.resize( ext );
    ktextobject.zoom( _fakt );
}

/*==================== zoom orig =================================*/
void KPTextObject::zoomOrig()
{
    KPObject::zoomOrig();
    //ktextobject.resize( ext );
    ktextobject.zoomOrig();
}

/*================================================================*/
void KPTextObject::extendObject2Contents( KPresenterView *view )
{
    KSize s( ktextobject.neededSize() );

    setSize( s.width(), s.height() );
    
//     ResizeCmd *resizeCmd = new ResizeCmd( i18n( "Resize object" ), KPoint( 0, 0 ), KSize( s.width() - ext.width(), 
//                                                                                           s.height() - ext.height() ),
//                                           this, view->kPresenterDoc() );
//     resizeCmd->execute();
//     view->kPresenterDoc()->commands()->addCommand( resizeCmd );
}

/*=========================== save ktextobject ===================*/
void KPTextObject::saveKTextObject( ostream& out )
{
    TxtObj *txtObj;
    TxtLine *txtLine;
    TxtParagraph *txtParagraph;
    unsigned int i, j, k;
    QFont font;
    bool lastWasSpace = false;

    out << otag << "<TEXTOBJ objType=\"" << static_cast<int>( ktextobject.objType() ) << "\">" << endl;
    out << indent << "<ENUMLISTTYPE type=\"" << ktextobject.enumListType().type << "\" before=\""
        << ktextobject.enumListType().before.utf8().data() << "\" after=\""
	<< ktextobject.enumListType().after.utf8().data()
        << "\" start=\"" << ktextobject.enumListType().start << "\" family=\""
        << ktextobject.enumListType().font.family().ascii() << "\" pointSize=\"" << ktextobject.enumListType().font.pointSize()
        << "\" bold=\"" << ktextobject.enumListType().font.bold() << "\" italic=\"" << ktextobject.enumListType().font.italic()
        << "\" underline=\"" << ktextobject.enumListType().font.underline() << "\" red=\""
        << ktextobject.enumListType().color.red() << "\" green=\"" << ktextobject.enumListType().color.green()
        << "\" blue=\"" << ktextobject.enumListType().color.blue() << "\"/>" << endl;
    for ( i = 0; i < 16; i++ )
    {
        out << indent << "<UNSORTEDLISTTYPE family=\"" << ktextobject.unsortListType().font->at( i )->family().ascii() << "\" pointSize=\""
            << ktextobject.unsortListType().font->at( i )->pointSize()
            << "\" bold=\"" << ktextobject.unsortListType().font->at( i )->bold() << "\" italic=\""
            << ktextobject.unsortListType().font->at( i )->italic()
            << "\" underline=\"" << ktextobject.unsortListType().font->at( i )->underline() << "\" red=\""
            << ktextobject.unsortListType().color->at( i )->red() << "\" green=\"" << ktextobject.unsortListType().color->at( i )->green()
            << "\" blue=\"" << ktextobject.unsortListType().color->at( i )->blue() << "\" chr=\""
            << static_cast<unsigned short>( ktextobject.unsortListType().chr->at( i )->unicode() )
            << "\"/>" << endl;
    }

    for ( i = 0; i < ktextobject.paragraphs(); i++ )
    {
        txtParagraph = ktextobject.paragraphAt( i );

        out << otag << "<PARAGRAPH horzAlign=\"" << static_cast<int>( txtParagraph->horzAlign() ) << "\" depth=\""
            << txtParagraph->getDepth() << "\" lineSpacing=\"" << txtParagraph->getLineSpacing() << "\" distBefore=\""
            << txtParagraph->getDistBefore() << "\" distAfter=\"" << txtParagraph->getDistAfter() << "\">" << endl;

        lastWasSpace = false;

        for ( j = 0; j < txtParagraph->lines(); j++ )
        {
            txtLine = txtParagraph->lineAt( j );

            out << otag << "<LINE>" << endl;

            for ( k = 0; k < txtLine->items(); k++ )
            {
                txtObj = txtLine->itemAt( k );
                if ( lastWasSpace && txtObj->type() == TxtObj::SEPARATOR ) continue;
                if ( txtObj->type() == TxtObj::SEPARATOR ) lastWasSpace = true;
                else lastWasSpace = false;
                font = txtObj->font();

                out << otag << "<OBJ>" << endl;
                out << indent << "<TYPE value=\"" << static_cast<int>( txtObj->type() ) << "\"/>" << endl;
                out << indent << "<FONT family=\"" << font.family().ascii() << "\" pointSize=\""
                    << font.pointSize() << "\" bold=\"" << font.bold() << "\" italic=\"" << font.italic()
                    << "\" underline=\"" << font.underline() << "\"/>" << endl;
                out << indent << "<COLOR red=\"" << txtObj->color().red() << "\" green=\""
                    << txtObj->color().green() << "\" blue=\"" << txtObj->color().blue() << "\"/>" << endl;
                out << indent << "<VERTALIGN value=\"" << static_cast<int>( txtObj->vertAlign() ) << "\"/>" << endl;
                out << indent << "<TEXT>" << decode( txtObj->text() ).utf8().data() << "</TEXT>" << endl;
                out << etag << "</OBJ>" << endl;
            }

            out << etag << "</LINE>" << endl;

        }

        out << etag << "</PARAGRAPH>" << endl;

    }

    out << etag << "</TEXTOBJ>" << endl;
}

/*====================== load ktextobject ========================*/
void KPTextObject::loadKTextObject( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string tag;
    string name;
    bool utf8 = false;

    KTextObject::EnumListType elt;
    KTextObject::UnsortListType ult = ktextobject.unsortListType();

    ult.font->clear();
    ult.color->clear();
    ult.ofont->clear();
    ult.chr->clear();

    QFont font;
    QColor color;
    int r = 0, g = 0, b = 0;
    QChar c;
    TxtParagraph *txtParagraph;
    TxtObj *objPtr;

    while ( parser.open( 0L, tag ) )
    {
        KOMLParser::parseTag( tag.c_str(), name, lst );

        // enumListType
        if ( name == "ENUMLISTTYPE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "type" )
                    elt.type = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "before" )
                {
                    if ( strlen( ( *it ).m_strValue.c_str() ) > 0 )
                        elt.before = QString::fromUtf8( ( *it ).m_strValue.c_str() );
                    else
                        elt.before = "";
                }
                if ( ( *it ).m_strName == "after" )
                {
                    if ( strlen( ( *it ).m_strValue.c_str() ) > 0 )
                        elt.after = QString::fromUtf8( ( *it ).m_strValue.c_str() );
                    else
                        elt.after = "";
                }
                if ( ( *it ).m_strName == "start" )
                    elt.start = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "family" )
                    font.setFamily( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "pointSize" )
                    font.setPointSize( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "bold" )
                    font.setBold( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "italic" )
                    font.setItalic( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "underline" )
                    font.setUnderline( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "red" )
                    r = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "green" )
                    g = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "blue" )
                    b = atoi( ( *it ).m_strValue.c_str() );
            }
            color.setRgb( r, g, b );
            elt.font = font;
            elt.color = color;
            ktextobject.setEnumListType( elt );
        }

        // unsortListType
        else if ( name == "UNSORTEDLISTTYPE" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "chr" )
                    c = QChar( static_cast<unsigned short>( atoi( ( *it ).m_strValue.c_str() ) ) );
                if ( ( *it ).m_strName == "family" )
                    font.setFamily( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "pointSize" )
                    font.setPointSize( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "bold" )
                    font.setBold( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "italic" )
                    font.setItalic( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "underline" )
                    font.setUnderline( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "red" )
                    r = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "green" )
                    g = atoi( ( *it ).m_strValue.c_str() );
                if ( ( *it ).m_strName == "blue" )
                    b = atoi( ( *it ).m_strValue.c_str() );
            }
            color.setRgb( r, g, b );
            ult.chr->append( new QChar( c ) );
            ult.font->append( new QFont( font ) );
            ult.color->append( new QColor( color ) );
            ult.ofont->append( new QFont() );
        }

        // paragraph
        else if ( name == "PARAGRAPH" )
        {
            txtParagraph = ktextobject.addParagraph();
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "horzAlign" )
                    txtParagraph->setHorzAlign( ( TxtParagraph::HorzAlign )atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "depth" )
                    txtParagraph->setDepth( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "lineSpacing" )
                    txtParagraph->setLineSpacing( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "distBefore" )
                    txtParagraph->setDistBefore( atoi( ( *it ).m_strValue.c_str() ) );
                if ( ( *it ).m_strName == "distAfter" )
                    txtParagraph->setDistAfter( atoi( ( *it ).m_strValue.c_str() ) );
            }

            while ( parser.open( 0L, tag ) )
            {
                KOMLParser::parseTag( tag.c_str(), name, lst );

                // line
                if ( name == "LINE" )
                {
                    while ( parser.open( 0L, tag ) )
                    {
                        KOMLParser::parseTag( tag.c_str(), name, lst );

                        // object
                        if ( name == "OBJ" )
                        {
                            objPtr = new TxtObj();

                            while ( parser.open( 0L, tag ) )
                            {
                                KOMLParser::parseTag( tag.c_str(), name, lst );

                                // type
                                if ( name == "TYPE" )
                                {
                                    KOMLParser::parseTag( tag.c_str(), name, lst );
                                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                                    for( ; it != lst.end(); it++ )
                                    {
                                        if ( ( *it ).m_strName == "value" )
                                            objPtr->setType( ( TxtObj::ObjType )atoi( ( *it ).m_strValue.c_str() ) );
                                    }
                                }

                                // font
                                else if ( name == "FONT" )
                                {
                                    KOMLParser::parseTag( tag.c_str(), name, lst );
                                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                                    for( ; it != lst.end(); it++ )
                                    {
                                        if ( ( *it ).m_strName == "family" )
                                            font.setFamily( ( *it ).m_strValue.c_str() );
                                        if ( ( *it ).m_strName == "pointSize" )
                                            font.setPointSize( atoi( ( *it ).m_strValue.c_str() ) );
                                        if ( ( *it ).m_strName == "bold" )
                                            font.setBold( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                                        if ( ( *it ).m_strName == "italic" )
                                            font.setItalic( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                                        if ( ( *it ).m_strName == "underline" )
                                            font.setUnderline( ( bool )atoi( ( *it ).m_strValue.c_str() ) );
                                    }
                                    objPtr->setFont( font );
                                }

                                // color
                                else if ( name == "COLOR" )
                                {
                                    KOMLParser::parseTag( tag.c_str(), name, lst );
                                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                                    for( ; it != lst.end(); it++ )
                                    {
                                        if ( ( *it ).m_strName == "red" )
                                            r = atoi( ( *it ).m_strValue.c_str() );
                                        if ( ( *it ).m_strName == "green" )
                                            g = atoi( ( *it ).m_strValue.c_str() );
                                        if ( ( *it ).m_strName == "blue" )
                                            b = atoi( ( *it ).m_strValue.c_str() );
                                    }
                                    color.setRgb( r, g, b );
                                    objPtr->setColor( color );
                                }

                                // vertical align
                                else if ( name == "VERTALIGN" )
                                {
                                    KOMLParser::parseTag( tag.c_str(), name, lst );
                                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                                    for( ; it != lst.end(); it++ )
                                    {
                                        if ( ( *it ).m_strName == "value" )
                                            objPtr->setVertAlign( ( TxtObj::VertAlign )atoi( ( *it ).m_strValue.c_str() ) );
                                    }
                                }

                                // text
                                else if ( name == "TEXT" )
                                {
                                    QString tmp2;
                                    string tmp;

                                    KOMLParser::parseTag( tag.c_str(), name, lst );
                                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                                    for( ; it != lst.end(); it++ )
                                    {
                                        if ( ( *it ).m_strName == "value" )
                                            tmp2 = ( *it ).m_strValue.c_str();
                                    }

                                    if ( parser.readText( tmp ) )
                                    {
                                        QString s = tmp.c_str();
                                        if ( s.simplifyWhiteSpace().length() > 0 || utf8 )
                                        {
                                            tmp2 = tmp.c_str();
                                            utf8 = true;
                                        }
                                    }

                                    tmp2 = QString::fromUtf8( tmp2.ascii() );
                                    objPtr->append( tmp2 );
                                }

                                else
                                    cerr << "Unknown tag '" << tag << "' in OBJ" << endl;

                                if ( !parser.close( tag ) )
                                {
                                    cerr << "ERR: Closing Child" << endl;
                                    return;
                                }

                            }
                            txtParagraph->append( objPtr );
                        }

                        else
                            cerr << "Unknown tag '" << tag << "' in LINE" << endl;

                        if ( !parser.close( tag ) )
                        {
                            cerr << "ERR: Closing Child" << endl;
                            return;
                        }
                    }
                }

                else
                    cerr << "Unknown tag '" << tag << "' in PARAGRAPH" << endl;

                if ( !parser.close( tag ) )
                {
                    cerr << "ERR: Closing Child" << endl;
                    return;
                }
            }
            txtParagraph->setDepth( txtParagraph->getDepth() );
        }

        else
            cerr << "Unknown tag '" << tag << "' in TEXTOBJ" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            return;
        }
    }

    if ( ult.font->count() < 16 )
    {
        while ( ult.font->count() < 16 )
        {
            ult.font->append( new QFont( "times", 20 ) );
            ult.color->append( new QColor( Qt::red ) );
            ult.chr->append( new QChar( '-' ) );
            ult.ofont->append( new QFont() );
        }
    }
    ktextobject.setUnsortListType( ult );
}

/*================================================================*/
QString KPTextObject::decode( const QString &_str )
{
    QString str( _str );

    // HACK
    str.append( "_" );

    str.replace( QRegExp( "<" ), "&lt; " );
    str.replace( QRegExp( ">" ), "&gt; " );

    str.remove( str.length() - 1, 1 );

    return QString( str );
}


