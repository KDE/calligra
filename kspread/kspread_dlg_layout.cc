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

#include <stdlib.h>
#include <math.h>

#include "kspread_dlg_layout.h"
#include "kspread_undo.h"
#include "kspread_table.h"
#include "kspread_util.h"
#include "kspread_canvas.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qfontdatabase.h>
#include <kdebug.h>
#include <knumvalidator.h>
#include <qradiobutton.h>
#include <klineedit.h>
#include <qcheckbox.h>
#include <kcolorbutton.h>
#include <qcombobox.h>

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

    QPainter painter( this );

    if ( !undefined )
    {
        QPen pen( penColor, penWidth, penStyle);
        painter.setPen( pen );
        painter.drawLine( 6, height()/2, width() - 6,height()/2  );
    }
    else
    {
        painter.fillRect( 2, 2, width() - 4, height() - 4, BDiagPattern );
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

CellLayoutDlg::CellLayoutDlg( KSpreadView *_view, KSpreadTable *_table,
                              int _left, int _top, int _right, int _bottom )
  : QObject()
{
    formatOnlyNegSignedPixmap = 0L;
    formatRedOnlyNegSignedPixmap = 0L;
    formatRedNeverSignedPixmap = 0L;
    formatAlwaysSignedPixmap = 0L;
    formatRedAlwaysSignedPixmap = 0L;
    table = _table;

    //We need both conditions quite often, so store the condition here too
    isRowSelected = util_isRowSelected(_view->selection());
    isColumnSelected = util_isColumnSelected(_view->selection());

    //Do we really need these as arguments? (_view->selection())
    left = _left;
    top = _top;
    right = _right;
    bottom = _bottom;

    m_pView = _view;

    m_bValue=false;
    m_bDate=false;
    m_bTime=false;

    // We assume, that all other objects have the same values
    for (int i=0; i < BorderType_END; i++)
    {
      borders[i].bStyle = TRUE;
      borders[i].bColor = TRUE;
    }
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
    bFormatType = TRUE;
    bDontprintText = TRUE;

    if( left == right )
        oneCol = TRUE;
    else
        oneCol = FALSE;

    if( top == bottom )
        oneRow = TRUE;
    else
        oneRow = FALSE;


    KSpreadCell *obj = table->cellAt( _left, _top );
    oneCell = (left == right && top == bottom &&
               !obj->isForceExtraCells());

    isMerged = ((obj->isForceExtraCells() &&
                 left + obj->extraXCells() >= right &&
                 top + obj->extraYCells() >= bottom));


    // Initialize with the upper left object.
    borders[BorderType_Left].style = obj->leftBorderStyle( _left, _top );
    borders[BorderType_Left].width = obj->leftBorderWidth( _left, _top );
    borders[BorderType_Left].color = obj->leftBorderColor( _left, _top );
    borders[BorderType_Top].style = obj->topBorderStyle( _left, _top );
    borders[BorderType_Top].width = obj->topBorderWidth( _left, _top );
    borders[BorderType_Top].color = obj->topBorderColor( _left, _top );
    borders[BorderType_FallingDiagonal].style =
      obj->fallDiagonalStyle( _left, _top );
    borders[BorderType_FallingDiagonal].width =
      obj->fallDiagonalWidth( _left, _top );
    borders[BorderType_FallingDiagonal].color =
      obj->fallDiagonalColor( _left, _top );
    borders[BorderType_RisingDiagonal].style =
      obj->goUpDiagonalStyle( _left, _top );
    borders[BorderType_RisingDiagonal].width =
      obj->goUpDiagonalWidth( _left, _top );
    borders[BorderType_RisingDiagonal].color =
      obj->goUpDiagonalColor( _left, _top );

    // Look at the upper right one for the right border.
    obj = table->cellAt( _right, _top );
    borders[BorderType_Right].style = obj->rightBorderStyle( _right, _top );
    borders[BorderType_Right].width = obj->rightBorderWidth( _right, _top );
    borders[BorderType_Right].color = obj->rightBorderColor( _right, _top );

    // Look at the bottom left cell for the bottom border.
    obj = table->cellAt( _left, _bottom );
    borders[BorderType_Bottom].style = obj->bottomBorderStyle( _left, _bottom );
    borders[BorderType_Bottom].width = obj->bottomBorderWidth( _left, _bottom );
    borders[BorderType_Bottom].color = obj->bottomBorderColor( _left, _bottom );

    // Just an assumption
    obj = table->cellAt( _right, _top );
    if(obj->isObscuringForced())
    {
      obj = obj->obscuringCells().first();
      int moveX=obj->column();
      int moveY=_top;
      int moveX2 = _right;
      int moveY2 = obj->row();
      borders[BorderType_Vertical].style = obj->leftBorderStyle( moveX, moveY );
      borders[BorderType_Vertical].width = obj->leftBorderWidth( moveX, moveY );
      borders[BorderType_Vertical].color = obj->leftBorderColor( moveX, moveY );

      obj = table->cellAt( moveX2,  moveY2 );
      borders[BorderType_Horizontal].style = obj->topBorderStyle( moveX2, moveY2 );
      borders[BorderType_Horizontal].width = obj->topBorderWidth( moveX2, moveY2 );
      borders[BorderType_Horizontal].color = obj->topBorderColor( moveX2, moveY2 );
    }
    else
    {
      borders[BorderType_Vertical].style = obj->leftBorderStyle( _right, _top );
      borders[BorderType_Vertical].width = obj->leftBorderWidth( _right, _top );
      borders[BorderType_Vertical].color = obj->leftBorderColor( _right, _top );
      borders[BorderType_Horizontal].style = obj->topBorderStyle(_right, _bottom);
      borders[BorderType_Horizontal].width = obj->topBorderWidth(_right, _bottom);
      borders[BorderType_Horizontal].color = obj->topBorderColor(_right, _bottom);
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
    bDontprintText=obj->getDontprintText( _left, _top );
    textRotation = obj->getAngle(_left, _top);
    formatType = obj->getFormatType(_left, _top);

    indent = obj->getIndent(_left, _top);

    cellText=obj->text();

    if( obj->isNumeric() || obj->isBool())
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
    widthSize = 0.0;
    heigthSize = 0.0;

    if( !isRowSelected )
    {
      for ( int x = _left; x <= _right; x++ )
      {
        cl = m_pView->activeTable()->columnLayout( x );
        widthSize = QMAX( cl->dblWidth(), widthSize );
      }
    }

    if( !isColumnSelected )
    {
      for ( int y = _top; y <= _bottom; y++ )
      {
        rl = m_pView->activeTable()->rowLayout(y);
        heigthSize = QMAX( rl->dblHeight(), heigthSize );
      }
    }

    //select column(s)
    if( isColumnSelected )
    {
      int y=1;
      KSpreadCell* c = NULL;
      for(int x=_left;x<=_right;x++)
      {
        ColumnLayout *obj=table->nonDefaultColumnLayout(x);
        initParameters( obj,x,y);

        for(c = table->getFirstCellColumn(x); c != NULL;
            c = table->getNextCellDown(c->column(), c->row()))
        {
          initParameters( c, x, c->row());
          if ( eStyle != c->style() )
            eStyle = KSpreadCell::ST_Undef;
        }
      }

    }
    else if( isRowSelected )
    {
      int x=1;
      KSpreadCell* c = NULL;
      for(int y=_top;y<=_bottom;y++)
      {
        RowLayout *obj=table->nonDefaultRowLayout(y);
        initParameters( obj,x,y);

        for(c = table->getFirstCellRow(y); c != NULL;
            c = table->getNextCellRight(c->column(), c->row()) )
        {
          initParameters( c, c->column(), c->row());
          if ( eStyle != c->style() )
            eStyle = KSpreadCell::ST_Undef;
        }
      }
    }
    else
    {
      // Do the other objects have the same values ?
      for ( int x = _left; x <= _right; x++ )
      {
        for ( int y = _top; y <= _bottom; y++ )
        {
          KSpreadCell *obj = table->cellAt( x, y );
          initParameters( obj,x,y);
          if ( eStyle != obj->style() )
            eStyle = KSpreadCell::ST_Undef;
        }
      }
    }
    if( !bTextRotation )
      textRotation = 0;

    if( isColumnSelected )
    {
      int y=1;
      ColumnLayout *obj=table->nonDefaultColumnLayout(_left);
      checkBorderLeft( obj,_left, y);

      KSpreadCell* c = NULL;
      for(c = table->getFirstCellColumn(_left); c != NULL;
          c = table->getNextCellDown(c->column(), c->row()) )
      {
         checkBorderLeft(c, c->column(), c->row());
      }


      obj=table->nonDefaultColumnLayout(_right);
      checkBorderRight(obj,_right,y);
      c = NULL;
      for(c = table->getFirstCellColumn(_right); c != NULL;
          c = table->getNextCellDown(c->column(), c->row()) )
      {
        checkBorderRight(c, c->column(), c->row());
      }

      for ( int x = _left; x <= _right; x++ )
      {
        KSpreadCell *obj = table->cellAt( x, _top );
        checkBorderTop(obj,x, _top);
        obj = table->cellAt( x, _bottom );
        checkBorderBottom(obj,x, _bottom);
        if ( x > _left )
        {
          ColumnLayout *obj = table->nonDefaultColumnLayout(x);
          checkBorderHorizontal(obj,x, y);
          checkBorderVertical(obj,x, y);
        }
      }
    }
    else if( isRowSelected )
    {
      int x=1;
      for ( int y = _top; y <= _bottom; y++ )
      {
        KSpreadCell *obj = table->cellAt( _right, y );
        checkBorderRight(obj,_right,y);
        obj = table->cellAt( _left, y );
        checkBorderLeft( obj,_left, y);
        if ( y > _top )
        {
          RowLayout* obj = table->nonDefaultRowLayout(y);
          checkBorderHorizontal(obj,x, y);
          checkBorderVertical(obj,x, y);
        }
      }

      RowLayout *obj=table->nonDefaultRowLayout(_top);
      checkBorderTop(obj,x, _top);
      obj=table->nonDefaultRowLayout(_bottom);
      checkBorderBottom(obj,x, _bottom);
    }
    else
    {
      for ( int y = _top; y <= _bottom; y++ )
      {
        KSpreadCell *obj = table->cellAt( _left, y );
        checkBorderLeft( obj,_left, y);
        obj = table->cellAt( _right, y );
        checkBorderRight(obj,_right,y);
      }

      for ( int x = _left; x <= _right; x++ )
      {
        KSpreadCell *obj = table->cellAt( x, _top );
        checkBorderTop( obj, x, _top );
        obj = table->cellAt( x, _bottom );
        checkBorderBottom( obj, x, _bottom );
      }

      // Look for the Outline
      for ( int x = _left; x <= _right; x++ )
      {
        for ( int y = _top+1; y <= _bottom; y++ )
        {
          KSpreadCell *obj = table->cellAt( x, y );
            checkBorderHorizontal(obj,x, y);
        }
      }

      for ( int x = _left+1; x <= _right; x++ )
      {
        for ( int y = _top; y <= _bottom; y++ )
        {
          KSpreadCell *obj = table->cellAt( x, y );
          checkBorderVertical(obj,x,y);
        }
      }
    }
    init();
}

CellLayoutDlg::~CellLayoutDlg()
{
  delete formatOnlyNegSignedPixmap;
  delete formatRedOnlyNegSignedPixmap;
  delete formatRedNeverSignedPixmap;
  delete formatAlwaysSignedPixmap;
  delete formatRedAlwaysSignedPixmap;
}

void CellLayoutDlg::checkBorderRight(KSpreadLayout *obj,int x,int y)
{
  if ( borders[BorderType_Right].style != obj->rightBorderStyle( x, y ) ||
       borders[BorderType_Right].width != obj->rightBorderWidth( x, y ) )
    borders[BorderType_Right].bStyle = FALSE;
  if ( borders[BorderType_Right].color != obj->rightBorderColor( x, y ) )
    borders[BorderType_Right].bColor = FALSE;
}

void CellLayoutDlg::checkBorderLeft(KSpreadLayout *obj,int x,int y)
{
  if ( borders[BorderType_Left].style != obj->leftBorderStyle( x, y ) ||
       borders[BorderType_Left].width != obj->leftBorderWidth( x, y ) )
    borders[BorderType_Left].bStyle = FALSE;
  if ( borders[BorderType_Left].color != obj->leftBorderColor( x, y ) )
    borders[BorderType_Left].bColor = FALSE;
}

void CellLayoutDlg::checkBorderTop(KSpreadLayout *obj,int x,int y)
{
  if ( borders[BorderType_Top].style != obj->topBorderStyle( x, y ) ||
       borders[BorderType_Top].width != obj->topBorderWidth( x, y ) )
    borders[BorderType_Top].bStyle = FALSE;
  if ( borders[BorderType_Top].color != obj->topBorderColor( x, y ) )
    borders[BorderType_Top].bColor = FALSE;
}

void CellLayoutDlg::checkBorderBottom(KSpreadLayout *obj,int x,int y)
{
  if ( borders[BorderType_Bottom].style != obj->bottomBorderStyle( x, y ) ||
       borders[BorderType_Bottom].width != obj->bottomBorderWidth( x, y ) )
    borders[BorderType_Bottom].bStyle = FALSE;
  if ( borders[BorderType_Bottom].color != obj->bottomBorderColor( x, y ) )
    borders[BorderType_Bottom].bColor = FALSE;
}

void CellLayoutDlg::checkBorderVertical(KSpreadLayout *obj,int x,int y)
{
  if (borders[BorderType_Vertical].style != obj->leftBorderStyle( x, y ) ||
      borders[BorderType_Vertical].width != obj->leftBorderWidth( x, y ))
    borders[BorderType_Vertical].bStyle = FALSE;
  if ( borders[BorderType_Vertical].color != obj->leftBorderColor( x, y ) )
    borders[BorderType_Vertical].bColor = FALSE;
}

void CellLayoutDlg::checkBorderHorizontal(KSpreadLayout *obj,int x,int y)
{
  if ( borders[BorderType_Horizontal].style != obj->topBorderStyle( x, y ) ||
       borders[BorderType_Horizontal].width != obj->topBorderWidth( x, y ) )
    borders[BorderType_Horizontal].bStyle = FALSE;
  if ( borders[BorderType_Horizontal].color != obj->topBorderColor( x, y ) )
    borders[BorderType_Horizontal].bColor = FALSE;
}


void CellLayoutDlg::initParameters(KSpreadLayout *obj,int x,int y)
{
  if (borders[BorderType_FallingDiagonal].style != obj->fallDiagonalStyle( x, y ))
    borders[BorderType_FallingDiagonal].bStyle = FALSE;
  if (borders[BorderType_FallingDiagonal].width != obj->fallDiagonalWidth( x, y ))
    borders[BorderType_FallingDiagonal].bStyle = FALSE;
  if (borders[BorderType_FallingDiagonal].color != obj->fallDiagonalColor( x, y ))
    borders[BorderType_FallingDiagonal].bColor = FALSE;

  if (borders[BorderType_RisingDiagonal].style != obj->goUpDiagonalStyle( x, y ))
    borders[BorderType_RisingDiagonal].bStyle = FALSE;
  if (borders[BorderType_RisingDiagonal].width != obj->goUpDiagonalWidth( x, y ))
    borders[BorderType_RisingDiagonal].bStyle = FALSE;
  if (borders[BorderType_RisingDiagonal].color != obj->goUpDiagonalColor( x, y ))
    borders[BorderType_RisingDiagonal].bColor = FALSE;
  if ( strike != obj->textFontStrike( x, y ) )
    bStrike = FALSE;
  if ( underline != obj->textFontUnderline( x, y ) )
    bUnderline = FALSE;
  if ( prefix != obj->prefix( x, y ) )
    prefix = QString::null;
  if ( postfix != obj->postfix( x, y ) )
    postfix = QString::null;
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
  if( textRotation != obj->getAngle(left, top) )
    bTextRotation = FALSE;
  if( formatType != obj->getFormatType(left, top) )
    bFormatType = FALSE;
  if( bMultiRow != obj->multiRow( left, top ) )
    bMultiRow = FALSE;
  if( bVerticalText!=obj->verticalText( left, top ) )
    bVerticalText = FALSE;
  if(  bDontprintText!=obj->getDontprintText( left, top ) )
    bDontprintText= FALSE;
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
    }

    tab = new QTabDialog( (QWidget*)m_pView, 0L, TRUE );
    tab->setGeometry( tab->x(), tab->y(), 420, 400 );

    floatPage = new CellLayoutPageFloat( tab, this );
    tab->addTab( floatPage, i18n("&Data Format") );

    borderPage = new CellLayoutPageBorder( tab, this );
    tab->addTab( borderPage, i18n("&Border") );

//    miscPage = new CellLayoutPageMisc( tab, this );
//    tab->addTab( miscPage, i18n("&Misc") );

    fontPage = new CellLayoutPageFont( tab, this );
    tab->addTab( fontPage, i18n("&Text") );

    positionPage = new CellLayoutPagePosition( tab, this);
    tab->addTab( positionPage, i18n("&Position") );

    patternPage=new CellLayoutPagePattern(tab,this);
    tab->addTab( patternPage,i18n("Back&ground"));

    tab->setCancelButton();

    tab->setCaption(i18n("Cell Format"));

    connect( tab, SIGNAL( applyButtonPressed() ), this, SLOT( slotApply() ) );

    tab->exec();
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
  KSpreadCell *cell = NULL;

    KSpreadMacroUndoAction *macroUndo=new KSpreadMacroUndoAction( table->doc(),i18n("Change Layout") );

    if( isMerged!= positionPage->getMergedCellState())
    {
        if(positionPage->getMergedCellState())
        {
            KSpreadCell *obj = table->nonDefaultCell( left, top );

            KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( table->doc(), table, left ,top ,obj->extraXCells() ,obj->extraYCells());
            macroUndo->addCommand(undo);

            //merge cell doesn't create undo
            table->mergeCells(m_pView->selection(), false);
            right=left;
            bottom=top;
        }
        else
        {
            //dissociate cells
            KSpreadCell *obj = table->nonDefaultCell( left, top );
            right=obj->extraXCells()+left;
            bottom=obj->extraYCells()+top;

            KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( table->doc(), table, left ,top,obj->extraXCells() ,obj->extraYCells());
            macroUndo->addCommand(undo);

            table->dissociateCell(QPoint(left,top),false);
        }

    }

    // Prepare the undo buffer
    if ( !table->doc()->undoBuffer()->isLocked() )
    {
        QRect rect;
        // Since the right/bottom border is stored in objects right + 1 ( or: bottom + 1 )
        // So we have to save these layouts, too
        if( (!isRowSelected ) && ( !isColumnSelected ) )
            rect.setCoords( left, top, right + 1, bottom + 1 );
        else if( isRowSelected )
            rect.setCoords( left, top, right , bottom+1  );
        else if( isColumnSelected )
        {
            //create cell before to apply
            RowLayout* rw =table->firstRow();
            for( ; rw; rw = rw->next() )
            {
                if ( !rw->isDefault() )
                {
                    for(int i=left;i<=right;i++)
                    {
		      cell = table->nonDefaultCell( i, rw->row() );
                    }
                }
            }
            rect.setCoords( left, top, right+1 , bottom  );
        }

        QString title=i18n("Change layout");
        KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( table->doc(), table, rect, title );
//        table->doc()->undoBuffer()->appendUndo( undo );
        macroUndo->addCommand(undo);

/*	if( miscPage->getStyle()!=eStyle)
        {
	    //make undo for style of cell
	    KSpreadUndoStyleCell *undo3 = new KSpreadUndoStyleCell( table->doc(), table, rect );
	    //table->doc()->undoBuffer()->appendUndo( undo3 );
            macroUndo->addCommand( undo3 );
        }*/
    }
    borderPage->applyOutline();

    if( ( !isRowSelected ) && ( !isColumnSelected ) )
    {
        for ( int x = left; x <= right; x++ )
            for ( int y = top; y <= bottom; y++ )
            {
                KSpreadCell *obj = table->nonDefaultCell( x, y );
                if( !obj->isObscuringForced() )
                {
                    floatPage->apply( obj );
//                    miscPage->apply( obj );
                    fontPage->apply( obj );
                    positionPage->apply( obj );
                    patternPage->apply(obj);
                }
            }

        // Check for a change in the height and width of the cells
        if( int( positionPage->getSizeHeight() ) != int( heigthSize )
            || int( positionPage->getSizeWidth() ) != int( widthSize ) )
        {
            if ( !table->doc()->undoBuffer()->isLocked() )
            {
                QRect rect;
                rect.setCoords( left, top, right , bottom  );
                KSpreadUndoResizeColRow *undo2 = new KSpreadUndoResizeColRow( table->doc(), table , rect );
                //table->doc()->undoBuffer()->appendUndo( undo2 );
                macroUndo->addCommand( undo2 );
            }
        }
        if( int( positionPage->getSizeHeight() ) != int( heigthSize ) )
        {
            for ( int x = top; x <= bottom; x++ ) // The loop seems to be doubled, already done in resizeRow: Philipp -> fixme
                m_pView->vBorderWidget()->resizeRow( positionPage->getSizeHeight(), x, false );

        }
        if( int( positionPage->getSizeWidth() ) != int( widthSize ) ) // The loop seems to be doubled, already done in resizeColumn: Philipp -> fixme
        {
            for ( int x = left; x <= right; x++ )
                m_pView->hBorderWidget()->resizeColumn( positionPage->getSizeWidth(), x, false );
        }

    }
    else if( isRowSelected )
    {
        for(int i=top;i<=bottom;i++)
        {
            RowLayout *rw=table->nonDefaultRowLayout(i);
            floatPage->apply(rw );
            fontPage->apply( rw );
            positionPage->apply( rw );
            patternPage->apply(rw);
        }
//        miscPage->applyRow( );
        if( int( positionPage->getSizeHeight() ) != int( heigthSize ) )
        {
            if ( !table->doc()->undoBuffer()->isLocked())
            {
                QRect rect;
                rect.setCoords( left, top, right , bottom  );
                KSpreadUndoResizeColRow *undo2 = new KSpreadUndoResizeColRow( table->doc(),table , rect );
                //table->doc()->undoBuffer()->appendUndo( undo2 );
                macroUndo->addCommand(undo2);
            }
            for ( int x = top; x <= bottom; x++ ) // The loop seems to be doubled, already done in resizeRow: Philipp -> fixme
                m_pView->vBorderWidget()->resizeRow(positionPage->getSizeHeight(),x,false );
        }
    }
    else if( isColumnSelected )
    {
        for(int i=left;i<=right;i++)
        {
            ColumnLayout *cl=table->nonDefaultColumnLayout(i);
            floatPage->apply(cl );
            fontPage->apply( cl );
            positionPage->apply( cl );
            patternPage->apply(cl);
        }
//        miscPage->applyColumn( );

        if( int( positionPage->getSizeWidth() ) != int( widthSize ) )
        {
            if ( !table->doc()->undoBuffer()->isLocked())
            {
                QRect rect;
                rect.setCoords( left, top, right , bottom  );
                KSpreadUndoResizeColRow *undo2 = new KSpreadUndoResizeColRow( table->doc(),table , rect );
                //table->doc()->undoBuffer()->appendUndo( undo2 );
                macroUndo->addCommand(undo2);
            }
            for ( int x = left; x <= right; x++ ) // The loop seems to be doubled, already done in resizeColumn: Philipp -> fixme
                m_pView->hBorderWidget()->resizeColumn(positionPage->getSizeWidth(),x,false );
        }
    }

    if ( !table->doc()->undoBuffer()->isLocked())
    {
        table->doc()->undoBuffer()->appendUndo( macroUndo );
    }
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

    QButtonGroup *grp = new QButtonGroup( i18n("Format"),this);
    QGridLayout *grid = new QGridLayout(grp,8,2,15,7);
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

    date=new QRadioButton(i18n("Date format"),grp);
    grid->addWidget(date,5,0);

    time=new QRadioButton(i18n("Time format"),grp);
    grid->addWidget(time,6,0);

    textFormat=new QRadioButton(i18n("Text"),grp);
    grid->addWidget(textFormat,7,0);

    QGroupBox *box2 = new QGroupBox( grp, "Box");
    box2->setTitle(i18n("Preview"));
    QGridLayout *grid3 = new QGridLayout(box2,1,3,14,7);

    exampleLabel=new QLabel(box2);
    grid3->addWidget(exampleLabel,0,1);

    grid->addMultiCellWidget(box2,6,7,1,1);

    listFormat=new QListBox(grp);
    grid->addMultiCellWidget(listFormat,0,5,1,1);
    layout->addWidget(grp);


    /* *** */

    QGroupBox *box = new QGroupBox( this, "Box");

    grid = new QGridLayout(box,3,4,7,7);

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
    tmpQLabel->setText( i18n("Postfix:") );

    if ( dlg->postfix.isNull() )
        postfix->setText( "########" );
    else
        postfix->setText( dlg->postfix );

    tmpQLabel = new QLabel( box, "Label_2" );
    grid->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText( i18n("Prefix:") );

    tmpQLabel = new QLabel( box, "Label_3" );
    grid->addWidget(tmpQLabel,1,0);
    tmpQLabel->setText( i18n("Precision:") );

    if ( dlg->prefix.isNull() )
        prefix->setText( "########" );
    else
        prefix->setText( dlg->prefix );


    format->insertItem( *_dlg->formatOnlyNegSignedPixmap, 0 );
    format->insertItem( *_dlg->formatRedOnlyNegSignedPixmap, 1 );
    format->insertItem( *_dlg->formatRedNeverSignedPixmap, 2 );
    format->insertItem( *_dlg->formatAlwaysSignedPixmap, 3 );
    format->insertItem( *_dlg->formatRedAlwaysSignedPixmap, 4 );

    tmpQLabel = new QLabel( box, "Label_4" );
    grid->addWidget(tmpQLabel,0,2);
    tmpQLabel->setText( i18n("Format:") );


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

    cellFormatType=dlg->formatType;

    if(!cellFormatType)
          number->setChecked(true);
    else
        {
        if(cellFormatType==KSpreadCell::Number)
                number->setChecked(true);
        else if(cellFormatType==KSpreadCell::Percentage)
                percent->setChecked(true);
        else if(cellFormatType==KSpreadCell::Money)
                money->setChecked(true);
        else if(cellFormatType==KSpreadCell::Scientific)
                scientific->setChecked(true);
        else if(cellFormatType==KSpreadCell::TextDate ||
        cellFormatType==KSpreadCell::ShortDate
        ||((int)(cellFormatType)>=200 && (int)(cellFormatType)<=217))
                date->setChecked(true);
        else if(cellFormatType==KSpreadCell::Time ||
        cellFormatType==KSpreadCell::SecondeTime
        ||cellFormatType==KSpreadCell::Time_format1
        ||cellFormatType==KSpreadCell::Time_format2
        ||cellFormatType==KSpreadCell::Time_format3)
                time->setChecked(true);
        else if(cellFormatType==KSpreadCell::fraction_half ||
        cellFormatType==KSpreadCell::fraction_quarter ||
        cellFormatType==KSpreadCell::fraction_eighth ||
        cellFormatType==KSpreadCell::fraction_sixteenth ||
        cellFormatType==KSpreadCell::fraction_tenth ||
        cellFormatType==KSpreadCell::fraction_hundredth ||
        cellFormatType==KSpreadCell::fraction_one_digit ||
        cellFormatType==KSpreadCell::fraction_two_digits ||
        cellFormatType==KSpreadCell::fraction_three_digits)
                fraction->setChecked(true);
	else if(cellFormatType==KSpreadCell::Text_format)
	  textFormat->setChecked(true);
        }

    connect(fraction,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(money,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(date,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(scientific,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(number,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(percent,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(time,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(textFormat,SIGNAL(clicked()),this,SLOT(slotChangeState()));

    connect(listFormat,SIGNAL(selectionChanged ()),this,SLOT(makeformat()));
    connect(precision,SIGNAL(valueChanged(int)),this,SLOT(slotChangeValue(int)));
    connect(prefix,SIGNAL(textChanged ( const QString & ) ),this,SLOT(makeformat()));
    connect(postfix,SIGNAL(textChanged ( const QString & ) ),this,SLOT(makeformat()));
    connect(format,SIGNAL(activated ( int ) ),this,SLOT(formatChanged(int)));
    slotChangeState();
    m_bFormatColorChanged=false;
    m_bFormatTypeChanged=false;
    this->resize( 400, 400 );
}

void CellLayoutPageFloat::formatChanged(int)
{
    m_bFormatColorChanged=true;
}

void CellLayoutPageFloat::slotChangeValue(int)
{
    makeformat();
}
void CellLayoutPageFloat::slotChangeState()
{
    QStringList list;
    listFormat->clear();
    if(dlg->cellText.isEmpty() || dlg->m_bValue || !dlg->isSingleCell())
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
    {
        listFormat->setEnabled(false);
        precision->setValue(2);
    }
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
            list+=i18n("Halves 1/2");
            list+=i18n("Quarters 1/4");
            list+=i18n("Eighth's 1/8");
            list+=i18n("Sixteenth's 1/16");
            list+=i18n("Tenth's 1/10");
            list+=i18n("Hundredth's 1/100");
            list+=i18n("One digit 5/9");
            list+=i18n("Two digits 15/22");
            list+=i18n("Three digits 153/652");
            listFormat->insertStringList(list);
            if(cellFormatType==KSpreadCell::fraction_half)
                listFormat->setCurrentItem(0);
            else if(cellFormatType==KSpreadCell::fraction_quarter)
                listFormat->setCurrentItem(1);
            else if(cellFormatType==KSpreadCell::fraction_eighth )
                listFormat->setCurrentItem(2);
            else if(cellFormatType==KSpreadCell::fraction_sixteenth )
                listFormat->setCurrentItem(3);
            else if(cellFormatType==KSpreadCell::fraction_tenth )
                listFormat->setCurrentItem(4);
            else if(cellFormatType==KSpreadCell::fraction_hundredth )
                listFormat->setCurrentItem(5);
            else if(cellFormatType==KSpreadCell::fraction_one_digit )
                listFormat->setCurrentItem(6);
            else if(cellFormatType==KSpreadCell::fraction_two_digits )
                listFormat->setCurrentItem(7);
            else if(cellFormatType==KSpreadCell::fraction_three_digits )
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


            list+=i18n("System: ")+dlg->locale()->formatTime(QTime::currentTime(),false);
            list+=i18n("System: ")+dlg->locale()->formatTime(QTime::currentTime(),true);
            QTime tmpTime(10,35,25);
            list+= util_timeFormat(dlg->locale(), tmpTime, KSpreadCell::Time_format1);
            list+= util_timeFormat(dlg->locale(), tmpTime, KSpreadCell::Time_format2);
            list+= util_timeFormat(dlg->locale(), tmpTime, KSpreadCell::Time_format3);
            listFormat->insertStringList(list);

            if( cellFormatType==KSpreadCell::Time )
                listFormat->setCurrentItem(0);
            else if(cellFormatType==KSpreadCell::SecondeTime)
                listFormat->setCurrentItem(1);
            else if(cellFormatType==KSpreadCell::Time_format1)
                listFormat->setCurrentItem(2);
            else if(cellFormatType==KSpreadCell::Time_format2)
                listFormat->setCurrentItem(3);
            else if(cellFormatType==KSpreadCell::Time_format3)
                listFormat->setCurrentItem(4);
            else
                listFormat->setCurrentItem(0);
        }
    else if(textFormat->isChecked())
      {
	listFormat->setEnabled(false);
      }
    m_bFormatTypeChanged=true;
    if( date->isChecked() && dlg->m_bDate)
        makeDateFormat();
    else
        makeformat();
}

void CellLayoutPageFloat::makeDateFormat()
{
    KSpreadCell::FormatType tmpFormat=KSpreadCell::ShortDate;
    QString tmp;
    if( listFormat->currentItem()==0)
        tmpFormat=KSpreadCell::ShortDate;
    else if(listFormat->currentItem()==1)
        tmpFormat=KSpreadCell::TextDate;
    else if(listFormat->currentItem()==2)/*18-Feb-99*/
        tmpFormat=KSpreadCell::date_format1;
    else if(listFormat->currentItem()==3) /*18-Feb-1999*/
        tmpFormat=KSpreadCell::date_format2;
    else if(listFormat->currentItem()==4) /*18-Feb*/
        tmpFormat=KSpreadCell::date_format3;
    else if(listFormat->currentItem()==5) /*18-05*/
        tmpFormat=KSpreadCell::date_format4;
    else if(listFormat->currentItem()==6) /*18/05/00*/
        tmpFormat=KSpreadCell::date_format5;
    else if(listFormat->currentItem()==7) /*18/05/1999*/
        tmpFormat=KSpreadCell::date_format6;
    else if(listFormat->currentItem()==8) /*Feb-99*/
        tmpFormat=KSpreadCell::date_format7;
    else if(listFormat->currentItem()==9) /*February-99*/
        tmpFormat=KSpreadCell::date_format8;
    else if(listFormat->currentItem()==10) /*February-1999*/
        tmpFormat=KSpreadCell::date_format9;
    else if(listFormat->currentItem()==11) /*F-99*/
        tmpFormat=KSpreadCell::date_format10;
    else if(listFormat->currentItem()==12) /*18/Feb*/
        tmpFormat=KSpreadCell::date_format11;
    else if(listFormat->currentItem()==13) /*18/02*/
        tmpFormat=KSpreadCell::date_format12;
    else if(listFormat->currentItem()==14) /*18/Feb/1999*/
        tmpFormat=KSpreadCell::date_format13;
    else if(listFormat->currentItem()==15) /*2000/Feb/18*/
        tmpFormat=KSpreadCell::date_format14;
    else if(listFormat->currentItem()==16) /*2000-Feb-18*/
        tmpFormat=KSpreadCell::date_format15;
    else if(listFormat->currentItem()==17) /*2000-02-18*/
        tmpFormat=KSpreadCell::date_format16;
    else if( listFormat->currentItem()==18) /*2000-02-18*/
        tmpFormat=KSpreadCell::date_format17;
    tmp= util_dateFormat( dlg->locale(), dlg->m_date, tmpFormat);
    exampleLabel->setText(tmp);
}


void CellLayoutPageFloat::init()
{
    QStringList list;
    QString tmp;
    QString tmp2;
    QDate tmpDate( 2000,2,18);
    list+=i18n("System: ")+dlg->locale()->formatDate(QDate::currentDate(),true);
    list+=i18n("System: ")+dlg->locale()->formatDate(QDate::currentDate(),false);
    /*18-Feb-00*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format1);
    /*18-Feb-1999*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format2);
    /*18-Feb*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format3);
    /*18-2*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format4);
    /*18/2/00*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format5);
    /*18/5/1999*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format6);
    /*Feb-99*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format7);
    /*February-99*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format8);
    /*February-1999*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format9);
    /*F-99*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format10);
    /*18/Feb*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format11);
    /*18/2*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format12);
    /*18/Feb/1999*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format13);
    /*2000/Feb/18*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format14);
    /*2000-Feb-18*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format15);
    /*2000-2-18*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format16);
    /*2 february 2000*/
    list+=util_dateFormat( dlg->locale(), tmpDate, KSpreadCell::date_format17);

    listFormat->insertStringList(list);
    if( cellFormatType==KSpreadCell::ShortDate )
        listFormat->setCurrentItem(0);
    else if(cellFormatType==KSpreadCell::TextDate)
        listFormat->setCurrentItem(1);
    else if(cellFormatType==KSpreadCell::date_format1)
        listFormat->setCurrentItem(2);
    else if(cellFormatType==KSpreadCell::date_format2)
        listFormat->setCurrentItem(3);
    else if(cellFormatType==KSpreadCell::date_format3)
        listFormat->setCurrentItem(4);
    else if(cellFormatType==KSpreadCell::date_format4)
        listFormat->setCurrentItem(5);
    else if(cellFormatType==KSpreadCell::date_format5)
        listFormat->setCurrentItem(6);
    else if(cellFormatType==KSpreadCell::date_format6)
        listFormat->setCurrentItem(7);
    else if(cellFormatType==KSpreadCell::date_format7)
        listFormat->setCurrentItem(8);
    else if(cellFormatType==KSpreadCell::date_format8)
        listFormat->setCurrentItem(9);
    else if(cellFormatType==KSpreadCell::date_format9)
        listFormat->setCurrentItem(10);
    else if(cellFormatType==KSpreadCell::date_format10)
        listFormat->setCurrentItem(11);
    else if(cellFormatType==KSpreadCell::date_format11)
        listFormat->setCurrentItem(12);
    else if(cellFormatType==KSpreadCell::date_format12)
        listFormat->setCurrentItem(13);
    else if(cellFormatType==KSpreadCell::date_format13)
        listFormat->setCurrentItem(14);
    else if(cellFormatType==KSpreadCell::date_format14)
        listFormat->setCurrentItem(15);
    else if(cellFormatType==KSpreadCell::date_format15)
        listFormat->setCurrentItem(16);
    else if(cellFormatType==KSpreadCell::date_format16)
        listFormat->setCurrentItem(17);
    else if(cellFormatType==KSpreadCell::date_format17)
        listFormat->setCurrentItem(18);
    else
        listFormat->setCurrentItem(0);

}

void CellLayoutPageFloat::makeTimeFormat()
{
    QString tmp;
    QString tmpTime;
    KSpreadCell::FormatType tmpFormat=KSpreadCell::Time;
    if( listFormat->currentItem()==0)
        tmpFormat=KSpreadCell::Time;
    else if(listFormat->currentItem()==1)
        tmpFormat=KSpreadCell::SecondeTime;
    else if(listFormat->currentItem()==2)
        tmpFormat=KSpreadCell::Time_format1;
    else if(listFormat->currentItem()==3)
        tmpFormat=KSpreadCell::Time_format2;
    else if(listFormat->currentItem()==4)
        tmpFormat=KSpreadCell::Time_format3;
    tmp= util_timeFormat( dlg->locale(), dlg->m_time, tmpFormat);
    exampleLabel->setText(tmp);
}

QString CellLayoutPageFloat::makeFractionFormat()
{
    double result = (dlg->m_value)-floor(dlg->m_value);
    QString tmp;
    if(result == 0 )
    {
        return tmp.setNum( dlg->m_value );
    }
    else
    {
        KSpreadCell::FormatType tmpFormat=KSpreadCell::fraction_half;
        switch( listFormat->currentItem())
        {
            case 0:
                tmpFormat=KSpreadCell::fraction_half;
                break;
            case 1:
                tmpFormat=KSpreadCell::fraction_quarter;
                break;
            case 2:
                tmpFormat=KSpreadCell::fraction_eighth;
                break;
            case 3:
                tmpFormat=KSpreadCell::fraction_sixteenth;
                break;
            case 4:
                tmpFormat=KSpreadCell::fraction_tenth;
                break;
            case 5:
                tmpFormat=KSpreadCell::fraction_hundredth;
                break;
            case 6:
                tmpFormat=KSpreadCell::fraction_one_digit;
                break;
            case 7:
                tmpFormat=KSpreadCell::fraction_two_digits;
                break;
            case 8:
                tmpFormat=KSpreadCell::fraction_three_digits;
                break;
        }
        return util_fractionFormat( dlg->m_value,tmpFormat);
    }
}

void CellLayoutPageFloat::makeformat()
{
    m_bFormatTypeChanged=true;
    QString tmp;
    int p;
    p = (precision->value() == -1) ? 8 : precision->value();
    QChar decimal_point= dlg->locale()->decimalSymbol()[0];
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
                    makeTimeFormat();
                }
            else
                exampleLabel->setText(dlg->cellText);
        }
    else if(dlg->m_bValue)
        {
            if(number->isChecked())
                tmp=dlg->locale()->formatNumber(dlg->m_value,p );
            else if(money->isChecked())
                tmp=dlg->locale()->formatMoney(dlg->m_value,dlg->locale()->currencySymbol(),p );
            else if(percent->isChecked())
                tmp=dlg->locale()->formatNumber(dlg->m_value*100.0, p)+ " %";
            else if(scientific->isChecked())
                {
                    tmp=QString::number(dlg->m_value, 'E', p);
                    int pos;
                    if((pos=tmp.find('.'))!=-1)
                        tmp=tmp.replace(pos,1,decimal_point);
                }
            else if(fraction->isChecked())
                {
                    tmp=makeFractionFormat();
                }
	    else if(textFormat->isChecked())
	      {
		tmp=QString::number(dlg->m_value);
	      }
            if ( precision->value() == -1 && tmp.find(decimal_point) >= 0 && !textFormat->isChecked())
                {
                    int start=0;
                    if(tmp.find('%')!=-1)
                        start=2;
                    else if(tmp.find(dlg->locale()->currencySymbol())==((int)(tmp.length()-dlg->locale()->currencySymbol().length())))
                        start=dlg->locale()->currencySymbol().length()+1;
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
        exampleLabel->setText(i18n("Error"));
}


void CellLayoutPageFloat::applyLayout( KSpreadLayout *_obj )
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

    if(m_bFormatColorChanged)
        {
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
    if(m_bFormatTypeChanged)
        {
            _obj->setFactor(1.0);
            if(number->isChecked())
                _obj->setFormatType(KSpreadCell::Number);
            else if(percent->isChecked())
                {
                    _obj->setFormatType(KSpreadCell::Percentage);
                    _obj->setFactor(100.0);
                }
            else if(fraction->isChecked())
                {
                    if( listFormat->currentItem()==0)
                        _obj->setFormatType(KSpreadCell::fraction_half);
                    else if( listFormat->currentItem()==1)
                        _obj->setFormatType(KSpreadCell::fraction_quarter);
                    else if( listFormat->currentItem()==2)
                        _obj->setFormatType(KSpreadCell::fraction_eighth);
                    else if( listFormat->currentItem()==3)
                        _obj->setFormatType(KSpreadCell::fraction_sixteenth);
                    else if( listFormat->currentItem()==4)
                        _obj->setFormatType(KSpreadCell::fraction_tenth);
                    else if( listFormat->currentItem()==5)
                        _obj->setFormatType(KSpreadCell::fraction_hundredth);
                    else if( listFormat->currentItem()==6)
                        _obj->setFormatType(KSpreadCell::fraction_one_digit);
                    else if( listFormat->currentItem()==7)
                        _obj->setFormatType(KSpreadCell::fraction_two_digits);
                    else if( listFormat->currentItem()==8)
                        _obj->setFormatType(KSpreadCell::fraction_three_digits);
                }
            else if(date->isChecked())
                {
                    if( listFormat->currentItem()==0)
                        _obj->setFormatType(KSpreadCell::ShortDate );
                    else if(listFormat->currentItem()==1)
                        _obj->setFormatType(KSpreadCell::TextDate );
                    else if(listFormat->currentItem()==2)
                        _obj->setFormatType(KSpreadCell::date_format1 );
                    else if(listFormat->currentItem()==3)
                        _obj->setFormatType(KSpreadCell::date_format2 );
                    else if(listFormat->currentItem()==4)
                        _obj->setFormatType(KSpreadCell::date_format3 );
                    else if(listFormat->currentItem()==5)
                        _obj->setFormatType(KSpreadCell::date_format4 );
                    else if(listFormat->currentItem()==6)
                        _obj->setFormatType(KSpreadCell::date_format5 );
                    else if(listFormat->currentItem()==7)
                        _obj->setFormatType(KSpreadCell::date_format6 );
                    else if(listFormat->currentItem()==8)
                        _obj->setFormatType(KSpreadCell::date_format7 );
                    else if(listFormat->currentItem()==9)
                        _obj->setFormatType(KSpreadCell::date_format8 );
                    else if(listFormat->currentItem()==10)
                        _obj->setFormatType(KSpreadCell::date_format9 );
                    else if(listFormat->currentItem()==11)
                        _obj->setFormatType(KSpreadCell::date_format10 );
                    else if(listFormat->currentItem()==12)
                        _obj->setFormatType(KSpreadCell::date_format11 );
                    else if(listFormat->currentItem()==13)
                        _obj->setFormatType(KSpreadCell::date_format12 );
                    else if(listFormat->currentItem()==14)
                        _obj->setFormatType(KSpreadCell::date_format13 );
                    else if(listFormat->currentItem()==15)
                        _obj->setFormatType(KSpreadCell::date_format14 );
                    else if(listFormat->currentItem()==16)
                        _obj->setFormatType(KSpreadCell::date_format15 );
                    else if(listFormat->currentItem()==17)
                        _obj->setFormatType(KSpreadCell::date_format16 );
                    else if(listFormat->currentItem()==18)
                        _obj->setFormatType(KSpreadCell::date_format17 );
                }
            else if(time->isChecked())
                {
                    if( listFormat->currentItem()==0)
                        _obj->setFormatType(KSpreadCell::Time );
                    else if(listFormat->currentItem()==1)
                        _obj->setFormatType(KSpreadCell::SecondeTime );
                    else if(listFormat->currentItem()==2)
                        _obj->setFormatType(KSpreadCell::Time_format1 );
                    else if(listFormat->currentItem()==3)
                        _obj->setFormatType(KSpreadCell::Time_format2 );
                    else if(listFormat->currentItem()==4)
                        _obj->setFormatType(KSpreadCell::Time_format3 );
                }
            else if(money->isChecked())
                _obj->setFormatType(KSpreadCell::Money);
            else if(scientific->isChecked())
                _obj->setFormatType(KSpreadCell::Scientific);
	    else if(textFormat->isChecked())
	      _obj->setFormatType(KSpreadCell::Text_format);
        }
}

void CellLayoutPageFloat::apply( KSpreadCell *_obj )
{
    applyLayout(_obj);
}

void CellLayoutPageFloat::apply( RowLayout *_obj )
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c = NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for( c = table->getFirstCellRow(row); c != NULL;
         c = table->getNextCellRight(c->column(), c->row()) )
    {
      if ( dlg->precision != precision->value() )
      {
        c->clearProperty(KSpreadCell::PPrecision);
        c->clearNoFallBackProperties( KSpreadCell::PPrecision );
      }
      if ( postfix->text() != dlg->postfix )
      {
        if ( postfix->text() != "########" )
        {
          c->clearProperty(KSpreadCell::PPostfix);
          c->clearNoFallBackProperties( KSpreadCell::PPostfix );
        }
      }
      if ( prefix->text() != dlg->prefix )
      {
        if ( postfix->text() != "########" )
        {
          c->clearProperty(KSpreadCell::PPrefix);
          c->clearNoFallBackProperties( KSpreadCell::PPrefix );
        }
      }
      if(m_bFormatColorChanged)
      {
        c->clearProperty(KSpreadCell::PFloatFormat);
        c->clearNoFallBackProperties( KSpreadCell::PFloatFormat );
        c->clearProperty(KSpreadCell::PFloatColor);
        c->clearNoFallBackProperties( KSpreadCell::PFloatColor );
      }
      if(m_bFormatTypeChanged)
      {
        c->clearProperty(KSpreadCell::PFormatType);
        c->clearNoFallBackProperties( KSpreadCell::PFormatType );
        c->clearProperty(KSpreadCell::PFactor);
        c->clearNoFallBackProperties( KSpreadCell::PFactor );
      }
    }
  }
  applyLayout(_obj);
}

void CellLayoutPageFloat::apply( ColumnLayout *_obj )
{
  KSpreadTable *table = dlg->getTable();
  KSpreadCell* c = NULL;
  for (int col = dlg->left; col <= dlg->right; col++)
  {
    for( c = table->getFirstCellColumn(col); c != NULL;
         c = table->getNextCellDown(c->column(), c->row()) )
    {
      if ( dlg->precision != precision->value() )
      {
        c->clearProperty(KSpreadCell::PPrecision);
        c->clearNoFallBackProperties( KSpreadCell::PPrecision );
      }
      if ( postfix->text() != dlg->postfix )
      {
        if ( postfix->text() != "########" )
        {
          c->clearProperty(KSpreadCell::PPostfix);
          c->clearNoFallBackProperties( KSpreadCell::PPostfix );
        }
      }
      if ( prefix->text() != dlg->prefix )
      {
        if ( prefix->text() != "########" )
        {
          c->clearProperty(KSpreadCell::PPrefix);
          c->clearNoFallBackProperties( KSpreadCell::PPrefix );
        }
      }
      if(m_bFormatColorChanged)
      {
        c->clearProperty(KSpreadCell::PFloatFormat);
        c->clearNoFallBackProperties( KSpreadCell::PFloatFormat );
        c->clearProperty(KSpreadCell::PFloatColor);
        c->clearNoFallBackProperties( KSpreadCell::PFloatColor );
      }
      if(m_bFormatTypeChanged)
      {
        c->clearProperty(KSpreadCell::PFormatType);
        c->clearNoFallBackProperties( KSpreadCell::PFormatType );
        c->clearProperty(KSpreadCell::PFactor);
        c->clearNoFallBackProperties( KSpreadCell::PFactor );
      }
    }
  }
  applyLayout(_obj);

  RowLayout* rw =dlg->getTable()->firstRow();
  for( ; rw; rw = rw->next() )
  {
    if ( !rw->isDefault() &&
         (rw->hasProperty(KSpreadCell::PPrecision) ||
          rw->hasProperty(KSpreadCell::PPostfix) ||
          rw->hasProperty(KSpreadCell::PPrefix) ||
          rw->hasProperty(KSpreadCell::PFloatFormat) ||
          rw->hasProperty(KSpreadCell::PFloatColor) ||
          rw->hasProperty(KSpreadCell::PFormatType) ||
          rw->hasProperty(KSpreadCell::PFactor) ))
    {
      for(int i=dlg->left;i<=dlg->right;i++)
      {
        KSpreadCell *cell = dlg->getTable()->nonDefaultCell( i, rw->row() );
        applyLayout(cell );
      }
    }
  }
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

    box = new QGroupBox( this, "Box1");
    grid = new QGridLayout(box,4,1,7,7);
    dontPrintText= new QCheckBox(i18n("Don't print text"),box);
    dontPrintText->setChecked(dlg->bDontprintText);

    grid->addWidget(dontPrintText,0,0);
    layout->addWidget(box,1,0);


    this->resize( 400, 400 );
}

void CellLayoutPageMisc::apply( KSpreadCell *_obj )
{
 applyLayout(_obj);
}

void CellLayoutPageMisc::applyColumn( )
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c = NULL;
  for (int col = dlg->left; col <= dlg->right; col++)
  {
    for( c = table->getFirstCellColumn(col); c != NULL;
         c = table->getNextCellDown(c->column(), c->row()) )
      {
        applyLayout(c);
      }
  }
}

void CellLayoutPageMisc::applyRow( )
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c= NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for( c = table->getFirstCellRow(row); c != NULL;
         c = table->getNextCellRight(c->column(), c->row()) )
    {
      applyLayout(c);
    }
  }
}

KSpreadCell::Style CellLayoutPageMisc::getStyle()
{
  switch(styleButton->currentItem())
    {
    case 0:
      return KSpreadCell::ST_Normal;
    case 1:
      return KSpreadCell::ST_Button;
    case 2:
      return  KSpreadCell::ST_Select;
    case 3 :
      return  KSpreadCell::ST_Undef;
    default :
      return KSpreadCell::ST_Normal;
    }
}

void CellLayoutPageMisc::applyLayout( KSpreadCell *_obj )
{
    if ( styleButton->currentItem() == idStyleNormal )
      _obj->setStyle( KSpreadCell::ST_Normal );
    else if ( styleButton->currentItem() == idStyleButton )
      _obj->setStyle( KSpreadCell::ST_Button );
    else if ( styleButton->currentItem() == idStyleSelect )
      _obj->setStyle( KSpreadCell::ST_Select );
    if ( actionText->isEnabled() )
      _obj->setAction( actionText->text() );
    if( dlg->bDontprintText!=dontPrintText->isChecked())
      _obj->setDontPrintText(dontPrintText->isChecked());
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
  tmpQLabel->setText( i18n("Text color:") );
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
  delete fontDataBase;

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
  weight_combo->insertItem( i18n("Normal") );
  weight_combo->insertItem( i18n("Bold") );

  weight_combo->setInsertionPolicy(QComboBox::NoInsertion);
  grid2->addWidget(weight_combo,3,1);
  connect( weight_combo, SIGNAL(activated(const QString &)),
           SLOT(weight_chosen_slot(const QString &)) );

  style_combo = new QComboBox( box1, "Style" );
  style_combo->insertItem( "", 0 );
  style_combo->insertItem( i18n("Roman") );
  style_combo->insertItem( i18n("Italic"), 2 );
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
  fontChanged=false;
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
bTextColorUndefined=false;
}

void CellLayoutPageFont::apply( ColumnLayout *_obj)
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c= NULL;
  for (int col = dlg->left; col <= dlg->right; col++)
  {
    for( c = table->getFirstCellColumn(col); c != NULL;
         c = table->getNextCellDown(c->column(), c->row()) )
    {
      if ( !bTextColorUndefined )
      {
        c->clearProperty(KSpreadCell::PTextPen);
        c->clearNoFallBackProperties( KSpreadCell::PTextPen );
      }
      if(fontChanged)
      {
        c->clearProperty(KSpreadCell::PFont);
        c->clearNoFallBackProperties( KSpreadCell::PFont );
      }
    }
  }

  applyLayout(_obj);
  RowLayout* rw =dlg->getTable()->firstRow();
  for( ; rw; rw = rw->next() )
  {
    if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PFont) ))
    {
      for(int i=dlg->left;i<=dlg->right;i++)
      {
        KSpreadCell *cell = dlg->getTable()->nonDefaultCell( i, rw->row() );
        applyLayout(cell );
      }
    }
  }
}

