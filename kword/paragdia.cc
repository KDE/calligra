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

#include <paragdia.h>
#include <kwdoc.h>
#include <kcharselectdia.h>
#include <defs.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qhbuttongroup.h>
#include <qlistbox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qwhatsthis.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kcolorbtn.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <koRuler.h>

#include <stdlib.h>
#include <stdio.h>

/******************************************************************/
/* class KWPagePreview                                            */
/******************************************************************/

/*================================================================*/
KWPagePreview::KWPagePreview( QWidget* parent, const char* name )
    : QGroupBox( i18n( "Preview" ), parent, name )
{
    left = 0;
    right = 0;
    first = 0;
    spacing = 0;
    before = 0;
    after = 0;
}

/*================================================================*/
void KWPagePreview::drawContents( QPainter* p )
{
    int wid = 148;
    int hei = 210;
    int _x = ( width() - wid ) / 2;
    int _y = ( height() - hei ) / 2;

    int dl = static_cast<int>( left / 2 );
    int dr = static_cast<int>( right / 2 );
    //first+left because firstlineIndent is relative to leftIndent
    int df = static_cast<int>( (first+left) / 2 );

    int spc = static_cast<int>( POINT_TO_MM( spacing ) / 5 );

    // draw page
    p->setPen( QPen( black ) );
    p->setBrush( QBrush( black ) );

    p->drawRect( _x + 1, _y + 1, wid, hei );

    p->setBrush( QBrush( white ) );
    p->drawRect( _x, _y, wid, hei );

    // draw parags
    p->setPen( NoPen );
    p->setBrush( QBrush( lightGray ) );

    for ( int i = 1; i <= 4; i++ )
        p->drawRect( _x + 6, _y + 6 + ( i - 1 ) * 12 + 2, wid - 12 - ( ( i / 4 ) * 4 == i ? 50 : 0 ), 6 );

    p->setBrush( QBrush( darkGray ) );

    for ( int i = 5; i <= 8; i++ )
      {
	QRect rect( ( i == 5 ? df : dl ) + _x + 6, _y + 6 + ( i - 1 ) * 12 + 2 + ( i - 5 ) * spc + static_cast<int>( before / 2 ),
		    wid - 12 - ( ( i / 4 ) * 4 == i ? 50 : 0 ) - ( ( i == 12 ? 0 : dr ) + ( i == 5 ? df : dl ) ), 6);

	if(rect.width ()>=0)
	  p->drawRect( rect );
      }
    p->setBrush( QBrush( lightGray ) );

    for ( int i = 9; i <= 12; i++ )
        p->drawRect( _x + 6, _y + 6 + ( i - 1 ) * 12 + 2 + 3 * spc +
                     static_cast<int>( before / 2 ) + static_cast<int>( after / 2 ),
                     wid - 12 - ( ( i / 4 ) * 4 == i ? 50 : 0 ), 6 );

}

/******************************************************************/
/* class KWPagePreview2                                           */
/******************************************************************/

/*================================================================*/
KWPagePreview2::KWPagePreview2( QWidget* parent, const char* name )
    : QGroupBox( i18n( "Preview" ), parent, name )
{
    align = Qt::AlignLeft;
}

/*================================================================*/
void KWPagePreview2::drawContents( QPainter* p )
{
    int wid = 148;
    int hei = 210;
    int _x = ( width() - wid ) / 2;
    int _y = ( height() - hei ) / 2;

    // draw page
    p->setPen( QPen( black ) );
    p->setBrush( QBrush( black ) );

    p->drawRect( _x + 1, _y + 1, wid, hei );

    p->setBrush( QBrush( white ) );
    p->drawRect( _x, _y, wid, hei );

    // draw parags
    p->setPen( NoPen );
    p->setBrush( QBrush( lightGray ) );

    for ( int i = 1; i <= 4; i++ )
        p->drawRect( _x + 6, _y + 6 + ( i - 1 ) * 12 + 2, wid - 12 - ( ( i / 4 ) * 4 == i ? 50 : 0 ), 6 );

    p->setBrush( QBrush( darkGray ) );

    int __x = 0, __w = 0;
    for ( int i = 5; i <= 8; i++ ) {
        switch ( i ) {
        case 5: __w = wid - 12;
            break;
        case 6: __w = wid - 52;
            break;
        case 7: __w = wid - 33;
            break;
        case 8: __w = wid - 62;
        default: break;
        }

        switch ( align ) {
            case Qt3::AlignAuto:
            case Qt::AlignLeft:
                __x = _x + 6;
                break;
            case Qt::AlignCenter:
                __x = _x + ( wid - __w ) / 2;
                break;
            case Qt::AlignRight:
                __x = _x + ( wid - __w ) - 6;
                break;
            case Qt3::AlignJustify:
            {
                if ( i < 8 ) __w = wid - 12;
                __x = _x + 6;
            } break;
        }

        p->drawRect( __x, _y + 6 + ( i - 1 ) * 12 + 2 + ( i - 5 ), __w, 6 );
    }

    p->setBrush( QBrush( lightGray ) );

    for ( int i = 9; i <= 12; i++ )
        p->drawRect( _x + 6, _y + 6 + ( i - 1 ) * 12 + 2 + 3, wid - 12 - ( ( i / 4 ) * 4 == i ? 50 : 0 ), 6 );

}

/******************************************************************/
/* class KWBorderPreview                                          */
/******************************************************************/


/*================================================================*/
KWBorderPreview::KWBorderPreview( QWidget* parent, const char* name )
    :QFrame(parent,name)
{
}

/*================================================================*/
void KWBorderPreview::mousePressEvent( QMouseEvent *_ev )
{
    emit choosearea(_ev);
}


/*================================================================*/
void KWBorderPreview::drawContents( QPainter* painter )
{
    QRect r = contentsRect();
    QFontMetrics fm( font() );

    painter->fillRect( r.x() + fm.width( 'W' ), r.y() + fm.height(), r.width() - 2 * fm.width( 'W' ),
                       r.height() - 2 * fm.height(), white );
    painter->setClipRect( r.x() + fm.width( 'W' ), r.y() + fm.height(), r.width() - 2 * fm.width( 'W' ),
                          r.height() - 2 * fm.height() );

    if ( topBorder.ptWidth > 0 ) {
        painter->setPen( setBorderPen( topBorder ) );
        painter->drawLine( r.x() + 20, r.y() + 20, r.right() - 20, r.y() + 20 );
    }

    if ( bottomBorder.ptWidth > 0 ) {
        painter->setPen( setBorderPen( bottomBorder ) );
        painter->drawLine( r.x() + 20, r.bottom() - 20, r.right() - 20, r.bottom() - 20 );
    }

    if ( leftBorder.ptWidth > 0 ) {
        painter->setPen( setBorderPen( leftBorder ) );
        painter->drawLine( r.x() + 20, r.y() + 20, r.x() + 20, r.bottom() - 20 );
    }

    if ( rightBorder.ptWidth > 0 ) {
        painter->setPen( setBorderPen( rightBorder ) );
        painter->drawLine( r.right() - 20, r.y() + 20, r.right() - 20, r.bottom() - 20 );
    }
}

