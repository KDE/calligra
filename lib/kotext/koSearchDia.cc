/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001, S.R.Haque <srhaque@iee.org>
   Copyright (C) 2001, David Faure <david@mandrakesoft.com>

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

#include <kcolorbutton.h>
#include <klocale.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <kdebug.h>
#include "koSearchDia.h"
#include <kotextobject.h>
#include <kcommand.h>
#include <kotextview.h>
#include <kfontcombo.h>
#include <kseparator.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qregexp.h>
#include <kozoomhandler.h>

KoSearchContext::KoSearchContext()
{
    m_family = "times";
    m_color = Qt::black;
    m_backGroundColor = Qt::black;

    m_size = 12;
    m_vertAlign = KoTextFormat::AlignNormal;
    m_optionsMask = 0;
    m_options = KoFindDialog::FromCursor | KoReplaceDialog::PromptOnReplace;
    m_underline = KoTextFormat::U_NONE;
    m_strikeOut = KoTextFormat::S_NONE;
    m_attribute = KoTextFormat::ATT_NONE;
}

KoSearchContext::~KoSearchContext()
{
}


KoSearchContextUI::KoSearchContextUI( KoSearchContext *ctx, QWidget *parent )
    : QObject(parent), m_ctx(ctx), m_parent(parent)
{
    m_bOptionsShown = false;
    m_btnShowOptions = new QPushButton( i18n("Show Formatting Options"), parent );
    connect( m_btnShowOptions, SIGNAL( clicked() ), SLOT( slotShowOptions() ) );

    m_grid = new QGridLayout( m_parent, 1, 1, 0, 6 );
    m_grid->addWidget( m_btnShowOptions, 0, 0 );
    m_btnShowOptions->setEnabled( true );
}

void KoSearchContextUI::slotShowOptions()
{
    KoFormatDia * dlg = new KoFormatDia( m_parent, i18n("Formatting Options"), m_ctx );
    if ( dlg->exec())
    {
        dlg->ctxOptions( );
        m_bOptionsShown = true;
    }

    delete dlg;
}

void KoSearchContextUI::setCtxOptions( long options )
{

    if ( m_bOptionsShown )
    {
        options |= m_ctx->m_options;
    }
    m_ctx->m_options = options;
}

void KoSearchContextUI::setCtxHistory( const QStringList & history )
{
    m_ctx->m_strings = history;
}

KoSearchDia::KoSearchDia( QWidget * parent,const char *name, KoSearchContext *find, bool hasSelection ):
    KoFindDialog( parent, name, find->m_options, find->m_strings )
{
    // The dialog extension.
    m_findUI = new KoSearchContextUI( find, findExtension() );
    setHasSelection(hasSelection);
}

void KoSearchDia::slotOk()
{
    KoFindDialog::slotOk();

    // Save the current state back into the context required.
    if ( optionSelected() )
        m_findUI->setCtxOptions( options() );
    m_findUI->setCtxHistory( findHistory() );
}

KoReplaceDia::KoReplaceDia( QWidget *parent, const char *name, KoSearchContext *find, KoSearchContext *replace, bool hasSelection ):
    KoReplaceDialog( parent, name, find->m_options, find->m_strings, replace->m_strings )
{
    // The dialog extension.
    m_findUI = new KoSearchContextUI( find, findExtension() );
    m_replaceUI = new KoSearchContextUI( replace, replaceExtension() );
    // Look whether we have a selection
    setHasSelection(hasSelection);
}

void KoReplaceDia::slotOk()
{
    KoReplaceDialog::slotOk();

    // Save the current state back into the context required.
    m_findUI->setCtxOptions( KoReplaceDialog::options() );
    if ( optionFindSelected() )
        m_findUI->setCtxHistory( findHistory() );

    m_replaceUI->setCtxHistory( replacementHistory() );
    if ( optionSearchSelected() )
        m_replaceUI->setCtxOptions( KoReplaceDialog::options() );
}



KoFindReplace::KoFindReplace( QWidget * parent, KoSearchDia * dialog, KoTextView *textView,const QPtrList<KoTextObject> & lstObject)
    : m_find( new KoTextFind( dialog->pattern(), dialog->options(), this, parent ) ),
      m_replace( 0L ),
      m_findDlg( dialog ), // guaranteed to remain alive while we live
      m_replaceDlg( 0L ),
      m_currentTextObj( 0L ),
      m_macroCmd( 0L ),
      m_textView(textView),
      m_lstObject(lstObject),
      m_destroying( false )
{
    connect( m_find, SIGNAL( highlight( const QString &, int, int, const QRect & ) ),
             this, SLOT( highlight( const QString &, int, int, const QRect & ) ) );
}

