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
#include <kprcommand.h>

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

#include <qrichtext_p.h>
#include <kotextobject.h>
#include <kostyle.h>
#include <kotextdocument.h>
#include <kotextformatter.h>
#include <kotextformat.h>
#include <kozoomhandler.h>

#include <qfont.h>

#include "kptextobject.moc"
#include "page.h"
#include <koAutoFormat.h>
#include <koparagcounter.h>
#include <kotextparag.h>
#include <koDataTool.h>
#include <qpopupmenu.h>
#include <koVariable.h>
#include <koVariableDlgs.h>

using namespace std;

/******************************************************************/
/* Class: KPTextObject                                            */
/******************************************************************/

const QString &KPTextObject::tagTEXTOBJ=KGlobal::staticQString("TEXTOBJ");
const QString &KPTextObject::attrLineSpacing=KGlobal::staticQString("lineSpacing");
const QString &KPTextObject::attrParagSpacing=KGlobal::staticQString("paragSpacing");
const QString &KPTextObject::attrMargin=KGlobal::staticQString("margin");
const QString &KPTextObject::attrBulletType1=KGlobal::staticQString("bulletType1");
const QString &KPTextObject::attrBulletType2=KGlobal::staticQString("bulletType2");
const QString &KPTextObject::attrBulletType3=KGlobal::staticQString("bulletType3");
const QString &KPTextObject::attrBulletType4=KGlobal::staticQString("bulletType4");
const QString &KPTextObject::attrBulletColor1=KGlobal::staticQString("bulletColor1");
const QString &KPTextObject::attrBulletColor2=KGlobal::staticQString("bulletColor2");
const QString &KPTextObject::attrBulletColor3=KGlobal::staticQString("bulletColor3");
const QString &KPTextObject::attrBulletColor4=KGlobal::staticQString("bulletColor4");
const QString &KPTextObject::attrObjType=KGlobal::staticQString("objType");
const QString &KPTextObject::tagP=KGlobal::staticQString("P");
const QString &KPTextObject::attrAlign=KGlobal::staticQString("align");
const QString &KPTextObject::attrType=KGlobal::staticQString("type");
const QString &KPTextObject::attrDepth=KGlobal::staticQString("depth");
const QString &KPTextObject::tagTEXT=KGlobal::staticQString("TEXT");
const QString &KPTextObject::attrFamily=KGlobal::staticQString("family");
const QString &KPTextObject::attrPointSize=KGlobal::staticQString("pointSize");
const QString &KPTextObject::attrBold=KGlobal::staticQString("bold");
const QString &KPTextObject::attrItalic=KGlobal::staticQString("italic");
const QString &KPTextObject::attrUnderline=KGlobal::staticQString("underline");
const QString &KPTextObject::attrStrikeOut=KGlobal::staticQString("strikeOut");
const QString &KPTextObject::attrColor=KGlobal::staticQString("color");
const QString &KPTextObject::attrWhitespace=KGlobal::staticQString("whitespace");
const QString &KPTextObject::attrTextBackColor=KGlobal::staticQString("textbackcolor");
const QString &KPTextObject::attrVertAlign=KGlobal::staticQString("VERTALIGN");

/*================ default constructor ===========================*/
KPTextObject::KPTextObject(  KPresenterDoc *doc )
    : KP2DObject()
{
    m_doc=doc;

    KoTextDocument * textdoc = new KoTextDocument( doc->zoomHandler(),
                                                   new KoTextFormatCollection( doc->defaultFont() ));

    m_textobj = new KoTextObject( textdoc, doc->standardStyle());

    brush = Qt::NoBrush;
    pen = QPen( Qt::black, 1, Qt::NoPen );
    drawEditRect = true;
    drawEmpty = true;

    connect( m_textobj, SIGNAL( newCommand( KCommand * ) ),
             SLOT( slotNewCommand( KCommand * ) ) );
    connect( m_textobj, SIGNAL( availableHeightNeeded() ),
             SLOT( slotAvailableHeightNeeded() ) );
    connect( m_textobj, SIGNAL( repaintChanged( KoTextObject* ) ),
             SLOT( slotRepaintChanged() ) );

    // Send our "repaintChanged" signals to the document.
    connect( this, SIGNAL( repaintChanged( KPTextObject * ) ),
             m_doc, SLOT( slotRepaintChanged( KPTextObject * ) ) );
}

KPTextObject::~KPTextObject()
{
}

