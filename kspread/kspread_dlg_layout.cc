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
#include <math.h>

#include "kspread_dlg_layout.h"
#include "kspread_undo.h"
#include "kspread_table.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_view.h"
#include "kspread_canvas.h"

#include <qlabel.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qfontdatabase.h>
#include <kcolordlg.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <knumvalidator.h>

KSpreadPatternSelect::KSpreadPatternSelect( QWidget *parent, const char * ) : QFrame( parent )
{
    penStyle = NoPen;
    penWidth = 1;
    penColor = colorGroup().text();
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
        painter.drawLine( 6, height()/2, width() - 6,height()/2  );
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

CellLayoutDlg::CellLayoutDlg( KSpreadView *_view, KSpreadTable *_table, int _left, int _top,
                              int _right, int _bottom ) : QObject()
{
    undefinedPixmap = 0L;
    formatOnlyNegSignedPixmap = 0L;
    formatRedOnlyNegSignedPixmap = 0L;
    formatRedNeverSignedPixmap = 0L;
    formatAlwaysSignedPixmap = 0L;
    formatRedAlwaysSignedPixmap = 0L;
    undefinedPixmap = 0L;
    table = _table;
    left = _left;
    top = _top;
    right = _right;
    bottom = _bottom;
    m_pView = _view;

    m_bValue=false;
    m_bDate=false;
    m_bTime=false;

    KSpreadCell *obj = table->cellAt( _left, _top );
    oneCell=false;
    if( left==right && top==bottom)
        {
        if(obj->extraXCells()!=0 ||obj->extraYCells()!=0)
                isMerged=true;
        else
                {
                oneCell=true;
                isMerged=false;
                }
        }
    else
        isMerged=false;
    // Initialize with the upper left object.
    leftBorderStyle = obj->leftBorderStyle( _left, _top );
    leftBorderWidth = obj->leftBorderWidth( _left, _top );
    leftBorderColor = obj->leftBorderColor( _left, _top );
    topBorderStyle = obj->topBorderStyle( _left, _top );
    topBorderWidth = obj->topBorderWidth( _left, _top );
    topBorderColor = obj->topBorderColor( _left, _top );
    fallDiagonalStyle = obj->fallDiagonalStyle( _left, _top );
    fallDiagonalWidth = obj->fallDiagonalWidth( _left, _top );
    fallDiagonalColor = obj->fallDiagonalColor( _left, _top );
    goUpDiagonalStyle = obj->goUpDiagonalStyle( _left, _top );
    goUpDiagonalWidth = obj->goUpDiagonalWidth( _left, _top );
    goUpDiagonalColor = obj->goUpDiagonalColor( _left, _top );

    // Look at the upper right one for the right border.
    obj = table->cellAt( _right, _top );
    rightBorderStyle = obj->rightBorderStyle( _right, _top );
    rightBorderWidth = obj->rightBorderWidth( _right, _top );
    rightBorderColor = obj->rightBorderColor( _right, _top );

    // Look at the bottom left cell for the bottom border.
    obj = table->cellAt( _left, _bottom );
    bottomBorderStyle = obj->bottomBorderStyle( _left, _bottom );
    bottomBorderWidth = obj->bottomBorderWidth( _left, _bottom );
    bottomBorderColor = obj->bottomBorderColor( _left, _bottom );

    // Just an assumption
    obj = table->cellAt( _right, _top );
    if(obj->isObscuringForced())
        {
        int moveX=obj->obscuringCellsColumn();
        int moveY=_top;
        obj = table->cellAt( moveX,  moveY );
        verticalBorderStyle = obj->leftBorderStyle( moveX,moveY );
        verticalBorderWidth = obj->leftBorderWidth( moveX, moveY);
        verticalBorderColor = obj->leftBorderColor( moveX, moveY );
        }
    else
        {
        verticalBorderStyle = obj->leftBorderStyle( _right, _top );
        verticalBorderWidth = obj->leftBorderWidth( _right, _top );
        verticalBorderColor = obj->leftBorderColor( _right, _top );
        }

    if(obj->isObscuringForced())
        {
        int moveX=_right;
        int moveY=obj->obscuringCellsRow();
        obj = table->cellAt( moveX,  moveY );
        horizontalBorderStyle = obj->topBorderStyle( moveX, moveY );
        horizontalBorderWidth = obj->topBorderWidth( moveX, moveY );
        horizontalBorderColor = obj->topBorderColor( moveX, moveY );
        }
    else
        {
        horizontalBorderStyle = obj->topBorderStyle( _right, _bottom );
        horizontalBorderWidth = obj->topBorderWidth( _right, _bottom );
        horizontalBorderColor = obj->topBorderColor( _right, _bottom );
        }

    obj = table->cellAt( _left, _top );
    prefix = obj->prefix( _left, _top );
    postfix = obj->postfix( _left, _top );
    precision = obj->precision( _left, _top );
    floatFormat = obj->floatFormat( _left, _top );
    floatColor = obj->floatColor( _left, _top );
    alignX = obj->align( _left, _top );
    alignY = obj->alignY( _left, _top );
    textColor = obj->textColor( _left, _top );
    bgColor = obj->bgColor( _left, _top );
    textFontSize = obj->textFontSize( _left, _top );
    textFontFamily = obj->textFontFamily( _left, _top );
    textFontBold = obj->textFontBold( _left, _top );
    textFontItalic = obj->textFontItalic( _left, _top );
    strike=obj->textFontStrike( _left, _top );
    underline=obj->textFontUnderline( _left, _top );
    // Needed to initialize the font correctly ( bug in Qt )
    textFont = obj->textFont( _left, _top );
    eStyle = obj->style();
    actionText = obj->action();

    brushColor = obj->backGroundBrushColor( _left, _top );
    brushStyle = obj->backGroundBrushStyle( _left,_top );

    bMultiRow = obj->multiRow( _left, _top );
    bVerticalText=obj->verticalText( _left, _top );

    textRotation = obj->getAngle(_left, _top);
    formatNumber = obj->getFormatNumber(_left, _top);

    indent = obj->getIndent(_left, _top);

    cellText=obj->text();

    if( obj->isValue())
        {
        m_bValue=true;
        m_value=obj->valueDouble();
        }
    else if(obj->isDate())
        {
        m_bDate=true;
        m_date=obj->valueDate();
        }
    else if(obj->isTime())
        {
        m_bTime=true;
        m_time=obj->valueTime();
        }

    RowLayout *rl;
    ColumnLayout *cl;
    widthSize = 0;
    heigthSize = 0;
    for ( int x = _left; x <= _right; x++ )
    {
        cl = m_pView->activeTable()->columnLayout( x );
        widthSize = QMAX( cl->width( /* m_pView->canvasWidget() */ ), widthSize );
    }

    for ( int y = _top; y <= _bottom; y++ )
    {
        rl = m_pView->activeTable()->rowLayout(y);
        heigthSize = QMAX( rl->height( /* m_pView->canvasWidget() */ ),heigthSize );
    }

    // We assume, that all other objects have the same values
    bLeftBorderStyle = TRUE;
    bLeftBorderColor = TRUE;
    bRightBorderStyle = TRUE;
    bRightBorderColor = TRUE;
    bTopBorderStyle = TRUE;
    bTopBorderColor = TRUE;
    bBottomBorderColor = TRUE;
    bBottomBorderStyle = TRUE;
    bVerticalBorderColor = TRUE;
    bVerticalBorderStyle = TRUE;
    bHorizontalBorderColor = TRUE;
    bHorizontalBorderStyle = TRUE;
    bFallDiagonalStyle = TRUE;
    bfallDiagonalColor = TRUE;
    bGoUpDiagonalStyle = TRUE;
    bGoUpDiagonalColor = TRUE;
    bFloatFormat = TRUE;
    bFloatColor = TRUE;
    bTextColor = TRUE;
    bBgColor = TRUE;
    bTextFontFamily = TRUE;
    bTextFontSize = TRUE;
    bTextFontBold = TRUE;
    bTextFontItalic = TRUE;
    bStrike = TRUE;
    bUnderline = TRUE;
    bTextRotation = TRUE;
    bFormatNumber = TRUE;
    if( left == right )
        oneCol = TRUE;
    else
        oneCol = FALSE;

    if( top == bottom )
        oneRow = TRUE;
    else
        oneRow = FALSE;

    // Do the other objects have the same values ?
    for ( int x = _left; x <= _right; x++ )
    {
        for ( int y = _top; y <= _bottom; y++ )
        {
            KSpreadCell *obj = table->cellAt( x, y );
            if(!obj->isObscuringForced())
                {
                if ( fallDiagonalStyle != obj->fallDiagonalStyle( x, y ) )
                        bFallDiagonalStyle = FALSE;
                if ( fallDiagonalWidth != obj->fallDiagonalWidth( x, y ) )
                        bFallDiagonalStyle = FALSE;
                if ( fallDiagonalColor != obj->fallDiagonalColor( x, y ) )
                        bfallDiagonalColor = FALSE;
                if ( goUpDiagonalStyle != obj->goUpDiagonalStyle( x, y ) )
                        bGoUpDiagonalStyle = FALSE;
                if ( goUpDiagonalWidth != obj->goUpDiagonalWidth( x, y ) )
                        bGoUpDiagonalStyle = FALSE;
                if ( goUpDiagonalColor != obj->goUpDiagonalColor( x, y ) )
                        bGoUpDiagonalColor = FALSE;
                if ( strike != obj->textFontStrike( x, y ) )
                        bStrike = FALSE;
                if ( underline != obj->textFontUnderline( x, y ) )
                        bUnderline = FALSE;
                if ( prefix != obj->prefix( x, y ) )
                        prefix = QString::null;
                if ( postfix != obj->postfix( x, y ) )
                        postfix = QString::null;
                /*if ( precision != obj->precision( x, y ) )
                        precision = -2;*/
                if ( floatFormat != obj->floatFormat( x, y ) )
                        bFloatFormat = FALSE;
                if ( floatColor != obj->floatColor( x, y ) )
                        bFloatColor = FALSE;
                if ( textColor != obj->textColor( x, y ) )
                        bTextColor = FALSE;
                if ( textFontFamily != obj->textFontFamily( x, y ) )
                        bTextFontFamily = FALSE;
                if ( textFontSize != obj->textFontSize( x, y ) )
                        bTextFontSize = FALSE;
                if ( textFontBold != obj->textFontBold( x, y ) )
                        bTextFontBold = FALSE;
                if ( textFontItalic != obj->textFontItalic( x, y ) )
                        bTextFontItalic = FALSE;
                if ( bgColor != obj->bgColor( x, y ) )
                        bBgColor = FALSE;
                if( textRotation != obj->getAngle(_left, _top) )
                        bTextRotation = FALSE;
                if( formatNumber != obj->getFormatNumber(_left, _top) )
                        bFormatNumber = FALSE;
                if ( eStyle != obj->style() )
                        eStyle = KSpreadCell::ST_Undef;
                }
        }
    }

    if( !bTextRotation )
        textRotation = 0;

    for ( int y = _top; y <= _bottom; y++ )
    {
        KSpreadCell *obj = table->cellAt( _left, y );
        if(!obj->isObscuringForced())
                {
                if ( leftBorderStyle != obj->leftBorderStyle( _left, y ) )
                        bLeftBorderStyle = FALSE;
                if ( leftBorderWidth != obj->leftBorderWidth( _left, y ) )
                        bLeftBorderStyle = FALSE;
                if ( leftBorderColor != obj->leftBorderColor( _left, y ) )
                        bLeftBorderColor = FALSE;
                }
    }

    bool once=false;
    for ( int y = _top; y <= _bottom; y++ )
    {
        KSpreadCell *obj = table->cellAt( _right, y );
        if(!obj->isObscuringForced())
                {
                if ( rightBorderStyle != obj->rightBorderStyle( _right, y ) )
                        bRightBorderStyle = FALSE;
                if ( rightBorderWidth != obj->rightBorderWidth( _right, y ) )
                        bRightBorderStyle = FALSE;
                if ( rightBorderColor != obj->rightBorderColor( _right, y ) )
                        bRightBorderColor = FALSE;
                }
        else if( obj->isObscuringForced() && !once)
                {
                once=true;
                int moveX=obj->obscuringCellsColumn();
                int moveY=obj->obscuringCellsRow();
                obj = table->nonDefaultCell( moveX,  moveY );
                if ( rightBorderStyle != obj->rightBorderStyle( moveX,  moveY ) )
                        bRightBorderStyle = FALSE;
                if ( rightBorderWidth != obj->rightBorderWidth(moveX,  moveY ) )
                        bRightBorderStyle = FALSE;
                if ( rightBorderColor != obj->rightBorderColor( moveX,  moveY ) )
                        bRightBorderColor = FALSE;
                }
    }

    for ( int x = _left; x <= _right; x++ )
    {
        KSpreadCell *obj = table->cellAt( x, _top );
        if(!obj->isObscuringForced())
                {
                if (  topBorderStyle != obj->topBorderStyle( x, _top ) )
                        bTopBorderStyle = FALSE;
                if ( topBorderWidth != obj->topBorderWidth( x, _top ) )
                        bTopBorderStyle = FALSE;
                if ( topBorderColor != obj->topBorderColor( x, _top ) )
                        bTopBorderColor = FALSE;
                }
    }

    once = false;
    for ( int x = _left; x <= _right; x++ )
    {
        KSpreadCell *obj = table->cellAt( x, _bottom );
        if(!obj->isObscuringForced())
                {
                if ( bottomBorderStyle != obj->bottomBorderStyle( x, _bottom ) )
                        bBottomBorderStyle = FALSE;
                if ( bottomBorderWidth != obj->bottomBorderWidth( x, _bottom ) )
                        bBottomBorderStyle = FALSE;
                if ( bottomBorderColor != obj->bottomBorderColor( x, _bottom ) )
                        bBottomBorderColor = FALSE;
                }
        else if( obj->isObscuringForced() && !once)
                {
                once=true;
                int moveX=obj->obscuringCellsColumn();
                int moveY=obj->obscuringCellsRow();
                obj = table->nonDefaultCell( moveX,  moveY );
                if ( bottomBorderStyle != obj->bottomBorderStyle( moveX,  moveY ) )
                        bBottomBorderStyle = FALSE;
                if ( bottomBorderWidth != obj->bottomBorderWidth( moveX,  moveY ) )
                        bBottomBorderStyle = FALSE;
                if ( bottomBorderColor != obj->bottomBorderColor( moveX,  moveY ) )
                        bBottomBorderColor = FALSE;
                }
    }


    // Look for the Outline
    for ( int x = _left; x <= _right; x++ )
    {
        for ( int y = _top+1; y <= _bottom; y++ )
        {
            KSpreadCell *obj = table->cellAt( x, y );
            if(!obj->isObscuringForced())
                {
                if ( horizontalBorderStyle != obj->topBorderStyle( x, y ) )
                        bHorizontalBorderStyle = FALSE;
                if ( horizontalBorderWidth != obj->topBorderWidth( x, y ) )
                        bHorizontalBorderStyle = FALSE;
                if ( horizontalBorderColor != obj->topBorderColor( x, y ) )
                        bHorizontalBorderColor = FALSE;
                }
        }
    }

    for ( int x = _left+1; x <= _right; x++ )
    {
        for ( int y = _top; y <= _bottom; y++ )
        {
            KSpreadCell *obj = table->cellAt( x, y );
            if(!obj->isObscuringForced())
                {
                if ( verticalBorderStyle != obj->leftBorderStyle( x, y ) )
                        bVerticalBorderStyle = FALSE;
                if ( verticalBorderWidth != obj->leftBorderWidth( x, y ) )
                        bVerticalBorderStyle = FALSE;
                if ( verticalBorderColor != obj->leftBorderColor( x, y ) )
                        bVerticalBorderColor = FALSE;
                }
        }
    }

    init();
}

CellLayoutDlg::~CellLayoutDlg()
{
  delete undefinedPixmap;
  delete formatOnlyNegSignedPixmap;
  delete formatRedOnlyNegSignedPixmap;
  delete formatRedNeverSignedPixmap;
  delete formatAlwaysSignedPixmap;
  delete formatRedAlwaysSignedPixmap;
}

void CellLayoutDlg::init()
{
    QColorGroup colorGroup = QApplication::palette().active();
    // Did we initialize the bitmaps ?
    if ( formatOnlyNegSignedPixmap == 0L )
    {
        QColor black = colorGroup.text(); // not necessarily black :)
        formatOnlyNegSignedPixmap = paintFormatPixmap( "123.456", black, "-123.456", black );
        formatRedOnlyNegSignedPixmap = paintFormatPixmap( "123.456", black, "-123.456", Qt::red );
        formatRedNeverSignedPixmap = paintFormatPixmap( "123.456", black, "123.456", Qt::red );
        formatAlwaysSignedPixmap = paintFormatPixmap( "+123.456", black, "-123.456", black );
        formatRedAlwaysSignedPixmap = paintFormatPixmap( "+123.456", black, "-123.456", Qt::red );

        // Make the undefined pixmap
        undefinedPixmap = new QPixmap( 100, 12 );
        QPainter painter;
        painter.begin( undefinedPixmap );
        painter.setBackgroundColor( colorGroup.base() );
        painter.setPen( colorGroup.text() );
        painter.fillRect( 0, 0, 100, 12, BDiagPattern );
        painter.end();
    }

    tab = new QTabDialog( (QWidget*)m_pView, 0L, TRUE );
    tab->setGeometry( tab->x(), tab->y(), 420, 400 );

    borderPage = new CellLayoutPageBorder( tab, this );
    tab->addTab( borderPage, i18n("&Border") );

    floatPage = new CellLayoutPageFloat( tab, this );
    tab->addTab( floatPage, i18n("&Number Format") );

    miscPage = new CellLayoutPageMisc( tab, this );
    tab->addTab( miscPage, i18n("&Misc") );

    fontPage = new CellLayoutPageFont( tab, this );
    tab->addTab( fontPage, i18n("&Text") );

    positionPage = new CellLayoutPagePosition( tab, this);
    tab->addTab( positionPage, i18n("&Position") );

    patternPage=new CellLayoutPagePattern(tab,this);
    tab->addTab( patternPage,i18n("Back&ground"));

    tab->setCancelButton();

    tab->setCaption(i18n("Cell Layout"));

    connect( tab, SIGNAL( applyButtonPressed() ), this, SLOT( slotApply() ) );

    tab->show();
}

QPixmap* CellLayoutDlg::paintFormatPixmap( const char *_string1, const QColor & _color1,
                                             const char *_string2, const QColor & _color2 )
{
    QPixmap *pixmap = new QPixmap( 150, 14 );

    QPainter painter;
    painter.begin( pixmap );
    painter.fillRect( 0, 0, 150, 14, QApplication::palette().active().base() );
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

    if( isMerged!= positionPage->getMergedCellState())
    {
        if(positionPage->getMergedCellState())
                {
                //merge cell
                table->mergeCell(QPoint(left,top));
                right=left;
                bottom=top;
                }
        else
                {
                //dissociate cells
                KSpreadCell *obj = table->nonDefaultCell( top, left );
                right=obj->extraXCells()+left;
                bottom=obj->extraYCells()+top;
                table->dissociateCell(QPoint(left,top));
                }

    }
    // Prepare the undo buffer
    if ( !table->doc()->undoBuffer()->isLocked() )
    {
        QRect rect;
        // Since the right/bottom border is stored in objects right + 1 ( or: bottom + 1 )
        // So we have to save these layouts, too
        rect.setCoords( left, top, right + 1, bottom + 1 );
        KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( table->doc(), table, rect );
        table->doc()->undoBuffer()->appendUndo( undo );
    }

    for ( int x = left; x <= right; x++ )
        for ( int y = top; y <= bottom; y++ )
        {
            KSpreadCell *obj = table->nonDefaultCell( x, y );
            if(!obj->isObscuringForced())
                {
                floatPage->apply( obj );
                miscPage->apply( obj );
                fontPage->apply( obj );
                positionPage->apply( obj );
                patternPage->apply(obj);
                }
        }

    if(positionPage->getSizeHeight()!=heigthSize
    || positionPage->getSizeWidth()!=widthSize)
    {
        if ( !table->doc()->undoBuffer()->isLocked())
        {
                QRect rect;
                rect.setCoords( left, top, right , bottom  );
                KSpreadUndoResizeColRow *undo2 = new KSpreadUndoResizeColRow( table->doc(),table , rect );
                table->doc()->undoBuffer()->appendUndo( undo2 );
        }
    }
    if(positionPage->getSizeHeight()!=heigthSize)
        {
        for ( int x = top; x <= bottom; x++ )
                {
                m_pView->vBorderWidget()->resizeRow(positionPage->getSizeHeight(),x,false );
                }
        }
    if(positionPage->getSizeWidth()!=widthSize)
        {
        for ( int x = left; x <= right; x++ )
                {
                m_pView->hBorderWidget()->resizeColumn(positionPage->getSizeWidth(),x,false );
                }
        }

    // Outline
      borderPage->applyOutline( left, top, right, bottom );

    // m_pView->drawVisibleCells();
    QRect r;
    r.setCoords( left, top, right, bottom );
    m_pView->slotUpdateView( table, r );
    m_pView->doc()->setModified( true );
    // Update the toolbar (bold/italic/font...)
    m_pView->updateEditWidget();
}


CellLayoutPageFloat::CellLayoutPageFloat( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget ( parent )
{
    dlg = _dlg;

    QVBoxLayout* layout = new QVBoxLayout( this, 6,10 );
    QGroupBox *box = new QGroupBox( this, "Box");

    QGridLayout *grid = new QGridLayout(box,3,4,7,7);


    postfix = new QLineEdit( box, "LineEdit_1" );
    grid->addWidget(postfix,2,1);
    precision = new KIntNumInput( dlg->precision, box, 10 );
    precision->setSpecialValueText(i18n("variable"));
    precision->setRange(-1,10,1,false);

    grid->addWidget(precision,1,1);

    prefix = new QLineEdit( box, "LineEdit_3" );
    grid->addWidget(prefix,0,1);

    format = new QComboBox( box, "ListBox_1" );
    grid->addWidget(format,0,3);

    QLabel* tmpQLabel;
    tmpQLabel = new QLabel( box, "Label_1" );
    grid->addWidget(tmpQLabel,2,0);
    tmpQLabel->setText( i18n("Prefix") );

    if ( dlg->postfix.isNull() )
        postfix->setText( "########" );
    else
        postfix->setText( dlg->postfix );

    tmpQLabel = new QLabel( box, "Label_2" );
    grid->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText( i18n("Postfix") );

    tmpQLabel = new QLabel( box, "Label_3" );
    grid->addWidget(tmpQLabel,1,0);
    tmpQLabel->setText( i18n("Precision") );

    if ( dlg->prefix.isNull() )
        prefix->setText( "########" );
    else
        prefix->setText( dlg->prefix );


    format->insertItem( *_dlg->formatOnlyNegSignedPixmap, 0 );
    format->insertItem( *_dlg->formatRedOnlyNegSignedPixmap, 1 );
    format->insertItem( *_dlg->formatRedNeverSignedPixmap, 2 );
    format->insertItem( *_dlg->formatAlwaysSignedPixmap, 3 );
    format->insertItem( *_dlg->formatRedAlwaysSignedPixmap, 4 );
    format->insertItem( *_dlg->undefinedPixmap, 5 );

    tmpQLabel = new QLabel( box, "Label_4" );
    grid->addWidget(tmpQLabel,0,2);
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
    layout->addWidget(box);


    QButtonGroup *grp = new QButtonGroup( i18n("Format"),this);
    grid = new QGridLayout(grp,7,2,15,7);
    grp->setRadioButtonExclusive( TRUE );
    number=new QRadioButton(i18n("Number"),grp);
    grid->addWidget(number,0,0);

    percent=new QRadioButton(i18n("Percent"),grp);
    grid->addWidget(percent,1,0);

    money=new QRadioButton(i18n("Money"),grp);
    grid->addWidget(money,2,0);

    scientific=new QRadioButton(i18n("Scientific"),grp);
    grid->addWidget(scientific,3,0);

    fraction=new QRadioButton(i18n("Fraction"),grp);
    grid->addWidget(fraction,4,0);

    date=new QRadioButton(i18n("Date Format"),grp);
    grid->addWidget(date,5,0);

    time=new QRadioButton(i18n("Time Format"),grp);
    grid->addWidget(time,6,0);

    QGroupBox *box2 = new QGroupBox( grp, "Box");
    box2->setTitle(i18n("Preview"));
    QGridLayout *grid3 = new QGridLayout(box2,1,3,14,7);

    exampleLabel=new QLabel(box2);
    grid3->addWidget(exampleLabel,0,1);

    grid->addMultiCellWidget(box2,0,1,1,1);

    listFormat=new QListBox(grp);
    grid->addMultiCellWidget(listFormat,2,6,1,1);
    layout->addWidget(grp);

    if(!dlg->bFormatNumber)
          number->setChecked(true);
    else
        {
        if(dlg->formatNumber==KSpreadCell::Number)
                number->setChecked(true);
        else if(dlg->formatNumber==KSpreadCell::Percentage)
                percent->setChecked(true);
        else if(dlg->formatNumber==KSpreadCell::Money)
                money->setChecked(true);
        else if(dlg->formatNumber==KSpreadCell::Scientific)
                scientific->setChecked(true);
        else if(dlg->formatNumber==KSpreadCell::TextDate ||
        dlg->formatNumber==KSpreadCell::ShortDate
        ||((int)(dlg->formatNumber)>=200 && (int)(dlg->formatNumber)<=217))
                date->setChecked(true);
        else if(dlg->formatNumber==KSpreadCell::Time ||
        dlg->formatNumber==KSpreadCell::SecondeTime
        ||dlg->formatNumber==KSpreadCell::Time_format1
        ||dlg->formatNumber==KSpreadCell::Time_format2
        ||dlg->formatNumber==KSpreadCell::Time_format3)
                time->setChecked(true);
        else if(dlg->formatNumber==KSpreadCell::fraction_half ||
        dlg->formatNumber==KSpreadCell::fraction_quarter ||
        dlg->formatNumber==KSpreadCell::fraction_eighth ||
        dlg->formatNumber==KSpreadCell::fraction_sixteenth ||
        dlg->formatNumber==KSpreadCell::fraction_tenth ||
        dlg->formatNumber==KSpreadCell::fraction_hundredth ||
        dlg->formatNumber==KSpreadCell::fraction_one_digit ||
        dlg->formatNumber==KSpreadCell::fraction_two_digits ||
        dlg->formatNumber==KSpreadCell::fraction_three_digits)
                fraction->setChecked(true);
        }
    connect(fraction,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(money,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(date,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(scientific,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(number,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(percent,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(time,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(listFormat,SIGNAL(selectionChanged ()),this,SLOT(makeformat()));
    connect(precision,SIGNAL(valueChanged(int)),this,SLOT(slotChangeValue(int)));
    connect(prefix,SIGNAL(textChanged ( const QString & ) ),this,SLOT(makeformat()));
    connect(postfix,SIGNAL(textChanged ( const QString & ) ),this,SLOT(makeformat()));
    slotChangeState();
    this->resize( 400, 400 );
}

void CellLayoutPageFloat::slotChangeValue(int)
{
makeformat();
}
void CellLayoutPageFloat::slotChangeState()
{
QStringList list;
listFormat->clear();
if(dlg->cellText.isEmpty() || dlg->m_bValue)
        {
        precision->setEnabled(true);
        prefix->setEnabled(true);
        postfix->setEnabled(true);
        format->setEnabled(true);
        }
else
        {
        precision->setEnabled(false);
        prefix->setEnabled(false);
        postfix->setEnabled(false);
        format->setEnabled(false);
        }
if(number->isChecked())
        listFormat->setEnabled(false);
else if(percent->isChecked())
        listFormat->setEnabled(false);
else if(money->isChecked())
        listFormat->setEnabled(false);
else if(scientific->isChecked())
        listFormat->setEnabled(false);
else if(date->isChecked())
        {
        format->setEnabled(false);
        precision->setEnabled(false);
        prefix->setEnabled(false);
        postfix->setEnabled(false);
        listFormat->setEnabled(true);
        init();
        }
else if(fraction->isChecked())
        {
        precision->setEnabled(false);
        listFormat->setEnabled(true);
        list+=i18n("Demis 1/2");
        list+=i18n("Quarters 1/4");
        list+=i18n("Eighths 1/8");
        list+=i18n("Sixteenths 1/16");
        list+=i18n("Tenths 1/10");
        list+=i18n("Hundredth 1/100");
        list+=i18n("One digit 5/9");
        list+=i18n("Two digits 15/22");
        list+=i18n("Three digits 153/652");
        listFormat->insertStringList(list);
        if(dlg->formatNumber==KSpreadCell::fraction_half)
                listFormat->setCurrentItem(0);
        else if(dlg->formatNumber==KSpreadCell::fraction_quarter)
                listFormat->setCurrentItem(1);
        else if(dlg->formatNumber==KSpreadCell::fraction_eighth )
                listFormat->setCurrentItem(2);
        else if(dlg->formatNumber==KSpreadCell::fraction_sixteenth )
                listFormat->setCurrentItem(3);
        else if(dlg->formatNumber==KSpreadCell::fraction_tenth )
                listFormat->setCurrentItem(4);
        else if(dlg->formatNumber==KSpreadCell::fraction_hundredth )
                listFormat->setCurrentItem(5);
        else if(dlg->formatNumber==KSpreadCell::fraction_one_digit )
                listFormat->setCurrentItem(6);
        else if(dlg->formatNumber==KSpreadCell::fraction_two_digits )
                listFormat->setCurrentItem(7);
        else if(dlg->formatNumber==KSpreadCell::fraction_three_digits )
                listFormat->setCurrentItem(8);
        else
                listFormat->setCurrentItem(0);
        }
else if(time->isChecked())
        {
        precision->setEnabled(false);
        prefix->setEnabled(false);
        postfix->setEnabled(false);
        format->setEnabled(false);
        listFormat->setEnabled(true);

        list+=i18n("System : ")+KGlobal::locale()->formatTime(QTime::currentTime(),false);
        list+=i18n("System : ")+KGlobal::locale()->formatTime(QTime::currentTime(),true);
        QString tmp;
        list+=tmp.setNum(10)+":"+tmp.setNum(35) +" " + i18n("PM");
        list+=tmp.setNum(10)+":"+tmp.setNum(35)+":"+tmp.setNum(25)+" " + i18n("PM");
        list+=tmp.setNum(10)+" "+i18n("h")+" "+tmp.setNum(35)+" "+i18n("min")+" "+tmp.setNum(25)  +" "+ i18n("s");

        listFormat->insertStringList(list);
        if( dlg->formatNumber==KSpreadCell::Time )
                listFormat->setCurrentItem(0);
        else if(dlg->formatNumber==KSpreadCell::SecondeTime)
                listFormat->setCurrentItem(1);
        else if(dlg->formatNumber==KSpreadCell::Time_format1)
                listFormat->setCurrentItem(2);
        else if(dlg->formatNumber==KSpreadCell::Time_format2)
                listFormat->setCurrentItem(3);
        else if(dlg->formatNumber==KSpreadCell::Time_format3)
                listFormat->setCurrentItem(4);
        else
                listFormat->setCurrentItem(0);
        }
if( date->isChecked() && dlg->m_bDate)
        makeDateFormat();
else
        makeformat();
}

void CellLayoutPageFloat::makeDateFormat()
{
QString tmp,tmp2;
if( listFormat->currentItem()==0)
        tmp=KGlobal::locale()->formatDate(dlg->m_date,true);
else if(listFormat->currentItem()==1)
        tmp=KGlobal::locale()->formatDate(dlg->m_date,false);
else if(listFormat->currentItem()==2)/*18-Feb-99*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day());
        tmp=tmp+"-"+KGlobal::locale()->monthName(dlg->m_date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(dlg->m_date.year()).right(2);
        }
else if(listFormat->currentItem()==3) /*18-Feb-1999*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day());
        tmp=tmp+"-"+KGlobal::locale()->monthName(dlg->m_date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(dlg->m_date.year());
        }
else if(listFormat->currentItem()==4) /*18-Feb*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day());
        tmp=tmp+"-"+KGlobal::locale()->monthName(dlg->m_date.month(), true);
        }
else if(listFormat->currentItem()==5) /*18-05*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day());
        tmp=tmp+"-"+QString().sprintf("%02d", dlg->m_date.month());
        }
else if(listFormat->currentItem()==6) /*18/05/00*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day());
        tmp=tmp+"/"+QString().sprintf("%02d", dlg->m_date.month())+"/";
        tmp=tmp+tmp2.setNum(dlg->m_date.year()).right(2);
        }
else if(listFormat->currentItem()==7) /*18/05/1999*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day());
        tmp=tmp+"/"+QString().sprintf("%02d", dlg->m_date.month())+"/";
        tmp=tmp+tmp2.setNum(dlg->m_date.year());
        }
else if(listFormat->currentItem()==8) /*Feb-99*/
        {
        tmp=KGlobal::locale()->monthName(dlg->m_date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(dlg->m_date.year()).right(2);
        }
else if(listFormat->currentItem()==9) /*February-99*/
        {
        tmp=KGlobal::locale()->monthName(dlg->m_date.month())+"-";
        tmp=tmp+tmp2.setNum(dlg->m_date.year()).right(2);
        }
else if(listFormat->currentItem()==10) /*February-1999*/
        {
        tmp=KGlobal::locale()->monthName(dlg->m_date.month())+"-";
        tmp=tmp+tmp2.setNum(dlg->m_date.year());
        }
else if(listFormat->currentItem()==11) /*F-99*/
        {
        tmp=KGlobal::locale()->monthName(dlg->m_date.month()).at(0)+"-";
        tmp=tmp+tmp2.setNum(dlg->m_date.year()).right(2);
        }
else if(listFormat->currentItem()==12) /*18/Feb*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day())+"/";
        tmp+=KGlobal::locale()->monthName(dlg->m_date.month(),true);
        }
else if(listFormat->currentItem()==13) /*18/02*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day())+"/";
        tmp+=QString().sprintf("%02d", dlg->m_date.month());
        }
else if(listFormat->currentItem()==14) /*18/Feb/1999*/
        {
        tmp=QString().sprintf("%02d", dlg->m_date.day());
        tmp=tmp+"/"+KGlobal::locale()->monthName(dlg->m_date.month(),true)+"/";
        tmp=tmp+tmp2.setNum(dlg->m_date.year());
        }
else if(listFormat->currentItem()==15) /*2000/Feb/18*/
        {
        tmp=tmp2.setNum(dlg->m_date.year());
        tmp=tmp+"/"+KGlobal::locale()->monthName(dlg->m_date.month(),true)+"/";
        tmp=tmp+QString().sprintf("%02d", dlg->m_date.day());
        }
else if(listFormat->currentItem()==16) /*2000-Feb-18*/
        {
        tmp=tmp2.setNum(dlg->m_date.year());
        tmp=tmp+"-"+KGlobal::locale()->monthName(dlg->m_date.month(),true)+"-";
        tmp=tmp+QString().sprintf("%02d", dlg->m_date.day());
        }
else if(listFormat->currentItem()==17) /*2000-02-18*/
        {
        tmp=tmp2.setNum(dlg->m_date.year());
        tmp=tmp+"-"+QString().sprintf("%02d", dlg->m_date.month())+"-";
        tmp=tmp+QString().sprintf("%02d", dlg->m_date.day());
        }
exampleLabel->setText(tmp);
}


void CellLayoutPageFloat::init()
{
QStringList list;
QString tmp;
QString tmp2;
list+=i18n("System : ")+KGlobal::locale()->formatDate(QDate::currentDate(),true);
list+=i18n("System : ")+KGlobal::locale()->formatDate(QDate::currentDate(),false);
/*18-Feb-00*/
tmp=tmp2.setNum(18);
tmp+="-"+KGlobal::locale()->monthName(2, true)+"-";
tmp+=tmp2.setNum(2000).right(2);
list+=tmp;
/*18-Feb-1999*/
tmp=tmp2.setNum(18);
tmp+="-"+KGlobal::locale()->monthName(2, true)+"-";
tmp+=tmp2.setNum(2000);
list+=tmp;
/*18-Feb*/
tmp=tmp2.setNum(18);
tmp=tmp+"-"+KGlobal::locale()->monthName(2, true);
list+=tmp;
/*18-2*/
tmp=tmp2.setNum(18);
tmp=tmp+"-"+QString().sprintf("%02d", 2);
list+=tmp;
/*18/2/00*/
tmp=tmp2.setNum(18);
tmp=tmp+"/"+QString().sprintf("%02d", 2)+"/";
tmp=tmp+tmp2.setNum(2000).right(2);
list+=tmp;
/*18/5/1999*/
tmp=tmp2.setNum(18);
tmp=tmp+"/"+QString().sprintf("%02d", 2)+"/";
tmp=tmp+tmp2.setNum(2000);
list+=tmp;
/*Feb-99*/
tmp=KGlobal::locale()->monthName(2, true)+"-";
tmp=tmp+tmp2.setNum(2000).right(2);
list+=tmp;
/*February-99*/
tmp=KGlobal::locale()->monthName(2)+"-";
tmp=tmp+tmp2.setNum(2000).right(2);
list+=tmp;
/*February-1999*/
tmp=KGlobal::locale()->monthName(2)+"-";
tmp=tmp+tmp2.setNum(2000);
list+=tmp;
/*F-99*/
tmp=KGlobal::locale()->monthName(2).at(0)+"-";
tmp=tmp+tmp2.setNum(2000).right(2);
list+=tmp;
/*18/Feb*/
tmp=tmp2.setNum(18)+"/";
tmp+=KGlobal::locale()->monthName(2,true);
list+=tmp;
/*18/2*/
tmp=tmp2.setNum(18)+"/";
tmp+=QString().sprintf("%02d", 2);
list+=tmp;
/*18/Feb/1999*/
tmp=tmp2.setNum(18);
tmp=tmp+"/"+KGlobal::locale()->monthName(2,true)+"/";
tmp=tmp+tmp2.setNum(2000);
list+=tmp;
/*2000/Feb/18*/
tmp=tmp2.setNum(2000);
tmp=tmp+"/"+KGlobal::locale()->monthName(2,true)+"/";
tmp=tmp+tmp2.setNum(18);
list+=tmp;
/*2000-Feb-18*/
tmp=tmp2.setNum(2000);
tmp=tmp+"-"+KGlobal::locale()->monthName(2,true)+"-";
tmp=tmp+tmp2.setNum(18);
list+=tmp;
/*2000-2-18*/
tmp=tmp.setNum(2000);
tmp=tmp+"-"+QString().sprintf("%02d", 2)+"-";
tmp=tmp+tmp2.setNum(18);
list+=tmp;
listFormat->insertStringList(list);
if( dlg->formatNumber==KSpreadCell::ShortDate )
        listFormat->setCurrentItem(0);
else if(dlg->formatNumber==KSpreadCell::TextDate)
        listFormat->setCurrentItem(1);
else if(dlg->formatNumber==KSpreadCell::date_format1)
        listFormat->setCurrentItem(2);
else if(dlg->formatNumber==KSpreadCell::date_format2)
        listFormat->setCurrentItem(3);
else if(dlg->formatNumber==KSpreadCell::date_format3)
        listFormat->setCurrentItem(4);
else if(dlg->formatNumber==KSpreadCell::date_format4)
        listFormat->setCurrentItem(5);
else if(dlg->formatNumber==KSpreadCell::date_format5)
        listFormat->setCurrentItem(6);
else if(dlg->formatNumber==KSpreadCell::date_format6)
        listFormat->setCurrentItem(7);
else if(dlg->formatNumber==KSpreadCell::date_format7)
        listFormat->setCurrentItem(8);
else if(dlg->formatNumber==KSpreadCell::date_format8)
        listFormat->setCurrentItem(9);
else if(dlg->formatNumber==KSpreadCell::date_format9)
        listFormat->setCurrentItem(10);
else if(dlg->formatNumber==KSpreadCell::date_format10)
        listFormat->setCurrentItem(11);
else if(dlg->formatNumber==KSpreadCell::date_format11)
        listFormat->setCurrentItem(12);
else if(dlg->formatNumber==KSpreadCell::date_format12)
        listFormat->setCurrentItem(13);
else if(dlg->formatNumber==KSpreadCell::date_format13)
        listFormat->setCurrentItem(14);
else if(dlg->formatNumber==KSpreadCell::date_format14)
        listFormat->setCurrentItem(15);
else if(dlg->formatNumber==KSpreadCell::date_format15)
        listFormat->setCurrentItem(16);
else if(dlg->formatNumber==KSpreadCell::date_format16)
        listFormat->setCurrentItem(17);
else
        listFormat->setCurrentItem(0);

}
void CellLayoutPageFloat::makeformat()
{
QString tmp;
int p;
p = (precision->value() == -1) ? 8 : precision->value();
QChar decimal_point= KGlobal::locale()->decimalSymbol()[0];
if(!dlg->m_bValue&&!dlg->m_bDate&&!dlg->m_bTime)
        {
        QString tmpText;
        if( dlg->cellText.length()>50)
                tmpText=dlg->cellText.left(50);
        exampleLabel->setText(tmpText);
        }
else if(dlg->m_bDate)
        {
        if(date->isChecked())
                {
                makeDateFormat();
                }
        else
                exampleLabel->setText(dlg->cellText);
        }
else if(dlg->m_bTime)
        {
        if(time->isChecked())
                {
                QString tmp;
                QString tmpTime;
                if( listFormat->currentItem()==0)
                        exampleLabel->setText(KGlobal::locale()->formatTime(dlg->m_time,false));
                else if(listFormat->currentItem()==1)
                        exampleLabel->setText(KGlobal::locale()->formatTime(dlg->m_time,true));
                else if(listFormat->currentItem()==2)
                        {
                        if( dlg->m_time.hour()>12)
                                tmp=QString().sprintf("%02d", dlg->m_time.hour()-12)+":"+QString().sprintf("%02d",dlg->m_time.minute()) +" "+ i18n("PM");
                        else
                                tmp=QString().sprintf("%02d", dlg->m_time.hour())+":"+QString().sprintf("%02d",dlg->m_time.minute()) +" "+ i18n("AM");
                        exampleLabel->setText(tmp);
                        }
                else if(listFormat->currentItem()==3)
                        {

                        if( dlg->m_time.hour()>12)
                                tmp=QString().sprintf("%02d", dlg->m_time.hour()-12)+":"+QString().sprintf("%02d",dlg->m_time.minute()) +":"+QString().sprintf("%02d",dlg->m_time.second())+" "+ i18n("PM");
                        else
                                tmp=QString().sprintf("%02d", dlg->m_time.hour())+":"+QString().sprintf("%02d",dlg->m_time.minute()) +":"+QString().sprintf("%02d",dlg->m_time.second())+" "+ i18n("AM");
                        exampleLabel->setText(tmp);
                        }
                else if(listFormat->currentItem()==4)
                        {
                        tmp=QString().sprintf("%02d",dlg->m_time.hour())+" "+i18n("h")+" "+QString().sprintf("%02d",dlg->m_time.minute())+" "+i18n("min")+" "+QString().sprintf("%02d",dlg->m_time.second())  +" "+ i18n("s");
                        exampleLabel->setText(tmp);
                        }
                }
        else
                exampleLabel->setText(dlg->cellText);
        }
else if(dlg->m_bValue)
        {
        if(number->isChecked())
                tmp=KGlobal::locale()->formatNumber(dlg->m_value,p );
        else if(money->isChecked())
                tmp=KGlobal::locale()->formatMoney(dlg->m_value,KGlobal::locale()->currencySymbol(),p );
        else if(percent->isChecked())
                tmp=KGlobal::locale()->formatNumber(dlg->m_value*100.0, p)+ " %";
        else if(scientific->isChecked())
                {
                tmp=QString::number(dlg->m_value, 'E', p);
                int pos;
                if((pos=tmp.find('.'))!=-1)
                        tmp=tmp.replace(pos,1,decimal_point);
                }
        else if(fraction->isChecked())
                {
                double result = (dlg->m_value)-floor(dlg->m_value);

                if(result == 0 )
                {
                        tmp = tmp.setNum( dlg->m_value );
                }
                else
                {
                        int index = 0;
                        switch( listFormat->currentItem())
                        {
                        case 0:
                                index=2;
                                break;
                        case 1:
                                index=4;
                                break;
                        case 2:
                                index=8;
                                break;
                        case 3:
                                index=16;
                                break;
                        case 4:
                                index=10;
                                break;
                        case 5:
                                index=100;
                                break;
                        case 6:
                                index=3;
                                break;
                        case 7:
                                index=4;
                                break;
                        case 8:
                                index=5;
                                break;
                        }
                        if( listFormat->currentItem()!=6
                        &&listFormat->currentItem()!=7
                        &&listFormat->currentItem()!=8)
                                {
                                double calc = 0;
                                int index1 = 1;
                                double diff = result;
                                for(int i=1;i<index;i++)
                                {
                                        calc = i*1.0 / index;
                                        if( fabs( result - calc ) < diff )
                                                {
                                                index1=i;
                                                diff = fabs(result-calc);
                                                }
                                }
                                tmp = tmp.setNum( floor(dlg->m_value) ) + " " + tmp.setNum( index1 ) + "/" + tmp.setNum( index );
                        }
                        else
                        {
                        int limit=0;

                        double preci=0;
                        if(listFormat->currentItem()==6)
                                limit=9;
                        else if(listFormat->currentItem()==7)
                                limit=99;
                        else if(listFormat->currentItem()==8)
                                limit=999;
                        double denominator=0;
                        double numerator=0;
                        do
                        {
                        double val1=result;
                        double inter2=1;
                        double inter4=0;
                        double p=0;
                        double q=0;
                        double val2=rint(result);

                        preci=pow((float)10,(-1*index));
                        numerator=rint(result);
                        denominator=1;
                        while(fabs(numerator/denominator-result)>preci)
                                {
                                val1=(1/(val1-val2));
                                val2=rint(val1);
                                p= val2*numerator + inter2;
                                q= val2*denominator + inter4;
                                inter2=numerator;
                                inter4=denominator;
                                numerator=p;
                                denominator=q;
                                }
                        index--;
                        }
                        while (fabs(denominator)>limit) ;
                        if(fabs(denominator)==fabs(numerator))
                                tmp = tmp.setNum( floor(dlg->m_value+1) );
                        else
                                tmp = tmp.setNum( floor(dlg->m_value) ) + " " + tmp.setNum( fabs(numerator) ) + "/" + tmp.setNum( fabs(denominator) );

                        }
                }
                }
        if ( precision->value() == -1 && tmp.find(decimal_point) >= 0 )
        {
            int start=0;
            if(tmp.find('%')!=-1)
                start=2;
            else if(tmp.find(KGlobal::locale()->currencySymbol())==(tmp.length()-KGlobal::locale()->currencySymbol().length()))
                start=KGlobal::locale()->currencySymbol().length()+1;
            else if((start=tmp.find('E'))!=-1)
                start=tmp.length()-start;
            else
                start=0;
            int i = tmp.length()-start;
            bool bFinished = FALSE;


            while ( !bFinished && i > 0 )
            {
                QChar ch = tmp[ i - 1 ];
                if ( ch == '0' )
                    tmp.remove(--i,1);
                else
                {
                    bFinished = TRUE;
                    if ( ch == decimal_point )
                        tmp.remove(--i,1);
                }
            }

        }
        if(dlg->m_bValue && !time->isChecked() && !date->isChecked())
        {
                if ( prefix->text() != "########" )
                        tmp=prefix->text()+" "+tmp;
                if ( postfix->text() != "########" )
                        tmp+=" "+postfix->text();
        }
        exampleLabel->setText(tmp);
        }
        else
                exampleLabel->setText("Error");
}
void CellLayoutPageFloat::apply( KSpreadCell *_obj )
{
    if ( postfix->text() != dlg->postfix )
        if ( postfix->text() != "########" && postfix->isEnabled())
        {
            // If we are in here it *never* can be disabled - FIXME (Werner)!
            if( postfix->isEnabled())
                _obj->setPostfix( postfix->text() );
            else
                _obj->setPostfix( "" );
        }
    if ( prefix->text() != dlg->prefix )
        if ( prefix->text() != "########" )
        {
            if(prefix->isEnabled())
                _obj->setPrefix( prefix->text() );
            else
                _obj->setPrefix( "" );

        }

    if ( dlg->precision != precision->value() )
        _obj->setPrecision( precision->value() );

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
    _obj->setFaktor(1.0);
    if(number->isChecked())
        _obj->setFormatNumber(KSpreadCell::Number);
    else if(percent->isChecked())
        {
        _obj->setFormatNumber(KSpreadCell::Percentage);
        _obj->setFaktor(100.0);
        }
    else if(fraction->isChecked())
        {
        if( listFormat->currentItem()==0)
                _obj->setFormatNumber(KSpreadCell::fraction_half);
        else if( listFormat->currentItem()==1)
                _obj->setFormatNumber(KSpreadCell::fraction_quarter);
        else if( listFormat->currentItem()==2)
                _obj->setFormatNumber(KSpreadCell::fraction_eighth);
        else if( listFormat->currentItem()==3)
                _obj->setFormatNumber(KSpreadCell::fraction_sixteenth);
        else if( listFormat->currentItem()==4)
                _obj->setFormatNumber(KSpreadCell::fraction_tenth);
        else if( listFormat->currentItem()==5)
                _obj->setFormatNumber(KSpreadCell::fraction_hundredth);
        else if( listFormat->currentItem()==6)
                _obj->setFormatNumber(KSpreadCell::fraction_one_digit);
        else if( listFormat->currentItem()==7)
                _obj->setFormatNumber(KSpreadCell::fraction_two_digits);
        else if( listFormat->currentItem()==8)
                _obj->setFormatNumber(KSpreadCell::fraction_three_digits);
        }
    else if(date->isChecked())
        {
        if( listFormat->currentItem()==0)
                _obj->setFormatNumber(KSpreadCell::ShortDate );
        else if(listFormat->currentItem()==1)
                _obj->setFormatNumber(KSpreadCell::TextDate );
        else if(listFormat->currentItem()==2)
                _obj->setFormatNumber(KSpreadCell::date_format1 );
        else if(listFormat->currentItem()==3)
                _obj->setFormatNumber(KSpreadCell::date_format2 );
        else if(listFormat->currentItem()==4)
                _obj->setFormatNumber(KSpreadCell::date_format3 );
        else if(listFormat->currentItem()==5)
                _obj->setFormatNumber(KSpreadCell::date_format4 );
        else if(listFormat->currentItem()==6)
                _obj->setFormatNumber(KSpreadCell::date_format5 );
        else if(listFormat->currentItem()==7)
                _obj->setFormatNumber(KSpreadCell::date_format6 );
        else if(listFormat->currentItem()==8)
                _obj->setFormatNumber(KSpreadCell::date_format7 );
        else if(listFormat->currentItem()==9)
                _obj->setFormatNumber(KSpreadCell::date_format8 );
        else if(listFormat->currentItem()==10)
                _obj->setFormatNumber(KSpreadCell::date_format9 );
        else if(listFormat->currentItem()==11)
                _obj->setFormatNumber(KSpreadCell::date_format10 );
        else if(listFormat->currentItem()==12)
                _obj->setFormatNumber(KSpreadCell::date_format11 );
        else if(listFormat->currentItem()==13)
                _obj->setFormatNumber(KSpreadCell::date_format12 );
        else if(listFormat->currentItem()==14)
                _obj->setFormatNumber(KSpreadCell::date_format13 );
        else if(listFormat->currentItem()==15)
                _obj->setFormatNumber(KSpreadCell::date_format14 );
        else if(listFormat->currentItem()==16)
                _obj->setFormatNumber(KSpreadCell::date_format15 );
        else if(listFormat->currentItem()==17)
                _obj->setFormatNumber(KSpreadCell::date_format16 );
        }
    else if(time->isChecked())
        {
        if( listFormat->currentItem()==0)
                _obj->setFormatNumber(KSpreadCell::Time );
        else if(listFormat->currentItem()==1)
                _obj->setFormatNumber(KSpreadCell::SecondeTime );
        else if(listFormat->currentItem()==2)
                _obj->setFormatNumber(KSpreadCell::Time_format1 );
        else if(listFormat->currentItem()==3)
                _obj->setFormatNumber(KSpreadCell::Time_format2 );
        else if(listFormat->currentItem()==4)
                _obj->setFormatNumber(KSpreadCell::Time_format3 );
        }
    else if(money->isChecked())
        _obj->setFormatNumber(KSpreadCell::Money);
    else if(scientific->isChecked())
        _obj->setFormatNumber(KSpreadCell::Scientific);

}


CellLayoutPageMisc::CellLayoutPageMisc( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget( parent )
{
    dlg = _dlg;


    QGridLayout* layout = new QGridLayout( this, 2,2,7,7 );
    QGroupBox *box = new QGroupBox( this, "Box");

    QGridLayout *grid = new QGridLayout(box,2,4,7,7);
    QLabel *tmpQLabel;

    tmpQLabel = new QLabel( box, "Label_3" );
    grid->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText( i18n("Functionality") );

    styleButton = new QComboBox( box, "ComboBox_2" );

    grid->addWidget(styleButton,1,0);

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

    tmpQLabel = new QLabel( box, "Label_3" );
    grid->addWidget(tmpQLabel,2,0);
    tmpQLabel->setText( i18n("Action") );

    actionText = new QLineEdit( box );
    grid->addMultiCellWidget(actionText,3,3,0,1);

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

    layout->addWidget(box,0,0);

    this->resize( 400, 400 );
}

void CellLayoutPageMisc::apply( KSpreadCell *_obj )
{
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


CellLayoutPageFont::CellLayoutPageFont( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget ( parent )
{
  dlg = _dlg;

  bTextColorUndefined = !dlg->bTextColor;

  QVBoxLayout* grid = new QVBoxLayout( this, 6 );

  box1 = new QGroupBox( this, "Box1");
  box1->setTitle(i18n("Requested Font"));
  QGridLayout *grid2 = new QGridLayout(box1,6,3,15,7);
  family_label = new QLabel(box1,"family");
  family_label->setText(i18n("Family:"));
  grid2->addWidget(family_label,0,0);

  size_label = new QLabel(box1,"size");
  size_label->setText(i18n("Size:"));
  grid2->addWidget(size_label,0,2);

  weight_label = new QLabel(box1,"weight");
  weight_label->setText(i18n("Weight:"));
  grid2->addWidget(weight_label,2,1);

  QLabel *tmpQLabel = new QLabel( box1, "Label_1" );
  tmpQLabel->setText( i18n("Text Color") );
  grid2->addWidget(tmpQLabel,4,1);

  textColorButton = new KColorButton( box1, "textColor" );
  grid2->addWidget(textColorButton,5,1);

  connect( textColorButton, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotSetTextColor( const QColor & ) ) );



  style_label = new QLabel(box1,"style");
  style_label->setText(i18n("Style:"));
  grid2->addWidget(style_label,0,1);

  family_combo = new QListBox( box1, "Family" );

  QStringList tmpListFont;
  QFontDatabase *fontDataBase = new QFontDatabase();
  tmpListFont = fontDataBase->families();

  listFont.setItems(tmpListFont);

  family_combo->insertStringList( tmpListFont);
  selFont = dlg->textFont;

   if ( dlg->bTextFontFamily )
   {
        selFont.setFamily( dlg->textFontFamily );
        kdDebug(36001) << "Family = " << dlg->textFontFamily << endl;

        if( !family_combo->findItem(dlg->textFontFamily))
                {
                family_combo->insertItem("",0);
                family_combo->setCurrentItem(0);
                }
        else
                family_combo->setCurrentItem(family_combo->index(family_combo->findItem(dlg->textFontFamily)));
   }
   else
   {
        family_combo->insertItem("",0);
        family_combo->setCurrentItem(0);
   }

  grid2->addMultiCellWidget(family_combo,2,5,0,0);

  connect( family_combo, SIGNAL(highlighted(const QString &)),
           SLOT(family_chosen_slot(const QString &)) );

  searchFont=new KLineEdit(box1);
  grid2->addWidget(searchFont,1,0);
  searchFont->setCompletionMode(KGlobalSettings::CompletionAuto  );
  searchFont->setCompletionObject( &listFont,true );

  connect(searchFont, SIGNAL( textChanged( const QString & )),
          this,SLOT(slotSearchFont(const QString &)));

  size_combo = new QComboBox( true, box1, "Size" );
  QStringList lst;
  lst.append("");
  for ( unsigned int i = 1; i < 100; ++i )
        lst.append( QString( "%1" ).arg( i ) );

  size_combo->insertStringList( lst );


  size_combo->setInsertionPolicy(QComboBox::NoInsertion);
  grid2->addWidget(size_combo,1,2);
  connect( size_combo, SIGNAL(activated(const QString &)),
           SLOT(size_chosen_slot(const QString &)) );
  connect( size_combo ,SIGNAL( textChanged(const QString &)),
        this,SLOT(size_chosen_slot(const QString &)));

  weight_combo = new QComboBox( box1, "Weight" );
  weight_combo->insertItem( "", 0 );
  weight_combo->insertItem( i18n("normal") );
  weight_combo->insertItem( i18n("bold") );

  weight_combo->setInsertionPolicy(QComboBox::NoInsertion);
  grid2->addWidget(weight_combo,3,1);
  connect( weight_combo, SIGNAL(activated(const QString &)),
           SLOT(weight_chosen_slot(const QString &)) );

  style_combo = new QComboBox( box1, "Style" );
  style_combo->insertItem( "", 0 );
  style_combo->insertItem( i18n("roman") );
  style_combo->insertItem( i18n("italic"), 2 );
  grid2->addWidget(style_combo,1,1);
  style_combo->setInsertionPolicy(QComboBox::NoInsertion);
  connect( style_combo, SIGNAL(activated(const QString &)),
           SLOT(style_chosen_slot(const QString &)) );

  strike = new QCheckBox(i18n("Strike out"),box1);
  grid2->addWidget(strike,5,2);
  strike->setChecked(dlg->strike);
  connect( strike, SIGNAL( clicked()),
           SLOT(strike_chosen_slot()) );

  underline = new QCheckBox(i18n("Underline"),box1);
  grid2->addWidget(underline,3,2);
  underline->setChecked(dlg->underline);
  connect( underline, SIGNAL( clicked()),
           SLOT(underline_chosen_slot()) );


  grid->addWidget(box1);

  box1 = new QGroupBox(this, "Box2");
  box1->setTitle(i18n("Actual Font"));
  grid2 = new QGridLayout(box1,2,4,15,7);

  actual_family_label = new QLabel(box1,"afamily");
  actual_family_label->setText(i18n("Family:"));
  grid2->addWidget(actual_family_label,0,0);

  actual_family_label_data = new QLabel(box1,"afamilyd");
  grid2->addWidget(actual_family_label_data,0,1);

  actual_size_label = new QLabel(box1,"asize");
  actual_size_label->setText(i18n("Size:"));
  grid2->addWidget(actual_size_label,1,0);

  actual_size_label_data = new QLabel(box1,"asized");
  grid2->addWidget(actual_size_label_data,1,1);

  actual_weight_label = new QLabel(box1,"aweight");
  actual_weight_label->setText(i18n("Weight:"));
  grid2->addWidget(actual_weight_label,2,0);

  actual_weight_label_data = new QLabel(box1,"aweightd");
  grid2->addWidget(actual_weight_label_data,2,1);

  actual_style_label = new QLabel(box1,"astyle");
  actual_style_label->setText(i18n("Style:"));
  grid2->addWidget(actual_style_label,3,0);

  actual_style_label_data = new QLabel(box1,"astyled");
  grid2->addWidget(actual_style_label_data,3,1);


  example_label = new QLabel(box1,"examples");
  example_label->setFont(selFont);
  example_label->setAlignment(AlignCenter);
  example_label->setBackgroundColor(colorGroup().base());
  example_label->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
  example_label->setLineWidth( 1 );
  example_label->setText(i18n("Dolor Ipse"));
  //  example_label->setAutoResize(true);
  grid2->addMultiCellWidget(example_label,0,3,2,3);
  connect(this,SIGNAL(fontSelected( const QFont&  )),
          this,SLOT(display_example( const QFont&)));


  grid->addWidget(box1);
  setCombos();
  display_example( selFont );

  this->resize( 400, 400 );
}

void CellLayoutPageFont::slotSearchFont(const QString &_text)
{
QString result;
result=listFont.makeCompletion( _text );
if(!result.isNull())
        family_combo->setCurrentItem(family_combo->index(family_combo->findItem(result)));
}


void CellLayoutPageFont::slotSetTextColor( const QColor &_color )
{
textColor=_color;
}

void CellLayoutPageFont::apply( KSpreadCell *_obj )
{
    if ( !bTextColorUndefined )
        _obj->setTextColor( textColor );
    if ( size_combo->currentItem() != 0 )
        _obj->setTextFontSize( selFont.pointSize() );
    if ( !family_combo->currentText().isEmpty() )
        _obj->setTextFontFamily( selFont.family() );
    if ( weight_combo->currentItem() != 0 )
        _obj->setTextFontBold( selFont.bold() );
    if ( style_combo->currentItem() != 0 )
        _obj->setTextFontItalic( selFont.italic() );
    _obj->setTextFontStrike( strike->isChecked() );
    _obj->setTextFontUnderline(underline->isChecked() );
}

void CellLayoutPageFont::underline_chosen_slot()
{
   selFont.setUnderline( underline->isChecked() );
   emit fontSelected(selFont);
}

void CellLayoutPageFont::strike_chosen_slot()
{
   selFont.setStrikeOut( strike->isChecked() );
   emit fontSelected(selFont);
}

void CellLayoutPageFont::family_chosen_slot(const QString & family)
{
  selFont.setFamily(family);
  emit fontSelected(selFont);
}

void CellLayoutPageFont::size_chosen_slot(const QString & size)
{
  QString size_string = size;

  selFont.setPointSize(size_string.toInt());
  emit fontSelected(selFont);
}

void CellLayoutPageFont::weight_chosen_slot(const QString & weight)
{
  QString weight_string = weight;

  if ( weight_string == QString(i18n("normal")))
    selFont.setBold(false);
  if ( weight_string == QString(i18n("bold")))
       selFont.setBold(true);
  emit fontSelected(selFont);
}

void CellLayoutPageFont::style_chosen_slot(const QString & style)
{
  QString style_string = style;

  if ( style_string == QString(i18n("roman")))
    selFont.setItalic(false);
  if ( style_string == QString(i18n("italic")))
    selFont.setItalic(true);
  emit fontSelected(selFont);
}


void CellLayoutPageFont::display_example(const QFont& font)
{
  QString string;

  example_label->setFont(font);
  example_label->repaint();

  kdDebug(36001) << "FAMILY 2 '" << font.family() << "' " << font.pointSize() << endl;

  QFontInfo info = example_label->fontInfo();
  actual_family_label_data->setText(info.family());

  kdDebug(36001) << "FAMILY 3 '" << info.family() << "' " << info.pointSize() << endl;

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

 if ( dlg->bTextColor )
        textColor = dlg->textColor;
 else
        textColor = colorGroup().text();
 textColorButton->setColor( textColor );

 // Needed to initialize this font
 /*if ( dlg->bTextFontFamily )
 {
     selFont.setFamily( dlg->textFontFamily );
     kdDebug(36001) << "Family = " << dlg->textFontFamily.data() << endl;
     number_of_entries =  family_combo->count();
     string = dlg->textFontFamily;
     found = false;

     for (int i = 1; i < number_of_entries - 1; i++)
     {
         if ( string == (QString) family->text(i))
         {

             family->setCurrentItem(i);
             //     kdDebug(36001) << "Found Font " << string.data() << endl;
             found = true;
             break;
         }
     }
 }
 else
     family->setCurrentItem(0);
 */

 combo = size_combo;
 if ( dlg->bTextFontSize )
 {
     kdDebug(36001) << "SIZE=" << dlg->textFontSize << endl;
     selFont.setPointSize( dlg->textFontSize );
     number_of_entries = size_combo->count();
     string.setNum( dlg->textFontSize );
     found = false;

     for (int i = 0; i < number_of_entries ; i++){
         if ( string == (QString) combo->text(i)){
             combo->setCurrentItem(i);
             found = true;
             // kdDebug(36001) << "Found Size " << string.data() << " setting to item " i << endl;
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

    QGridLayout *grid3 = new QGridLayout(this,4,2,15,7);
    QButtonGroup *grp = new QButtonGroup( i18n("Horizontal"),this);
    grp->setRadioButtonExclusive( TRUE );

    QGridLayout *grid2 = new QGridLayout(grp,3,2,15,7);
    standard = new QRadioButton( i18n("Standard"), grp );
    grid2->addWidget(standard,1,0);
    left = new QRadioButton( i18n("Left"), grp );
    grid2->addWidget(left,0,1);
    center = new QRadioButton( i18n("Center"), grp );
    grid2->addWidget(center,1,1);
    right = new QRadioButton( i18n("Right"), grp );
    grid2->addWidget(right,2,1);
    grid3->addWidget(grp,0,0);

    if(dlg->alignX==KSpreadCell::Left)
        left->setChecked(true);
    else if(dlg->alignX==KSpreadCell::Center)
        center->setChecked(true);
    else if(dlg->alignX==KSpreadCell::Right)
        right->setChecked(true);
    else if( dlg->alignX==KSpreadCell::Undefined)
        standard->setChecked(true);

    connect(grp, SIGNAL(clicked(int)),this,SLOT(slotStateChanged(int)));

    grp = new QButtonGroup( i18n("Vertical"),this);
    grp->setRadioButtonExclusive( TRUE );

    grid2 = new QGridLayout(grp,3,1,15,7);
    top = new QRadioButton( i18n("Top"), grp );
    grid2->addWidget(top,0,0);
    middle = new QRadioButton( i18n("Middle"), grp );
    grid2->addWidget(middle,1,0);
    bottom = new QRadioButton( i18n("Bottom"), grp );
    grid2->addWidget(bottom,2,0);
    grid3->addWidget(grp,0,1);

    if(dlg->alignY==KSpreadCell::Top)
        top->setChecked(true);
    else if(dlg->alignY==KSpreadCell::Middle)
        middle->setChecked(true);
    else if(dlg->alignY==KSpreadCell::Bottom)
        bottom->setChecked(true);

    grp = new QButtonGroup( i18n("Text option"),this);
    //grp->setRadioButtonExclusive( false );
    grid2 = new QGridLayout(grp,2,1,15,7);
    multi = new QCheckBox( i18n("Goto line automatically"),grp);

    grid2->addWidget(multi,0,0);
    multi->setChecked(dlg->bMultiRow);

    vertical = new QCheckBox( i18n("Vertical text"),grp);
    grid2->addWidget(vertical,1,0);
    vertical->setChecked(dlg->bVerticalText);

    grid3->addWidget(grp,1,0);

    grp = new QButtonGroup( i18n("Rotation"),this);

    grid2 = new QGridLayout(grp,1,1,15,7);
    angleRotation=new KIntNumInput(dlg->textRotation, grp, 10);
    angleRotation->setLabel(i18n("Angle :"));
    angleRotation->setRange(-90, 90, 1);
    angleRotation->setSuffix(" °");

    grid2->addWidget(angleRotation,0,0);
    grid3->addWidget(grp,1,1);
    if(dlg->textRotation!=0)
        multi->setEnabled(false);

    grp = new QButtonGroup( i18n("Merge cells"),this);
    grid2 = new QGridLayout(grp,1,1,15,7);
    mergeCell=new QCheckBox(i18n("Merge cells"),grp);
    mergeCell->setChecked(dlg->isMerged);
    mergeCell->setEnabled(!dlg->oneCell);
    grid2->addWidget(mergeCell,0,0);
    grid3->addWidget(grp,2,0);

    grp = new QButtonGroup( i18n("Indent"),this);
    grid2 = new QGridLayout(grp,1,1,15,7);
    indent=new KIntNumInput(dlg->indent, grp, 10);
    indent->setRange(0, 400, 10);
    indent->setSuffix(i18n(" pt"));
    grid2->addWidget(indent,0,0);
    grid3->addWidget(grp,2,1);

    grp = new QButtonGroup( i18n("Size of cell"),this);
    grid2 = new QGridLayout(grp,2,4,15,7);

    QLabel *tmpLabel=new QLabel(grp,"label");
    tmpLabel->setText(i18n("Width :"));
    grid2->addWidget(tmpLabel,0,0);

    width=new KIntNumInput(dlg->widthSize, grp, 10);
    width->setRange(20, 400, 1);
    grid2->addWidget(width,0,1);
    defaultWidth=new QCheckBox(i18n("Default width (60)"),grp);
    grid2->addMultiCellWidget(defaultWidth,1,1,0,1);

    tmpLabel=new QLabel(grp,"label1");
    tmpLabel->setText(i18n("Height  :"));
    grid2->addWidget(tmpLabel,0,2);

    height=new KIntNumInput(dlg->heigthSize, grp, 10);
    height->setRange(20, 400, 1);
    grid2->addWidget(height,0,3);

    defaultHeight=new QCheckBox(i18n("Default height (20)"),grp);
    grid2->addMultiCellWidget(defaultHeight,1,1,2,3);

    grid3->addMultiCellWidget(grp,3,3,0,1);

    connect(defaultWidth , SIGNAL(clicked() ),this, SLOT(slotChangeWidthState()));
    connect(defaultHeight , SIGNAL(clicked() ),this, SLOT(slotChangeHeightState()));
    connect(vertical , SIGNAL(clicked() ),this, SLOT(slotChangeVerticalState()));
    connect(multi , SIGNAL(clicked() ),this, SLOT(slotChangeMultiState()));
    connect(angleRotation, SIGNAL(valueChanged(int)),this,SLOT(slotChangeAngle(int)));
    slotStateChanged(0);
    this->resize( 400, 400 );
}

void CellLayoutPagePosition::slotChangeMultiState()
{
if(vertical->isChecked())
        {
        vertical->setChecked(false);
        }
}

void CellLayoutPagePosition::slotChangeVerticalState()
{
if(multi->isChecked())
        {
        multi->setChecked(false);
        }

}

void CellLayoutPagePosition::slotStateChanged(int)
{
if(right->isChecked() || center->isChecked())
        indent->setEnabled(false);
else
        indent->setEnabled(true);

}
bool CellLayoutPagePosition::getMergedCellState()
{
    return  mergeCell->isChecked();
}

void CellLayoutPagePosition::slotChangeWidthState()
{
    if( defaultWidth->isChecked())
        width->setEnabled(false);
    else
        width->setEnabled(true);
}

void CellLayoutPagePosition::slotChangeHeightState()
{
    if( defaultHeight->isChecked())
        height->setEnabled(false);
    else
        height->setEnabled(true);
}

void CellLayoutPagePosition::slotChangeAngle(int _angle)
{
if(_angle==0)
    {
    multi->setEnabled(true);
    vertical->setEnabled(true);
    }
else
    {
    multi->setEnabled(false);
    vertical->setEnabled(false);
    }
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
  else if( standard->isChecked())
    _obj->setAlign(KSpreadCell::Undefined);
  if(multi->isEnabled())
        _obj->setMultiRow(multi->isChecked());
  else
        _obj->setMultiRow(false);

  if(vertical->isEnabled())
        _obj->setVerticalText(vertical->isChecked());
  else
        _obj->setVerticalText(false);

  _obj->setAngle(angleRotation->value());
  if(dlg->indent!=indent->value() && indent->isEnabled())
        _obj->setIndent(indent->value());
}

int CellLayoutPagePosition::getSizeHeight()
{
  if(defaultHeight->isChecked())
        return 20;
  else
        return height->value();
}

int CellLayoutPagePosition::getSizeWidth()
{
  if(defaultWidth->isChecked())
        return 60;
  else
        return width->value();
}

KSpreadBorderButton::KSpreadBorderButton( QWidget *parent, const char *_name ) : QPushButton(parent,_name)
{
  penStyle = Qt::NoPen;
  penWidth = 1;
  penColor = colorGroup().text();
  setToggleButton( TRUE );
  setOn( false);
  setChanged(false);
}
void KSpreadBorderButton::mousePressEvent( QMouseEvent * )
{

  this->setOn(!isOn());
  emit clicked( this );
}

void KSpreadBorderButton::setUndefined()
{
 setPenStyle(SolidLine );
 setPenWidth(1);
 setColor(colorGroup().midlight());
}


void KSpreadBorderButton::unselect()
{
  setOn(false);
  setPenWidth(1);
  setPenStyle(Qt::NoPen);
  setColor( colorGroup().text() );
  setChanged(true);
}

KSpreadBorder::KSpreadBorder( QWidget *parent, const char *_name,bool _oneCol, bool _oneRow )
    : QFrame( parent, _name )
{
  oneCol=_oneCol;
  oneRow=_oneRow;
}


#define OFFSETX 5
#define OFFSETY 5
void KSpreadBorder::paintEvent( QPaintEvent *_ev )
{
  QFrame::paintEvent( _ev );
  QPen pen;
  QPainter painter;
  painter.begin( this );

  pen.setColor( colorGroup().midlight() );
  pen.setStyle( SolidLine );
  pen.setWidth( 2 );
  painter.setPen( pen );

  painter.drawLine( OFFSETX-5, OFFSETY, OFFSETX , OFFSETY );
  painter.drawLine( OFFSETX, OFFSETY-5, OFFSETX , OFFSETY );
  painter.drawLine( width()-OFFSETX, OFFSETY, width() , OFFSETY );
  painter.drawLine( width()-OFFSETX, OFFSETY-5, width()-OFFSETX , OFFSETY );

  painter.drawLine( OFFSETX, height()-OFFSETY, OFFSETX , height() );
  painter.drawLine( OFFSETX-5, height()-OFFSETY, OFFSETX , height()-OFFSETY );

  painter.drawLine( width()-OFFSETX, height()-OFFSETY, width() , height()-OFFSETY );
  painter.drawLine( width()-OFFSETX, height()-OFFSETY, width()-OFFSETX , height() );
  if(oneCol==false)
  {
        painter.drawLine( width()/2, OFFSETY-5, width()/2 , OFFSETY );
        painter.drawLine( width()/2-5, OFFSETY, width()/2+5 , OFFSETY );
        painter.drawLine( width()/2, height()-OFFSETY, width()/2 , height() );
        painter.drawLine( width()/2-5, height()-OFFSETY, width()/2+5 , height()-OFFSETY );
  }
  if(oneRow==false)
  {
        painter.drawLine( OFFSETX-5, height()/2, OFFSETX , height()/2 );
        painter.drawLine( OFFSETX, height()/2-5, OFFSETX , height()/2+5 );
        painter.drawLine( width()-OFFSETX, height()/2, width(), height()/2 );
        painter.drawLine( width()-OFFSETX, height()/2-5, width()-OFFSETX , height()/2+5 );
  }
  painter.end();
  emit redraw();
}

void KSpreadBorder::mousePressEvent( QMouseEvent* _ev )
{
  emit choosearea(_ev);
}

CellLayoutPageBorder::CellLayoutPageBorder( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget( parent )
{
  dlg = _dlg;

  QGridLayout *grid = new QGridLayout(this,5,2,15,15);

  QGroupBox* tmpQGroupBox;
  tmpQGroupBox = new QGroupBox( this, "GroupBox_1" );
  tmpQGroupBox->setFrameStyle( 49 );
  tmpQGroupBox->setTitle( i18n("Border") );
  tmpQGroupBox->setAlignment( 1 );

  QGridLayout *grid2 = new QGridLayout(tmpQGroupBox,5,5,15,7);

  area=new KSpreadBorder(tmpQGroupBox,"area",dlg->oneCol,dlg->oneRow);
  grid2->addMultiCellWidget(area,1,3,1,3);
  area->setBackgroundColor( colorGroup().base() );

  top=new KSpreadBorderButton(tmpQGroupBox,"top");
  loadIcon("border_top",top);
  grid2->addWidget(top,0,2);

  bottom=new KSpreadBorderButton(tmpQGroupBox,"bottom");
  loadIcon("border_bottom",bottom);
  grid2->addWidget(bottom,4,2);

  left=new KSpreadBorderButton(tmpQGroupBox,"left");
  loadIcon("border_left",left);
  grid2->addWidget(left,2,0);

  right=new KSpreadBorderButton(tmpQGroupBox,"right");
  loadIcon("border_right",right);
  grid2->addWidget(right,2,4);

  fallDiagonal=new KSpreadBorderButton(tmpQGroupBox,"fall");
  loadIcon("border_fall",fallDiagonal);
  grid2->addWidget(fallDiagonal,0,0);

  goUpDiagonal=new KSpreadBorderButton(tmpQGroupBox,"go");
  loadIcon("border_up",goUpDiagonal);
  grid2->addWidget(goUpDiagonal,0,4);

  vertical=new KSpreadBorderButton(tmpQGroupBox,"vertical");
  loadIcon("border_vertical",vertical);
  grid2->addWidget(vertical,4,4);

  horizontal=new KSpreadBorderButton(tmpQGroupBox,"horizontal");
  loadIcon("border_horizontal",horizontal);
  grid2->addWidget(horizontal,4,0);

  grid->addMultiCellWidget(tmpQGroupBox,0,2,0,0);


  tmpQGroupBox = new QGroupBox( this, "GroupBox_3" );
  tmpQGroupBox->setFrameStyle( 49 );
  tmpQGroupBox->setTitle( i18n("Preselect") );
  tmpQGroupBox->setAlignment( 1 );

  grid2 = new QGridLayout(tmpQGroupBox,1,3,15,7);

  outline=new KSpreadBorderButton(tmpQGroupBox,"outline");
  loadIcon("border_outline",outline);
  grid2->addWidget(outline,0,2);

  remove=new KSpreadBorderButton(tmpQGroupBox,"remove");
  loadIcon("border_remove",remove);
  grid2->addWidget(remove,0,0);

  all=new KSpreadBorderButton(tmpQGroupBox,"all");
  grid2->addWidget(all,0,1);

  if((dlg->oneRow==true)&&(dlg->oneCol==false))
        {
        loadIcon("border_vertical",all);
        }
  else if((dlg->oneRow==false)&&(dlg->oneCol==true))
        {
        loadIcon("border_horizontal",all);
        }
  else if((dlg->oneRow==false)&&(dlg->oneCol==false))
        {
         loadIcon("border_inside",all);
        }
  else
        {
        loadIcon("border_inside",all);
        all->setEnabled(false);
        }
  grid->addMultiCellWidget(tmpQGroupBox,3,4,0,0);

  tmpQGroupBox = new QGroupBox( this, "GroupBox_10" );
  tmpQGroupBox->setFrameStyle( 49 );
  tmpQGroupBox->setTitle( i18n("Pattern") );
  tmpQGroupBox->setAlignment( 1 );

  grid2 = new QGridLayout(tmpQGroupBox,6,2,15,7);

  pattern1 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_1" );
  pattern1->setFrameStyle( 50 );
  grid2->addWidget(pattern1,0,0);

  pattern2 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_2" );
  pattern2->setFrameStyle( 50 );
  grid2->addWidget(pattern2,1,0);

  pattern3 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_3" );
  pattern3->setFrameStyle( 50 );
  grid2->addWidget(pattern3,2,0);

  pattern4 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_4" );
  pattern4->setFrameStyle( 50 );
  grid2->addWidget(pattern4,0,1);

  pattern5 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_5" );
  pattern5->setFrameStyle( 50 );
  grid2->addWidget(pattern5,1,1);

  pattern6 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_6" );
  pattern6->setFrameStyle( 50 );
  grid2->addWidget(pattern6,2,1);

  pattern7 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_7" );
  pattern7->setFrameStyle( 50 );
  grid2->addWidget(pattern7,3,1);

  pattern8 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_8" );
  pattern8->setFrameStyle( 50 );
  grid2->addWidget(pattern8,4,1);

  pattern9 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_9" );
  pattern9->setFrameStyle( 50 );
  grid2->addWidget(pattern9,3,0);

  pattern10 = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_10" );
  pattern10->setFrameStyle( 50 );
  grid2->addWidget(pattern10,4,0);



    color = new KColorButton (tmpQGroupBox, "PushButton_1" );
    grid2->addWidget(color,6,1);

    QLabel *tmpQLabel = new QLabel( tmpQGroupBox, "Label_6" );
    tmpQLabel->setText( i18n("Color") );
    grid2->addWidget(tmpQLabel,6,0);


    QGridLayout *grid3 = new QGridLayout(tmpQGroupBox,2,2,0,7);
    customize  = new QCheckBox(i18n("Customize"),tmpQGroupBox);
    grid3->addWidget(customize,0,0);
    connect( customize, SIGNAL( clicked()),
           SLOT(cutomize_chosen_slot()) );

    size=new QComboBox(true,tmpQGroupBox);
    grid3->addWidget(size,1,1);
    size->setValidator(new KIntValidator( size ));
    QString tmp;
    for(int i=0;i<10;i++)
        {
        tmp=tmp.setNum(i);
        size->insertItem(tmp);
        }
    size->setCurrentItem(1);

    style=new QComboBox(tmpQGroupBox);
    grid3->addWidget(style,1,0);
    style->insertItem(paintFormatPixmap(DotLine),0 );
    style->insertItem(paintFormatPixmap(DashLine) ,1);
    style->insertItem(paintFormatPixmap(DashDotLine),2 );
    style->insertItem(paintFormatPixmap(DashDotDotLine),3  );
    style->insertItem(paintFormatPixmap(SolidLine),4);
    style->setBackgroundColor( colorGroup().background() );

    grid2->addMultiCell(grid3,5,5,0,1);
    grid->addMultiCellWidget(tmpQGroupBox,0,3,1,1);


    tmpQGroupBox = new QGroupBox(this, "GroupBox_4" );
    tmpQGroupBox->setFrameStyle( 49 );
    tmpQGroupBox->setTitle( i18n("Preview") );
    tmpQGroupBox->setAlignment( 1 );

    grid2 = new QGridLayout(tmpQGroupBox,1,1,14,7);



    preview = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_preview" );
    preview->setFrameStyle( 50 );
    grid2->addWidget(preview,0,0);

    grid->addWidget(tmpQGroupBox,4,1);

 if(dlg->leftBorderStyle != Qt::NoPen || !dlg->bLeftBorderStyle )
    {
    if ( dlg->bLeftBorderColor && dlg->bLeftBorderStyle  )
      {
        left->setPenStyle(dlg->leftBorderStyle );
        left->setPenWidth(dlg->leftBorderWidth);
        left->setColor(dlg->leftBorderColor);
        left->setOn(true);
      }
    else
      {

        left->setUndefined();
      }
 }
 if(dlg->rightBorderStyle!=Qt::NoPen|| !dlg->bRightBorderStyle)
    {
    if ( dlg->bRightBorderColor && dlg->bRightBorderStyle && dlg->rightBorderStyle!=NoPen)
      {
        right->setPenStyle(dlg->rightBorderStyle );
        right->setPenWidth(dlg->rightBorderWidth);
        right->setColor(dlg->rightBorderColor);
        right->setOn(true);
      }
    else
      {

        right->setUndefined();
      }
     }

   if(  dlg->topBorderStyle!=Qt::NoPen || !dlg->bTopBorderStyle)
    {
    if ( dlg->bTopBorderColor && dlg->bTopBorderStyle)
      {
        top->setPenStyle(dlg->topBorderStyle );
        top->setPenWidth(dlg->topBorderWidth);
        top->setColor(dlg->topBorderColor);
        top->setOn(true);
      }
    else
      {

        top->setUndefined();
      }
     }

 if(dlg->bottomBorderStyle != Qt::NoPen || !dlg->bBottomBorderStyle)
 {
    if ( dlg->bBottomBorderColor && dlg->bBottomBorderStyle )

      {
        bottom->setPenStyle(dlg->bottomBorderStyle );
        bottom->setPenWidth(dlg->bottomBorderWidth);
        bottom->setColor(dlg->bottomBorderColor);
        bottom->setOn(true);
      }
    else
      {

        bottom->setUndefined();
      }
 }

  if(dlg->oneRow==FALSE)
  {
   if(dlg->horizontalBorderStyle!=Qt::NoPen ||!dlg->bHorizontalBorderStyle)
   {
    if ( dlg->bHorizontalBorderColor && dlg->bHorizontalBorderStyle )
      {
        horizontal->setPenStyle(dlg->horizontalBorderStyle );
        horizontal->setPenWidth(dlg->horizontalBorderWidth);
        horizontal->setColor(dlg->horizontalBorderColor);
        horizontal->setOn(true);
       }
    else
       {
         horizontal->setUndefined();
       }
    }
  }
  else
        horizontal->setEnabled(false);

  if(dlg->oneCol==FALSE)
  {
        if(dlg->verticalBorderStyle!=Qt::NoPen || !dlg->bVerticalBorderStyle)
        {
                if ( dlg->bVerticalBorderColor && dlg->bVerticalBorderStyle )
                {
                vertical->setPenStyle(dlg->verticalBorderStyle );
                vertical->setPenWidth(dlg->verticalBorderWidth);
                vertical->setColor(dlg->verticalBorderColor);
                vertical->setOn(true);
                }
        else
                {
                vertical->setUndefined();
                }
         }
   }
   else
        {
        vertical->setEnabled(false);
        }

  if(dlg->fallDiagonalStyle!=Qt::NoPen || !dlg->bFallDiagonalStyle)
  {
    if ( dlg->bfallDiagonalColor && dlg->bFallDiagonalStyle  )
      {
        fallDiagonal->setPenStyle(dlg->fallDiagonalStyle );
        fallDiagonal->setPenWidth(dlg->fallDiagonalWidth);
        fallDiagonal->setColor(dlg->fallDiagonalColor);
        fallDiagonal->setOn(true);
      }
    else
      {
        fallDiagonal->setUndefined();
      }
   }

 if(dlg->goUpDiagonalStyle!=Qt::NoPen || !dlg->bGoUpDiagonalStyle)
    {
    if ( dlg->bGoUpDiagonalColor && dlg->bGoUpDiagonalStyle )
      {
        goUpDiagonal->setPenStyle(dlg->goUpDiagonalStyle );
        goUpDiagonal->setPenWidth(dlg->goUpDiagonalWidth);
        goUpDiagonal->setColor(dlg->goUpDiagonalColor);
        goUpDiagonal->setOn(true);
      }
    else
      {
        goUpDiagonal->setUndefined();
      }
    }


    pattern1->setPattern( black, 1, DotLine );
    pattern2->setPattern( black, 1, DashLine );
    pattern3->setPattern( black, 1, SolidLine );
    pattern4->setPattern( black, 2, SolidLine );
    pattern5->setPattern( black, 3, SolidLine );
    pattern6->setPattern( black, 4, SolidLine );
    pattern7->setPattern( black, 5, SolidLine );
    pattern8->setPattern( black, 1, NoPen );
    pattern9->setPattern( black, 1, DashDotLine );
    pattern10->setPattern( black, 1, DashDotDotLine );



    slotSetColorButton( black );

    connect( color, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotSetColorButton( const QColor & ) ) );


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
    connect( pattern9, SIGNAL( clicked( KSpreadPatternSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
    connect( pattern10, SIGNAL( clicked( KSpreadPatternSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );

  connect( goUpDiagonal, SIGNAL( clicked (KSpreadBorderButton *) ),
           this, SLOT( changeState( KSpreadBorderButton *) ) );
  connect( top, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( changeState(KSpreadBorderButton *) ) );
  connect( right, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( changeState(KSpreadBorderButton *) ) );

  connect( fallDiagonal, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( changeState(KSpreadBorderButton *) ) );

  connect( bottom, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( changeState(KSpreadBorderButton *) ) );
  connect( left, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( changeState(KSpreadBorderButton *) ) );
  connect( horizontal, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( changeState(KSpreadBorderButton *) ) );
  connect( vertical, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( changeState(KSpreadBorderButton *) ) );

  connect( all, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( preselect(KSpreadBorderButton *) ) );
  connect( remove, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( preselect(KSpreadBorderButton *) ) );
  connect( outline, SIGNAL( clicked(KSpreadBorderButton *) ),
           this, SLOT( preselect(KSpreadBorderButton *) ) );

  connect( area ,SIGNAL( redraw()),this,SLOT(draw()));
  connect( area ,SIGNAL( choosearea(QMouseEvent * )),
           this,SLOT( slotPressEvent(QMouseEvent *)));

  connect( style ,SIGNAL( activated(int)),this,SLOT(slotChangeStyle(int)));
  connect( size ,SIGNAL( textChanged(const QString &)),this,SLOT(slotChangeStyle(const QString &)));
  connect( size ,SIGNAL( activated(int)),this,SLOT(slotChangeStyle(int)));
  preview->slotSelect();
  pattern1->slotSelect();
  style->setEnabled(false);
  size->setEnabled(false);
  preview->setPattern( black , 1, DotLine );
  this->resize( 400, 400 );
}


void CellLayoutPageBorder::cutomize_chosen_slot()
{
        if(customize->isChecked())
                {
                style->setEnabled(true);
                size->setEnabled(true);
                }
        else
                {
                style->setEnabled(false);
                size->setEnabled(false);
                pattern1->slotSelect();
                preview->setPattern( black , 1, DotLine );
                }
}

void CellLayoutPageBorder::slotChangeStyle(const QString &)
{
slotChangeStyle(0);
}

void CellLayoutPageBorder::slotChangeStyle(int)
{
  int index =style->currentItem();
  QString tmp;
  int penSize=size->currentText().toInt();
  if( !penSize)
       preview->setPattern( preview->getColor(), penSize, NoPen );
  else
  {
  switch(index)
        {
        case 0:
                preview->setPattern( preview->getColor(), penSize, DotLine );
                break;
        case 1:
                preview->setPattern( preview->getColor(), penSize, DashLine );
                break;
        case 2:
                preview->setPattern( preview->getColor(), penSize, DashDotLine );
                break;
        case 3:
                preview->setPattern( preview->getColor(), penSize, DashDotDotLine );
                break;
        case 4:
                preview->setPattern( preview->getColor(), penSize, SolidLine );
                break;
        default:
                kdDebug(36001)<<"Error in combobox\n";
                break;
        }
 }
 slotUnselect2(preview);
}

QPixmap CellLayoutPageBorder::paintFormatPixmap(PenStyle _style)
{
    QPixmap pixmap( style->width(), 14 );
    QPainter painter;
    QPen pen;
    pen.setColor( colorGroup().text() );
    pen.setStyle( _style );
    pen.setWidth( 1 );
    painter.begin( &pixmap );
    painter.fillRect( 0, 0, style->width(), 14, colorGroup().background() );
    painter.setPen( pen );
    painter.drawLine( 0, 7, style->width(), 7 );
    painter.end();
    return pixmap;
}

void CellLayoutPageBorder::loadIcon( QString _pix,KSpreadBorderButton *_button)
{
    _button->setPixmap( QPixmap( KSBarIcon(_pix) ) );
}

void CellLayoutPageBorder::applyOutline( int _left, int _top, int _right, int _bottom )
{

    if( horizontal->isChanged())
        {
        for ( int x = _left; x <= _right; x++ )
                {
                for ( int y = _top+1; y <= _bottom; y++ )
                        {
                        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x, y );
                        if(!obj->isObscuringForced())
                                {
                                obj->setTopBorderColor( horizontal->getColor() );
                                obj->setTopBorderStyle( horizontal->getPenStyle() );
                                obj->setTopBorderWidth( horizontal->getPenWidth() );
                                }
                        }
                }
         }

    if( vertical->isChanged())
    {
    for ( int x = _left+1; x <= _right; x++ )
        {
        for ( int y = _top; y <= _bottom; y++ )
                {
                KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x,y );
                 if(!obj->isObscuringForced())
                        {
                        obj->setLeftBorderColor( vertical->getColor() );
                        obj->setLeftBorderStyle( vertical->getPenStyle() );
                        obj->setLeftBorderWidth( vertical->getPenWidth() );
                        }
                }
        }
    }


 if ( left->isChanged() )
    {
    for ( int y = _top; y <= _bottom; y++ )
        {
        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( _left,y );
         if(!obj->isObscuringForced())
                {
                obj->setLeftBorderColor( left->getColor() );
                obj->setLeftBorderStyle( left->getPenStyle() );
                obj->setLeftBorderWidth( left->getPenWidth() );
                }
        }
    }

 if ( right->isChanged() )
    {
    bool once=false;
    for ( int y = _top; y <= _bottom; y++ )
        {
        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( _right,y );
         if(!obj->isObscuringForced())
                {
                obj->setRightBorderColor( right->getColor() );
                obj->setRightBorderStyle( right->getPenStyle() );
                obj->setRightBorderWidth( right->getPenWidth() );
                }
         else if(obj->isObscuringForced() && !once )
                {
                once=true;
                int moveX=obj->obscuringCellsColumn();
                int moveY=obj->obscuringCellsRow();
                obj = dlg->getTable()->nonDefaultCell( moveX,  moveY );
                obj->setRightBorderColor( right->getColor() );
                obj->setRightBorderStyle( right->getPenStyle() );
                obj->setRightBorderWidth( right->getPenWidth() );
                }
        }
    }

 if ( top->isChanged() )
    {
    for ( int x = _left; x <= _right; x++ )
        {
        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x,_top );
         if(!obj->isObscuringForced())
                {
                obj->setTopBorderColor( top->getColor() );
                obj->setTopBorderStyle( top->getPenStyle() );
                obj->setTopBorderWidth( top->getPenWidth() );
                }
        }
    }

 if ( bottom->isChanged() )
    {
    bool once = false;
    for ( int x = _left; x <= _right; x++ )
        {
        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x,_bottom );

         if(!obj->isObscuringForced())
                {
                obj->setBottomBorderColor( bottom->getColor() );
                obj->setBottomBorderStyle( bottom->getPenStyle() );
                obj->setBottomBorderWidth( bottom->getPenWidth() );
                }
         else if(obj->isObscuringForced() && !once )
                {
                once = true;
                int moveX=obj->obscuringCellsColumn();
                int moveY=obj->obscuringCellsRow();
                obj = dlg->getTable()->nonDefaultCell( moveX,  moveY );
                obj->setBottomBorderColor( bottom->getColor() );
                obj->setBottomBorderStyle( bottom->getPenStyle() );
                obj->setBottomBorderWidth( bottom->getPenWidth() );
                }
        }
    }

 for ( int x = _left; x <= _right; x++ )
        {
        for ( int y = _top; y <= _bottom; y++ )
                {
                KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x,y );
                if(!obj->isObscuringForced())
                        {
                        if ( fallDiagonal->isChanged() )
                                {
                                obj->setFallDiagonalColor( fallDiagonal->getColor() );
                                obj->setFallDiagonalStyle( fallDiagonal->getPenStyle() );
                                obj->setFallDiagonalWidth( fallDiagonal->getPenWidth() );
                                }
                        if ( goUpDiagonal->isChanged() )
                                {
                                obj->setGoUpDiagonalColor( goUpDiagonal->getColor() );
                                obj->setGoUpDiagonalStyle( goUpDiagonal->getPenStyle() );
                                obj->setGoUpDiagonalWidth( goUpDiagonal->getPenWidth() );
                                }
                        }
                }
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
    pattern9->setColor( currentColor );
    pattern10->setColor( currentColor );
    preview->setColor( currentColor );

}

void CellLayoutPageBorder::slotUnselect2( KSpreadPatternSelect *_p )
{
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
    if ( pattern9 != _p )
        pattern9->slotUnselect();
    if ( pattern10 != _p )
        pattern10->slotUnselect();
    preview->setPattern( _p->getColor(), _p->getPenWidth(), _p->getPenStyle() );
}

void CellLayoutPageBorder::preselect( KSpreadBorderButton *_p)
{
  _p->setOn(false);
  if(_p==remove)
        {
         if(left->isOn())
                 left->unselect();

         if(right->isOn())
                 right->unselect();

          if(top->isOn())
                 top->unselect();

          if(bottom->isOn())
                 bottom->unselect();

          if(fallDiagonal->isOn())
                 fallDiagonal->unselect();

         if(goUpDiagonal->isOn())
                 goUpDiagonal->unselect();

          if(vertical->isOn())
                 vertical->unselect();

          if(horizontal->isOn())
                 horizontal->unselect();
        }
  if(_p==outline)
        {
        top->setOn(true);
        top->setPenWidth(preview->getPenWidth());
        top->setPenStyle(preview->getPenStyle());
        top->setColor( currentColor );
        top->setChanged(true);
        bottom->setOn(true);
        bottom->setPenWidth(preview->getPenWidth());
        bottom->setPenStyle(preview->getPenStyle());
        bottom->setColor( currentColor );
        bottom->setChanged(true);
        left->setOn(true);
        left->setPenWidth(preview->getPenWidth());
        left->setPenStyle(preview->getPenStyle());
        left->setColor( currentColor );
        left->setChanged(true);
        right->setOn(true);
        right->setPenWidth(preview->getPenWidth());
        right->setPenStyle(preview->getPenStyle());
        right->setColor( currentColor );
        right->setChanged(true);
        }
  if(_p==all)
        {
        if(dlg->oneRow==false)
                {
                horizontal->setOn(true);
                horizontal->setPenWidth(preview->getPenWidth());
                horizontal->setPenStyle(preview->getPenStyle());
                horizontal->setColor( currentColor );
                horizontal->setChanged(true);
                }
        if(dlg->oneCol==false)
                {
                vertical->setOn(true);
                vertical->setPenWidth(preview->getPenWidth());
                vertical->setPenStyle(preview->getPenStyle());
                vertical->setColor( currentColor );
                vertical->setChanged(true);
                }
        }
  area->repaint();
}

void CellLayoutPageBorder::changeState( KSpreadBorderButton *_p)
{
  _p->setChanged(true);

      if(_p->isOn())
        {
          _p->setPenWidth(preview->getPenWidth());
          _p->setPenStyle(preview->getPenStyle());
          _p->setColor( currentColor );
        }
      else
        {
          _p->setPenWidth(1);
          _p->setPenStyle(Qt::NoPen);
          _p->setColor( colorGroup().text() );
        }

 area->repaint();
}

void CellLayoutPageBorder::draw()
{
  QPen pen;
  QPainter painter;
  painter.begin( area );

  if((bottom->getPenStyle())!=Qt::NoPen)
    {
      pen.setColor( bottom->getColor() );
      pen.setStyle( bottom->getPenStyle() );
      pen.setWidth( bottom->getPenWidth() );

      painter.setPen( pen );
      painter.drawLine( OFFSETX, area->height()-OFFSETY, area->width()-OFFSETX , area->height()-OFFSETY );
    }
  if((top->getPenStyle())!=Qt::NoPen)
    {

      pen.setColor( top->getColor() );
      pen.setStyle( top->getPenStyle() );
      pen.setWidth( top->getPenWidth() );
      painter.setPen( pen );
      painter.drawLine( OFFSETX, OFFSETY, area->width() -OFFSETX, OFFSETY );
    }
 if((left->getPenStyle())!=Qt::NoPen)
    {

      pen.setColor( left->getColor() );
      pen.setStyle( left->getPenStyle() );
      pen.setWidth( left->getPenWidth() );
      painter.setPen( pen );
      painter.drawLine( OFFSETX, OFFSETY, OFFSETX , area->height()-OFFSETY );
    }
 if((right->getPenStyle())!=Qt::NoPen)
    {
      pen.setColor( right->getColor() );
      pen.setStyle( right->getPenStyle() );
      pen.setWidth( right->getPenWidth() );
      painter.setPen( pen );
      painter.drawLine( area->width()-OFFSETX, OFFSETY, area->width()-OFFSETX, area->height()-OFFSETY );

    }
 if((fallDiagonal->getPenStyle())!=Qt::NoPen)
    {
      pen.setColor( fallDiagonal->getColor() );
      pen.setStyle( fallDiagonal->getPenStyle() );
      pen.setWidth( fallDiagonal->getPenWidth() );
      painter.setPen( pen );
      painter.drawLine( OFFSETX, OFFSETY, area->width()-OFFSETX, area->height()-OFFSETY );
      if(dlg->oneCol==false&& dlg->oneRow==false)
        {
        painter.drawLine( area->width()/2, OFFSETY, area->width()-OFFSETX, area->height()/2 );
        painter.drawLine( OFFSETX,area->height()/2 , area->width()/2, area->height()-OFFSETY );
        }
    }
 if((goUpDiagonal->getPenStyle())!=Qt::NoPen)
    {

      pen.setColor( goUpDiagonal->getColor() );
      pen.setStyle( goUpDiagonal->getPenStyle() );
      pen.setWidth( goUpDiagonal->getPenWidth() );
      painter.setPen( pen );
      painter.drawLine( OFFSETX, area->height()-OFFSETY , area->width()-OFFSETX , OFFSETY );
      if(dlg->oneCol==false&& dlg->oneRow==false)
        {
        painter.drawLine( area->width()/2, OFFSETY, OFFSETX, area->height()/2 );
        painter.drawLine( area->width()/2,area->height()-OFFSETY , area->width()-OFFSETX, area->height()/2 );
        }

    }
 if((vertical->getPenStyle())!=Qt::NoPen)
    {
      pen.setColor( vertical->getColor() );
      pen.setStyle( vertical->getPenStyle() );
      pen.setWidth( vertical->getPenWidth() );
      painter.setPen( pen );
      painter.drawLine( area->width()/2, 5 , area->width()/2 , area->height()-5 );
    }
  if((horizontal->getPenStyle())!=Qt::NoPen)
    {
      pen.setColor( horizontal->getColor() );
      pen.setStyle( horizontal->getPenStyle() );
      pen.setWidth( horizontal->getPenWidth() );
      painter.setPen( pen );
      painter.drawLine( OFFSETX,area->height()/2,area->width()-OFFSETX, area->height()/2 );
    }
  painter.end();
}

void CellLayoutPageBorder::invertState(KSpreadBorderButton *_p)
{
  if(_p->isOn())
        {
        _p->unselect();
        }
  else
        {
        _p->setOn(!_p->isOn());
        _p->setPenWidth(preview->getPenWidth());
        _p->setPenStyle(preview->getPenStyle());
        _p->setColor( currentColor );
        _p->setChanged(true);
        }
}

void CellLayoutPageBorder::slotPressEvent(QMouseEvent *_ev)
{
  QRect rect(OFFSETX,OFFSETY-8,area->width()-OFFSETX,OFFSETY+8);
  if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {
        if(((top->getPenWidth()!=preview->getPenWidth()) ||(top->getColor()!=currentColor)
        ||(top->getPenStyle()!=preview->getPenStyle()))&& top->isOn())
                {
                top->setPenWidth(preview->getPenWidth());
                top->setPenStyle(preview->getPenStyle());
                top->setColor( currentColor );
                top->setChanged(true);
                }
        else
                invertState(top);
        }
  rect.setCoords(OFFSETX,area->height()-OFFSETY-8,area->width()-OFFSETX,area->height()-OFFSETY+8);
  if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {
         if(((bottom->getPenWidth()!=preview->getPenWidth()) ||(bottom->getColor()!=currentColor)
        ||(bottom->getPenStyle()!=preview->getPenStyle()))&& bottom->isOn())
                {
                bottom->setPenWidth(preview->getPenWidth());
                bottom->setPenStyle(preview->getPenStyle());
                bottom->setColor( currentColor );
                bottom->setChanged(true);
                }
        else
                invertState(bottom);
        }

  rect.setCoords(OFFSETX-8,OFFSETY,OFFSETX+8,area->height()-OFFSETY);
  if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {
        if(((left->getPenWidth()!=preview->getPenWidth()) ||(left->getColor()!=currentColor)
        ||(left->getPenStyle()!=preview->getPenStyle()))&& left->isOn())
                {
                left->setPenWidth(preview->getPenWidth());
                left->setPenStyle(preview->getPenStyle());
                left->setColor( currentColor );
                left->setChanged(true);
                }
        else
                invertState(left);
        }
  rect.setCoords(area->width()-OFFSETX-8,OFFSETY,area->width()-OFFSETX+8,area->height()-OFFSETY);
 if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {
        if(((right->getPenWidth()!=preview->getPenWidth()) ||(right->getColor()!=currentColor)
        ||(right->getPenStyle()!=preview->getPenStyle()))&& right->isOn())
                {
                right->setPenWidth(preview->getPenWidth());
                right->setPenStyle(preview->getPenStyle());
                right->setColor( currentColor );
                right->setChanged(true);
                }
        else
                invertState(right);
        }

//don't work because I don't know how create a rectangle
//for diagonal
/*rect.setCoords(OFFSETX,OFFSETY,XLEN-OFFSETX,YHEI-OFFSETY);
if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {
         invertState(fallDiagonal);
        }
rect.setCoords(OFFSETX,YHEI-OFFSETY,XLEN-OFFSETX,OFFSETY);
if(rect.contains(QPoint(_ev->x(),_ev->y())))
        {
         invertState(goUpDiagonal);
        } */

 if(dlg->oneCol==false)
        {
         rect.setCoords(area->width()/2-8,OFFSETY,area->width()/2+8,area->height()-OFFSETY);

        if(rect.contains(QPoint(_ev->x(),_ev->y())))
                {
                if(((vertical->getPenWidth()!=preview->getPenWidth()) ||(vertical->getColor()!=currentColor)
                ||(vertical->getPenStyle()!=preview->getPenStyle()))&& vertical->isOn())
                        {
                        vertical->setPenWidth(preview->getPenWidth());
                        vertical->setPenStyle(preview->getPenStyle());
                        vertical->setColor( currentColor );
                        vertical->setChanged(true);
                        }
                else
                        invertState(vertical);
                }
        }
 if(dlg->oneRow==false)
        {
        rect.setCoords(OFFSETX,area->height()/2-8,area->width()-OFFSETX,area->height()/2+8);
        if(rect.contains(QPoint(_ev->x(),_ev->y())))
                {
                if(((horizontal->getPenWidth()!=preview->getPenWidth()) ||(horizontal->getColor()!=currentColor)
                ||(horizontal->getPenStyle()!=preview->getPenStyle()))&& horizontal->isOn())
                        {
                        horizontal->setPenWidth(preview->getPenWidth());
                        horizontal->setPenStyle(preview->getPenStyle());
                        horizontal->setColor( currentColor );
                        horizontal->setChanged(true);
                        }
                else
                        invertState(horizontal);
                }
        }

 area->repaint();
}

KSpreadBrushSelect::KSpreadBrushSelect( QWidget *parent, const char * ) : QFrame( parent )
{
    brushStyle = Qt::NoBrush;
    brushColor = Qt::red;
    selected = FALSE;
}

void KSpreadBrushSelect::setPattern( const QColor &_color,BrushStyle _style )
{
    brushStyle = _style;
    brushColor = _color;
    repaint();
}


void KSpreadBrushSelect::paintEvent( QPaintEvent *_ev )
{
    QFrame::paintEvent( _ev );

    QPainter painter;
    QBrush brush;
    brush.setStyle(brushStyle);
    brush.setColor(brushColor);
    painter.begin( this );
    painter.setPen( Qt::NoPen );
    painter.setBrush( brush);
    painter.drawRect( 2, 2, width()-4, height()-4);
    painter.end();
}

void KSpreadBrushSelect::mousePressEvent( QMouseEvent * )
{
    slotSelect();

    emit clicked( this );
}

void KSpreadBrushSelect::slotUnselect()
{
    selected = FALSE;

    setLineWidth( 1 );
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    repaint();
}

void KSpreadBrushSelect::slotSelect()
{
    selected = TRUE;

    setLineWidth( 2 );
    setFrameStyle( QFrame::Panel | QFrame::Plain );
    repaint();
}


CellLayoutPagePattern::CellLayoutPagePattern( QWidget* parent, CellLayoutDlg *_dlg ) : QWidget( parent )
{
    dlg = _dlg;

    bBgColorUndefined = !dlg->bBgColor;

    QGridLayout *grid = new QGridLayout(this,5,2,15,15);

    QGroupBox* tmpQGroupBox;
    tmpQGroupBox = new QGroupBox( this, "GroupBox_20" );
    tmpQGroupBox->setFrameStyle( 49 );
    tmpQGroupBox->setTitle( i18n("Pattern") );
    tmpQGroupBox->setAlignment( 1 );

    QGridLayout *grid2 = new QGridLayout(tmpQGroupBox,7,3,15,7);

    brush1 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_1" );
    brush1->setFrameStyle( 50 );
    grid2->addWidget(brush1,0,0);

    brush2 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_2" );
    brush2->setFrameStyle( 50 );
    grid2->addWidget(brush2,0,1);

    brush3 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_3" );
    brush3->setFrameStyle( 50 );
    grid2->addWidget(brush3,0,2);

    brush4 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_4" );
    brush4->setFrameStyle( 50 );
    grid2->addWidget(brush4,1,0);

    brush5 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_5" );
    brush5->setFrameStyle( 50 );
    grid2->addWidget(brush5,1,1);

    brush6 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_6" );
    brush6->setFrameStyle( 50 );
    grid2->addWidget(brush6,1,2);

    brush7 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_7" );
    brush7->setFrameStyle( 50 );
    grid2->addWidget(brush7,2,0);

    brush8 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_8" );
    brush8->setFrameStyle( 50 );
    grid2->addWidget(brush8,2,1);

    brush9 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_9" );
    brush9->setFrameStyle( 50 );
    grid2->addWidget(brush9,2,2);

    brush10 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_10" );
    brush10->setFrameStyle( 50 );
    grid2->addWidget(brush10,3,0);

    brush11 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_11" );
    brush11->setFrameStyle( 50 );
    grid2->addWidget(brush11,3,1);

    brush12 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_12" );
    brush12->setFrameStyle( 50 );
    grid2->addWidget(brush12,3,2);

    brush13 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_13" );
    brush13->setFrameStyle( 50 );
    grid2->addWidget(brush13,4,0);

    brush14 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_14" );
    brush14->setFrameStyle( 50 );
    grid2->addWidget(brush14,4,1);

    brush15 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_15" );
    brush15->setFrameStyle( 50 );
    grid2->addWidget(brush15,4,2);

    QGridLayout *grid3=new QGridLayout(tmpQGroupBox,1,2);
    color = new KColorButton (tmpQGroupBox, "ColorButton_1" );
    grid3->addWidget(color,0,1);

    QLabel *tmpQLabel = new QLabel( tmpQGroupBox, "Label_1" );
    tmpQLabel->setText( i18n("Color") );
    grid3->addWidget(tmpQLabel,0,0);

    grid2->addMultiCell(grid3,5,5,0,2);

    grid3=new QGridLayout(tmpQGroupBox,1,3);

    tmpQLabel = new QLabel( tmpQGroupBox, "Label_2" );
    grid3->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText( i18n("Background Color") );

    bgColorButton = new KColorButton( tmpQGroupBox, "ColorButton" );
    grid3->addWidget(bgColorButton,0,1);
    if ( dlg->bBgColor )
        bgColor = dlg->bgColor;
    else
        bgColor = colorGroup().base();
    bgColorButton->setColor(bgColor);
    connect( bgColorButton, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotSetBackgroundColor( const QColor & ) ) );

    notAnyColor=new QPushButton(i18n("No Color"),tmpQGroupBox);
    grid3->addWidget(notAnyColor,0,2);
    connect( notAnyColor, SIGNAL( clicked( ) ),
             this, SLOT( slotNotAnyColor(  ) ) );
    b_notAnyColor=false;

    grid2->addMultiCell(grid3,6,6,0,2);

    grid->addMultiCellWidget(tmpQGroupBox,0,3,0,0);

    tmpQGroupBox = new QGroupBox( this, "GroupBox1" );
    tmpQGroupBox->setTitle( i18n("Preview") );
    tmpQGroupBox->setFrameStyle( 49 );
    tmpQGroupBox->setAlignment( 1 );

    grid2 = new QGridLayout(tmpQGroupBox,1,1,14,4);

    current = new KSpreadBrushSelect( tmpQGroupBox, "Current" );
    current->setFrameStyle( 50 );
    grid2->addWidget(current,0,0);
    grid->addWidget( tmpQGroupBox,4,0);

    connect( brush1, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush2, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush3, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush4, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush5, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush6, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush7, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush8, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush9, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush10, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush11, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush12, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush13, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush14, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );
    connect( brush15, SIGNAL( clicked( KSpreadBrushSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadBrushSelect* ) ) );

    brush1->setPattern( Qt::red, Qt::VerPattern );
    brush2->setPattern( Qt::red,Qt::HorPattern );
    brush3->setPattern( Qt::red,Qt::Dense1Pattern );
    brush4->setPattern( Qt::red,Qt::Dense2Pattern );
    brush5->setPattern( Qt::red,Qt::Dense3Pattern );
    brush6->setPattern( Qt::red,Qt::Dense4Pattern );
    brush7->setPattern( Qt::red,Qt::Dense5Pattern );
    brush8->setPattern( Qt::red,Qt::Dense6Pattern );
    brush9->setPattern(  Qt::red,Qt::Dense7Pattern );
    brush10->setPattern(  Qt::red,Qt::CrossPattern );
    brush11->setPattern( Qt::red,Qt::BDiagPattern );
    brush12->setPattern( Qt::red,Qt::FDiagPattern );
    brush13->setPattern( Qt::red,Qt::VerPattern );
    brush14->setPattern( Qt::red,Qt::DiagCrossPattern );
    brush15->setPattern( Qt::red,Qt::NoBrush );

    current->setPattern(dlg->brushColor,dlg->brushStyle);
    current->slotSelect();
    selectedBrush=current;
    color->setColor(dlg->brushColor);
    current->setBackgroundColor(bgColor);

    connect( color, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotSetColorButton( const QColor & ) ) );

    slotSetColorButton( dlg->brushColor );
    init();
    this->resize( 400, 400 );
}

void CellLayoutPagePattern::slotNotAnyColor()
{
b_notAnyColor=true;
bgColorButton->setColor(colorGroup().base());
current->setBackgroundColor(colorGroup().base());
}

void CellLayoutPagePattern::slotSetBackgroundColor( const QColor &_color )
{
bgColor =_color;
current->setBackgroundColor(bgColor);
bBgColorUndefined=false;
b_notAnyColor=false;
}

void CellLayoutPagePattern::init()
{
    if(dlg->brushStyle==Qt::VerPattern)
        {
        brush1->slotSelect();
        }
    else if(dlg->brushStyle==Qt::HorPattern)
        {
        brush2->slotSelect();
        }
    else if(dlg->brushStyle==Qt::Dense1Pattern)
        {
        brush3->slotSelect();
        }
    else if(dlg->brushStyle==Qt::Dense2Pattern)
        {
        brush4->slotSelect();
        }
    else if(dlg->brushStyle==Qt::Dense3Pattern)
        {
        brush5->slotSelect();
        }
    else if(dlg->brushStyle==Qt::Dense4Pattern)
        {
        brush6->slotSelect();
        }
    else if(dlg->brushStyle==Qt::Dense5Pattern)
        {
        brush7->slotSelect();
        }
    else if(dlg->brushStyle==Qt::Dense6Pattern)
        {
        brush8->slotSelect();
        }
    else if(dlg->brushStyle==Qt::Dense7Pattern)
        {
        brush9->slotSelect();
        }
    else if(dlg->brushStyle==Qt::CrossPattern)
        {
        brush10->slotSelect();
        }
    else if(dlg->brushStyle==Qt::BDiagPattern)
        {
        brush11->slotSelect();
        }
    else if(dlg->brushStyle==Qt::FDiagPattern)
        {
        brush12->slotSelect();
        }
    else if(dlg->brushStyle==Qt::VerPattern)
        {
        brush13->slotSelect();
        }
    else if(dlg->brushStyle==Qt::DiagCrossPattern)
        {
        brush14->slotSelect();
        }
    else if(dlg->brushStyle==Qt::NoBrush)
        {
        brush15->slotSelect();
        }
    else
        kdDebug(36001) << "Error in brushStyle" << endl;
}

void CellLayoutPagePattern::slotSetColorButton( const QColor &_color )
{
    currentColor = _color;

    brush1->setBrushColor( currentColor );
    brush2->setBrushColor( currentColor );
    brush3->setBrushColor( currentColor );
    brush4->setBrushColor( currentColor );
    brush5->setBrushColor( currentColor );
    brush6->setBrushColor( currentColor );
    brush7->setBrushColor( currentColor );
    brush8->setBrushColor( currentColor );
    brush9->setBrushColor( currentColor );
    brush10->setBrushColor( currentColor );
    brush11->setBrushColor( currentColor );
    brush12->setBrushColor( currentColor );
    brush13->setBrushColor( currentColor );
    brush14->setBrushColor( currentColor );
    brush15->setBrushColor( currentColor );
    current->setBrushColor( currentColor );
}

void CellLayoutPagePattern::slotUnselect2( KSpreadBrushSelect *_p )
{
    selectedBrush = _p;

    if ( brush1 != _p )
        brush1->slotUnselect();
    if ( brush2 != _p )
        brush2->slotUnselect();
    if ( brush3 != _p )
        brush3->slotUnselect();
    if ( brush4 != _p )
        brush4->slotUnselect();
    if ( brush5 != _p )
        brush5->slotUnselect();
    if ( brush6 != _p )
        brush6->slotUnselect();
    if ( brush7 != _p )
        brush7->slotUnselect();
    if ( brush8 != _p )
        brush8->slotUnselect();
    if ( brush9 != _p )
        brush9->slotUnselect();
    if ( brush10 != _p )
        brush10->slotUnselect();
    if ( brush11 != _p )
        brush11->slotUnselect();
    if ( brush12 != _p )
        brush12->slotUnselect();
    if ( brush13 != _p )
        brush13->slotUnselect();
    if ( brush14 != _p )
        brush14->slotUnselect();
    if ( brush15 != _p )
        brush15->slotUnselect();

    current->setBrushStyle(selectedBrush->getBrushStyle());
}

void CellLayoutPagePattern::apply( KSpreadCell *_obj )
{
  if(selectedBrush!=0L)
        {
         _obj->setBackGroundBrushColor(selectedBrush->getBrushColor() );
         _obj->setBackGroundBrushStyle(selectedBrush->getBrushStyle() );
        }
  if( b_notAnyColor)
        _obj->setBgColor( QColor() );
  else if ( !bBgColorUndefined )
        _obj->setBgColor( bgColor );
}
#include "kspread_dlg_layout.moc"