void CellLayoutPageFont::apply( RowLayout *_obj)
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c= NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for( c = table->getFirstCellRow(row); c != NULL;
         c = table->getNextCellRight(c->column(), c->row()) )
    {
      if ( !bTextColorUndefined )
      {
        c->clearProperty(KSpreadCell::PTextPen);
        c->clearNoFallBackProperties( KSpreadCell::PTextPen );
      }
      if(fontChanged)
      {
        c->clearProperty(KSpreadCell::PFont);
        c->clearNoFallBackProperties( KSpreadCell::PFont );
      }
    }
  }
  applyLayout(_obj);
}


void CellLayoutPageFont::apply( KSpreadCell *_obj )
{
  applyLayout(_obj);
}

void CellLayoutPageFont::applyLayout( KSpreadLayout *_obj )
{
  if ( !bTextColorUndefined )
    _obj->setTextColor( textColor );
  if(fontChanged)
  {
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
  fontChanged=true;
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

 if(!textColor.isValid())
     textColor =colorGroup().text();

 textColorButton->setColor( textColor );


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

    grp = new QButtonGroup( i18n("Text Option"),this);
    //grp->setRadioButtonExclusive( false );
    grid2 = new QGridLayout(grp,2,1,15,7);
    multi = new QCheckBox( i18n("Go to line automatically"),grp);

    grid2->addWidget(multi,0,0);
    multi->setChecked(dlg->bMultiRow);

    vertical = new QCheckBox( i18n("Vertical text"),grp);
    grid2->addWidget(vertical,1,0);
    vertical->setChecked(dlg->bVerticalText);

    grid3->addWidget(grp,1,0);

    grp = new QButtonGroup( i18n("Rotation"),this);

    grid2 = new QGridLayout(grp,1,1,15,7);
    angleRotation=new KIntNumInput((-dlg->textRotation), grp, 10);
    angleRotation->setLabel(i18n("Angle:"));
    angleRotation->setRange(-90, 90, 1);
    angleRotation->setSuffix(" °");

    grid2->addWidget(angleRotation,0,0);
    grid3->addWidget(grp,1,1);
    if(dlg->textRotation!=0)
      {
        multi->setEnabled(false);
	vertical->setEnabled(false);
      }

    grp = new QButtonGroup( i18n("Merge Cells"),this);
    grid2 = new QGridLayout(grp,1,1,15,7);
    mergeCell=new QCheckBox(i18n("Merge cells"),grp);
    mergeCell->setChecked(dlg->isMerged);
    mergeCell->setEnabled(!dlg->oneCell && ((!dlg->isRowSelected) && (!dlg->isColumnSelected)));
    grid2->addWidget(mergeCell,0,0);
    grid3->addWidget(grp,2,0);

    grp = new QButtonGroup( i18n("Indent"),this);
    grid2 = new QGridLayout(grp,1,1,15,7);
    indent=new KIntNumInput(dlg->indent, grp, 10);
    indent->setRange(0, 400, 10);
    indent->setSuffix(i18n(" pt"));
    grid2->addWidget(indent,0,0);
    grid3->addWidget(grp,2,1);

    grp = new QButtonGroup( i18n("Size of Cell"),this);
    grid2 = new QGridLayout(grp,2,4,15,7);

    QLabel *tmpLabel=new QLabel(grp,"label");
    tmpLabel->setText(i18n("Width:"));
    grid2->addWidget(tmpLabel,0,0);

    width = new KDoubleNumInput( grp );
    width->setRange( KoUnit::ptToUnit( 2.0, dlg->getTable()->doc()->getUnit()), KoUnit::ptToUnit( 400.0, dlg->getTable()->doc()->getUnit()), KoUnit::ptToUnit( 1.0, dlg->getTable()->doc()->getUnit()) );
    width->setPrecision ( 2 );
    width->setValue ( KoUnit::ptToUnit( dlg->widthSize, dlg->getTable()->doc()->getUnit() ) );
    //to ensure, that we don't get rounding problems, we store the displayed value (for later check for changes)
    dlg->widthSize = KoUnit::ptFromUnit( width->value(), dlg->getTable()->doc()->getUnit() );

    if( dlg->isRowSelected )
        width->setEnabled(false);

    grid2->addWidget(width,0,1);
    defaultWidth=new QCheckBox(i18n("Default width (%1 %2)").arg(KoUnit::ptToUnit( 60, dlg->getTable()->doc()->getUnit())).arg(dlg->getTable()->doc()->getUnitName()),grp);
    if( dlg->isRowSelected )
        defaultWidth->setEnabled(false);

    grid2->addMultiCellWidget(defaultWidth,1,1,0,1);

    tmpLabel=new QLabel(grp,"label1");
    tmpLabel->setText(i18n("Height:"));
    grid2->addWidget(tmpLabel,0,2);

    height=new KDoubleNumInput( grp );
    height->setRange( KoUnit::ptToUnit( 2.0, dlg->getTable()->doc()->getUnit()), KoUnit::ptToUnit( 400.0, dlg->getTable()->doc()->getUnit()), KoUnit::ptToUnit( 1.0, dlg->getTable()->doc()->getUnit()) );
    height->setPrecision( 2 );
    height->setValue( KoUnit::ptToUnit( dlg->heigthSize, dlg->getTable()->doc()->getUnit() ) );
    //to ensure, that we don't get rounding problems, we store the displayed value (for later check for changes)
    dlg->heigthSize = KoUnit::ptFromUnit( height->value(), dlg->getTable()->doc()->getUnit() );

    if( dlg->isColumnSelected )
        height->setEnabled(false);

    grid2->addWidget(height,0,3);

    defaultHeight=new QCheckBox(i18n("Default height (%1 %2)").arg(KoUnit::ptToUnit(  20 , dlg->getTable()->doc()->getUnit())).arg(dlg->getTable()->doc()->getUnitName()),grp);
    if( dlg->isColumnSelected )
        defaultHeight->setEnabled(false);

    grid2->addMultiCellWidget(defaultHeight,1,1,2,3);

    grid3->addMultiCellWidget(grp,3,3,0,1);

    connect(defaultWidth , SIGNAL(clicked() ),this, SLOT(slotChangeWidthState()));
    connect(defaultHeight , SIGNAL(clicked() ),this, SLOT(slotChangeHeightState()));
    connect(vertical , SIGNAL(clicked() ),this, SLOT(slotChangeVerticalState()));
    connect(multi , SIGNAL(clicked() ),this, SLOT(slotChangeMultiState()));
    connect(angleRotation, SIGNAL(valueChanged(int)),this,SLOT(slotChangeAngle(int)));
    slotStateChanged(0);
    m_bOptionText=false;
    this->resize( 400, 400 );
}

void CellLayoutPagePosition::slotChangeMultiState()
{
    m_bOptionText=true;
    if(vertical->isChecked())
    {
        vertical->setChecked(false);
    }
}

void CellLayoutPagePosition::slotChangeVerticalState()
{
    m_bOptionText=true;
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

void CellLayoutPagePosition::apply( ColumnLayout *_obj )
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c= NULL;
  for (int col = dlg->left; col <= dlg->right; col++)
  {
    for( c = table->getFirstCellColumn(col); c != NULL;
         c = table->getNextCellDown(c->column(), c->row()) )
    {
      if(dlg->indent!=indent->value() && indent->isEnabled())
      {
        c->clearProperty(KSpreadCell::PIndent);
        c->clearNoFallBackProperties( KSpreadCell::PIndent );
      }
      c->clearProperty(KSpreadCell::PAlign);
      c->clearNoFallBackProperties( KSpreadCell::PAlign );
      c->clearProperty(KSpreadCell::PAlignY);
      c->clearNoFallBackProperties( KSpreadCell::PAlignY );
      if( m_bOptionText)
      {
        c->clearProperty(KSpreadCell::PMultiRow);
        c->clearNoFallBackProperties( KSpreadCell::PMultiRow );
      }

      if( m_bOptionText)
      {
        c->clearProperty(KSpreadCell::PVerticalText);
        c->clearNoFallBackProperties( KSpreadCell::PVerticalText );
      }

      if(dlg->textRotation!=angleRotation->value())
      {
        c->clearProperty(KSpreadCell::PAngle);
        c->clearNoFallBackProperties( KSpreadCell::PAngle );
      }
    }
  }
  applyLayout(_obj);

  RowLayout* rw =dlg->getTable()->firstRow();
  for( ; rw; rw = rw->next() )
  {
    if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PAngle) ||
                              rw->hasProperty(KSpreadCell::PVerticalText) ||
                              rw->hasProperty(KSpreadCell::PMultiRow) ||
                              rw->hasProperty(KSpreadCell::PAlignY) ||
                              rw->hasProperty(KSpreadCell::PAlign) ||
                              rw->hasProperty(KSpreadCell::PIndent) ))
    {
      for(int i=dlg->left;i<=dlg->right;i++)
      {
        KSpreadCell *cell = dlg->getTable()->nonDefaultCell( i, rw->row() );
        applyLayout(cell );
      }
    }
  }
}

