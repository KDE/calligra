// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "kptextobject.h"
#include "kpgradient.h"
#include "kprcommand.h"

#include <qwidget.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include <qapplication.h>
#include <qfontdatabase.h>

#include <klocale.h>
#include <kdebug.h>
#include <kdeversion.h>

#include "kpresenter_view.h"
#include "kpresenter_doc.h"
#include "kprbgspellcheck.h"
#include <korichtext.h>
#include <kotextobject.h>
#include <kostyle.h>
#include <kotextformatter.h>
#include <kozoomhandler.h>
#include "KPTextViewIface.h"
#include "KPTextObjectIface.h"
#include <kooasiscontext.h>
#include <koStyleStack.h>
#include <ktempfile.h>

#include <qfont.h>
#include <qfile.h>

#include "kptextobject.moc"
#include "kprcanvas.h"
#include "kprpage.h"
#include "koAutoFormat.h"
#include <koparagcounter.h>
#include <kaction.h>
#include <qpopupmenu.h>
#include <kovariable.h>
#include <koVariableDlgs.h>
#include "kprvariable.h"
#include <koRuler.h>
#include "kprdrag.h"
#include <qclipboard.h>
#include <koSize.h>
#include <float.h>
using namespace std;

#undef S_NONE // Solaris defines it in sys/signal.h

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


KPTextObject::KPTextObject(  KPresenterDoc *doc )
    : KP2DObject()
{
    m_doc=doc;
    m_textVertAlign = KP_TOP;
    // Default color should be QColor() ... but kpresenter isn't fully color-scheme-aware yet
    KoTextFormatCollection* fc = new KoTextFormatCollection( doc->defaultFont(), Qt::black, doc->globalLanguage(), doc->globalHyphenation() );
    KPrTextDocument * textdoc = new KPrTextDocument( this, fc );
    if ( m_doc->tabStopValue() != -1 )
        textdoc->setTabStops( m_doc->zoomHandler()->ptToLayoutUnitPixX( m_doc->tabStopValue() ));

    m_textobj = new KoTextObject( textdoc, m_doc->styleCollection()->findStyle( "Standard" ), this );

#ifdef HAVE_LIBKSPELL2
    m_doc->backSpeller()->registerNewTextObject( m_textobj );
#endif
    brush = Qt::NoBrush;
    brush.setColor(QColor());
    pen = defaultPen();
    drawEditRect = true;
    drawEmpty = true;
    editingTextObj = false;

    bleft = 0.0;
    btop = 0.0;
    bright = 0.0;
    bbottom = 0.0;
    alignVertical = 0.0;

    connect( m_textobj, SIGNAL( newCommand( KCommand * ) ),
             SLOT( slotNewCommand( KCommand * ) ) );
    connect( m_textobj, SIGNAL( availableHeightNeeded() ),
             SLOT( slotAvailableHeightNeeded() ) );
    connect( m_textobj, SIGNAL( repaintChanged( KoTextObject* ) ),
             SLOT( slotRepaintChanged() ) );

    // Send our "repaintChanged" signals to the document.
    connect( this, SIGNAL( repaintChanged( KPTextObject * ) ),
             m_doc, SLOT( slotRepaintChanged( KPTextObject * ) ) );
    connect(m_textobj, SIGNAL( showFormatObject(const KoTextFormat &) ),
            SLOT( slotFormatChanged(const KoTextFormat &)) );
    connect( m_textobj, SIGNAL( afterFormatting( int, KoTextParag*, bool* ) ),
             SLOT( slotAfterFormatting( int, KoTextParag*, bool* ) ) );
    connect( m_textobj, SIGNAL( paragraphDeleted( KoTextParag*) ),
             SLOT( slotParagraphDeleted(KoTextParag*) ));

}

KPTextObject::~KPTextObject()
{
    textDocument()->takeFlow();
    m_doc = 0L;
}

DCOPObject* KPTextObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPTextObjectIface( this );
    return dcop;
}

void KPTextObject::slotParagraphDeleted(KoTextParag*_parag)
{
    m_doc->spellCheckParagraphDeleted( _parag,  this);
}

QBrush KPTextObject::getBrush() const
{
    QBrush tmpBrush(brush);
    if(!tmpBrush.color().isValid())
        tmpBrush.setColor(QApplication::palette().color( QPalette::Active, QColorGroup::Base ));
    return tmpBrush;
}

void KPTextObject::resizeTextDocument( bool widthChanged, bool heightChanged )
{
    if ( heightChanged )
    {
        // Recalc available height
        slotAvailableHeightNeeded();
        // Recalc the vertical centering, if enabled
        recalcVerticalAlignment();
    }
    if ( widthChanged )
    {
        // not when simply changing the height, otherwise the auto-resize code
        // prevents making a textobject less high than it currently is.
        textDocument()->setWidth( m_doc->zoomHandler()->ptToLayoutUnitPixX( innerWidth() ) );
        m_textobj->setLastFormattedParag( textDocument()->firstParag() );
        m_textobj->formatMore( 2 );
    }
}

void KPTextObject::setSize( double _width, double _height )
{
    bool widthModified = KABS( _width - ext.width() ) > DBL_EPSILON ; // floating-point equality test
    bool heightModified = KABS( _height - ext.height() ) > DBL_EPSILON;
    if ( widthModified || heightModified )
    {
        KPObject::setSize( _width, _height );
        resizeTextDocument( widthModified, heightModified ); // will call formatMore() if widthModified
    }
}

QDomDocumentFragment KPTextObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc, offset);
    fragment.appendChild(saveKTextObject( doc ));
    return fragment;
}

bool KPTextObject::saveOasis( KoXmlWriter &xmlWriter, KoSavingContext& context, int indexObj  ) const
{
    xmlWriter.startElement( "draw:text-box" );
    // #### This should use KoGenStyle to share the style
    xmlWriter.addAttribute( "draw:style-name", KP2DObject::saveOasisBackgroundStyle( xmlWriter, context.mainStyles(), indexObj ) );
    if( !objectName.isEmpty())
        xmlWriter.addAttribute( "draw:name", objectName );
    m_textobj->saveOasisContent( xmlWriter, context );
    xmlWriter.endElement();
    return true;
}

void KPTextObject::saveOasisMarginElement( KoGenStyle &styleobjectauto ) const
{
    kdDebug()<<"void KPTextObject::saveOasisMarginElement( KoGenStyle &styleobjectauto )\n";
    if ( btop != 0.0 )
        styleobjectauto.addPropertyPt("fo:padding-top", btop );
    if ( bbottom != 0.0 )
        styleobjectauto.addPropertyPt("fo:padding-bottom", bbottom );
    if ( bleft != 0.0 )
        styleobjectauto.addPropertyPt("fo:padding-left", bleft );
    if ( bright != 0.0 )
        styleobjectauto.addPropertyPt("fo:padding-right", bright );

    //add vertical alignment
    switch( m_textVertAlign )
    {
    case KP_TOP:
        styleobjectauto.addProperty("draw:textarea-vertical-align", "top" );
        break;
    case KP_CENTER:
        styleobjectauto.addProperty("draw:textarea-vertical-align", "middle" );
        break;
    case KP_BOTTOM:
        styleobjectauto.addProperty("draw:textarea-vertical-align", "bottom" );
        break;
    }

    // fo:padding-top="1.372cm" fo:padding-bottom="0.711cm" fo:padding-left="1.118cm" fo:padding-right="1.27cm"
}

void KPTextObject::loadOasis(const QDomElement &element, KoOasisContext& context,
                             KPRLoadingInfo *info )
{
    KP2DObject::loadOasis(element, context, info);
    //todo other attribute
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "graphic" );
    if( styleStack.hasAttribute( "fo:padding-top" ) )
        btop = KoUnit::parseValue( styleStack.attribute( "fo:padding-top" ) );
    if( styleStack.hasAttribute( "fo:padding-bottom" ) )
        bbottom = KoUnit::parseValue( styleStack.attribute( "fo:padding-bottom" ) );
    if( styleStack.hasAttribute( "fo:padding-left") )
        bleft = KoUnit::parseValue( styleStack.attribute( "fo:padding-left" ) );
    if( styleStack.hasAttribute( "fo:padding-right" ) )
        bright = KoUnit::parseValue( styleStack.attribute( "fo:padding-right" ) );
    kdDebug()<<" KPTextObject::loadOasis : btp :"<<btop<<" bbottom :"<<bbottom<<" bleft :"<<bleft<<" bright :"<<bright<<endl;
    // vertical alignment
    if ( styleStack.hasAttribute( "draw:textarea-vertical-align" ) )
    {
        QString alignment = styleStack.attribute( "draw:textarea-vertical-align" );
        if ( alignment == "top" )
            m_textVertAlign= KP_TOP;
        else if ( alignment == "middle" )
            m_textVertAlign= KP_CENTER;
        else if ( alignment == "bottom" )
            m_textVertAlign= KP_BOTTOM;
    }
    kdDebug()<<" vertical Alignment :"<< ( ( m_textVertAlign== KP_TOP ) ? "top" : ( m_textVertAlign==  KP_CENTER ) ? "center": "bottom" )<<endl;
    resizeTextDocument(); // this will to formatMore()

    m_textobj->loadOasisContent( element, context, m_doc->styleCollection() );
}


double KPTextObject::load(const QDomElement &element)
{
    double offset=KP2DObject::load(element);
    QDomElement e=element.namedItem(tagTEXTOBJ).toElement();
    if(!e.isNull()) {
        if ( e.hasAttribute( "protectcontent"))
            setProtectContent((bool)e.attribute( "protectcontent" ).toInt());
        if (e.hasAttribute( "bleftpt"))
            bleft = e.attribute( "bleftpt").toDouble();
        if (e.hasAttribute( "brightpt"))
            bright = e.attribute( "brightpt").toDouble();
        if (e.hasAttribute( "btoppt"))
            btop = e.attribute( "btoppt").toDouble();
        if (e.hasAttribute( "bbottompt"))
            bbottom = e.attribute( "bbottompt").toDouble();
        if ( e.hasAttribute("verticalAlign"))
        {
            QString str =e.attribute("verticalAlign");
            if ( str == "bottom" )
                m_textVertAlign= KP_BOTTOM;
            else if ( str == "center" )
                m_textVertAlign= KP_CENTER;
            else if ( str == "top" )//never
                m_textVertAlign= KP_TOP;
        }
        if ( e.hasAttribute( "verticalValue" ))
            alignVertical = e.attribute( "verticalValue" ).toDouble();

        loadKTextObject( e );
    }

    shadowCompatibility();

    resizeTextDocument(); // this will to formatMore()
    return offset;
}

void KPTextObject::shadowCompatibility()
{
    if ( shadowDistance != 0)
    {
        int sx = 0;
        int sy = 0;
        switch ( shadowDirection )
        {
        case SD_LEFT_BOTTOM:
        case SD_LEFT:
        case SD_LEFT_UP:
            sx = - shadowDistance;
        case SD_RIGHT_UP:
        case SD_RIGHT:
        case SD_RIGHT_BOTTOM:
            sx = shadowDistance;
        default:
            break;
        }
        switch ( shadowDirection )
        {
        case SD_LEFT_UP:
        case SD_UP:
        case SD_RIGHT_UP:
            sy = - shadowDistance;
        case SD_LEFT_BOTTOM:
        case SD_BOTTOM:
        case SD_RIGHT_BOTTOM:
            sy = shadowDistance;
        default:
            break;
        }
        KoTextFormat tmpFormat;
        tmpFormat.setShadow( sx, sy, shadowColor );
        KCommand* cmd = m_textobj->setFormatCommand( &tmpFormat, KoTextFormat::ShadowText );
        delete cmd;
    }
    //force to reset shadow compatibility between koffice 1.1 and 1.2
    shadowDirection = SD_RIGHT_BOTTOM;
    shadowDistance = 0;
    shadowColor = Qt::gray;
}


