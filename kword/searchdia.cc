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

#include <kcolorbtn.h>
//#include <kglobal.h>
//#include <kglobalsettings.h>
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
#include <qstrlist.h>
#include <qwidget.h>
//#include <kmessagebox.h>
#include <searchdia.h>

KWSearchContext::KWSearchContext()
{
    m_family = "times";
    m_color = Qt::black;
    m_size = 12;
    //vertAlign = KWFormat::VA_NORMAL;
    m_optionsMask = 0;
    m_options = 0;
}

//
// This class represents the GUI elements that correspond to KWSearchContext.
//
class KWSearchContextUI
{
public:
    KWSearchContextUI( KWSearchContext *ctx, QWidget *parent );
    void setCtxOptions( long options );
private:
    KWSearchContext *m_ctx;
    QGridLayout *m_grid;
    QCheckBox *m_checkFamily;
    QCheckBox *m_checkSize;
    QCheckBox *m_checkColor;
    QCheckBox *m_checkBold;
    QCheckBox *m_checkItalic;
    QCheckBox *m_checkUnderline;
    QCheckBox *m_checkVertAlign;
    QComboBox *m_familyItem;
    QSpinBox *m_sizeItem;
    KColorButton *m_colorItem;
    QCheckBox *m_boldItem;
    QCheckBox *m_italicItem;
    QCheckBox *m_underlineItem;
    QComboBox *m_vertAlignItem;
};

KWSearchContextUI::KWSearchContextUI( KWSearchContext *ctx, QWidget *parent )
{
    m_ctx = ctx;
    m_grid = new QGridLayout( parent, 7, 2, 0, 6 );

    m_checkFamily = new QCheckBox( i18n( "Family" ), parent );
    m_checkSize = new QCheckBox( i18n( "Size" ), parent );
    m_checkColor = new QCheckBox( i18n( "Color" ), parent );
    m_checkBold = new QCheckBox( i18n( "Bold" ), parent );
    m_checkItalic = new QCheckBox( i18n( "Italic" ), parent );
    m_checkUnderline = new QCheckBox( i18n( "Underline" ), parent );
    m_checkVertAlign = new QCheckBox( i18n( "Vertical Alignment" ), parent );

    m_familyItem = new QComboBox( true, parent );
    m_familyItem->insertStringList( m_ctx->m_family );
    for ( int j = 0; j < m_familyItem->count(); j++ )
    {
        if ( QString( m_familyItem->text( j ) ) == m_ctx->m_family )
            m_familyItem->setCurrentItem( j );
    }
//    connect( familyItem, SIGNAL( activated( const QString & ) ), this, SLOT( slotFamily( const QString & ) ) );

    m_sizeItem = new QSpinBox( 4, 100, 1, parent );
    m_sizeItem->setValue( m_ctx->m_size );
//    connect( sizeItem, SIGNAL( activated( const QString & ) ), this, SLOT( slotSize( const QString & ) ) );

    m_colorItem = new KColorButton( parent );
    m_colorItem->setColor( m_ctx->m_color );
//    connect( bColor, SIGNAL( changed( const QColor& ) ), this, SLOT( slotColor( const QColor& ) ) );

    m_boldItem = new QCheckBox( i18n( "Bold" ), parent );
    m_italicItem = new QCheckBox( i18n( "Italic" ), parent );
    m_underlineItem = new QCheckBox( i18n( "Underline" ), parent );

//    connect( boldItem, SIGNAL( clicked() ), this, SLOT( slotBold() ) );

//    connect( italicItem, SIGNAL( clicked() ), this, SLOT( slotItalic() ) );

//    connect( underlineItem, SIGNAL( clicked() ), this, SLOT( slotUnderline() ) );

    m_vertAlignItem = new QComboBox( false, parent );
    m_vertAlignItem->insertItem( i18n( "Normal" ), -1 );
    m_vertAlignItem->insertItem( i18n( "Subscript" ), -1 );
    m_vertAlignItem->insertItem( i18n( "Superscript" ), -1 );
/*
    switch ( vertAlign )
    {
    case KWFormat::VA_NORMAL:
        vertAlignItem->setCurrentItem( 0 );
        break;
    case KWFormat::VA_SUB:
        vertAlignItem->setCurrentItem( 1 );
        break;
    case KWFormat::VA_SUPER:
        vertAlignItem->setCurrentItem( 2 );
        break;
    }
*/
 //   connect( vertAlignItem, SIGNAL( activated( int ) ), this, SLOT( slotVertAlign( int ) ) );
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

//    slotCheckFamily();
//    slotCheckColor();
//    slotCheckSize();
//    slotCheckBold();
//    slotCheckItalic();
//    slotCheckUnderline();
//    slotCheckVertAlign();

//    connect( this, SIGNAL( closeClicked() ), this, SLOT( saveSettings() ) );
}

