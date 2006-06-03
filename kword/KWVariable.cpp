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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWVariable.h"

#include "KWTextFrameSet.h"
#include "KWTextDocument.h"
#include "KWMailMergeDataBase.h"
#include "KWDocument.h"
#include "KWCommand.h"
#include "KWViewMode.h"
#include "KWPageManager.h"

#include <KoVariable.h>
#include <KoTextParag.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoDom.h>

#include <klocale.h>
#include <kdebug.h>

KWVariableSettings::KWVariableSettings() : KoVariableSettings()
{
    m_footNoteCounter.setSuffix( QString::null );
    m_endNoteCounter.setSuffix( QString::null );
    // By default endnotes are numbered in lowercase roman numbers, in other WPs.
    m_endNoteCounter.setStyle( KoParagCounter::STYLE_ROM_NUM_L );
}

void KWVariableSettings::changeFootNoteCounter( KoParagCounter _c )
{
    m_footNoteCounter = _c;
}

void KWVariableSettings::changeEndNoteCounter( KoParagCounter _c )
{
    m_endNoteCounter = _c;
}

void KWVariableSettings::saveNoteConfiguration( KoXmlWriter& writer ) const
{
    writer.startElement( "text:notes-configuration" );
    writer.addAttribute( "text:note-class", "footnote" );
    // let the counter save: num-prefix num-suffix num-format start-value
    m_footNoteCounter.saveOasisListLevel( writer, false );
    writer.addAttribute( "text:footnotes-position", "page" ); // tell OO what we do
    writer.addAttribute( "text:start-numbering-at", "document" ); // tell OO what we do
    writer.endElement();
    writer.startElement( "text:notes-configuration" );
    writer.addAttribute( "text:note-class", "endnote" );
    // let the counter save: num-prefix num-suffix num-format start-value
    m_endNoteCounter.saveOasisListLevel( writer, false );
    writer.addAttribute( "text:start-numbering-at", "document" ); // tell OO what we do
    writer.endElement();
}

void KWVariableSettings::loadNoteConfiguration( const QDomElement& parent )
{
    QDomElement e;
    forEachElement( e, parent )
    {
        if ( e.localName() == "notes-configuration" && e.namespaceURI() == KoXmlNS::text )
        {
            const QString noteClass = e.attributeNS( KoXmlNS::text, "note-class", QString::null );
            if ( noteClass == "footnote" ) {
                m_footNoteCounter.loadOasisListStyle( e, QDomElement(), QDomElement(), -1, true, false, 1, false );
                m_footNoteCounter.setNumbering( KoParagCounter::NUM_FOOTNOTE );
                m_footNoteCounter.setRestartCounter( false );
            }
            else if ( noteClass == "endnote" ) {
                m_endNoteCounter.loadOasisListStyle( e, QDomElement(), QDomElement(), -1, true, false, 1, false );
                m_endNoteCounter.setNumbering( KoParagCounter::NUM_FOOTNOTE );
                m_endNoteCounter.setRestartCounter( false );
            }
        }
    }
}

void KWVariableSettings::save( QDomElement &parentElem )
{
    KoVariableSettings::save( parentElem );
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement footNoteSettingElem = doc.createElement( "FOOTNOTESETTING" );
    parentElem.appendChild( footNoteSettingElem );
    m_footNoteCounter.save( footNoteSettingElem );
    QDomElement endNoteSettingElem = doc.createElement( "ENDNOTESETTING" );
    parentElem.appendChild( endNoteSettingElem );
    m_endNoteCounter.save( endNoteSettingElem );
}

void KWVariableSettings::load( QDomElement &elem )
{
    KoVariableSettings::load( elem );
    QDomElement footNoteSettings = elem.namedItem( "FOOTNOTESETTING" ).toElement();
    if ( !footNoteSettings.isNull() )
        m_footNoteCounter.load( footNoteSettings );
    QDomElement endNoteSettings = elem.namedItem( "ENDNOTESETTING" ).toElement();
    if ( !endNoteSettings.isNull() )
        m_endNoteCounter.load( endNoteSettings );
}