void CellLayoutPagePosition::apply( RowLayout *_obj )
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c= NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for( c = table->getFirstCellRow(row); c != NULL;
         c = table->getNextCellRight(c->column(), c->row()) )
    {
      if(dlg->indent!=indent->value() && indent->isEnabled())
      {
        c->clearProperty(KSpreadCell::PIndent);
        c->clearNoFallBackProperties( KSpreadCell::PIndent );
      }
      c->clearProperty(KSpreadCell::PAlign);
      c->clearNoFallBackProperties( KSpreadCell::PAlign );
      c->clearProperty(KSpreadCell::PAlignY);
      c->clearNoFallBackProperties( KSpreadCell::PAlignY );

      if( m_bOptionText)
      {
        c->clearProperty(KSpreadCell::PMultiRow);
        c->clearNoFallBackProperties( KSpreadCell::PMultiRow );
      }
      if( m_bOptionText)
      {
        c->clearProperty(KSpreadCell::PVerticalText);
        c->clearNoFallBackProperties( KSpreadCell::PVerticalText );
      }
      if(dlg->textRotation!=angleRotation->value())
      {
        c->clearProperty(KSpreadCell::PAngle);
        c->clearNoFallBackProperties( KSpreadCell::PAngle );
      }
    }
  }
  applyLayout(_obj);
}