/*======================= set size ===============================*/
void KPTextObject::setSize( int _width, int _height )
{
    KPObject::setSize( _width, _height );
    if ( move )
        return;

    textDocument()->setWidth( KoTextZoomHandler::ptToLayoutUnit( _width ) );
    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*======================= set size ===============================*/
void KPTextObject::resizeBy( int _dx, int _dy )
{
    KPObject::resizeBy( _dx, _dy );
    if ( move )
        return;

    if ( fillType == FT_GRADIENT && gradient )
        gradient->setSize( getSize() );
}

/*========================= save =================================*/
QDomDocumentFragment KPTextObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc);
    fragment.appendChild(saveKTextObject( doc ));
    return fragment;
}

/*========================== load ================================*/
void KPTextObject::load(const QDomElement &element)
{
    KP2DObject::load(element);
    QDomElement e=element.namedItem(tagTEXTOBJ).toElement();
    if(!e.isNull()) {

#if 0
        ktextobject.document()->setLineSpacing( e.attribute( attrLineSpacing ).toInt() );
        ktextobject.document()->setParagSpacing( e.attribute( attrParagSpacing ).toInt() );
        ktextobject.document()->setMargin( e.attribute( attrMargin ).toInt() );
        KTextEditDocument::TextSettings settings = ktextobject.document()->textSettings();
        settings.bulletColor[0] = QColor( e.attribute( attrBulletColor1, Qt::black.name() ) );
        settings.bulletColor[1] = QColor( e.attribute( attrBulletColor2, Qt::black.name() ) );
        settings.bulletColor[2] = QColor( e.attribute( attrBulletColor3, Qt::black.name() ) );
        settings.bulletColor[3] = QColor( e.attribute( attrBulletColor4, Qt::black.name() ) );
        settings.bulletType[0] = (KTextEditDocument::Bullet)e.attribute( attrBulletType1, 0 ).toInt();
        settings.bulletType[1] = (KTextEditDocument::Bullet)e.attribute( attrBulletType2, 0 ).toInt();
        settings.bulletType[2] = (KTextEditDocument::Bullet)e.attribute( attrBulletType3, 0 ).toInt();
        settings.bulletType[3] = (KTextEditDocument::Bullet)e.attribute( attrBulletType4, 0 ).toInt();
        ktextobject.document()->setTextSettings( settings );
        QString type = e.attribute( attrObjType );
        int t = -1;
        if ( !type.isEmpty() ) {
            if ( type == "1" )
                t = KTextEdit::EnumList;
            if ( type == "2" )
                t = KTextEdit::BulletList;
        }
        loadKTextObject( e, t );
#endif
        loadKTextObject( e, -1 /*TODO*/ );
    }
    setSize( ext.width(), ext.height() );
}

/*========================= draw =================================*/
void KPTextObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    draw( _painter, _diffx, _diffy, false, 0L, true );
}

void KPTextObject::draw( QPainter *_painter, int _diffx, int _diffy,
                         bool onlyChanged, QTextCursor* cursor, bool resetChanged )
{
    _painter->save();
    setupClipRegion( _painter, getBoundingRect( _diffx, _diffy ) );

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    _painter->setPen( pen );
    _painter->setBrush( brush );

    // Handle the rotation, draw the background/border, then call drawTextObject()
    int penw = pen.width() / 2;
    _painter->translate( ox, oy );
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
        drawTextObject( _painter, onlyChanged, cursor, resetChanged );
    }
    else
    {
        QRect br = QRect( 0, 0, ow, oh );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int yPos = -rr.y();
        int xPos = -rr.x();
        br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m;
        m.translate( pw / 2, ph / 2 );
        m.rotate( angle );

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
        drawTextObject( _painter, onlyChanged, cursor, resetChanged );
    }
    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

// This method simply draws the paragraphs (and their shadow) in the given painter
// Assumes the painter is already set up correctly.
void KPTextObject::drawTextObject( QPainter* _painter, bool onlyChanged, QTextCursor* cursor, bool resetChanged )
{
    if ( shadowDistance > 0 )
    {
        int sx = 0;
        int sy = 0;
        getShadowCoords( sx, sy );
        _painter->save();
        _painter->translate( sx, sy );
#if 0
        // TODO
        ktextobject.document()->enableDrawAllInOneColor( shadowColor );
#endif
        drawText( _painter, onlyChanged, 0L, false );
        // TODO disable 'drawInAllOneColor'
        _painter->restore();
    }
    // Now draw the normal text
    drawText( _painter, onlyChanged, cursor, resetChanged );
}