KWVariableCollection::KWVariableCollection(KWVariableSettings *_setting, KoVariableFormatCollection* coll)
    : KoVariableCollection(_setting, coll)
{
}

KoVariable* KWVariableCollection::loadOasisField( KoTextDocument* textdoc, const QDomElement& tag, KoOasisContext& context )
{
    const QString localName( tag.localName() );
    const bool isTextNS = tag.namespaceURI() == KoXmlNS::text;
    if ( isTextNS )
    {
        //kDebug()<<" localName :"<<localName<<endl;
        if ( localName ==  "note" )
        {
            QString key = "STRING";
            int type = VT_FOOTNOTE;
            return loadOasisFieldCreateVariable( textdoc, tag, context, key, type );
        }
        else if ( localName == "table-count"  ||
                  localName == "object-count"  ||
                  localName == "picture-count"  ||
                  localName == "paragraph-count"  ||
                  localName == "word-count"  ||
                  localName == "character-count" ||
                  localName == "sentence-count" ||
                  localName == "line-count" ||
                  localName == "frame-count"  ||
                  localName == "non-whitespace-character-count" ||
                  localName == "syllable-count")
        {
            QString key = "NUMBER";
            int type = VT_STATISTIC;
            return loadOasisFieldCreateVariable( textdoc, tag, context, key, type );
        }
        else
            return KoVariableCollection::loadOasisField( textdoc, tag, context );
    }
    else
        return KoVariableCollection::loadOasisField( textdoc, tag, context );
}

KoVariable *KWVariableCollection::createVariable( int type, short int subtype, KoVariableFormatCollection * coll, KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc, int _correct,bool _forceDefaultFormat, bool loadFootNote )
{
    KWDocument *m_doc = static_cast<KWDocument *>(doc);
    KoVariable * var = 0L;
    switch(type) {
    case VT_PGNUM:
        if ( !varFormat )
            varFormat = (subtype == KoPageVariable::VST_CURRENT_SECTION) ? coll->format("STRING") : coll->format("NUMBER");
        var = new KWPgNumVariable( textdoc, subtype, varFormat, this, m_doc );
        break;
    case VT_MAILMERGE:
        var = new KWMailMergeVariable( textdoc, QString::null, coll->format("STRING"), this, m_doc );
        break;
    case VT_FOOTNOTE:
        if ( !loadFootNote )
            return 0L;
        if ( !varFormat )
            varFormat =  coll->format("STRING");
        var = new KWFootNoteVariable( textdoc, varFormat, this, m_doc );
        break;
    case VT_STATISTIC:
        if ( !varFormat )
            varFormat = coll->format("NUMBER");
        var = new KWStatisticVariable( textdoc, subtype, varFormat, this, m_doc );
        break;
    default:
        return KoVariableCollection::createVariable( type, subtype, coll, varFormat, textdoc, doc, _correct, _forceDefaultFormat );
    }
    return var;
}

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/
KWPgNumVariable::KWPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl, KWDocument *doc  )
    : KoPageVariable( textdoc, subtype, varFormat ,_varColl ), m_doc(doc)
{
}

void KWPgNumVariable::recalc()
{
    if ( !m_doc->layoutViewMode()->hasPages() ) // ModeText
    {
        //necessary to resize it in this mode because in this mode
        //we don't call KWTextFrameSet::drawFrame()
        resize();
        return;
    }

    if ( m_subtype == VST_PGNUM_TOTAL )
    {
        m_varValue = QVariant(m_doc->pageCount()+m_varColl->variableSetting()->startingPageNumber()-1);
        resize();
    }
    // The other cases are handled by the more dynamic code in KWTextFrameSet::drawFrame()
    // But we don't want to keep a width of -1 ...
    if ( width == -1 )
        width = 0;
}

