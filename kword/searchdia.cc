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

#include <kcolorbtn.h>
#include <klocale.h>
#include <koFind.h>
#include <koReplace.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qwidget.h>
#include <kdebug.h>
#include "searchdia.h"
#include "kwcanvas.h"
#include "kwdoc.h"
#include "kwtextframeset.h"
#include "kwformat.h"

KWSearchContext::KWSearchContext()
{
    m_family = "times";
    m_color = Qt::black;
    m_size = 12;
    m_vertAlign = QTextFormat::AlignNormal;
    m_optionsMask = 0;
    m_options = 0;
}

KWSearchContextUI::KWSearchContextUI( KWSearchContext *ctx, QWidget *parent )
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

void KWSearchContextUI::slotShowOptions()
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

    m_checkFamily->setChecked( m_ctx->m_optionsMask & KWSearchContext::Family );
    m_familyItem->setEnabled(m_checkFamily->isChecked());

    m_checkSize->setChecked( m_ctx->m_optionsMask & KWSearchContext::Size );
    m_sizeItem->setEnabled(m_checkSize->isChecked());

    m_checkColor->setChecked( m_ctx->m_optionsMask & KWSearchContext::Color );
    m_colorItem->setEnabled(m_checkColor->isChecked());

    m_checkBold->setChecked( m_ctx->m_optionsMask & KWSearchContext::Bold );
    m_boldItem->setEnabled(m_checkBold->isChecked());

    m_checkItalic->setChecked( m_ctx->m_optionsMask & KWSearchContext::Italic );
    m_italicItem->setEnabled(m_checkItalic->isChecked());

    m_checkUnderline->setChecked( m_ctx->m_optionsMask & KWSearchContext::Underline );
    m_underlineItem->setEnabled(m_checkUnderline->isChecked());

    m_checkVertAlign->setChecked( m_ctx->m_optionsMask & KWSearchContext::VertAlign );
    m_vertAlignItem->setEnabled(m_checkVertAlign->isChecked());

    m_boldItem->setChecked( m_ctx->m_options & KWSearchContext::Bold );
    m_italicItem->setChecked( m_ctx->m_options & KWSearchContext::Italic );
    m_underlineItem->setChecked( m_ctx->m_options & KWSearchContext::Underline );
}

void KWSearchContextUI::setCtxOptions( long options )
{
    if ( m_bOptionsShown )
    {
        long optionsMask = 0;

        if ( m_checkFamily->isChecked() )
            optionsMask |= KWSearchContext::Family;
        if ( m_checkSize->isChecked() )
            optionsMask |= KWSearchContext::Size;
        if ( m_checkColor->isChecked() )
            optionsMask |= KWSearchContext::Color;
        if ( m_checkBold->isChecked() )
            optionsMask |= KWSearchContext::Bold;
        if ( m_checkItalic->isChecked() )
            optionsMask |= KWSearchContext::Italic;
        if ( m_checkUnderline->isChecked() )
            optionsMask |= KWSearchContext::Underline;
        if ( m_checkVertAlign->isChecked() )
            optionsMask |= KWSearchContext::VertAlign;

        if ( m_boldItem->isChecked() )
            options |= KWSearchContext::Bold;
        if ( m_italicItem->isChecked() )
            options |= KWSearchContext::Italic;
        if ( m_underlineItem->isChecked() )
            options |= KWSearchContext::Underline;
        m_ctx->m_optionsMask = optionsMask;
        m_ctx->m_family = m_familyItem->currentText();
        m_ctx->m_size = m_sizeItem->cleanText().toInt();
        m_ctx->m_color = m_colorItem->color();
        m_ctx->m_vertAlign = (QTextFormat::VerticalAlignment)m_vertAlignItem->currentItem();
    }
    m_ctx->m_options = options;
}

void KWSearchContextUI::setCtxHistory( const QStringList & history )
{
    m_ctx->m_strings = history;
}