// Standard paint method for KP2DObjects.
void KPTextObject::paint( QPainter *_painter, KoZoomHandler*_zoomHandler,
                          int pageNum, bool drawingShadow, bool drawContour )
{
    // Never draw shadow (in text objects, it's a character property, not an object property)
    KPrPage *p = m_doc->pageList().at( pageNum );
    recalcPageNum( p );
    if ( drawingShadow ) return;
    paint( _painter, _zoomHandler, false, 0L, true, drawContour );
}

// Special method for drawing a text object that is being edited
void KPTextObject::paintEdited( QPainter *_painter, KoZoomHandler*_zoomHandler,
                                bool onlyChanged, KoTextCursor* cursor, bool resetChanged )
{
    _painter->save();
    _painter->translate( _zoomHandler->zoomItX(orig.x()), _zoomHandler->zoomItY(orig.y()) );

    if ( angle != 0 )
        rotateObject(_painter,_zoomHandler);
    paint( _painter, _zoomHandler, onlyChanged, cursor, resetChanged, false /*not drawContour*/ );
    _painter->restore();
}

// Common functionality for the above 2 methods
void KPTextObject::paint( QPainter *_painter, KoZoomHandler*_zoomHandler,
                          bool onlyChanged, KoTextCursor* cursor, bool resetChanged,
                          bool drawContour )
{
    double ow = ext.width();
    double oh = ext.height();
    double pw = pen.width() / 2;
    if ( drawContour ) {
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
        _painter->setRasterOp( Qt::NotXorROP );
        _painter->drawRect( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw),
                            _zoomHandler->zoomItX(ow), _zoomHandler->zoomItY( oh) );

        return;
    }

    _painter->save();
    QPen pen2(pen);
    pen2.setWidth(_zoomHandler->zoomItX(pen.width()));
    //QRect clip=QRect(_zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw), _zoomHandler->zoomItX( ow - 2 * pw),_zoomHandler->zoomItY( oh - 2 * pw));
    //kdDebug(33001) << "KPTextObject::paint cliprect:" << DEBUGRECT(_zoomHandler->zoomRect( getBoundingRect() )) << endl;
    //setupClipRegion( _painter, clip );
    //for debug
    //_painter->fillRect( clip, Qt::blue );
    _painter->setPen( pen2 );

    if ( editingTextObj && _painter->device() && _painter->device()->devType() != QInternal::Printer)  // editing text object
        _painter->setBrush( QBrush( m_doc->txtBackCol(), Qt::SolidPattern ) );
    else {
        // Handle the rotation, draw the background/border, then call drawText()
        if ( fillType == FT_BRUSH || !gradient ) {
            _painter->setBrush( brush );
        }
        else {
            QSize size( _zoomHandler->zoomSize( ext ) );
            gradient->setSize( size );
            _painter->drawPixmap( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItX(pw), gradient->pixmap(), 0, 0,
                                  _zoomHandler->zoomItX( ow - 2 * pw ),
                                  _zoomHandler->zoomItY( oh - 2 * pw ) );
        }
    }
    if ( !editingTextObj || !onlyChanged )
    {
        /// #### Port this to KoBorder, see e.g. kword/kwframe.cc:590
        // (so that the border gets drawn OUTSIDE of the object area)
        _painter->drawRect( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItX(pw), _zoomHandler->zoomItX( ow - 2 * pw),
                            _zoomHandler->zoomItY( oh - 2 * pw) );
    }

    drawText( _painter, _zoomHandler, onlyChanged, cursor, resetChanged );
    _painter->restore();


    // And now draw the border for text objects.
    // When they are drawn outside of the object, this can be moved to the standard paint() method,
    // so that we don't have to do it while editing the object, maybe.
    if ( m_doc->firstView() && m_doc->firstView()->getCanvas()->getEditMode() &&
         getDrawEditRect() && getPen().style() == Qt::NoPen )
    {
        _painter->save();

        _painter->setPen( QPen( Qt::gray, 1, Qt::DotLine ) );
        _painter->setBrush( Qt::NoBrush );
        _painter->setRasterOp( Qt::NotXorROP );
        _painter->drawRect( 0, 0, _zoomHandler->zoomItX(ow), _zoomHandler->zoomItY( oh) );

        _painter->restore();
    }
}

// This method simply draws the paragraphs in the given painter
// Assumes the painter is already set up correctly.
void KPTextObject::drawText( QPainter* _painter, KoZoomHandler *zoomHandler, bool onlyChanged, KoTextCursor* cursor, bool resetChanged )
{
    //kdDebug(33001) << "KPTextObject::drawText onlyChanged=" << onlyChanged << " cursor=" << cursor << " resetChanged=" << resetChanged << endl;
    recalcVerticalAlignment();
    QColorGroup cg = QApplication::palette().active();
    _painter->save();
    _painter->translate( m_doc->zoomHandler()->zoomItX( bLeft()), m_doc->zoomHandler()->zoomItY( bTop()+alignVertical));
    if ( !editingTextObj || (_painter->device() && _painter->device()->devType() == QInternal::Printer))
        cg.setBrush( QColorGroup::Base, NoBrush );
    else
        cg.setColor( QColorGroup::Base, m_doc->txtBackCol() );

    QRect r = zoomHandler->zoomRect( KoRect( 0, 0, innerWidth(), innerHeight() ) );
    bool editMode = false;
    if( m_doc->firstView() && m_doc->firstView()->getCanvas())
        editMode = m_doc->firstView()->getCanvas()->getEditMode();

    uint drawingFlags = 0;
    if ( _painter->device() && _painter->device()->devType() != QInternal::Printer )
        drawingFlags |= KoTextDocument::DrawSelections;
    if ( m_doc->backgroundSpellCheckEnabled() && editMode )
        drawingFlags |= KoTextDocument::DrawMisspelledLine;
    if ( !editMode )
        drawingFlags |= KoTextDocument::DontDrawNoteVariable;
    if ( m_doc->viewFormattingChars() && editMode )
        drawingFlags |= KoTextDocument::DrawFormattingChars;

    if ( specEffects )
    {
        switch ( effect2 )
        {
        case EF2T_PARA:
            //kdDebug(33001) << "KPTextObject::draw onlyCurrStep=" << onlyCurrStep << " subPresStep=" << subPresStep << endl;
            drawParags( _painter, zoomHandler, cg, ( onlyCurrStep ? subPresStep : 0 ), subPresStep );
            break;
        default:
            /*KoTextParag * lastFormatted =*/ textDocument()->drawWYSIWYG(
                _painter, r.x(), r.y(), r.width(), r.height(),
                cg, zoomHandler,
                onlyChanged, cursor != 0, cursor, resetChanged, drawingFlags );
        }
    }
    else
    {

        //kdDebug(33001) << "KPTextObject::drawText r=" << DEBUGRECT(r) << endl;
        /*KoTextParag * lastFormatted = */ textDocument()->drawWYSIWYG(
            _painter, r.x(), r.y(), r.width(), r.height(),
            cg, zoomHandler,
            onlyChanged, cursor != 0, cursor, resetChanged, drawingFlags );
    }
    _painter->restore();
}

int KPTextObject::getSubPresSteps() const
{
    int paragraphs = 0;
    KoTextParag * parag = textDocument()->firstParag();
    for ( ; parag ; parag = parag->next() )
        paragraphs++;
    return paragraphs;
}


QDomElement KPTextObject::saveKTextObject( QDomDocument& doc )
{
#if 0
    KTextEditParag *parag = ktextobject.document()->firstParag();
    KTextEditDocument::TextSettings textSettings = ktextobject.document()->textSettings();
#endif

    QDomElement textobj=doc.createElement(tagTEXTOBJ);
    if ( isProtectContent() )
        textobj.setAttribute( "protectcontent", (int)isProtectContent());
    if (bleft !=0.0)
        textobj.setAttribute( "bleftpt", bleft );
    if (bright !=0.0)
        textobj.setAttribute( "brightpt", bright );
    if (btop !=0.0)
        textobj.setAttribute( "btoppt", btop );
    if (bbottom !=0.0)
        textobj.setAttribute( "bbottompt", bbottom );
    if ( m_textVertAlign != KP_TOP )
    {
        if ( m_textVertAlign == KP_BOTTOM )
            textobj.setAttribute( "verticalAlign", "bottom" );
        else if ( m_textVertAlign == KP_CENTER )
            textobj.setAttribute( "verticalAlign", "center" );
        else if ( m_textVertAlign == KP_TOP )//never
            textobj.setAttribute( "verticalAlign", "top" );
        textobj.setAttribute( "verticalValue",alignVertical );
    }
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
        saveParagraph( doc, parag, textobj, 0, parag->length()-2 );
        parag = static_cast<KoTextParag*>( parag->next());
    }
    return textobj;
}

void KPTextObject::saveFormat( QDomElement & element, KoTextFormat*lastFormat )
{
    QString tmpFamily, tmpColor, tmpTextBackColor;
    unsigned int tmpBold=false, tmpItalic=false, tmpUnderline=false,tmpStrikeOut=false;
    int tmpVerticalAlign=-1;

    tmpFamily=lastFormat->font().family();
    tmpBold=static_cast<unsigned int>(lastFormat->font().bold());
    tmpItalic=static_cast<unsigned int>(lastFormat->font().italic());
    tmpUnderline=static_cast<unsigned int>(lastFormat->underline());
    tmpStrikeOut=static_cast<unsigned int>(lastFormat->strikeOut());
    tmpColor=lastFormat->color().name();
    tmpVerticalAlign=static_cast<unsigned int>(lastFormat->vAlign());
    if(lastFormat->textBackgroundColor().isValid())
        tmpTextBackColor=lastFormat->textBackgroundColor().name();

    element.setAttribute(attrFamily, tmpFamily);
    element.setAttribute(attrPointSize, lastFormat->pointSize());

    if(tmpBold)
        element.setAttribute(attrBold, tmpBold);
    if(tmpItalic)
        element.setAttribute(attrItalic, tmpItalic);
    if ( lastFormat->underlineType()!= KoTextFormat::U_NONE )
    {
        if(lastFormat->doubleUnderline())
            element.setAttribute(attrUnderline, "double");
        if(lastFormat->underlineType()==KoTextFormat::U_SIMPLE_BOLD)
            element.setAttribute(attrUnderline, "single-bold");
        else if( lastFormat->underlineType()==KoTextFormat::U_WAVE)
            element.setAttribute(attrUnderline, "wave");
        else if(tmpUnderline)
            element.setAttribute(attrUnderline, tmpUnderline);
        QString strLineType=KoTextFormat::underlineStyleToString( lastFormat->underlineStyle() );
        element.setAttribute( "underlinestyleline", strLineType );
        if ( lastFormat->textUnderlineColor().isValid() )
            element.setAttribute( "underlinecolor", lastFormat->textUnderlineColor().name() );
    }
    if ( lastFormat->strikeOutType()!= KoTextFormat::S_NONE )
    {
        if ( lastFormat->doubleStrikeOut() )
            element.setAttribute(attrStrikeOut, "double");
        else if ( lastFormat->strikeOutType()== KoTextFormat::S_SIMPLE_BOLD)
            element.setAttribute(attrStrikeOut, "single-bold");
        else if(tmpStrikeOut)
            element.setAttribute(attrStrikeOut, tmpStrikeOut);
        QString strLineType=KoTextFormat::strikeOutStyleToString( lastFormat->strikeOutStyle() );
        element.setAttribute( "strikeoutstyleline", strLineType );

    }
    element.setAttribute(attrColor, tmpColor);

    if(!tmpTextBackColor.isEmpty())
        element.setAttribute(attrTextBackColor, tmpTextBackColor);
    if(tmpVerticalAlign!=-1)
    {
        element.setAttribute(attrVertAlign,tmpVerticalAlign);
        if(lastFormat->relativeTextSize()!=0.66)
            element.setAttribute("relativetextsize",lastFormat->relativeTextSize());
    }

    if ( lastFormat->shadowDistanceX() != 0
        || lastFormat->shadowDistanceY() != 0)
        element.setAttribute("text-shadow", lastFormat->shadowAsCss());
    if ( lastFormat->offsetFromBaseLine()!=0 )
        element.setAttribute( "offsetfrombaseline" , lastFormat->offsetFromBaseLine());
    if ( lastFormat->wordByWord() )
        element.setAttribute("wordbyword", true);
    if ( lastFormat->attributeFont()!= KoTextFormat::ATT_NONE )
        element.setAttribute("fontattribute", KoTextFormat::attributeFontToString(lastFormat->attributeFont() ));
    if ( lastFormat->language()!= QString::null)
        element.setAttribute("language", lastFormat->language());
}