void CellLayoutPagePosition::apply( KSpreadCell *_obj )
{
  applyLayout(_obj);
}

void CellLayoutPagePosition::applyLayout( KSpreadLayout *_obj )
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

  if( m_bOptionText )
        {
        if(multi->isEnabled())
                _obj->setMultiRow(multi->isChecked());
        else
                _obj->setMultiRow(false);
        }

  if(m_bOptionText )
        {
        if(vertical->isEnabled())
                _obj->setVerticalText(vertical->isChecked());
        else
                _obj->setVerticalText(false);
        }

  if(dlg->textRotation!=angleRotation->value())
        _obj->setAngle((-angleRotation->value()));
  if(dlg->indent!=indent->value() && indent->isEnabled())
        _obj->setIndent(indent->value());
}

double CellLayoutPagePosition::getSizeHeight()
{
  if( defaultHeight->isChecked() )
      return 20.0;
  else
      return KoUnit::ptFromUnit( height->value(), dlg->getTable()->doc()->getUnit() );
}

double CellLayoutPagePosition::getSizeWidth()
{
  if( defaultWidth->isChecked() )
        return 60.0;
  else
        return KoUnit::ptFromUnit( width->value(), dlg->getTable()->doc()->getUnit() );
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
  pen=QPen( colorGroup().midlight(),2,SolidLine);
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

CellLayoutPageBorder::CellLayoutPageBorder( QWidget* parent, CellLayoutDlg *_dlg )
  : QWidget( parent ),
    dlg( _dlg )
{
  table = dlg->getTable();

  InitializeGrids();
  InitializeBorderButtons();
  InitializePatterns();
  SetConnections();

  preview->slotSelect();
  pattern[2]->slotSelect();

  style->setEnabled(false);
  size->setEnabled(false);
  preview->setPattern( black , 1, SolidLine );
  this->resize( 400, 400 );
}

void CellLayoutPageBorder::InitializeGrids()
{
  QGridLayout *grid = new QGridLayout(this,5,2,15,15);
  QGridLayout *grid2 = NULL;
  QGroupBox* tmpQGroupBox = NULL;

  /***********************/
  /* here is the data to initialize all the border buttons with */
  const char borderButtonNames[BorderType_END][20] =
    {"top", "bottom", "left", "right", "vertical", "fall", "go", "horizontal"};

  const char shortcutButtonNames[BorderShortcutType_END][20] =
    {"remove", "all", "outline"};

  QString borderButtonIconNames[BorderType_END] =
    {"border_top", "border_bottom", "border_left", "border_right",
     "border_vertical", "border_horizontal", "border_fall", "border_up"};

  QString shortcutButtonIconNames[BorderShortcutType_END] =
    { "border_remove", "", "border_outline"};

  int borderButtonPositions[BorderType_END][2] =
    {{0,2}, {4,2}, {2,0}, {2,4}, {4,4}, {4,0}, {0,0}, {0,4}};

  int shortcutButtonPositions[BorderShortcutType_END][2] =
    { {0,0}, {0,1},{0,2} };
  /***********************/


  /* set up a layout box for most of the border setting buttons */
  tmpQGroupBox = new QGroupBox( this, "GroupBox_1" );
  tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
  tmpQGroupBox->setTitle( i18n("Border") );
  tmpQGroupBox->setAlignment( AlignLeft );
  grid2 = new QGridLayout(tmpQGroupBox,5,5,15,7);
  area=new KSpreadBorder(tmpQGroupBox,"area",dlg->oneCol,dlg->oneRow);
  grid2->addMultiCellWidget(area,1,3,1,3);
  area->setBackgroundColor( colorGroup().base() );

  /* initailize the buttons that are in this box */
  for (int i=BorderType_Top; i < BorderType_END; i++)
  {
    borderButtons[i] = new KSpreadBorderButton(tmpQGroupBox,
                                               borderButtonNames[i]);
    loadIcon(borderButtonIconNames[i], borderButtons[i]);
    grid2->addWidget(borderButtons[i], borderButtonPositions[i][0],
                     borderButtonPositions[i][1]);
  }

  grid->addMultiCellWidget(tmpQGroupBox,0,2,0,0);

  /* the remove, all, and outline border buttons are in a second box down
     below.*/

  tmpQGroupBox = new QGroupBox( this, "GroupBox_3" );
  tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
  tmpQGroupBox->setTitle( i18n("Preselect") );
  tmpQGroupBox->setAlignment( AlignLeft );

  grid2 = new QGridLayout(tmpQGroupBox,1,3,15,7);

  /* the "all" button is different depending on what kind of region is currently
     selected */
  if((dlg->oneRow==true)&&(dlg->oneCol==false))
  {
    shortcutButtonIconNames[BorderShortcutType_All] = "border_vertical";
  }
  else if((dlg->oneRow==false)&&(dlg->oneCol==true))
  {
    shortcutButtonIconNames[BorderShortcutType_All] = "border_horizontal";
  }
  else
  {
    shortcutButtonIconNames[BorderShortcutType_All] = "border_inside";
  }

  for (int i=BorderShortcutType_Remove; i < BorderShortcutType_END; i++)
  {
    shortcutButtons[i] = new KSpreadBorderButton(tmpQGroupBox,
                                                 shortcutButtonNames[i]);
    loadIcon(shortcutButtonIconNames[i], shortcutButtons[i]);
    grid2->addWidget(shortcutButtons[i], shortcutButtonPositions[i][0],
                     shortcutButtonPositions[i][1]);
  }

  if (dlg->oneRow && dlg->oneCol)
  {
    shortcutButtons[BorderShortcutType_All]->setEnabled(false);
  }

  grid->addMultiCellWidget(tmpQGroupBox,3,4,0,0);

  /* now set up the group box with the pattern selector */
  tmpQGroupBox = new QGroupBox( this, "GroupBox_10" );
  tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
  tmpQGroupBox->setTitle( i18n("Pattern") );
  tmpQGroupBox->setAlignment( AlignLeft );

  grid2 = new QGridLayout(tmpQGroupBox,6,2,15,7);

  char name[] = "PatternXX";
  Q_ASSERT(NUM_BORDER_PATTERNS < 100);

  for (int i=0; i < NUM_BORDER_PATTERNS; i++)
  {
    name[7] = '0' + (i+1) / 10;
    name[8] = '0' + (i+1) % 10;
    pattern[i] = new KSpreadPatternSelect( tmpQGroupBox, name );
    pattern[i]->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(pattern[i], i % 5, i / 5);
    /* this puts them in the pattern:
       1  6
       2  7
       3  8
       4  9
       5  10
    */
  }

  color = new KColorButton (tmpQGroupBox, "PushButton_1" );
  grid2->addWidget(color,6,1);

  QLabel *tmpQLabel = new QLabel( tmpQGroupBox, "Label_6" );
  tmpQLabel->setText( i18n("Color:") );
  grid2->addWidget(tmpQLabel,6,0);

  /* tack on the 'customize' border pattern selector */
  QGridLayout *grid3 = new QGridLayout( 2, 2, 7 );
  customize  = new QCheckBox(i18n("Customize"),tmpQGroupBox);
  grid3->addWidget(customize,0,0);
  connect( customize, SIGNAL( clicked()), SLOT(cutomize_chosen_slot()) );

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

  /* Now the preview box is put together */
  tmpQGroupBox = new QGroupBox(this, "GroupBox_4" );
  tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
  tmpQGroupBox->setTitle( i18n("Preview") );
  tmpQGroupBox->setAlignment( AlignLeft );

  grid2 = new QGridLayout(tmpQGroupBox,1,1,14,7);



  preview = new KSpreadPatternSelect( tmpQGroupBox, "Pattern_preview" );
  preview->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  grid2->addWidget(preview,0,0);

  grid->addWidget(tmpQGroupBox,4,1);
}

void CellLayoutPageBorder::InitializeBorderButtons()
{
  for (int i=BorderType_Top; i < BorderType_END; i++)
  {
    if(dlg->borders[i].style != Qt::NoPen ||
       !dlg->borders[i].bStyle )
    {
      /* the horozontil and vertical buttons might be disabled depending on what
         kind of area is selected so check that first. */
      if ((dlg->oneRow == TRUE && i == BorderType_Horizontal) ||
          (dlg->oneCol == TRUE && i == BorderType_Vertical))
      {
        borderButtons[i]->setEnabled(false);
      }
      else if ( dlg->borders[i].bColor && dlg->borders[i].bStyle  )
      {
        borderButtons[i]->setPenStyle(dlg->borders[i].style );
        borderButtons[i]->setPenWidth(dlg->borders[i].width);
        borderButtons[i]->setColor(dlg->borders[i].color);
        borderButtons[i]->setOn(true);
      }
      else
      {
        borderButtons[i]->setUndefined();
      }
    }
  }


}

void CellLayoutPageBorder::InitializePatterns()
{
  pattern[0]->setPattern( black, 1, DotLine );
  pattern[1]->setPattern( black, 1, DashLine );
  pattern[2]->setPattern( black, 1, SolidLine );
  pattern[3]->setPattern( black, 1, DashDotLine );
  pattern[4]->setPattern( black, 1, DashDotDotLine );
  pattern[5]->setPattern( black, 2, SolidLine );
  pattern[6]->setPattern( black, 3, SolidLine );
  pattern[7]->setPattern( black, 4, SolidLine );
  pattern[8]->setPattern( black, 5, SolidLine );
  pattern[9]->setPattern( black, 1, NoPen );

  slotSetColorButton( black );
}

void CellLayoutPageBorder::SetConnections()
{
  connect( color, SIGNAL( changed( const QColor & ) ),
           this, SLOT( slotSetColorButton( const QColor & ) ) );

  for (int i=0; i < NUM_BORDER_PATTERNS; i++)
  {
    connect( pattern[i], SIGNAL( clicked( KSpreadPatternSelect* ) ),
             this, SLOT( slotUnselect2( KSpreadPatternSelect* ) ) );
  }

  for (int i = BorderType_Top; i < BorderType_END; i++)
  {
    connect( borderButtons[i], SIGNAL( clicked (KSpreadBorderButton *) ),
             this, SLOT( changeState( KSpreadBorderButton *) ) );
  }

  for (int i = BorderShortcutType_Remove; i < BorderShortcutType_END; i++)
  {
    connect( shortcutButtons[i], SIGNAL( clicked(KSpreadBorderButton *) ),
             this, SLOT( preselect(KSpreadBorderButton *) ) );
  }

  connect( area ,SIGNAL( redraw()),this,SLOT(draw()));
  connect( area ,SIGNAL( choosearea(QMouseEvent * )),
           this,SLOT( slotPressEvent(QMouseEvent *)));

  connect( style, SIGNAL( activated(int)), this, SLOT(slotChangeStyle(int)));
  connect( size, SIGNAL( textChanged(const QString &)),
           this, SLOT(slotChangeStyle(const QString &)));
  connect( size ,SIGNAL( activated(int)), this, SLOT(slotChangeStyle(int)));
}

void CellLayoutPageBorder::cutomize_chosen_slot()
{
  if(customize->isChecked())
  {
    style->setEnabled(true);
    size->setEnabled(true);
    slotUnselect2( preview );
  }
  else
  {
    style->setEnabled(false);
    size->setEnabled(false);
    pattern[2]->slotSelect();
    preview->setPattern( black , 1, SolidLine );
  }
}

void CellLayoutPageBorder::slotChangeStyle(const QString &)
{
  /* if they try putting text in the size box, then erase the line */
  slotChangeStyle(0);
}

void CellLayoutPageBorder::slotChangeStyle(int)
{
  int index = style->currentItem();
  QString tmp;
  int penSize = size->currentText().toInt();
  if( !penSize)
  {
    preview->setPattern( preview->getColor(), penSize, NoPen );
  }
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
  pen=QPen( colorGroup().text(),1,_style);
  painter.begin( &pixmap );
  painter.fillRect( 0, 0, style->width(), 14, colorGroup().background() );
  painter.setPen( pen );
  painter.drawLine( 0, 7, style->width(), 7 );
  painter.end();
  return pixmap;
}

void CellLayoutPageBorder::loadIcon( QString _pix, KSpreadBorderButton *_button)
{
  _button->setPixmap( QPixmap( KSBarIcon(_pix) ) );
}

void CellLayoutPageBorder::applyOutline()
{
  if (borderButtons[BorderType_Horizontal]->isChanged())
  {
    applyHorizontalOutline();
  }
  if(borderButtons[BorderType_Vertical]->isChanged())
  {
    applyVerticalOutline();
  }
  if ( borderButtons[BorderType_Left]->isChanged() )
  {
    applyLeftOutline();
  }
  if ( borderButtons[BorderType_Right]->isChanged() )
  {
    applyRightOutline();
  }

  if ( borderButtons[BorderType_Top]->isChanged() )
  {
    applyTopOutline();
  }

  if ( borderButtons[BorderType_Bottom]->isChanged() )
  {
    applyBottomOutline();
  }

  if ( borderButtons[BorderType_RisingDiagonal]->isChanged() ||
       borderButtons[BorderType_FallingDiagonal]->isChanged() )
  {
    applyDiagonalOutline();
  }
}

void CellLayoutPageBorder::applyTopOutline()
{
  KSpreadBorderButton* top = borderButtons[BorderType_Top];

  QPen tmpPen( top->getColor(),top->getPenWidth(),top->getPenStyle());

  /* if a column is selected then _top will just be row 1 so there's no special
     handling */
  if( !dlg->isRowSelected )
  {
    for ( int x = dlg->left; x <= dlg->right; x++ )
    {
      KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x, dlg->top );
      obj->setTopBorderPen( tmpPen );
    }
  }
  else if( dlg->isRowSelected )
  {
    KSpreadCell* c = NULL;
    for( c = table->getFirstCellRow(dlg->top); c != NULL;
         c = table->getNextCellRight(c->column(), c->row()) )
    {
      c->clearProperty(KSpreadCell::PTopBorder);
      c->clearNoFallBackProperties( KSpreadCell::PTopBorder );
    }

    RowLayout *obj=dlg->getTable()->nonDefaultRowLayout(dlg->top-1);
    obj->setBottomBorderPen( tmpPen );
  }
}