/*================================================================*/
QPen KWBorderPreview::setBorderPen( Border _brd )
{
    QPen pen( black, 1, SolidLine );

    pen.setWidth( _brd.ptWidth );
    pen.setColor( _brd.color );

    switch ( _brd.style ) {
    case Border::SOLID:
        pen.setStyle( SolidLine );
        break;
    case Border::DASH:
        pen.setStyle( DashLine );
        break;
    case Border::DOT:
        pen.setStyle( DotLine );
        break;
    case Border::DASH_DOT:
        pen.setStyle( DashDotLine );
        break;
    case Border::DASH_DOT_DOT:
        pen.setStyle( DashDotDotLine );
        break;
    }

    return QPen( pen );
}

/******************************************************************/
/* class KWNumPreview                                             */
/******************************************************************/

/*================================================================*/
KWNumPreview::KWNumPreview( QWidget* parent, const char* name )
    : QGroupBox( i18n( "Preview" ), parent, name )
{
}

/*================================================================*/
void KWNumPreview::drawContents( QPainter* )
{
}

/******************************************************************/
/* Class: KWParagDia                                              */
/******************************************************************/

/*================================================================*/
KWParagDia::KWParagDia( QWidget* parent, const char* name, QStringList _fontList,
                        int _flags, KWDocument *_doc )
    : KDialogBase(Tabbed, QString::null, Ok | Cancel, Ok, parent, name, true )
{
    flags = _flags;
    fontList = _fontList;
    doc = _doc;
    unit=KWUnit::unitType( doc->getUnit() );
    if ( _flags & PD_SPACING )
        setupTab1();
    if ( _flags & PD_ALIGN )
        setupTab2();
    if ( _flags & PD_BORDERS )
        setupTab3();
    if ( _flags & PD_NUMBERING )
        setupTab4();
    if ( _flags & PD_TABS )
        setupTab5();
    m_bListTabulatorChanged=false;
    setInitialSize( QSize(600, 500) );
}

/*================================================================*/
KWParagDia::~KWParagDia()
{
}

/*================================================================*/
void KWParagDia::setLeftIndent( KWUnit _left )
{
    kdDebug() << "KWParagDia::setLeftIndent mm=" << _left.mm() << " pt=" << _left.pt() << endl;
    QString str = QString::number( _left.value( unit ) );
    eLeft->setText( str );
    prev1->setLeft( _left.mm() );
}

/*================================================================*/
void KWParagDia::setRightIndent( KWUnit _right )
{
    kdDebug() << "KWParagDia::setRightIndent mm=" << _right.mm() << " pt=" << _right.pt() << endl;
    QString str = QString::number( _right.value( unit ) );
    eRight->setText( str );
    prev1->setRight( _right.mm() );
}

/*================================================================*/
void KWParagDia::setFirstLineIndent( KWUnit _first )
{
    QString str = QString::number( _first.value( unit ) );
    eFirstLine->setText( str );
    prev1->setFirst( _first.mm() );
}

/*================================================================*/
void KWParagDia::setSpaceBeforeParag( KWUnit _before )
{
    QString str = QString::number( _before.value( unit ) );
    eBefore->setText( str );
    prev1->setBefore( _before.mm() );
}

/*================================================================*/
void KWParagDia::setSpaceAfterParag( KWUnit _after )
{
    QString str = QString::number( _after.value( unit ) );
    eAfter->setText( str );
    prev1->setAfter( _after.mm() );
}

/*================================================================*/
void KWParagDia::setLineSpacing( KWUnit _spacing )
{
    QString str = QString::number( _spacing.value( unit ) );
    eSpacing->setText( str );
    prev1->setSpacing( _spacing.mm() );
}

/*================================================================*/
void KWParagDia::setAlign( int align )
{
    prev2->setAlign( align );

    clearAligns();
    switch ( align ) {
        case Qt3::AlignAuto: // see KWView::setAlign
        case Qt::AlignLeft:
            rLeft->setChecked( true );
            break;
        case Qt::AlignCenter:
            rCenter->setChecked( true );
            break;
        case Qt::AlignRight:
            rRight->setChecked( true );
            break;
        case Qt3::AlignJustify:
            rJustify->setChecked( true );
            break;
    }
}

/*================================================================*/
int KWParagDia::align() const
{
    if ( rLeft->isChecked() ) return Qt::AlignLeft;
    else if ( rCenter->isChecked() ) return Qt::AlignCenter;
    else if ( rRight->isChecked() ) return Qt::AlignRight;
    else if ( rJustify->isChecked() ) return Qt3::AlignJustify;

    return Qt::AlignLeft;
}