QDomElement KPTextObject::saveHelper(const QString &tmpText,KoTextFormat*lastFormat , QDomDocument &doc)
{
    QDomElement element=doc.createElement(tagTEXT);

    saveFormat ( element, lastFormat );

    if(tmpText.stripWhiteSpace().isEmpty())
        // working around a bug in QDom
        element.setAttribute(attrWhitespace, tmpText.length());
    element.appendChild(doc.createTextNode(tmpText));
    return element;
}

void KPTextObject::loadKTextObject( const QDomElement &elem )
{
    QDomElement e = elem.firstChild().toElement();
    KoTextParag *lastParag = static_cast<KoTextParag *>(textDocument()->firstParag());
    int i = 0;
    int listNum = 0;
    // Initialize lineSpacing and paragSpacing with the values of the object-level attributes
    // (KOffice-1.1 file format)
    int lineSpacing = elem.attribute( attrLineSpacing ).toInt();
    int bottomBorder = elem.attribute( attrParagSpacing ).toInt();
    int topBorder = 0;

    while ( !e.isNull() ) {
        QValueList<QDomElement> listVariable;
        listVariable.clear();

        if ( e.tagName() == tagP ) {
            QDomElement n = e.firstChild().toElement();

            //skip the whitespace if it's a bullet/number
            if( e.hasAttribute( attrType ) && n.hasAttribute( attrWhitespace ) )
                if ( e.attribute( attrType )!="0" && n.attribute( attrWhitespace )=="1" ) {
                    e = e.nextSibling().toElement();
                    continue;
                }

            KoParagLayout paragLayout = loadParagLayout(e, m_doc, true);

            // compatibility (bullet/numbering depth); only a simulation thru the margins, this is how it _looked_ before
            double depth = 0.0;
            if( e.hasAttribute(attrDepth) ) {
                depth = e.attribute( attrDepth ).toDouble();
                paragLayout.margins[QStyleSheetItem::MarginLeft] = depth * MM_TO_POINT(10.0);
            }

            //kdDebug(33001) << k_funcinfo << "old bullet depth is: " << depth  << endl;

            // 1.1 compatibility (bullets)
            QString type;
            if( e.hasAttribute(attrType) )
                type = e.attribute( attrType );

            //kdDebug(33001) << k_funcinfo << "old PARAG type is: " << type  << endl;

            // Do not import type="2" (enum list). The enum was there in 1.1, but not the code!
            if(type == "1")
            {
                if(!paragLayout.counter)
                    paragLayout.counter = new KoParagCounter;
                paragLayout.counter->setStyle(KoParagCounter::STYLE_DISCBULLET);
                paragLayout.counter->setNumbering(KoParagCounter::NUM_LIST);
                paragLayout.counter->setPrefix(QString::null);
                paragLayout.counter->setSuffix(QString::null);
            }

            // This is for very old (KOffice-1.0) documents.
            if ( e.hasAttribute( attrLineSpacing ) )
                lineSpacing = e.attribute( attrLineSpacing ).toInt();
            if ( e.hasAttribute( "distBefore" ) )
                topBorder = e.attribute( "distBefore" ).toInt();
            if ( e.hasAttribute( "distAfter" ) )
                bottomBorder = e.attribute( "distAfter" ).toInt();

            // Apply values coming from 1.0 or 1.1 documents
            if ( paragLayout.lineSpacingValue() == 0 )
                paragLayout.setLineSpacingValue(lineSpacing);
            if ( paragLayout.margins[ QStyleSheetItem::MarginTop ] == 0 )
                paragLayout.margins[ QStyleSheetItem::MarginTop ] = topBorder;
            if ( paragLayout.margins[ QStyleSheetItem::MarginBottom ] == 0 )
                paragLayout.margins[ QStyleSheetItem::MarginBottom ] = bottomBorder;
            lastParag->setParagLayout( paragLayout );
            //lastParag->setAlign(Qt::AlignAuto);

            if(e.hasAttribute(attrAlign))
            {
                int tmpAlign=e.attribute( attrAlign ).toInt();
                if(tmpAlign==1 || tmpAlign==0 /* a kpresenter version I think a cvs version saved leftAlign = 0 for header/footer */)
                    lastParag->setAlign(Qt::AlignLeft);
                else if(tmpAlign==2)
                    lastParag->setAlign(Qt::AlignRight);
                else if(tmpAlign==4)
                    lastParag->setAlign(Qt::AlignHCenter);
                else if(tmpAlign==8)
                    lastParag->setAlign(Qt::AlignJustify);
                else
                    kdDebug(33001) << "Error in e.attribute( attrAlign ).toInt()" << endl;
            }
            // ######## TODO  paragraph direction (LTR or RTL)

            // TODO check/convert values
            bool firstTextTag = true;
            while ( !n.isNull() ) {
                if ( n.tagName() == tagTEXT ) {

                    if ( firstTextTag ) {
                        lastParag->remove( 0, 1 ); // Remove current trailing space
                        firstTextTag = false;
                    }
                    KoTextFormat fm = loadFormat( n, lastParag->paragraphFormat(), m_doc->defaultFont(), m_doc->globalLanguage(),
                                                  m_doc->globalHyphenation() );

                    QString txt = n.firstChild().toText().data();

                    if(n.hasAttribute(attrWhitespace)) {
                        int ws=n.attribute(attrWhitespace).toInt();
                        txt.fill(' ', ws);
                    }
                    n=n.nextSibling().toElement();
                    if ( n.isNull() )
                        txt += ' '; // trailing space at end of paragraph
                    lastParag->append( txt, true );
                    lastParag->setFormat( i, txt.length(), textDocument()->formatCollection()->format( &fm ) );
                    //kdDebug(33001)<<"setFormat :"<<txt<<" i :"<<i<<" txt.length() "<<txt.length()<<endl;
                    i += txt.length();
                }
                else if ( n.tagName() == "CUSTOM" )
                {
                    listVariable.append( n );
                    n = n.nextSibling().toElement();
                }
                else
                    n = n.nextSibling().toElement();
            }
        } else if ( e.tagName() == "UNSORTEDLISTTYPE" ) {
            if ( listNum < 4 ) {
                //QColor c( e.attribute( "red" ).toInt(), e.attribute( "green" ).toInt(), e.attribute( "blue" ).toInt() );
                // ## settings.bulletColor[ listNum++ ] = c;
            }
        }
        e = e.nextSibling().toElement();
        loadVariable( listVariable,lastParag );
        if ( e.isNull() )
            break;
        i = 0;
        if ( !lastParag->length() == 0 )
            lastParag = new KoTextParag( textDocument(), lastParag, 0 );
    }
}

void KPTextObject::loadVariable( QValueList<QDomElement> & listVariable,KoTextParag *lastParag, int offset )
{
    QValueList<QDomElement>::Iterator it = listVariable.begin();
    QValueList<QDomElement>::Iterator end = listVariable.end();
    for ( ; it != end ; ++it )
    {
        QDomElement elem = *it;
        if ( !elem.hasAttribute("pos"))
            continue;
        int index = elem.attribute("pos").toInt();
        index+=offset;
        QDomElement varElem = elem.namedItem( "VARIABLE" ).toElement();
        if ( !varElem.isNull() )
        {
            QDomElement typeElem = varElem.namedItem( "TYPE" ).toElement();
            int type = typeElem.attribute( "type" ).toInt();
            QString key = typeElem.attribute( "key" );
            int correct = 0;
            if (typeElem.hasAttribute( "correct" ))
                correct = typeElem.attribute("correct").toInt();
            kdDebug(33001) << "loadKTextObject variable type=" << type << " key=" << key << endl;
            KoVariableFormat * varFormat = key.isEmpty() ? 0 : m_doc->variableFormatCollection()->format( key.latin1() );
            // If varFormat is 0 (no key specified), the default format will be used.
            KoVariable * var =m_doc->getVariableCollection()->createVariable( type, -1, m_doc->variableFormatCollection(),
                                                                              varFormat, lastParag->textDocument(),
                                                                              m_doc, correct, true/* force default format for date/time*/ );
            if ( var )
            {
                var->load( varElem );
                KoTextFormat format = loadFormat( *it, lastParag->paragraphFormat(), m_doc->defaultFont(), m_doc->globalLanguage(),
                                                  m_doc->globalHyphenation() );
                lastParag->setCustomItem( index, var, lastParag->document()->formatCollection()->format( &format ));
                var->recalc();
            }
        }
    }
}

KoTextFormat KPTextObject::loadFormat( QDomElement &n, KoTextFormat * refFormat, const QFont & defaultFont,
                                       const QString & defaultLanguage, bool hyphen )
{
    KoTextFormat format;
    format.setHyphenation( hyphen );
    QFont fn;
    if ( refFormat )
    {
        format = *refFormat;
        format.setCollection( 0 ); // Out of collection copy
        fn = format.font();
    }
    else
        fn = defaultFont;

    if ( !n.isNull() )
    {
        QFontDatabase fdb;
        QStringList families = fdb.families();
        if ( families.findIndex( n.attribute( attrFamily ) ) != -1 )
            fn.setFamily( n.attribute( attrFamily ) );
        else
            fn = defaultFont;
    }
    else if ( !refFormat )
    {   // No reference format and no FONT tag -> use default font
        fn = defaultFont;
    }


    int size = n.attribute( attrPointSize ).toInt();
    bool bold=false;
    if(n.hasAttribute(attrBold))
        bold = (bool)n.attribute( attrBold ).toInt();
    bool italic = false;
    if(n.hasAttribute(attrItalic))
        italic=(bool)n.attribute( attrItalic ).toInt();

    if(n.hasAttribute( attrUnderline ))
    {
        QString value = n.attribute( attrUnderline );
        if ( value == "double" )
            format.setUnderlineType ( KoTextFormat::U_DOUBLE);
        else if ( value == "single" )
            format.setUnderlineType ( KoTextFormat::U_SIMPLE);
        else if ( value == "single-bold" )
            format.setUnderlineType ( KoTextFormat::U_SIMPLE_BOLD);
        else if( value =="wave" )
            format.setUnderlineType( KoTextFormat::U_WAVE);
        else
            format.setUnderlineType ( (bool)value.toInt() ? KoTextFormat::U_SIMPLE :KoTextFormat::U_NONE);
    }
    if (n.hasAttribute("underlinestyleline") )
        format.setUnderlineStyle( KoTextFormat::stringToUnderlineStyle( n.attribute("underlinestyleline") ));

    if (n.hasAttribute("underlinecolor"))
        format.setTextUnderlineColor(QColor(n.attribute("underlinecolor")));

    if(n.hasAttribute(attrStrikeOut))
    {
        QString value = n.attribute( attrStrikeOut );
        if ( value == "double" )
            format.setStrikeOutType ( KoTextFormat::S_DOUBLE);
        else if ( value == "single" )
            format.setStrikeOutType ( KoTextFormat::S_SIMPLE);
        else if ( value == "single-bold" )
            format.setStrikeOutType ( KoTextFormat::S_SIMPLE_BOLD);
        else
            format.setStrikeOutType ( (bool)value.toInt() ? KoTextFormat::S_SIMPLE :KoTextFormat::S_NONE);
    }

    if (n.hasAttribute("strikeoutstyleline"))
    {
        QString strLineType = n.attribute("strikeoutstyleline");
        format.setStrikeOutStyle( KoTextFormat::stringToStrikeOutStyle( strLineType ));
    }

    QString color = n.attribute( attrColor );
    fn.setPointSize( size );
    fn.setBold( bold );
    fn.setItalic( italic );
    //kdDebug(33001) << "KPTextObject::loadFormat: family=" << fn.family() << " size=" << fn.pointSize() << endl;
    QColor col( color );

    format.setFont( fn );
    format.setColor( col );
    QString textBackColor=n.attribute(attrTextBackColor);
    if(!textBackColor.isEmpty())
    {
        QColor tmpCol(textBackColor);
        tmpCol=tmpCol.isValid() ? tmpCol : QApplication::palette().color( QPalette::Active, QColorGroup::Base );
        format.setTextBackgroundColor(tmpCol);
    }
    if(n.hasAttribute(attrVertAlign))
        format.setVAlign( static_cast<KoTextFormat::VerticalAlignment>(n.attribute(attrVertAlign).toInt() ) );
    if ( n.hasAttribute("text-shadow") )
        format.parseShadowFromCss( n.attribute("text-shadow") );
    if ( n.hasAttribute("relativetextsize") )
        format.setRelativeTextSize( n.attribute("relativetextsize").toDouble() ) ;
    if ( n.hasAttribute("offsetfrombaseline") )
        format.setOffsetFromBaseLine( static_cast<int>(n.attribute("offsetfrombaseline").toInt() ) );
    if ( n.hasAttribute("wordbyword") )
        format.setWordByWord( static_cast<int>(n.attribute("wordbyword").toInt() ) );

    if ( n.hasAttribute("fontattribute") )
        format.setAttributeFont( KoTextFormat::stringToAttributeFont(n.attribute("fontattribute") )  );
    if ( n.hasAttribute("language"))
        format.setLanguage( n.attribute("language"));
    else
    {   // No reference format and no language tag -> use default font
        format.setLanguage( defaultLanguage);
    }

    //kdDebug(33001)<<"loadFormat :"<<format.key()<<endl;
    return format;
}

