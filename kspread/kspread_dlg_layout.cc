/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <qprinter.h>
#include <stdio.h>
#include <stdlib.h>

#include "kspread_dlg_layout.h"
#include "kspread_undo.h"
#include "kspread_table.h"
#include "kspread_cell.h"
#include "kspread_view.h"

#include <qlabel.h>
#include <qpainter.h>
#include <kcolordlg.h>
#include <klocale.h>

KSpreadPatternSelect::KSpreadPatternSelect( QWidget *parent, const char * ) : QFrame( parent )
{
    penStyle = NoPen;
    penWidth = 1;
    penColor = black;
    selected = FALSE;
    undefined = FALSE;
}

void KSpreadPatternSelect::setPattern( const QColor &_color, int _width, PenStyle _style )
{
    penStyle = _style;
    penColor = _color;
    penWidth = _width;
    repaint();
}

void KSpreadPatternSelect::setUndefined()
{
    undefined = TRUE;
}

void KSpreadPatternSelect::paintEvent( QPaintEvent *_ev )
{
    QFrame::paintEvent( _ev );

    QPainter painter;
    QPen pen;

    if ( !undefined )
    {
	pen.setColor( penColor );
	pen.setStyle( penStyle );
	pen.setWidth( penWidth );

	painter.begin( this );
	painter.setPen( pen );
	painter.drawLine( 3, 9, width() - 6, 9 );
	painter.end();
    }
    else
    {
	painter.begin( this );
	painter.fillRect( 2, 2, width() - 4, height() - 4, BDiagPattern );
	painter.end();
    }
}

void KSpreadPatternSelect::mousePressEvent( QMouseEvent * )
{
    slotSelect();

    emit clicked( this );
}

void KSpreadPatternSelect::slotUnselect()
{
    selected = FALSE;

    setLineWidth( 1 );
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    repaint();
}

void KSpreadPatternSelect::slotSelect()
{
    selected = TRUE;

    setLineWidth( 2 );
    setFrameStyle( QFrame::Panel | QFrame::Plain );
    repaint();
}

QPixmap* CellLayoutDlg::formatOnlyNegSignedPixmap = 0L;
QPixmap* CellLayoutDlg::formatRedOnlyNegSignedPixmap = 0L;
QPixmap* CellLayoutDlg::formatRedNeverSignedPixmap = 0L;
QPixmap* CellLayoutDlg::formatAlwaysSignedPixmap = 0L;
QPixmap* CellLayoutDlg::formatRedAlwaysSignedPixmap = 0L;
QPixmap* CellLayoutDlg::undefinedPixmap = 0L;

CellLayoutDlg::CellLayoutDlg( KSpreadView *_view, KSpreadTable *_table, int _left, int _top,
			      int _right, int _bottom ) : QObject()
{
    table = _table;
    left = _left;
    top = _top;
    right = _right;
    bottom = _bottom;
    m_pView = _view;

    KSpreadCell *obj = table->cellAt( _left, _top );

    // Initialize with the upper left object
    leftBorderStyle = obj->leftBorderStyle( _left, _top );
    leftBorderWidth = obj->leftBorderWidth( _left, _top );
    leftBorderColor = obj->leftBorderColor( _left, _top );
    rightBorderStyle = obj->rightBorderStyle( _left, _top );
    rightBorderWidth = obj->rightBorderWidth( _left, _top );
    rightBorderColor = obj->rightBorderColor( _left, _top );
    topBorderStyle = obj->topBorderStyle( _left, _top );
    topBorderWidth = obj->topBorderWidth( _left, _top );
    topBorderColor = obj->topBorderColor( _left, _top );
    bottomBorderStyle = obj->bottomBorderStyle( _left, _top );
    bottomBorderWidth = obj->bottomBorderWidth( _left, _top );
    bottomBorderColor = obj->bottomBorderColor( _left, _top );
    // Just an assumption
    outlineBorderStyle = obj->topBorderStyle( _left, _top );
    outlineBorderWidth = obj->topBorderWidth( _left, _top );
    outlineBorderColor = obj->topBorderColor( _left, _top );
    prefix = obj->prefix();
    postfix = obj->postfix();
    precision = obj->precision();
    floatFormat = obj->floatFormat();
    floatColor = obj->floatColor();

    alignX = obj->align();
    alignY = obj->alignY();

    textColor = obj->textColor();
    bgColor = obj->bgColor( _left, _top );
    textFontSize = obj->textFontSize();
    textFontFamily = obj->textFontFamily();
    textFontBold = obj->textFontBold();
    textFontItalic = obj->textFontItalic();
    // Needed to initialize the font correctly ( bug in Qt )
    textFont = obj->textFont();
    eStyle = obj->style();
    actionText = obj->action();

    // We assume, that all other objects have the same values
    bLeftBorderStyle = TRUE;
    bLeftBorderColor = TRUE;
    bRightBorderStyle = TRUE;
    bRightBorderColor = TRUE;
    bTopBorderStyle = TRUE;
    bTopBorderColor = TRUE;
    bBottomBorderColor = TRUE;
    bBottomBorderStyle = TRUE;
    bOutlineBorderColor = TRUE;
    bOutlineBorderStyle = TRUE;
    bFloatFormat = TRUE;
    bFloatColor = TRUE;
    bTextColor = TRUE;
    bBgColor = TRUE;
    bTextFontFamily = TRUE;
    bTextFontSize = TRUE;
    bTextFontBold = TRUE;
    bTextFontItalic = TRUE;

    // Do the other objects have the same values ?
    for ( int x = _left; x <= _right; x++ )
	for ( int y = _top; y <= _bottom; y++ )
	{
	    KSpreadCell *obj = table->cellAt( x, y );

	    if ( leftBorderStyle != obj->leftBorderStyle( x, y ) )
		bLeftBorderStyle = FALSE;
	    if ( leftBorderWidth != obj->leftBorderWidth( x, y ) )
		bLeftBorderStyle = FALSE;
	    if ( leftBorderColor != obj->leftBorderColor( x, y ) )
		bLeftBorderColor = FALSE;
	    if ( rightBorderStyle != obj->rightBorderStyle( x, y ) )
		bRightBorderStyle = FALSE;
	    if ( rightBorderWidth != obj->rightBorderWidth( x, y ) )
		bRightBorderStyle = FALSE;
	    if ( rightBorderColor != obj->rightBorderColor( x, y ) )
		bRightBorderColor = FALSE;
	    if (  topBorderStyle != obj->topBorderStyle( x, y ) )
		bTopBorderStyle = FALSE;
	    if ( topBorderWidth != obj->topBorderWidth( x, y ) )
		bTopBorderStyle = FALSE;
	    if ( topBorderColor != obj->topBorderColor( x, y ) )
		bTopBorderColor = FALSE;
	    if ( bottomBorderStyle != obj->bottomBorderStyle( x, y ) )
		bBottomBorderStyle = FALSE;
	    if ( bottomBorderWidth != obj->bottomBorderWidth( x, y ) )
		bBottomBorderStyle = FALSE;
	    if ( bottomBorderColor != obj->bottomBorderColor( x, y ) )
		bBottomBorderColor = FALSE;
	    if ( prefix != obj->prefix() )
		prefix = QString::null;
	    if ( postfix != obj->postfix() )
		postfix = QString::null;
	    if ( precision != obj->precision() )
		precision = -2;
	    if ( floatFormat != obj->floatFormat() )
		bFloatFormat = FALSE;
	    if ( floatColor != obj->floatColor() )
		bFloatColor = FALSE;
	    if ( textColor != obj->textColor() )
		bTextColor = FALSE;
	    if ( strcmp( textFontFamily.data(), obj->textFontFamily() ) != 0 )
		bTextFontFamily = FALSE;
	    if ( textFontSize != obj->textFontSize() )
		bTextFontSize = FALSE;
	    if ( textFontBold != obj->textFontBold() )
		bTextFontBold = FALSE;
	    if ( textFontItalic != obj->textFontItalic() )
		bTextFontItalic = FALSE;
	    if ( bgColor != obj->bgColor( x, y ) )
		bBgColor = FALSE;
	    if ( eStyle != obj->style() )
		eStyle = KSpreadCell::ST_Undef;
	}

    // Look for the Outline
    for ( int x = _left; x <= _right; x++ )
    {
	KSpreadCell *obj = table->cellAt( x, _top );

	if ( outlineBorderStyle != obj->topBorderStyle( x, _top ) )
	    bOutlineBorderStyle = FALSE;
	if ( outlineBorderWidth != obj->topBorderWidth( x, _top ) )
	    bOutlineBorderStyle = FALSE;
	if ( outlineBorderColor != obj->topBorderColor( x, _top ) )
	    bOutlineBorderColor = FALSE;

	obj = table->cellAt( x, _bottom );

	if ( outlineBorderStyle != obj->bottomBorderStyle( x, _bottom ) )
	    bOutlineBorderStyle = FALSE;
	if ( outlineBorderWidth != obj->bottomBorderWidth( x, _bottom ) )
	    bOutlineBorderStyle = FALSE;
	if ( outlineBorderColor != obj->bottomBorderColor( x, _bottom ) )
	    bOutlineBorderColor = FALSE;
    }

    for ( int y = _top; y <= _bottom; y++ )
    {
	KSpreadCell *obj = table->cellAt( _left, y );

	if ( outlineBorderStyle != obj->leftBorderStyle( _left, y ) )
	    bOutlineBorderStyle = FALSE;
	if ( outlineBorderWidth != obj->leftBorderWidth( _left, y ) )
	    bOutlineBorderStyle = FALSE;
	if ( outlineBorderColor != obj->leftBorderColor( _left, y ) )
	    bOutlineBorderColor = FALSE;

	obj = table->cellAt( _right, y );

	if ( outlineBorderStyle != obj->rightBorderStyle( _left, y ) )
	    bOutlineBorderStyle = FALSE;
	if ( outlineBorderWidth != obj->rightBorderWidth( _left, y ) )
	    bOutlineBorderStyle = FALSE;
	if ( outlineBorderColor != obj->rightBorderColor( _left, y ) )
	    bOutlineBorderColor = FALSE;
    }

    init();
}