KoFindReplace::KoFindReplace( QWidget * parent, KoReplaceDia * dialog, KoTextView *textView,const QPtrList<KoTextObject> & lstObject)
    : m_find( 0L ),
      m_replace( new KoTextReplace( dialog->pattern(), dialog->replacement(), dialog->options(), this, parent ) ),
      m_findDlg( 0L ),
      m_replaceDlg( dialog ), // guaranteed to remain alive while we live
      m_currentTextObj( 0L ),
      m_macroCmd( 0L ),
      m_textView(textView),
      m_lstObject(lstObject),
      m_destroying( false )
{
    connect( m_replace, SIGNAL( highlight( const QString &, int, int, const QRect & ) ),
             this, SLOT( highlight( const QString &, int, int, const QRect & ) ) );
    connect( m_replace, SIGNAL( replace( const QString &, int , int, int, const QRect & ) ),
             this, SLOT( replace( const QString &, int , int,int, const QRect & ) ) );
}

KoFindReplace::~KoFindReplace()
{
    kdDebug(32500) << "KWFindReplace::~KWFindReplace m_destroying=" << m_destroying << endl;
    if ( !m_destroying )
    {
        delete m_find;
        delete m_replace;
    }
    // If the KWView was destroyed, it destroyed the child dialog already
}

void KoFindReplace::changeListObject(const QPtrList<KoTextObject> & lstObject)
{
    m_lstObject=lstObject;
    //for kpresenter when we switch in other page we close textview object
    //for the future perhaps add a reference to textview.
    m_textView=0L;
}

bool KoFindReplace::proceed()
{
    if ( m_replace )
        m_replace->resetCounts();
    else
        m_find->resetCounts();
    bool ret = true;
    KoTextObject *firstTextObj=0;
    // Start point
    KoTextParag * firstParag = 0;
    int firstIndex = 0;

    // 'From Cursor' option
    KoTextView * edit = m_textView;
    if ( edit && ( options() & KoFindDialog::FromCursor ) )
    {
        firstParag = edit->cursor()->parag();
        firstIndex = edit->cursor()->index();
        firstTextObj =edit->textObject();
    } // no else here !

    // 'Selected Text' option
    if ( edit && ( options() & KoFindDialog::SelectedText ) )
    {
        firstTextObj =edit->textObject();
        if ( !firstParag ) // not set by 'from cursor'
        {
            KoTextCursor c1 = firstTextObj->textDocument()->selectionStartCursor( KoTextDocument::Standard );
            firstParag = c1.parag();
            firstIndex = c1.index();
        }
        KoTextCursor c2 = firstTextObj->textDocument()->selectionEndCursor( KoTextDocument::Standard );
        //firstTextObj->emitHideCursor();
        // Find in the selection
        ret = findInTextObject( firstTextObj, firstParag, firstIndex, c2.parag(), c2.index() );
        if ( !m_destroying ) {
            //firstTextObj->emitShowCursor();
            firstTextObj->removeHighlight(true);
        }
    }
    else // Not 'find in selection', need to iterate over the framesets
    {
        bool firstTextObjectFound = !firstTextObj;
        QPtrListIterator<KoTextObject> fit(m_lstObject);
        for ( ; fit.current() ; ++fit )
        {
            KoTextObject * fs = fit.current();
            if ( fs /*&& fs->isVisible()*/ )
            {
                KoTextParag * lastParag = fs->textDocument()->lastParag();
                //fs->emitHideCursor();
                if (!firstTextObjectFound && firstTextObj == fs && firstParag)  // first frameset
                {
                    firstTextObjectFound = true;
                    ret = findInTextObject( fs, firstParag, firstIndex, lastParag, lastParag->length()-1 );
                }
                else if ( firstTextObjectFound ) // another frameset -> search entirely
                {
                    ret = findInTextObject( fs, fs->textDocument()->firstParag(), 0, lastParag, lastParag->length()-1 );
                }
                if ( !m_destroying ) {
                    //fs->emitShowCursor();
                    fs->removeHighlight(true);  // we're done with this frameset
                }
                if (!ret || m_destroying) break;      // stop here if the user cancelled
            }
        }
    }
    if(!m_destroying && m_macroCmd)
        emitNewCommand(m_macroCmd);
    m_macroCmd = 0L;
    return ret && !m_destroying;
}

