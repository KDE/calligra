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
KoSearchContext::KoSearchContext()
{
    m_family = "times";
    m_color = Qt::black;
    m_size = 12;
    m_vertAlign = KoTextFormat::AlignNormal;
    m_optionsMask = 0;
    m_options = KoFindDialog::FromCursor | KoReplaceDialog::PromptOnReplace;
}

KoSearchContextUI::KoSearchContextUI( KoSearchContext *ctx, QWidget *parent )
    : QObject(parent), m_ctx(ctx), m_parent(parent)
{
    m_bOptionsShown = false;
    m_btnShowOptions = new QPushButton( i18n("Show Formatting Options"), parent );
    m_btnNoOptions= new QPushButton( i18n("No Options"), parent );
    connect( m_btnShowOptions, SIGNAL( clicked() ), SLOT( slotShowOptions() ) );
    connect( m_btnNoOptions, SIGNAL( clicked() ), SLOT( slotNoOptions() ) );

    m_grid = new QGridLayout( m_parent, 1, 1, 0, 6 );
    m_grid->addWidget( m_btnShowOptions, 0, 0 );
    m_grid->addWidget( m_btnNoOptions, 1, 0 );
    m_btnShowOptions->setEnabled( true );
    m_btnNoOptions->setEnabled( false );
}

void KoSearchContextUI::slotShowOptions()
{
    m_bOptionsShown = true;
    m_btnNoOptions->setEnabled( true );
    KoFormatDia * dlg = new KoFormatDia( m_parent, m_ctx );
    if ( dlg->exec())
    {
        dlg->ctxOptions( );
    }
    delete dlg;
}

void KoSearchContextUI::slotNoOptions()
{
    m_bOptionsShown = false;
    m_btnNoOptions->setEnabled( false);
    m_ctx->m_optionsMask = 0;
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
    m_findUI->setCtxHistory( findHistory() );
    m_replaceUI->setCtxHistory( replacementHistory() );
    m_replaceUI->setCtxOptions( KoReplaceDialog::options() );
}



KoFindReplace::KoFindReplace( QWidget * parent, KoSearchDia * dialog, KoTextView *textView,const QPtrList<KoTextObject> & lstObject)
    : m_find( new KoTextFind( dialog->pattern(), dialog->options(), dialog->searchContext(),this, parent ) ),
      m_replace( 0L ),
      m_findDlg( dialog ), // guaranteed to remain alive while we live
      m_replaceDlg( 0L ),
      m_options( dialog->options() ),
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
      m_replace( new KoTextReplace( dialog->pattern(), dialog->replacement(), dialog->options(), dialog->searchContext(), dialog->replaceContext(), this, parent ) ),
      m_findDlg( 0L ),
      m_replaceDlg( dialog ), // guaranteed to remain alive while we live
      m_options( dialog->options() ),
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
    kdDebug() << "KWFindReplace::~KWFindReplace m_destroying=" << m_destroying << endl;
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

