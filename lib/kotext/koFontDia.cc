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
#include <qradiobutton.h>

struct KoFontChooser::KoFontChooserPrivate
{
    QComboBox *m_strikeOut;
    QColor m_textColor;
};

KoFontChooser::KoFontChooser( QWidget* parent, const char* name, bool _withSubSuperScript, uint fontListCriteria)
    : QTabWidget( parent, name )
{
    d = new KoFontChooserPrivate;
    setupTab1(_withSubSuperScript, fontListCriteria );
    setupTab2();
    m_changedFlags = 0;
}

KoFontChooser::~KoFontChooser()
{
    delete d; d = 0;
}

QColor KoFontChooser::color() const
{
    return d->m_textColor;
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

    m_superScript = new QRadioButton(i18n("Superscript"),grp);
    grid->addWidget(m_superScript,0,0);

    m_subScript = new QRadioButton(i18n("Subscript"),grp);
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

    QLabel * lab = new QLabel( i18n("Underlining:"), grp);
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
    QStringList lstType;
    lstType <<i18n("Solid Line");
    lstType <<i18n("Dash Line");
    lstType <<i18n("Dot Line");
    lstType <<i18n("Dash Dot line");
    lstType <<i18n("Dash Dot Dot line");
    m_underlineType->insertStringList( lstType );


    m_underlineColorButton = new QPushButton( i18n( "Change Color..." ), grp );
    grid->addWidget(m_underlineColorButton,1,2);


    lab = new QLabel( i18n("Strikethrough:"), grp);
    grid->addWidget( lab, 2, 0);

    d->m_strikeOut = new QComboBox( grp );
    grid->addWidget( d->m_strikeOut, 3, 0);
    d->m_strikeOut->insertStringList( lst );


    m_strikeOutType= new QComboBox(grp );
    grid->addWidget( m_strikeOutType, 3, 1);
    m_strikeOutType->insertStringList( lstType );


    connect( d->m_strikeOut, SIGNAL(activated ( int )), this, SLOT( slotStrikeOutTypeChanged( int ) ) );
    connect( m_underlineColorButton, SIGNAL(clicked()), this, SLOT( slotUnderlineColor() ) );
    connect( m_underlining,  SIGNAL( activated ( int  ) ), this, SLOT( slotChangeUnderlineType( int )));
    connect( m_strikeOutType,  SIGNAL( activated ( int  ) ), this, SLOT( slotChangeStrikeOutType( int )));
}


void KoFontChooser::setFont( const QFont &_font, bool _subscript, bool _superscript )
{
    m_newFont = _font;

    m_subScript->setChecked( _subscript );
    m_superScript->setChecked( _superscript );

    m_chooseFont->setFont( m_newFont );
    m_changedFlags = 0;
}

void KoFontChooser::setColor( const QColor & col )
{
    d->m_textColor = col;
    if ( col.isValid() )
        m_chooseFont->setColor( col );
    else
        m_chooseFont->setColor( QApplication::palette().color( QPalette::Active, QColorGroup::Text ) );
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
    kdDebug() << "KWFontChooser::slotFontChanged m_changedFlags=" << m_changedFlags << endl;
    m_newFont = f;
}