bool KoFindReplace::findInTextObject( KoTextObject * textObj, KoTextParag * firstParag, int firstIndex,
                                    KoTextParag * lastParag, int lastIndex )
{
    m_currentTextObj = textObj;
    // TODO port to KWParagVisitor
    m_currentParag = firstParag;
    m_offset = 0;
    if ( firstParag == lastParag )
    {
        m_offset = firstIndex;
        return process( firstParag->string()->toString().mid( firstIndex, lastIndex-firstIndex ) );
    }
    else
    {
        bool forw = ! ( options() & KoFindDialog::FindBackwards );
        bool ret = true;
        if ( forw )
        {
            m_offset = firstIndex;
            QString str = m_currentParag->string()->toString();
            str.truncate( str.length() - 1 ); // damn trailing space
            ret = process( str.mid( firstIndex ) );
            if (!ret) return false;
        }
        else
        {
            m_currentParag = lastParag;
            ret = process( lastParag->string()->toString().left( lastIndex + 1 ) );
            if (!ret) return false;
        }

        m_currentParag = forw ? firstParag->next() : lastParag->prev();
        m_offset = 0;
        KoTextParag * endParag = forw ? lastParag : firstParag;
        while ( m_currentParag && m_currentParag != endParag )
        {
            QString str = m_currentParag->string()->toString();
            ret = process( str.left(str.length()-1) );
            if (!ret) return false;
            m_currentParag = forw ? m_currentParag->next() : m_currentParag->prev();
        }
        Q_ASSERT( endParag == m_currentParag );
        if ( forw )
        {
            ret = process( lastParag->string()->toString().left( lastIndex + 1 ) );
        } else {
            m_offset = firstIndex;
            QString str = m_currentParag->string()->toString();
            str.truncate( str.length() - 1 ); // damn trailing space
            ret = process( str.mid( firstIndex ) );
        }
        return ret;
    }
}

bool KoFindReplace::process( const QString &_text )
{
    bool ret;
    if ( m_find )
    {
        ret = m_find->find( _text, QRect() );
    }
    else
    {
        QString text( _text );
        ret = m_replace->replace( text, QRect() );
    }

    return ret;
}

long KoFindReplace::options() const
{
    // We need to update m_options from m_replace after each replacement.
    // It can have changed, e.g. if pressing "All" during a replace operation.
    return m_find ? m_find->options() : m_replace->options();
}

// slot connected to the 'highlight' signal
void KoFindReplace::highlight( const QString &, int matchingIndex, int matchingLength, const QRect & )
{
    //kdDebug(32500) << "KoFindReplace::highlight " << matchingIndex << "," << matchingLength << endl;
    highlightPortion(m_currentParag, m_offset + matchingIndex, matchingLength, m_currentTextObj->textDocument());
}

// slot connected to the 'replace' signal
void KoFindReplace::replace( const QString &text, int matchingIndex,
                             int replacementLength, int matchedLength, const QRect &/*expose*/ )
{
    //kdDebug(32500) << "KoFindReplace::replace m_offset=" << m_offset << " matchingIndex=" << matchingIndex << " matchedLength=" << matchedLength << " options=" << options() << endl;
    int index = m_offset + matchingIndex;
    // highlight might not have happened (if 'prompt on replace' is off)
    if ( (options() & KoReplaceDialog::PromptOnReplace) == 0 ) {
        highlightPortion(m_currentParag, index, matchedLength, m_currentTextObj->textDocument());
    }

    KoTextDocument * textdoc = m_currentTextObj->textDocument();
    KoTextCursor cursor( textdoc );
    cursor.setParag( m_currentParag );
    cursor.setIndex( index );

    //reactive spellchecking
    m_currentTextObj->setNeedSpellCheck(true);
    if ( m_replaceDlg->replaceContext()->m_optionsMask )
    {
        replaceWithAttribut( &cursor, index );
    }
    // Don't repaint if we're doing batch changes
    bool repaint = options() & KoReplaceDialog::PromptOnReplace;

    // Grab replacement string
    QString rep = text.mid( matchingIndex, replacementLength );
    KCommand *cmd = m_currentTextObj->replaceSelectionCommand(&cursor, rep, KoTextObject::HighlightSelection, QString::null, repaint );
    if( cmd )
        macroCommand()->addCommand(cmd);
}

