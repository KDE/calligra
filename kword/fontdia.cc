/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#include "fontdia.h"
#include "kwdoc.h"
#include <kotextformat.h>

#include <kapp.h>
#include <kcolordialog.h>
#include <kfontdialog.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qdialog.h>
#include <qvbox.h>
#include <qgroupbox.h>

KWFontChooser::KWFontChooser( QWidget* parent, const char* name, bool _withSubSuperScript )
    : QWidget( parent, name )
{
    QVBoxLayout *lay1 = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );
    m_chooseFont = new KFontChooser(this);
    lay1->addWidget(m_chooseFont);

    QGroupBox *grp = new QGroupBox(this);
    lay1->addWidget(grp);
    QGridLayout *grid = new QGridLayout( grp, 2, 3, KDialog::marginHint(), KDialog::spacingHint() );

    m_underline = new QCheckBox(i18n("&Underline"),grp);
    grid->addWidget(m_underline,0,1);

    m_superScript = new QCheckBox(i18n("SuperScript"),grp);
    grid->addWidget(m_superScript,0,0);

    m_subScript = new QCheckBox(i18n("SubScript"),grp);
    grid->addWidget(m_subScript,1,0);

    if(!_withSubSuperScript)
    {
        m_subScript->setEnabled(false);
        m_superScript->setEnabled(false);
    }
    m_strikeOut = new QCheckBox(i18n("Strike &Out"),grp);
    grid->addWidget(m_strikeOut,1,1);

    m_colorButton = new QPushButton( i18n( "Change &Color..." ), grp );
    grid->addWidget(m_colorButton,0,2);

    connect( m_underline, SIGNAL(clicked()), this, SLOT( slotUnderlineClicked() ) );
    connect( m_strikeOut, SIGNAL(clicked()), this, SLOT( slotStrikeOutClicked() ) );
    connect( m_subScript, SIGNAL(clicked()), this, SLOT( slotSubScriptClicked() ) );
    connect( m_superScript, SIGNAL(clicked()), this, SLOT( slotSuperScriptClicked() ) );
    connect( m_colorButton, SIGNAL(clicked()), this, SLOT( slotChangeColor() ) );

    connect( m_chooseFont, SIGNAL( fontSelected( const QFont & )),
             this, SLOT( slotFontChanged(const QFont &) ) );

    m_changedFlags = 0;
}

void KWFontChooser::setFont( const QFont &_font, bool _subscript, bool _superscript )
{
    m_newFont = _font;
    m_chooseFont->setFont( m_newFont );
    m_underline->setChecked( _font.underline() );
    m_strikeOut->setChecked( _font.strikeOut() );
    m_subScript->setChecked( _subscript );
    m_superScript->setChecked( _superscript );
    m_changedFlags = 0;
}

void KWFontChooser::setColor( const QColor & col )
{
    // TODO: when kdelibs-2.2 is a requirement, get rid of m_color
    // and use m_chooseFont->color() directly.
    m_color = col;
#ifdef KFONTCHOOSER_HAS_SETCOLOR
    m_chooseFont->setColor( col );
#endif
    m_changedFlags = 0;
}

void KWFontChooser::slotFontChanged(const QFont & f)
{
    if ( f.weight() != m_newFont.weight() )
        m_changedFlags |= QTextFormat::Bold;
    if ( f.italic() != m_newFont.italic() )
        m_changedFlags |= QTextFormat::Italic;
    if ( f.underline() != m_newFont.underline() )
        m_changedFlags |= QTextFormat::Underline;
    if ( f.family() != m_newFont.family() )
        m_changedFlags |= QTextFormat::Family;
    if ( f.pointSize() != m_newFont.pointSize() )
        m_changedFlags |= QTextFormat::Size;
    kdDebug() << "KWFontChooser::slotFontChanged newcharset=" << f.charSet() << " oldcharset=" << m_newFont.charSet() << endl;
    if ( f.charSet() != m_newFont.charSet() )
        m_changedFlags |= KoTextFormat::CharSet;
    kdDebug() << "KWFontChooser::slotFontChanged m_changedFlags=" << m_changedFlags << endl;
    m_newFont = f;
}

void KWFontChooser::slotUnderlineClicked()
{
    m_newFont.setUnderline(m_underline->isChecked());
    m_chooseFont->setFont(m_newFont);
    m_changedFlags |= QTextFormat::Underline;
}

void KWFontChooser::slotStrikeOutClicked()
{
    m_newFont.setStrikeOut(m_strikeOut->isChecked());
    m_chooseFont->setFont(m_newFont);
    m_changedFlags |= KoTextFormat::StrikeOut;
}

void KWFontChooser::slotSubScriptClicked()
{
    if(m_superScript->isChecked())
        m_superScript->setChecked(false);
    m_changedFlags |= QTextFormat::VAlign;
}

void KWFontChooser::slotSuperScriptClicked()
{
    if(m_subScript->isChecked())
        m_subScript->setChecked(false);
    m_changedFlags |= QTextFormat::VAlign;
}

void KWFontChooser::slotChangeColor()
{
    QColor color = m_color;
    if ( KColorDialog::getColor( color ) )
    {
        if ( color != m_color )
        {
            m_changedFlags |= QTextFormat::Color;
            m_color = color;
#ifdef KFONTCHOOSER_HAS_SETCOLOR
            m_chooseFont->setColor( color );
#endif
        }
    }
}


KWFontDia::KWFontDia( QWidget* parent, const char* name, const QFont &_font,
                      bool _subscript, bool _superscript, const QColor & color,
                      bool _withSubSuperScript )
    : KDialogBase( parent, name, true,
                   i18n("Select Font"), Ok|Cancel, Ok )
{
    m_chooser = new KWFontChooser( this, "kwfontchooser", _withSubSuperScript );
    setMainWidget( m_chooser );
    m_chooser->setFont( _font, _subscript, _superscript );
    m_chooser->setColor( color );
}

#include "fontdia.moc"
