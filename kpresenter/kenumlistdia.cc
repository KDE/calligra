/******************************************************************/
/* KEnumListDia - (c) by Reginald Stadlbauer 1998                 */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KEnumListDia is under GNU GPL                                  */
/******************************************************************/
/* Module: Enumerated List Dialog                                 */
/******************************************************************/

#include "kenumlistdia.h"
#include "kenumlistdia.moc"

#include <qlabel.h>
#include <qpushbt.h>
#include <qlayout.h>
#include <qcombo.h>
#include <qstringlist.h>

#include <qchkbox.h>
#include <qlined.h>
#include <qradiobt.h>

#include <kapp.h>
#include <kbuttonbox.h>
#include <kcolorbtn.h>

#include <X11/Xlib.h>

#include <stdio.h>

/******************************************************************/
/* class KEnumListDia                                             */
/******************************************************************/

/*======================= constructor ============================*/
KEnumListDia::KEnumListDia( QWidget* parent, const char* name, int __type, QFont __font,
                            QColor __color, QString __before, QString __after, int __start, QStringList _fontList )
    : QDialog( parent, name, true )
{
    _type = __type;
    _font = __font;
    _color = __color;
    _before = __before;
    _after = __after;
    _start = __start;

    setCaption( "Configure the list" );

    grid = new QGridLayout( this, 8, 5, 15, 7 );

    fontList = _fontList;

    lFont = new QLabel( "Font:", this );
    lFont->resize( lFont->sizeHint() );
    grid->addWidget( lFont, 0, 0 );

    fontCombo = new QComboBox( false, this );
    fontCombo->insertStringList( fontList );
    fontCombo->resize( fontCombo->sizeHint() );
    QValueList<QString>::Iterator it = fontList.find( _font.family().lower() );
    if ( it != fontList.end() )
    {
        int pos = 0;
        QValueList<QString>::Iterator it2 = fontList.begin();
        for ( ; it != it2; ++it2, ++pos );
        fontCombo->setCurrentItem( pos );
    }
    grid->addMultiCellWidget( fontCombo, 1, 1, 0, 2 );
    connect( fontCombo, SIGNAL( activated( const QString & ) ), this, SLOT( fontSelected( const QString & ) ) );

    lSize = new QLabel( "Size:", this );
    lSize->resize( lSize->sizeHint() );
    grid->addWidget( lSize, 0, 3 );

    sizeCombo = new QComboBox( false, this );
    char chr[ 5 ];
    for ( unsigned int i = 4; i <= 100; i++ )
    {
        sprintf( chr, "%d", i );
        sizeCombo->insertItem( chr, -1 );
    }
    sizeCombo->resize( sizeCombo->sizeHint() );
    grid->addWidget( sizeCombo, 1, 3 );
    sizeCombo->setCurrentItem( _font.pointSize()-4 );
    connect( sizeCombo, SIGNAL( activated( int ) ), this, SLOT( sizeSelected( int ) ) );

    lColor = new QLabel( "Color:", this );
    lColor->resize( lColor->sizeHint() );
    grid->addWidget( lColor, 0, 4 );

    colorButton = new KColorButton( _color, this );
    colorButton->resize( colorButton->sizeHint() );
    grid->addWidget( colorButton, 1, 4 );
    connect( colorButton, SIGNAL( changed( const QColor& ) ), this, SLOT( colorChanged( const QColor& ) ) );

    lAttrib = new QLabel( "Attributes:", this );
    lAttrib->resize( lAttrib->sizeHint() );
    grid->addMultiCellWidget( lAttrib, 2, 2, 0, 2 );

    bold = new QCheckBox( "Bold", this );
    bold->resize( bold->sizeHint() );
    grid->addWidget( bold, 3, 0 );
    bold->setChecked( _font.bold() );
    connect( bold, SIGNAL( clicked() ), this, SLOT( boldChanged() ) );

    italic= new QCheckBox( "Italic", this );
    italic->resize( italic->sizeHint() );
    grid->addWidget( italic, 3, 1 );
    italic->setChecked( _font.italic() );
    connect( italic, SIGNAL( clicked() ), this, SLOT( italicChanged() ) );

    underl = new QCheckBox( "Underlined", this );
    underl->resize( underl->sizeHint() );
    grid->addWidget( underl, 3, 2 );
    underl->setChecked( _font.underline() );
    connect( underl, SIGNAL( clicked() ), this, SLOT( underlChanged() ) );

    lBefore = new QLabel( "Before:", this );
    lBefore->resize( lBefore->sizeHint() );
    grid->addWidget( lBefore, 4, 0 );

    eBefore = new QLineEdit( this );
    eBefore->resize( lBefore->width(), eBefore->sizeHint().height() );
    eBefore->setMaxLength( 4 );
    eBefore->setText( _before.data() );
    grid->addWidget( eBefore, 5, 0 );
    connect( eBefore, SIGNAL( textChanged( const QString & ) ), this, SLOT( beforeChanged( const QString & ) ) );

    lAfter = new QLabel( "After:", this );
    lAfter->resize( lAfter->sizeHint() );
    grid->addWidget( lAfter, 4, 1 );

    eAfter = new QLineEdit( this );
    eAfter->resize( lAfter->width(), eAfter->sizeHint().height() );
    eAfter->setMaxLength( 4 );
    eAfter->setText( _after.data() );
    grid->addWidget( eAfter, 5, 1 );
    connect( eAfter, SIGNAL( textChanged( const QString & ) ), this, SLOT( afterChanged( const QString & ) ) );

    lStart = new QLabel( "Start:", this );
    lStart->resize( lStart->sizeHint() );
    grid->addWidget( lStart, 4, 2 );

    eStart = new QLineEdit( this );
    eStart->resize( lStart->width(), eStart->sizeHint().height() );
    eStart->setMaxLength( 1 );
    if ( _type == 1 )
        sprintf( chr, "%d", _start );
    else
        sprintf( chr, "%c", _start );
    eStart->setText( chr );
    grid->addWidget( eStart, 5, 2 );
    connect( eStart, SIGNAL( textChanged( const QString & ) ), this, SLOT( startChanged( const QString & ) ) );
    _start = QChar( eStart->text()[ 0 ] );

    number = new QRadioButton( "Numeric", this );
    number->resize( number->sizeHint() );
    grid->addWidget( number, 4, 4 );
    connect( number, SIGNAL( clicked() ), this, SLOT( numChanged() ) );

    alphabeth = new QRadioButton( "Alphabethic", this );
    alphabeth->resize( alphabeth->sizeHint() );
    grid->addWidget( alphabeth, 5, 4 );
    connect( alphabeth, SIGNAL( clicked() ), this, SLOT( alphaChanged() ) );

    if ( _type == NUMBER )
        number->setChecked( true );
    else
        alphabeth->setChecked( true );

    bbox = new KButtonBox( this, KButtonBox::HORIZONTAL, 7 );
    bbox->addStretch( 20 );
    bOk = bbox->addButton( "OK" );
    bOk->setAutoRepeat( false );
    bOk->setAutoResize( false );
    bOk->setAutoDefault( true );
    bOk->setDefault( true );
    connect( bOk, SIGNAL( clicked() ), SLOT( accept() ) );
    bCancel = bbox->addButton( "Cancel" );
    connect( bCancel, SIGNAL( clicked() ), SLOT( reject() ) );
    bbox->layout();
    grid->addWidget( bbox, 7, 4 );

//   grid->addColSpacing( 0, lFont->width() );
//   grid->addColSpacing( 0, fontCombo->width()/3 );
//   grid->addColSpacing( 0, bold->width() );
//   grid->addColSpacing( 0, lBefore->width() );
//   grid->addColSpacing( 1, fontCombo->width()/3 );
//   grid->addColSpacing( 1, italic->width() );
//   grid->addColSpacing( 1, lAfter->width() );
//   grid->addColSpacing( 2, fontCombo->width()/3 );
//   grid->addColSpacing( 2, underl->width() );
//   grid->addColSpacing( 2, lStart->width() );
//   grid->addColSpacing( 3, lSize->width() );
//   grid->addColSpacing( 3, sizeCombo->width() );
//   grid->addColSpacing( 4, lColor->width() );
//   grid->addColSpacing( 4, colorButton->width() );
//   grid->addColSpacing( 4, number->width() );
//   grid->addColSpacing( 4, alphabeth->width() );

//   grid->addRowSpacing( 0, lFont->height() );
//   grid->addRowSpacing( 0, lSize->height() );
//   grid->addRowSpacing( 0, lColor->height() );
//   grid->addRowSpacing( 1, fontCombo->height() );
//   grid->addRowSpacing( 1, sizeCombo->height() );
//   grid->addRowSpacing( 1, colorButton->height() );
//   grid->addRowSpacing( 2, lAttrib->height() );
//   grid->addRowSpacing( 3, bold->height() );
//   grid->addRowSpacing( 3, italic->height() );
//   grid->addRowSpacing( 3, underl->height() );
//   grid->addRowSpacing( 4, lBefore->height() );
//   grid->addRowSpacing( 4, lAfter->height() );
//   grid->addRowSpacing( 4, lStart->height() );
//   grid->addRowSpacing( 4, number->height() );
//   grid->addRowSpacing( 5, eBefore->height() );
//   grid->addRowSpacing( 5, eAfter->height() );
//   grid->addRowSpacing( 5, eStart->height() );
//   grid->addRowSpacing( 5, alphabeth->height() );
//   grid->addRowSpacing( 6, 10 );
//   grid->activate();

//   resize( 0, 0 );
//   setMaximumSize( size() );
//   setMinimumSize( size() );
}