QString KWPgNumVariable::text(bool realValue)
{
    if (m_varColl->variableSetting()->displayFieldCode()&& !realValue)
        return fieldCode();
    // #### ??? What?
    else if ( m_subtype !=  VST_CURRENT_SECTION && !m_doc->layoutViewMode()->hasPages() && !realValue)
        return fieldCode();
    else
        return m_varFormat->convert( m_varValue );
}

/******************************************************************/
/* Class: KWMailMergeVariable                                  */
/******************************************************************/
KWMailMergeVariable::KWMailMergeVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KWDocument *doc  )
    : KoMailMergeVariable( textdoc, name, varFormat,_varColl ), m_doc(doc)
{
}

QString KWMailMergeVariable::value() const
{
    return m_doc->mailMergeDataBase()->getValue( m_varValue.toString() );
}

QString KWMailMergeVariable::text(bool realValue)
{
    if (m_varColl->variableSetting()->displayFieldCode()&& !realValue)
        return fieldCode();
    // ## should use a format maybe
    QString v = value();
    if ( m_doc->mailMergeDataBase()->isSampleRecord() )
        return '<' + v + '>';
    return v;
}

void KWMailMergeVariable::recalc()
{
    resize();
}

/////////////

KWFootNoteVariable::KWFootNoteVariable( KoTextDocument *textdoc, KoVariableFormat *varFormat, KoVariableCollection *varColl, KWDocument *doc )
    : KoVariable( textdoc, varFormat, varColl ),
      m_doc(doc),
      m_frameset( 0L ),
      m_numberingType( Auto ),
      m_num( -1 ),
      m_numDisplay( -1 )
{
    m_varValue = QVariant( QString::null );
}

void KWFootNoteVariable::setNumberingType( Numbering _type )
{
    m_numberingType = _type;
    //delete m_varFormat;
    setVariableFormat(m_doc->variableFormatCollection()->format("STRING"));
}

void KWFootNoteVariable::loadOasis( const QDomElement& footNoteTag, KoOasisContext& context )
{
#if 0
    /*<text:note text:id="ftn0" text:note-class="footnote"><text:note-citation>1</text:note-citation><text:note-body>
      <text:p text:style-name="Footnote"></text:p></text:note-body></text:note> */
    const QString id = footNoteTag.attributeNS( KoXmlNS::text, "id", QString::null );

    if ( footNoteTag.hasAttributeNS( KoXmlNS::text, "note-class" ) )
    {
      const QString str =  footNoteTag.attributeNS( KoXmlNS::text, "note-class", QString::null );
      if ( str == "footnote" )
        m_noteType = FootNote;
      else if ( str == "endnote" )
        m_noteType = EndNote;
      else {
        kWarning()<<" Unknown footnote type: '" << str << "'" << endl;
        m_noteType = FootNote;
      }
    }

    QDomElement element;
    QDomElement bodyElement;
    forEachElement( element, footNoteTag )
    {
      if ( element.namespaceURI() != KoXmlNS::text )
        continue;

      const QString localName = element.localName();
      if( localName == "note-citation" )
      {
        if ( element.hasAttributeNS( KoXmlNS::text, "label" ) )
            m_numberingType = Manual;
        else
            m_numberingType = Auto;
        if ( m_numberingType == Auto )
        {
            //kDebug()<<" automatic \n";
            m_numDisplay = element.text().toInt();
            formatedNote();
        }
        else
        {
           // kDebug()<<" manual \n";
            m_varValue = QVariant( element.text() );
        }
      } else if ( localName == "note-body" ) {
          bodyElement = element;
      }
    }
    Q_ASSERT( !bodyElement.isNull() );

    Q_ASSERT( !m_frameset );
    m_frameset = new KWFootNoteFrameSet( m_doc, id );
    m_frameset->setFrameSetInfo( KWFrameSet::FI_FOOTNOTE );

    m_frameset->setFootNoteVariable( this );
    m_frameset->createInitialFrame( 0 );
    m_doc->addFrameSet( m_frameset );

    // Load the body of the footnote
    m_frameset->loadOasisContent( bodyElement, context );
#endif
}