/*================================================================*/
KWSearchDia::KWSearchDia( KWCanvas *canvas, const char *name, KWSearchContext *find ):
    KoFindDialog( canvas, name, find->m_options, find->m_strings )
{
    // The dialog extension.
    m_findUI = new KWSearchContextUI( find, findExtension() );
    // Look whether we have a selection
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(canvas->currentFrameSetEdit());
    setHasSelection( edit && static_cast<KWTextFrameSet *>(edit->frameSet())->hasSelection() );
}

void KWSearchDia::slotOk()
{
    KoFindDialog::slotOk();

    // Save the current state back into the context required.
    m_findUI->setCtxOptions( options() );
    m_findUI->setCtxHistory( findHistory() );
}

KWReplaceDia::KWReplaceDia( KWCanvas *canvas, const char *name, KWSearchContext *find, KWSearchContext *replace ):
    KoReplaceDialog( canvas, name, find->m_options, find->m_strings, replace->m_strings )
{
    // The dialog extension.
    m_findUI = new KWSearchContextUI( find, findExtension() );
    m_replaceUI = new KWSearchContextUI( replace, replaceExtension() );
    // Look whether we have a selection
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(canvas->currentFrameSetEdit());
    setHasSelection( edit && static_cast<KWTextFrameSet *>(edit->frameSet())->hasSelection() );
}

void KWReplaceDia::slotOk()
{
    KoReplaceDialog::slotOk();

    // Save the current state back into the context required.
    m_findUI->setCtxOptions( KoReplaceDialog::options() );
    m_findUI->setCtxHistory( findHistory() );
    m_replaceUI->setCtxHistory( replacementHistory() );
}


/*================================================================*/

KWFindReplace::KWFindReplace( KWCanvas * canvas, KWSearchDia * dialog )
    : m_find( new KoFind( dialog->pattern(), dialog->options(), canvas ) ),
      m_replace( 0L ),
      m_findDlg( dialog ), // guaranteed to remain alive while we live
      m_replaceDlg( 0L ),
      m_options( dialog->options() ),
      m_canvas( canvas )
{
    connect( m_find, SIGNAL( highlight( const QString &, int, int, const QRect & ) ),
             this, SLOT( highlight( const QString &, int, int, const QRect & ) ) );
    m_currentFrameSet = 0L;
}

KWFindReplace::KWFindReplace( KWCanvas * canvas, KWReplaceDia * dialog )
    : m_find( 0L ),
      m_replace( new KoReplace( dialog->pattern(), dialog->replacement(), dialog->options(), canvas ) ),
      m_findDlg( 0L ),
      m_replaceDlg( dialog ), // guaranteed to remain alive while we live
      m_options( dialog->options() ),
      m_canvas( canvas )
{
    connect( m_replace, SIGNAL( highlight( const QString &, int, int, const QRect & ) ),
             this, SLOT( highlight( const QString &, int, int, const QRect & ) ) );
    connect( m_replace, SIGNAL( replace( const QString &, int , int, const QRect & ) ),
             this, SLOT( replace( const QString &, int , int, const QRect & ) ) );
    m_currentFrameSet = 0L;
}

KWFindReplace::~KWFindReplace()
{
    delete m_find;
    delete m_replace;
}