KoParagLayout KPTextObject::loadParagLayout( QDomElement & parentElem, KPresenterDoc *doc, bool findStyle)
{
    KoParagLayout layout;

    // Only when loading paragraphs, not when loading styles
    if ( findStyle )
    {
        KoParagStyle *style;
        // Name of the style. If there is no style, then we do not supply
        // any default!
        QDomElement element = parentElem.namedItem( "NAME" ).toElement();
        if ( !element.isNull() )
        {
            QString styleName = element.attribute( "value" );
            style = doc->styleCollection()->findStyle( styleName );
            if (!style)
            {
                kdError(33001) << "Cannot find style \"" << styleName << "\" specified in paragraph LAYOUT - using Standard" << endl;
                style = doc->styleCollection()->findStyle( "Standard" );
            }
            //else kdDebug(33001) << "KoParagLayout::KoParagLayout setting style to " << style << " " << style->name() << endl;
        }
        else
        {
            kdError(33001) << "Missing NAME tag in paragraph LAYOUT - using Standard" << endl;
            style = doc->styleCollection()->findStyle( "Standard" );
        }
        Q_ASSERT(style);
        layout.style = style;
    }

    QDomElement element = parentElem.namedItem( "INDENTS" ).toElement();
    if ( !element.isNull() )
    {
        double val=0.0;
        if(element.hasAttribute("first"))
            val=element.attribute("first").toDouble();
        layout.margins[QStyleSheetItem::MarginFirstLine] = val;
        val=0.0;
        if(element.hasAttribute( "left"))
            // The GUI prevents a negative indent, so let's fixup broken docs too
            val=QMAX(0, element.attribute( "left").toDouble());
        layout.margins[QStyleSheetItem::MarginLeft] = val;
        val=0.0;
        if(element.hasAttribute("right"))
            // The GUI prevents a negative indent, so let's fixup broken docs too
            val=QMAX(0, element.attribute("right").toDouble());
        layout.margins[QStyleSheetItem::MarginRight] = val;
    }
    element = parentElem.namedItem( "LINESPACING" ).toElement();
    if ( !element.isNull() )
    {
        //compatibility with koffice 1.1
        if ( element.hasAttribute( "value" ))
        {
            QString value = element.attribute( "value" );
            if ( value == "oneandhalf" )
            {
                layout.lineSpacingType = KoParagLayout::LS_ONEANDHALF;
                layout.setLineSpacingValue(0);
            }
            else if ( value == "double" )
            {
                layout.lineSpacingType = KoParagLayout::LS_DOUBLE;
                layout.setLineSpacingValue(0);
            }
            else
            {
                layout.lineSpacingType = KoParagLayout::LS_CUSTOM;
                layout.setLineSpacingValue(value.toDouble());
            }
        }
        else
        {
            QString type = element.attribute( "type" );
            if ( type == "oneandhalf" )
            {
                layout.lineSpacingType = KoParagLayout::LS_ONEANDHALF;
                layout.setLineSpacingValue(0);
            }
            else if ( type == "double" )
            {
                layout.lineSpacingType = KoParagLayout::LS_DOUBLE;
                layout.setLineSpacingValue(0);
            }
            else if ( type == "custom" )
            {
                layout.lineSpacingType = KoParagLayout::LS_CUSTOM;
                layout.setLineSpacingValue(element.attribute( "spacingvalue" ).toDouble());
            }
            else if ( type == "atleast" )
            {
                layout.lineSpacingType = KoParagLayout::LS_AT_LEAST;
                layout.setLineSpacingValue(element.attribute( "spacingvalue" ).toDouble());
            }
            else if ( type == "multiple" )
            {
                layout.lineSpacingType = KoParagLayout::LS_MULTIPLE;
                layout.setLineSpacingValue(element.attribute( "spacingvalue" ).toDouble());
            }
        }
    }

    element = parentElem.namedItem( "OFFSETS" ).toElement();
    if ( !element.isNull() )
    {
        double val =0.0;
        if(element.hasAttribute("before"))
            val=QMAX(0, element.attribute("before").toDouble());
        layout.margins[QStyleSheetItem::MarginTop] = val;
        val = 0.0;
        if(element.hasAttribute("after"))
            val=QMAX(0, element.attribute("after").toDouble());
        layout.margins[QStyleSheetItem::MarginBottom] = val;
    }


    element = parentElem.namedItem( "LEFTBORDER" ).toElement();
    if ( !element.isNull() )
        layout.leftBorder = KoBorder::loadBorder( element );
    else
        layout.leftBorder.setPenWidth( 0);

    element = parentElem.namedItem( "RIGHTBORDER" ).toElement();
    if ( !element.isNull() )
        layout.rightBorder = KoBorder::loadBorder( element );
    else
        layout.rightBorder.setPenWidth( 0);

    element = parentElem.namedItem( "TOPBORDER" ).toElement();
    if ( !element.isNull() )
        layout.topBorder = KoBorder::loadBorder( element );
    else
        layout.topBorder.setPenWidth(0);

    element = parentElem.namedItem( "BOTTOMBORDER" ).toElement();
    if ( !element.isNull() )
        layout.bottomBorder = KoBorder::loadBorder( element );
    else
        layout.bottomBorder.setPenWidth(0);

    element = parentElem.namedItem( "COUNTER" ).toElement();
    if ( !element.isNull() )
    {
        layout.counter = new KoParagCounter;
        layout.counter->load( element );
    }

    KoTabulatorList tabList;
    element = parentElem.firstChild().toElement();
    for ( ; !element.isNull() ; element = element.nextSibling().toElement() )
    {
        if ( element.tagName() == "TABULATOR" )
        {
            KoTabulator tab;
            tab.type=T_LEFT;
            if(element.hasAttribute("type"))
                tab.type = static_cast<KoTabulators>( element.attribute("type").toInt());
            tab.ptPos=0.0;
            if(element.hasAttribute("ptpos"))
                tab.ptPos=element.attribute("ptpos").toDouble();
            tab.filling=TF_BLANK;
            if(element.hasAttribute("filling"))
                tab.filling = static_cast<KoTabulatorFilling>( element.attribute("filling").toInt());
            tab.ptWidth=0.5;
            if(element.hasAttribute("width"))
                tab.ptWidth=element.attribute("width").toDouble();
            tabList.append( tab );
        }
    }
    layout.setTabList( tabList );


    return layout;
}

void KPTextObject::saveParagLayout( const KoParagLayout& layout, QDomElement & parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement element = doc.createElement( "NAME" );
    parentElem.appendChild( element );
    if ( layout.style )
        element.setAttribute( "value", layout.style->name() );
    else
        kdWarning() << "KWTextParag::saveParagLayout: style==0L!" << endl;


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


    if ( layout.margins[QStyleSheetItem::MarginTop] != 0 ||
         layout.margins[QStyleSheetItem::MarginBottom] != 0 )
    {
        element = doc.createElement( "OFFSETS" );
        parentElem.appendChild( element );
        if ( layout.margins[QStyleSheetItem::MarginTop] != 0 )
            element.setAttribute( "before", layout.margins[QStyleSheetItem::MarginTop] );
        if ( layout.margins[QStyleSheetItem::MarginBottom] != 0 )
            element.setAttribute( "after", layout.margins[QStyleSheetItem::MarginBottom] );
    }

    if ( layout.lineSpacingType != KoParagLayout::LS_SINGLE )
    {
        element = doc.createElement( "LINESPACING" );
        parentElem.appendChild( element );
        if ( layout.lineSpacingType == KoParagLayout::LS_ONEANDHALF )
            element.setAttribute( "type", "oneandhalf" );
        else if ( layout.lineSpacingType == KoParagLayout::LS_DOUBLE )
            element.setAttribute( "type", "double" );
        else if ( layout.lineSpacingType == KoParagLayout::LS_CUSTOM )
        {
            element.setAttribute( "type", "custom" );
            element.setAttribute( "spacingvalue", layout.lineSpacingValue());
        }
        else if ( layout.lineSpacingType == KoParagLayout::LS_AT_LEAST )
        {
            element.setAttribute( "type", "atleast" );
            element.setAttribute( "spacingvalue", layout.lineSpacingValue());
        }
        else if ( layout.lineSpacingType == KoParagLayout::LS_MULTIPLE )
        {
            element.setAttribute( "type", "multiple" );
            element.setAttribute( "spacingvalue", layout.lineSpacingValue());
        }
        else
            kdDebug(33001) << " error in lineSpacing Type" << endl;
    }

    if ( layout.leftBorder.penWidth() > 0 )
    {
        element = doc.createElement( "LEFTBORDER" );
        parentElem.appendChild( element );
        layout.leftBorder.save( element );
    }
    if ( layout.rightBorder.penWidth() > 0 )
    {
        element = doc.createElement( "RIGHTBORDER" );
        parentElem.appendChild( element );
        layout.rightBorder.save( element );
    }
    if ( layout.topBorder.penWidth() > 0 )
    {
        element = doc.createElement( "TOPBORDER" );
        parentElem.appendChild( element );
        layout.topBorder.save( element );
    }
    if ( layout.bottomBorder.penWidth() > 0 )
    {
        element = doc.createElement( "BOTTOMBORDER" );
        parentElem.appendChild( element );
        layout.bottomBorder.save( element );
    }

    if ( layout.counter && layout.counter->numbering() != KoParagCounter::NUM_NONE )
    {
        element = doc.createElement( "COUNTER" );
        parentElem.appendChild( element );
        if (layout.counter )
            layout.counter->save( element );
    }

    KoTabulatorList tabList = layout.tabList();
    KoTabulatorList::ConstIterator it = tabList.begin();
    for ( ; it != tabList.end() ; it++ )
    {
        element = doc.createElement( "TABULATOR" );
        parentElem.appendChild( element );
        element.setAttribute( "type", (*it).type );
        element.setAttribute( "ptpos", (*it).ptPos );
        element.setAttribute( "filling", (*it).filling );
        element.setAttribute( "width", (*it).ptWidth );
    }
}