void KWFootNoteVariable::saveOasis( KoXmlWriter& writer, KoSavingContext& context ) const
{
#if 0
    //<text:note text:id="ftn0" text:note-class="footnote"><text:note-citation>1</text:note-citation><text:note-body><text:p text:style-name="Footnote"/></text:note-body></text:note>
    //<text:note text:id="ftn1" text:note-class="endnote"><text:note-citation>i</text:note-citation><text:note-body><text:p text:style-name="Endnote"/></text:note-body></text:note>
    //<text:note text:id="ftn2" text:note-class="footnote"><text:note-citation text:label="vv">vv</text:note-citation><text:note-body><text:p text:style-name="Footnote"/></text:note-body></text:note>
    writer.startElement( "text:note" );
    writer.addAttribute( "text:id",m_frameset->name() );
    writer.addAttribute( "text:note-class", m_noteType == FootNote ? "footnote" : "endnote" );

    writer.startElement( "text:note-citation" );
    if ( m_numberingType == Auto )
        writer.addTextNode( QString( "%1" ).arg( m_numDisplay ) );
    else
    {
        writer.addAttribute( "text:label", m_varValue.toString() );
        writer.addTextNode(m_varValue.toString() );
    }
    writer.endElement();

    writer.startElement( "text:note-body" );
    //save text from end/footnote
    m_frameset->saveOasisContent( writer, context );
    writer.endElement();

    writer.endElement();
#endif
}

void KWFootNoteVariable::saveVariable( QDomElement &parentElem )
{
    QDomElement footnoteElem = parentElem.ownerDocument().createElement( "FOOTNOTE" );
    parentElem.appendChild( footnoteElem );
    //footnoteElem.setAttribute( "subtype", 0 );
    if ( m_numberingType == Auto )
        footnoteElem.setAttribute( "value", m_numDisplay );
    else
        footnoteElem.setAttribute( "value", m_varValue.toString() );
    footnoteElem.setAttribute( "notetype", m_noteType == FootNote ? "footnote" : "endnote" );
    footnoteElem.setAttribute( "numberingtype", m_numberingType == Auto ? "auto" : "manual" );
    Q_ASSERT( m_frameset );
    if( m_frameset )
      footnoteElem.setAttribute( "frameset", m_frameset->name() );
}

void KWFootNoteVariable::load( QDomElement &elem )
{
    KoVariable::load( elem );
    QDomElement footnoteElem = elem.namedItem( "FOOTNOTE" ).toElement();
    if (!footnoteElem.isNull())
    {
        //m_subtype = footnoteElem.attribute("subtype").toInt();
        QString str = footnoteElem.attribute("notetype").lower();
        if ( str == "footnote" )
            m_noteType = FootNote;
        else if ( str == "endnote" )
            m_noteType = EndNote;
        else
            kWarning() << "Unknown footnote type: '" << str << "'" << endl;

        str = footnoteElem.attribute("numberingtype").lower();
        if ( str == "auto" )
            m_numberingType = Auto;
        else if ( str == "manual")
            m_numberingType = Manual;
        else
            kWarning() << "Unknown footnote numbering: '" << str << "'" << endl;

        if ( m_numberingType == Auto )
        {
            m_numDisplay = footnoteElem.attribute("value").toInt();
            formatedNote();
        }
        else
            m_varValue = QVariant(footnoteElem.attribute("value"));

        str = footnoteElem.attribute("frameset");
        m_doc->addFootNoteRequest( str, this );
    }
}

void KWFootNoteVariable::formatedNote()
{
    if ( m_numberingType == Auto )
    {
        m_varValue = QVariant(applyStyle());
    }
}