void KWFindReplace::proceed()
{
    KWTextFrameSet * firstFrameSet = 0;
    // Start point
    QTextParag * firstParag = 0;
    int firstIndex = 0;

    // 'From Cursor' option
    KWTextFrameSetEdit * edit = dynamic_cast<KWTextFrameSetEdit *>(m_canvas->currentFrameSetEdit());
    if ( edit && ( m_options & KoFindDialog::FromCursor ) )
    {
        firstParag = edit->getCursor()->parag();
        firstIndex = edit->getCursor()->index();
        firstFrameSet = static_cast<KWTextFrameSet *>(edit->frameSet());
    } // no else here !

    // 'Selected Text' option
    if ( edit && ( m_options & KoFindDialog::SelectedText ) )
    {
        firstFrameSet = static_cast<KWTextFrameSet *>(edit->frameSet());
        if ( !firstParag ) // no set by 'from cursor'
        {
            QTextCursor c1 = firstFrameSet->textDocument()->selectionStartCursor( QTextDocument::Standard );
            firstParag = c1.parag();
            firstIndex = c1.index();
        }
        QTextCursor c2 = firstFrameSet->textDocument()->selectionEndCursor( QTextDocument::Standard );
        // Find in the selection
        findInFrameSet( firstFrameSet, firstParag, firstIndex, c2.parag(), c2.index() );
        firstFrameSet->removeHighlight();
    }
    else // Not 'find in selection', need to iterate over the framesets
    {
        bool firstFrameSetFound = !firstFrameSet;
        QListIterator<KWFrameSet> fit = m_canvas->kWordDocument()->framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            // Only interested in text framesets
            KWTextFrameSet * fs = dynamic_cast<KWTextFrameSet *>(fit.current());
            if ( fs && fs->isVisible() )
            {
                QTextParag * lastParag = fs->textDocument()->lastParag();
                bool ret = true;
                if (!firstFrameSetFound && firstFrameSet == fs && firstParag)  // first frameset
                {
                    firstFrameSetFound = true;
                    ret = findInFrameSet( fs, firstParag, firstIndex, lastParag, lastParag->length()-1 );
                }
                else if ( firstFrameSetFound ) // another frameset -> search entirely
                {
                    ret = findInFrameSet( fs, fs->textDocument()->firstParag(), 0, lastParag, lastParag->length()-1 );
                }
                fs->removeHighlight();  // we're done with this frameset
                if (!ret) break;      // stop here if the user cancelled
            }
        }
    }
}

bool KWFindReplace::findInFrameSet( KWTextFrameSet * fs, QTextParag * firstParag, int firstIndex,
                                    QTextParag * lastParag, int lastIndex )
{
    // TODO formatting options are not implemented !
    // We need to reimplement what KoFind::find does, and add that.
    m_currentFrameSet = fs;
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
        QTextParag * endParag = forw ? lastParag : firstParag;
        while ( m_currentParag && m_currentParag != endParag )
        {
            QString str = m_currentParag->string()->toString();
            ret = process( str.left(str.length()-1) );
            if (!ret) return false;
            m_currentParag = forw ? m_currentParag->next() : m_currentParag->prev();
        }
        ASSERT( endParag == m_currentParag );
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

bool KWFindReplace::process( const QString &_text )
{
    if ( m_find )
        return m_find->find( _text, QRect() );
    else
    {
        QString text( _text );
        return m_replace->replace( text, QRect() );
    }
}

void KWFindReplace::highlight( const QString &, int matchingIndex, int matchingLength, const QRect & )
{
    //kdDebug() << "KWFind::highlight " << matchingIndex << "," << matchingLength << endl;
    m_currentFrameSet->highlightPortion( m_currentParag, m_offset + matchingIndex, matchingLength, m_canvas );
}

void KWFindReplace::replace( const QString &, int matchingIndex,
                             int matchingLength, const QRect &/*expose*/ )
{
    int index = m_offset + matchingIndex;
    // highlight might not have happened (if 'prompt on replace' is off)
    m_currentFrameSet->highlightPortion( m_currentParag, index, matchingLength, m_canvas );
    QTextDocument * textdoc = m_currentFrameSet->textDocument();
    QTextCursor cursor( textdoc );
    cursor.setParag( m_currentParag );
    cursor.setIndex( index );
    // Remove the match
    textdoc->removeSelectedText( KWTextFrameSet::HighlightSelection, &cursor );
    // Insert the replacement
    QTextFormat * format = m_currentParag->at( index )->format();
    // ## TODO back references (qt3)
    // TODO a macro command somehow
    m_currentFrameSet->insert( &cursor, static_cast<KWTextFormat *>(format),
                               m_replaceDlg->replacement(), true, false,
                               i18n("Insert Replacement") );
}

#include "searchdia.moc"
