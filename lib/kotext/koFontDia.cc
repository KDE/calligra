/* This file is part of the KDE project
   Copyright (C)  2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#include "koFontDia_p.h"
#include "koFontDia.h"
#include "korichtext.h"

#include <kcolordialog.h>
#include <klocale.h>
#include <kdebug.h>

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <knuminput.h>
#include <koGlobal.h>
#include <qvgroupbox.h>
#include <qvbox.h>
#include <qhgroupbox.h>
#include <qhbuttongroup.h>
#include <kcolorbutton.h>
#include <kiconloader.h>

KoShadowPreview::KoShadowPreview( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    setFrameStyle( WinPanel | Sunken );
    setBackgroundColor( white );
}

void KoShadowPreview::drawContents( QPainter* painter )
{
    QFont font(KoGlobal::defaultFont().family(), 30, QFont::Bold);
    QFontMetrics fm( font );

    const QString text ( i18n ( "Test string for shadow dialog", "KOffice") );
    QRect br = fm.boundingRect( text );
    int x = ( width() - br.width() ) / 2;
    int y = ( height() - br.height() ) / 2 + br.height();
    painter->save();

    int sx = x + qRound(shadowDistanceX);
    int sy = y + qRound(shadowDistanceY);

    painter->setFont( font );
    painter->setPen( shadowColor );
    painter->drawText( sx, sy, text );

    painter->setPen( blue );
    painter->drawText( x, y, text );

    painter->restore();
}

KoTextShadowWidget::KoTextShadowWidget( QWidget * parent, const char * name )
  : QWidget( parent, name )
{
    QGridLayout *grid = new QGridLayout( this, 8, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QGroupBox *shadowBox = new QGroupBox( i18n( "Shadow" ), this, "shadow" );
    grid->addMultiCellWidget( shadowBox, 0, 3,0,0 );

    QGridLayout *grid2 = new QGridLayout( shadowBox, 4, 2, 2*KDialog::marginHint(), 2*KDialog::spacingHint() );

    QLabel *lcolor = new QLabel( i18n( "Co&lor:" ), shadowBox );
    grid2->addWidget(lcolor,0,0);
    color = new KColorButton( black,
                              black,
                              shadowBox );
    lcolor->setBuddy( color );
    grid2->addWidget(color,1,0);
    connect( color, SIGNAL( changed( const QColor& ) ), this, SLOT( colorChanged( const QColor& ) ) );

    QLabel *ldistance = new QLabel( i18n( "&Distance (pt):" ), shadowBox );
    grid2->addWidget(ldistance,2,0);

    // ### TODO: use KoUnitDoubleSpinBox instead
    distance = new QSpinBox( 0, 20, 1, shadowBox );
    distance->setSuffix(i18n("pt"));
    ldistance->setBuddy( distance );
    connect( distance, SIGNAL( valueChanged( int ) ), this, SLOT( distanceChanged( int ) ) );
    grid2->addWidget(distance,3,0);

    QLabel *ldirection = new QLabel( i18n( "Di&rection:" ), shadowBox );
    grid2->addWidget(ldirection,0,1);

    QGridLayout *grid3 = new QGridLayout( 0L, 3, 3, KDialog::marginHint(), KDialog::spacingHint() );

    lu = new QPushButton( shadowBox );
    grid3->addWidget(lu,0,0);
    lu->setToggleButton( true );
        ldirection->setBuddy( lu );
    u = new QPushButton( shadowBox );
    grid3->addWidget(u,0,1);
    u->setToggleButton( true );
    ru = new QPushButton( shadowBox );
    grid3->addWidget(ru,0,2);
    ru->setToggleButton( true );
    r = new QPushButton( shadowBox );
    grid3->addWidget(r,1,2);
    r->setToggleButton( true );
    rb = new QPushButton( shadowBox );
    grid3->addWidget(rb,2,2);
    rb->setToggleButton( true );
    b = new QPushButton( shadowBox );
    grid3->addWidget(b,2,1);
    b->setToggleButton( true );
    lb = new QPushButton( shadowBox );
    grid3->addWidget(lb,2,0);
    lb->setToggleButton( true );
    l = new QPushButton( shadowBox );
    grid3->addWidget(l,1,0);
    l->setToggleButton( true );

    lu->setPixmap( BarIcon( "shadowLU" ) );
    u->setPixmap( BarIcon( "shadowU" ) );
    ru->setPixmap( BarIcon( "shadowRU" ) );
    r->setPixmap( BarIcon( "shadowR" ) );
    rb->setPixmap( BarIcon( "shadowRB" ) );
    b->setPixmap( BarIcon( "shadowB" ) );
    lb->setPixmap( BarIcon( "shadowLB" ) );
    l->setPixmap( BarIcon( "shadowL" ) );

    connect( lu, SIGNAL( clicked() ), this, SLOT( luChanged() ) );
    connect( u, SIGNAL( clicked() ), this, SLOT( uChanged() ) );
    connect( ru, SIGNAL( clicked() ), this, SLOT( ruChanged() ) );
    connect( r, SIGNAL( clicked() ), this, SLOT( rChanged() ) );
    connect( rb, SIGNAL( clicked() ), this, SLOT( rbChanged() ) );
    connect( b, SIGNAL( clicked() ), this, SLOT( bChanged() ) );
    connect( lb, SIGNAL( clicked() ), this, SLOT( lbChanged() ) );
    connect( l, SIGNAL( clicked() ), this, SLOT( lChanged() ) );


    grid2->addMultiCellLayout (grid3, 1,3, 1, 1 );

    m_shadowPreview = new KoShadowPreview( this, "preview" );
    grid->addMultiCellWidget( m_shadowPreview, 0, 3, 1, 1 );
}

void KoTextShadowWidget::setShadowDirection( short int sd )
{
    m_shadowDirection = sd;
    m_shadowPreview->setShadowDistanceX( shadowDistanceX() );
    m_shadowPreview->setShadowDistanceY( shadowDistanceY() );

    lu->setOn( false );
    u->setOn( false );
    ru->setOn( false );
    r->setOn( false );
    rb->setOn( false );
    b->setOn( false );
    lb->setOn( false );
    l->setOn( false );

    switch ( m_shadowDirection )
    {
    case SD_LEFT_UP:
        lu->setOn( true );
        break;
    case SD_UP:
        u->setOn( true );
        break;
    case SD_RIGHT_UP:
        ru->setOn( true );
        break;
    case SD_RIGHT:
        r->setOn( true );
        break;
    case SD_RIGHT_BOTTOM:
        rb->setOn( true );
        break;
    case SD_BOTTOM:
        b->setOn( true );
        break;
    case SD_LEFT_BOTTOM:
        lb->setOn( true );
        break;
    case SD_LEFT:
        l->setOn( true );
        break;
    }
}

void KoTextShadowWidget::setShadow( double shadowDistanceX, double shadowDistanceY, const QColor& shadowColor )
{
    // Figure out shadow direction from x and y distance
    // Ugly temporary code
    m_shadowDirection = SD_RIGHT_BOTTOM;
    if ( shadowDistanceX > 0 ) // right
        if ( shadowDistanceY == 0 )
            m_shadowDirection = SD_RIGHT;
        else
            m_shadowDirection = shadowDistanceY > 0 ? SD_RIGHT_BOTTOM : SD_RIGHT_UP;
    else if ( shadowDistanceX == 0 ) // top/bottom
            m_shadowDirection = shadowDistanceY > 0 ? SD_BOTTOM : SD_UP;
    else // left
        if ( shadowDistanceY == 0 )
            m_shadowDirection = SD_LEFT;
        else
            m_shadowDirection = shadowDistanceY > 0 ? SD_LEFT_BOTTOM : SD_LEFT_UP;

    m_shadowDistance = QMAX( QABS(shadowDistanceX), QABS(shadowDistanceY) );
    m_shadowPreview->setShadowDistanceX( shadowDistanceX );
    m_shadowPreview->setShadowDistanceY( shadowDistanceY );
    // TODO turn distance into a KDoubleNumInput
    distance->setValue( (int)m_shadowDistance );

    m_shadowColor = shadowColor;
    m_shadowPreview->setShadowColor( m_shadowColor );
    color->setColor( m_shadowColor.isValid() ? m_shadowColor: gray  );
}

void KoTextShadowWidget::luChanged()
{
    setShadowDirection( SD_LEFT_UP );
}

void KoTextShadowWidget::uChanged()
{
    setShadowDirection( SD_UP );
}

void KoTextShadowWidget::ruChanged()
{
    setShadowDirection( SD_RIGHT_UP );
}

void KoTextShadowWidget::rChanged()
{
    setShadowDirection( SD_RIGHT );
}

void KoTextShadowWidget::rbChanged()
{
    setShadowDirection( SD_RIGHT_BOTTOM );
}

void KoTextShadowWidget::bChanged()
{
    setShadowDirection( SD_BOTTOM );
}

void KoTextShadowWidget::lbChanged()
{
    setShadowDirection( SD_LEFT_BOTTOM );
}

void KoTextShadowWidget::lChanged()
{
    setShadowDirection( SD_LEFT );
}

void KoTextShadowWidget::colorChanged( const QColor& col )
{
    m_shadowColor = col;
    m_shadowPreview->setShadowColor( col );
    emit changed();
}

void KoTextShadowWidget::distanceChanged( int _val )
{
    m_shadowDistance = _val;
    m_shadowPreview->setShadowDistanceX( shadowDistanceX() );
    m_shadowPreview->setShadowDistanceY( shadowDistanceY() );
    emit changed();
}

QString KoTextShadowWidget::tabName() {
    return i18n( "S&hadow" );
}

double KoTextShadowWidget::shadowDistanceX() const
{
    switch ( m_shadowDirection )
    {
    case SD_LEFT_BOTTOM:
    case SD_LEFT:
    case SD_LEFT_UP:
        return - m_shadowDistance;
    case SD_UP:
    case SD_BOTTOM:
        return 0;
    case SD_RIGHT_UP:
    case SD_RIGHT:
    case SD_RIGHT_BOTTOM:
        return m_shadowDistance;
    }
    return 0;
}

double KoTextShadowWidget::shadowDistanceY() const
{
    switch ( m_shadowDirection )
    {
    case SD_LEFT_UP:
    case SD_UP:
    case SD_RIGHT_UP:
        return - m_shadowDistance;
    case SD_LEFT:
    case SD_RIGHT:
        return 0;
    case SD_LEFT_BOTTOM:
    case SD_BOTTOM:
    case SD_RIGHT_BOTTOM:
        return m_shadowDistance;
    }
    return 0;
}

class KoFontChooser::KoFontChooserPrivate
{
public:
    QComboBox *m_strikeOut;
    QColor m_textColor;
    KoTextShadowWidget *m_shadowWidget;
    KIntNumInput *m_relativeSize;
    QLabel *m_lRelativeSize;
    KIntNumInput *m_offsetBaseLine;
    QCheckBox *m_wordByWord;
#ifdef ATTRCOMBO
    QComboBox *m_fontAttribute;
#else
    QButtonGroup* m_fontAttribute;
#endif
    QComboBox *m_language;
#ifdef HAVE_LIBKSPELL2
    KSpell2::Broker::Ptr m_broker;
#endif
    QCheckBox *m_hyphenation;
};

KoFontChooser::KoFontChooser( QWidget* parent, const char* name, bool _withSubSuperScript, uint fontListCriteria
#ifdef HAVE_LIBKSPELL2
                   , KSpell2::Broker::Ptr broker
#endif
    )
    : QTabWidget( parent, name )
{
    d = new KoFontChooserPrivate;
    d->m_broker = broker;
    setupTab1( fontListCriteria );
    setupTab2( _withSubSuperScript );
    // More modular solution: one widget per tab....
    d->m_shadowWidget = new KoTextShadowWidget( this );
    connect( d->m_shadowWidget, SIGNAL(changed()), this, SLOT(slotShadowChanged()) );
    addTab( d->m_shadowWidget, d->m_shadowWidget->tabName() );
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

void KoFontChooser::setupTab1( uint fontListCriteria )
{
    QWidget *page = new QWidget( this );
    addTab( page, i18n( "&Fonts" ) );
    QVBoxLayout *lay1 = new QVBoxLayout( page, KDialog::marginHint(), KDialog::spacingHint() );
    QStringList list;
    KFontChooser::getFontList(list,fontListCriteria);
    m_chooseFont = new KFontChooser(page, "FontList", false, list);
    lay1->addWidget(m_chooseFont);

    QHGroupBox* colorsGrp = new QHGroupBox(i18n("Colors"), page);
    lay1->addWidget(colorsGrp);
    //grid = new QGridLayout( colorsGrp, 2, 2, KDialog::marginHint(), KDialog::spacingHint() );
    m_colorButton = new QPushButton( i18n( "Change Co&lor..." ), colorsGrp );
    //grid->addWidget(m_colorButton,0,0);

    m_backGroundColorButton = new QPushButton( i18n( "Change Bac&kground Color..." ), colorsGrp );
    //grid->addWidget(m_backGroundColorButton,0,1);

    connect( m_colorButton, SIGNAL(clicked()), this, SLOT( slotChangeColor() ) );

    connect( m_backGroundColorButton,  SIGNAL(clicked()), this, SLOT( slotChangeBackGroundColor() ) );

    connect( m_chooseFont, SIGNAL( fontSelected( const QFont & )),
             this, SLOT( slotFontChanged(const QFont &) ) );

}

void KoFontChooser::setupTab2( bool _withSubSuperScript )
{
    QWidget *page = new QWidget( this );
    addTab( page, i18n( "Font &Effects" ) );

    //QVBoxLayout *lay1 = new QVBoxLayout( page, KDialog::marginHint(), KDialog::spacingHint() );
    // A single groupbox, without title, looks stupid.
    //QGroupBox *grp = new QGroupBox(page);
    //lay1->addWidget(grp);
    QWidget* grp = page; // just to be able to go back to a groupbox

    QGridLayout *grid = new QGridLayout( grp, 10, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QLabel * lab = new QLabel( i18n("&Underlining:"), grp);
    grid->addWidget( lab, 0, 0);

    m_underlining = new QComboBox( grp );
    lab->setBuddy( m_underlining );
    grid->addWidget( m_underlining, 1, 0);

    m_underlining->insertStringList( KoTextFormat::underlineTypeList() );

    m_underlineType = new QComboBox(grp );
    grid->addWidget( m_underlineType, 1, 1);
    m_underlineType->insertStringList( KoTextFormat::underlineStyleList() );


    m_underlineColorButton = new QPushButton( i18n( "Change Co&lor..." ), grp );
    grid->addWidget(m_underlineColorButton,1,2);


    QLabel * lab2 = new QLabel( i18n("&Strikethrough:"), grp);
    grid->addWidget( lab2, 2, 0);
    d->m_strikeOut = new QComboBox( grp );
    lab2->setBuddy( d->m_strikeOut );
    grid->addWidget( d->m_strikeOut, 3, 0);
    d->m_strikeOut->insertStringList( KoTextFormat::strikeOutTypeList() );


    m_strikeOutType= new QComboBox(grp );
    grid->addWidget( m_strikeOutType, 3, 1);
    m_strikeOutType->insertStringList( KoTextFormat::strikeOutStyleList() );

    d->m_wordByWord = new QCheckBox( i18n("&Word by word"), grp);
    grid->addWidget( d->m_wordByWord, 4, 0);

    QVGroupBox *grp1 = new QVGroupBox(i18n("Position"), page);
    grid->addMultiCellWidget( grp1, 5, 5, 0, grid->numCols()-1 );
    QWidget* grpBox = new QWidget( grp1 ); // container for the grid - laid out inside the QVGroupBox
    QGridLayout *grid1 = new QGridLayout( grpBox, 2, 3, 0, KDialog::spacingHint() );
    grid1->setColStretch( 1, 1 ); // better stretch labels than spinboxes.

    // superscript/subscript need to be checkboxes, not radiobuttons.
    // otherwise it's not possible to disable both, and there's no room for a 3rd one like 'none'
    m_superScript = new QCheckBox(i18n("Su&perscript"),grpBox);
    grid1->addWidget(m_superScript,0,0);

    m_subScript = new QCheckBox(i18n("Su&bscript"),grpBox);
    grid1->addWidget(m_subScript,1,0);

    d->m_lRelativeSize = new QLabel ( i18n("Relative &size:"), grpBox);
    d->m_lRelativeSize->setAlignment( Qt::AlignRight );
    grid1->addWidget(d->m_lRelativeSize,0,1);

    // ## How to make this widget smaller? Sounds like the [minimum]sizeHint for KIntNumInput is big...
    d->m_relativeSize = new KIntNumInput( grpBox );
    d->m_lRelativeSize->setBuddy( d->m_relativeSize );
    grid1->addWidget(d->m_relativeSize,0,2);

    d->m_relativeSize-> setRange(1, 100, 1,false);
    d->m_relativeSize->setSuffix("%");

    QLabel *lab5 = new QLabel ( i18n("Offse&t from baseline:"), grpBox);
    lab5->setAlignment( Qt::AlignRight );
    grid1->addWidget(lab5,1,1);

    d->m_offsetBaseLine= new KIntNumInput( grpBox );
    lab5->setBuddy( d->m_offsetBaseLine );
    grid1->addWidget(d->m_offsetBaseLine,1,2);

    d->m_offsetBaseLine->setRange(-9, 9, 1,false);
    d->m_offsetBaseLine->setSuffix("pt");

    if(!_withSubSuperScript)
    {
        m_subScript->setEnabled(false);
        m_superScript->setEnabled(false);
        d->m_relativeSize->setEnabled( false );
        d->m_lRelativeSize->setEnabled( false );
    }


#ifdef ATTRCOMBO
    QLabel * lab3 = new QLabel( i18n("A&ttribute:"), grp);
    grid->addWidget( lab3, 6, 0);

    d->m_fontAttribute = new QComboBox( grp );
    lab3->setBuddy( d->m_fontAttribute );
    grid->addWidget( d->m_fontAttribute, 7, 0);

    d->m_fontAttribute->insertStringList( KoTextFormat::fontAttributeList() );
    connect( d->m_fontAttribute, SIGNAL( activated( int ) ), this, SLOT( slotChangeAttributeFont( int )));
#else
    d->m_fontAttribute = new QHButtonGroup( i18n("Capitalization"), grp );
    grid->addMultiCellWidget( d->m_fontAttribute, 6, 6, 0, grid->numCols()-1 );
    QStringList fontAttributes = KoTextFormat::fontAttributeList();
    for( QStringList::Iterator it = fontAttributes.begin(); it != fontAttributes.end(); ++it ) {
        (void) new QRadioButton( *it, d->m_fontAttribute );
    }
    connect( d->m_fontAttribute,  SIGNAL( clicked( int ) ), this, SLOT( slotChangeAttributeFont( int )));
#endif

    QLabel * lab4 = new QLabel( i18n("La&nguage:"), grp);
    grid->addWidget( lab4, 8, 0);

    d->m_language = new QComboBox( grp );
    // TODO insert an icon in front of each language for which spellchecking is available, like in OOo.
    const QStringList langNames = KoGlobal::listOfLanguages();
    const QStringList langTags = KoGlobal::listTagOfLanguages();
    QStringList spellCheckLanguages;
#ifdef HAVE_LIBKSPELL2
    if ( d->m_broker )
        spellCheckLanguages = d->m_broker->languages();
#endif
    QStringList::ConstIterator itName = langNames.begin();
    QStringList::ConstIterator itTag = langTags.begin();
    for ( ; itName != langNames.end() && itTag != langTags.end(); ++itName, ++itTag )
    {
        if ( spellCheckLanguages.find( *itTag ) != spellCheckLanguages.end() )
            d->m_language->insertItem( SmallIcon( "spellcheck" ), *itName );
        else
            d->m_language->insertItem( *itName );
    }
    lab4->setBuddy( d->m_language );
    grid->addWidget( d->m_language, 9, 0 );

    d->m_hyphenation = new QCheckBox( i18n("Auto h&yphenation"), grp );
    grid->addWidget( d->m_hyphenation, 10, 0 );

    // Add one row that can stretch
    grid->expand( grid->numRows() + 1, grid->numCols() );
    grid->setRowStretch( grid->numRows(), 1 );

    connect( d->m_strikeOut, SIGNAL(activated ( int )), this, SLOT( slotStrikeOutTypeChanged( int ) ) );
    connect( m_underlineColorButton, SIGNAL(clicked()), this, SLOT( slotUnderlineColor() ) );
    connect( m_underlining,  SIGNAL( activated ( int ) ), this, SLOT( slotChangeUnderlining( int )));
    connect( m_strikeOutType,  SIGNAL( activated ( int ) ), this, SLOT( slotChangeStrikeOutType( int )));
    connect( m_underlineType,  SIGNAL( activated ( int ) ), this, SLOT( slotChangeUnderlineType( int )));
    connect( d->m_wordByWord, SIGNAL(clicked()), this, SLOT( slotWordByWordClicked() ) );
    connect( d->m_language,  SIGNAL( activated ( int ) ), this, SLOT( slotChangeLanguage( int )));
    connect( d->m_hyphenation, SIGNAL( clicked()), this, SLOT( slotHyphenationClicked()));
    connect( m_subScript, SIGNAL(clicked()), this, SLOT( slotSubScriptClicked() ) );
    connect( m_superScript, SIGNAL(clicked()), this, SLOT( slotSuperScriptClicked() ) );
    connect( d->m_relativeSize, SIGNAL( valueChanged(int) ), this, SLOT( slotRelativeSizeChanged( int )));
    connect( d->m_offsetBaseLine, SIGNAL( valueChanged(int) ), this, SLOT( slotOffsetFromBaseLineChanged( int )));

    updatePositionButton();


}

void KoFontChooser::updatePositionButton()
{
    bool state = (m_superScript->isChecked() || m_subScript->isChecked());
    d->m_relativeSize->setEnabled( state );
    d->m_lRelativeSize->setEnabled( state );
}

void KoFontChooser::setLanguage( const QString & _tag)
{
    d->m_language->setCurrentText( KoGlobal::languageFromTag( _tag ) );
}

QString KoFontChooser::language() const
{
    return KoGlobal::tagOfLanguage( d->m_language->currentText() );
}

KoTextFormat::AttributeStyle KoFontChooser::fontAttribute()const
{
#ifdef ATTRCOMBO
    int currentItem = d->m_fontAttribute->currentItem ();
#else
    int currentItem = 0;
    for ( int i = 0; i < d->m_fontAttribute->count(); ++i )
    {
        if ( d->m_fontAttribute->find( i )->isOn() )
        {
            currentItem = i;
            break;
        }
    }
#endif
    switch ( currentItem )
    {
    case 0:
        return KoTextFormat::ATT_NONE;
        break;
    case 1:
        return KoTextFormat::ATT_UPPER;
        break;
    case 2:
        return KoTextFormat::ATT_LOWER;
        break;
    case 3:
        return KoTextFormat::ATT_SMALL_CAPS;
        break;
    default:
        return KoTextFormat::ATT_NONE;
    }
}

void KoFontChooser::setFontAttribute( KoTextFormat::AttributeStyle _att)
{
    int currentItem = 0;
    if ( _att == KoTextFormat::ATT_NONE)
        currentItem = 0;
    else if ( _att == KoTextFormat::ATT_UPPER)
        currentItem = 1;
    else if ( _att == KoTextFormat::ATT_LOWER )
        currentItem = 2;
    else if ( _att == KoTextFormat::ATT_SMALL_CAPS )
        currentItem = 3;
#ifdef ATTRCOMBO
    d->m_fontAttribute->setCurrentItem( currentItem );
#else
    d->m_fontAttribute->setButton( currentItem );
#endif
}

bool KoFontChooser::wordByWord()const
{
    return d->m_wordByWord->isChecked();
}

void KoFontChooser::setWordByWord( bool _b)
{
    d->m_wordByWord->setChecked( _b);
}


double KoFontChooser::relativeTextSize()const
{
    return ((double)d->m_relativeSize->value()/100.0);
}

void KoFontChooser::setRelativeTextSize(double _size)
{
    d->m_relativeSize->setValue( (int)(_size * 100) );
}

int KoFontChooser::offsetFromBaseLine()const
{
    return d->m_offsetBaseLine->value();
}

void KoFontChooser::setOffsetFromBaseLine(int _offset)
{
    d->m_offsetBaseLine->setValue( _offset );
}

void KoFontChooser::setFont( const QFont &_font, bool _subscript, bool _superscript )
{

    m_newFont = _font;
    kdDebug()<<" setFont m_newFont.bold() :"<<m_newFont.bold()<<" m_newFont.italic():"<<m_newFont.italic()<<endl;
    kdDebug()<<" setfont m_newFont.family() :"<<m_newFont.family()<<endl;

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
    kdDebug()<<" slotFontChanged m_newFont.bold() :"<<f.bold()<<" m_newFont.italic():"<<f.italic()<<endl;
    kdDebug()<<" slotFontChanged m_newFont.family() :"<<f.family()<<endl;

    if ( f.weight() != m_newFont.weight() )
        m_changedFlags |= KoTextFormat::Bold;
    if ( f.italic() != m_newFont.italic() )
        m_changedFlags |= KoTextFormat::Italic;
    if ( f.family() != m_newFont.family() )
        m_changedFlags |= KoTextFormat::Family;
    if ( f.pointSize() != m_newFont.pointSize() )
        m_changedFlags |= KoTextFormat::Size;
    kdDebug(32500) << "KWFontChooser::slotFontChanged m_changedFlags=" << m_changedFlags << endl;
    m_newFont = f;
}

bool KoFontChooser::hyphenation() const
{
    return d->m_hyphenation->isChecked();
}
void KoFontChooser::setHyphenation( bool _b)
{
    d->m_hyphenation->setChecked( _b);
}

void KoFontChooser::slotHyphenationClicked()
{
    m_changedFlags |= KoTextFormat::Hyphenation;
}

void KoFontChooser::slotStrikeOutTypeChanged( int _val)
{
    m_changedFlags |= KoTextFormat::StrikeOut;
    m_strikeOutType->setEnabled( _val!=0 );
    d->m_wordByWord->setEnabled( ( _val != 0 ) || ( m_underlining->currentItem()!=0 ));
}

void KoFontChooser::slotSubScriptClicked()
{
    if(m_superScript->isChecked())
        m_superScript->setChecked(false);
    m_changedFlags |= KoTextFormat::VAlign;
    updatePositionButton();
}

void KoFontChooser::slotSuperScriptClicked()
{
    if(m_subScript->isChecked())
        m_subScript->setChecked(false);
    m_changedFlags |= KoTextFormat::VAlign;
    updatePositionButton();
}

void KoFontChooser::slotRelativeSizeChanged( int )
{
    m_changedFlags |= KoTextFormat::VAlign;
}

void KoFontChooser::slotOffsetFromBaseLineChanged( int )
{
    m_changedFlags |= KoTextFormat::OffsetFromBaseLine;
}

void KoFontChooser::slotShadowChanged()
{
    m_changedFlags |= KoTextFormat::ShadowText;
}

void KoFontChooser::slotWordByWordClicked()
{
    m_changedFlags |= KoTextFormat::WordByWord;
}

void KoFontChooser::slotChangeAttributeFont( int )
{
    m_changedFlags |= KoTextFormat::Attribute;
}

void KoFontChooser::slotChangeLanguage( int )
{
    m_changedFlags |= KoTextFormat::Language;
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

KoTextFormat::UnderlineType KoFontChooser::underlineType() const
{
    switch (m_underlining->currentItem () )
    {
    case 0:
        return KoTextFormat::U_NONE;
        break;
    case 1:
        return KoTextFormat::U_SIMPLE;
        break;
    case 2:
        return KoTextFormat::U_SIMPLE_BOLD;
        break;
    case 3:
        return KoTextFormat::U_DOUBLE;
        break;
    case 4:
        return KoTextFormat::U_WAVE;
        break;
    default:
        return KoTextFormat::U_NONE;
    }

}

KoTextFormat::StrikeOutType KoFontChooser::strikeOutType() const
{
    switch (d->m_strikeOut->currentItem () )
    {
    case 0:
        return KoTextFormat::S_NONE;
        break;
    case 1:
        return KoTextFormat::S_SIMPLE;
        break;
    case 2:
        return KoTextFormat::S_SIMPLE_BOLD;
        break;
    case 3:
        return KoTextFormat::S_DOUBLE;
        break;
    default:
        return KoTextFormat::S_NONE;
    }
}


void KoFontChooser::setStrikeOutlineType(KoTextFormat::StrikeOutType nb)
{
    switch ( nb )
    {
    case KoTextFormat::S_NONE:
        d->m_strikeOut->setCurrentItem(0);
        break;
    case KoTextFormat::S_SIMPLE:
         d->m_strikeOut->setCurrentItem(1);
        break;
    case KoTextFormat::S_SIMPLE_BOLD:
        d->m_strikeOut->setCurrentItem(2);
        break;
    case KoTextFormat::S_DOUBLE:
         d->m_strikeOut->setCurrentItem(3);
        break;
    }
    m_strikeOutType->setEnabled( d->m_strikeOut->currentItem()!= 0);
    d->m_wordByWord->setEnabled( ( d->m_strikeOut->currentItem()!= 0 ) || ( m_underlining->currentItem()!=0 ));

    m_changedFlags = 0;
}


void KoFontChooser::setUnderlineType(KoTextFormat::UnderlineType nb)
{
    switch ( nb )
    {
    case KoTextFormat::U_NONE:
        m_underlining->setCurrentItem(0);
        break;
    case KoTextFormat::U_SIMPLE:
        m_underlining->setCurrentItem(1);
        break;
    case KoTextFormat::U_SIMPLE_BOLD:
        m_underlining->setCurrentItem(2);
        break;
    case KoTextFormat::U_DOUBLE:
        m_underlining->setCurrentItem(3);
        break;
    case KoTextFormat::U_WAVE:
        m_underlining->setCurrentItem(4);
        break;
    default:
        m_underlining->setCurrentItem(0);
        break;
    }
    m_changedFlags = 0;
}

void KoFontChooser::setUnderlineStyle(KoTextFormat::UnderlineStyle _t)
{
    switch ( _t )
    {
    case KoTextFormat::U_SOLID:
        m_underlineType->setCurrentItem( 0 );
        break;
    case KoTextFormat::U_DASH:
        m_underlineType->setCurrentItem( 1 );
        break;
    case KoTextFormat::U_DOT:
        m_underlineType->setCurrentItem( 2 );
        break;
    case KoTextFormat::U_DASH_DOT:
        m_underlineType->setCurrentItem( 3 );
        break;
    case KoTextFormat::U_DASH_DOT_DOT:
        m_underlineType->setCurrentItem( 4 );
        break;
    default:
        m_underlineType->setCurrentItem( 0 );
        break;
    }
    m_underlineType->setEnabled( m_underlining->currentItem()!= 0);
    m_underlineColorButton->setEnabled( m_underlining->currentItem()!=0);

    d->m_wordByWord->setEnabled( ( d->m_strikeOut->currentItem()!= 0 ) || ( m_underlining->currentItem()!=0 ));

    m_changedFlags = 0;
}

void KoFontChooser::setStrikeOutStyle(KoTextFormat::StrikeOutStyle _t)
{
    switch ( _t )
    {
    case KoTextFormat::S_NONE:
        m_strikeOutType->setCurrentItem( 0 );
        break;
    case KoTextFormat::S_DASH:
        m_strikeOutType->setCurrentItem( 1 );
        break;
    case KoTextFormat::S_DOT:
        m_strikeOutType->setCurrentItem( 2 );
        break;
    case KoTextFormat::S_DASH_DOT:
        m_strikeOutType->setCurrentItem( 3 );
        break;
    case KoTextFormat::S_DASH_DOT_DOT:
        m_strikeOutType->setCurrentItem( 4 );
        break;
    default:
        m_strikeOutType->setCurrentItem( 0 );
        break;
    }
    m_changedFlags = 0;
}

KoTextFormat::UnderlineStyle KoFontChooser::underlineStyle() const
{
    switch ( m_underlineType->currentItem() )
    {
    case 0:
        return KoTextFormat::U_SOLID;
        break;
    case 1:
        return KoTextFormat::U_DASH;
        break;
    case 2:
        return KoTextFormat::U_DOT;
        break;
    case 3:
        return KoTextFormat::U_DASH_DOT;
        break;
    case 4:
        return KoTextFormat::U_DASH_DOT_DOT;
        break;
    default:
        return KoTextFormat::U_SOLID;
    }
}

KoTextFormat::StrikeOutStyle KoFontChooser::strikeOutStyle() const
{

    switch ( m_strikeOutType->currentItem() )
    {
    case 0:
        return KoTextFormat::S_SOLID;
        break;
    case 1:
        return KoTextFormat::S_DASH;
        break;
    case 2:
        return KoTextFormat::S_DOT;
        break;
    case 3:
        return KoTextFormat::S_DASH_DOT;
        break;
    case 4:
        return KoTextFormat::S_DASH_DOT_DOT;
        break;
    default:
        return KoTextFormat::S_SOLID;
    }
}


void KoFontChooser::slotChangeUnderlineType( int /*i*/)
{
    m_changedFlags |= KoTextFormat::ExtendUnderLine;
}