QString KWFootNoteVariable::applyStyle()
{
    KWVariableSettings* settings = static_cast<KWVariableSettings*>(m_varColl->variableSetting());
    KoParagCounter tmpCounter = (m_noteType == FootNote) ? settings->footNoteCounter() : settings->endNoteCounter();

    QString tmp;
    int val = m_numDisplay + tmpCounter.startNumber()-1;
    Q_ASSERT( val >= 0 );
    if ( val < 0 ) // let's not go into makeRomanNumber with a negative number :}
        return i18n("ERROR");
    switch ( tmpCounter.style() )
    {
    case KoParagCounter::STYLE_NUM:
        tmp.setNum( val );
        break;
    case KoParagCounter::STYLE_ALPHAB_L:
        tmp=KoParagCounter::makeAlphaLowerNumber( val );
        break;
    case KoParagCounter::STYLE_ALPHAB_U:
        tmp=KoParagCounter::makeAlphaUpperNumber( val );
        break;
    case KoParagCounter::STYLE_ROM_NUM_L:
        tmp = KoParagCounter::makeRomanNumber( val ).lower();
        break;
    case KoParagCounter::STYLE_ROM_NUM_U:
        tmp = KoParagCounter::makeRomanNumber( val ).upper();
        break;
    case KoParagCounter::STYLE_CUSTOMBULLET:
        //todo change font
        tmp = tmpCounter.customBulletCharacter();
        break;
    default:
        tmp.setNum( val );
        break;
    }

    tmp.prepend( tmpCounter.prefix() );
    tmp.append( tmpCounter.suffix() );
    return tmp;
}

QString KWFootNoteVariable::text(bool realValue)
{
    if (m_varColl->variableSetting()->displayFieldCode()&& !realValue)
        return fieldCode();
    return m_varFormat->convert( m_varValue );
}

void KWFootNoteVariable::setNumDisplay( int val )
{
    m_numDisplay = val;
    if ( val != -1 ) // -1 is used to 'invalidate so that renumberFootNotes recalcs'
        formatedNote();
}

QString KWFootNoteVariable::fieldCode()
{
    return (noteType()==FootNote) ?i18n("Footnote"):i18n("Endnote");
}

void KWFootNoteVariable::drawCustomItem( QPainter* p, int x, int y, int wpix, int hpix, int ascentpix, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/, const QColorGroup& cg, bool selected, int _offset, bool drawingShadow )
{
    KoTextFormat * fmt = format();
    KoTextZoomHandler * zh = textDocument()->paintingZoomHandler();

    // Force drawing as "superscript" - hmm, the formatting will use too big font metrics though.
    QFont font( fmt->screenFont( zh ) );
    int pointSize = ( ( font.pointSize() * 2 ) / 3 );
    font.setPointSize( pointSize );

    int offset = _offset;
    if ( offset == 0 )
    {
        int h = zh->layoutUnitToPixelY( /*_y HACK,*/ height );
        offset = -( h - QFontMetrics(font).height() );
    }

    QColor textColor( fmt->color() );
    drawCustomItemHelper( p, x, y, wpix, hpix, ascentpix, cg, selected, offset, fmt, font, textColor, drawingShadow );
}

void KWFootNoteVariable::finalize()
{
#if 0
    Q_ASSERT( m_frameset );
    if (!m_frameset )
        return;
    Q_ASSERT( !m_frameset->isDeleted() );
    if ( m_frameset->isDeleted() )
        return;

    //kDebug(32001) << "KWFootNoteVariable::finalize" << endl;

    int pageNum = this->pageNum();
    if ( pageNum == -1 )
        return;

    KWFrame* footNoteFrame = m_frameset->frame( 0 );
    int framePage = footNoteFrame->pageNumber();
    if ( framePage != pageNum )
    {
        //kDebug(32001) << "Footnote var '" << text() << "' at page " << pageNum << ", footnote frame at page " << framePage << " -> abortFormatting() and recalcFrames()" << endl;
        KWTextFrameSet * fs = static_cast<KWTextDocument *>(textDocument())->textFrameSet();
        fs->textObject()->abortFormatting();

        // abortFormatting is a bool in kotextobject. So we need to return there before
        // starting text layout again.
        m_doc->delayedRecalcFrames( qMin( pageNum, framePage ) );
        m_doc->delayedRepaintAllViews();
    }
#endif
}