void CellLayoutPageBorder::applyBottomOutline()
{
  KSpreadTable* table = dlg->getTable();
  KSpreadBorderButton* bottom = borderButtons[BorderType_Bottom];
  QPen tmpPen( bottom->getColor(),bottom->getPenWidth(),bottom->getPenStyle());
  if( !dlg->isRowSelected && !dlg->isColumnSelected )
  {
    for ( int x = dlg->left; x <= dlg->right; x++ )
    {
      KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x, dlg->bottom );
      obj->setBottomBorderPen( tmpPen );
    }
  }
  else if( dlg->isRowSelected )
  {
    KSpreadCell* c = NULL;
    for( c = table->getFirstCellRow(dlg->bottom); c != NULL;
         c = table->getNextCellRight(c->column(), c->row()) )
    {
      c->clearProperty(KSpreadCell::PBottomBorder);
      c->clearNoFallBackProperties( KSpreadCell::PBottomBorder );
    }

    RowLayout *obj=dlg->getTable()->nonDefaultRowLayout(dlg->bottom);
    obj->setBottomBorderPen( tmpPen );
  }
}

void CellLayoutPageBorder::applyLeftOutline()
{
  KSpreadBorderButton* left = borderButtons[BorderType_Left];
  QPen tmpPen( left->getColor(),left->getPenWidth(),left->getPenStyle());
  if( !dlg->isColumnSelected )
  {
    for ( int y = dlg->top; y <= dlg->bottom; y++ )
    {
      KSpreadCell *obj = dlg->getTable()->nonDefaultCell( dlg->left, y );
      obj->setLeftBorderPen( tmpPen );
    }
  }
  else
  {
    KSpreadCell* c = NULL;
    for( c = table->getFirstCellColumn(dlg->left); c != NULL;
         c = table->getNextCellDown(c->column(), c->row()) )
    {
      c->clearProperty(KSpreadCell::PLeftBorder);
      c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );
    }
    ColumnLayout *obj=dlg->getTable()->nonDefaultColumnLayout(dlg->left);
    obj->setLeftBorderPen( tmpPen );

    RowLayout* rw =dlg->getTable()->firstRow();
    for( ; rw; rw = rw->next() )
    {
      if (rw->row()==dlg->left&& !rw->isDefault() &&
          (rw->hasProperty(KSpreadCell::PLeftBorder)  ))
      {
        for(int i=dlg->left;i<=dlg->right;i++)
        {
          KSpreadCell *cell =
            dlg->getTable()->nonDefaultCell( i, rw->row() );
          cell->setLeftBorderPen( tmpPen );
        }
      }
    }
  }
}