void KoFindReplace::replaceWithAttribut( KoTextCursor * cursor, int index )
{
    KoTextFormat * lastFormat = m_currentParag->at( index )->format();
    KoTextFormat * newFormat = new KoTextFormat(*lastFormat);
    KoSearchContext *m_replaceContext = m_replaceDlg->replaceContext();
    int flags = 0;
    if (m_replaceContext->m_optionsMask & KoSearchContext::Bold)
    {
        flags |= KoTextFormat::Bold;
        newFormat->setBold( (bool)(m_replaceContext->m_options & KoSearchContext::Bold) );
    }
    if (m_replaceContext->m_optionsMask & KoSearchContext::Size)
    {
        flags |= KoTextFormat::Size;
        newFormat->setPointSize( KoTextZoomHandler::ptToLayoutUnitPt(m_replaceContext->m_size) );

    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::Family)
    {
        flags |= KoTextFormat::Family;
        newFormat->setFamily( m_replaceContext->m_family );
    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::Color)
    {
        flags |= KoTextFormat::Color;
        newFormat->setColor( m_replaceContext->m_color );
    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::Italic)
    {
        flags |= KoTextFormat::Italic;
        newFormat->setItalic( (bool)(m_replaceContext->m_options & KoSearchContext::Italic) );
    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::Underline)
    {
        flags |= KoTextFormat::ExtendUnderLine;
        newFormat->setUnderlineLineType( m_replaceContext->m_underline );

    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::VertAlign)
    {
        flags |= KoTextFormat::VAlign;
        newFormat->setVAlign( m_replaceContext->m_vertAlign);
    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::StrikeOut)
    {
        flags |= KoTextFormat::StrikeOut;
        newFormat->setStrikeOutLineType( m_replaceContext->m_strikeOut);
    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::BgColor)
    {
        newFormat->setTextBackgroundColor(m_replaceContext->m_backGroundColor);
        flags |=KoTextFormat::TextBackgroundColor;
    }
    if (m_replaceContext->m_optionsMask & KoSearchContext::Shadow)
    {
        flags |= KoTextFormat::ShadowText;
        newFormat->setShadowText( (bool)(m_replaceContext->m_options & KoSearchContext::Shadow) );
    }
    if (m_replaceContext->m_optionsMask & KoSearchContext::WordByWord)
    {
        flags |= KoTextFormat::WordByWord;
        newFormat->setShadowText( (bool)(m_replaceContext->m_options & KoSearchContext::WordByWord) );
    }

    KCommand *cmd=m_currentTextObj->setFormatCommand( cursor, &lastFormat ,newFormat,flags , false, KoTextObject::HighlightSelection );

    if( cmd )
        macroCommand()->addCommand(cmd);
}

KMacroCommand* KoFindReplace::macroCommand()
{
    // Create on demand, to avoid making an empty command
    if(!m_macroCmd)
        m_macroCmd = new KMacroCommand(i18n("Replace text"));
    return m_macroCmd;
}

void KoFindReplace::setActiveWindow()
{
    if ( m_find )
        m_find->setActiveWindow();
    else
        m_replace->setActiveWindow();
}

void KoFindReplace::abort()
{
    // This is called when the KWView is being destroyed.
    // (Not when the user presses Cancel)
    kdDebug(32500) << "KWFindReplace::abort" << endl;
    if ( m_find )
        m_find->abort();
    else
        m_replace->abort();
    m_destroying = true;
    // Note that proceed() won't return until we go back to the event loop

    // ~KWView will run immediately though. And we don't want it to destroy m_findDlg/m_replaceDlg
    // (built on the stack!)
    if ( m_findDlg )
       m_findDlg->reparent( 0, QPoint( 0, 0 ) );
    else if ( m_replaceDlg )
       m_replaceDlg->reparent( 0, QPoint( 0, 0 ) );
}

/*int KoFindReplace::numMatches() const
{
    return m_find->numMatches();
}

int KoFindReplace::numReplacements() const
{
    return m_replace->numReplacements();
}*/

////

KoTextFind::KoTextFind( const QString &pattern, long options, KoFindReplace *_findReplace, QWidget *parent )
    : KoFind( pattern, options, parent),
      m_findReplace( _findReplace)
{
}

KoTextFind::~KoTextFind()
{
}

bool KoTextFind::validateMatch( const QString &text, int index, int matchedlength )
{
    return m_findReplace->validateMatch( text, index, matchedlength );
}

KoTextReplace::KoTextReplace(const QString &pattern, const QString &replacement, long options, KoFindReplace *_findReplace, QWidget *parent )
    : KoReplace( pattern, replacement, options, parent),
      m_findReplace( _findReplace)
{
}

KoTextReplace::~KoTextReplace()
{
}

bool KoTextReplace::validateMatch( const QString &text, int index, int matchedlength )
{
    return m_findReplace->validateMatch( text, index, matchedlength );
}