void KWFootNoteVariable::resize()
{
    if ( m_deleted )
        return;
    KoTextFormat *fmt = format();
    QFont font( fmt->refFont() ); // LU font
    if ( fmt->vAlign() == KoTextFormat::AlignNormal ) // if it's still normal...
    {
        int pointSize = ( ( font.pointSize() * 2 ) / 3 ); // ...force superscript
        font.setPointSize( pointSize );
    }
    QFontMetrics fm( font );
    QString txt = text();
    width = 0;
    for ( int i = 0 ; i < (int)txt.length() ; ++i )
        width += fm.charWidth( txt, i ); // size at 100%
    // zoom to LU
    width = qRound( KoTextZoomHandler::ptToLayoutUnitPt( width ) );
    height = fmt->height();
    m_ascent = fmt->ascent();
    //kDebug() << "KWFootNoteVariable::resize text=" << txt << " width=" << width << " height=" << height << endl;
}

void KWFootNoteVariable::setDeleted( bool del )
{
#if 0
    kDebug() << "KWFootNoteVariable::setDeleted " << del << endl;
    if ( del )
    {
        Q_ASSERT( m_frameset );
        if ( m_frameset ) {
            m_frameset->deleteAllFrames(); // Important, because we don't want to save it!
            m_frameset->setVisible( false );
        }
    }
    else
    {
        Q_ASSERT( m_frameset );
        if ( m_frameset ) {
            kDebug() << "Making frameset " << m_frameset << " visible" << endl;
            m_frameset->setVisible( true );
            if ( m_frameset->isDeleted() )
                m_frameset->createInitialFrame( 0 ); // Page number shouldn't matter (see recalcFrames below).
            Q_ASSERT( m_frameset->isVisible() );
        }
    }

    // hmm, maybe compress all the stuff below and do only once
    // (e.g. when deleting multiple footnotes)?
    // (but we can't really delay it with a sst, the formatMore after undo/redo
    // needs this to be done already, I think). Bah.

    // Re-number footnote variables
    KWTextFrameSet * textfs = static_cast<KWTextDocument *>(textDocument())->textFrameSet();
    textfs->renumberFootNotes();

    m_doc->recalcFrames();

    if (!del)
        m_frameset->layout(); // format its text, so that it resizes the frame
    KoVariable::setDeleted( del );
    // Does this compress? Probably not.
    m_doc->delayedRepaintAllViews();
#endif
}

int KWFootNoteVariable::pageNum() const
{
    int page = m_doc->pageManager()->pageNumber(varY());
    Q_ASSERT( page <= m_doc->lastPage() );
    return page;
}

double KWFootNoteVariable::varY() const
{
#if 0
    // Find out the position of the footnote variable in document coordinates.
    int paragy = paragraph()->rect().y();
    KWTextFrameSet * fs = static_cast<KWTextDocument *>(textDocument())->textFrameSet();
    if ( !fs->hasFramesInPageArray() ) // we need it for internalToDocument
    {
        kDebug(32001) << "KWFootNoteVariable::varY too early, no updateFrames yet" << endl;
        return 0; // this happens on loading - frame layout is done before text layout
    }
    // What we need is "has never been formatted". Not "has just been invalidated"...
    //if ( !paragraph()->isValid() )
    //{
    //    kDebug(32001) << "KWFootNoteVariable::varY called but paragraph " << paragraph()->paragId() << " not valid" << endl;
    //    return 0;
    //}
    KoPoint dPoint;
    //kDebug(32001) << "KWFootNoteVariable::pageNum position of variable (LU): " << QPoint( x(), paragy + y() + height ) << endl;
    KWFrame* containingFrame = fs->internalToDocument( QPoint( x(), paragy + y() + height ), dPoint );
    if ( containingFrame )
    {
        // Ok, the (bottom of the) footnote variable is at dPoint.
        double varY = dPoint.y();
        //kDebug(32001) << " found containingFrame " << containingFrame << " page:" << containingFrame->pageNumber() << "  varY=" << varY << endl;
        //int pageNum = containingFrame->pageNumber(); // and at page pageNum
        return varY;
    } else
    {
        // This can happen if the page hasn't been created yet
        //kDebug(32001) << "KWFootNoteVariable::pageNum internalToDocument returned 0L for " << x << ", " << y+paragy << endl;
        return 0;
    }
#endif
}