void KoFindReplace::proceed()
{
    KoTextObject *firstTextObj=0;
    // Start point
    KoTextParag * firstParag = 0;
    int firstIndex = 0;

    // 'From Cursor' option
    KoTextView * edit = m_textView;
    if ( edit && ( m_options & KoFindDialog::FromCursor ) )
    {
        firstParag = edit->cursor()->parag();
        firstIndex = edit->cursor()->index();
        firstTextObj =edit->textObject();
    } // no else here !

    // 'Selected Text' option
    if ( edit && ( m_options & KoFindDialog::SelectedText ) )
    {
        firstTextObj =edit->textObject();
        if ( !firstParag ) // not set by 'from cursor'
        {
            KoTextCursor c1 = firstTextObj->textDocument()->selectionStartCursor( KoTextDocument::Standard );
            firstParag = c1.parag();
            firstIndex = c1.index();
        }
        KoTextCursor c2 = firstTextObj->textDocument()->selectionEndCursor( KoTextDocument::Standard );
        // Find in the selection
        findInFrameSet( firstTextObj, firstParag, firstIndex, c2.parag(), c2.index() );
        //todo fix it
        //before we used kwframeset->removeHighlight()
        if ( !m_destroying )
            firstTextObj->removeHighlight();
    }
    else // Not 'find in selection', need to iterate over the framesets
    {
        bool firstFrameSetFound = !firstTextObj;
        QPtrListIterator<KoTextObject> fit(m_lstObject);
        for ( ; fit.current() ; ++fit )
        {
            // Only interested in text framesets
            KoTextObject * fs = fit.current();
            if ( fs /*&& fs->isVisible()*/ )
            {
                KoTextParag * lastParag = fs->textDocument()->lastParag();
                bool ret = true;
                if (!firstFrameSetFound && firstTextObj == fs && firstParag)  // first frameset
                {
                    firstFrameSetFound = true;
                    ret = findInFrameSet( fs, firstParag, firstIndex, lastParag, lastParag->length()-1 );
                }
                else if ( firstFrameSetFound ) // another frameset -> search entirely
                {
                    ret = findInFrameSet( fs, fs->textDocument()->firstParag(), 0, lastParag, lastParag->length()-1 );
                }
                if ( !m_destroying )
                    fs->removeHighlight();  // we're done with this frameset
                if (!ret) break;      // stop here if the user cancelled
            }
        }
    }
    if(!m_destroying && m_macroCmd)
        emitNewCommand(m_macroCmd);
    m_macroCmd= 0L;
    kdDebug() << "KWFindReplace::findInFrameSet done" << endl;
}

bool KoFindReplace::findInFrameSet( KoTextObject * textObj, KoTextParag * firstParag, int firstIndex,
                                    KoTextParag * lastParag, int lastIndex )
{
    // TODO formatting options are not implemented !
    // We need to reimplement what KoFind::find does, and add that.
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
        bool forw = ! ( m_options & KoFindDialog::FindBackwards );
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
    if ( m_find )
        return m_find->find( _text, QRect() );
    else
    {
        QString text( _text );
        return m_replace->replace( text, QRect() );
    }
}

void KoFindReplace::highlight( const QString &, int matchingIndex, int matchingLength, const QRect & )
{
    //kdDebug() << "KoFindReplace::highlight " << matchingIndex << "," << matchingLength << endl;
    highlightPortion(m_currentParag, m_offset + matchingIndex, matchingLength, m_currentTextObj->textDocument());
}

void KoFindReplace::replace( const QString &, int matchingIndex,
                             int /*matchingLength*/ ,int matchedLength,  const QRect &/*expose*/ )
{
    if(!m_macroCmd)
        m_macroCmd=new KMacroCommand(i18n("Insert Replacement"));
    //kdDebug() << "KoFindReplace::replace m_offset=" << m_offset << " matchingIndex=" << matchingIndex << " matchedLength=" << matchedLength << endl;
    int index = m_offset + matchingIndex;
    // highlight might not have happened (if 'prompt on replace' is off)
    highlightPortion(m_currentParag, index, matchedLength, m_currentTextObj->textDocument());

    KoTextDocument * textdoc = m_currentTextObj->textDocument();
    KoTextCursor cursor( textdoc );
    cursor.setParag( m_currentParag );
    cursor.setIndex( index );

    //reactive spellchecking
    m_currentTextObj->setNeedSpellCheck(true);
    if ( m_replace->replaceContext()->m_optionsMask )
    {
        replaceWithAttribut( &cursor, index );
    }
    KCommand *cmd=m_currentTextObj->replaceSelectionCommand(&cursor, m_replaceDlg->replacement(), KoTextObject::HighlightSelection, QString::null );
    if( cmd )
        m_macroCmd->addCommand(cmd);
}