void KPTextObject::drawText( QPainter* _painter, bool onlyChanged, QTextCursor* cursor, bool resetChanged )
{
    kdDebug() << "KPTextObject::drawText onlyChanged=" << onlyChanged << " cursor=" << cursor << " resetChanged=" << resetChanged << endl;
    QColorGroup cg = QApplication::palette().active();
    QRect r( 0, 0, ext.width(), ext.height() );

    if ( specEffects )
    {
        switch ( effect2 )
        {
        case EF2T_PARA:
            kdDebug(33001) << "KPTextObject::draw onlyCurrStep=" << onlyCurrStep << " subPresStep=" << subPresStep << endl;
            drawParags( _painter, ( onlyCurrStep ? subPresStep : 0 ), subPresStep );
            break;
        default:
            Qt3::QTextParag * lastFormatted = textDocument()->drawWYSIWYG(
                _painter, r.x(), r.y(), r.width(), r.height(),
                cg, m_doc->zoomHandler(), // TODO (long term) the view's zoomHandler
                onlyChanged, cursor != 0, cursor, resetChanged );
        }
    }
    else
    {
        //kdDebug() << "KPTextObject::drawText r=" << DEBUGRECT(r) << endl;
        Qt3::QTextParag * lastFormatted = textDocument()->drawWYSIWYG(
            _painter, r.x(), r.y(), r.width(), r.height(),
            cg, m_doc->zoomHandler(), // TODO (long term) the view's zoomHandler
            onlyChanged, cursor != 0, cursor, resetChanged );
    }
}

/*========================= zoom =================================*/
void KPTextObject::zoom( float _fakt )
{
    kdDebug()<<"KPTextObject::zoom( float _fakt ) :"<< _fakt<<endl;
    KPObject::zoom( _fakt );
#if 0
    ktextobject.zoom( _fakt );
#endif
}

/*==================== zoom orig =================================*/
void KPTextObject::zoomOrig()
{
    kdDebug()<<"KPTextObject::zoomOrig()\n";
    KPObject::zoomOrig();
#if 0
    ktextobject.unzoom();
#endif
}

/*================================================================*/
void KPTextObject::extendObject2Contents( KPresenterView */*view*/ )
{
#if 0
    QSize s( ktextobject.neededSize() );
    setSize( s.width(), s.height() );
#endif
}

/*=========================== save ktextobject ===================*/
QDomElement KPTextObject::saveKTextObject( QDomDocument& doc )
{
#if 0
    KTextEditParag *parag = ktextobject.document()->firstParag();
    KTextEditDocument::TextSettings textSettings = ktextobject.document()->textSettings();
#endif

    QDomElement textobj=doc.createElement(tagTEXTOBJ);
#if 0
    textobj.setAttribute(attrLineSpacing, ktextobject.document()->lineSpacing());
    textobj.setAttribute(attrParagSpacing, ktextobject.document()->paragSpacing());
    textobj.setAttribute(attrMargin, ktextobject.document()->margin());
    textobj.setAttribute(attrBulletType1, (int)textSettings.bulletType[0]);
    textobj.setAttribute(attrBulletType2, (int)textSettings.bulletType[1]);
    textobj.setAttribute(attrBulletType3, (int)textSettings.bulletType[2]);
    textobj.setAttribute(attrBulletType4, (int)textSettings.bulletType[3]);
    textobj.setAttribute(attrBulletColor1, textSettings.bulletColor[0].name());
    textobj.setAttribute(attrBulletColor2, textSettings.bulletColor[1].name());
    textobj.setAttribute(attrBulletColor3, textSettings.bulletColor[2].name());
    textobj.setAttribute(attrBulletColor4, textSettings.bulletColor[3].name());
#endif
    KoTextParag *parag = static_cast<KoTextParag*> (textDocument()->firstParag());
    // ### fix this loop (Werner)
    while ( parag ) {
        QDomElement paragraph=doc.createElement(tagP);
        int tmpAlign=0;
        switch(parag->alignment())
        {
        case Qt::AlignLeft:
            tmpAlign=1;
            break;
        case Qt::AlignRight:
            tmpAlign=2;
            break;
        case Qt::AlignCenter:
            tmpAlign=4;
            break;
        default:
            tmpAlign=1;
        }
        if(tmpAlign!=1)
            paragraph.setAttribute(attrAlign, tmpAlign);

        saveParagLayout( parag->paragLayout(), paragraph );
#if 0
        paragraph.setAttribute(attrType, (int)parag->type());
        paragraph.setAttribute(attrDepth, parag->listDepth());
        KTextEditFormat *lastFormat = 0;
#endif
        KoTextFormat *lastFormat = 0;
        QString tmpText, tmpFamily, tmpColor, tmpTextBackColor;
        int tmpPointSize=10;
        unsigned int tmpBold=false, tmpItalic=false, tmpUnderline=false,tmpStrikeOut=false,tmpVerticalAlign=false;
        for ( int i = 0; i < parag->length(); ++i ) {
            //KTextEditString::Char *c = parag->at( i );
            QTextStringChar &c = parag->string()->at(i);
            if ( !lastFormat || c.format()->key() != lastFormat->key() ) {
                if ( lastFormat )
                    paragraph.appendChild(saveHelper(tmpText, tmpFamily, tmpColor, tmpPointSize,
                                                     tmpBold, tmpItalic, tmpUnderline,tmpStrikeOut,tmpTextBackColor,tmpVerticalAlign, doc));
                lastFormat = static_cast<KoTextFormat*> (c.format());
                tmpText="";
                tmpFamily=lastFormat->font().family();
                tmpPointSize=lastFormat->font().pointSize();
                tmpBold=static_cast<unsigned int>(lastFormat->font().bold());
                tmpItalic=static_cast<unsigned int>(lastFormat->font().italic());
                tmpUnderline=static_cast<unsigned int>(lastFormat->font().underline());
                tmpStrikeOut=static_cast<unsigned int>(lastFormat->font().strikeOut());
                tmpColor=lastFormat->color().name();
                tmpVerticalAlign=static_cast<unsigned int>(lastFormat->vAlign());
                if(lastFormat->textBackgroundColor().isValid())
                    tmpTextBackColor=lastFormat->textBackgroundColor().name();
            }
            tmpText+=c.c;
        }
        if ( lastFormat ) {
            paragraph.appendChild(saveHelper(tmpText, tmpFamily, tmpColor, tmpPointSize,
                                             tmpBold, tmpItalic, tmpUnderline, tmpStrikeOut,tmpTextBackColor ,tmpVerticalAlign, doc));
        }
        textobj.appendChild(paragraph);
        parag = static_cast<KoTextParag*>( parag->next());
    }
    return textobj;
}