/*========================= destructor ===========================*/
KEnumListDia::~KEnumListDia()
{
}

/*====================== show enum list dialog ===================*/
bool KEnumListDia::enumListDia( int& __type, QFont& __font, QColor& __color,
                                QString& __before, QString& __after, int& __start, QStringList _fontList )
{
    bool res = false;

    KEnumListDia *dlg = new KEnumListDia( 0, "EnumListDia", __type, __font, __color,
                                          __before, __after, __start, _fontList );

    if ( dlg->exec() == QDialog::Accepted )
    {
        __type = dlg->type();
        __font = dlg->font();
        __color = dlg->color();
        __before = dlg->before();
        __after = dlg->after();
        __start = dlg->start();
        if ( __type == 1 ) __start -= 48;
        res = true;
    }

    delete dlg;

    return res;
}

/*=========================== Font selected =====================*/
void KEnumListDia::fontSelected( const QString &_family )
{
    _font.setFamily( _family.lower() );
}

/*===================== size selected ===========================*/
void KEnumListDia::sizeSelected( int i )
{
    _font.setPointSize( i+4 );
}

/*===================== color selected ==========================*/
void KEnumListDia::colorChanged( const QColor& __color )
{
    _color = __color;
}

/*====================== bold clicked ===========================*/
void KEnumListDia::boldChanged()
{
    _font.setBold( bold->isChecked() );
}

/*====================== italic clicked =========================*/
void KEnumListDia::italicChanged()
{
    _font.setItalic( italic->isChecked() );
}

/*====================== underline clicked ======================*/
void KEnumListDia::underlChanged()
{
    _font.setUnderline( underl->isChecked() );
}

/*======================= before changed ========================*/
void KEnumListDia::beforeChanged( const QString & str )
{
    _before = str;
}

/*======================= after changed =========================*/
void KEnumListDia::afterChanged( const QString & str )
{
    _after = str;
}


/*======================= start changed =========================*/
void KEnumListDia::startChanged( const QString & str )
{
    _start = str[ 0 ];
}

/*======================== rb number changed ====================*/
void KEnumListDia::numChanged()
{
    _type = 1;
    alphabeth->setChecked( false );
}

/*======================== rb alphabeth changed =================*/
void KEnumListDia::alphaChanged()
{
    _type = 2;
    number->setChecked( false );
}