void KPTextObject::recalcPageNum( KPrPage *page )
{
    int pgnum=m_doc->pageList().findRef(page);

    if ( (pgnum==-1 && isSticky()) || m_doc->hasHeader() || m_doc->hasFooter() )
    {
        if ( m_doc->activePage())
        {
            page = m_doc->activePage();
            pgnum=m_doc->pageList().findRef(page);
        }
    }

    pgnum+=1;
    QPtrListIterator<KoTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
        KPrPgNumVariable * var = dynamic_cast<KPrPgNumVariable *>( cit.current() );
        if ( var && !var->isDeleted()  )
        {
            switch ( var->subType() ) {
            case KPrPgNumVariable::VST_PGNUM_CURRENT:
                var->setPgNum( pgnum + kPresenterDocument()->getVariableCollection()->variableSetting()->startingPage()-1);
                break;
            case KPrPgNumVariable::VST_CURRENT_SECTION:
                var->setSectionTitle( page->pageTitle("") );
                break;
            case KPrPgNumVariable::VST_PGNUM_PREVIOUS:
                var->setPgNum( QMAX( pgnum -1 , 0) +
                               kPresenterDocument()->getVariableCollection()->variableSetting()->startingPage());
                break;
            case KPrPgNumVariable::VST_PGNUM_NEXT:
                var->setPgNum( QMIN( (int)m_doc->getPageNums(), pgnum+1 ) +
                               kPresenterDocument()->getVariableCollection()->variableSetting()->startingPage());
                break;
            default:
                break;
            }

            var->resize();
            var->paragraph()->invalidate( 0 ); // size may have changed -> need reformatting !
            var->paragraph()->setChanged( true );
        }
    }
}

void KPTextObject::layout()
{
    invalidate();
    // Get the thing going though, repainting doesn't call formatMore
    m_textobj->formatMore( 2 );
}

void KPTextObject::invalidate()
{
    //kdDebug(33001) << "KWTextFrameSet::invalidate " << getName() << endl;
    m_textobj->setLastFormattedParag( textDocument()->firstParag() );
    textDocument()->formatter()->setViewFormattingChars( m_doc->viewFormattingChars() );
    textDocument()->invalidate(); // lazy layout, real update follows upon next repaint
}

// For the "paragraph after paragraph" effect
void KPTextObject::drawParags( QPainter *painter, KoZoomHandler* zoomHandler, const QColorGroup& cg, int from, int to )
{
    // The fast and difficult way would be to call drawParagWYSIWYG
    // only on the paragraphs to be drawn. Then we have duplicate quite some code
    // (or lose double-buffering).
    // Easy (and not so slow) way:
    // we call KoTextDocument::drawWYSIWYG with a cliprect.
    Q_ASSERT( from <= to );
    int i = 0;
    bool editMode=false;
    if( m_doc->firstView() && m_doc->firstView()->getCanvas())
        editMode = m_doc->firstView()->getCanvas()->getEditMode();

    QRect r = zoomHandler->zoomRect( KoRect( 0, 0, innerWidth(), innerHeight() ) );
    KoTextParag *parag = textDocument()->firstParag();
    while ( parag ) {
        if ( !parag->isValid() )
            parag->format();
        if ( i == from )
            r.setTop( m_doc->zoomHandler()->layoutUnitToPixelY( parag->rect().top() ) );
        if ( i == to ) {
            r.setBottom( m_doc->zoomHandler()->layoutUnitToPixelY( parag->rect().bottom() ) );
            break;
        }
        ++i;
        parag = parag->next();
    }

    uint drawingFlags = 0; // don't draw selections
    if ( m_doc->backgroundSpellCheckEnabled() && editMode )
        drawingFlags |= KoTextDocument::DrawMisspelledLine;
    textDocument()->drawWYSIWYG(
        painter, r.x(), r.y(), r.width(), r.height(),
        cg, m_doc->zoomHandler(), // TODO (long term) the view's zoomHandler
        false /*onlyChanged*/, false /*cursor != 0*/, 0 /*cursor*/,
        true /*resetChanged*/, drawingFlags );
}

void KPTextObject::drawCursor( QPainter *p, KoTextCursor *cursor, bool cursorVisible, KPrCanvas* canvas )
{
    // The implementation is very related to KWord's KWTextFrameSet::drawCursor
    KoZoomHandler *zh = m_doc->zoomHandler();
    QPoint origPix = zh->zoomPoint( orig+KoPoint(bLeft(), bTop()+alignVertical) );
    // Painter is already translated for diffx/diffy, but not for the object yet
    p->translate( origPix.x(), origPix.y() );
    if ( angle != 0 )
        rotateObject( p, zh );

    KoTextParag* parag = cursor->parag();
    QPoint topLeft = parag->rect().topLeft();         // in QRT coords
    int lineY;
    // Cursor height, in pixels
    int cursorHeight = zh->layoutUnitToPixelY( topLeft.y(), parag->lineHeightOfChar( cursor->index(), 0, &lineY ) );
    QPoint iPoint( topLeft.x() + cursor->x(),
                   topLeft.y() + lineY );
    // from now on, iPoint will be in pixels
    iPoint = zh->layoutUnitToPixel( iPoint );

    QPoint vPoint = iPoint; // vPoint and iPoint are the same currently
                            // do not simplify this, will be useful with viewmodes.
    //int xadj = parag->at( cursor->index() )->pixelxadj;
    //iPoint.rx() += xadj;
    //vPoint.rx() += xadj;
    // very small clipping around the cursor
    QRect clip( vPoint.x() - 5, vPoint.y() , 10, cursorHeight );
    setupClipRegion( p, clip );

    // for debug only!
    //p->fillRect( clip, Qt::blue );

    QPixmap *pix = 0;
    QColorGroup cg = QApplication::palette().active();
    cg.setColor( QColorGroup::Base, m_doc->txtBackCol() );

    uint drawingFlags = KoTextDocument::DrawSelections;
    if ( m_doc->backgroundSpellCheckEnabled() )
        drawingFlags |= KoTextDocument::DrawMisspelledLine;
    if ( m_doc->viewFormattingChars() )
        drawingFlags |= KoTextDocument::DrawFormattingChars;

    // To force the drawing to happen:
    bool wasChanged = parag->hasChanged();
    int oldLineChanged = parag->lineChanged();
    int line; // line number
    parag->lineStartOfChar( cursor->index(), 0, &line );
    parag->setChanged( false ); // not all changed, only from a given line
    parag->setLineChanged( line );
    //kdDebug(33001) << "KPTextObject::drawCursor cursorVisible=" << cursorVisible << " line=" << line << endl;

    textDocument()->drawParagWYSIWYG(
        p, parag,
        QMAX(0, iPoint.x() - 5), // negative values create problems
        iPoint.y(), clip.width(), clip.height(),
        pix, cg, m_doc->zoomHandler(),
        cursorVisible, cursor, FALSE /*resetChanged*/, drawingFlags );

    if ( wasChanged )      // Maybe we have more changes to draw, than those in the small cliprect
        cursor->parag()->setLineChanged( oldLineChanged ); // -1 = all
    else
        cursor->parag()->setChanged( false );

    // XIM Position
    QPoint ximPoint = vPoint;
    QFont f = parag->at( cursor->index() )->format()->font();
    canvas->setXimPosition( ximPoint.x() + origPix.x(), ximPoint.y() + origPix.y(),
                            0, cursorHeight - parag->lineSpacing( line ), &f );
}

KPrTextDocument * KPTextObject::textDocument() const
{
    return static_cast<KPrTextDocument*>(m_textobj->textDocument());
}

void KPTextObject::slotNewCommand( KCommand * cmd)
{
    m_doc->addCommand(cmd);
}

int KPTextObject::availableHeight() const
{
    return m_textobj->availableHeight();
}

void KPTextObject::slotAvailableHeightNeeded()
{
    int ah = m_doc->zoomHandler()->ptToLayoutUnitPixY( innerHeight() );
    m_textobj->setAvailableHeight( ah );
    //kdDebug(33001)<<"slotAvailableHeightNeeded: height=:"<<ah<<endl;
}

void KPTextObject::slotRepaintChanged()
{
    emit repaintChanged( this );
}

KPTextView * KPTextObject::createKPTextView( KPrCanvas * _canvas )
{
    return new KPTextView( this, _canvas );
}

void KPTextObject::removeHighlight ()
{
    m_textobj->removeHighlight( true /*repaint*/ );
}

void KPTextObject::highlightPortion( KoTextParag * parag, int index, int length, KPrCanvas* canvas, bool repaint, KDialogBase* dialog )
{
    m_textobj->highlightPortion( parag, index, length, repaint );
    if ( repaint )
    {
        KPresenterDoc* doc = canvas->getView()->kPresenterDoc();

        // Is this object in the current active page?
        if ( canvas->activePage()->findTextObject( this ) ||
             (isSticky() && doc->stickyPage()->findTextObject( this ) ) )
        {
            kdDebug(33001) << k_funcinfo << "object in current page" << endl;
        }
        else
        {
            // No -> find the right page and activate it
            // ** slow method **
            KPrPage* page = doc->findSideBarPage( this );
            if ( page ) {
                int pageNum = doc->pageList().findRef( page );
                Q_ASSERT( pageNum > -1 );
                canvas->getView()->skipToPage( pageNum );
            } else
                kdWarning(33001) << "object " << this << " not found in any page!?" << endl;
        }
        // Now ensure text is fully visible
        QRect rect = m_doc->zoomHandler()->zoomRect( getRect() );
        QRect expose = m_doc->zoomHandler()->layoutUnitToPixel( parag->rect() );
        expose.moveBy( rect.x(), rect.y() );
        canvas->ensureVisible( (expose.left()+expose.right()) / 2,  // point = center of the rect
                               (expose.top()+expose.bottom()) / 2,
                               (expose.right()-expose.left()) / 2,  // margin = half-width of the rect
                               (expose.bottom()-expose.top()) / 2);
#if KDE_IS_VERSION(3,1,90)
        if ( dialog ) {
            QRect globalRect( expose );
            globalRect.moveTopLeft( canvas->mapToGlobal( globalRect.topLeft() ) );
            KDialog::avoidArea( dialog, globalRect );
        }
#endif
    }
}

KCommand * KPTextObject::pasteOasis( KoTextCursor * cursor, const QCString & data, bool removeSelected )
{
    // Having data as a QCString instead of a QByteArray seems to fix the trailing 0 problem
    // I tried using QDomDocument::setContent( QByteArray ) but that leads to parse error at the end

    //kdDebug(33001) << "KWTextFrameSet::pasteKPresenter" << endl;
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Paste Text") );
    if ( removeSelected && textDocument()->hasSelection( KoTextDocument::Standard ) )
        macroCmd->addCommand( m_textobj->removeSelectedTextCommand( cursor, KoTextDocument::Standard ) );
    m_textobj->emitHideCursor();
    m_textobj->setLastFormattedParag( cursor->parag()->prev() ?
                                      cursor->parag()->prev() : cursor->parag() );

    // We have our own command for this.
    // Using insert() wouldn't help storing the parag stuff for redo
    KPrOasisPasteTextCommand * cmd = new KPrOasisPasteTextCommand( textDocument(), cursor->parag()->paragId(), cursor->index(), data );
    textDocument()->addCommand( cmd );

    macroCmd->addCommand( new KoTextCommand( m_textobj, /*cmd, */QString::null ) );
    *cursor = *( cmd->execute( cursor ) );

    m_textobj->formatMore( 2 );
    emit repaintChanged( this );
    m_textobj->emitEnsureCursorVisible();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitShowCursor();
    m_textobj->selectionChangedNotify();
    return macroCmd;
}


void KPTextObject::setShadowParameter(int _distance,ShadowDirection _direction,const QColor &_color)
{
    int sx = 0;
    int sy = 0;
    switch ( _direction )
    {
    case SD_LEFT_BOTTOM:
    case SD_LEFT:
    case SD_LEFT_UP:
        sx = - _distance;
    case SD_RIGHT_UP:
    case SD_RIGHT:
    case SD_RIGHT_BOTTOM:
        sx = _distance;
    default:
        break;
    }
    switch ( _direction )
    {
    case SD_LEFT_UP:
    case SD_UP:
    case SD_RIGHT_UP:
        sy = - _distance;
    case SD_LEFT_BOTTOM:
    case SD_BOTTOM:
    case SD_RIGHT_BOTTOM:
        sy = _distance;
    default:
        break;
    }
    KoTextFormat tmpFormat;
    tmpFormat.setShadow( sx, sy, _color );
    KCommand* cmd = m_textobj->setFormatCommand( &tmpFormat, KoTextFormat::ShadowText );
    if ( cmd )
        m_doc->addCommand(cmd);
}