QDomElement KPTextObject::saveHelper(const QString &tmpText, const QString &tmpFamily, const QString &tmpColor,
                                     int tmpPointSize, unsigned int tmpBold, unsigned int tmpItalic,
                                     unsigned int tmpUnderline, unsigned int tmpStrikeOut, const QString & tmpTextBackColor, unsigned int tmpVerticalAlign, QDomDocument &doc) {
    QDomElement element=doc.createElement(tagTEXT);
    element.setAttribute(attrFamily, tmpFamily);
    element.setAttribute(attrPointSize, tmpPointSize);

    if(tmpBold)
        element.setAttribute(attrBold, tmpBold);
    if(tmpItalic)
        element.setAttribute(attrItalic, tmpItalic);
    if(tmpUnderline)
        element.setAttribute(attrUnderline, tmpUnderline);
    if(tmpStrikeOut)
        element.setAttribute(attrStrikeOut, tmpStrikeOut);
    element.setAttribute(attrColor, tmpColor);

    if(!tmpTextBackColor.isEmpty())
        element.setAttribute(attrColor, tmpTextBackColor);
    if(tmpVerticalAlign!=Qt::AlignLeft)
        element.setAttribute(attrVertAlign,tmpVerticalAlign);
    if(tmpText.stripWhiteSpace().isEmpty())
        // working around a bug in QDom
        element.setAttribute(attrWhitespace, tmpText.length());
    element.appendChild(doc.createTextNode(tmpText));
    return element;
}