KoFormatDia::KoFormatDia( QWidget* parent, const QString & _caption, KoSearchContext *_ctx ,  const char* name)
    : KDialogBase( parent, name, true, _caption, Ok|Cancel|User1 |User2 ),
      m_ctx(_ctx)
{
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    setButtonText( KDialogBase::User1, i18n("Reset") );
    setButtonText( KDialogBase::User2, i18n("Clear") );

    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotReset()));
    connect( this, SIGNAL( user2Clicked() ), this, SLOT(slotClear()));

    QGridLayout *m_grid = new QGridLayout( page, 14, 2, 0, 6 );
    m_checkFamily = new QCheckBox( i18n( "Family:" ),page  );
    m_checkSize = new QCheckBox( i18n( "Size:" ), page );
    m_checkColor = new QCheckBox( i18n( "Color:" ), page );
    m_checkBgColor = new QCheckBox( i18n( "Background color:" ), page );
    m_checkBold = new QCheckBox( i18n( "Bold:" ), page );
    m_checkItalic = new QCheckBox( i18n( "Italic:" ),page );
    m_checkShadow = new QCheckBox( i18n( "Shadow:" ), page );
    m_checkWordByWord = new QCheckBox( i18n( "WordByWord:" ), page );

    m_checkUnderline = new QCheckBox( i18n( "Underline:" ), page);
    m_underlineItem = new QComboBox( page );
    m_underlineItem->insertItem( i18n( "Without" ), -1 );
    m_underlineItem->insertItem( i18n( "Simple" ), -1 );
    m_underlineItem->insertItem( i18n( "Double" ), -1 );
    m_underlineItem->insertItem( i18n( "Simple Bold" ), -1 );
    m_underlineItem->setCurrentItem( (int)m_ctx->m_underline );

    m_checkStrikeOut= new QCheckBox( i18n( "Strikeout:" ), page);

    m_strikeOutItem = new QComboBox( page );
    m_strikeOutItem->insertItem( i18n( "Without" ), -1 );
    m_strikeOutItem->insertItem( i18n( "Simple" ), -1 );
    m_strikeOutItem->insertItem( i18n( "Double" ), -1 );
    m_strikeOutItem->insertItem( i18n( "Simple Bold" ), -1 );
    m_strikeOutItem->setCurrentItem( (int)m_ctx->m_strikeOut );


    m_checkFontAttribute = new QCheckBox( i18n( "Attribute:" ), page);
    m_fontAttributeItem = new QComboBox( page );
    m_fontAttributeItem->insertItem( i18n("Without"), -1 );
    m_fontAttributeItem->insertItem( i18n("Uppercase"), -1 );
    m_fontAttributeItem->insertItem( i18n("LowerCase"), -1 );
    m_fontAttributeItem->setCurrentItem( (int)m_ctx->m_attribute );


    m_checkVertAlign = new QCheckBox( i18n( "Vertical alignment:" ), page );

    m_familyItem = new KFontCombo(page);
    m_familyItem->setCurrentFont(m_ctx->m_family);

    m_sizeItem = new QSpinBox( 4, 100, 1, page );
    m_sizeItem->setValue( m_ctx->m_size );

    m_colorItem = new KColorButton( page );
    m_colorItem->setColor( m_ctx->m_color );

    m_bgColorItem = new KColorButton( page );
    m_bgColorItem->setColor( m_ctx->m_backGroundColor);



    QButtonGroup *grpBold = new QButtonGroup( 1, QGroupBox::Vertical, page );
    grpBold->setRadioButtonExclusive( TRUE );
    grpBold->layout();
    m_boldYes=new QRadioButton( i18n("Yes"), grpBold );
    m_boldNo=new QRadioButton( i18n("No"), grpBold );

    QButtonGroup *grpItalic = new QButtonGroup( 1, QGroupBox::Vertical, page );
    grpItalic->setRadioButtonExclusive( TRUE );
    grpItalic->layout();
    m_italicYes=new QRadioButton( i18n("Yes"), grpItalic );
    m_italicNo=new QRadioButton( i18n("No"), grpItalic );

    QButtonGroup *grpShadow = new QButtonGroup( 1, QGroupBox::Vertical, page );
    grpShadow->setRadioButtonExclusive( TRUE );
    grpShadow->layout();
    m_shadowYes=new QRadioButton( i18n("Yes"), grpShadow );
    m_shadowNo=new QRadioButton( i18n("No"), grpShadow );

    QButtonGroup *grpWordByWord = new QButtonGroup( 1, QGroupBox::Vertical, page );
    grpWordByWord->setRadioButtonExclusive( TRUE );
    grpWordByWord->layout();
    m_wordByWordYes=new QRadioButton( i18n("Yes"), grpWordByWord );
    m_wordByWordNo=new QRadioButton( i18n("No"), grpWordByWord );


    m_vertAlignItem = new QComboBox( false, page );
    m_vertAlignItem->insertItem( i18n( "Normal" ), -1 );
    m_vertAlignItem->insertItem( i18n( "Subscript" ), -1 );
    m_vertAlignItem->insertItem( i18n( "Superscript" ), -1 );
    m_vertAlignItem->setCurrentItem( (int)m_ctx->m_vertAlign );

    m_grid->addWidget( m_checkFamily, 1, 0 );
    m_grid->addWidget( m_checkSize, 2, 0 );
    m_grid->addWidget( m_checkColor, 3, 0 );
    m_grid->addWidget( m_checkBgColor, 4, 0);
    m_grid->addWidget( m_checkBold, 5, 0 );
    m_grid->addWidget( m_checkItalic, 6, 0 );
    m_grid->addWidget( m_checkStrikeOut, 7, 0 );
    m_grid->addWidget( m_checkUnderline, 8, 0 );
    m_grid->addWidget( m_checkVertAlign, 9, 0 );
    m_grid->addWidget( m_checkShadow, 10, 0 );
    m_grid->addWidget( m_checkWordByWord, 11, 0 );
    m_grid->addWidget( m_checkFontAttribute, 12, 0 );

    m_grid->addWidget( m_familyItem, 1, 1 );
    m_grid->addWidget( m_sizeItem, 2, 1 );
    m_grid->addWidget( m_colorItem, 3, 1 );
    m_grid->addWidget( m_bgColorItem, 4, 1);
    m_grid->addWidget( grpBold, 5, 1 );
    m_grid->addWidget( grpItalic, 6, 1 );

    m_grid->addWidget( m_strikeOutItem, 7, 1 );
    m_grid->addWidget( m_underlineItem, 8, 1 );

    m_grid->addWidget( m_vertAlignItem, 9, 1 );
    m_grid->addWidget( grpShadow, 10, 1 );
    m_grid->addWidget( grpWordByWord, 11, 1 );

    m_grid->addWidget( m_fontAttributeItem, 12, 1);

    KSeparator *tmpSep = new KSeparator( page );
    m_grid->addMultiCellWidget( tmpSep, 13, 13, 0, 1 );

    // signals and slots connections
    QObject::connect( m_checkFamily, SIGNAL( toggled( bool ) ), m_familyItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkSize, SIGNAL( toggled( bool ) ), m_sizeItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkColor, SIGNAL( toggled( bool ) ), m_colorItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkBgColor, SIGNAL( toggled( bool ) ), m_bgColorItem, SLOT( setEnabled( bool ) ) );

    QObject::connect( m_checkBold, SIGNAL( toggled( bool ) ), m_boldYes, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkItalic, SIGNAL( toggled( bool ) ), m_italicYes, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkStrikeOut, SIGNAL( toggled( bool ) ), m_strikeOutItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkShadow, SIGNAL( toggled( bool ) ), m_shadowYes, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkWordByWord, SIGNAL( toggled( bool ) ), m_wordByWordYes, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkFontAttribute, SIGNAL( toggled( bool ) ), m_fontAttributeItem, SLOT( setEnabled( bool ) ) );



    QObject::connect( m_checkBold, SIGNAL( toggled( bool ) ), m_boldNo, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkItalic, SIGNAL( toggled( bool ) ), m_italicNo, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkShadow, SIGNAL( toggled( bool ) ), m_shadowNo, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkWordByWord, SIGNAL( toggled( bool ) ), m_wordByWordNo, SLOT( setEnabled( bool ) ) );


    QObject::connect( m_checkVertAlign, SIGNAL( toggled( bool ) ), m_vertAlignItem, SLOT( setEnabled( bool ) ) );

    QObject::connect( m_checkUnderline, SIGNAL( toggled( bool ) ), m_underlineItem, SLOT( setEnabled( bool ) ) );

    slotReset();
}

