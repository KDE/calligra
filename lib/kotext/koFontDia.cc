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

#include "koFontDia.h"
#include "qrichtext_p.h"

#include <kcolordialog.h>
#include <klocale.h>
#include <kdebug.h>

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qcombobox.h>
KoFontChooser::KoFontChooser( QWidget* parent, const char* name, bool _withSubSuperScript, uint fontListCriteria)
    : QTabWidget( parent, name )
{
    setupTab1(_withSubSuperScript, fontListCriteria );
    setupTab2();
    m_changedFlags = 0;
}

void KoFontChooser::setupTab1(bool _withSubSuperScript, uint fontListCriteria )
{
    QWidget *page = new QWidget( this );
    addTab( page, i18n( "Fonts" ) );
    QVBoxLayout *lay1 = new QVBoxLayout( page, KDialog::marginHint(), KDialog::spacingHint() );
    QStringList list;
    KFontChooser::getFontList(list,fontListCriteria);
    m_chooseFont = new KFontChooser(page, "FontList", false, list);
    lay1->addWidget(m_chooseFont);

    QGroupBox *grp = new QGroupBox(page);
    lay1->addWidget(grp);
    QGridLayout *grid = new QGridLayout( grp, 2, 3, KDialog::marginHint(), KDialog::spacingHint() );

    m_superScript = new QCheckBox(i18n("SuperScript"),grp);
    grid->addWidget(m_superScript,0,0);

    m_subScript = new QCheckBox(i18n("SubScript"),grp);
    grid->addWidget(m_subScript,1,0);

    if(!_withSubSuperScript)
    {
        m_subScript->setEnabled(false);
        m_superScript->setEnabled(false);
    }

    m_colorButton = new QPushButton( i18n( "Change Color..." ), grp );
    grid->addWidget(m_colorButton,0,3);

    m_backGroundColorButton = new QPushButton( i18n( "Change Background Color..." ), grp );
    grid->addWidget(m_backGroundColorButton,1,3);

    connect( m_subScript, SIGNAL(clicked()), this, SLOT( slotSubScriptClicked() ) );
    connect( m_superScript, SIGNAL(clicked()), this, SLOT( slotSuperScriptClicked() ) );
    connect( m_colorButton, SIGNAL(clicked()), this, SLOT( slotChangeColor() ) );

    connect( m_backGroundColorButton,  SIGNAL(clicked()), this, SLOT( slotChangeBackGroundColor() ) );

    connect( m_chooseFont, SIGNAL( fontSelected( const QFont & )),
             this, SLOT( slotFontChanged(const QFont &) ) );

}
void KoFontChooser::setupTab2()
{
    QWidget *page = new QWidget( this );
    addTab( page, i18n( "Fonts Effect" ) );

    QVBoxLayout *lay1 = new QVBoxLayout( page, KDialog::marginHint(), KDialog::spacingHint() );
    QGroupBox *grp = new QGroupBox(page);
    lay1->addWidget(grp);
    QGridLayout *grid = new QGridLayout( grp, 10, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QLabel * lab = new QLabel( i18n("Underlining"), grp);
    grid->addWidget( lab, 0, 0);

    m_underlining = new QComboBox( grp );
    grid->addWidget( m_underlining, 1, 0);

    QStringList lst;
    lst <<i18n("Without");
    lst <<i18n("Single");
    lst <<i18n("Double");
    m_underlining->insertStringList( lst );

    m_underlineType = new QComboBox(grp );
    grid->addWidget( m_underlineType, 1, 1);
    lst.clear();
    lst <<i18n("Solid line");
    lst <<i18n("Dash line");
    lst <<i18n("Dot line");
    lst <<i18n("Dash Dot line");
    lst <<i18n("Dash Dot Dot line");
    m_underlineType->insertStringList( lst );


    m_underlineColorButton = new QPushButton( i18n( "Change Color..." ), grp );
    grid->addWidget(m_underlineColorButton,1,2);


    lab = new QLabel( i18n("StrikeThrough"), grp);
    grid->addWidget( lab, 2, 1);

    m_strikeOut = new QCheckBox(i18n("Strike out"),grp);
    grid->addWidget(m_strikeOut,3,0);

    m_strikeOutType= new QComboBox(grp );
    grid->addWidget( m_strikeOutType, 3, 1);
    m_strikeOutType->insertStringList( lst );


    connect( m_strikeOut, SIGNAL(clicked()), this, SLOT( slotStrikeOutClicked() ) );
    connect( m_underlineColorButton, SIGNAL(clicked()), this, SLOT( slotUnderlineColor() ) );
    connect( m_underlining,  SIGNAL( activated ( int  ) ), this, SLOT( slotChangeUnderlineType( int )));
    connect( m_strikeOutType,  SIGNAL( activated ( int  ) ), this, SLOT( slotChangeStrikeOutType( int )));
}


void KoFontChooser::setFont( const QFont &_font, bool _subscript, bool _superscript )
{
    m_newFont = _font;
    m_strikeOut->setChecked( _font.strikeOut() );
    m_strikeOutType->setEnabled( m_strikeOut->isChecked() );

    m_subScript->setChecked( _subscript );
    m_superScript->setChecked( _superscript );

    m_chooseFont->setFont( m_newFont );
    m_changedFlags = 0;
}

void KoFontChooser::setColor( const QColor & col )
{
    m_chooseFont->setColor( col );
    m_changedFlags = 0;
}

void KoFontChooser::setUnderlineColor( const QColor & col )
{
    m_underlineColor = col;
    m_changedFlags = 0;
}

void KoFontChooser::setBackGroundColor ( const QColor & col )
{
    m_backGroundColor = col;
    m_changedFlags = 0;
}

void KoFontChooser::slotFontChanged(const QFont & f)
{
    if ( f.weight() != m_newFont.weight() )
        m_changedFlags |= KoTextFormat::Bold;
    if ( f.italic() != m_newFont.italic() )
        m_changedFlags |= KoTextFormat::Italic;
    if ( f.family() != m_newFont.family() )
        m_changedFlags |= KoTextFormat::Family;
    if ( f.pointSize() != m_newFont.pointSize() )
        m_changedFlags |= KoTextFormat::Size;
    //kdDebug() << "KWFontChooser::slotFontChanged newcharset=" << f.charSet() << " oldcharset=" << m_newFont.charSet() << endl;
    // ######## Not needed in 3.0?
    //if ( f.charSet() != m_newFont.charSet() )
    //    m_changedFlags |= KoTextFormat::CharSet;
    kdDebug() << "KWFontChooser::slotFontChanged m_changedFlags=" << m_changedFlags << endl;
    m_newFont = f;
}


void KoFontChooser::slotStrikeOutClicked()
{
    m_newFont.setStrikeOut(m_strikeOut->isChecked());
    m_chooseFont->setFont(m_newFont);
    m_changedFlags |= KoTextFormat::StrikeOut;
    m_strikeOutType->setEnabled( m_strikeOut->isChecked() );
}

void KoFontChooser::slotSubScriptClicked()
{
    if(m_superScript->isChecked())
        m_superScript->setChecked(false);
    m_changedFlags |= KoTextFormat::VAlign;
}

void KoFontChooser::slotSuperScriptClicked()
{
    if(m_subScript->isChecked())
        m_subScript->setChecked(false);
    m_changedFlags |= KoTextFormat::VAlign;
}

void KoFontChooser::slotChangeColor()
{
    QColor color = m_chooseFont->color();
    if ( KColorDialog::getColor( color, QApplication::palette().color( QPalette::Active, QColorGroup::Text ) ) )
    {
        if ( color != m_chooseFont->color() )
        {
            m_changedFlags |= KoTextFormat::Color;
            m_chooseFont->setColor( color );
        }
    }
}

void KoFontChooser::slotChangeBackGroundColor()
{
    QColor color = m_backGroundColor;
    if ( KColorDialog::getColor( color, QApplication::palette().color( QPalette::Active, QColorGroup::Base ) ) )
    {
        if ( color != m_chooseFont->color() )
        {
            m_changedFlags |= KoTextFormat::TextBackgroundColor;
            m_backGroundColor = color;
        }
    }
}

void KoFontChooser::slotUnderlineColor()
{
    QColor color = m_underlineColor;
    if ( KColorDialog::getColor( color, QApplication::palette().color( QPalette::Active, QColorGroup::Base ) ) )
    {
        if ( color != m_underlineColor )
        {
            m_changedFlags |= KoTextFormat::ExtendUnderLine;
            m_underlineColor = color;
        }
    }
}

KoTextFormat::NbLine KoFontChooser::getNblineType()
{
    switch (m_underlining->currentItem () )
    {
    case 0:
        return KoTextFormat::NONE;
        break;
    case 1:
        return KoTextFormat::SIMPLE;
        break;
    case 2:
        return KoTextFormat::DOUBLE;
        break;
    default:
        return KoTextFormat::NONE;
    }
}

KoTextFormat::LineType KoFontChooser::getTypeOfLine( int val)
{
    switch ( val )
    {
    case 0:
        return KoTextFormat::SOLID;
        break;
    case 1:
        return KoTextFormat::DASH;
        break;
    case 2:
        return KoTextFormat::DOT;
        break;
    case 3:
        return KoTextFormat::DASH_DOT;
        break;
    case 4:
        return KoTextFormat::DASH_DOT_DOT;
        break;
    default:
        return KoTextFormat::SOLID;
    }
}

void KoFontChooser::setNblineType(KoTextFormat::NbLine nb)
{
    switch (nb )
    {
    case KoTextFormat::NONE:
        m_underlining->setCurrentItem( 0 );
        break;
    case KoTextFormat::SIMPLE:
        m_underlining->setCurrentItem( 1 );
        break;
    case KoTextFormat::DOUBLE:
        m_underlining->setCurrentItem( 2 );
        break;
    default:
        m_underlining->setCurrentItem( 0 );
        break;
    }
    m_changedFlags = 0;
}

void KoFontChooser::setUnderlineType(KoTextFormat::LineType _t)
{
    switch ( _t )
    {
    case KoTextFormat::SOLID:
        m_underlineType->setCurrentItem( 0 );
        break;
    case KoTextFormat::DASH:
        m_underlineType->setCurrentItem( 1 );
        break;
    case KoTextFormat::DOT:
        m_underlineType->setCurrentItem( 2 );
        break;
    case KoTextFormat::DASH_DOT:
        m_underlineType->setCurrentItem( 3 );
        break;
    case KoTextFormat::DASH_DOT_DOT:
        m_underlineType->setCurrentItem( 4 );
        break;
    default:
        m_underlineType->setCurrentItem( 0 );
        break;
    }
    m_underlineType->setEnabled( m_underlining->currentItem()!= 0);
    m_underlineColorButton->setEnabled( m_underlining->currentItem()!=0);
    m_changedFlags = 0;
}

void KoFontChooser::setStrikeOutType(KoTextFormat::LineType _t)
{
    switch ( _t )
    {
    case KoTextFormat::SOLID:
        m_strikeOutType->setCurrentItem( 0 );
        break;
    case KoTextFormat::DASH:
        m_strikeOutType->setCurrentItem( 1 );
        break;
    case KoTextFormat::DOT:
        m_strikeOutType->setCurrentItem( 2 );
        break;
    case KoTextFormat::DASH_DOT:
        m_strikeOutType->setCurrentItem( 3 );
        break;
    case KoTextFormat::DASH_DOT_DOT:
        m_strikeOutType->setCurrentItem( 4 );
        break;
    default:
        m_strikeOutType->setCurrentItem( 0 );
        break;
    }
    m_changedFlags = 0;
}

KoTextFormat::LineType KoFontChooser::getUnderlineType()
{
    return getTypeOfLine( m_underlineType->currentItem());
}

KoTextFormat::LineType KoFontChooser::getStrikeOutType()
{
    return getTypeOfLine( m_strikeOutType->currentItem());
}


void KoFontChooser::slotChangeUnderlineType( int i)
{
    m_changedFlags |= KoTextFormat::ExtendUnderLine;
    m_underlineType->setEnabled( i!= 0);
    m_underlineColorButton->setEnabled( i!=0);
}

void KoFontChooser::slotChangeStrikeOutType( int )
{
    m_changedFlags |= KoTextFormat::StrikeOut;
}

KoFontDia::KoFontDia( QWidget* parent, const char* name, const QFont &_font,
                      bool _subscript, bool _superscript, const QColor & color,
                      const QColor & backGroundColor ,
                      const QColor & underlineColor,
                      KoTextFormat::NbLine _nbLine,
                      KoTextFormat::LineType _underlineType,
                      KoTextFormat::LineType _strikeOutType,
                      bool _withSubSuperScript )
    : KDialogBase( parent, name, true,
                   i18n("Select Font"), Ok|Cancel|User1|Apply, Ok ),
      m_font(_font),
      m_subscript( _subscript ),
      m_superscript(_superscript ),
      m_color( color),
      m_backGroundColor( backGroundColor),
      m_underlineColor( underlineColor ),
      m_nbLine( _nbLine ),
      m_underlineType( _underlineType ),
      m_strikeOutType( _strikeOutType )
{
    setButtonText( KDialogBase::User1, i18n("Reset") );

    m_chooser = new KoFontChooser( this, "kofontchooser", _withSubSuperScript );
    setMainWidget( m_chooser );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotReset()));

    slotReset();
}

void KoFontDia::slotApply()
{
    emit apply();
}

void KoFontDia::slotOk()
{
    slotApply();
    KDialogBase::slotOk();
}

void KoFontDia::slotReset()
{
    m_chooser->setFont( m_font, m_subscript, m_superscript );
    m_chooser->setColor( m_color );
    m_chooser->setBackGroundColor(m_backGroundColor);
    m_chooser->setUnderlineColor( m_underlineColor );

    m_chooser->setNblineType(m_nbLine);
    m_chooser->setUnderlineType(m_underlineType);
    m_chooser->setStrikeOutType(m_strikeOutType);
}

#include "koFontDia.moc"