/*====================== load ktextobject ========================*/
void KPTextObject::loadKTextObject( const QDomElement &elem, int type )
{
    QDomElement e = elem.firstChild().toElement();
    KoTextParag *lastParag = static_cast<KoTextParag *>(textDocument()->firstParag());
    lastParag->remove( 0, 1 ); // ?
    int i = 0;
    int listNum = 0;
    //KTextEditDocument::TextSettings settings = ktextobject.document()->textSettings();
    int lineSpacing = 0, paragSpacing = 0;
    while ( !e.isNull() ) {
        if ( e.tagName() == tagP ) {
            QDomElement n = e.firstChild().toElement();
#if 0
            if ( type != -1 )
                lastParag->setType( (KTextEditParag::Type)type );
            else
                lastParag->setType( (KTextEditParag::Type)e.attribute( attrType ).toInt() );
#endif
            KoParagLayout paragLayout = loadParagLayout(e);
            lastParag->setParagLayout( paragLayout );

            if(e.hasAttribute(attrAlign))
            {
                int tmpAlign=e.attribute( attrAlign ).toInt();
                if(tmpAlign==1)
                    lastParag->setAlignment(Qt::AlignLeft);
                else if(tmpAlign==2)
                    lastParag->setAlignment(Qt::AlignRight);
                else if(tmpAlign==4)
                    lastParag->setAlignment(Qt::AlignCenter);
                else
                    kdDebug()<<"Error in e.attribute( attrAlign ).toInt()\n";
            }
            // ## lastParag->setListDepth( e.attribute( attrDepth ).toInt() );
            // TODO check/convert values
            lineSpacing = QMAX( e.attribute( attrLineSpacing ).toInt(), lineSpacing );
            paragSpacing = QMAX( QMAX( e.attribute( "distBefore" ).toInt(), e.attribute( "distAfter" ).toInt() ), paragSpacing );
            while ( !n.isNull() ) {
                if ( n.tagName() == tagTEXT ) {
                    QString family = n.attribute( attrFamily );
                    int size = n.attribute( attrPointSize ).toInt();
                    bool bold=false;
                    if(n.hasAttribute(attrBold))
                        bold = (bool)n.attribute( attrBold ).toInt();
                    bool italic = false;
                    if(n.hasAttribute(attrItalic))
                        bold=(bool)n.attribute( attrItalic ).toInt();
                    bool underline=false;
                    if(n.hasAttribute( attrUnderline ))
                        underline = (bool)n.attribute( attrUnderline ).toInt();
                    bool strikeOut=false;
                    if(n.hasAttribute(attrStrikeOut))
                        strikeOut = (bool)n.attribute( attrStrikeOut ).toInt();

                    QString color = n.attribute( attrColor );
                    QFont fn( family );
                    fn.setPointSize( KoTextZoomHandler::ptToLayoutUnit( size ) );
                    fn.setBold( bold );
                    fn.setItalic( italic );
                    fn.setUnderline( underline );
                    fn.setStrikeOut( strikeOut );
                    QColor col( color );

                    //todo FIXME : KoTextFormat

                    KoTextFormat *fm = static_cast<KoTextFormat*> (textDocument()->formatCollection()->format( fn, col ));
                    QString textBackColor=n.attribute(attrTextBackColor);
                    if(!textBackColor.isEmpty())
                    {
                        QColor tmpCol(textBackColor);
                        tmpCol=tmpCol.isValid() ? tmpCol : QApplication::palette().color( QPalette::Active, QColorGroup::Base );
                        fm->setTextBackgroundColor(tmpCol);
                    }
                    //TODO FIXME : value is correct, but format is not good :(
                    if(n.hasAttribute(attrVertAlign))
                        fm->setVAlign( static_cast<QTextFormat::VerticalAlignment>(n.attribute(attrVertAlign).toInt() ) );

                    QString txt = n.firstChild().toText().data();
                    if(n.hasAttribute(attrWhitespace)) {
                        int ws=n.attribute(attrWhitespace).toInt();
                        txt.fill(' ', ws);
                    }
                    n=n.nextSibling().toElement();
                    if ( txt.isEmpty() )
                        txt = ' ';
                    if ( !txt.isEmpty() ) {
                        if ( ( !txt[txt.length()-1].isSpace()  && n.isNull() ) )
                            txt+=' ';
                        lastParag->append( txt );
                        lastParag->setFormat( i, txt.length(), fm );
                        i += txt.length();
                    }
                }
                else
                    n = n.nextSibling().toElement();
            }
        } else if ( e.tagName() == "UNSORTEDLISTTYPE" ) {
            if ( listNum < 4 ) {
                QColor c( e.attribute( "red" ).toInt(), e.attribute( "green" ).toInt(), e.attribute( "blue" ).toInt() );
                // ## settings.bulletColor[ listNum++ ] = c;
            }
        }
        e = e.nextSibling().toElement();
        if ( e.isNull() )
            break;
        i = 0;
        if ( !lastParag->length() == 0 )
            lastParag = new KoTextParag( textDocument(), lastParag, 0 );
    }

#if 0
    settings.lineSpacing = lineSpacing;
    settings.paragSpacing = QMAX( ktextobject.document()->paragSpacing(), paragSpacing );
    ktextobject.document()->setTextSettings( settings );
    ktextobject.updateCurrentFormat();
#endif
}