void KoFormatDia::slotClear()
{
    m_ctx->m_optionsMask = 0;
    m_ctx->m_options = 0;
    slotReset();
}

void KoFormatDia::slotReset()
{
    m_checkFamily->setChecked( m_ctx->m_optionsMask & KoSearchContext::Family );
    m_familyItem->setEnabled(m_checkFamily->isChecked());

    m_checkSize->setChecked( m_ctx->m_optionsMask & KoSearchContext::Size );
    m_sizeItem->setEnabled(m_checkSize->isChecked());

    m_checkColor->setChecked( m_ctx->m_optionsMask & KoSearchContext::Color );
    m_colorItem->setEnabled(m_checkColor->isChecked());

    m_checkBgColor->setChecked( m_ctx->m_optionsMask & KoSearchContext::BgColor );
    m_bgColorItem->setEnabled(m_checkBgColor->isChecked());


    m_checkBold->setChecked( m_ctx->m_optionsMask & KoSearchContext::Bold );
    m_boldYes->setEnabled(m_checkBold->isChecked());
    m_boldNo->setEnabled(m_checkBold->isChecked());

    m_checkShadow->setChecked( m_ctx->m_optionsMask & KoSearchContext::Shadow );
    m_shadowYes->setEnabled(m_checkShadow->isChecked());
    m_shadowNo->setEnabled(m_checkShadow->isChecked());

    m_checkWordByWord->setChecked( m_ctx->m_optionsMask & KoSearchContext::WordByWord );
    m_wordByWordYes->setEnabled(m_checkWordByWord->isChecked());
    m_wordByWordNo->setEnabled(m_checkWordByWord->isChecked());


    m_checkStrikeOut->setChecked( m_ctx->m_optionsMask & KoSearchContext::StrikeOut );
    m_strikeOutItem->setEnabled( m_checkStrikeOut->isChecked());


    m_checkItalic->setChecked( m_ctx->m_optionsMask & KoSearchContext::Italic );
    m_italicNo->setEnabled(m_checkItalic->isChecked());
    m_italicYes->setEnabled(m_checkItalic->isChecked());

    m_checkUnderline->setChecked( m_ctx->m_optionsMask & KoSearchContext::Underline );
    m_underlineItem->setEnabled(m_checkUnderline->isChecked());

    m_checkVertAlign->setChecked( m_ctx->m_optionsMask & KoSearchContext::VertAlign );
    m_vertAlignItem->setEnabled(m_checkVertAlign->isChecked());

    m_checkFontAttribute->setChecked( m_ctx->m_optionsMask & KoSearchContext::Attribute );
    m_fontAttributeItem->setEnabled(m_checkFontAttribute->isChecked());



    if (m_ctx->m_options & KoSearchContext::Bold)
        m_boldYes->setChecked( true );
    else
        m_boldNo->setChecked( true );

    if (m_ctx->m_options & KoSearchContext::Italic)
        m_italicYes->setChecked( true );
    else
        m_italicNo->setChecked( true );

    if (m_ctx->m_options & KoSearchContext::Shadow)
        m_shadowYes->setChecked( true );
    else
        m_shadowNo->setChecked( true );

    if (m_ctx->m_options & KoSearchContext::WordByWord)
        m_wordByWordYes->setChecked( true );
    else
        m_wordByWordNo->setChecked( true );

}