void CellLayoutPageBorder::applyRightOutline()
{
  KSpreadBorderButton* right = borderButtons[BorderType_Right];
  QPen tmpPen( right->getColor(),right->getPenWidth(),right->getPenStyle());
  if( (!dlg->isRowSelected) && (!dlg->isColumnSelected) )
  {
    for ( int y = dlg->top; y <= dlg->bottom; y++ )
    {
      KSpreadCell *obj = dlg->getTable()->nonDefaultCell( dlg->right, y );
      obj->setRightBorderPen(tmpPen);
    }
  }
  else if(  dlg->isColumnSelected )
  {
    KSpreadCell* c = NULL;
    for( c = table->getFirstCellColumn(dlg->right); c != NULL;
         c = table->getNextCellDown(c->column(), c->row()) )
    {
      c->clearProperty(KSpreadCell::PRightBorder);
      c->clearNoFallBackProperties( KSpreadCell::PRightBorder );
    }

    ColumnLayout *obj=dlg->getTable()->nonDefaultColumnLayout(dlg->right);
    obj->setRightBorderPen(tmpPen);

    RowLayout* rw =dlg->getTable()->firstRow();
    for( ; rw; rw = rw->next() )
    {
      if (rw->row()==dlg->right&& !rw->isDefault() &&
          (rw->hasProperty(KSpreadCell::PRightBorder)  ))
      {
        for(int i=dlg->left;i<=dlg->right;i++)
        {
          KSpreadCell *cell =
            dlg->getTable()->nonDefaultCell( i, rw->row() );
          cell->setRightBorderPen(tmpPen);
        }
      }
    }
  }
}