KoParagLayout KPTextObject::loadParagLayout( QDomElement & parentElem)
{
    KoParagLayout layout;

    QDomElement element = parentElem.namedItem( "INDENTS" ).toElement();
    if ( !element.isNull() )
    {
        double val=0.0;
        if(element.hasAttribute("first"))
            val=element.attribute("first").toDouble();
        layout.margins[QStyleSheetItem::MarginFirstLine] = val;
        val=0.0;
        if(element.hasAttribute( "left"))
            val=element.attribute( "left").toDouble();
        layout.margins[QStyleSheetItem::MarginLeft] = val;
        val=0.0;
        if(element.hasAttribute("right"))
            val=element.attribute("right").toDouble();
        layout.margins[QStyleSheetItem::MarginRight] = val;
    }

    element = parentElem.namedItem( "COUNTER" ).toElement();
    if ( !element.isNull() )
    {
        layout.counter = new KoParagCounter;
        layout.counter->load( element );
    }
    return layout;
}

void KPTextObject::saveParagLayout( const KoParagLayout& layout, QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element;
    if ( layout.margins[QStyleSheetItem::MarginFirstLine] != 0 ||
         layout.margins[QStyleSheetItem::MarginLeft] != 0 ||
         layout.margins[QStyleSheetItem::MarginRight] != 0 )
    {
        element = doc.createElement( "INDENTS" );
        parentElem.appendChild( element );
        if ( layout.margins[QStyleSheetItem::MarginFirstLine] != 0 )
            element.setAttribute( "first", layout.margins[QStyleSheetItem::MarginFirstLine] );
        if ( layout.margins[QStyleSheetItem::MarginLeft] != 0 )
            element.setAttribute( "left", layout.margins[QStyleSheetItem::MarginLeft] );
        if ( layout.margins[QStyleSheetItem::MarginRight] != 0 )
            element.setAttribute( "right", layout.margins[QStyleSheetItem::MarginRight] );
    }

    if ( layout.counter && layout.counter->numbering() != KoParagCounter::NUM_NONE )
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        if (layout.counter )
            layout.counter->save( element );
    }
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

void KPTextObject::drawParags( QPainter */*p*/, int /*from*/, int /*to*/ )
{
#if 0
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
#endif
}

KoTextDocument * KPTextObject::textDocument() const
{
    return m_textobj->textDocument();
}

void KPTextObject::slotNewCommand( KCommand * cmd)
{
    m_doc->addCommand(cmd);
}

void KPTextObject::slotAvailableHeightNeeded()
{
    m_textobj->setAvailableHeight(getSize().height ());
    kdDebug()<<"slotAvailableHeightNeeded: height=:"<<getSize().height()<<endl;
}

void KPTextObject::slotRepaintChanged()
{
    emit repaintChanged( this );
}

KPTextView * KPTextObject::createKPTextView( Page * _page )
{
    return new KPTextView( this, _page );
}


void KPTextObject::removeHighlight ()
{
    m_textobj->removeHighlight();
}

void KPTextObject::highlightPortion( Qt3::QTextParag * parag, int index, int length, Page */*_page*/ )
{
    m_textobj->highlightPortion( parag, index, length );
#if 0
    QRect expose = canvas->viewMode()->normalToView( paragRect( parag ) );
    canvas->ensureVisible( (expose.left()+expose.right()) / 2,  // point = center of the rect
                           (expose.top()+expose.bottom()) / 2,
                           (expose.right()-expose.left()) / 2,  // margin = half-width of the rect
                           (expose.bottom()-expose.top()) / 2);
#endif
}


KPTextView::KPTextView( KPTextObject * txtObj,Page *_page )
    : KoTextView( txtObj->textObject() )
{
    m_page=_page;
    m_kptextobj=txtObj;
    KoTextView::setReadWrite( txtObj->kPresenterDocument()->isReadWrite() );
    connect( textView(), SIGNAL( cut() ), SLOT( cut() ) );
    connect( textView(), SIGNAL( copy() ), SLOT( copy() ) );
    connect( textView(), SIGNAL( paste() ), SLOT( paste() ) );
    updateUI( true, true );
    m_actionList.setAutoDelete( true );
}


void KPTextView::cut()
{
    if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
        copy();
        textObject()->removeSelectedText( cursor() );
    }
}

void KPTextView::copy()
{
    kdDebug()<<"KPTextView::copy()\n";
    //todo
}

void KPTextView::paste()
{
    kdDebug()<<"KPTextView::paste()\n";
    //todo
}