void CellLayoutDlg::init()
{
    // Did we initialize the bitmaps ?
    if ( formatOnlyNegSignedPixmap == 0L )
    {
	formatOnlyNegSignedPixmap = paintFormatPixmap( "123.456", black, "-123.456", black );
	formatRedOnlyNegSignedPixmap = paintFormatPixmap( "123.456", black, "-123.456", red );
	formatRedNeverSignedPixmap = paintFormatPixmap( "123.456", black, "123.456", black );
	formatAlwaysSignedPixmap = paintFormatPixmap( "+123.456", black, "-123.456", black );
	formatRedAlwaysSignedPixmap = paintFormatPixmap( "+123.456", black, "-123.456", red );

	// Make the undefined pixmap
        undefinedPixmap = new QPixmap( 100, 12 );
        QPainter painter;
        painter.begin( undefinedPixmap );
	painter.setBackgroundColor( white );
	painter.setPen( black );
        painter.fillRect( 0, 0, 100, 12, BDiagPattern );
        painter.end();
    }

    tab = new QTabDialog( 0L, 0L, TRUE );
    tab->setGeometry( tab->x(), tab->y(), 420, 400 );

    borderPage = new CellLayoutPageBorder( tab, this );
    tab->addTab( borderPage, i18n("Border") );

    floatPage = new CellLayoutPageFloat( tab, this );
    tab->addTab( floatPage, i18n("Number Format") );

    miscPage = new CellLayoutPageMisc( tab, this );
    tab->addTab( miscPage, i18n("Misc") );

    fontPage = new CellLayoutPageFont( tab, this );
    tab->addTab( fontPage, i18n("Font") );

    positionPage = new CellLayoutPagePosition( tab, this);
    tab->addTab( positionPage, i18n("Position") );

    // tab->setApplyButton();
    tab->setCancelButton();

    connect( tab, SIGNAL( applyButtonPressed() ), this, SLOT( slotApply() ) );
    // connect( tab, SIGNAL(cancelButtonPressed()), SLOT(setup()) );

    tab->show();
}

QPixmap* CellLayoutDlg::paintFormatPixmap( const char *_string1, const QColor & _color1,
					     const char *_string2, const QColor & _color2 )
{
    QPixmap *pixmap = new QPixmap( 150, 14 );

    QPainter painter;
    painter.begin( pixmap );
    painter.fillRect( 0, 0, 150, 14, white );
    painter.setPen( _color1 );
    painter.drawText( 2, 11, _string1 );
    painter.setPen( _color2 );
    painter.drawText( 75, 11, _string2 );
    painter.end();

    return pixmap;
}

int CellLayoutDlg::exec()
{
    return ( tab->exec() );
}