void KPTextObject::slotFormatChanged(const KoTextFormat &_format)
{
    if(m_doc && m_doc->firstView())
        m_doc->firstView()->showFormat( _format );
}

void KPTextObject::applyStyleChange( KoStyleChangeDefMap changed )
{
    m_textobj->applyStyleChange( changed );
}

void KPTextObject::slotAfterFormatting( int bottom, KoTextParag* lastFormatted, bool* abort)
{
    recalcVerticalAlignment();
    int availHeight = availableHeight() - m_doc->zoomHandler()->ptToLayoutUnitPixY(alignmentValue());
    if ( ( bottom > availHeight ) ||   // this parag is already below the avail height
         ( lastFormatted && (bottom + lastFormatted->rect().height() > availHeight) ) ) // or next parag will be below it
    {
        int difference = ( bottom + 2 ) - availHeight; // in layout unit pixels
        if( lastFormatted && bottom + lastFormatted->rect().height() > availHeight )
        {
            difference += lastFormatted->rect().height();
        }
#if 0
        if(lastFormatted)
            kdDebug(33001) << "slotAfterFormatting We need more space in " << this
                           << " bottom=" << bottom + lastFormatted->rect().height()
                           << " availHeight=" << availHeight
                           << " ->difference=" << difference << endl;
        else
            kdDebug(33001) << "slotAfterFormatting We need more space in " << this
                           << " bottom2=" << bottom << " availHeight=" << availHeight
                           << " ->difference=" << difference << endl;
#endif
        // We only auto-grow. We don't auto-shrink.
        if(difference > 0 && !isProtect())
        {
            double wantedPosition = m_doc->zoomHandler()->layoutUnitPtToPt( m_doc->zoomHandler()->pixelYToPt( difference ) )
                                    + getRect().bottom();
            const KoPageLayout& p = m_doc->pageLayout();
            double pageBottom = p.ptHeight - p.ptBottom;
            double newBottom = QMIN( wantedPosition, pageBottom ); // don't grow bigger than the page
            newBottom = QMAX( newBottom, getOrig().y() ); // avoid negative heights
            //kdDebug(33001) << k_funcinfo << " current bottom=" << getRect().bottom() << " newBottom=" << newBottom << endl;
            if ( getRect().bottom() != newBottom )
            {
                // We resize the text object, but skipping the KPTextObject::setSize code
                // (which invalidates everything etc.)
                KPObject::setSize( getSize().width(), newBottom - getOrig().y() );
                // Do recalculate the new available height though
                slotAvailableHeightNeeded();
                m_doc->updateRuler();
                m_doc->repaint( true );
                *abort = false;
            }
        }
        else if ( isProtect() )
            m_textobj->setLastFormattedParag( 0 );
    }
}

// "Extend Contents to Object Height"
KCommand * KPTextObject::textContentsToHeight()
{
    if (isProtect() )
        return 0L;

    // Count total number of lines and sum up their height (linespacing excluded)
    KoTextParag * parag = textDocument()->firstParag();
    int numLines = 0;
    int textHeightLU = 0;
    bool lineSpacingEqual = false;
    int oldLineSpacing = 0;
    for ( ; parag ; parag = parag->next() )
    {
        int lines = parag->lines();
        numLines += lines;
        for ( int line = 0 ; line < lines ; ++line )
        {
            int y, h, baseLine;
            parag->lineInfo( line, y, h, baseLine );
            int ls = parag->lineSpacing( line );
            lineSpacingEqual = (oldLineSpacing == ls);
            oldLineSpacing = ls;
            textHeightLU += h - ls;
        }
    }

    double textHeight = m_doc->zoomHandler()->layoutUnitPtToPt( textHeightLU );
    double lineSpacing = ( innerHeight() - textHeight ) /  numLines; // this gives the linespacing diff to apply, in pt
    //kdDebug(33001) << k_funcinfo << "lineSpacing=" << lineSpacing << endl;

    if ( KABS( innerHeight() - textHeight ) < DBL_EPSILON ) // floating-point equality test
        return 0L; // nothing to do
    bool oneLine =(textDocument()->firstParag() == textDocument()->lastParag() && numLines == 1);
    if ( lineSpacing < 0  || oneLine) // text object is too small
        lineSpacing = 0; // we can't do smaller linespacing than that, but we do need to apply it
                         // (in case there's some bigger linespacing in use)
    if ( (oneLine || lineSpacingEqual) && (textDocument()->firstParag()->kwLineSpacing() == lineSpacing))
        return 0L;
    // Apply the new linespacing to the whole object
    textDocument()->selectAll( KoTextDocument::Temp );
    KCommand* cmd = m_textobj->setLineSpacingCommand( 0L, lineSpacing, KoParagLayout::LS_CUSTOM, KoTextDocument::Temp );
    textDocument()->removeSelection( KoTextDocument::Temp );
    return cmd;
}

// "Resize Object to fit Contents"
KCommand * KPTextObject::textObjectToContents()
{
    if ( isProtect() )
        return 0L;
    // Calculate max parag width (in case all parags are short, otherwise - with wrapping -
    // the width is more or less the current object's width anyway).
    KoTextParag * parag = textDocument()->firstParag();
    double txtWidth = 10;
    for ( ; parag ; parag = parag->next() )
        txtWidth = QMAX( txtWidth, m_doc->zoomHandler()->layoutUnitPtToPt( parag->widthUsed() ));

    // Calculate text height
    int heightLU = textDocument()->height();
    double txtHeight = m_doc->zoomHandler()->layoutUnitPtToPt( heightLU );

    // Compare with current object's size
    KoSize sizeDiff = KoSize( txtWidth, txtHeight ) - innerRect().size();
    if( !sizeDiff.isNull() )
    {
        // The command isn't named since it's always put into a macro command.
        return new ResizeCmd( QString::null, KoPoint( 0, 0 ), sizeDiff, this, m_doc);
    }
    return 0L;
}

void KPTextObject::setTextMargins( double _left, double _top, double _right, double _bottom)
{
    bleft = _left;
    btop = _top;
    bright = _right;
    bbottom = _bottom;
}

KoRect KPTextObject::innerRect() const
{
    KoRect inner( getRect());
    inner.moveBy( bLeft(), bTop());
    inner.setWidth( inner.width() - bLeft() - bRight() );
    inner.setHeight( inner.height() - bTop() - bBottom() );
    return inner;
}

double KPTextObject::innerWidth() const
{
    return getSize().width() - bLeft() - bRight();
}

double KPTextObject::innerHeight() const
{
    return getSize().height() - bTop() - bBottom();
}

void KPTextObject::setVerticalAligment( VerticalAlignmentType _type)
{
    m_textVertAlign = _type;
    recalcVerticalAlignment();
}

void KPTextObject::recalcVerticalAlignment()
{
    double txtHeight = m_doc->zoomHandler()->layoutUnitPtToPt( m_doc->zoomHandler()->pixelYToPt( textDocument()->height() ) ) + btop + bbottom;
    double diffy = getSize().height() - txtHeight;

    //kdDebug(33001) << k_funcinfo << "txtHeight: " << txtHeight << " rectHeight:" << getSize().height() << " -> diffy=" << diffy << endl;

    if ( diffy <= 0.0 ) {
        alignVertical = 0.0;
        return;
    }
    switch( m_textVertAlign )
    {
    case KP_CENTER:
        alignVertical = diffy/2.0;
        break;
    case KP_TOP:
        alignVertical = 0.0;
        break;
    case KP_BOTTOM:
        alignVertical = diffy;
        break;
    }
}

KPTextView::KPTextView( KPTextObject * txtObj, KPrCanvas *_canvas )
    : KoTextView( txtObj->textObject() )
{
    m_canvas=_canvas;
    m_kptextobj=txtObj;
    connect( txtObj->textObject(), SIGNAL( selectionChanged(bool) ),
             m_canvas, SIGNAL( selectionChanged(bool) ) );
    KoTextView::setReadWrite( txtObj->kPresenterDocument()->isReadWrite() );
    connect( textView(), SIGNAL( cut() ), SLOT( cut() ) );
    connect( textView(), SIGNAL( copy() ), SLOT( copy() ) );
    connect( textView(), SIGNAL( paste() ), SLOT( paste() ) );
    updateUI( true, true );

    txtObj->setEditingTextObj( true );
}

KPTextView::~KPTextView()
{
}

KoTextViewIface* KPTextView::dcopObject()
{
    if ( !dcop )
        dcop = new KPTextViewIface( this );

    return dcop;
}

void KPTextView::terminate(bool removeSelection)
{
    disconnect( textView()->textObject(), SIGNAL( selectionChanged(bool) ),
                m_canvas, SIGNAL( selectionChanged(bool) ) );
    textView()->terminate(removeSelection);
}

void KPTextView::cut()
{
    if ( textDocument()->hasSelection( KoTextDocument::Standard ) ) {
        copy();
        textObject()->removeSelectedText( cursor() );
    }
}

void KPTextView::copy()
{
    //kdDebug(33001)<<"void KPTextView::copy() "<<endl;
    if ( textDocument()->hasSelection( KoTextDocument::Standard ) ) {
        KPrTextDrag *kd = newDrag( 0L );
        QApplication::clipboard()->setData( kd );
    }
}

void KPTextView::paste()
{
    //kdDebug(33001) << "KPTextView::paste()" << endl;

    QMimeSource *data = QApplication::clipboard()->data();
    QString returnMimeType;
    if ( KPrTextDrag::provides( data , KPrTextDrag::selectionMimeType(), KPrTextDrag::acceptSelectionMimeType(), returnMimeType) )
    {
        QByteArray arr = data->encodedData( returnMimeType.latin1() );
        if ( arr.size() )
        {
            kdDebug(33001)<<"QCString( arr ) :"<<QCString( arr.data(), arr.size()+1 )<<endl;
            KCommand *cmd = kpTextObject()->pasteOasis( cursor(), QCString( arr.data(), arr.count()+1 ), true );
            if ( cmd )
                kpTextObject()->kPresenterDocument()->addCommand(cmd);
        }
    }
    else
    {
        // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
        // In particular it handles charsets (in the mimetype).
        QString text = QApplication::clipboard()->text();
        if ( !text.isEmpty() )
            textObject()->pasteText( cursor(), text, currentFormat(), true );
    }
    kpTextObject()->layout();
}

void KPTextView::updateUI( bool updateFormat, bool force  )
{
    KoTextView::updateUI( updateFormat, force  );
    // Paragraph settings
    KoTextParag * parag = static_cast<KoTextParag*>( cursor()->parag());
    if ( m_paragLayout.alignment != parag->resolveAlignment() || force ) {
        m_paragLayout.alignment = parag->resolveAlignment();
        m_canvas->getView()->alignChanged(  m_paragLayout.alignment );
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
        m_canvas->getView()->showCounter( * m_paragLayout.counter );

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
        kdWarning(33001) << "Paragraph " << parag->paragId() << " has no style" << endl;
    else if ( m_paragLayout.style != parag->style() || force )
    {
        m_paragLayout.style = parag->style();
        m_canvas->getView()->showStyle( m_paragLayout.style->name() );
    }

    if( m_paragLayout.margins[QStyleSheetItem::MarginLeft] != parag->margin(QStyleSheetItem::MarginLeft)
        || m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] != parag->margin(QStyleSheetItem::MarginFirstLine)
        || m_paragLayout.margins[QStyleSheetItem::MarginRight] != parag->margin(QStyleSheetItem::MarginRight)
        || force )
    {
        m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] = parag->margin(QStyleSheetItem::MarginFirstLine);
        m_paragLayout.margins[QStyleSheetItem::MarginLeft] = parag->margin(QStyleSheetItem::MarginLeft);
        m_paragLayout.margins[QStyleSheetItem::MarginRight] = parag->margin(QStyleSheetItem::MarginRight);
        m_canvas->getView()->showRulerIndent( m_paragLayout.margins[QStyleSheetItem::MarginLeft],
                                              m_paragLayout.margins[QStyleSheetItem::MarginFirstLine],
                                              m_paragLayout.margins[QStyleSheetItem::MarginRight],
                                              parag->string()->isRightToLeft() );
    }

    if( m_paragLayout.tabList() != parag->tabList() || force )
    {
        m_paragLayout.setTabList( parag->tabList() );
        KoRuler * hr = m_canvas->getView()->getHRuler();
        if ( hr )
            hr->setTabList( parag->tabList() );
    }
}