void KPTextView::updateUI( bool updateFormat, bool force  )
{
    kdDebug()<<"updateUI( bool updateFormat, bool force  )\n";
    KoTextView::updateUI( updateFormat, force  );
    // Paragraph settings
    KoTextParag * parag = static_cast<KoTextParag*>( cursor()->parag());
    if ( m_paragLayout.alignment != parag->alignment() || force ) {
        m_paragLayout.alignment = parag->alignment();
        m_page->getView()->alignChanged(  m_paragLayout.alignment );
    }

    // Counter
    if ( !m_paragLayout.counter )
        m_paragLayout.counter = new KoParagCounter; // we can afford to always have one here
    KoParagCounter::Style cstyle = m_paragLayout.counter->style();
    if ( parag->counter() )
        *m_paragLayout.counter = *parag->counter();
    else
    {
        m_paragLayout.counter->setNumbering( KoParagCounter::NUM_NONE );
        m_paragLayout.counter->setStyle( KoParagCounter::STYLE_NONE );
    }
    if ( m_paragLayout.counter->style() != cstyle || force )
    {
        m_page->getView()->showCounter( * m_paragLayout.counter );
    }
    if(m_paragLayout.leftBorder!=parag->leftBorder() ||
       m_paragLayout.rightBorder!=parag->rightBorder() ||
       m_paragLayout.topBorder!=parag->topBorder() ||
       m_paragLayout.bottomBorder!=parag->bottomBorder() || force )
    {
        m_paragLayout.leftBorder = parag->leftBorder();
        m_paragLayout.rightBorder = parag->rightBorder();
        m_paragLayout.topBorder = parag->topBorder();
        m_paragLayout.bottomBorder = parag->bottomBorder();
        //todo
        //m_canvas->gui()->getView()->showParagBorders( m_paragLayout.leftBorder, m_paragLayout.rightBorder, m_paragLayout.topBorder, m_paragLayout.bottomBorder );
    }

    if ( !parag->style() )
        kdWarning() << "Paragraph " << parag->paragId() << " has no style" << endl;
    else if ( m_paragLayout.style != parag->style() || force )
    {
        m_paragLayout.style = parag->style();
        //todo
        //m_canvas->gui()->getView()->showStyle( m_paragLayout.style->name() );
    }



    if( m_paragLayout.margins[QStyleSheetItem::MarginLeft] != parag->margin(QStyleSheetItem::MarginLeft)
        || m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] != parag->margin(QStyleSheetItem::MarginFirstLine)
        || m_paragLayout.margins[QStyleSheetItem::MarginRight] != parag->margin(QStyleSheetItem::MarginRight)
	|| force )
    {
        m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] = parag->margin(QStyleSheetItem::MarginFirstLine);
        m_paragLayout.margins[QStyleSheetItem::MarginLeft] = parag->margin(QStyleSheetItem::MarginLeft);
        m_paragLayout.margins[QStyleSheetItem::MarginRight] = parag->margin(QStyleSheetItem::MarginRight);
        //todo Perhaps change koRuler properties
        //m_canvas->gui()->getView()->showRulerIndent( m_paragLayout.margins[QStyleSheetItem::MarginLeft], m_paragLayout.margins[QStyleSheetItem::MarginFirstLine], m_paragLayout.margins[QStyleSheetItem::MarginRight] );
    }
}

void KPTextView::ensureCursorVisible()
{

    kdDebug()<<"KPTextView::ensureCursorVisible()\n";
}

void KPTextView::doAutoFormat( QTextCursor* cursor, KoTextParag *parag, int index, QChar ch )
{
    KoAutoFormat * autoFormat = m_kptextobj->kPresenterDocument()->getAutoFormat();
    if ( autoFormat )
        autoFormat->doAutoFormat( cursor, parag, index, ch, textObject());
}

void KPTextView::startDrag()
{
    kdDebug()<<"KPTextView::startDrag()\n";
}


void KPTextView::showFormat( KoTextFormat *format )
{
    kdDebug()<<"KPTextView::showFormat( KoTextFormat *format )\n";
    m_page->getView()->showFormat( *format );
}

void KPTextView::pgUpKeyPressed()
{
    kdDebug()<<"KPTextView::pgUpKeyPressed()\n";
    //todo
}

void KPTextView::pgDownKeyPressed()
{
    kdDebug()<<"KPTextView::pgDownKeyPressed()\n";
    //todo
}

void KPTextView::keyPressEvent( QKeyEvent *e )
{
    handleKeyPressEvent(e);
}

void KPTextView::clearSelection()
{
    if ( textDocument()->hasSelection( QTextDocument::Standard ) )
    {
        textDocument()->removeSelection(QTextDocument::Standard );
    }
}