void KoFormatDia::ctxOptions( )
{
    long optionsMask = 0;
    long options = 0;
    if ( m_checkFamily->isChecked() )
        optionsMask |= KoSearchContext::Family;
    if ( m_checkSize->isChecked() )
        optionsMask |= KoSearchContext::Size;
    if ( m_checkColor->isChecked() )
        optionsMask |= KoSearchContext::Color;
    if ( m_checkBgColor->isChecked() )
        optionsMask |= KoSearchContext::BgColor;
    if ( m_checkBold->isChecked() )
        optionsMask |= KoSearchContext::Bold;
    if ( m_checkItalic->isChecked() )
        optionsMask |= KoSearchContext::Italic;
    if ( m_checkUnderline->isChecked() )
        optionsMask |= KoSearchContext::Underline;
    if ( m_checkVertAlign->isChecked() )
        optionsMask |= KoSearchContext::VertAlign;
    if ( m_checkStrikeOut->isChecked() )
        optionsMask |= KoSearchContext::StrikeOut;
    if ( m_checkShadow->isChecked() )
        optionsMask |= KoSearchContext::Shadow;
    if ( m_checkWordByWord->isChecked() )
        optionsMask |= KoSearchContext::WordByWord;


    if ( m_boldYes->isChecked() )
        options |= KoSearchContext::Bold;
    if ( m_italicYes->isChecked() )
        options |= KoSearchContext::Italic;
    if ( m_shadowYes->isChecked() )
        options |= KoSearchContext::Shadow;
    if ( m_wordByWordYes->isChecked() )
        options |= KoSearchContext::WordByWord;


    m_ctx->m_optionsMask = optionsMask;
    m_ctx->m_family = m_familyItem->currentText();
    m_ctx->m_size = m_sizeItem->cleanText().toInt();
    m_ctx->m_color = m_colorItem->color();
    m_ctx->m_backGroundColor = m_bgColorItem->color();
    m_ctx->m_vertAlign = (KoTextFormat::VerticalAlignment)m_vertAlignItem->currentItem();
    m_ctx->m_underline = (KoTextFormat::UnderlineLineType)m_underlineItem->currentItem();
    m_ctx->m_strikeOut = (KoTextFormat::StrikeOutLineType)m_strikeOutItem->currentItem();
    m_ctx->m_attribute = ( KoTextFormat::AttributeStyle)m_fontAttributeItem->currentItem();
    m_ctx->m_options = options;
}