void KoFontChooser::slotStrikeOutTypeChanged( int _val)
{
    m_changedFlags |= KoTextFormat::StrikeOut;
    m_strikeOutType->setEnabled( _val!=0 );
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
    QColor color = d->m_textColor;
    QColor defaultTextColor = QApplication::palette().color( QPalette::Active, QColorGroup::Text );
    if ( KColorDialog::getColor( color, defaultTextColor ) )
    {
        if ( color != d->m_textColor )
        {
            d->m_textColor = color;
            m_changedFlags |= KoTextFormat::Color;
            if ( color.isValid() )
                m_chooseFont->setColor( color );
            else
                m_chooseFont->setColor( defaultTextColor );
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

KoTextFormat::NbLine KoFontChooser::getUnderlineNbLineType()
{
    return getNbLineType( m_underlining );
}

KoTextFormat::NbLine KoFontChooser::getNbLineType( QComboBox * combo )
{
    switch (combo->currentItem () )
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

KoTextFormat::NbLine KoFontChooser::getStrikeOutNbLineType()
{
    return getNbLineType( d->m_strikeOut );
}


KoTextFormat::LineStyle KoFontChooser::getTypeOfLine( int val)
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

void KoFontChooser::changeNbLineType( KoTextFormat::NbLine _nb, QComboBox * combo)
{
    switch ( _nb )
    {
    case KoTextFormat::NONE:
        combo->setCurrentItem( 0 );
        break;
    case KoTextFormat::SIMPLE:
        combo->setCurrentItem( 1 );
        break;
    case KoTextFormat::DOUBLE:
        combo->setCurrentItem( 2 );
        break;
    default:
        combo->setCurrentItem( 0 );
        break;
    }
}

void KoFontChooser::setStrikeOutNblineType(KoTextFormat::NbLine nb)
{
    changeNbLineType( nb, d->m_strikeOut );
    m_changedFlags = 0;
}


void KoFontChooser::setUnderlineNbLineType(KoTextFormat::NbLine nb)
{
    changeNbLineType( nb, m_underlining);
    m_changedFlags = 0;
}

void KoFontChooser::changeLineStyle( KoTextFormat::LineStyle _t, QComboBox *combo )
{
    switch ( _t )
    {
    case KoTextFormat::SOLID:
        combo->setCurrentItem( 0 );
        break;
    case KoTextFormat::DASH:
        combo->setCurrentItem( 1 );
        break;
    case KoTextFormat::DOT:
        combo->setCurrentItem( 2 );
        break;
    case KoTextFormat::DASH_DOT:
        combo->setCurrentItem( 3 );
        break;
    case KoTextFormat::DASH_DOT_DOT:
        combo->setCurrentItem( 4 );
        break;
    default:
        combo->setCurrentItem( 0 );
        break;
    }

}

void KoFontChooser::setUnderlineLineStyle(KoTextFormat::LineStyle _t)
{
    changeLineStyle(  _t, m_underlineType );
    m_underlineType->setEnabled( m_underlining->currentItem()!= 0);
    m_underlineColorButton->setEnabled( m_underlining->currentItem()!=0);
    m_changedFlags = 0;
}

void KoFontChooser::setStrikeOutLineStyle(KoTextFormat::LineStyle _t)
{
    changeLineStyle(  _t, m_strikeOutType );
    m_strikeOutType->setEnabled( d->m_strikeOut->currentItem()!= 0);

    m_changedFlags = 0;
}

KoTextFormat::LineStyle KoFontChooser::getUnderlineLineStyle()
{
    return getTypeOfLine( m_underlineType->currentItem());
}

KoTextFormat::LineStyle KoFontChooser::getStrikeOutLineStyle()
{
    return getTypeOfLine( m_strikeOutType->currentItem());
}


void KoFontChooser::slotChangeUnderlineType( int i)
{
    m_changedFlags |= KoTextFormat::ExtendUnderLine;
    m_underlineType->setEnabled( i!= 0);
    m_underlineColorButton->setEnabled( i!=0);
}

void KoFontChooser::slotChangeStrikeOutType( int i)
{
    m_changedFlags |= KoTextFormat::StrikeOut;
    m_strikeOutType->setEnabled( i!= 0);
}

KoFontDia::KoFontDia( QWidget* parent, const char* name, const QFont &_font,
                      bool _subscript, bool _superscript, const QColor & color,
                      const QColor & backGroundColor ,
                      const QColor & underlineColor,
                      KoTextFormat::NbLine _underlineNbLine,
                      KoTextFormat::LineStyle _underlineType,
                      KoTextFormat::LineStyle _strikeOutType,
                      KoTextFormat::NbLine _strikeOutNbLine,
                      bool _withSubSuperScript )
    : KDialogBase( parent, name, true,
                   i18n("Select Font"), Ok|Cancel|User1|Apply, Ok ),
      m_font(_font),
      m_subscript( _subscript ),
      m_superscript(_superscript ),
      m_color( color),
      m_backGroundColor( backGroundColor),
      m_underlineColor( underlineColor ),
      m_underlineNbLine( _underlineNbLine ),
      m_underlineLineStyle( _underlineType ),
      m_strikeOutLineStyle( _strikeOutType ),
      m_strikeOutNbLine( _strikeOutNbLine)
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

    m_chooser->setUnderlineNbLineType(m_underlineNbLine);
    m_chooser->setUnderlineLineStyle(m_underlineLineStyle);
    m_chooser->setStrikeOutNblineType( m_strikeOutNbLine);

    m_chooser->setStrikeOutLineStyle(m_strikeOutLineStyle);
}

#include "koFontDia.moc"