KWStatisticVariable::KWStatisticVariable( KoTextDocument *textdoc,  int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KWDocument *doc )
    : KoStatisticVariable( textdoc, subtype, varFormat, _varColl ),
      m_doc(doc)
{
}

void KWStatisticVariable::recalc()
{
    int nb = 0;
    ulong charsWithSpace = 0L;
    ulong charsWithoutSpace = 0L;
    ulong words = 0L;
    ulong sentences = 0L;
    ulong lines = 0L;
    ulong syllables = 0L;
    bool frameInfo = ( m_subtype == VST_STATISTIC_NB_WORD ||
                        m_subtype == VST_STATISTIC_NB_SENTENCE ||
                        m_subtype == VST_STATISTIC_NB_LINES ||
                        m_subtype == VST_STATISTIC_NB_CHARACTERE);
    Q3PtrListIterator<KWFrameSet> framesetIt( m_doc->framesetsIterator() );
    //TODO change int to ulong
    for ( framesetIt.toFirst(); framesetIt.current(); ++framesetIt )
    {
        KWFrameSet *frameSet = framesetIt.current();
        if ( frameSet->isVisible() )
        {
            if ( m_subtype == VST_STATISTIC_NB_FRAME )
            {
                ++nb;
            }
            else if( m_subtype == VST_STATISTIC_NB_PICTURE && frameSet->type() == FT_PICTURE )
            {
                ++nb;
            }
            else if( m_subtype == VST_STATISTIC_NB_TABLE && frameSet->type() == FT_TABLE )
            {
                ++nb;
            }
            else if( m_subtype == VST_STATISTIC_NB_EMBEDDED && frameSet->type() == FT_PART )
            {
                ++nb;
            }
            if ( frameInfo
                 && (frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE || frameSet->frameSetInfo() == KWFrameSet::FI_BODY)
                 && frameSet->isVisible() )
            {
                frameSet->statistics( 0L, charsWithSpace, charsWithoutSpace,
                                      words, sentences, syllables, lines, false );
            }
        }
        if ( frameInfo )
        {
            if( m_subtype == VST_STATISTIC_NB_WORD )
            {
                nb = words;
            }
            else if( m_subtype == VST_STATISTIC_NB_SENTENCE )
            {
                nb = sentences;
            }
            else if( m_subtype == VST_STATISTIC_NB_LINES )
            {
                nb = lines;
            }
            else if ( m_subtype == VST_STATISTIC_NB_CHARACTERE )
            {
                nb = charsWithSpace;
            }
            else if ( m_subtype == VST_STATISTIC_NB_NON_WHITESPACE_CHARACTERE )
            {
                nb = charsWithoutSpace;
            }
            else if ( m_subtype == VST_STATISTIC_NB_SYLLABLE )
            {
                nb = syllables;
            }
            else
                nb = 0;
        }
    }
    m_varValue = QVariant(nb);
    resize();
    if ( width == -1 )
        width = 0;
}

QString KWStatisticVariable::text(bool realValue)
{
    if ( m_varColl->variableSetting()->displayFieldCode() && !realValue )
        return fieldCode();
    else
        return m_varFormat->convert( m_varValue );
}