void KoFindReplace::replaceWithAttribut( KoTextCursor * cursor, int index )
{
    KoTextFormat * lastFormat = m_currentParag->at( index )->format();
    KoTextFormat * newFormat = new KoTextFormat(*lastFormat);
    KoSearchContext *m_replaceContext = m_replace->replaceContext();
    int flags = 0;
    if (m_replaceContext->m_optionsMask & KoSearchContext::Bold)
    {
        flags |= KoTextFormat::Bold;
        newFormat->setBold( (bool)(m_replaceContext->m_options & KoSearchContext::Bold) );
    }
    if (m_replaceContext->m_optionsMask & KoSearchContext::Size)
    {
        flags |= KoTextFormat::Size;
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
        flags |= KoTextFormat::Underline;
        newFormat->setUnderline( (bool)(m_replaceContext->m_options & KoSearchContext::Underline) );

    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::VertAlign)
    {
        flags |= KoTextFormat::VAlign;
        newFormat->setVAlign( m_replaceContext->m_vertAlign);
    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::StrikeOut)
    {
        flags |= KoTextFormat::StrikeOut;
        newFormat->setDoubleUnderline( (bool)(m_replaceContext->m_options & KoSearchContext::StrikeOut));
    }
    if ( m_replaceContext->m_optionsMask & KoSearchContext::DoubleUnderLine)
    {
        flags |= KoTextFormat::DoubleUnderline;
        newFormat->setStrikeOut( (bool)(m_replaceContext->m_options & KoSearchContext::DoubleUnderLine));
    }

    KCommand *cmd=m_currentTextObj->setFormatCommand( cursor, &lastFormat ,newFormat,flags , false, KoTextObject::HighlightSelection );

    if( cmd )
        m_macroCmd->addCommand(cmd);
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
    kdDebug(32001) << "KWFindReplace::abort" << endl;
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

KoTextFind::KoTextFind( const QString &pattern, long options, KoSearchContext * _searchContext, KoFindReplace *_findReplace, QWidget *parent )
    : KoFind( pattern, options, parent),
      m_searchContext(_searchContext),
      m_findReplace( _findReplace)
{
}

KoTextFind::~KoTextFind()
{
}

bool KoTextFind::validateMatch( const QString &/*text*/, int index, int matchedlength )
{
    if ( !m_searchContext || !m_searchContext->m_optionsMask)
        return true;
    KoTextString * s = m_findReplace->currentParag()->string();
    for ( int i = index ; i < index+matchedlength ; ++i )
    {
        KoTextStringChar & ch = s->at(i);
        KoTextFormat *format = ch.format();
        if (m_searchContext->m_optionsMask & KoSearchContext::Bold)
        {
            if ( (!format->font().bold() && (m_searchContext->m_options & KoSearchContext::Bold)) || (format->font().bold() && ((m_searchContext->m_options & KoSearchContext::Bold)==0)))
                return false;
        }
        if (m_searchContext->m_optionsMask & KoSearchContext::Size)
        {
            if ( format->font().pointSize () != m_searchContext->m_size)
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::Family)
        {
            if (format->font().family() != m_searchContext->m_family)
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::Color)
        {
            if (format->color() != m_searchContext->m_color)
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::BgColor)
        {
            if (format->textBackgroundColor() != m_searchContext->m_backGroungColor)
                return false;
        }

        if ( m_searchContext->m_optionsMask & KoSearchContext::Italic)
        {
            if ( (!format->font().italic() && (m_searchContext->m_options & KoSearchContext::Italic)) || (format->font().italic() && ((m_searchContext->m_options & KoSearchContext::Italic)==0)))
                return false;

        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::Underline)
        {
            if ( (!format->font().underline() && (m_searchContext->m_options & KoSearchContext::Underline)) || (format->font().underline() && ((m_searchContext->m_options & KoSearchContext::Underline)==0)))
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::StrikeOut)
        {
            if ( (!format->font().strikeOut() && (m_searchContext->m_options & KoSearchContext::StrikeOut)) || (format->font().strikeOut() && ((m_searchContext->m_options & KoSearchContext::StrikeOut)==0)))
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::DoubleUnderLine)
        {
            if ( (!format->doubleUnderline() && (m_searchContext->m_options & KoSearchContext::DoubleUnderLine)) || (format->doubleUnderline() && ((m_searchContext->m_options & KoSearchContext::DoubleUnderLine)==0)))
                return false;
        }


        if ( m_searchContext->m_optionsMask & KoSearchContext::VertAlign)
        {
            if ( format->vAlign() != m_searchContext->m_vertAlign )
                return false;
        }
    }
    return true;
}

KoTextReplace::KoTextReplace(const QString &pattern, const QString &replacement, long options, KoSearchContext * _searchContext, KoSearchContext *_replaceContext, KoFindReplace *_findReplace, QWidget *parent )
    : KoReplace( pattern,replacement, options, parent),
      m_searchContext(_searchContext),
      m_replaceContext(_replaceContext),
      m_findReplace( _findReplace)
{
}

KoTextReplace::~KoTextReplace()
{
}

bool KoTextReplace::validateMatch( const QString &/*text*/, int index, int matchedlength )
{
    if ( !m_searchContext || !m_searchContext->m_optionsMask)
        return true;
    KoTextString * s = m_findReplace->currentParag()->string();
    for ( int i = index ; i < index+matchedlength ; ++i )
    {
        KoTextStringChar & ch = s->at(i);
        KoTextFormat *format = ch.format();
        if (m_searchContext->m_optionsMask & KoSearchContext::Bold)
        {
            if ( (!format->font().bold() && (m_searchContext->m_options & KoSearchContext::Bold)) || (format->font().bold() && ((m_searchContext->m_options & KoSearchContext::Bold)==0)))
                return false;
        }
        if (m_searchContext->m_optionsMask & KoSearchContext::Size)
        {
            if ( format->font().pointSize () != m_searchContext->m_size)
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::Family)
        {
            if (format->font().family() != m_searchContext->m_family)
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::Color)
        {
            if (format->color() != m_searchContext->m_color)
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::BgColor)
        {
            if (format->textBackgroundColor() != m_searchContext->m_backGroungColor)
                return false;
        }

        if ( m_searchContext->m_optionsMask & KoSearchContext::Italic)
        {
            if ( (!format->font().italic() && (m_searchContext->m_options & KoSearchContext::Italic)) || (format->font().italic() && ((m_searchContext->m_options & KoSearchContext::Italic)==0)))
                return false;

        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::Underline)
        {
            if ( (!format->font().underline() && (m_searchContext->m_options & KoSearchContext::Underline)) || (format->font().underline() && ((m_searchContext->m_options & KoSearchContext::Underline)==0)))
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::StrikeOut)
        {
            if ( (!format->font().strikeOut() && (m_searchContext->m_options & KoSearchContext::StrikeOut)) || (format->font().strikeOut() && ((m_searchContext->m_options & KoSearchContext::StrikeOut)==0)))
                return false;
        }
        if ( m_searchContext->m_optionsMask & KoSearchContext::DoubleUnderLine)
        {
            if ( (!format->doubleUnderline() && (m_searchContext->m_options & KoSearchContext::DoubleUnderLine)) || (format->doubleUnderline() && ((m_searchContext->m_options & KoSearchContext::DoubleUnderLine)==0)))
                return false;
        }

        if ( m_searchContext->m_optionsMask & KoSearchContext::VertAlign)
        {
            if ( format->vAlign() != m_searchContext->m_vertAlign )
                return false;
        }

    }
    return true;

}

KoFormatDia::KoFormatDia( QWidget* parent, KoSearchContext *_ctx ,  const char* name)
    : KDialogBase( parent, name, true, i18n("Formatting Options"), Ok|Cancel|User1 ),
      m_ctx(_ctx)
{
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    setButtonText( KDialogBase::User1, i18n("Reset") );

    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotReset()));

    QGridLayout *m_grid = new QGridLayout( page, 12, 2, 0, 6 );
    m_checkFamily = new QCheckBox( i18n( "Family" ),page  );
    m_checkSize = new QCheckBox( i18n( "Size" ), page );
    m_checkColor = new QCheckBox( i18n( "Color" ), page );
    m_checkBgColor = new QCheckBox( i18n( "Background color" ), page );
    m_checkBold = new QCheckBox( i18n( "Bold" ), page );
    m_checkItalic = new QCheckBox( i18n( "Italic" ),page );
    m_checkUnderline = new QCheckBox( i18n( "Underline" ), page);
    m_checkStrikeOut = new QCheckBox( i18n( "Strikeout" ), page);
    m_checkDoubleLine = new QCheckBox( i18n( "Double underline" ), page);


    m_checkVertAlign = new QCheckBox( i18n( "Vertical alignment" ), page );

    m_familyItem = new KFontCombo(page);
    m_familyItem->setCurrentFont(m_ctx->m_family);

    m_sizeItem = new QSpinBox( 4, 100, 1, page );
    m_sizeItem->setValue( m_ctx->m_size );

    m_colorItem = new KColorButton( page );
    m_colorItem->setColor( m_ctx->m_color );

    m_bgColorItem = new KColorButton( page );
    m_bgColorItem->setColor( m_ctx->m_backGroungColor);



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

    QButtonGroup *grpUnderline = new QButtonGroup( 1, QGroupBox::Vertical, page );
    grpUnderline->setRadioButtonExclusive( TRUE );
    grpUnderline->layout();
    m_underlineYes=new QRadioButton( i18n("Yes"), grpUnderline );
    m_underlineNo=new QRadioButton( i18n("No"), grpUnderline );

    QButtonGroup *grpStrikeOut = new QButtonGroup( 1, QGroupBox::Vertical, page );
    grpStrikeOut->setRadioButtonExclusive( TRUE );
    grpStrikeOut->layout();
    m_strikeOutYes=new QRadioButton( i18n("Yes"), grpStrikeOut );
    m_strikeOutNo=new QRadioButton( i18n("No"), grpStrikeOut );

    QButtonGroup *grpDoubleLine = new QButtonGroup( 1, QGroupBox::Vertical, page );
    grpDoubleLine->setRadioButtonExclusive( TRUE );
    grpDoubleLine->layout();
    m_doubleLineYes=new QRadioButton( i18n("Yes"), grpDoubleLine );
    m_doubleLineNo=new QRadioButton( i18n("No"), grpDoubleLine );

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
    m_grid->addWidget( m_checkUnderline, 7, 0 );
    m_grid->addWidget( m_checkStrikeOut, 8, 0 );
    m_grid->addWidget( m_checkDoubleLine, 9, 0 );
    m_grid->addWidget( m_checkVertAlign, 10, 0 );

    m_grid->addWidget( m_familyItem, 1, 1 );
    m_grid->addWidget( m_sizeItem, 2, 1 );
    m_grid->addWidget( m_colorItem, 3, 1 );
    m_grid->addWidget( m_bgColorItem, 4, 1);
    m_grid->addWidget( grpBold, 5, 1 );
    m_grid->addWidget( grpItalic, 6, 1 );
    m_grid->addWidget( grpUnderline, 7, 1 );
    m_grid->addWidget( grpStrikeOut, 8, 1 );
    m_grid->addWidget( grpDoubleLine, 9, 1 );
    m_grid->addWidget( m_vertAlignItem, 10, 1 );

    KSeparator *tmpSep = new KSeparator( page );
    m_grid->addMultiCellWidget( tmpSep, 11, 11, 0, 1 );

    // signals and slots connections
    QObject::connect( m_checkFamily, SIGNAL( toggled( bool ) ), m_familyItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkSize, SIGNAL( toggled( bool ) ), m_sizeItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkColor, SIGNAL( toggled( bool ) ), m_colorItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkBgColor, SIGNAL( toggled( bool ) ), m_bgColorItem, SLOT( setEnabled( bool ) ) );

    QObject::connect( m_checkBold, SIGNAL( toggled( bool ) ), m_boldYes, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkItalic, SIGNAL( toggled( bool ) ), m_italicYes, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkUnderline, SIGNAL( toggled( bool ) ), m_underlineYes, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkStrikeOut, SIGNAL( toggled( bool ) ), m_strikeOutYes, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkDoubleLine, SIGNAL( toggled( bool ) ), m_doubleLineYes, SLOT( setEnabled( bool ) ) );

    QObject::connect( m_checkBold, SIGNAL( toggled( bool ) ), m_boldNo, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkItalic, SIGNAL( toggled( bool ) ), m_italicNo, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkUnderline, SIGNAL( toggled( bool ) ), m_underlineNo, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkStrikeOut, SIGNAL( toggled( bool ) ), m_strikeOutNo, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkDoubleLine, SIGNAL( toggled( bool ) ), m_doubleLineNo, SLOT( setEnabled( bool ) ) );


    QObject::connect( m_checkVertAlign, SIGNAL( toggled( bool ) ), m_vertAlignItem, SLOT( setEnabled( bool ) ) );
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

    m_checkStrikeOut->setChecked( m_ctx->m_optionsMask & KoSearchContext::StrikeOut );
    m_strikeOutYes->setEnabled( m_checkStrikeOut->isChecked());
    m_strikeOutNo->setEnabled( m_checkStrikeOut->isChecked());

    m_checkDoubleLine->setChecked( m_ctx->m_optionsMask & KoSearchContext::DoubleUnderLine );
    m_doubleLineNo->setEnabled( m_checkDoubleLine->isChecked());
    m_doubleLineYes->setEnabled( m_checkDoubleLine->isChecked());

    m_checkItalic->setChecked( m_ctx->m_optionsMask & KoSearchContext::Italic );
    m_italicNo->setEnabled(m_checkItalic->isChecked());
    m_italicYes->setEnabled(m_checkItalic->isChecked());

    m_checkUnderline->setChecked( m_ctx->m_optionsMask & KoSearchContext::Underline );
    m_underlineNo->setEnabled(m_checkUnderline->isChecked());
    m_underlineYes->setEnabled(m_checkUnderline->isChecked());

    m_checkVertAlign->setChecked( m_ctx->m_optionsMask & KoSearchContext::VertAlign );
    m_vertAlignItem->setEnabled(m_checkVertAlign->isChecked());

    if (m_ctx->m_options & KoSearchContext::Bold)
        m_boldYes->setChecked( true );
    else
        m_boldNo->setChecked( true );

    if (m_ctx->m_options & KoSearchContext::Italic)
        m_italicYes->setChecked( true );
    else
        m_italicNo->setChecked( true );

    if (m_ctx->m_options & KoSearchContext::Underline )
        m_underlineYes->setChecked( true );
    else
        m_underlineNo->setChecked( true );
    if (m_ctx->m_options & KoSearchContext::StrikeOut )
        m_strikeOutYes->setChecked( true);
    else
        m_strikeOutNo->setChecked( true );
    if (m_ctx->m_options & KoSearchContext::DoubleUnderLine)
        m_doubleLineYes->setChecked( true);
    else
        m_doubleLineNo->setChecked( true);
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
    if ( m_checkDoubleLine->isChecked() )
        optionsMask |= KoSearchContext::DoubleUnderLine;


    if ( m_boldYes->isChecked() )
        options |= KoSearchContext::Bold;
    if ( m_italicYes->isChecked() )
        options |= KoSearchContext::Italic;
    if ( m_underlineYes->isChecked() )
        options |= KoSearchContext::Underline;
    if ( m_strikeOutYes->isChecked() )
        options |= KoSearchContext::StrikeOut;
    if ( m_doubleLineYes->isChecked() )
        options |= KoSearchContext::DoubleUnderLine;


    m_ctx->m_optionsMask = optionsMask;
    m_ctx->m_family = m_familyItem->currentText();
    m_ctx->m_size = m_sizeItem->cleanText().toInt();
    m_ctx->m_color = m_colorItem->color();
    m_ctx->m_backGroungColor = m_bgColorItem->color();
    m_ctx->m_vertAlign = (KoTextFormat::VerticalAlignment)m_vertAlignItem->currentItem();
    m_ctx->m_options = options;
}


#include "koSearchDia.moc"