void KPTextView::insertSpecialChar(QChar _c)
{
    if(textObject()->hasSelection() )
        m_kptextobj->kPresenterDocument()->addCommand(textObject()->replaceSelectionCommand(
            cursor(), _c, QTextDocument::Standard, i18n("Insert Special Char")));
    else
        textObject()->insert( cursor(), currentFormat(), _c, false /* no newline */, true, i18n("Insert Special Char") );
}

void KPTextView::insertLink(const QString &_linkName, const QString & hrefName)
{
    KoTextFormat format=*currentFormat();
    format.setAnchorName(_linkName);
    format.setAnchorHref( hrefName);
    textObject()->insert( cursor(), &format, _linkName+" " , false , true, i18n("Insert Link") );
}

void KPTextView::insertSoftHyphen()
{
    textObject()->insert( cursor(), currentFormat(), QChar(0xad) /* see QRichText */,
                          false /* no newline */, true, i18n("Insert Soft Hyphen") );
}

void KPTextView::selectAll()
{
    textObject()->selectAll( true );
}

void KPTextView::drawCursor( bool b )
{
    KoTextView::drawCursor( b );
    if ( !cursor()->parag() )
        return;
    if ( !kpTextObject()->kPresenterDocument()->isReadWrite() )
        return;

    // We repaint the whole object.
    // TODO a kword-like painting method (many changes required though)
    kpTextObject()->kPresenterDocument()->repaint( kpTextObject() );
}

void KPTextView::mousePressEvent( QMouseEvent *e, const QPoint &_pos)
{
    handleMousePressEvent( e, _pos );
}

void KPTextView::mouseDoubleClickEvent( QMouseEvent *e, const QPoint &pos)
{
    handleMouseDoubleClickEvent( e, pos  );
}

void KPTextView::mouseMoveEvent( QMouseEvent *e, const QPoint &_pos )
{
    handleMouseMoveEvent(e, _pos );
}

void KPTextView::mouseReleaseEvent( QMouseEvent *, const QPoint & )
{
    handleMouseReleaseEvent();
}

void KPTextView::showPopup( KPresenterView *view, const QPoint &point )
{
    m_actionList.clear();
    m_actionList = dataToolActionList(view->kPresenterDoc()->instance());
    kdDebug() << "KPTextView::openPopupMenuInsideFrame plugging actionlist with " << m_actionList.count() << " actions" << endl;
    view->plugActionList( "datatools", m_actionList );
    QPopupMenu * popup = view->popupMenu("text_popup");
    Q_ASSERT(popup);
    if (popup)
        popup->popup( point ); // using exec() here breaks the spellcheck tool (event loop pb)
}


void KPTextView::insertCustomVariable( const QString &name)
{
     KoVariable * var = 0L;
     KPresenterDoc * doc = kpTextObject()->kPresenterDocument();
     var = new KoCustomVariable( textObject()->textDocument(), name, doc->variableFormatCollection()->format( "STRING" ),  doc->getVariableCollection());
     insertVariable( var);
}

void KPTextView::insertVariable( int type, int subtype )
{
    kdDebug() << "KWTextFrameSetEdit::insertVariable " << type << endl;
    KPresenterDoc * doc = kpTextObject()->kPresenterDocument();

    KoVariable * var = 0L;
    if ( type == VT_CUSTOM )
    {
        // Choose an existing variable
        KoVariableNameDia dia( m_page, doc->getVariableCollection()->getVariables() );
        if ( dia.exec() == QDialog::Accepted )
            var = new KoCustomVariable( textObject()->textDocument(), dia.getName(), doc->variableFormatCollection()->format( "STRING" ),doc->getVariableCollection() );
    }
    else
        var = KoVariable::createVariable( type, subtype,  doc->variableFormatCollection(), 0L, textObject()->textDocument(),doc,doc->getVariableCollection());

    insertVariable( var );
}

void KPTextView::insertVariable( KoVariable *var )
{
    if ( var )
    {
        CustomItemsMap customItemsMap;
        customItemsMap.insert( 0, var );
        kdDebug() << "KPTextView::insertVariable inserting into paragraph" << endl;
#ifdef DEBUG_FORMATS
        kdDebug() << "KPTextView::insertVariable currentFormat=" << currentFormat() << endl;
#endif
        textObject()->insert( cursor(), currentFormat(), KoTextObject::customItemChar(),
                                false, false, i18n("Insert Variable"),
                                customItemsMap );
        var->recalc();
        cursor()->parag()->invalidate(0);
        cursor()->parag()->setChanged( true );

        kpTextObject()->kPresenterDocument()->refreshMenuCustomVariable();
        kpTextObject()->kPresenterDocument()->repaint( kpTextObject() );
    }
}