/*================================================================*/
void KWSearchContextUI::setCtxOptions( long options )
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
    m_ctx->m_options = options;
    m_ctx->m_optionsMask = optionsMask;
    m_ctx->m_family = m_familyItem->currentText();
    m_ctx->m_size = m_sizeItem->cleanText().toInt();
    m_ctx->m_color = m_colorItem->color();
/*
    switch ( num )
    {
    case 0:
        searchEntry->vertAlign = KWFormat::VA_NORMAL;
        break;
    case 1:
        searchEntry->vertAlign = KWFormat::VA_SUB;
        break;
    case 3:
        searchEntry->vertAlign = KWFormat::VA_SUPER;
        break;
    }
*/
}

/*================================================================*/
KWSearchDia::KWSearchDia( QWidget *parent, const char *name, KWSearchContext *find ):
    KoFindDialog( parent, name, find->m_options, find->m_strings )
{
    // The dialog extension.
    m_find = new KWSearchContextUI( find, findExtension() );
}

void KWSearchDia::slotOk()
{
    KoFindDialog::slotOk();

    // Save the current state back into the context required.
    m_find->setCtxOptions( KoFindDialog::options() );
}

KWReplaceDia::KWReplaceDia( QWidget *parent, const char *name, KWSearchContext *find, KWSearchContext *replace ):
    KoReplaceDialog( parent, name, find->m_options, find->m_strings, replace->m_strings )
{
    // The dialog extension.
    m_find = new KWSearchContextUI( find, findExtension() );
    m_replace = new KWSearchContextUI( replace, replaceExtension() );
}

void KWReplaceDia::slotOk()
{
    KoReplaceDialog::slotOk();

    // Save the current state back into the context required.
    m_find->setCtxOptions( KoReplaceDialog::options() );
    m_replace->setCtxOptions( KoReplaceDialog::options() );
}

/*================================================================*/


#if 0
/*================================================================*/
void KWSearchDia::searchFirst()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool addlen = true;

#if 0
    if ( !cRev->isChecked() )
        canvas->find( expr, searchEntry, true, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(), cWildcard->isChecked(), addlen );
    else
        canvas->findRev( expr, searchEntry, true, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(), cWildcard->isChecked(), addlen );
#endif
}

/*================================================================*/
void KWSearchDia::searchNext()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool addlen = true;

#if 0
    if ( !cRev->isChecked() )
        canvas->find( expr, searchEntry, false, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(), cWildcard->isChecked(), addlen );
    else
        canvas->findRev( expr, searchEntry, false, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(), cWildcard->isChecked(), addlen );
#endif
}

/*================================================================*/
void KWSearchDia::replaceFirst()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool addlen = false;
    bool replace = false;

#if 0
    if ( !cRev->isChecked() )
        replace = canvas->find( expr, searchEntry, true, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                              cWildcard->isChecked(), addlen, false );
    else
        replace = canvas->findRev( expr, searchEntry, true, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                                 cWildcard->isChecked(), addlen, false );

    if ( replace )
        canvas->replace( eReplace->text(), replaceEntry, addlen );
#endif
}

/*================================================================*/
void KWSearchDia::replaceNext()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool addlen = false;
    bool replace = false;

#if 0
    if ( !cRev->isChecked() )
        replace = canvas->find( expr, searchEntry, false, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                              cWildcard->isChecked(), addlen, false );
    else
        replace = canvas->findRev( expr, searchEntry, false, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                                 cWildcard->isChecked(), addlen, false );

    if ( replace )
        canvas->replace( eReplace->text(), replaceEntry, addlen );
#endif
}

/*================================================================*/
void KWSearchDia::replaceAll()
{
    QString expr = eSearch->text();
    if ( expr.isEmpty() ) return;

    bool first = true;
    bool addlen = false;
    bool replace = false;
    bool select = cAsk->isChecked();

#if 0
    while ( true )
    {
        if ( !cRev->isChecked() )
            replace = canvas->find( expr, searchEntry, first, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                                  cWildcard->isChecked(), addlen, select );
        else
            replace = canvas->findRev( expr, searchEntry, first, cCase->isChecked(), cWholeWords->isChecked(), cRegExp->isChecked(),
                                     cWildcard->isChecked(), addlen, select );
        first = false;

        if ( replace && cAsk->isChecked() )
        {
            int result = KMessageBox::warningYesNoCancel( this,
                                i18n( "Replace selected text?" ),
                                i18n( "Replace" ),
                                i18n( "&Replace" ), i18n( "&Skip" ) );

            if (result == KMessageBox::Cancel)
            {
               // Cancel
               break;
            }

            if (result == KMessageBox::No)
            {
                // Skip
                if ( addlen ) canvas->addLen();
                canvas->repaintScreen( false );
                continue;
            };

            // KMessageBox::Yes
            // Replace continues
        }

        if ( replace )
            canvas->replace( eReplace->text(), replaceEntry, addlen );
        else
            break;
    }

    canvas->repaintScreen( false );
#endif
}
#endif

#include "searchdia.moc"