bool KoFindReplace::validateMatch( const QString & /*text*/, int index, int matchedlength )
{
    KoSearchContext* searchContext = m_findDlg ? m_findDlg->searchContext() : m_replaceDlg->searchContext();
    bool optionSelected = m_findDlg ? m_findDlg->optionSelected() : (m_replaceDlg->optionSearchSelected() || m_replaceDlg->optionFindSelected());
    if ( !searchContext || !searchContext->m_optionsMask || !optionSelected)
        return true;
    KoTextString * s = currentParag()->string();
    for ( int i = index ; i < index+matchedlength ; ++i )
    {
        KoTextStringChar & ch = s->at(i);
        KoTextFormat *format = ch.format();
        if (searchContext->m_optionsMask & KoSearchContext::Bold)
        {
            if ( (!format->font().bold() && (searchContext->m_options & KoSearchContext::Bold)) || (format->font().bold() && ((searchContext->m_options & KoSearchContext::Bold)==0)))
                return false;
        }
        if (searchContext->m_optionsMask & KoSearchContext::Shadow)
        {
            if ( (!format->shadowText() && (searchContext->m_options & KoSearchContext::Shadow)) || (format->shadowText() && ((searchContext->m_options & KoSearchContext::Shadow)==0)))
                return false;
        }

        if (searchContext->m_optionsMask & KoSearchContext::WordByWord)
        {
            if ( (!format->wordByWord() && (searchContext->m_options & KoSearchContext::WordByWord)) || (format->wordByWord() && ((searchContext->m_options & KoSearchContext::WordByWord)==0)))
                return false;
        }


        if (searchContext->m_optionsMask & KoSearchContext::Size)
        {
            if ( format->font().pointSize () !=  KoTextZoomHandler::ptToLayoutUnitPt(searchContext->m_size))
                return false;
        }
        if ( searchContext->m_optionsMask & KoSearchContext::Family)
        {
            if (format->font().family() != searchContext->m_family)
                return false;
        }
        if ( searchContext->m_optionsMask & KoSearchContext::Color)
        {
            if (format->color() != searchContext->m_color)
                return false;
        }
        if ( searchContext->m_optionsMask & KoSearchContext::BgColor)
        {
            if (format->textBackgroundColor() != searchContext->m_backGroundColor)
                return false;
        }

        if ( searchContext->m_optionsMask & KoSearchContext::Italic)
        {
            if ( (!format->font().italic() && (searchContext->m_options & KoSearchContext::Italic)) || (format->font().italic() && ((searchContext->m_options & KoSearchContext::Italic)==0)))
                return false;

        }
        if ( searchContext->m_optionsMask & KoSearchContext::Underline)
        {
            if ( format->underlineLineType() != searchContext->m_underline )
                return false;
        }
        if ( searchContext->m_optionsMask & KoSearchContext::StrikeOut)
        {
            if ( format->strikeOutLineType() != searchContext->m_strikeOut )
                return false;
        }

        if ( searchContext->m_optionsMask & KoSearchContext::VertAlign)
        {
            if ( format->vAlign() != searchContext->m_vertAlign )
                return false;
        }

        if ( searchContext->m_optionsMask & KoSearchContext::Attribute)
        {
            if ( format->attributeFont() != searchContext->m_attribute )
                return false;
        }

    }
    return true;
}

bool KoFindReplace::shouldRestart()
{
    // TODO use the fact that KoReplace inherits KoFind now ;)
    if ( m_find )
        return m_find->shouldRestart();
    else
        return m_replace->shouldRestart();
}

#include "koSearchDia.moc"