/*================================================================*/
void KWParagDia::setupTab1()
{
    QWidget *tab = addPage( i18n( "Indent and Spacing" ) );
    QGridLayout *grid = new QGridLayout( tab, 4, 2, 15, 7 );

    // --------------- indent ---------------
    indentFrame = new QGroupBox( i18n( "Indent" ), tab );
    indentGrid = new QGridLayout( indentFrame, 4, 2, 15, 7 );

    lLeft = new QLabel( i18n("Left ( %1 ):").arg(doc->getUnit()), indentFrame );
    lLeft->setAlignment( AlignRight );
    indentGrid->addWidget( lLeft, 1, 0 );

    eLeft = new QLineEdit( indentFrame );
    if ( unit == U_PT )
        eLeft->setValidator( new QIntValidator( eLeft ) );
    else
        eLeft->setValidator( new QDoubleValidator( eLeft ) );
    eLeft->setText( i18n("0.00") );
    eLeft->setMaxLength( 5 );
    eLeft->setEchoMode( QLineEdit::Normal );
    eLeft->setFrame( true );
    indentGrid->addWidget( eLeft, 1, 1 );
    connect( eLeft, SIGNAL( textChanged( const QString & ) ), this, SLOT( leftChanged( const QString & ) ) );

    lRight = new QLabel( i18n("Right ( %1 ):").arg(doc->getUnit()), indentFrame );
    lRight->setAlignment( AlignRight );
    indentGrid->addWidget( lRight, 2, 0 );

    eRight = new QLineEdit( indentFrame );
    if ( unit == U_PT )
        eRight->setValidator( new QIntValidator( eRight ) );
    else
        eRight->setValidator( new QDoubleValidator( eRight ) );
    eRight->setText( i18n("0.00") );
    eRight->setMaxLength( 5 );
    eRight->setEchoMode( QLineEdit::Normal );
    eRight->setFrame( true );
    indentGrid->addWidget( eRight, 2, 1 );
    connect( eRight, SIGNAL( textChanged( const QString & ) ), this, SLOT( rightChanged( const QString & ) ) );

    lFirstLine = new QLabel( i18n("First Line ( %1 ):").arg(doc->getUnit()), indentFrame );
    lFirstLine->setAlignment( AlignRight );
    indentGrid->addWidget( lFirstLine, 3, 0 );

    eFirstLine = new QLineEdit( indentFrame );
    if ( unit == U_PT )
        eFirstLine->setValidator( new QIntValidator( eFirstLine ) );
    else
        eFirstLine->setValidator( new QDoubleValidator( eFirstLine ) );
    eFirstLine->setText( i18n("0.00") );
    eFirstLine->setMaxLength( 5 );
    eFirstLine->setEchoMode( QLineEdit::Normal );
    eFirstLine->setFrame( true );
    connect( eFirstLine, SIGNAL( textChanged( const QString & ) ), this, SLOT( firstChanged( const QString & ) ) );
    indentGrid->addWidget( eFirstLine, 3, 1 );

     // grid row spacing
    indentGrid->addRowSpacing( 0, 5 );
    grid->addWidget( indentFrame, 0, 0 );

    // --------------- spacing ---------------
    spacingFrame = new QGroupBox( i18n( "Line Spacing" ), tab );
    spacingGrid = new QGridLayout( spacingFrame, 3, 1, 15, 7 );

    cSpacing = new QComboBox( false, spacingFrame, "" );
    cSpacing->insertItem( i18n( "0.5 lines" ) );
    cSpacing->insertItem( i18n( "1.0 line" ) );
    cSpacing->insertItem( i18n( "1.5 lines" ) );
    cSpacing->insertItem( i18n( "2.0 lines" ) );
    cSpacing->insertItem( i18n( "Space ( %1 )" ).arg(doc->getUnit()) );
    connect( cSpacing, SIGNAL( activated( int ) ), this, SLOT( spacingActivated( int ) ) );
    spacingGrid->addWidget( cSpacing, 1, 0 );

    eSpacing = new QLineEdit( spacingFrame );
    if ( unit == U_PT )
        eSpacing->setValidator( new QIntValidator( eSpacing ) );
    else
        eSpacing->setValidator( new QDoubleValidator( eSpacing ) );
    eSpacing->setText( i18n("0") );
    eSpacing->setMaxLength( 2 );
    eSpacing->setEchoMode( QLineEdit::Normal );
    eSpacing->setFrame( true );
    connect( eSpacing, SIGNAL( textChanged( const QString & ) ), this, SLOT( spacingChanged( const QString & ) ) );
    spacingGrid->addWidget( eSpacing, 2, 0 );


    // grid row spacing
    spacingGrid->addRowSpacing( 0, 5 );
    grid->addWidget( spacingFrame, 1, 0 );

    cSpacing->setCurrentItem( 4 );
    cSpacing->setEnabled( false ); // TODO: handle 0.5 lines, 1 line etc
    eSpacing->setEnabled( true );

    // --------------- paragraph spacing ---------------
    pSpaceFrame = new QGroupBox( i18n( "Paragraph Space" ), tab );
    pSpaceGrid = new QGridLayout( pSpaceFrame, 3, 2, 15, 7 );

    lBefore = new QLabel( i18n("Before ( %1 ):").arg(doc->getUnit()), pSpaceFrame );
    lBefore->setAlignment( AlignRight );
    pSpaceGrid->addWidget( lBefore, 1, 0 );

    eBefore = new QLineEdit( pSpaceFrame );
    if ( unit == U_PT )
        eBefore->setValidator( new QIntValidator( eBefore ) );
    else
        eBefore->setValidator( new QDoubleValidator( eBefore ) );
    eBefore->setText( i18n("0.00") );
    eBefore->setMaxLength( 5 );
    eBefore->setEchoMode( QLineEdit::Normal );
    eBefore->setFrame( true );
    connect( eBefore, SIGNAL( textChanged( const QString & ) ), this, SLOT( beforeChanged( const QString & ) ) );
    pSpaceGrid->addWidget( eBefore, 1, 1 );

    lAfter = new QLabel( i18n("After ( %1 ):").arg(doc->getUnit()), pSpaceFrame );
    lAfter->setAlignment( AlignRight );
    pSpaceGrid->addWidget( lAfter, 2, 0 );

    eAfter = new QLineEdit( pSpaceFrame );
    if ( unit == U_PT )
        eAfter->setValidator( new QIntValidator( eAfter ) );
    else
        eAfter->setValidator( new QDoubleValidator( eAfter ) );
    eAfter->setText( i18n("0.00") );
    eAfter->setMaxLength( 5 );
    eAfter->setEchoMode( QLineEdit::Normal );
    eAfter->setFrame( true );
    connect( eAfter, SIGNAL( textChanged( const QString & ) ), this, SLOT( afterChanged( const QString & ) ) );
    pSpaceGrid->addWidget( eAfter, 2, 1 );

    // grid row spacing
    pSpaceGrid->addRowSpacing( 0, 5 );
    grid->addWidget( pSpaceFrame, 2, 0 );

    // --------------- preview --------------------
    prev1 = new KWPagePreview( tab );
    grid->addMultiCellWidget( prev1, 0, 3, 1, 1 );

    grid->setColStretch( 1, 1 );
    grid->setRowStretch( 3, 1 );
}

/*================================================================*/
void KWParagDia::setupTab2()
{
    QWidget *tab = addPage( i18n( "Aligns" ) );
    QGridLayout *grid = new QGridLayout( tab, 6, 2, 15, 7 );

    lAlign = new QLabel( i18n( "Align:" ), tab );
    grid->addWidget( lAlign, 0, 0 );

    rLeft = new QRadioButton( i18n( "Left" ), tab );
    grid->addWidget( rLeft, 1, 0 );
    connect( rLeft, SIGNAL( clicked() ), this, SLOT( alignLeft() ) );

    rCenter = new QRadioButton( i18n( "Center" ), tab );
    grid->addWidget( rCenter, 2, 0 );
    connect( rCenter, SIGNAL( clicked() ), this, SLOT( alignCenter() ) );

    rRight = new QRadioButton( i18n( "Right" ), tab );
    grid->addWidget( rRight, 3, 0 );
    connect( rRight, SIGNAL( clicked() ), this, SLOT( alignRight() ) );

    rJustify = new QRadioButton( i18n( "Justify" ), tab );
    grid->addWidget( rJustify, 4, 0 );
    connect( rJustify, SIGNAL( clicked() ), this, SLOT( alignJustify() ) );

    clearAligns();
    rLeft->setChecked( true );

    // --------------- preview --------------------
    prev2 = new KWPagePreview2( tab );
    grid->addMultiCellWidget( prev2, 0, 5, 1, 1 );

    // --------------- main grid ------------------
    grid->setColStretch( 1, 1 );
    grid->setRowStretch( 5, 1 );
}