void KoFontChooser::slotChangeUnderlining( int i)
{
    m_changedFlags |= KoTextFormat::ExtendUnderLine;
    m_underlineType->setEnabled( i!= 0);
    m_underlineColorButton->setEnabled( i!=0);
    d->m_wordByWord->setEnabled( ( d->m_strikeOut->currentItem()!= 0 ) || ( i !=0 ));

}

void KoFontChooser::slotChangeStrikeOutType( int /*i*/ )
{
    m_changedFlags |= KoTextFormat::StrikeOut;
}


void KoFontChooser::setShadow( double shadowDistanceX, double shadowDistanceY, const QColor& shadowColor )
{
    d->m_shadowWidget->setShadow( shadowDistanceX, shadowDistanceY, shadowColor );
}

double KoFontChooser::shadowDistanceX() const
{
    return d->m_shadowWidget->shadowDistanceX();
}

double KoFontChooser::shadowDistanceY() const
{
    return d->m_shadowWidget->shadowDistanceY();
}

QColor KoFontChooser::shadowColor() const
{
    return d->m_shadowWidget->shadowColor();
}

void KoFontChooser::setFormat( const KoTextFormat& format )
{
    setFont( format.font(),
                        format.vAlign() & KoTextFormat::AlignSubScript,
                        format.vAlign() & KoTextFormat::AlignSuperScript );
    setColor( format.color() );
    setBackGroundColor( format.textBackgroundColor() );
    setUnderlineColor( format.textUnderlineColor() );

    setUnderlineType( format.underlineType() );
    setUnderlineStyle( format.underlineStyle() );

    setStrikeOutlineType( format.strikeOutType() );
    setStrikeOutStyle( format.strikeOutStyle() );

    setShadow( format.shadowDistanceX(), format.shadowDistanceY(), format.shadowColor() );

    setWordByWord( format.wordByWord() );
    setRelativeTextSize( format.relativeTextSize() );
    setOffsetFromBaseLine( format.offsetFromBaseLine() );
    setFontAttribute( format.attributeFont() );
    setLanguage( format.language() );

    setHyphenation( format.hyphenation() );

    updatePositionButton();
}

