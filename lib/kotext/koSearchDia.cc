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

KoSearchContext::KoSearchContext()
{
    m_family = "times";
    m_color = Qt::black;
    m_size = 12;
    m_vertAlign = QTextFormat::AlignNormal;
    m_optionsMask = 0;
    m_options = KoFindDialog::FromCursor | KoReplaceDialog::PromptOnReplace;
}

KoSearchContextUI::KoSearchContextUI( KoSearchContext *ctx, QWidget *parent )
    : QObject(parent), m_ctx(ctx), m_parent(parent)
{
    m_bOptionsShown = false;
    m_btnShowOptions = new QPushButton( i18n("Show Formatting Options"), parent );
    connect( m_btnShowOptions, SIGNAL( clicked() ), SLOT( slotShowOptions() ) );
    m_grid = new QGridLayout( m_parent, 1, 1, 0, 6 );
    m_grid->addWidget( m_btnShowOptions, 0, 0 );
    m_checkFamily = 0L;

    // ### currently not implemented
    m_btnShowOptions->setEnabled( false );
}

void KoSearchContextUI::slotShowOptions()
{
    m_bOptionsShown = true;
    delete m_btnShowOptions;
    delete m_grid;
    m_grid = new QGridLayout( m_parent, 7, 2, 0, 6 );
    m_checkFamily = new QCheckBox( i18n( "Family" ), m_parent );
    m_checkSize = new QCheckBox( i18n( "Size" ), m_parent );
    m_checkColor = new QCheckBox( i18n( "Color" ), m_parent );
    m_checkBold = new QCheckBox( i18n( "Bold" ), m_parent );
    m_checkItalic = new QCheckBox( i18n( "Italic" ), m_parent );
    m_checkUnderline = new QCheckBox( i18n( "Underline" ), m_parent );
    m_checkVertAlign = new QCheckBox( i18n( "Vertical Alignment" ), m_parent );

    m_familyItem = new QComboBox( true, m_parent );
    m_familyItem->insertStringList( m_ctx->m_family );
    for ( int j = 0; j < m_familyItem->count(); j++ )
        if ( m_familyItem->text( j ) == m_ctx->m_family )
        {
            m_familyItem->setCurrentItem( j );
            break;
        }

    m_sizeItem = new QSpinBox( 4, 100, 1, m_parent );
    m_sizeItem->setValue( m_ctx->m_size );

    m_colorItem = new KColorButton( m_parent );
    m_colorItem->setColor( m_ctx->m_color );

    m_boldItem = new QCheckBox( i18n( "Bold" ), m_parent );
    m_italicItem = new QCheckBox( i18n( "Italic" ), m_parent );
    m_underlineItem = new QCheckBox( i18n( "Underline" ), m_parent );

    m_vertAlignItem = new QComboBox( false, m_parent );
    m_vertAlignItem->insertItem( i18n( "Normal" ), -1 );
    m_vertAlignItem->insertItem( i18n( "Subscript" ), -1 );
    m_vertAlignItem->insertItem( i18n( "Superscript" ), -1 );
    m_vertAlignItem->setCurrentItem( (int)m_ctx->m_vertAlign );

    m_grid->addWidget( m_checkFamily, 1, 0 );
    m_grid->addWidget( m_checkSize, 2, 0 );
    m_grid->addWidget( m_checkColor, 3, 0 );
    m_grid->addWidget( m_checkBold, 4, 0 );
    m_grid->addWidget( m_checkItalic, 5, 0 );
    m_grid->addWidget( m_checkUnderline, 6, 0 );
    m_grid->addWidget( m_checkVertAlign, 7, 0 );
    m_grid->addWidget( m_familyItem, 1, 1 );
    m_grid->addWidget( m_sizeItem, 2, 1 );
    m_grid->addWidget( m_colorItem, 3, 1 );
    m_grid->addWidget( m_boldItem, 4, 1 );
    m_grid->addWidget( m_italicItem, 5, 1 );
    m_grid->addWidget( m_underlineItem, 6, 1 );
    m_grid->addWidget( m_vertAlignItem, 7, 1 );

    m_checkFamily->show();
    m_checkSize->show();
    m_checkColor->show();
    m_checkBold->show();
    m_checkItalic->show();
    m_checkUnderline->show();
    m_checkVertAlign->show();
    m_familyItem->show();
    m_sizeItem->show();
    m_colorItem->show();
    m_boldItem->show();
    m_italicItem->show();
    m_underlineItem->show();
    m_vertAlignItem->show();

    // signals and slots connections
    QObject::connect( m_checkFamily, SIGNAL( toggled( bool ) ), m_familyItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkSize, SIGNAL( toggled( bool ) ), m_sizeItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkColor, SIGNAL( toggled( bool ) ), m_colorItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkBold, SIGNAL( toggled( bool ) ), m_boldItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkItalic, SIGNAL( toggled( bool ) ), m_italicItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkUnderline, SIGNAL( toggled( bool ) ), m_underlineItem, SLOT( setEnabled( bool ) ) );
    QObject::connect( m_checkVertAlign, SIGNAL( toggled( bool ) ), m_vertAlignItem, SLOT( setEnabled( bool ) ) );

    m_checkFamily->setChecked( m_ctx->m_optionsMask & KoSearchContext::Family );
    m_familyItem->setEnabled(m_checkFamily->isChecked());

    m_checkSize->setChecked( m_ctx->m_optionsMask & KoSearchContext::Size );
    m_sizeItem->setEnabled(m_checkSize->isChecked());

    m_checkColor->setChecked( m_ctx->m_optionsMask & KoSearchContext::Color );
    m_colorItem->setEnabled(m_checkColor->isChecked());

    m_checkBold->setChecked( m_ctx->m_optionsMask & KoSearchContext::Bold );
    m_boldItem->setEnabled(m_checkBold->isChecked());

    m_checkItalic->setChecked( m_ctx->m_optionsMask & KoSearchContext::Italic );
    m_italicItem->setEnabled(m_checkItalic->isChecked());

    m_checkUnderline->setChecked( m_ctx->m_optionsMask & KoSearchContext::Underline );
    m_underlineItem->setEnabled(m_checkUnderline->isChecked());

    m_checkVertAlign->setChecked( m_ctx->m_optionsMask & KoSearchContext::VertAlign );
    m_vertAlignItem->setEnabled(m_checkVertAlign->isChecked());

    m_boldItem->setChecked( m_ctx->m_options & KoSearchContext::Bold );
    m_italicItem->setChecked( m_ctx->m_options & KoSearchContext::Italic );
    m_underlineItem->setChecked( m_ctx->m_options & KoSearchContext::Underline );
}