/*================================================================*/
void KWParagDia::setupTab3()
{
    QWidget *tab = addPage( i18n( "Borders" ) );
    QGridLayout *grid = new QGridLayout( tab, 8, 2, 15, 7 );

    lStyle = new QLabel( i18n( "Style:" ), tab );
    grid->addWidget( lStyle, 0, 0 );

    cStyle = new QComboBox( false, tab );
    cStyle->insertItem( i18n( "solid line" ) );
    cStyle->insertItem( i18n( "dash line ( ---- )" ) );
    cStyle->insertItem( i18n( "dot line ( **** )" ) );
    cStyle->insertItem( i18n( "dash dot line ( -*-* )" ) );
    cStyle->insertItem( i18n( "dash dot dot line ( -**- )" ) );
    grid->addWidget( cStyle, 1, 0 );
    connect( cStyle, SIGNAL( activated( const QString & ) ), this, SLOT( brdStyleChanged( const QString & ) ) );

    lWidth = new QLabel( i18n( "Width:" ), tab );
    grid->addWidget( lWidth, 2, 0 );

    cWidth = new QComboBox( false, tab );
    for( unsigned int i = 1; i <= 10; i++ )
        cWidth->insertItem(QString::number(i));
    grid->addWidget( cWidth, 3, 0 );
    connect( cWidth, SIGNAL( activated( const QString & ) ), this, SLOT( brdWidthChanged( const QString & ) ) );

    lColor = new QLabel( i18n( "Color:" ), tab );
    grid->addWidget( lColor, 4, 0 );

    bColor = new KColorButton( tab );
    grid->addWidget( bColor, 5, 0 );
    connect( bColor, SIGNAL( changed( const QColor& ) ), this, SLOT( brdColorChanged( const QColor& ) ) );

    QButtonGroup * bb = new QHButtonGroup( tab );
    bb->setFrameStyle(QFrame::NoFrame);
    bLeft = new QPushButton(bb);
    bLeft->setPixmap( KWBarIcon( "borderleft" ) );
    bLeft->setToggleButton( true );
    bRight = new QPushButton(bb);
    bRight->setPixmap( KWBarIcon( "borderright" ) );
    bRight->setToggleButton( true );
    bTop = new QPushButton(bb);
    bTop->setPixmap( KWBarIcon( "bordertop" ) );
    bTop->setToggleButton( true );
    bBottom = new QPushButton(bb);
    bBottom->setPixmap( KWBarIcon( "borderbottom" ) );
    bBottom->setToggleButton( true );
    grid->addWidget( bb, 6, 0 );

    connect( bLeft, SIGNAL( toggled( bool ) ), this, SLOT( brdLeftToggled( bool ) ) );
    connect( bRight, SIGNAL( toggled( bool ) ), this, SLOT( brdRightToggled( bool ) ) );
    connect( bTop, SIGNAL( toggled( bool ) ), this, SLOT( brdTopToggled( bool ) ) );
    connect( bBottom, SIGNAL( toggled( bool ) ), this, SLOT( brdBottomToggled( bool ) ) );

    QGroupBox *grp=new QGroupBox( i18n( "Preview" ), tab );
    grid->addMultiCellWidget( grp , 0, 7, 1, 1 );
    prev3 = new KWBorderPreview( grp );
    QVBoxLayout *lay1 = new QVBoxLayout( grp );
    lay1->setMargin( 15 );
    lay1->setSpacing( 1 );
    lay1->addWidget(prev3);

    connect( prev3 ,SIGNAL( choosearea(QMouseEvent * )),
           this,SLOT( slotPressEvent(QMouseEvent *)));

    grid->setRowStretch( 7, 1 );
    grid->setColStretch( 1, 1 );

    m_bAfterInitBorder=false;
}

#define OFFSETX 15
#define OFFSETY 7
#define KW_SPACE 30
void KWParagDia::slotPressEvent(QMouseEvent *_ev)
{
    QRect r = prev3->contentsRect();
    QRect rect(r.x()+OFFSETX,r.y()+OFFSETY,r.width()-OFFSETX,r.y()+OFFSETY+KW_SPACE);
    if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {
            if( (  ((int)m_topBorder.ptWidth != cWidth->currentText().toInt()) ||(m_topBorder.color != bColor->color() )
                   ||(m_topBorder.style!=Border::getStyle(cStyle->currentText()) )) && bTop->isOn() )
                {
                    m_topBorder.ptWidth = cWidth->currentText().toInt();
                    m_topBorder.color = QColor( bColor->color() );
                    m_topBorder.style=Border::getStyle(cStyle->currentText());
                    prev3->setTopBorder( m_topBorder );
                }
            else
                bTop->setOn(!bTop->isOn());
        }
    rect.setCoords(r.x()+OFFSETX,r.height()-OFFSETY-KW_SPACE,r.width()-OFFSETX,r.height()-OFFSETY);
    if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {
            if( (  ((int)m_bottomBorder.ptWidth != cWidth->currentText().toInt()) ||(m_bottomBorder.color != bColor->color() )
                   ||(m_bottomBorder.style!=Border::getStyle(cStyle->currentText()) )) && bBottom->isOn() )
                {
                    m_bottomBorder.ptWidth = cWidth->currentText().toInt();
                    m_bottomBorder.color = QColor( bColor->color() );
                    m_bottomBorder.style=Border::getStyle(cStyle->currentText());
                    prev3->setBottomBorder( m_bottomBorder );
                }
            else
                bBottom->setOn(!bBottom->isOn());
        }

    rect.setCoords(r.x()+OFFSETX,r.y()+OFFSETY,r.x()+KW_SPACE+OFFSETX,r.height()-OFFSETY);
    if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {

            if( (  ((int)m_leftBorder.ptWidth != cWidth->currentText().toInt()) ||(m_leftBorder.color != bColor->color() )
                   ||(m_leftBorder.style!=Border::getStyle(cStyle->currentText()) )) && bLeft->isOn() )
                {
                    m_leftBorder.ptWidth = cWidth->currentText().toInt();
                    m_leftBorder.color = QColor( bColor->color() );
                    m_leftBorder.style=Border::getStyle(cStyle->currentText());
                    prev3->setLeftBorder( m_leftBorder );
                }
            else
                bLeft->setOn(!bLeft->isOn());
        }
    rect.setCoords(r.width()-OFFSETX-KW_SPACE,r.y()+OFFSETY,r.width()-OFFSETX,r.height()-OFFSETY);
    if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {

            if( (  ((int)m_rightBorder.ptWidth != cWidth->currentText().toInt()) ||(m_rightBorder.color != bColor->color() )
                   ||(m_rightBorder.style!=Border::getStyle(cStyle->currentText()) )) && bRight->isOn() )
                {
                    m_rightBorder.ptWidth = cWidth->currentText().toInt();
                    m_rightBorder.color = QColor( bColor->color() );
                    m_rightBorder.style=Border::getStyle(cStyle->currentText());
                    prev3->setRightBorder( m_rightBorder );
                }
            else
                bRight->setOn(!bRight->isOn());
        }


}