void KPTextView::ensureCursorVisible()
{
    //kdDebug(33001) << "KWTextFrameSetEdit::ensureCursorVisible paragId=" << cursor()->parag()->paragId() << endl;
    KoTextParag * parag = cursor()->parag();
    kpTextObject()->textObject()->ensureFormatted( parag );
    KoTextStringChar *chr = parag->at( cursor()->index() );
    int h = parag->lineHeightOfChar( cursor()->index() );
    int x = parag->rect().x() + chr->x;
    int y = 0; int dummy;

    parag->lineHeightOfChar( cursor()->index(), &dummy, &y );
    y += parag->rect().y();
    int w = 1;
    KPresenterDoc *doc= m_kptextobj->kPresenterDocument();
    KoPoint pt= kpTextObject()->getOrig();
    pt.setX( doc->zoomHandler()->layoutUnitPtToPt( doc->zoomHandler()->pixelXToPt( x) ) +pt.x());
    pt.setY( doc->zoomHandler()->layoutUnitPtToPt( doc->zoomHandler()->pixelYToPt( y ))+pt.y() );

    QPoint p = m_kptextobj->kPresenterDocument()->zoomHandler()->zoomPoint( pt );
    w = m_kptextobj->kPresenterDocument()->zoomHandler()->layoutUnitToPixelX( w );
    h = m_kptextobj->kPresenterDocument()->zoomHandler()->layoutUnitToPixelY( h );
    m_canvas->ensureVisible( p.x(), p.y() + h / 2, w, h / 2 + 2 );
}

void KPTextView::doCompletion( KoTextCursor* cursor, KoTextParag *parag, int index )
{
    if( m_kptextobj->kPresenterDocument()->allowAutoFormat() )
    {
        KoAutoFormat * autoFormat = m_kptextobj->kPresenterDocument()->getAutoFormat();
        if( autoFormat )
            autoFormat->doCompletion(  cursor, parag, index, textObject());
    }
}


void KPTextView::doAutoFormat( KoTextCursor* cursor, KoTextParag *parag, int index, QChar ch )
{
    if( m_kptextobj->kPresenterDocument()->allowAutoFormat())
    {
        KoAutoFormat * autoFormat = m_kptextobj->kPresenterDocument()->getAutoFormat();
        if ( autoFormat )
            autoFormat->doAutoFormat( cursor, parag, index, ch, textObject());
    }
}

bool KPTextView::doIgnoreDoubleSpace(KoTextParag * parag, int index,QChar ch )
{
    if( m_kptextobj->kPresenterDocument()->allowAutoFormat())
    {

        KoAutoFormat * autoFormat = m_kptextobj->kPresenterDocument()->getAutoFormat();
        if( autoFormat )
            return autoFormat->doIgnoreDoubleSpace( parag, index,ch );
    }
    return false;
}

void KPTextView::startDrag()
{
    textView()->dragStarted();
    m_canvas->dragStarted();
    KPrTextDrag *drag = newDrag( m_canvas );
    if ( !kpTextObject()->kPresenterDocument()->isReadWrite() )
        drag->dragCopy();
    else
    {
        if ( drag->drag() && QDragObject::target() != m_canvas  )
            textObject()->removeSelectedText( cursor() );
    }
}

void KPTextView::showFormat( KoTextFormat *format )
{
    m_canvas->getView()->showFormat( *format );
}

bool KPTextView::pgUpKeyPressed()
{
    KoTextCursor *cursor = textView()->cursor();
    KoTextParag *s = cursor->parag();
    s = textDocument()->firstParag();

    textView()->cursor()->setParag( s );
    textView()->cursor()->setIndex( 0 );
    return true;
}

bool KPTextView::pgDownKeyPressed()
{
    KoTextCursor *cursor = textView()->cursor();
    KoTextParag *s = cursor->parag();
    s = textDocument()->lastParag();
    cursor->setParag( s );
    cursor->setIndex( s->length() - 1 );
    return true;
}

void KPTextView::keyPressEvent( QKeyEvent *e )
{
    handleKeyPressEvent(e);
}

void KPTextView::keyReleaseEvent( QKeyEvent *e )
{
    handleKeyReleaseEvent(e);
}

void KPTextView::clearSelection()
{
    if ( textDocument()->hasSelection( KoTextDocument::Standard ) )
        textDocument()->removeSelection(KoTextDocument::Standard );
}

void KPTextView::selectAll(bool select)
{
    textObject()->selectAll( select );
}

void KPTextView::drawCursor( bool b )
{
    KoTextView::drawCursor( b );
    if ( !cursor()->parag() )
        return;
    if ( !kpTextObject()->kPresenterDocument()->isReadWrite() )
        return;

    QPainter painter( m_canvas );
    painter.translate( -m_canvas->diffx(), -m_canvas->diffy() );
    painter.setBrushOrigin( -m_canvas->diffx(), -m_canvas->diffy() );

    kpTextObject()->drawCursor( &painter, cursor(), b, m_canvas );
}

QPoint KPTextView::cursorPosition( const QPoint & pos )
{
#if 0
    QPoint tmp(pos);
    QWMatrix m;
    m.translate( kpTextObject()->kPresenterDocument()->zoomHandler()->zoomItX(kpTextObject()->getSize().width() / 2.0),
                 kpTextObject()->kPresenterDocument()->zoomHandler()->zoomItY(kpTextObject()->getSize().height() /  2.0) );
    m.rotate( kpTextObject()->getAngle() );



    m.translate( kpTextObject()->kPresenterDocument()->zoomHandler()->zoomItX(kpTextObject()->getOrig().x()),
                 kpTextObject()->kPresenterDocument()->zoomHandler()->zoomItY(kpTextObject()->getOrig().y()) );
    //m = m.invert();
    tmp = m * pos;

    kdDebug(33001)<<" tmp.x() :"<<tmp.x()<<" tmp.y() "<<tmp.y()<<endl;

    KoRect br = KoRect( 0, 0, kpTextObject()->getSize().width(), kpTextObject()->getSize().height() );
    double pw = br.width();
    double ph = br.height();
    KoRect rr = br;
    double yPos = -rr.y();
    double xPos = -rr.x();
    rr.moveTopLeft( KoPoint( -rr.width() / 2.0, -rr.height() / 2.0 ) );

    m.translate( kpTextObject()->kPresenterDocument()->zoomHandler()->zoomItX(pw / 2.0),
                 kpTextObject()->kPresenterDocument()->zoomHandler()->zoomItY(ph / 2.0 ));
    m.rotate( kpTextObject()->getAngle() );
    m.translate( kpTextObject()->kPresenterDocument()->zoomHandler()->zoomItX(rr.left() + xPos),
                 kpTextObject()->kPresenterDocument()->zoomHandler()->zoomItY(rr.top() + yPos) );

    m = m.invert();

    tmp = m * pos;

    kdDebug(33001)<<" tmp.x() :"<<tmp.x()<<" tmp.y() "<<tmp.y()<<endl;
#endif

    QPoint iPoint=pos - kpTextObject()->kPresenterDocument()->zoomHandler()->zoomPoint(
        kpTextObject()->getOrig()+KoPoint( kpTextObject()->bLeft(),
                                           kpTextObject()->bTop()+kpTextObject()->alignmentValue()) );
    iPoint=kpTextObject()->kPresenterDocument()->zoomHandler()->pixelToLayoutUnit(
        QPoint(iPoint.x()+ m_canvas->diffx(),iPoint.y()+m_canvas->diffy()) );
    return iPoint;
}

void KPTextView::mousePressEvent( QMouseEvent *e, const QPoint &/*_pos*/)
{
    bool addParag = handleMousePressEvent( e, cursorPosition( e->pos() ),true /*bool canStartDrag*/,
                                           kpTextObject()->kPresenterDocument()->insertDirectCursor() );
    if ( addParag )
        kpTextObject()->kPresenterDocument()->setModified( true );
}

void KPTextView::mouseDoubleClickEvent( QMouseEvent *e, const QPoint &pos)
{
    handleMouseDoubleClickEvent( e, pos  );
}

void KPTextView::mouseMoveEvent( QMouseEvent *e, const QPoint &_pos )
{
    if ( textView()->maybeStartDrag( e ) )
        return;
    if ( _pos.y() > 0  )
        textView()->handleMouseMoveEvent( e,cursorPosition( e->pos() ) );
}

void KPTextView::mouseReleaseEvent( QMouseEvent *, const QPoint & )
{
    handleMouseReleaseEvent();
}

void KPTextView::showPopup( KPresenterView *view, const QPoint &point, QPtrList<KAction>& actionList )
{
    QString word = wordUnderCursor( *cursor() );
    view->unplugActionList( "datatools" );
    view->unplugActionList( "datatools_link" );
    view->unplugActionList( "spell_result_action" );
    view->unplugActionList( "variable_action" );
    QPtrList<KAction> &variableList = view->variableActionList();
    variableList.clear();
    actionList.clear();

    view->kPresenterDoc()->getVariableCollection()->setVariableSelected(variable());
    if ( variable() )
    {
        variableList = view->kPresenterDoc()->getVariableCollection()->popupActionList();
    }

    if( variableList.count()>0)
    {
        view->plugActionList( "variable_action", variableList );
        QPopupMenu * popup = view->popupMenu("variable_popup");
        Q_ASSERT(popup);
        if (popup)
            popup->popup( point ); // using exec() here breaks the spellcheck tool (event loop pb)

    }
    else
    {
        bool singleWord= false;
        actionList = dataToolActionList(view->kPresenterDoc()->instance(), word, singleWord);
        //kdDebug(33001) << "KWView::openPopupMenuInsideFrame plugging actionlist with " << actionList.count() << " actions" << endl;
        if(refLink().isNull())
        {
            QPopupMenu * popup;
            view->plugActionList( "datatools", actionList );

            KoNoteVariable * var = dynamic_cast<KoNoteVariable *>(variable());
            KoCustomVariable * varCustom = dynamic_cast<KoCustomVariable *>(variable());
            if( var )
                popup = view->popupMenu("note_popup");
            else if( varCustom )
                popup = view->popupMenu("custom_var_popup");
            else
            {
                if ( singleWord )
                {
                    QPtrList<KAction> actionCheckSpellList =view->listOfResultOfCheckWord( word );
                    if ( actionCheckSpellList.count()>0)
                    {
                        view->plugActionList( "spell_result_action", actionCheckSpellList );
                        popup = view->popupMenu("text_popup_spell_with_result");
                    }
                    else
                        popup = view->popupMenu("text_popup_spell");
                }
                else
                    popup = view->popupMenu("text_popup");
            }
            Q_ASSERT(popup);
            if (popup)
                popup->popup( point ); // using exec() here breaks the spellcheck tool (event loop pb)
        }
        else
        {
            view->plugActionList( "datatools_link", actionList );
            QPopupMenu * popup = view->popupMenu("text_popup_link");
            Q_ASSERT(popup);
            if (popup)
                popup->popup( point ); // using exec() here breaks the spellcheck tool (event loop pb)
        }
    }
}

void KPTextView::insertCustomVariable( const QString &name)
{
    KoVariable * var = 0L;
    KPresenterDoc * doc = kpTextObject()->kPresenterDocument();
    var = new KoCustomVariable( textDocument(), name, doc->variableFormatCollection()->format( "STRING" ),
                                doc->getVariableCollection());
    insertVariable( var);
}