void CellLayoutPageBorder::applyDiagonalOutline()
{
  KSpreadBorderButton* fallDiagonal = borderButtons[BorderType_FallingDiagonal];
  KSpreadBorderButton* goUpDiagonal = borderButtons[BorderType_RisingDiagonal];
  QPen tmpPenFall( fallDiagonal->getColor(), fallDiagonal->getPenWidth(),
                   fallDiagonal->getPenStyle());
  QPen tmpPenGoUp( goUpDiagonal->getColor(), goUpDiagonal->getPenWidth(),
                   goUpDiagonal->getPenStyle());
  if( (!dlg->isRowSelected) && (!dlg->isColumnSelected) )
  {
    for ( int x = dlg->left; x <= dlg->right; x++ )
    {
      for ( int y = dlg->top; y <= dlg->bottom; y++ )
      {
        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x, y );
        if ( fallDiagonal->isChanged() )
          obj->setFallDiagonalPen( tmpPenFall );
        if ( goUpDiagonal->isChanged() )
          obj->setGoUpDiagonalPen( tmpPenGoUp );
      }
    }
  }
  else if( dlg->isColumnSelected )
  {
    KSpreadCell* c = NULL;
    for (int col = dlg->left; col <= dlg->right; col++)
    {
      for (c = table->getFirstCellColumn(col); c != NULL;
           c = table->getNextCellDown(c->column(), c->row()))
      {
        if ( fallDiagonal->isChanged() )
        {
          c->clearProperty(KSpreadCell::PFallDiagonal);
          c->clearNoFallBackProperties( KSpreadCell::PFallDiagonal );
        }
        if ( goUpDiagonal->isChanged() )
        {
          c->clearProperty(KSpreadCell::PGoUpDiagonal);
          c->clearNoFallBackProperties( KSpreadCell::PGoUpDiagonal);
        }
      }

      ColumnLayout *obj=dlg->getTable()->nonDefaultColumnLayout(col);
      if ( fallDiagonal->isChanged() )
        obj->setFallDiagonalPen( tmpPenFall );
      if ( goUpDiagonal->isChanged() )
        obj->setGoUpDiagonalPen( tmpPenGoUp );
    }


    RowLayout* rw =dlg->getTable()->firstRow();
    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PFallDiagonal)
                                ||rw->hasProperty(KSpreadCell::PGoUpDiagonal) ))
      {
        for(int i=dlg->left;i<=dlg->right;i++)
        {
          KSpreadCell *cell =
            dlg->getTable()->nonDefaultCell( i, rw->row() );
          cell->setFallDiagonalPen( tmpPenFall );
          cell->setGoUpDiagonalPen( tmpPenGoUp );
        }
      }
    }
  }
  else if( dlg->isRowSelected )
  {
    KSpreadCell* c = NULL;
    for (int row = dlg->top; row <= dlg->bottom; row++)
    {
      for (c = table->getFirstCellRow(row); c != NULL;
           c = table->getNextCellRight(c->column(), c->row()))
      {
        if ( fallDiagonal->isChanged() )
        {
          c->clearProperty(KSpreadCell::PFallDiagonal);
          c->clearNoFallBackProperties( KSpreadCell::PFallDiagonal );
        }
        if ( goUpDiagonal->isChanged() )
        {
          c->clearProperty(KSpreadCell::PGoUpDiagonal);
          c->clearNoFallBackProperties( KSpreadCell::PGoUpDiagonal);
        }
      }

      RowLayout *obj=dlg->getTable()->nonDefaultRowLayout(row);
      if ( fallDiagonal->isChanged() )
        obj->setFallDiagonalPen( tmpPenFall );
      if ( goUpDiagonal->isChanged() )
        obj->setGoUpDiagonalPen( tmpPenGoUp );
    }
  }
}

void CellLayoutPageBorder::applyHorizontalOutline()
{
  QPen tmpPen( borderButtons[BorderType_Horizontal]->getColor(),
               borderButtons[BorderType_Horizontal]->getPenWidth(),
               borderButtons[BorderType_Horizontal]->getPenStyle());

  if( (!dlg->isRowSelected) && (!dlg->isColumnSelected) )
  {
    for ( int x = dlg->left; x <= dlg->right; x++ )
    {
      for ( int y = dlg->top+1; y <= dlg->bottom; y++ )
      {
        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x, y );
        obj->setTopBorderPen(tmpPen);
      }
    }
  }
  else if( dlg->isColumnSelected )
  {
    KSpreadCell* c = NULL;
    for (int col = dlg->left; col <= dlg->right; col++)
    {
      for (c = table->getFirstCellColumn(col); c != NULL;
           c = table->getNextCellDown(c->column(), c->row()))
      {
        c->clearProperty(KSpreadCell::PTopBorder);
        c->clearNoFallBackProperties( KSpreadCell::PTopBorder );
      }

      ColumnLayout *obj=dlg->getTable()->nonDefaultColumnLayout(col);
      obj->setTopBorderPen(tmpPen);
    }

    RowLayout* rw =dlg->getTable()->firstRow();
    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PTopBorder)  ))
      {
        for(int i=dlg->left;i<=dlg->right;i++)
        {
          KSpreadCell *cell =
            dlg->getTable()->nonDefaultCell( i, rw->row() );
          cell->setTopBorderPen(tmpPen);
        }
      }
    }
  }
  else if( dlg->isRowSelected )
  {
    KSpreadCell* c = NULL;
    for (int row = dlg->top + 1; row <= dlg->bottom; row++)
    {
      for (c = table->getFirstCellRow(row); c != NULL;
           c = table->getNextCellRight(c->column(), c->row()))
      {
        c->clearProperty(KSpreadCell::PTopBorder);
        c->clearNoFallBackProperties( KSpreadCell::PTopBorder );
      }

      RowLayout *obj = dlg->getTable()->nonDefaultRowLayout(row);
      obj->setTopBorderPen(tmpPen);
    }
  }
}

void CellLayoutPageBorder::applyVerticalOutline()
{
  KSpreadBorderButton* vertical = borderButtons[BorderType_Vertical];
  QPen tmpPen( vertical->getColor(), vertical->getPenWidth(),
               vertical->getPenStyle());
  if( (!dlg->isRowSelected) && (!dlg->isColumnSelected) )
  {
    for ( int x = dlg->left+1; x <= dlg->right; x++ )
    {
      for ( int y = dlg->top; y <= dlg->bottom; y++ )
      {
        KSpreadCell *obj = dlg->getTable()->nonDefaultCell( x, y );
        obj->setLeftBorderPen( tmpPen );
      }
    }
  }
  else if( dlg->isColumnSelected )
  {
    KSpreadCell* c = NULL;
    for (int col = dlg->left + 1; col <= dlg->right; col++)
    {
      for (c = table->getFirstCellColumn(col); c != NULL;
           c = table->getNextCellDown(c->column(), c->row()))
      {
        c->clearProperty(KSpreadCell::PLeftBorder);
        c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );
      }
      ColumnLayout *obj=dlg->getTable()->nonDefaultColumnLayout(col);
      obj->setLeftBorderPen( tmpPen );
    }

    RowLayout* rw =dlg->getTable()->firstRow();
    for( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PLeftBorder)  ))
      {
        for(int i=dlg->left + 1;i<=dlg->right;i++)
        {
          KSpreadCell *cell =
            dlg->getTable()->nonDefaultCell( i, rw->row() );
          cell->setLeftBorderPen( tmpPen );
        }
      }
    }
  }
  else if( dlg->isRowSelected )
  {
    KSpreadCell* c = NULL;
    for (int row = dlg->top; row <= dlg->bottom; row++)
    {
      for (c = table->getFirstCellRow(row); c != NULL;
           c = table->getNextCellRight(c->column(), c->row()))
      {
        c->clearProperty(KSpreadCell::PLeftBorder);
        c->clearNoFallBackProperties( KSpreadCell::PLeftBorder );
      }
      RowLayout *obj=dlg->getTable()->nonDefaultRowLayout(row);
      obj->setLeftBorderPen( tmpPen );
    }
  }
}


void CellLayoutPageBorder::slotSetColorButton( const QColor &_color )
{
    currentColor = _color;

    for (int i = 0; i < NUM_BORDER_PATTERNS; i++)
    {
      pattern[i]->setColor( currentColor );
    }
    preview->setColor( currentColor );

}

void CellLayoutPageBorder::slotUnselect2( KSpreadPatternSelect *_p )
{
    for (int i = 0; i < NUM_BORDER_PATTERNS; i++)
    {
      if (pattern[i] != _p)
      {
        pattern[i]->slotUnselect();
      }
    }
    preview->setPattern( _p->getColor(), _p->getPenWidth(), _p->getPenStyle() );
}