void CellLayoutDlg::slotApply()
{
    // Prepare the undo buffer
    KSpreadUndoCellLayout *undo;
    if ( !table->doc()->undoBuffer()->isLocked() )
    {
	QRect rect;
	// Since the right/bottom border is stored in objects right + 1 ( or: bottom + 1 )
	// So we have to save these layouts, too
	rect.setCoords( left, top, right + 1, bottom + 1 );
	undo = new KSpreadUndoCellLayout( table->doc(), table, rect );
	table->doc()->undoBuffer()->appendUndo( undo );
    }

    for ( int x = left; x <= right; x++ )
	for ( int y = top; y <= bottom; y++ )
	{
	    KSpreadCell *obj = table->nonDefaultCell( x, y );
	    floatPage->apply( obj );
	    borderPage->apply( obj );
	    miscPage->apply( obj );
	    fontPage->apply( obj );
            positionPage->apply( obj );
	}

    // Outline
    if ( left != right && top != bottom )
	borderPage->applyOutline( left, top, right, bottom );

    // m_pView->drawVisibleCells();
    QRect r;
    r.setCoords( left, top, right, bottom );
    m_pView->slotUpdateView( table, r );
}


CellLayoutPageFloat::CellLayoutPageFloat( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget ( parent )
{
    dlg = _dlg;

    postfix = new QLineEdit( this, "LineEdit_1" );
    precision = new QLineEdit ( this, "LineEdit_2" );
    prefix = new QLineEdit( this, "LineEdit_3" );
    // format = new QListBox( this, "ListBox_1" );
    format = new QComboBox( this, "ListBox_1" );

    QLabel* tmpQLabel;
    tmpQLabel = new QLabel( this, "Label_1" );
    tmpQLabel->setGeometry( 10, 30, 40, 30 );
    tmpQLabel->setText( i18n("Prefix") );

    postfix->raise();
    postfix->setGeometry( 70, 70, 100, 30 );
    if ( dlg->postfix.isNull() )
	postfix->setText( "########" );
    else
	postfix->setText( dlg->postfix.data() );

    tmpQLabel = new QLabel( this, "Label_2" );
    tmpQLabel->setGeometry( 10, 70, 50, 30 );
    tmpQLabel->setText( i18n("Postfix") );

    precision->raise();
    precision->setGeometry( 70, 110, 100, 30 );
    char buffer[ 100 ];
    if ( dlg->precision == -1 )
    {
      precision->setText( i18n("variable") );
    }
    else if ( dlg->precision != -2 )
    {
	sprintf( buffer, "%i", dlg->precision );
	precision->setText( buffer );
    }
    else
	precision->setText( "########" );

    tmpQLabel = new QLabel( this, "Label_3" );
    tmpQLabel->setGeometry( 10, 110, 60, 30 );
    tmpQLabel->setText( i18n("Precision") );

    prefix->raise();
    prefix->setGeometry( 70, 30, 100, 30 );
    if ( dlg->prefix.isNull() )
	prefix->setText( "########" );
    else
	prefix->setText( dlg->prefix.data() );

    format->raise();
    format->setGeometry( 190, 30, 170, 30 );

    format->insertItem( *CellLayoutDlg::formatOnlyNegSignedPixmap, 0 );
    format->insertItem( *CellLayoutDlg::formatRedOnlyNegSignedPixmap, 1 );
    format->insertItem( *CellLayoutDlg::formatRedNeverSignedPixmap, 2 );
    format->insertItem( *CellLayoutDlg::formatAlwaysSignedPixmap, 3 );
    format->insertItem( *CellLayoutDlg::formatRedAlwaysSignedPixmap, 4 );
    format->insertItem( *CellLayoutDlg::undefinedPixmap, 5 );

    tmpQLabel = new QLabel( this, "Label_4" );
    tmpQLabel->setGeometry( 190, 0, 100, 30 );
    tmpQLabel->setText( i18n("Format") );

    if ( !dlg->bFloatFormat || !dlg->bFloatColor )
	format->setCurrentItem( 5 );
    else if ( dlg->floatFormat == KSpreadCell::OnlyNegSigned && dlg->floatColor == KSpreadCell::AllBlack )
	format->setCurrentItem( 0 );
    else if ( dlg->floatFormat == KSpreadCell::OnlyNegSigned && dlg->floatColor == KSpreadCell::NegRed )
	format->setCurrentItem( 1 );
    else if ( dlg->floatFormat == KSpreadCell::AlwaysUnsigned && dlg->floatColor == KSpreadCell::NegRed )
	format->setCurrentItem( 2 );
    else if ( dlg->floatFormat == KSpreadCell::AlwaysSigned && dlg->floatColor == KSpreadCell::AllBlack )
	format->setCurrentItem( 3 );
    else if ( dlg->floatFormat == KSpreadCell::AlwaysSigned && dlg->floatColor == KSpreadCell::NegRed )
	format->setCurrentItem( 4 );

    this->resize( 400, 400 );
}

void CellLayoutPageFloat::apply( KSpreadCell *_obj )
{
    if ( strcmp( postfix->text(), dlg->postfix.data() ) != 0 )
	if ( strcmp( postfix->text(), "########" ) != 0 )
	    _obj->setPostfix( postfix->text() );
    if ( strcmp( prefix->text(), dlg->prefix.data() ) != 0 )
	if ( strcmp( prefix->text(), "########" ) != 0 )
	    _obj->setPrefix( prefix->text() );
    if ( precision->text() && precision->text()[0] != '#' )
    {
      int prec = -1;
      if ( precision->text()[0] >= '0' && precision->text()[0] <= '9' )
	prec = atoi( precision->text() );
      if ( dlg->precision != prec )
	_obj->setPrecision( prec );
    }

    switch( format->currentItem() )
    {
    case 0:
	_obj->setFloatFormat( KSpreadCell::OnlyNegSigned );
	_obj->setFloatColor( KSpreadCell::AllBlack );
	break;
    case 1:
	_obj->setFloatFormat( KSpreadCell::OnlyNegSigned );
	_obj->setFloatColor( KSpreadCell::NegRed );
	break;
    case 2:
	_obj->setFloatFormat( KSpreadCell::AlwaysUnsigned );
	_obj->setFloatColor( KSpreadCell::NegRed );
	break;
    case 3:
	_obj->setFloatFormat( KSpreadCell::AlwaysSigned );
	_obj->setFloatColor( KSpreadCell::AllBlack );
	break;
    case 4:
	_obj->setFloatFormat( KSpreadCell::AlwaysSigned );
	_obj->setFloatColor( KSpreadCell::NegRed );
	break;
    }
}

CellLayoutPageBorder::CellLayoutPageBorder( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget( parent )
{
    selectedPattern = 0L;
    selectedBorder = 0L;

    dlg = _dlg;
    
    QGroupBox* tmpQGroupBox;
    tmpQGroupBox = new QGroupBox( this, "GroupBox_2" );
    tmpQGroupBox->setGeometry( 135, 10, 140, 175 );
    tmpQGroupBox->setFrameStyle( 49 );
    tmpQGroupBox->setTitle( i18n("Pattern") );
    tmpQGroupBox->setAlignment( 1 );

    tmpQGroupBox = new QGroupBox( this, "GroupBox_1" );
    tmpQGroupBox->setGeometry( 10, 10, 120, 175 );
    tmpQGroupBox->setFrameStyle( 49 );
    tmpQGroupBox->setTitle( i18n("Border") );
    tmpQGroupBox->setAlignment( 1 );

    QLabel* tmpQLabel;
    tmpQLabel = new QLabel( this, "Label_1" );
    tmpQLabel->setGeometry( 20, 25, 50, 30 );
    tmpQLabel->setText( i18n("Outline") );

    tmpQLabel = new QLabel( this, "Label_2" );
    tmpQLabel->setGeometry( 20, 55, 40, 30 );
    tmpQLabel->setText( i18n("Left") );

    tmpQLabel = new QLabel( this, "Label_3" );
    tmpQLabel->setGeometry( 20, 85, 40, 30 );
    tmpQLabel->setText( i18n("Right") );

    tmpQLabel = new QLabel( this, "Label_4" );
    tmpQLabel->setGeometry( 20, 115, 30, 30 );
    tmpQLabel->setText( i18n("Top") );

    tmpQLabel = new QLabel( this, "Label_5" );
    tmpQLabel->setGeometry( 20, 145, 40, 30 );
    tmpQLabel->setText( i18n("Bottom") );

    outline = new KSpreadPatternSelect( this, "Frame_3" );
    outline->setGeometry( 70, 30, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	outline->setPalette( palette );
    }
    outline->setFrameStyle( 50 );

    left = new KSpreadPatternSelect( this, "Frame_4" );
    left->setGeometry( 70, 60, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	left->setPalette( palette );
    }
    left->setFrameStyle( 50 );

    right = new KSpreadPatternSelect( this, "Frame_5" );
    right->setGeometry( 70, 90, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	right->setPalette( palette );
    }
    right->setFrameStyle( 50 );

    top = new KSpreadPatternSelect( this, "Frame_6" );
    top->setGeometry( 70, 120, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	top->setPalette( palette );
    }
    top->setFrameStyle( 50 );

    bottom = new KSpreadPatternSelect( this, "Frame_7" );
    bottom->setGeometry( 70, 150, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	bottom->setPalette( palette );
    }
    bottom->setFrameStyle( 50 );

    pattern1 = new KSpreadPatternSelect( this, "Frame_8" );
    pattern1->setGeometry( 145, 30, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	pattern1->setPalette( palette );
    }
    pattern1->setFrameStyle( 50 );

    pattern2 = new KSpreadPatternSelect( this, "Frame_9" );
    pattern2->setGeometry( 145, 60, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	pattern2->setPalette( palette );
    }
    pattern2->setFrameStyle( 50 );

    pattern3 = new KSpreadPatternSelect( this, "Frame_10" );
    pattern3->setGeometry( 145, 90, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	pattern3->setPalette( palette );
    }
    pattern3->setFrameStyle( 50 );

    pattern4 = new KSpreadPatternSelect( this, "Frame_11" );
    pattern4->setGeometry( 145, 120, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	pattern4->setPalette( palette );
    }
    pattern4->setFrameStyle( 50 );

    pattern5 = new KSpreadPatternSelect( this, "Frame_12" );
    pattern5->setGeometry( 210, 30, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	pattern5->setPalette( palette );
    }
    pattern5->setFrameStyle( 50 );

    pattern6 = new KSpreadPatternSelect( this, "Frame_13" );
    pattern6->setGeometry( 210, 60, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	pattern6->setPalette( palette );
    }
    pattern6->setFrameStyle( 50 );

    pattern7 = new KSpreadPatternSelect( this, "Frame_14" );
    pattern7->setGeometry( 210, 90, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	pattern7->setPalette( palette );
    }
    pattern7->setFrameStyle( 50 );

    pattern8 = new KSpreadPatternSelect( this, "Frame_15" );
    pattern8->setGeometry( 210, 120, 50, 20 );
    {
	QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	pattern8->setPalette( palette );
    }
    pattern8->setFrameStyle( 50 );

    color = new QPushButton( this, "PushButton_1" );
    color->setGeometry( 180, 150, 80, 25 );
    color->setText( "" );

    tmpQLabel = new QLabel( this, "Label_6" );
    tmpQLabel->setGeometry( 145, 150, 35, 30 );
    tmpQLabel->setText( i18n("Color") );

    // Set the color
    if ( dlg->bLeftBorderColor && dlg->bLeftBorderStyle )
	left->setPattern( dlg->leftBorderColor, dlg->leftBorderWidth, dlg->leftBorderStyle );
    else
	left->setUndefined();

    if ( dlg->bRightBorderColor && dlg->bRightBorderStyle )
	right->setPattern( dlg->rightBorderColor, dlg->rightBorderWidth, dlg->rightBorderStyle );
    else
	right->setUndefined();

    if ( dlg->bTopBorderColor && dlg->bTopBorderStyle )
	top->setPattern( dlg->topBorderColor, dlg->topBorderWidth, dlg->topBorderStyle );
    else
	top->setUndefined();

    if ( dlg->bBottomBorderColor && dlg->bBottomBorderStyle )
	bottom->setPattern( dlg->bottomBorderColor, dlg->bottomBorderWidth, dlg->bottomBorderStyle );
    else
	bottom->setUndefined();

    if ( dlg->bOutlineBorderColor && dlg->bOutlineBorderStyle )
	outline->setPattern( dlg->outlineBorderColor, dlg->outlineBorderWidth, dlg->outlineBorderStyle );
    else
	outline->setUndefined();

    pattern1->setPattern( black, 1, DotLine );
    pattern2->setPattern( black, 1, DashLine );
    pattern3->setPattern( black, 1, SolidLine );
    pattern4->setPattern( black, 2, SolidLine );
    pattern5->setPattern( black, 3, SolidLine );
    pattern6->setPattern( black, 4, SolidLine );
    pattern7->setPattern( black, 5, SolidLine );
    pattern8->setPattern( black, 1, NoPen );

    slotSetColorButton( black );

    connect( color, SIGNAL( clicked() ), this, SLOT( slotColorButton() ) );

    connect( left, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect1( KSpreadPatternSelect* ) ) );
    connect( right, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect1( KSpreadPatternSelect* ) ) );
    connect( top, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect1( KSpreadPatternSelect* ) ) );
    connect( bottom, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect1( KSpreadPatternSelect* ) ) );
    connect( outline, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect1( KSpreadPatternSelect* ) ) );
    connect( pattern1, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
    connect( pattern2, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
    connect( pattern3, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
    connect( pattern4, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
    connect( pattern5, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
    connect( pattern6, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
    connect( pattern7, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
    connect( pattern8, SIGNAL( clicked( KSpreadPatternSelect* ) ),
	     this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );

    this->resize( 400, 400 );
}

void CellLayoutPageBorder::slotColorButton()
{
    KColorDialog d( this, "color", TRUE );
    d.setColor( currentColor );
    if ( d.exec() )
    {
	slotSetColorButton( d.color() );
    }
}

void CellLayoutPageBorder::slotSetColorButton( const QColor &_color )
{
    currentColor = _color;

    pattern1->setColor( currentColor );
    pattern2->setColor( currentColor );
    pattern3->setColor( currentColor );
    pattern4->setColor( currentColor );
    pattern5->setColor( currentColor );
    pattern6->setColor( currentColor );
    pattern7->setColor( currentColor );
    pattern8->setColor( currentColor );

    if ( selectedBorder )
	selectedBorder->setColor( currentColor );

    QColorGroup normal( ( QColor( QRgb(0) ) ), _color, QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
    QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
    QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
    QPalette palette( normal, disabled, active );
    color->setPalette( palette );
}

void CellLayoutPageBorder::slotUnselect1( KSpreadPatternSelect *_p )
{
    selectedBorder = _p;

    if ( left != _p )
	left->slotUnselect();
    if ( right != _p )
	right->slotUnselect();
    if ( top != _p )
	top->slotUnselect();
    if ( bottom != _p )
	bottom->slotUnselect();
    if ( outline != _p )
	outline->slotUnselect();

    if ( selectedPattern == 0L )
    {
	if ( !_p->isDefined() )
	    return;
	
	if ( _p->getPenStyle() == DotLine )
	{
	    selectedPattern = pattern1;
	    pattern1->slotSelect();
	}
	else if ( _p->getPenStyle() == DashLine )
	{
	    selectedPattern = pattern2;
	    pattern2->slotSelect();
	}
	else if ( _p->getPenStyle() == NoPen )
	{
	    selectedPattern = pattern8;
	    pattern8->slotSelect();
	}
	else if ( _p->getPenWidth() == 1 )
	{
	    selectedPattern = pattern3;
	    pattern3->slotSelect();
	}
	else if ( _p->getPenWidth() == 2 )
	{
	    selectedPattern = pattern4;
	    pattern4->slotSelect();
	}
	else if ( _p->getPenWidth() == 3 )
	{
	    selectedPattern = pattern5;
	    pattern5->slotSelect();
	}
	else if ( _p->getPenWidth() == 4 )
	{
	    selectedPattern = pattern6;
	    pattern6->slotSelect();
	}
	else if ( _p->getPenWidth() == 5 )
	{
	    selectedPattern = pattern7;
	    pattern7->slotSelect();
	}

	slotSetColorButton( _p->getColor() );
	
	return;
    }

    _p->setColor( currentColor );
    _p->setPattern( selectedPattern->getColor(), selectedPattern->getPenWidth(),
		    selectedPattern->getPenStyle() );
    _p->setDefined();
    _p->repaint();
}

void CellLayoutPageBorder::slotUnselect2( KSpreadPatternSelect *_p )
{
    selectedPattern = _p;

    if ( pattern1 != _p )
	pattern1->slotUnselect();
    if ( pattern2 != _p )
	pattern2->slotUnselect();
    if ( pattern3 != _p )
	pattern3->slotUnselect();
    if ( pattern4 != _p )
	pattern4->slotUnselect();
    if ( pattern5 != _p )
	pattern5->slotUnselect();
    if ( pattern6 != _p )
	pattern6->slotUnselect();
    if ( pattern7 != _p )
	pattern7->slotUnselect();
    if ( pattern8 != _p )
	pattern8->slotUnselect();

    if ( selectedBorder )
    {
	selectedBorder->setPattern( currentColor, _p->getPenWidth(), _p->getPenStyle() );
	selectedBorder->setDefined();
    }
}

void CellLayoutPageBorder::apply( KSpreadCell *_obj )
{
    if ( left->isDefined() )
    {
	_obj->setLeftBorderColor( left->getColor() );
	_obj->setLeftBorderStyle( left->getPenStyle() );
	_obj->setLeftBorderWidth( left->getPenWidth() );
    }

    if ( right->isDefined() )
    {
	_obj->setRightBorderColor( right->getColor() );
	_obj->setRightBorderStyle( right->getPenStyle() );
	_obj->setRightBorderWidth( right->getPenWidth() );
    }

    if ( top->isDefined() )
    {
	_obj->setTopBorderColor( top->getColor() );
	_obj->setTopBorderStyle( top->getPenStyle() );
	_obj->setTopBorderWidth( top->getPenWidth() );
    }

    if ( bottom->isDefined() )
    {
	_obj->setBottomBorderColor( bottom->getColor() );
	_obj->setBottomBorderStyle( bottom->getPenStyle() );
	_obj->setBottomBorderWidth( bottom->getPenWidth() );
    }
}

void CellLayoutPageBorder::applyOutline( int _left, int _top, int _right, int _bottom )
{
    if ( !outline->isDefined() )
	return;

    for ( int x = _left; x <= _right; x++ )
    {
	KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x, _top );

	obj->setTopBorderColor( outline->getColor() );
	obj->setTopBorderStyle( outline->getPenStyle() );
	obj->setTopBorderWidth( outline->getPenWidth() );

	obj = dlg->getTable()->nonDefaultCell( x, _bottom );

	obj->setBottomBorderColor( outline->getColor() );
	obj->setBottomBorderStyle( outline->getPenStyle() );
	obj->setBottomBorderWidth( outline->getPenWidth() );
    }

    for ( int y = _top; y <= _bottom; y++ )
    {
        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( _left, y );

	obj->setLeftBorderColor( outline->getColor() );
	obj->setLeftBorderStyle( outline->getPenStyle() );
	obj->setLeftBorderWidth( outline->getPenWidth() );

	obj = dlg->getTable()->nonDefaultCell( _right, y );

	obj->setRightBorderColor( outline->getColor() );
	obj->setRightBorderStyle( outline->getPenStyle() );
	obj->setRightBorderWidth( outline->getPenWidth() );
    }
}

CellLayoutPageMisc::CellLayoutPageMisc( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget( parent )
{
    dlg = _dlg;

    bTextColorUndefined = !dlg->bTextColor;
    bBgColorUndefined = !dlg->bBgColor;

    QLabel *tmpQLabel;

    tmpQLabel = new QLabel( this, "Label_1" );
    tmpQLabel->setGeometry( 20, 20, 100, 30 );
    tmpQLabel->setText( i18n("Text Color") );

    tmpQLabel = new QLabel( this, "Label_2" );
    tmpQLabel->setGeometry( 140, 20, 120, 30 );
    tmpQLabel->setText( i18n("Background Color") );

    textColorButton = new QPushButton( this, "ComboBox_1" );
    textColorButton->setGeometry( 20, 50, 100, 30 );

    connect( textColorButton, SIGNAL( clicked() ),
	     this, SLOT( slotTextColor() ) );

    bgColorButton = new QPushButton( this, "ComboBox_3" );
    bgColorButton->setGeometry( 140, 50, 100, 30 );

    connect( bgColorButton, SIGNAL( clicked() ),
	     this, SLOT( slotBackgroundColor() ) );

    tmpQLabel = new QLabel( this, "Label_3" );
    tmpQLabel->setGeometry( 20, 100, 120, 30 );
    tmpQLabel->setText( i18n("Functionality") );

    styleButton = new QComboBox( this, "ComboBox_2" );
    styleButton->setGeometry( 20, 130, 100, 30 );
    idStyleNormal = 0; styleButton->insertItem( i18n("Normal"), 0 );
    idStyleButton = 1; styleButton->insertItem( i18n("Button"), 1 );
    idStyleSelect = 2; styleButton->insertItem( i18n("Select"), 2 );
    if ( dlg->eStyle == KSpreadCell::ST_Undef )
    {
      idStyleUndef = 3; styleButton->insertItem( i18n("######"), 3 );
    }
    else
      idStyleUndef = -1;
    connect( styleButton, SIGNAL( activated( int ) ), this, SLOT( slotStyle( int ) ) );

    tmpQLabel = new QLabel( this, "Label_3" );
    tmpQLabel->setGeometry( 20, 180, 120, 30 );
    tmpQLabel->setText( i18n("Action") );

    actionText = new QLineEdit( this );
    actionText->setGeometry( 20, 210, 200, 30 );
    if ( dlg->isSingleCell() )
    {
      if ( !dlg->actionText.isEmpty() )
	actionText->setText( dlg->actionText );
      if ( dlg->eStyle == KSpreadCell::ST_Normal || dlg->eStyle == KSpreadCell::ST_Undef )
	actionText->setEnabled( false );
    }
    else
      actionText->setEnabled( false );

    if ( dlg->eStyle == KSpreadCell::ST_Normal )
      styleButton->setCurrentItem( idStyleNormal );
    else if ( dlg->eStyle == KSpreadCell::ST_Button )
      styleButton->setCurrentItem( idStyleButton );
    else if ( dlg->eStyle == KSpreadCell::ST_Select )
      styleButton->setCurrentItem( idStyleSelect );
    else if ( dlg->eStyle == KSpreadCell::ST_Undef )
      styleButton->setCurrentItem( idStyleUndef );

    if ( dlg->bTextColor )
    {
	textColor = dlg->textColor;
	setColor( textColorButton, dlg->textColor );
    }
    else
    {
	textColor = black;
	setColor( textColorButton, black );
    }

    if ( dlg->bBgColor )
    {
	bgColor = dlg->bgColor;
	setColor( bgColorButton, bgColor );
    }
    else
    {
	bgColor = white;
	setColor( bgColorButton, white );
    }

    this->resize( 400, 400 );
}

void CellLayoutPageMisc::apply( KSpreadCell *_obj )
{
    if ( !bTextColorUndefined )
	_obj->setTextColor( textColor );
    if ( !bBgColorUndefined )
	_obj->setBgColor( bgColor );
    if ( styleButton->currentItem() == idStyleNormal )
      _obj->setStyle( KSpreadCell::ST_Normal );
    else if ( styleButton->currentItem() == idStyleButton )
      _obj->setStyle( KSpreadCell::ST_Button );
    else if ( styleButton->currentItem() == idStyleSelect )
      _obj->setStyle( KSpreadCell::ST_Select );
    if ( actionText->isEnabled() )
      _obj->setAction( actionText->text() );
}

void CellLayoutPageMisc::slotStyle( int _i )
{
  if ( dlg->isSingleCell() && _i != idStyleNormal && _i != idStyleUndef )
    actionText->setEnabled( true );
  else
    actionText->setEnabled( false );
}

void CellLayoutPageMisc::slotTextColor()
{
    bTextColorUndefined = FALSE;

    KColorDialog d( this, "color", TRUE );
    d.setColor( textColor );
    if ( d.exec() )
    {
	textColor = d.color();
	setColor( textColorButton, d.color() );
    }
}

void CellLayoutPageMisc::slotBackgroundColor()
{
    bBgColorUndefined = FALSE;

    KColorDialog d( this, "color", TRUE );
    d.setColor( bgColor );
    if ( d.exec() )
    {
	bgColor = d.color();
	setColor( bgColorButton, d.color() );
    }
}

void CellLayoutPageMisc::setColor( QPushButton *_button, const QColor &_color )
{
    QColorGroup normal( ( QColor( QRgb(0) ) ), _color, QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
    QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
    QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
    QPalette palette( normal, disabled, active );
    _button->setPalette( palette );
}

#define YOFFSET  5
#define XOFFSET  5
#define LABLE_LENGTH  40
#define LABLE_HEIGHT 20
#define SIZE_X 400
#define SIZE_Y 280
#define FONTLABLE_LENGTH 60
#define COMBO_BOX_HEIGHT 28
#define COMBO_ADJUST 3
#define OKBUTTONY 260
#define BUTTONHEIGHT 25

CellLayoutPageFont::CellLayoutPageFont( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget ( parent )
{
  dlg = _dlg;

  box1 = new QGroupBox(this, "Box1");
  box1->setGeometry(XOFFSET,YOFFSET,SIZE_X -  XOFFSET
		   ,130);
  box1->setTitle(i18n("Requested Font"));

  box1 = new QGroupBox(this, "Box2");
  box1->setGeometry(XOFFSET,140,SIZE_X -  XOFFSET
		   ,110);
  box1->setTitle(i18n("Actual Font"));


  family_label = new QLabel(this,"family");
  family_label->setText(i18n("Family:"));
  family_label->setGeometry(3*XOFFSET,8*YOFFSET,LABLE_LENGTH,LABLE_HEIGHT);

  actual_family_label = new QLabel(this,"afamily");
  actual_family_label->setText(i18n("Family:"));
  actual_family_label->setGeometry(3*XOFFSET,160,40,LABLE_HEIGHT);

  actual_family_label_data = new QLabel(this,"afamilyd");
  actual_family_label_data->setGeometry(3*XOFFSET +50 ,160,110,LABLE_HEIGHT);

  size_label = new QLabel(this,"size");
  size_label->setText(i18n("Size:"));
  size_label->setGeometry(6*XOFFSET + LABLE_LENGTH + 12*XOFFSET +2* FONTLABLE_LENGTH,
			  8*YOFFSET,LABLE_LENGTH,LABLE_HEIGHT);

  actual_size_label = new QLabel(this,"asize");
  actual_size_label->setText(i18n("Size:"));
  actual_size_label->setGeometry(3*XOFFSET,160 +LABLE_HEIGHT ,
				 LABLE_LENGTH,LABLE_HEIGHT);

  actual_size_label_data = new QLabel(this,"asized");
  actual_size_label_data->setGeometry(3*XOFFSET +50 ,160 + LABLE_HEIGHT
				      ,110,LABLE_HEIGHT);

  weight_label = new QLabel(this,"weight");
  weight_label->setText(i18n("Weight:"));
  weight_label->setGeometry(3*XOFFSET,15*YOFFSET + LABLE_HEIGHT
			  ,LABLE_LENGTH,LABLE_HEIGHT);

  actual_weight_label = new QLabel(this,"aweight");
  actual_weight_label->setText(i18n("Weight:"));
  actual_weight_label->setGeometry(3*XOFFSET,160 + 2*LABLE_HEIGHT ,
				 LABLE_LENGTH,LABLE_HEIGHT);

  actual_weight_label_data = new QLabel(this,"aweightd");
  actual_weight_label_data->setGeometry(3*XOFFSET +50 ,160 + 2*LABLE_HEIGHT
				      ,110,LABLE_HEIGHT);

  style_label = new QLabel(this,"style");
  style_label->setText(i18n("Style:"));
  style_label->setGeometry(6*XOFFSET + LABLE_LENGTH + 12*XOFFSET +
			   2*FONTLABLE_LENGTH,
			   15*YOFFSET + LABLE_HEIGHT
			 ,LABLE_LENGTH,
			   LABLE_HEIGHT);

  actual_style_label = new QLabel(this,"astyle");
  actual_style_label->setText(i18n("Style:"));
  actual_style_label->setGeometry(3*XOFFSET,160 + 3*LABLE_HEIGHT ,
				 LABLE_LENGTH,LABLE_HEIGHT);

  actual_style_label_data = new QLabel(this,"astyled");
  actual_style_label_data->setGeometry(3*XOFFSET +50 ,160 + 3*LABLE_HEIGHT
				      ,110,LABLE_HEIGHT);

  family_combo = new QComboBox( this, "Family" );
  family_combo->insertItem( "", 0 );
  family_combo->insertItem( "Times" );
  family_combo->insertItem( "Helvetica" );
  family_combo->insertItem( "Courier" );
  family_combo->insertItem( "Symbol" );

  family_combo->setInsertionPolicy(QComboBox::NoInsertion);

  family_combo->setGeometry(6*XOFFSET + LABLE_LENGTH
			    ,8*YOFFSET - COMBO_ADJUST ,4* LABLE_LENGTH,COMBO_BOX_HEIGHT);
  connect( family_combo, SIGNAL(activated(const QString &)),
	   SLOT(family_chosen_slot(const QString &)) );
  //  QToolTip::add( family_combo, "Select Font Family" );


  size_combo = new QComboBox( true, this, "Size" );
  /*size_combo->insertItem( "", 0 );
  size_combo->insertItem( "4" );
  size_combo->insertItem( "5" );
  size_combo->insertItem( "6" );
  size_combo->insertItem( "7" );
  size_combo->insertItem( "8" );
  size_combo->insertItem( "9" );
  size_combo->insertItem( "10" );
  size_combo->insertItem( "11" );
  size_combo->insertItem( "12" );
  size_combo->insertItem( "13" );
  size_combo->insertItem( "14" );
  size_combo->insertItem( "15" );
  size_combo->insertItem( "16" );
  size_combo->insertItem( "17" );
  size_combo->insertItem( "18" );
  size_combo->insertItem( "19" );
  size_combo->insertItem( "20" );
  size_combo->insertItem( "22" );
  size_combo->insertItem( "24" );
  size_combo->insertItem( "26" );
  size_combo->insertItem( "28" );
  size_combo->insertItem( "32" );
  size_combo->insertItem( "48" );
  size_combo->insertItem( "64" );*/
  QStringList lst;
    for ( unsigned int i = 0; i < 100; ++i )
	lst.append( QString( "%1" ).arg( i + 1 ) );

  size_combo->insertStringList( lst );

  size_combo->setInsertionPolicy(QComboBox::NoInsertion);
  size_combo->setGeometry(10*XOFFSET + 6*LABLE_LENGTH
			    ,8*YOFFSET - COMBO_ADJUST
			  ,2*LABLE_LENGTH + 20,COMBO_BOX_HEIGHT);
  connect( size_combo, SIGNAL(activated(const QString &)),
	   SLOT(size_chosen_slot(const QString &)) );
  //  QToolTip::add( size_combo, "Select Font Size in Points" );


  weight_combo = new QComboBox( this, "Weight" );
  weight_combo->insertItem( "", 0 );
  weight_combo->insertItem( i18n("normal") );
  weight_combo->insertItem( i18n("bold") );
  weight_combo->setGeometry(6*XOFFSET + LABLE_LENGTH
			    ,19*YOFFSET - COMBO_ADJUST
			    ,4*LABLE_LENGTH,COMBO_BOX_HEIGHT);
  weight_combo->setInsertionPolicy(QComboBox::NoInsertion);
  connect( weight_combo, SIGNAL(activated(const QString &)),
	   SLOT(weight_chosen_slot(const QString &)) );
  //  QToolTip::add( weight_combo, "Select Font Weight" );

  style_combo = new QComboBox( this, "Style" );
  style_combo->insertItem( "", 0 );
  style_combo->insertItem( i18n("roman") );
  style_combo->insertItem( i18n("italic"), 2 );
  style_combo->setGeometry(10*XOFFSET + 6*LABLE_LENGTH
			    ,19*YOFFSET- COMBO_ADJUST
			   ,2*LABLE_LENGTH + 20,COMBO_BOX_HEIGHT);
  style_combo->setInsertionPolicy(QComboBox::NoInsertion);
  connect( style_combo, SIGNAL(activated(const QString &)),
	   SLOT(style_chosen_slot(const QString &)) );
  // QToolTip::add( style_combo, "Select Font Style" );

  example_label = new QLabel(this,"examples");
  example_label->setFont(selFont);
  example_label->setGeometry(200,160,190, 80);
  example_label->setAlignment(AlignCenter);
  example_label->setBackgroundColor(white);
  example_label->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
  example_label->setLineWidth( 1 );
  example_label->setText(i18n("Dolor Ipse"));
  //  example_label->setAutoResize(true);

  connect(this,SIGNAL(fontSelected( const QFont&  )),
	  this,SLOT(display_example( const QFont&)));

  setCombos();
  display_example( selFont );

  this->resize( 400, 400 );
}

void CellLayoutPageFont::apply( KSpreadCell *_obj )
{
    if ( size_combo->currentItem() != 0 )
	_obj->setTextFontSize( selFont.pointSize() );
    if ( family_combo->currentItem() != 0 )
	_obj->setTextFontFamily( selFont.family() );
    if ( weight_combo->currentItem() != 0 )
	_obj->setTextFontBold( selFont.bold() );
    if ( style_combo->currentItem() != 0 )
	_obj->setTextFontItalic( selFont.italic() );
}

void CellLayoutPageFont::family_chosen_slot(const QString & family)
{
  selFont.setFamily(family);
  //display_example();
  emit fontSelected(selFont);
}

void CellLayoutPageFont::size_chosen_slot(const QString & size)
{
  QString size_string = size;

  selFont.setPointSize(size_string.toInt());
  //display_example();
  emit fontSelected(selFont);
}

void CellLayoutPageFont::weight_chosen_slot(const QString & weight)
{
  QString weight_string = weight;

  if ( weight_string == QString(i18n("normal")))
    selFont.setBold(false);
  if ( weight_string == QString(i18n("bold")))
       selFont.setBold(true);
  // display_example();
  emit fontSelected(selFont);
}

void CellLayoutPageFont::style_chosen_slot(const QString & style)
{
  QString style_string = style;

  if ( style_string == QString(i18n("roman")))
    selFont.setItalic(false);
  if ( style_string == QString(i18n("italic")))
    selFont.setItalic(true);
  //  display_example();
  emit fontSelected(selFont);
}


void CellLayoutPageFont::display_example(const QFont& font)
{
  QString string;

  example_label->setFont(font);
  example_label->repaint();

  printf("FAMILY 2 '%s' %i\n",font.family().ascii(), font.pointSize());

  QFontInfo info = example_label->fontInfo();
  actual_family_label_data->setText(info.family());

  printf("FAMILY 3 '%s' %i\n",info.family().latin1(), info.pointSize());

  string.setNum(info.pointSize());
  actual_size_label_data->setText(string);

  if (info.bold())
    actual_weight_label_data->setText(i18n("Bold"));
  else
    actual_weight_label_data->setText(i18n("Normal"));

  if (info.italic())
    actual_style_label_data->setText(i18n("italic"));
  else
    actual_style_label_data->setText(i18n("roman"));
}

void CellLayoutPageFont::setCombos()
{
 QString string;
 QComboBox* combo;
 int number_of_entries;
 bool found;

 // Needed to initialize this font
 selFont = dlg->textFont;

 combo = family_combo;
 if ( dlg->bTextFontFamily )
 {
     selFont.setFamily( dlg->textFontFamily );
     printf("Family = %s\n",dlg->textFontFamily.data());
     number_of_entries =  family_combo->count();
     string = dlg->textFontFamily;
     found = false;

     for (int i = 1; i < number_of_entries - 1; i++)
     {
	 if ( string == (QString) combo->text(i))
	 {
	     combo->setCurrentItem(i);
	     //     printf("Found Font %s\n",string.data());
	     found = true;
	     break;
	 }
     }
 }
 else
     combo->setCurrentItem( 0 );

 combo = size_combo;
 if ( dlg->bTextFontSize )
 {
     printf("SIZE=%i\n",dlg->textFontSize);
     selFont.setPointSize( dlg->textFontSize );
     number_of_entries = size_combo->count();
     string.setNum( dlg->textFontSize );
     found = false;

     for (int i = 0; i < number_of_entries ; i++){
	 if ( string == (QString) combo->text(i)){
	     combo->setCurrentItem(i);
	     found = true;
	     // printf("Found Size %s setting to item %d\n",string.data(),i);
	     break;
	 }
     }
 }
 else
     combo->setCurrentItem( 0 );

 if ( !dlg->bTextFontBold )
     weight_combo->setCurrentItem(0);
 else if ( dlg->textFontBold )
 {
     selFont.setBold( dlg->textFontBold );
     weight_combo->setCurrentItem(2);
 }
 else
 {
     selFont.setBold( dlg->textFontBold );
     weight_combo->setCurrentItem(1);
 }

 if ( !dlg->bTextFontItalic )
     weight_combo->setCurrentItem(0);
 else if ( dlg->textFontItalic )
 {
     selFont.setItalic( dlg->textFontItalic );
     style_combo->setCurrentItem(2);
 }
 else
 {
     selFont.setItalic( dlg->textFontItalic );
     style_combo->setCurrentItem(1);
 }
}

CellLayoutPagePosition::CellLayoutPagePosition( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget( parent )
{
    dlg = _dlg;
    QVBoxLayout *lay1 = new QVBoxLayout( this );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );
    QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, "Horizontal",this);
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);
    left = new QRadioButton( i18n("Left"), grp );
    center = new QRadioButton( i18n("Center"), grp );
    right = new QRadioButton( i18n("Right"), grp );
    setCaption( i18n("Horizontal") );

    if(dlg->alignX==KSpreadCell::Left)
        left->setChecked(true);
    else if(dlg->alignX==KSpreadCell::Center)
        center->setChecked(true);
    else if(dlg->alignX==KSpreadCell::Right)
        right->setChecked(true);


    grp = new QButtonGroup( 1, QGroupBox::Horizontal, "Vertical",this);
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);
    top = new QRadioButton( i18n("Top"), grp );
    middle = new QRadioButton( i18n("Middle"), grp );
    bottom = new QRadioButton( i18n("Bottom"), grp );

    setCaption( i18n("Vertical") );
    if(dlg->alignY==KSpreadCell::Top)
        top->setChecked(true);
    else if(dlg->alignY==KSpreadCell::Middle)
        middle->setChecked(true);
    else if(dlg->alignY==KSpreadCell::Bottom)
        bottom->setChecked(true);

    this->resize( 400, 400 );
}

void CellLayoutPagePosition::apply( KSpreadCell *_obj )
{
if(top->isChecked())
        _obj->setAlignY(KSpreadCell::Top);
else if(bottom->isChecked())
        _obj->setAlignY(KSpreadCell::Bottom);
else if(middle->isChecked())
        _obj->setAlignY(KSpreadCell::Middle);

if(left->isChecked())
        _obj->setAlign(KSpreadCell::Left);
else if(right->isChecked())
        _obj->setAlign(KSpreadCell::Right);
else if(center->isChecked())
        _obj->setAlign(KSpreadCell::Center);
}


#include "kspread_dlg_layout.moc"