KoTextFormat KoFontChooser::newFormat() const
{
    return KoTextFormat( newFont(),
                         vAlign(),
                         color(),
                         backGroundColor(),
                         underlineColor(),
                         underlineType(),
                         underlineStyle(),
                         strikeOutType(),
                         strikeOutStyle(),
                         fontAttribute(),
                         language(),
                         relativeTextSize(),
                         offsetFromBaseLine(),
                         wordByWord(),
                         hyphenation(),
                         shadowDistanceX(),
                         shadowDistanceY(),
                         shadowColor() );
}

//////////

KoFontDia::KoFontDia( const KoTextFormat& initialFormat, QWidget* parent, const char* name )
    : KDialogBase( parent, name, true,
                   i18n("Select Font"), Ok|Cancel|User1|Apply, Ok ),
      m_initialFormat(initialFormat)
{
    m_chooser = new KoFontChooser( this, "kofontchooser", true /*_withSubSuperScript*/,
                                   KFontChooser::SmoothScalableFonts);
    init();
}

#ifdef HAVE_LIBKSPELL2
KoFontDia::KoFontDia( const KoTextFormat& initialFormat,
                      KSpell2::Broker::Ptr broker,
                      QWidget* parent, const char* name )
    : KDialogBase( parent, name, true,
                   i18n("Select Font"), Ok|Cancel|User1|Apply, Ok ),
      m_initialFormat(initialFormat)
{
    m_chooser = new KoFontChooser( this, "kofontchooser", true /*_withSubSuperScript*/,
                                   KFontChooser::SmoothScalableFonts, broker );
    init();
}
#endif

void KoFontDia::init()
{
    setButtonText( KDialogBase::User1, i18n("&Reset") );

    setMainWidget( m_chooser );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotReset()) );

    slotReset();
}

void KoFontDia::slotApply()
{
    emit applyFont();
}

void KoFontDia::slotOk()
{
    slotApply();
    KDialogBase::slotOk();
}

void KoFontDia::slotReset()
{
    m_chooser->setFormat( m_initialFormat );
}

#include "koFontDia.moc"
#include "koFontDia_p.moc"