void KPTextView::insertLink(const QString &_linkName, const QString & hrefName)
{
    KoVariable * var = 0L;
    KPresenterDoc * doc = kpTextObject()->kPresenterDocument();
    var = new KoLinkVariable( textDocument(),_linkName, hrefName,
                              doc->variableFormatCollection()->format( "STRING" ),
                              doc->getVariableCollection());
    insertVariable( var);
}


void KPTextView::insertComment(const QString &_comment)
{
    KoVariable * var = 0L;
    KPresenterDoc * doc = kpTextObject()->kPresenterDocument();

    var = new KoNoteVariable( textDocument(),_comment, doc->variableFormatCollection()->format( "STRING" ),
                              doc->getVariableCollection());
    insertVariable( var, 0,false/*don't delete selected text*/);
}


void KPTextView::insertVariable( int type, int subtype )
{
    KPresenterDoc * doc = kpTextObject()->kPresenterDocument();
    bool refreshCustomMenu = false;
    KoVariable * var = 0L;
    if ( type == VT_CUSTOM )
    {
        KoCustomVarDialog dia( m_canvas );
        if ( dia.exec() == QDialog::Accepted )
        {
            KoCustomVariable *v = new KoCustomVariable( textDocument(), dia.name(),
                                                        doc->variableFormatCollection()->format( "STRING" ),
                                                        doc->getVariableCollection() );
            v->setValue( dia.value() );
            var = v;
            refreshCustomMenu = true;
        }
    }
    else
        var = doc->getVariableCollection()->createVariable( type, subtype,  doc->variableFormatCollection(), 0L, textDocument(),doc, 0);
    if ( var )
    {
        insertVariable( var , 0L, true,refreshCustomMenu);
        doc->recalcPageNum();
    }
}

void KPTextView::insertVariable( KoVariable *var, KoTextFormat *format /*=0*/, bool removeSelectedText,bool refreshCustomMenu )
{
    if ( var )
    {
        CustomItemsMap customItemsMap;
        customItemsMap.insert( 0, var );
        if (!format)
            format = currentFormat();
        //kdDebug(33001) << "KPTextView::insertVariable inserting into paragraph" << endl;
#ifdef DEBUG_FORMATS
        kdDebug(33001) << "KPTextView::insertVariable currentFormat=" << currentFormat() << endl;
#endif
        textObject()->insert( cursor(), format, KoTextObject::customItemChar(),
                              false, removeSelectedText, i18n("Insert Variable"),
                              customItemsMap );
        if ( refreshCustomMenu && var->type() == VT_CUSTOM )
            kpTextObject()->kPresenterDocument()->refreshMenuCustomVariable();
        kpTextObject()->kPresenterDocument()->repaint( kpTextObject() );
    }
}

KPrTextDrag * KPTextView::newDrag( QWidget * parent )
{
#if 0 //old koffice-1.3 format
    KoTextCursor c1 = textDocument()->selectionStartCursor( KoTextDocument::Standard );
    KoTextCursor c2 = textDocument()->selectionEndCursor( KoTextDocument::Standard );

    QString text;

    QDomDocument domDoc( "PARAGRAPHS" );
    QDomElement elem = domDoc.createElement( "TEXTOBJ" );
    domDoc.appendChild( elem );
    if ( c1.parag() == c2.parag() )
    {
        text = c1.parag()->toString( c1.index(), c2.index() - c1.index() );

        m_kptextobj->saveParagraph( domDoc, c1.parag(), elem, c1.index(), c2.index()-1 );
    }
    else
    {
        text += c1.parag()->toString( c1.index() ) + "\n";

        m_kptextobj->saveParagraph( domDoc, c1.parag(), elem, c1.index(), c1.parag()->length()-1 );
        KoTextParag *p = c1.parag()->next();
        while ( p && p != c2.parag() ) {
            text += p->toString() + "\n";
            m_kptextobj->saveParagraph( domDoc, p, elem, 0, p->length()-2 );
            p = p->next();
        }
        text += c2.parag()->toString( 0, c2.index() );
        m_kptextobj->saveParagraph( domDoc, c2.parag(), elem, 0, c2.index()-1 );
    }
#else
    KoGenStyles mainStyles;
    KoSavingContext savingContext( mainStyles, KoSavingContext::Flat );

    // Save user styles as KoGenStyle objects - useful when pasting to another document
    KPresenterDoc * doc = kpTextObject()->kPresenterDocument();
    KoSavingContext::StyleNameMap map = doc->styleCollection()->saveOasis( mainStyles, KoGenStyle::STYLE_USER );
    savingContext.setStyleNameMap( map );

    QBuffer buff;
    buff.open( IO_WriteOnly );
    KoXmlWriter contentWriter( &buff, "office:document-content" );
    // not sure how to avoid copy/pasting that code...
    KTempFile contentTmpFile;
    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();
    KoXmlWriter contentTmpWriter( tmpFile, 1 );
    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:presentation" );

    const QString plainText = textDocument()->copySelection( contentTmpWriter, savingContext, KoTextDocument::Standard );

    contentTmpWriter.endElement(); // office:text
    contentTmpWriter.endElement(); // office:body

    // Done with writing out the contents to the tempfile, we can now write out the automatic styles
    KPresenterDoc::writeAutomaticStyles( contentWriter, mainStyles );

    // And now we can copy over the contents from the tempfile to the real one
    tmpFile->close();
    contentWriter.addCompleteElement( tmpFile );
    contentTmpFile.close();
    contentWriter.endElement(); // document-content
    contentWriter.endDocument();

    const QByteArray data = buff.buffer();
    const QCString cstr( data.data(), data.size() + 1 ); // null-terminate

#endif
    KPrTextDrag *kd = new KPrTextDrag( parent );
    kd->setPlain( plainText );
    kd->setTextObjectNumber( m_canvas->textObjectNum(kpTextObject()) );
    kd->setKPresenter( cstr );
    kdDebug(33001) << "KPTextView::newDrag " << cstr << endl;
    return kd;
}

void KPTextView::dragEnterEvent( QDragEnterEvent *e )
{
    if ( !kpTextObject()->kPresenterDocument()->isReadWrite() || !KPrTextDrag::canDecode( e ) )
    {
        e->ignore();
        return;
    }
    e->acceptAction();
}

void KPTextView::dragMoveEvent( QDragMoveEvent *e, const QPoint & )
{
    KPresenterDoc *doc= kpTextObject()->kPresenterDocument();
    if ( !doc->isReadWrite() || !KPrTextDrag::canDecode( e ) )
    {
        e->ignore();
        return;
    }
    QPoint iPoint=e->pos() - doc->zoomHandler()->zoomPoint(
        kpTextObject()->getOrig()+KoPoint( kpTextObject()->bLeft(),kpTextObject()->bTop()+kpTextObject()->alignmentValue()));
    iPoint=kpTextObject()->kPresenterDocument()->zoomHandler()->pixelToLayoutUnit(
        QPoint(iPoint.x()+ m_canvas->diffx(),iPoint.y()+m_canvas->diffy()) );

    textObject()->emitHideCursor();
    placeCursor( iPoint );
    textObject()->emitShowCursor();
    e->acceptAction(); // here or out of the if ?
}

void KPTextView::dropEvent( QDropEvent * e )
{
    KPresenterDoc *doc= kpTextObject()->kPresenterDocument();
    if ( doc->isReadWrite() && KPrTextDrag::canDecode( e ) )
    {
        e->acceptAction();
        KoTextCursor dropCursor( textDocument() );
        QPoint dropPoint=e->pos() - doc->zoomHandler()->zoomPoint(
            kpTextObject()->getOrig()+KoPoint( kpTextObject()->bLeft(),kpTextObject()->bTop()+kpTextObject()->alignmentValue()));
        dropPoint=doc->zoomHandler()->pixelToLayoutUnit( QPoint(dropPoint.x()+ m_canvas->diffx(),dropPoint.y()+m_canvas->diffy()) );
        KMacroCommand *macroCmd=new KMacroCommand(i18n("Paste Text"));
        dropCursor.place( dropPoint, textDocument()->firstParag() );
        kdDebug(33001) << "KPTextView::dropEvent dropCursor at parag=" << dropCursor.parag()->paragId() << " index=" << dropCursor.index() << endl;

        if ( ( e->source() == m_canvas ) &&
             e->action() == QDropEvent::Move ) {
            //kdDebug(33001)<<"decodeFrameSetNumber( QMimeSource *e ) :"<<numberFrameSet<<endl;
            int objTextNum=-1;
            objTextNum=KPrTextDrag::decodeTextObjectNumber( e );
            KPTextObject * obj = m_canvas->textObjectByPos( objTextNum );

            obj =obj  ? obj : kpTextObject();
            if ( obj )
            {
                bool dropInSameObj= ( obj == kpTextObject());
                KCommand *cmd=textView()->dropEvent(obj->textObject(), dropCursor, dropInSameObj );
                if(cmd)
                {
                    obj->layout();
                    kpTextObject()->layout();
                    macroCmd->addCommand(cmd);
                }
                else
                {
                    delete macroCmd;
                    return;
                }
            }
        }
        else
        {   // drop coming from outside -> forget about current selection
            textDocument()->removeSelection( KoTextDocument::Standard );
            textObject()->selectionChangedNotify();
        }
        QString returnedTypeMime;
        if ( KPrTextDrag::provides( e , KPrTextDrag::selectionMimeType(), KPrTextDrag::acceptSelectionMimeType(),  returnedTypeMime) )
        {
            kdDebug()<<" dropevent !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
            QByteArray arr = e->encodedData( returnedTypeMime.latin1() );
            if ( arr.size() )
            {
                kdDebug()<<" arr.size !!!!!!!!!!!!\n";
                macroCmd->addCommand(kpTextObject()->pasteOasis( cursor(), QCString(arr, arr.size()+1), false ));
            }
        }
        else
        {
            QString text;
            if ( QTextDrag::decode( e, text ) )
                textObject()->pasteText( cursor(), text, currentFormat(), false );
        }
        doc->addCommand(macroCmd);
    }
}

void KPTextObject::saveParagraph( QDomDocument& doc,KoTextParag * parag,QDomElement &parentElem,
                                  int from /* default 0 */,
                                  int to /* default length()-2 */ )
{
    if(!parag)
        return;
    QDomElement paragraph=doc.createElement(tagP);
    int tmpAlign=0;
    switch(parag->resolveAlignment())
    {
    case Qt::AlignLeft:
        tmpAlign=1;
        break;
    case Qt::AlignRight:
        tmpAlign=2;
        break;
    case Qt::AlignHCenter:
        tmpAlign=4;
        break;
    case Qt::AlignJustify:
        tmpAlign=8;
    }
    if(tmpAlign!=1)
        paragraph.setAttribute(attrAlign, tmpAlign);

    saveParagLayout( parag->paragLayout(), paragraph );
    KoTextFormat *lastFormat = 0;
    QString tmpText;
    for ( int i = from; i <= to; ++i ) {
        KoTextStringChar &c = parag->string()->at(i);
        if ( c.isCustom() )
        {
            QDomElement variable = doc.createElement("CUSTOM");
            variable.setAttribute("pos", (i-from));
            saveFormat( variable, c.format() );
            paragraph.appendChild( variable );
            static_cast<KoTextCustomItem *>( c.customItem() )->save(variable );
        }
        if ( !lastFormat || c.format()->key() != lastFormat->key() ) {
            if ( lastFormat )
                paragraph.appendChild(saveHelper(tmpText, lastFormat, doc));
            lastFormat = static_cast<KoTextFormat*> (c.format());
            tmpText=QString::null;
        }
        tmpText+=QString(c.c);
    }
    if ( lastFormat )
        paragraph.appendChild(saveHelper(tmpText, lastFormat, doc));
    else
        paragraph.appendChild(saveHelper(tmpText, parag->string()->at(0).format(), doc));

    parentElem.appendChild(paragraph);
}

QPen KPTextObject::defaultPen() const
{
    return QPen( Qt::black, 1, Qt::NoPen );
}