void CellLayoutPageBorder::preselect( KSpreadBorderButton *_p)
{
  KSpreadBorderButton* top = borderButtons[BorderType_Top];
  KSpreadBorderButton* bottom = borderButtons[BorderType_Bottom];
  KSpreadBorderButton* left = borderButtons[BorderType_Left];
  KSpreadBorderButton* right = borderButtons[BorderType_Right];
  KSpreadBorderButton* vertical = borderButtons[BorderType_Vertical];
  KSpreadBorderButton* horizontal = borderButtons[BorderType_Horizontal];
  KSpreadBorderButton* remove = shortcutButtons[BorderShortcutType_Remove];
  KSpreadBorderButton* outline = shortcutButtons[BorderShortcutType_Outline];
  KSpreadBorderButton* all = shortcutButtons[BorderShortcutType_All];

  _p->setOn(false);
  if(_p == remove)
  {
    for (int i=BorderType_Top; i < BorderType_END; i++)
    {
      if (borderButtons[i]->isOn())
      {
        borderButtons[i]->unselect();
      }
    }
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
  KSpreadBorderButton* top = borderButtons[BorderType_Top];
  KSpreadBorderButton* bottom = borderButtons[BorderType_Bottom];
  KSpreadBorderButton* left = borderButtons[BorderType_Left];
  KSpreadBorderButton* right = borderButtons[BorderType_Right];
  KSpreadBorderButton* risingDiagonal = borderButtons[BorderType_RisingDiagonal];
  KSpreadBorderButton* fallingDiagonal = borderButtons[BorderType_FallingDiagonal];
  KSpreadBorderButton* vertical = borderButtons[BorderType_Vertical];
  KSpreadBorderButton* horizontal = borderButtons[BorderType_Horizontal];
  QPen pen;
  QPainter painter;
  painter.begin( area );

  if((bottom->getPenStyle())!=Qt::NoPen)
  {
    pen=QPen( bottom->getColor(), bottom->getPenWidth(),bottom->getPenStyle());
    painter.setPen( pen );
    painter.drawLine( OFFSETX, area->height()-OFFSETY, area->width()-OFFSETX , area->height()-OFFSETY );
  }
  if((top->getPenStyle())!=Qt::NoPen)
  {
    pen=QPen( top->getColor(), top->getPenWidth(),top->getPenStyle());
    painter.setPen( pen );
    painter.drawLine( OFFSETX, OFFSETY, area->width() -OFFSETX, OFFSETY );
  }
 if((left->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( left->getColor(), left->getPenWidth(),left->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( OFFSETX, OFFSETY, OFFSETX , area->height()-OFFSETY );
 }
 if((right->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( right->getColor(), right->getPenWidth(),right->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( area->width()-OFFSETX, OFFSETY, area->width()-OFFSETX,
                     area->height()-OFFSETY );

 }
 if((fallingDiagonal->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( fallingDiagonal->getColor(), fallingDiagonal->getPenWidth(),
             fallingDiagonal->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( OFFSETX, OFFSETY, area->width()-OFFSETX,
                     area->height()-OFFSETY );
   if(dlg->oneCol==false&& dlg->oneRow==false)
   {
     painter.drawLine( area->width()/2, OFFSETY, area->width()-OFFSETX,
                       area->height()/2 );
     painter.drawLine( OFFSETX,area->height()/2 , area->width()/2,
                       area->height()-OFFSETY );
   }
 }
 if((risingDiagonal->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( risingDiagonal->getColor(), risingDiagonal->getPenWidth(),
             risingDiagonal->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( OFFSETX, area->height()-OFFSETY , area->width()-OFFSETX ,
                     OFFSETY );
   if(dlg->oneCol==false&& dlg->oneRow==false)
   {
     painter.drawLine( area->width()/2, OFFSETY, OFFSETX, area->height()/2 );
     painter.drawLine( area->width()/2,area->height()-OFFSETY ,
                       area->width()-OFFSETX, area->height()/2 );
   }

 }
 if((vertical->getPenStyle())!=Qt::NoPen)
    {
      pen=QPen( vertical->getColor(), vertical->getPenWidth(),
                vertical->getPenStyle());
      painter.setPen( pen );
      painter.drawLine( area->width()/2, 5 , area->width()/2 , area->height()-5 );
    }
 if((horizontal->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( horizontal->getColor(), horizontal->getPenWidth(),
             horizontal->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( OFFSETX,area->height()/2,area->width()-OFFSETX,
                     area->height()/2 );
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
    _p->setOn(true);
    _p->setPenWidth(preview->getPenWidth());
    _p->setPenStyle(preview->getPenStyle());
    _p->setColor( currentColor );
    _p->setChanged(true);
  }
}

void CellLayoutPageBorder::slotPressEvent(QMouseEvent *_ev)
{
  KSpreadBorderButton* top = borderButtons[BorderType_Top];
  KSpreadBorderButton* bottom = borderButtons[BorderType_Bottom];
  KSpreadBorderButton* left = borderButtons[BorderType_Left];
  KSpreadBorderButton* right = borderButtons[BorderType_Right];
  KSpreadBorderButton* vertical = borderButtons[BorderType_Vertical];
  KSpreadBorderButton* horizontal = borderButtons[BorderType_Horizontal];


  QRect rect(OFFSETX,OFFSETY-8,area->width()-OFFSETX,OFFSETY+8);
  if(rect.contains(QPoint(_ev->x(),_ev->y())))
  {
    if(((top->getPenWidth()!=preview->getPenWidth()) ||
        (top->getColor()!=currentColor) ||
        (top->getPenStyle()!=preview->getPenStyle()))
       && top->isOn())
    {
      top->setPenWidth(preview->getPenWidth());
      top->setPenStyle(preview->getPenStyle());
      top->setColor( currentColor );
      top->setChanged(true);
    }
    else
      invertState(top);
  }
  rect.setCoords(OFFSETX,area->height()-OFFSETY-8,area->width()-OFFSETX,
                 area->height()-OFFSETY+8);
  if(rect.contains(QPoint(_ev->x(),_ev->y())))
  {
    if(((bottom->getPenWidth()!=preview->getPenWidth()) ||
        (bottom->getColor()!=currentColor) ||
        (bottom->getPenStyle()!=preview->getPenStyle()))
       && bottom->isOn())
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
    if(((left->getPenWidth()!=preview->getPenWidth()) ||
        (left->getColor()!=currentColor) ||
        (left->getPenStyle()!=preview->getPenStyle()))
       && left->isOn())
    {
      left->setPenWidth(preview->getPenWidth());
      left->setPenStyle(preview->getPenStyle());
      left->setColor( currentColor );
                left->setChanged(true);
    }
    else
      invertState(left);
  }
  rect.setCoords(area->width()-OFFSETX-8,OFFSETY,area->width()-OFFSETX+8,
                 area->height()-OFFSETY);
  if(rect.contains(QPoint(_ev->x(),_ev->y())))
  {
    if(((right->getPenWidth()!=preview->getPenWidth()) ||
        (right->getColor()!=currentColor) ||
        (right->getPenStyle()!=preview->getPenStyle()))
       && right->isOn())
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
    rect.setCoords(area->width()/2-8,OFFSETY,area->width()/2+8,
                   area->height()-OFFSETY);

    if(rect.contains(QPoint(_ev->x(),_ev->y())))
    {
      if(((vertical->getPenWidth()!=preview->getPenWidth()) ||
          (vertical->getColor()!=currentColor) ||
          (vertical->getPenStyle()!=preview->getPenStyle()))
         && vertical->isOn())
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
    rect.setCoords(OFFSETX,area->height()/2-8,area->width()-OFFSETX,
                   area->height()/2+8);
    if(rect.contains(QPoint(_ev->x(),_ev->y())))
    {
      if(((horizontal->getPenWidth()!=preview->getPenWidth()) ||
          (horizontal->getColor()!=currentColor) ||
          (horizontal->getPenStyle()!=preview->getPenStyle()))
         && horizontal->isOn())
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
    QBrush brush(brushColor,brushStyle);
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
    tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
    tmpQGroupBox->setTitle( i18n("Pattern") );
    tmpQGroupBox->setAlignment( AlignLeft );

    QGridLayout *grid2 = new QGridLayout(tmpQGroupBox,7,3,15,7);

    brush1 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_1" );
    brush1->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush1,0,0);

    brush2 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_2" );
    brush2->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush2,0,1);

    brush3 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_3" );
    brush3->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush3,0,2);

    brush4 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_4" );
    brush4->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush4,1,0);

    brush5 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_5" );
    brush5->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush5,1,1);

    brush6 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_6" );
    brush6->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush6,1,2);

    brush7 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_7" );
    brush7->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush7,2,0);

    brush8 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_8" );
    brush8->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush8,2,1);

    brush9 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_9" );
    brush9->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush9,2,2);

    brush10 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_10" );
    brush10->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush10,3,0);

    brush11 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_11" );
    brush11->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush11,3,1);

    brush12 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_12" );
    brush12->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush12,3,2);

    brush13 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_13" );
    brush13->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush13,4,0);

    brush14 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_14" );
    brush14->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush14,4,1);

    brush15 = new KSpreadBrushSelect( tmpQGroupBox, "Frame_15" );
    brush15->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush15,4,2);

    QGridLayout *grid3 = new QGridLayout( 1, 2 );
    color = new KColorButton (tmpQGroupBox, "ColorButton_1" );
    grid3->addWidget(color,0,1);

    QLabel *tmpQLabel = new QLabel( tmpQGroupBox, "Label_1" );
    tmpQLabel->setText( i18n("Color:") );
    grid3->addWidget(tmpQLabel,0,0);

    grid2->addMultiCell(grid3,5,5,0,2);

    grid3 = new QGridLayout( 1, 3 );

    tmpQLabel = new QLabel( tmpQGroupBox, "Label_2" );
    grid3->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText( i18n("Background color:") );

    bgColorButton = new KColorButton( tmpQGroupBox, "ColorButton" );
    grid3->addWidget(bgColorButton,0,1);
    if ( dlg->bBgColor )
        bgColor = dlg->bgColor;
    else
        bgColor = colorGroup().base();

    if(!bgColor.isValid())
        bgColor=colorGroup().base();

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
    tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
    tmpQGroupBox->setAlignment( AlignLeft );

    grid2 = new QGridLayout(tmpQGroupBox,1,1,14,4);

    current = new KSpreadBrushSelect( tmpQGroupBox, "Current" );
    current->setFrameStyle( QFrame::Panel | QFrame::Sunken );
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

void CellLayoutPagePattern::apply( ColumnLayout *_obj )
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c = NULL;
  for (int col = dlg->left; col <= dlg->right; col++)
  {
    for (c = table->getFirstCellColumn(col); c != NULL;
         c = table->getNextCellDown(c->column(), c->row()))
    {
      if(selectedBrush!=0L)
      {
        c->clearProperty(KSpreadCell::PBackgroundBrush);
        c->clearNoFallBackProperties( KSpreadCell::PBackgroundBrush );
      }
      if(!bBgColorUndefined || b_notAnyColor)
      {
        c->clearProperty(KSpreadCell::PBackgroundColor);
        c->clearNoFallBackProperties( KSpreadCell::PBackgroundColor );
      }
    }
  }
  applyLayout(_obj);

  RowLayout* rw =dlg->getTable()->firstRow();
  for( ; rw; rw = rw->next() )
  {
    if ( !rw->isDefault() && (rw->hasProperty(KSpreadCell::PBackgroundColor) || rw->hasProperty(KSpreadCell::PBackgroundBrush)))
    {
      for(int i=dlg->left;i<=dlg->right;i++)
      {
        KSpreadCell *cell =
          dlg->getTable()->nonDefaultCell( i, rw->row() );
        applyLayout(cell );
      }
    }
  }

}

void CellLayoutPagePattern::apply( RowLayout *_obj )
{
  KSpreadTable* table = dlg->getTable();
  KSpreadCell* c = NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for (c = table->getFirstCellRow(row); c != NULL;
         c = table->getNextCellRight(c->column(), c->row()))
    {
      if(selectedBrush!=0L)
      {
        c->clearProperty(KSpreadCell::PBackgroundBrush);
        c->clearNoFallBackProperties( KSpreadCell::PBackgroundBrush );
      }
      if(!bBgColorUndefined || b_notAnyColor)
      {
        c->clearProperty(KSpreadCell::PBackgroundColor);
        c->clearNoFallBackProperties( KSpreadCell::PBackgroundColor );
      }
    }
  }
  applyLayout(_obj);
}


void CellLayoutPagePattern::apply( KSpreadCell *_obj )
{
  applyLayout(_obj);
}

void CellLayoutPagePattern::applyLayout( KSpreadLayout *_obj )
{
  if(selectedBrush!=0L)
        _obj->setBackGroundBrush(QBrush( selectedBrush->getBrushColor(),selectedBrush->getBrushStyle()));
  if( b_notAnyColor)
        _obj->setBgColor( QColor() );
  else if ( !bBgColorUndefined )
        _obj->setBgColor( bgColor );
}
#include "kspread_dlg_layout.moc"