#undef OFFSETX
#undef OFFSETY
#undef KW_SPACE

/*================================================================*/
void KWParagDia::setupTab4()
{
    QWidget *tab = addPage( i18n( "Bullets/Numbers" ) );
    QGridLayout *grid = new QGridLayout( tab, 4, 2, 15, 7 );

    // What type of numbering is required?
    gNumbering = new QButtonGroup( i18n("Numbering"), tab );
    QGridLayout *ngrid = new QGridLayout( gNumbering, 1, 3, 15, 7 );

    QRadioButton *tmp;
    tmp = new QRadioButton( i18n( "&None" ), gNumbering );
    ngrid->addWidget( tmp, 0, 0 );
    gNumbering->insert( tmp, Counter::NUM_NONE );

    tmp = new QRadioButton( i18n( "&List Numbering" ), gNumbering );
    ngrid->addWidget( tmp, 1, 0 );
    gNumbering->insert( tmp, Counter::NUM_LIST );

    tmp = new QRadioButton( i18n( "&Chapter Numbering" ), gNumbering );
    ngrid->addWidget( tmp, 2, 0 );
    gNumbering->insert( tmp, Counter::NUM_CHAPTER );

    connect( gNumbering, SIGNAL( clicked( int ) ), this, SLOT( numTypeChanged( int ) ) );
    grid->addWidget( gNumbering, 0, 0 );

    // How should the numbers be displayed?
    gStyle = new QButtonGroup( i18n("Style"), tab );
    QGridLayout *tgrid = new QGridLayout( gStyle, 1, 10, 15, 7 );

    tmp = new QRadioButton( i18n( "&None" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 0, 0, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_NONE );

    tmp = new QRadioButton( i18n( "&Arabic Numbers ( 1, 2, 3, 4, ... )" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 1, 1, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_NUM );

    tmp = new QRadioButton( i18n( "L&ower Alphabetical ( a, b, c, d, ... )" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 2, 2, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_ALPHAB_L );

    tmp = new QRadioButton( i18n( "U&pper Alphabetical ( A, B, C, D, ... )" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 3, 3, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_ALPHAB_U );

    tmp = new QRadioButton( i18n( "&Lower Roman Numbers ( i, ii, iii, iv, ... )" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 4, 4, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_ROM_NUM_L );

    tmp = new QRadioButton( i18n( "&Upper Roman Numbers ( I, II, III, IV, ... )" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 5, 5, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_ROM_NUM_U );

    tmp = new QRadioButton( i18n( "&Custom" ), gStyle );
    tgrid->addWidget( tmp, 6, 0 );
    gStyle->insert( tmp, Counter::STYLE_CUSTOM );
    tmp->setEnabled(false); // Not implemented

    eCustomNum = new QLineEdit( gStyle );
    eCustomNum->setEnabled( false );
    tgrid->addWidget( eCustomNum, 6, 1 );
    connect( tmp, SIGNAL( toggled(bool) ), eCustomNum, SLOT( setEnabled(bool) ));
    connect( eCustomNum, SIGNAL( textChanged(const QString&) ),
             this, SLOT( numCounterDefChanged(const QString&) ) );


    QString custcountwt(i18n("<h1>Create custom counters</h1>\n"
        "<p>You can enter a string describing your custom counter, consisting of \n"
        " the following symbols. For now, this string may not contain any whitespace \n"
        " or additional text. This will change.</p>\n"
        "<ul><li>\\arabic - arabic numbers (1, 2, 3, ...)</li><li>\\roman or \\Roman - lower or uppercase roman numbers</li>\n"
        "<li>\\alph or \\Alph - lower or uppercase latin letters</li></ul>\n"
        "<p>This will hopefully have more options in the future (like enumerated lists or greek letters).</p>" ));
    QWhatsThis::add( tmp, custcountwt );
    QWhatsThis::add( eCustomNum, custcountwt );

    tmp = new QRadioButton( i18n( "&Disc Bullet" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 7, 7, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_DISCBULLET );
    rDisc = tmp;

    tmp = new QRadioButton( i18n( "&Square Bullet" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 8, 8, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_SQUAREBULLET );
    rSquare = tmp;

    tmp = new QRadioButton( i18n( "&Circle Bullet" ), gStyle );
    tgrid->addMultiCellWidget( tmp, 9, 9, 0, 1 );
    gStyle->insert( tmp, Counter::STYLE_CIRCLEBULLET );
    rCircle = tmp;

    tmp = new QRadioButton( i18n( "Custom Bullet" ), gStyle );
    tgrid->addWidget( tmp, 10, 0 );
    gStyle->insert( tmp, Counter::STYLE_CUSTOMBULLET );
    rCustom = tmp;

    bBullets = new QPushButton( gStyle );
    tgrid->addWidget( bBullets, 10, 1 );
    connect( bBullets, SIGNAL( clicked() ), this, SLOT( numChangeBullet() ) );

    grid->addWidget( gStyle, 1, 0 );
    connect( gStyle, SIGNAL( clicked( int ) ), this, SLOT( numStyleChanged( int ) ) );

    // Miscellaneous stuff.
    gText = new QGroupBox( i18n("Other Settings"), tab );
    QGridLayout *txtgrid = new QGridLayout( gText, 2, 4, 15, 7 );

    QLabel *lcLeft = new QLabel( i18n( "Prefix Text" ), gText );
    txtgrid->addWidget( lcLeft, 0, 0 );

    QLabel *lcRight = new QLabel( i18n( "Suffix Text" ), gText );
    txtgrid->addWidget( lcRight, 0, 1 );

    ecLeft = new QLineEdit( gText );
    txtgrid->addWidget( ecLeft, 1, 0 );
    connect( ecLeft, SIGNAL( textChanged( const QString & ) ), this, SLOT( numLeftTextChanged( const QString & ) ) );

    ecRight = new QLineEdit( gText );
    txtgrid->addWidget( ecRight, 1, 1 );
    connect( ecRight, SIGNAL( textChanged( const QString & ) ), this, SLOT( numRightTextChanged( const QString & ) ) );

    lStart = new QLabel( i18n( "Start at ( 1, 2, ... ) :" ), gText );
    lStart->setAlignment( AlignRight | AlignVCenter );
    txtgrid->addWidget( lStart, 2, 0 );

    // TODO: make this a spinbox or a combo, with values depending on the type
    // of numbering.
    eStart = new QLineEdit( gText );
    txtgrid->addWidget( eStart, 2, 1 );
    connect( eStart, SIGNAL( textChanged( const QString & ) ), this, SLOT( numStartChanged( const QString & ) ) );

    QLabel *lDepth = new QLabel( i18n( "Depth:" ), gText );
    lDepth->setAlignment( AlignRight | AlignVCenter );
    txtgrid->addWidget( lDepth, 3, 0 );

    sDepth = new QSpinBox( 0, 15, 1, gText );
    txtgrid->addWidget( sDepth, 3, 1 );
    connect( sDepth, SIGNAL( valueChanged( int ) ), this, SLOT( numDepthChanged( int ) ) );
    grid->addWidget( gText, 2, 0 );

    // Add the preview.
    prev4 = new KWNumPreview( tab );
    grid->addMultiCellWidget( prev4, 0, 2, 1, 1 );
}

/*================================================================*/
void KWParagDia::setupTab5()
{
    QWidget *tab = addPage( i18n( "Tabulators" ) );
    QGridLayout *grid = new QGridLayout( tab, 4, 2, 15, 7 );

    lTab = new QLabel(  tab );
    grid->addWidget( lTab, 0, 0 );

    eTabPos = new QLineEdit( tab );

    if ( unit == U_PT )
        eTabPos->setValidator( new QIntValidator( eTabPos ) );
    else
        eTabPos->setValidator( new QDoubleValidator( eTabPos ) );
    grid->addWidget( eTabPos, 1, 0 );

    QString unitText;
    switch ( unit )
      {
      case U_MM:
	unitText=i18n("in Millimeters (mm)");
	break;
      case U_INCH:
	unitText=i18n("in Inches (inch)");
	break;
      case U_PT:
      default:
	unitText=i18n("in points ( pt )" );
      }
    lTab->setText(i18n( "Tabulator positions are given %1" ).arg(unitText));

    KButtonBox * bbTabs = new KButtonBox( tab );
    bAdd = bbTabs->addButton( i18n( "Add" ), false );
    bDel = bbTabs->addButton( i18n( "Delete" ), false );
    bModify = bbTabs->addButton( i18n( "Modify" ), false );
    grid->addWidget( bbTabs, 2, 0 );

    lTabs = new QListBox( tab );
    grid->addWidget( lTabs, 3, 0 );

    g3 = new QButtonGroup( "", tab );
    tabGrid = new QGridLayout( g3, 5, 1, 15, 7 );
    g3->setExclusive( true );

    rtLeft = new QRadioButton( i18n( "Left" ), g3 );
    rtLeft->setChecked(true);
    tabGrid->addWidget( rtLeft, 0, 0 );
    g3->insert( rtLeft );

    rtCenter = new QRadioButton( i18n( "Center" ), g3 );
    tabGrid->addWidget( rtCenter, 1, 0 );
    g3->insert( rtCenter );

    rtRight = new QRadioButton( i18n( "Right" ), g3 );
    tabGrid->addWidget( rtRight, 2, 0 );
    g3->insert( rtRight );

    rtDecimal = new QRadioButton( i18n( "Decimal" ), g3 );
    tabGrid->addWidget( rtDecimal, 3, 0 );
    g3->insert( rtDecimal );

    tabGrid->setRowStretch( 4, 1 );
    tabGrid->setColStretch( 0, 1 );
    grid->addWidget( g3, 3, 1 );
    grid->setRowStretch( 3, 1 );
    if(lTabs->count()==0)
      {
	bDel->setEnabled(false);
	bModify->setEnabled(false);
      }

    _tabList.setAutoDelete( TRUE );


    connect(bAdd,SIGNAL(clicked ()),this,SLOT(addClicked()));
    connect(bModify,SIGNAL(clicked ()),this,SLOT(modifyClicked()));
    connect(bDel,SIGNAL(clicked ()),this,SLOT(delClicked()));
    connect(lTabs,SIGNAL(doubleClicked( QListBoxItem * ) ),this,SLOT(slotDoubleClicked( QListBoxItem * ) ));
    connect(lTabs,SIGNAL(clicked( QListBoxItem * ) ),this,SLOT(slotDoubleClicked( QListBoxItem * ) ));
}


/*================================================================*/
void KWParagDia::addClicked()
{
  if(!eTabPos->text().isEmpty())
    {
      if(findExistingValue(eTabPos->text().toDouble()))
	{
	  QString tmp=i18n("There is a tabulator at this position");
	  KMessageBox::error( this, tmp);
	  eTabPos->setText("");
	  return;
	}

      m_bListTabulatorChanged=true;
      lTabs->insertItem(eTabPos->text());
      bDel->setEnabled(true);
      bModify->setEnabled(true);

      KoTabulator *tab=new KoTabulator;
      if(rtLeft->isChecked())
	tab->type=T_LEFT;
      else if(rtCenter->isChecked())
	tab->type=T_CENTER;
      else if(rtRight->isChecked())
	tab->type=T_RIGHT;
      else if(rtDecimal->isChecked())
	tab->type=T_DEC_PNT;
      else
	tab->type=T_LEFT;
      double val=eTabPos->text().toDouble();
      switch ( unit )
	{
	case U_MM:
	  tab->mmPos=val;
	  tab->inchPos=MM_TO_INCH(val);
	  tab->ptPos=MM_TO_POINT(val);
	  break;
	case U_INCH:
	  tab->mmPos=INCH_TO_MM(val);
	  tab->inchPos=val;
	  tab->ptPos= INCH_TO_POINT(val);
	  break;
	case U_PT:
	default:
	  tab->mmPos=POINT_TO_MM(val);
	  tab->inchPos=POINT_TO_INCH(val);
	  tab->ptPos=val;
	}
      _tabList.append(tab);
      eTabPos->setText("");
    }
}

bool KWParagDia::findExistingValue(double val)
{
  KoTabulator *tmp;
  for ( tmp=_tabList.first(); tmp != 0; tmp= _tabList.next() )
    {
       switch ( unit )
	 {
	 case U_MM:
	   if(tmp->mmPos==val)
	     return true;
	   break;
	 case U_INCH:
	   if(tmp->inchPos==val)
	     return true;
	   break;
	 case U_PT:
	   if(tmp->ptPos==val)
	      return true;
	   break;
	 }
    }
  return false;
}

/*================================================================*/
void KWParagDia::modifyClicked()
{
  if(!eTabPos->text().isEmpty() && lTabs->currentItem()!=-1)
    {
       _tabList.remove(lTabs->currentItem());
      lTabs->removeItem(lTabs->currentItem());
      addClicked();
      eTabPos->setText("");
      m_bListTabulatorChanged=true;
    }

}

/*================================================================*/
void KWParagDia::delClicked()
{
    if(lTabs->currentItem()!=-1)
    {
        lTabs->removeItem(lTabs->currentItem());
        m_bListTabulatorChanged=true;
        _tabList.remove(lTabs->currentItem());
        eTabPos->setText("");
        if(lTabs->count()==0)
	{
            bDel->setEnabled(false);
            bModify->setEnabled(false);

	}
        else
	{
            lTabs->setCurrentItem(0);
            setActifItem(lTabs->currentText().toDouble());
	}
    }
}

void KWParagDia::setActifItem(double value)
{
  KoTabulator *tmp;
  for ( tmp=_tabList.first(); tmp != 0; tmp= _tabList.next() )
    {
      switch ( unit )
	{
	case U_MM:
	  if(tmp->mmPos==value)
	    {
	      switch(tmp->type)
		{
		case T_LEFT:
		  rtLeft->setChecked(true);
		  break;
		case T_CENTER:
		  rtCenter->setChecked(true);
		  break;
		case  T_RIGHT:
		  rtRight->setChecked(true);
		  break;
		case T_DEC_PNT:
		  rtDecimal->setChecked(true);
		  break;
		}
	    }
	  break;
	case U_INCH:
	  if(tmp->inchPos==value)
	    {
	      switch(tmp->type)
		{
		case T_LEFT:
		  rtLeft->setChecked(true);
		  break;
		case T_CENTER:
		  rtCenter->setChecked(true);
		  break;
		case  T_RIGHT:
		  rtRight->setChecked(true);
		  break;
		case T_DEC_PNT:
		  rtDecimal->setChecked(true);
		  break;
		}
	    }

	  break;
	case U_PT:
	default:
	  if(tmp->ptPos==value)
	    {
	      switch(tmp->type)
		{
		case T_LEFT:
		  rtLeft->setChecked(true);
		  break;
		case T_CENTER:
		  rtCenter->setChecked(true);
		  break;
		case  T_RIGHT:
		  rtRight->setChecked(true);
		  break;
		case T_DEC_PNT:
		  rtDecimal->setChecked(true);
		  break;
		}
	    }
	}
    }
}

/*================================================================*/
void KWParagDia::slotDoubleClicked( QListBoxItem * )
{
  if(lTabs->currentItem()!=-1)
    {
      eTabPos->setText(lTabs->currentText());
      double value=lTabs->currentText().toDouble();
      bDel->setEnabled(true);
      bModify->setEnabled(true);
      setActifItem(value);
    }
}



/*================================================================*/
void KWParagDia::clearAligns()
{
    rLeft->setChecked( false );
    rCenter->setChecked( false );
    rRight->setChecked( false );
    rJustify->setChecked( false );
}

/*================================================================*/
void KWParagDia::updateBorders()
{
    if ( m_leftBorder.ptWidth == 0 )
        bLeft->setOn( false );
    else
        bLeft->setOn( true );

    if ( m_rightBorder.ptWidth == 0 )
        bRight->setOn( false );
    else
        bRight->setOn( true );

    if ( m_topBorder.ptWidth == 0 )
        bTop->setOn( false );
    else
        bTop->setOn( true );

    if ( m_bottomBorder.ptWidth == 0 )
        bBottom->setOn( false );
    else
        bBottom->setOn( true );
    prev3->setLeftBorder( m_leftBorder );
    prev3->setRightBorder( m_rightBorder );
    prev3->setTopBorder( m_topBorder );
    prev3->setBottomBorder( m_bottomBorder );
}

/*================================================================*/
void KWParagDia::leftChanged( const QString & _text )
{
    prev1->setLeft( _text.toDouble() );
}

/*================================================================*/
void KWParagDia::rightChanged( const QString & _text )
{
  prev1->setRight( _text.toDouble() );
}

/*================================================================*/
void KWParagDia::firstChanged( const QString & _text )
{
    prev1->setFirst( _text.toDouble() );
}

/*================================================================*/
void KWParagDia::spacingActivated( int _index )
{
    if ( _index == 4 ) {
        eSpacing->setEnabled( true );
        eSpacing->setText( "12.0" );
        eSpacing->setFocus();
    } else {
        eSpacing->setEnabled( false );
        switch ( _index ) {
        case 0: eSpacing->setText( "14.0" );
            break;
        case 1: eSpacing->setText( "28.0" );
            break;
        case 2: eSpacing->setText( "42.0" );
            break;
        case 3: eSpacing->setText( "56.0" );
            break;
        }
    }
    prev1->setSpacing( eSpacing->text().toDouble() );
}

/*================================================================*/
void KWParagDia::spacingChanged( const QString & _text )
{
  prev1->setSpacing( _text.toDouble() );
}

/*================================================================*/
void KWParagDia::beforeChanged( const QString & _text )
{
    prev1->setBefore( _text.toDouble() );
}

/*================================================================*/
void KWParagDia::afterChanged( const QString & _text )
{
    prev1->setAfter( _text.toDouble() );
}

/*================================================================*/
void KWParagDia::alignLeft()
{
    prev2->setAlign( Qt::AlignLeft );
    clearAligns();
    rLeft->setChecked( true );
}

/*================================================================*/
void KWParagDia::alignCenter()
{
    prev2->setAlign( Qt::AlignCenter );
    clearAligns();
    rCenter->setChecked( true );
}

/*================================================================*/
void KWParagDia::alignRight()
{
    prev2->setAlign( Qt::AlignRight );
    clearAligns();
    rRight->setChecked( true );
}

/*================================================================*/
void KWParagDia::alignJustify()
{
    prev2->setAlign( Qt3::AlignJustify );
    clearAligns();
    rJustify->setChecked( true );
}

/*================================================================*/
void KWParagDia::brdLeftToggled( bool _on )
{
    if ( !_on )
        m_leftBorder.ptWidth = 0;
    else {
      if(m_bAfterInitBorder)
	{
	  m_leftBorder.ptWidth = cWidth->currentText().toInt();
	  m_leftBorder.color = QColor( bColor->color() );
	  m_leftBorder.style= Border::getStyle( cStyle->currentText() );
	}
    }
    prev3->setLeftBorder( m_leftBorder );
}

/*================================================================*/
void KWParagDia::brdRightToggled( bool _on )
{
    if ( !_on )
        m_rightBorder.ptWidth = 0;
    else {
      if(m_bAfterInitBorder)
	{
	  m_rightBorder.ptWidth = cWidth->currentText().toInt();
	  m_rightBorder.color = QColor( bColor->color() );
	  m_rightBorder.style= Border::getStyle( cStyle->currentText() );
	}
    }
    prev3->setRightBorder( m_rightBorder );
}

/*================================================================*/
void KWParagDia::brdTopToggled( bool _on )
{
    if ( !_on )
        m_topBorder.ptWidth = 0;
    else {
      if(m_bAfterInitBorder)
	{

	  m_topBorder.ptWidth = cWidth->currentText().toInt();
	  m_topBorder.color = QColor( bColor->color() );
	  m_topBorder.style= Border::getStyle( cStyle->currentText() );
	}
    }
    prev3->setTopBorder( m_topBorder );
}

/*================================================================*/
void KWParagDia::brdBottomToggled( bool _on )
{
    if ( !_on )
        m_bottomBorder.ptWidth = 0;
    else {
      if(m_bAfterInitBorder)
	{
	  m_bottomBorder.ptWidth = cWidth->currentText().toInt();
	  m_bottomBorder.color = QColor( bColor->color() );
	  m_bottomBorder.style=Border::getStyle(cStyle->currentText());
	}
    }
    prev3->setBottomBorder( m_bottomBorder );
}

/*================================================================*/
void KWParagDia::brdStyleChanged( const QString & )
{
}

/*================================================================*/
void KWParagDia::brdWidthChanged( const QString & )
{
}

/*================================================================*/
void KWParagDia::brdColorChanged( const QColor & )
{
}

/*================================================================*/
void KWParagDia::numChangeBullet()
{
    gStyle->setButton( Counter::STYLE_CUSTOMBULLET );
    numStyleChanged( Counter::STYLE_CUSTOMBULLET );
    QString f = m_counter.customBulletFont();
    if ( f.isEmpty() )
        f = "symbol";
    QChar c = m_counter.customBulletCharacter();

    if ( KCharSelectDia::selectChar( f, c ) )
    {
        m_counter.setCustomBulletFont( f );
        m_counter.setCustomBulletCharacter( c );
        bBullets->setText( c );
        if ( !f.isEmpty() )
            bBullets->setFont( QFont( m_counter.customBulletFont() ) );
        prev4->setCounter( m_counter );
    }
}

/*================================================================*/
void KWParagDia::numStyleChanged( int _type )
{
    m_counter.setStyle( static_cast<Counter::Style>( _type ) );

    // Disable start at for bullet styles.
    bool hasStart = !m_counter.isBullet();
    lStart->setEnabled( hasStart );
    eStart->setEnabled( hasStart );
}

/*================================================================*/
void KWParagDia::numCounterDefChanged( const QString& _cd )
{
    m_counter.setCustom( _cd );
}

/*================================================================*/
void KWParagDia::numTypeChanged( int _ntype )
{
    m_counter.setNumbering( static_cast<Counter::Numbering>( _ntype ) );

    // Disable all options for NUM_NONE.
    gText->setEnabled( m_counter.numbering() != Counter::NUM_NONE );
    gStyle->setEnabled( m_counter.numbering() != Counter::NUM_NONE );

    // Disable bullet styles for NUM_CHAPTER.
    bool isList = m_counter.numbering() == Counter::NUM_LIST;
    rDisc->setEnabled( isList );
    rSquare->setEnabled( isList );
    rCircle->setEnabled( isList );
    rCustom->setEnabled( isList );
    bBullets->setEnabled( isList );
    if ( !isList )
    {
        // Reset style if required by the internal logic of Counter.
        gStyle->setButton( m_counter.style() );
        numStyleChanged( m_counter.style() );
    }
}

/*================================================================*/
void KWParagDia::numLeftTextChanged( const QString & _c )
{
    m_counter.setPrefix( _c );
}

/*================================================================*/
void KWParagDia::numRightTextChanged( const QString & _c )
{
    m_counter.setSuffix( _c );
}

/*================================================================*/
void KWParagDia::numStartChanged( const QString & _c )
{
    m_counter.setStartNumber( _c.toInt() ); // HACK
}

/*================================================================*/
void KWParagDia::numDepthChanged( int _val )
{
    m_counter.setDepth( _val );
}

/*================================================================*/
void KWParagDia::setCounter( Counter _counter )
{
    prev4->setCounter( _counter );
    m_counter = _counter;

    gNumbering->setButton( m_counter.numbering() );
    numTypeChanged( m_counter.numbering() );

    gStyle->setButton( m_counter.style() );
    numStyleChanged( m_counter.style() );

    eCustomNum->setText( m_counter.custom() );

    bBullets->setText( m_counter.customBulletCharacter() );
    if ( !m_counter.customBulletFont().isEmpty() )
        bBullets->setFont( QFont( m_counter.customBulletFont() ) );

    ecLeft->setText( m_counter.prefix() );
    ecRight->setText( m_counter.suffix() );

    sDepth->setValue( m_counter.depth() );
    // What we really need is a combobox filled with values depending on
    // the type of numbering - or a spinbox. (DF)
    eStart->setText( QString::number( m_counter.startNumber() ) ); // HACK
}

/*================================================================*/
void KWParagDia::setTabList( const QList<KoTabulator> *tabList )
{
    _tabList.clear();
    QListIterator<KoTabulator> it( *tabList );
    for ( it.toFirst(); it.current(); ++it ) {
        KoTabulator *t = new KoTabulator;
        t->type = it.current()->type;
        t->mmPos = it.current()->mmPos;
        t->inchPos = it.current()->inchPos;
        t->ptPos = it.current()->ptPos;
        _tabList.append( t );
        switch ( unit )
        {
            case U_MM:
                lTabs->insertItem(QString::number(t->mmPos));
                break;
            case U_INCH:
                lTabs->insertItem(QString::number(t->inchPos));
                break;
            case U_PT:
                lTabs->insertItem(QString::number(t->ptPos));
                break;
        }
    }
}

/*================================================================*/
KWUnit KWParagDia::leftIndent() const
{
    return KWUnit::createUnit( QMAX(eLeft->text().toDouble(),0), unit );
}

/*================================================================*/
KWUnit KWParagDia::rightIndent() const
{
    return KWUnit::createUnit( QMAX(eRight->text().toDouble(),0), unit );
}

/*================================================================*/
KWUnit KWParagDia::firstLineIndent() const
{
  return KWUnit::createUnit( eFirstLine->text().toDouble(), unit );
}

/*================================================================*/
KWUnit KWParagDia::spaceBeforeParag() const
{
    return KWUnit::createUnit( QMAX(eBefore->text().toDouble(),0), unit );
}

/*================================================================*/
KWUnit KWParagDia::spaceAfterParag() const
{
    return KWUnit::createUnit( QMAX(eAfter->text().toDouble(),0), unit );
}

/*================================================================*/
KWUnit KWParagDia::lineSpacing() const
{
    return KWUnit::createUnit( QMAX(eSpacing->text().toDouble(),0), unit );
}

void KWParagDia::setParagLayout( const KWParagLayout & lay )
{
    setAlign( lay.alignment );
    setFirstLineIndent( lay.margins[QStyleSheetItem::MarginFirstLine] );
    setLeftIndent( lay.margins[QStyleSheetItem::MarginLeft] );
    setRightIndent( lay.margins[QStyleSheetItem::MarginRight] );
    setSpaceBeforeParag( lay.margins[QStyleSheetItem::MarginTop] );
    setSpaceAfterParag( lay.margins[QStyleSheetItem::MarginBottom] );
    setCounter( lay.counter );
    setLineSpacing( lay.lineSpacing );
    setLeftBorder( lay.leftBorder );
    setRightBorder( lay.rightBorder );
    setTopBorder( lay.topBorder );
    setBottomBorder( lay.bottomBorder );
    setTabList( lay.tabList() );
    oldLayout=lay;
    //setTabList( lay.ParagLayout->getTabList );
    //border init it's necessary to allow left border works
    m_bAfterInitBorder=true;
}
#include "paragdia.moc"