void KoSearchContextUI::setCtxOptions( long options )
{
    if ( m_bOptionsShown )
    {
        long optionsMask = 0;

        if ( m_checkFamily->isChecked() )
            optionsMask |= KoSearchContext::Family;
        if ( m_checkSize->isChecked() )
            optionsMask |= KoSearchContext::Size;
        if ( m_checkColor->isChecked() )
            optionsMask |= KoSearchContext::Color;
        if ( m_checkBold->isChecked() )
            optionsMask |= KoSearchContext::Bold;
        if ( m_checkItalic->isChecked() )
            optionsMask |= KoSearchContext::Italic;
        if ( m_checkUnderline->isChecked() )
            optionsMask |= KoSearchContext::Underline;
        if ( m_checkVertAlign->isChecked() )
            optionsMask |= KoSearchContext::VertAlign;

        if ( m_boldItem->isChecked() )
            options |= KoSearchContext::Bold;
        if ( m_italicItem->isChecked() )
            options |= KoSearchContext::Italic;
        if ( m_underlineItem->isChecked() )
            options |= KoSearchContext::Underline;
        m_ctx->m_optionsMask = optionsMask;
        m_ctx->m_family = m_familyItem->currentText();
        m_ctx->m_size = m_sizeItem->cleanText().toInt();
        m_ctx->m_color = m_colorItem->color();
        m_ctx->m_vertAlign = (QTextFormat::VerticalAlignment)m_vertAlignItem->currentItem();
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
}



KoFindReplace::KoFindReplace( QWidget * parent, KoSearchDia * dialog, KoTextView *textView,const QPtrList<KoTextObject> & lstObject)
    : m_find( new KoFind( dialog->pattern(), dialog->options(), parent ) ),
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
      m_replace( new KoReplace( dialog->pattern(), dialog->replacement(), dialog->options(), parent ) ),
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
    Qt3::QTextParag * firstParag = 0;
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
            QTextCursor c1 = firstTextObj->textDocument()->selectionStartCursor( KoTextDocument::Standard );
            firstParag = c1.parag();
            firstIndex = c1.index();
        }
        QTextCursor c2 = firstTextObj->textDocument()->selectionEndCursor( KoTextDocument::Standard );
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
                Qt3::QTextParag * lastParag = fs->textDocument()->lastParag();
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

    kdDebug() << "KWFindReplace::findInFrameSet done" << endl;
}

bool KoFindReplace::findInFrameSet( KoTextObject * textObj, Qt3::QTextParag * firstParag, int firstIndex,
                                    Qt3::QTextParag * lastParag, int lastIndex )
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
        Qt3::QTextParag * endParag = forw ? lastParag : firstParag;
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
    //kdDebug() << "KWFind::highlight " << matchingIndex << "," << matchingLength << endl;
#if 0
    m_currentTextObj->highlightPortion( m_currentParag, m_offset + matchingIndex, matchingLength);
    m_currentFrameSet->highlightPortion( m_currentParag, m_offset + matchingIndex, matchingLength, m_canvas );
#endif
    highlightPortion(m_currentParag, m_offset + matchingIndex, matchingLength, m_currentTextObj->textDocument());
}

void KoFindReplace::replace( const QString &, int matchingIndex,
                             int /*matchingLength*/ ,int matchedLength,  const QRect &/*expose*/ )
{
    if(!m_macroCmd)
        m_macroCmd=new KMacroCommand(i18n("Insert Replacement"));
    int index = m_offset + matchingIndex;
    // highlight might not have happened (if 'prompt on replace' is off)
#if 0
    m_currentFrameSet->highlightPortion( m_currentParag, index, matchedLength, m_canvas );
    m_currentTextObj->highlightPortion( m_currentParag, index, matchedLength);

#endif
    highlightPortion(m_currentParag, index,matchedLength , m_currentTextObj->textDocument());

    KoTextDocument * textdoc = m_currentTextObj->textDocument();
    QTextCursor cursor( textdoc );
    cursor.setParag( m_currentParag );
    cursor.setIndex( index );

    //reactive spellchecking
    m_currentTextObj->setNeedSpellCheck(true);

    m_macroCmd->addCommand(m_currentTextObj->replaceSelectionCommand(
        &cursor, m_replaceDlg->replacement(), KoTextObject::HighlightSelection, QString::null ));
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

#include "koSearchDia.moc"
