/* This file is part of the KDE project
   Copyright (C) 2004 Tomas Mecir <mecirt@gmail.com>
             (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 1999-2002 Harri Porten <porten@kde.org>
             (C) 2000-2001 David Faure <faure@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>
             (C) 2000 Werner Trobin <trobin@kde.org>
             (C) 1999 Reginald Stadlbauer <reggie@kde.org>
             (C) 1998-1999 Stephan Kulow <coolo@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <stdlib.h>
#include <math.h>

#include "kspread_canvas.h"
#include "kspread_dlg_layout.h"
#include "kspread_locale.h"
#include "kspread_sheet.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "kspread_util.h"
#include "valueformatter.h"

#include <qbitmap.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qfontdatabase.h>
#include <qwhatsthis.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <knumvalidator.h>
#include <qradiobutton.h>
#include <klineedit.h>
#include <qcheckbox.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <koUnitWidgets.h>

using namespace KSpread;

PatternSelect::PatternSelect( QWidget *parent, const char * )
  : QFrame( parent )
{
    penStyle = NoPen;
    penWidth = 1;
    penColor = colorGroup().text();
    selected = false;
    undefined = false;
}

void PatternSelect::setPattern( const QColor &_color, int _width, PenStyle _style )
{
    penStyle = _style;
    penColor = _color;
    penWidth = _width;
    repaint();
}

void PatternSelect::setUndefined()
{
    undefined = true;
}

void PatternSelect::paintEvent( QPaintEvent *_ev )
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

void PatternSelect::mousePressEvent( QMouseEvent * )
{
    slotSelect();

    emit clicked( this );
}

void PatternSelect::slotUnselect()
{
    selected = false;

    setLineWidth( 1 );
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    repaint();
}

void PatternSelect::slotSelect()
{
    selected = true;

    setLineWidth( 2 );
    setFrameStyle( QFrame::Panel | QFrame::Plain );
    repaint();
}



GeneralTab::GeneralTab( QWidget* parent, CellFormatDialog * dlg )
  : QWidget( parent ),
    m_dlg( dlg )
{
  QGridLayout * layout = new QGridLayout( this, 1, 1, KDialog::marginHint(), KDialog::spacingHint(), "layout");

  QGroupBox * groupBox = new QGroupBox( this, "groupBox1" );
  groupBox->setColumnLayout(0, Qt::Vertical );
  groupBox->setTitle( i18n( "Style" ) );
  groupBox->layout()->setSpacing( KDialog::spacingHint() );
  groupBox->layout()->setMargin( KDialog::marginHint() );

  QGridLayout * groupBoxLayout = new QGridLayout( groupBox->layout() );
  groupBoxLayout->setAlignment( Qt::AlignTop );

  QLabel * label1 = new QLabel( groupBox, "label1" );
  label1->setText( i18n( "Name:" ) );
  groupBoxLayout->addWidget( label1, 0, 0 );

  m_nameEdit = new KLineEdit( groupBox, "m_nameEdit" );
  m_nameEdit->setText( m_dlg->styleName );
  groupBoxLayout->addWidget( m_nameEdit, 0, 1 );

  QLabel * label2 = new QLabel( groupBox, "label2" );
  label2->setText( i18n( "Inherit style:" ) );
  groupBoxLayout->addWidget( label2, 1, 0 );

  m_parentBox = new KComboBox( false, groupBox, "m_parentBox" );
  m_parentBox->clear();
  m_parentBox->insertItem( i18n( "<None>" ) );
  QStringList tmp = m_dlg->getStyleManager()->styleNames();
  tmp.remove( m_dlg->styleName );
  m_parentBox->insertStringList( tmp );

  if ( m_dlg->getStyle()->parent() )
    m_parentBox->setCurrentText( m_dlg->getStyle()->parentName() );
  else
  {
    m_parentBox->setCurrentText( i18n( "<None>" ) );

    if ( m_dlg->getStyle()->definesAll() )
      m_parentBox->setEnabled( false );
  }

  connect( m_parentBox, SIGNAL( textChanged( const QString & ) ), this, SLOT( slotNewParent( const QString & ) ) );
  connect( m_nameEdit, SIGNAL( lostFocus() ), this, SLOT( slotNameChanged() ) );

  groupBoxLayout->addWidget( m_parentBox, 1, 1 );

  QSpacerItem * spacer = new QSpacerItem( 20, 260, QSizePolicy::Minimum, QSizePolicy::Expanding );

  layout->addWidget( groupBox, 0, 0 );
  layout->addItem( spacer, 1, 0 );

  if ( m_dlg->getStyle()->type() == Style::BUILTIN )
  {
    m_nameEdit->setEnabled( false );
    m_parentBox->setEnabled( false );
  }

  resize( QSize( 534, 447 ).expandedTo(minimumSizeHint()) );
}

GeneralTab::~GeneralTab()
{
}

void GeneralTab::slotNameChanged()
{
  checkName();
}

void GeneralTab::slotNewParent( const QString & parentName )
{
  kdDebug() << "New Parent" << endl;
  if ( !checkParent( parentName ) )
    return;

  if ( parentName.isEmpty() || parentName == i18n( "<None>" ) )
    m_dlg->getStyle()->setParent( 0 );
  else
    m_dlg->getStyle()->setParent( m_dlg->getStyleManager()->style( parentName ) );

  // Set difference to new parent, set GUI to parent values, add changes made before
  //  m_dlg->initGUI();
}

bool GeneralTab::checkName()
{
  if ( m_nameEdit->isEnabled() )
  {
    if ( !m_dlg->getStyleManager()->validateStyleName( m_nameEdit->text(), m_dlg->getStyle() ) )
    {
      KMessageBox::sorry( this, i18n( "A style with this name already exists." ) );
      return false;
    }
  }

  return true;
}

bool GeneralTab::checkParent( const QString & parentName )
{
  if ( m_dlg->getStyle()->parentName() != parentName
       && m_parentBox->isEnabled() && parentName != i18n( "<None>" ) && !parentName.isEmpty() )
  {
    if ( m_nameEdit->text() == parentName )
    {
      KMessageBox::sorry( this, i18n( "A style cannot inherit from itself." ) );
      return false;
    }
    if ( !m_dlg->checkCircle( m_nameEdit->text(), parentName ) )
    {
      KMessageBox::sorry( this,
                          i18n( "The style cannot inherit from '%1' because of recursive references." )
                          .arg( m_parentBox->currentText() ) );
      return false;
    }

    CustomStyle * p = m_dlg->getStyleManager()->style( parentName );

    if ( !p )
    {
      KMessageBox::sorry( this, i18n( "The parent style does not exist." ) );
      return false;
    }
  }

  return true;
}

bool GeneralTab::apply( CustomStyle * style )
{
  if ( !checkParent( m_parentBox->currentText() ) )
    return false;

  if ( !checkName() )
    return false;

  if ( m_nameEdit->isEnabled() )
  {
    if ( style->type() != Style::BUILTIN )
    {
      QString name( style->name() );
      style->setName( m_nameEdit->text() );
      if ( m_parentBox->isEnabled() )
      {
        if ( m_parentBox->currentText() == i18n( "None" ) || m_parentBox->currentText().isEmpty() )
          style->setParent( 0 );
        else
          style->setParent( m_dlg->getStyleManager()->style( m_parentBox->currentText() ) );
      }
      m_dlg->getStyleManager()->changeName( name, m_nameEdit->text() );
    }
  }

  if ( style->type() == Style::TENTATIVE )
    style->setType( Style::CUSTOM );

  return true;
}



CellFormatDialog::CellFormatDialog( View * _view, Sheet * _sheet,
                              int _left, int _top, int _right, int _bottom )
  : QObject(),
    m_doc( _sheet->doc() ),
    m_sheet( _sheet ),
    m_pView( _view ),
    m_style( 0 )
{
  initMembers();

  //We need both conditions quite often, so store the condition here too
  isRowSelected    = util_isRowSelected(_view->selection());
  isColumnSelected = util_isColumnSelected(_view->selection());

  //Do we really need these as arguments? (_view->selection())
  left = _left;
  top = _top;
  right = _right;
  bottom = _bottom;

  if ( left == right )
    oneCol = true;
  else
    oneCol = false;

  if ( top == bottom )
    oneRow = true;
  else
    oneRow = false;

  Cell * obj = m_sheet->cellAt( _left, _top );
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
  obj = m_sheet->cellAt( _right, _top );
  borders[BorderType_Right].style = obj->rightBorderStyle( _right, _top );
  borders[BorderType_Right].width = obj->rightBorderWidth( _right, _top );
  borders[BorderType_Right].color = obj->rightBorderColor( _right, _top );

  // Look at the bottom left cell for the bottom border.
  obj = m_sheet->cellAt( _left, _bottom );
  borders[BorderType_Bottom].style = obj->bottomBorderStyle( _left, _bottom );
  borders[BorderType_Bottom].width = obj->bottomBorderWidth( _left, _bottom );
  borders[BorderType_Bottom].color = obj->bottomBorderColor( _left, _bottom );

  // Just an assumption
  obj = m_sheet->cellAt( _right, _top );
  if ( obj->isObscuringForced() )
  {
    obj = obj->obscuringCells().first();
    int moveX  = obj->column();
    int moveY  = _top;
    int moveX2 = _right;
    int moveY2 = obj->row();
    borders[BorderType_Vertical].style = obj->leftBorderStyle( moveX, moveY );
    borders[BorderType_Vertical].width = obj->leftBorderWidth( moveX, moveY );
    borders[BorderType_Vertical].color = obj->leftBorderColor( moveX, moveY );

    obj = m_sheet->cellAt( moveX2,  moveY2 );
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

  obj = m_sheet->cellAt( _left, _top );
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
  underline = obj->textFontUnderline( _left, _top );
  // Needed to initialize the font correctly ( bug in Qt )
  textFont = obj->textFont( _left, _top );
  obj->currencyInfo( cCurrency );

  brushColor = obj->backGroundBrushColor( _left, _top );
  brushStyle = obj->backGroundBrushStyle( _left,_top );

  bMultiRow = obj->multiRow( _left, _top );
  bVerticalText = obj->verticalText( _left, _top );
  textRotation = obj->getAngle(_left, _top);
  formatType = obj->getFormatType(_left, _top);

  bDontPrintText = obj->getDontprintText( _left, _top );
  bHideFormula   = obj->isHideFormula( _left, _top );
  bHideAll       = obj->isHideAll( _left, _top );
  bIsProtected   = !obj->notProtected( _left, _top );

  indent = obj->getIndent(_left, _top);

  value = obj->value();

  RowFormat *rl;
  ColumnFormat *cl;
  widthSize = 0.0;
  heightSize = 0.0;

  if ( !isRowSelected )
  {
    for ( int x = _left; x <= _right; x++ )
    {
      cl = m_pView->activeSheet()->columnFormat( x );
      widthSize = QMAX( cl->dblWidth(), widthSize );
    }
  }

  if ( !isColumnSelected )
  {
    for ( int y = _top; y <= _bottom; y++ )
    {
      rl = m_pView->activeSheet()->rowFormat(y);
      heightSize = QMAX( rl->dblHeight(), heightSize );
    }
  }

  //select column(s)
  if ( isColumnSelected )
  {
    int y = 1;
    Cell* c = NULL;
    for (int x = _left;x <= _right; x++)
    {
      ColumnFormat *obj = m_sheet->nonDefaultColumnFormat(x);
      initParameters( obj,x,y);

      for (c = m_sheet->getFirstCellColumn(x); c != NULL;
           c = m_sheet->getNextCellDown(c->column(), c->row()))
      {
        initParameters( c, x, c->row());
      }
    }

  }
  else if ( isRowSelected )
  {
    int x = 1;
    Cell* c = NULL;
    for ( int y = _top;y<=_bottom;y++)
    {
      RowFormat *obj = m_sheet->nonDefaultRowFormat(y);
      initParameters( obj,x,y);

      for (c = m_sheet->getFirstCellRow(y); c != NULL;
           c = m_sheet->getNextCellRight(c->column(), c->row()) )
      {
        initParameters( c, c->column(), c->row());
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
        Cell *obj = m_sheet->cellAt( x, y );

        if ( obj->isObscuringForced() )
          continue;

        initParameters( obj,x,y);
      }
    }
  }
  if ( !bTextRotation )
    textRotation = 0;

  if ( isColumnSelected )
  {
    int y=1;
    ColumnFormat *obj=m_sheet->nonDefaultColumnFormat(_left);
    checkBorderLeft( obj,_left, y);

    Cell* c = NULL;
    for (c = m_sheet->getFirstCellColumn(_left); c != NULL;
         c = m_sheet->getNextCellDown(c->column(), c->row()) )
    {
      checkBorderLeft(c, c->column(), c->row());
    }


    obj=m_sheet->nonDefaultColumnFormat(_right);
    checkBorderRight(obj,_right,y);
    c = NULL;
    for (c = m_sheet->getFirstCellColumn(_right); c != NULL;
         c = m_sheet->getNextCellDown(c->column(), c->row()) )
    {
      checkBorderRight(c, c->column(), c->row());
    }

    for ( int x = _left; x <= _right; x++ )
    {
      Cell *obj = m_sheet->cellAt( x, _top );
      checkBorderTop(obj,x, _top);
      obj = m_sheet->cellAt( x, _bottom );
      checkBorderBottom(obj,x, _bottom);
      if ( x > _left )
      {
        ColumnFormat *obj = m_sheet->nonDefaultColumnFormat(x);
        checkBorderHorizontal(obj,x, y);
        checkBorderVertical(obj,x, y);
      }
    }
  }
  else if ( isRowSelected )
  {
    int x=1;
    for ( int y = _top; y <= _bottom; y++ )
    {
      Cell *obj = m_sheet->cellAt( _right, y );
      checkBorderRight(obj,_right,y);
      obj = m_sheet->cellAt( _left, y );
      checkBorderLeft( obj,_left, y);
      if ( y > _top )
      {
        RowFormat* obj = m_sheet->nonDefaultRowFormat(y);
        checkBorderHorizontal(obj,x, y);
        checkBorderVertical(obj,x, y);
      }
    }

    RowFormat *obj=m_sheet->nonDefaultRowFormat(_top);
    checkBorderTop(obj,x, _top);
    obj=m_sheet->nonDefaultRowFormat(_bottom);
    checkBorderBottom(obj,x, _bottom);
  }
  else
  {
    for ( int y = _top; y <= _bottom; y++ )
    {
      Cell *obj = m_sheet->cellAt( _left, y );
      checkBorderLeft( obj,_left, y);
      obj = m_sheet->cellAt( _right, y );
      checkBorderRight(obj,_right,y);
    }

    for ( int x = _left; x <= _right; x++ )
    {
      Cell *obj = m_sheet->cellAt( x, _top );
      checkBorderTop( obj, x, _top );
      obj = m_sheet->cellAt( x, _bottom );
      checkBorderBottom( obj, x, _bottom );
    }

    // Look for the Outline
    for ( int x = _left; x <= _right; x++ )
    {
      for ( int y = _top+1; y <= _bottom; y++ )
      {
        Cell *obj = m_sheet->cellAt( x, y );
        checkBorderHorizontal(obj,x, y);
      }
    }

    for ( int x = _left+1; x <= _right; x++ )
    {
      for ( int y = _top; y <= _bottom; y++ )
      {
        Cell *obj = m_sheet->cellAt( x, y );
        checkBorderVertical(obj,x,y);
      }
    }
  }

  init();
}

CellFormatDialog::CellFormatDialog( View * _view, CustomStyle * _style,
                              StyleManager * _manager, Doc * doc )
  : QObject(),
    m_doc( doc ),
    m_sheet( 0 ),
    m_pView( _view ),
    m_style( _style ),
    m_styleManager( _manager )
{
  initMembers();
  initGUI();
  init();
}

void CellFormatDialog::initGUI()
{
  isRowSelected    = false;
  isColumnSelected = false;
  styleName = m_style->name();

  borders[BorderType_Left].style = m_style->leftBorderPen().style();
  borders[BorderType_Left].width = m_style->leftBorderPen().width();
  borders[BorderType_Left].color = m_style->leftBorderPen().color();

  borders[BorderType_Top].style  = m_style->topBorderPen().style();
  borders[BorderType_Top].width  = m_style->topBorderPen().width();
  borders[BorderType_Top].color  = m_style->topBorderPen().color();

  borders[BorderType_Right].style = m_style->rightBorderPen().style();
  borders[BorderType_Right].width = m_style->rightBorderPen().width();
  borders[BorderType_Right].color = m_style->rightBorderPen().color();

  borders[BorderType_Bottom].style = m_style->bottomBorderPen().style();
  borders[BorderType_Bottom].width = m_style->bottomBorderPen().width();
  borders[BorderType_Bottom].color = m_style->bottomBorderPen().color();

  borders[BorderType_FallingDiagonal].style = m_style->fallDiagonalPen().style();
  borders[BorderType_FallingDiagonal].width = m_style->fallDiagonalPen().width();
  borders[BorderType_FallingDiagonal].color = m_style->fallDiagonalPen().color();

  borders[BorderType_RisingDiagonal].style  = m_style->goUpDiagonalPen().style();
  borders[BorderType_RisingDiagonal].width  = m_style->goUpDiagonalPen().width();
  borders[BorderType_RisingDiagonal].color  = m_style->goUpDiagonalPen().color();

  borders[BorderType_Vertical].style = m_style->leftBorderPen().style();
  borders[BorderType_Vertical].width = m_style->leftBorderPen().width();
  borders[BorderType_Vertical].color = m_style->leftBorderPen().color();
  borders[BorderType_Horizontal].style = m_style->topBorderPen().style();
  borders[BorderType_Horizontal].width = m_style->topBorderPen().width();
  borders[BorderType_Horizontal].color = m_style->topBorderPen().color();

  prefix         = m_style->prefix();
  postfix        = m_style->postfix();
  precision      = m_style->precision();
  floatFormat    = m_style->floatFormat();
  floatColor     = m_style->floatColor();
  alignX         = m_style->alignX();
  alignY         = m_style->alignY();
  textColor      = m_style->pen().color();
  bgColor        = m_style->bgColor();
  textFontSize   = m_style->fontSize();
  textFontFamily = m_style->fontFamily();

  uint flags     = m_style->fontFlags();
  textFontBold   = ( flags & (uint) Style::FBold );
  textFontItalic = ( flags & (uint) Style::FItalic );
  strike         = ( flags & (uint) Style::FStrike );
  underline      = ( flags & (uint) Style::FUnderline );

  // Needed to initialize the font correctly ( bug in Qt )
  textFont   = m_style->font();
  cCurrency  = m_style->currency();
  brushColor = m_style->backGroundBrush().color();
  brushStyle = m_style->backGroundBrush().style();

  bMultiRow     = m_style->hasProperty( Style::PMultiRow );
  bVerticalText = m_style->hasProperty( Style::PVerticalText );
  textRotation  = m_style->rotateAngle();
  formatType    = m_style->formatType();
  indent        = m_style->indent();

  bDontPrintText = m_style->hasProperty( Style::PDontPrintText );
  bHideFormula   = m_style->hasProperty( Style::PHideFormula );
  bHideAll       = m_style->hasProperty( Style::PHideAll );
  bIsProtected   = !m_style->hasProperty( Style::PNotProtected );
}

CellFormatDialog::~CellFormatDialog()
{
  delete formatOnlyNegSignedPixmap;
  delete formatRedOnlyNegSignedPixmap;
  delete formatRedNeverSignedPixmap;
  delete formatAlwaysSignedPixmap;
  delete formatRedAlwaysSignedPixmap;
}

void CellFormatDialog::initMembers()
{
  formatOnlyNegSignedPixmap    = 0L;
  formatRedOnlyNegSignedPixmap = 0L;
  formatRedNeverSignedPixmap   = 0L;
  formatAlwaysSignedPixmap     = 0L;
  formatRedAlwaysSignedPixmap  = 0L;

  // We assume, that all other objects have the same values
  for ( int i = 0; i < BorderType_END; ++i )
  {
    borders[i].bStyle = true;
    borders[i].bColor = true;
  }
  bFloatFormat    = true;
  bFloatColor     = true;
  bTextColor      = true;
  bBgColor        = true;
  bTextFontFamily = true;
  bTextFontSize   = true;
  bTextFontBold   = true;
  bTextFontItalic = true;
  bStrike         = true;
  bUnderline      = true;
  bTextRotation   = true;
  bFormatType     = true;
  bCurrency       = true;
  bDontPrintText  = false;
  bHideFormula    = false;
  bHideAll        = false;
  bIsProtected    = true;

  cCurrency.symbol = locale()->currencySymbol();
  cCurrency.type   = 0;
}

bool CellFormatDialog::checkCircle( QString const & name, QString const & parent )
{
  return m_styleManager->checkCircle( name, parent );
}

void CellFormatDialog::checkBorderRight(Format *obj,int x,int y)
{
  if ( borders[BorderType_Right].style != obj->rightBorderStyle( x, y ) ||
       borders[BorderType_Right].width != obj->rightBorderWidth( x, y ) )
    borders[BorderType_Right].bStyle = false;
  if ( borders[BorderType_Right].color != obj->rightBorderColor( x, y ) )
    borders[BorderType_Right].bColor = false;
}

void CellFormatDialog::checkBorderLeft(Format *obj,int x,int y)
{
  if ( borders[BorderType_Left].style != obj->leftBorderStyle( x, y ) ||
       borders[BorderType_Left].width != obj->leftBorderWidth( x, y ) )
    borders[BorderType_Left].bStyle = false;
  if ( borders[BorderType_Left].color != obj->leftBorderColor( x, y ) )
    borders[BorderType_Left].bColor = false;
}

void CellFormatDialog::checkBorderTop(Format *obj,int x,int y)
{
  if ( borders[BorderType_Top].style != obj->topBorderStyle( x, y ) ||
       borders[BorderType_Top].width != obj->topBorderWidth( x, y ) )
    borders[BorderType_Top].bStyle = false;
  if ( borders[BorderType_Top].color != obj->topBorderColor( x, y ) )
    borders[BorderType_Top].bColor = false;
}

void CellFormatDialog::checkBorderBottom(Format *obj,int x,int y)
{
  if ( borders[BorderType_Bottom].style != obj->bottomBorderStyle( x, y ) ||
       borders[BorderType_Bottom].width != obj->bottomBorderWidth( x, y ) )
    borders[BorderType_Bottom].bStyle = false;
  if ( borders[BorderType_Bottom].color != obj->bottomBorderColor( x, y ) )
    borders[BorderType_Bottom].bColor = false;
}

void CellFormatDialog::checkBorderVertical(Format *obj,int x,int y)
{
  if (borders[BorderType_Vertical].style != obj->leftBorderStyle( x, y ) ||
      borders[BorderType_Vertical].width != obj->leftBorderWidth( x, y ))
    borders[BorderType_Vertical].bStyle = false;
  if ( borders[BorderType_Vertical].color != obj->leftBorderColor( x, y ) )
    borders[BorderType_Vertical].bColor = false;
}

void CellFormatDialog::checkBorderHorizontal(Format *obj,int x,int y)
{
  if ( borders[BorderType_Horizontal].style != obj->topBorderStyle( x, y ) ||
       borders[BorderType_Horizontal].width != obj->topBorderWidth( x, y ) )
    borders[BorderType_Horizontal].bStyle = false;
  if ( borders[BorderType_Horizontal].color != obj->topBorderColor( x, y ) )
    borders[BorderType_Horizontal].bColor = false;
}


void CellFormatDialog::initParameters(Format *obj,int x,int y)
{
  if (borders[BorderType_FallingDiagonal].style != obj->fallDiagonalStyle( x, y ))
    borders[BorderType_FallingDiagonal].bStyle = false;
  if (borders[BorderType_FallingDiagonal].width != obj->fallDiagonalWidth( x, y ))
    borders[BorderType_FallingDiagonal].bStyle = false;
  if (borders[BorderType_FallingDiagonal].color != obj->fallDiagonalColor( x, y ))
    borders[BorderType_FallingDiagonal].bColor = false;

  if (borders[BorderType_RisingDiagonal].style != obj->goUpDiagonalStyle( x, y ))
    borders[BorderType_RisingDiagonal].bStyle = false;
  if (borders[BorderType_RisingDiagonal].width != obj->goUpDiagonalWidth( x, y ))
    borders[BorderType_RisingDiagonal].bStyle = false;
  if (borders[BorderType_RisingDiagonal].color != obj->goUpDiagonalColor( x, y ))
    borders[BorderType_RisingDiagonal].bColor = false;
  if ( strike != obj->textFontStrike( x, y ) )
    bStrike = false;
  if ( underline != obj->textFontUnderline( x, y ) )
    bUnderline = false;
  if ( prefix != obj->prefix( x, y ) )
    prefix = QString::null;
  if ( postfix != obj->postfix( x, y ) )
    postfix = QString::null;
  if ( floatFormat != obj->floatFormat( x, y ) )
    bFloatFormat = false;
  if ( floatColor != obj->floatColor( x, y ) )
    bFloatColor = false;
  if ( textColor != obj->textColor( x, y ) )
    bTextColor = false;
  if ( textFontFamily != obj->textFontFamily( x, y ) )
    bTextFontFamily = false;
  if ( textFontSize != obj->textFontSize( x, y ) )
    bTextFontSize = false;
  if ( textFontBold != obj->textFontBold( x, y ) )
    bTextFontBold = false;
  if ( textFontItalic != obj->textFontItalic( x, y ) )
    bTextFontItalic = false;
  if ( bgColor != obj->bgColor( x, y ) )
    bBgColor = false;
  if ( textRotation != obj->getAngle(left, top) )
    bTextRotation = false;
  if ( formatType != obj->getFormatType(left, top) )
    bFormatType = false;
  if ( bMultiRow != obj->multiRow( left, top ) )
    bMultiRow = false;
  if ( bVerticalText!=obj->verticalText( left, top ) )
    bVerticalText = false;
  if (  bDontPrintText!=obj->getDontprintText( left, top ) )
    bDontPrintText= false;

  Cell::Currency cur;
  if (!obj->currencyInfo(cur))
    bCurrency = false;
  else
    if (cur.symbol != cCurrency.symbol)
      bCurrency = false;
}

void CellFormatDialog::init()
{
  QColorGroup colorGroup = QApplication::palette().active();

  // Did we initialize the bitmaps ?
  if ( formatOnlyNegSignedPixmap == 0L )
  {
    QColor black = colorGroup.text(); // not necessarily black :)
    formatOnlyNegSignedPixmap    = paintFormatPixmap( "123.456", black, "-123.456", black );
    formatRedOnlyNegSignedPixmap = paintFormatPixmap( "123.456", black, "-123.456", Qt::red );
    formatRedNeverSignedPixmap   = paintFormatPixmap( "123.456", black, "123.456", Qt::red );
    formatAlwaysSignedPixmap     = paintFormatPixmap( "+123.456", black, "-123.456", black );
    formatRedAlwaysSignedPixmap  = paintFormatPixmap( "+123.456", black, "-123.456", Qt::red );
  }

  tab = new QTabDialog( (QWidget*)m_pView, 0L, true );
  //tab->setGeometry( tab->x(), tab->y(), 420, 400 );

  if ( m_style )
  {
    generalPage = new GeneralTab( tab, this );
    tab->addTab( generalPage, i18n( "&General" ) );
  }

  floatPage = new CellFormatPageFloat( tab, this );
  tab->addTab( floatPage, i18n("&Data Format") );

  fontPage = new CellFormatPageFont( tab, this );
  tab->addTab( fontPage, i18n("&Font") );

  //  miscPage = new CellFormatPageMisc( tab, this );
  //  tab->addTab( miscPage, i18n("&Misc") );

  positionPage = new CellFormatPagePosition( tab, this);
  tab->addTab( positionPage, i18n("&Position") );

  borderPage = new CellFormatPageBorder( tab, this );
  tab->addTab( borderPage, i18n("&Border") );

  patternPage=new CellFormatPagePattern(tab,this);
  tab->addTab( patternPage,i18n("Back&ground"));

  protectPage = new CellFormatPageProtection( tab, this );
  tab->addTab( protectPage, i18n("&Cell Protection") );

  tab->setCancelButton( i18n( "&Cancel" ) );
  tab->setOkButton( i18n( "&OK" ) );

  tab->setCaption( i18n( "Cell Format" ) );

  tab->adjustSize();

  connect( tab, SIGNAL( applyButtonPressed() ), this, SLOT( slotApply() ) );

  tab->exec();
}

QPixmap * CellFormatDialog::paintFormatPixmap( const char * _string1, const QColor & _color1,
                                            const char *_string2, const QColor & _color2 )
{
  QPixmap * pixmap = new QPixmap( 150, 14 );

  QPainter painter;
  painter.begin( pixmap );
  painter.fillRect( 0, 0, 150, 14, QApplication::palette().active().base() );
  painter.setPen( _color1 );
  painter.drawText( 2, 11, _string1 );
  painter.setPen( _color2 );
  painter.drawText( 75, 11, _string2 );
  painter.end();

  QBitmap bm( pixmap->size() );
  bm.fill( color0 );
  painter.begin( &bm );
  painter.setPen( color1 );
  painter.drawText( 2, 11, _string1 );
  painter.drawText( 75, 11, _string2 );
  painter.end();
  pixmap->setMask( bm );

  return pixmap;
}

int CellFormatDialog::exec()
{
  return ( tab->exec() );
}

void CellFormatDialog::applyStyle()
{
  generalPage->apply( m_style );

  borderPage->applyOutline();
  floatPage->apply( m_style );
  // miscPage->apply( m_style );
  fontPage->apply( m_style );
  positionPage->apply( m_style );
  patternPage->apply( m_style );
  protectPage->apply( m_style );
}

void CellFormatDialog::slotApply()
{
  if ( m_style )
  {
    applyStyle();
    return;
  }

  m_pView->doc()->emitBeginOperation( false );
  Cell * cell = 0;

  MacroUndoAction * macroUndo = new MacroUndoAction( m_doc, i18n("Change Format") );

  if ( isMerged != positionPage->getMergedCellState() )
  {
    if ( positionPage->getMergedCellState() )
    {
      Cell * obj = m_sheet->nonDefaultCell( left, top );

      UndoMergedCell * undo = new UndoMergedCell( m_doc, m_sheet, left,
                                                                top, obj->extraXCells(), obj->extraYCells() );
      macroUndo->addCommand( undo );

      //merge cell doesn't create undo
      m_sheet->mergeCells( m_pView->selection() );
      right  = left;
      bottom = top;
    }
    else
    {
      //dissociate cells
      Cell * obj = m_sheet->nonDefaultCell( left, top );
      right  = obj->extraXCells() + left;
      bottom = obj->extraYCells() + top;

      UndoMergedCell * undo = new UndoMergedCell( m_doc, m_sheet, left,
                                                                top, obj->extraXCells(), obj->extraYCells() );
      macroUndo->addCommand(undo);

      m_sheet->dissociateCell(QPoint(left,top));
    }
  }

  // Prepare the undo buffer
  if ( !m_doc->undoLocked() )
  {
    QRect rect;

    // Since the right/bottom border is stored in objects right + 1 ( or: bottom + 1 )
    // So we have to save these formats, too
    if ( (!isRowSelected ) && ( !isColumnSelected ) )
      rect.setCoords( left, top, right + 1, bottom + 1 );
    else if ( isRowSelected )
      rect.setCoords( left, top, right, bottom + 1  );
    else if ( isColumnSelected )
    {
      //create cell before to apply
      RowFormat * rw = m_sheet->firstRow();
      for ( ; rw; rw = rw->next() )
      {
        if ( !rw->isDefault() )
        {
          for ( int i = left; i <= right; ++i )
          {
            cell = m_sheet->nonDefaultCell( i, rw->row() );
          }
        }
      }
      rect.setCoords( left, top, right + 1, bottom  );
    }

    QString title = i18n( "Change Format" );
    UndoCellFormat * undo = new UndoCellFormat( m_doc, m_sheet, rect, title );
    // m_doc->addCommand( undo );
    macroUndo->addCommand( undo );

    /*	if ( miscPage->getStyle()!=eStyle)
        {
        //make undo for style of cell
        UndoStyleCell *undo3 = new UndoStyleCell( m_doc, m_sheet, rect );
        //m_doc->addCommand( undo3 );
        macroUndo->addCommand( undo3 );
        }*/
  }
  borderPage->applyOutline();

  if ( ( !isRowSelected ) && ( !isColumnSelected ) )
  {
    for ( int x = left; x <= right; x++ )
      for ( int y = top; y <= bottom; y++ )
      {
        Cell *obj = m_sheet->nonDefaultCell( x, y );
        if ( !obj->isObscuringForced() )
        {
          floatPage->apply( obj );
          //                    miscPage->apply( obj );
          fontPage->apply( obj );
          positionPage->apply( obj );
          patternPage->apply(obj);
          protectPage->apply( obj );
        }
      }

    // Check for a change in the height and width of the cells
    if ( int( positionPage->getSizeHeight() ) != int( heightSize )
         || int( positionPage->getSizeWidth() ) != int( widthSize ) )
    {
      if ( !m_doc->undoLocked() )
      {
        QRect rect;
        rect.setCoords( left, top, right , bottom  );
        UndoResizeColRow *undo2 = new UndoResizeColRow( m_doc, m_sheet , rect );
        //m_doc->addCommand( undo2 );
        macroUndo->addCommand( undo2 );
      }
    }
    if ( int( positionPage->getSizeHeight() ) != int( heightSize ) )
    {
      for ( int x = top; x <= bottom; x++ ) // The loop seems to be doubled, already done in resizeRow: Philipp -> fixme
        m_pView->vBorderWidget()->resizeRow( positionPage->getSizeHeight(), x, false );

    }
    if ( int( positionPage->getSizeWidth() ) != int( widthSize ) )
      // The loop seems to be doubled, already done in resizeColumn: Philipp -> fixme
    {
      for ( int x = left; x <= right; x++ )
        m_pView->hBorderWidget()->resizeColumn( positionPage->getSizeWidth(), x, false );
    }
  }
  else if ( isRowSelected )
  {
    for ( int i = top; i <= bottom; i++ )
    {
      RowFormat * rw = m_sheet->nonDefaultRowFormat( i );
      floatPage->apply( rw );
      fontPage->apply( rw );
      positionPage->apply( rw );
      patternPage->apply( rw );
      protectPage->apply( rw );
    }
    //        miscPage->applyRow( );
    if ( int( positionPage->getSizeHeight() ) != int( heightSize ) )
    {
      if ( !m_doc->undoLocked())
      {
        QRect rect;
        rect.setCoords( left, top, right, bottom  );
        UndoResizeColRow * undo2 = new UndoResizeColRow( m_doc, m_sheet , rect );
        //m_doc->addCommand( undo2 );
        macroUndo->addCommand(undo2);
      }
      for ( int x = top; x <= bottom; x++ ) // The loop seems to be doubled, already done in resizeRow: Philipp -> fixme
        m_pView->vBorderWidget()->resizeRow( positionPage->getSizeHeight(), x, false );
    }
  }
  else if ( isColumnSelected )
  {
    for ( int i = left; i <= right; ++i )
    {
      ColumnFormat * cl = m_sheet->nonDefaultColumnFormat( i );
      floatPage->apply( cl );
      fontPage->apply( cl );
      positionPage->apply( cl );
      patternPage->apply( cl );
      protectPage->apply( cl );
    }
    //        miscPage->applyColumn( );

    if ( int( positionPage->getSizeWidth() ) != int( widthSize ) )
    {
      if ( !m_doc->undoLocked())
      {
        QRect rect;
        rect.setCoords( left, top, right , bottom  );
        UndoResizeColRow * undo2 = new UndoResizeColRow( m_doc, m_sheet , rect );
        // m_doc->addCommand( undo2 );
        macroUndo->addCommand(undo2);
      }
      for ( int x = left; x <= right; x++ ) // The loop seems to be doubled, already done in resizeColumn: Philipp -> fixme
        m_pView->hBorderWidget()->resizeColumn(positionPage->getSizeWidth(), x, false );
    }
  }

  if ( !m_doc->undoLocked())
    m_doc->addCommand( macroUndo );

  // m_pView->drawVisibleCells();
  QRect r;
  r.setCoords( left, top, right, bottom );
  m_pView->doc()->setModified( true );
  // Update the toolbar (bold/italic/font...)
  m_pView->updateEditWidget();
  m_pView->slotUpdateView( m_sheet, r );
}


CellFormatPageFloat::CellFormatPageFloat( QWidget* parent, CellFormatDialog *_dlg )
  : QWidget ( parent ),
    dlg( _dlg )
{
    QVBoxLayout* layout = new QVBoxLayout( this, 6,10 );

    QButtonGroup *grp = new QButtonGroup( i18n("Format"),this);
    QGridLayout *grid = new QGridLayout(grp,11,2,KDialog::marginHint(), KDialog::spacingHint());

    int fHeight = grp->fontMetrics().height();
    grid->addRowSpacing( 0, fHeight/2 ); // groupbox title

    grp->setRadioButtonExclusive( true );
    generic=new QRadioButton(i18n("Generic"),grp);
    QWhatsThis::add(generic, i18n( "This is the default format and KSpread autodetects the actual data type depending on the current cell data. By default, KSpread right justifies numbers, dates and times within a cell and left justifies anything else." ) );
    grid->addWidget(generic,1,0);

    number=new QRadioButton(i18n("Number"),grp);
    QWhatsThis::add(number, i18n( "The number notation uses the notation you globally choose in KControl -> Regional & Accessibility -> Numbers tab. Numbers are right justified by default." ) );
    grid->addWidget(number,2,0);

    percent=new QRadioButton(i18n("Percent"),grp);
    QWhatsThis::add(percent, i18n( "When you have a number in the current cell and you switch from the dcell format from Generic to Percent, the current cell number will be multiplied by 100%.\nFor example if you enter 12 and set the cell format to Percent, the number will then be 1,200 %. Switching back to Generic cell format will bring it back to 12.\nYou can also use the Percent icon in the Format Toolbar." ) );
    grid->addWidget(percent,3,0);

    money=new QRadioButton(i18n("Money"),grp);
    QWhatsThis::add(money, i18n( "The Money format converts your number into money notation using the settings globally fixed in KControl in Regional & Accessibility -> Money. The currency symbol will be displayed and the precision will be the one set in KControl.\nYou can also use the Currency icon in the Format Toolbar to set the cell formatting to look like your current currency." ) );
    grid->addWidget(money,4,0);

    scientific=new QRadioButton(i18n("Scientific"),grp);
    QWhatsThis::add(scientific, i18n( "The scientific format changes your number using the scientific notation. For example, 0.0012 will be changed to 1.2E-03. Going back using Generic cell format will display 0.0012 again." ) );
    grid->addWidget(scientific,5,0);

    fraction=new QRadioButton(i18n("Fraction"),grp);
    QWhatsThis::add(fraction, i18n( "The fraction format changes your number into a fraction. For example, 0.1 can be changed to 1/8, 2/16, 1/10, etc. You define the type of fraction by choosing it in the field on the right. If the exact fraction is not possible in the fraction mode you choose, the nearest closest match is chosen.\n For example: when we have 1.5 as number, we choose Fraction and Sixteenths 1/16 the text displayed into cell is \"1 8/16\" which is an exact fraction. If you have 1.4 as number in your cell and you choose Fraction and Sixteenths 1/16 then the cell will display \"1 6/16\" which is the nearest closest Sixteenth fraction." ) );
    grid->addWidget(fraction,6,0);

    date=new QRadioButton(i18n("Date format"),grp);
    QWhatsThis::add(date, i18n( "To enter a date, you should enter it in one of the formats set in KControl in Regional & Accessibility ->Time & Dates. There are two formats set here: the date format and the short date format.\nJust like you can drag down numbers you can also drag down dates and the next cells will also get dates." ) );
    grid->addWidget(date,7,0);

    time=new QRadioButton(i18n("Time format"),grp);
    QWhatsThis::add(time, i18n( "This formats your cell content as a time. To enter a time, you should enter it in the Time format set in KControl in Regional & Accessibility ->Time & Dates. In the Cell Format dialog box you can set how the time should be displayed by choosing one of the available time format options. The default format is the system format set in KControl. When the number in the cell does not make sense as a time, KSpread will display 00:00 in the global format you have in KControl." ) );
    grid->addWidget(time,8,0);

    textFormat=new QRadioButton(i18n("Text"),grp);
    QWhatsThis::add(textFormat, i18n( "This formats your cell content as text. This can be useful if you want a number treated as text instead as a number, for example for a ZIP code. Setting a number as text format will left justify it. When numbers are formatted as text, they cannot be used in calculations or formulas. It also change the way the cell is justified." ) );
    grid->addWidget(textFormat,9,0);

    customFormat=new QRadioButton(i18n("Custom"),grp);
    QWhatsThis::add(customFormat, i18n( "The custom format does not work yet. To be enabled in the next release." ) );
    grid->addWidget(customFormat,10,0);
    customFormat->setEnabled( false );

    QGroupBox *box2 = new QGroupBox( grp, "Box");
    box2->setTitle(i18n("Preview"));
    QGridLayout *grid3 = new QGridLayout(box2,1,3,KDialog::marginHint(), KDialog::spacingHint());

    exampleLabel=new QLabel(box2);
    QWhatsThis::add(exampleLabel, i18n( "This will display a preview of your choice so you can know what it does before clicking the OK button to validate it." ) );
    grid3->addWidget(exampleLabel,0,1);

    grid->addMultiCellWidget(box2,9,10,1,1);

    customFormatEdit = new QLineEdit( grp );
    grid->addMultiCellWidget( customFormatEdit, 1, 1, 1, 1 );
    customFormatEdit->setHidden( true );

    listFormat=new QListBox(grp);
    grid->addMultiCellWidget(listFormat,2,7,1,1);
    QWhatsThis::add(listFormat, i18n( "Displays choices of format for the fraction, date or time formats." ) );
    layout->addWidget(grp);

    /* *** */

    QGroupBox *box = new QGroupBox( this, "Box");

    grid = new QGridLayout(box,3,4,KDialog::marginHint(), KDialog::spacingHint());

    postfix = new QLineEdit( box, "LineEdit_1" );
    QWhatsThis::add(postfix, i18n( "You can add here a Postfix such as a $HK symbol to the end of each cell content in the checked format." ) );
    grid->addWidget(postfix,2,1);
    precision = new KIntNumInput( dlg->precision, box, 10 );
    precision->setSpecialValueText(i18n("variable"));
    precision->setRange(-1,10,1,false);
    QWhatsThis::add(precision, i18n( "You can control how many digits are displayed after the decimal point for numeric values. This can also be changed using the Increase precision or Decrease precision icons in the Format toolbar. " ) );
    grid->addWidget(precision,1,1);

    prefix = new QLineEdit( box, "LineEdit_3" );
    QWhatsThis::add(prefix, i18n( "You can add here a Prefix such as a $ symbol at the start of each cell content in the checked format." ) );
    grid->addWidget(prefix,0,1);

    format = new QComboBox( box, "ListBox_1" );
    QWhatsThis::add(format, i18n( "You can choose whether positive values are displayed with a leading + sign and whether negative values are shown in red." ) );
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
    grid->addWidget(tmpQLabel, 0, 2);
    tmpQLabel->setText( i18n("Format:") );

    currencyLabel = new QLabel( box, "LabelCurrency" );
    grid->addWidget(currencyLabel, 1, 2);
    currencyLabel->setText( i18n("Currency:") );

    currency = new QComboBox( box, "ComboCurrency" );
    grid->addWidget(currency, 1, 3);

    currency->insertItem( i18n("Automatic") );

    int index = 2; //ignore first two in the list
    bool ok = true;
    QString text;

    while ( ok )
    {
      text = Currency::getChooseString( index, ok );
      if ( ok )
      {
        currency->insertItem( text );
      }
      else
      {
        break;
      }

      ++index;
    }

    currency->setCurrentItem( 0 );
    currency->hide();
    currencyLabel->hide();

    if ( !dlg->bFloatFormat || !dlg->bFloatColor )
        format->setCurrentItem( 5 );
    else if ( dlg->floatFormat == Cell::OnlyNegSigned && dlg->floatColor == Cell::AllBlack )
        format->setCurrentItem( 0 );
    else if ( dlg->floatFormat == Cell::OnlyNegSigned && dlg->floatColor == Cell::NegRed )
        format->setCurrentItem( 1 );
    else if ( dlg->floatFormat == Cell::AlwaysUnsigned && dlg->floatColor == Cell::NegRed )
        format->setCurrentItem( 2 );
    else if ( dlg->floatFormat == Cell::AlwaysSigned && dlg->floatColor == Cell::AllBlack )
        format->setCurrentItem( 3 );
    else if ( dlg->floatFormat == Cell::AlwaysSigned && dlg->floatColor == Cell::NegRed )
        format->setCurrentItem( 4 );
    layout->addWidget(box);

    cellFormatType=dlg->formatType;
    newFormatType = cellFormatType;

    if (!cellFormatType)
          generic->setChecked(true);
    else
    {
        if (cellFormatType==Number_format)
                number->setChecked(true);
        else if (cellFormatType==Percentage_format)
                percent->setChecked(true);
        else if (cellFormatType==Money_format)
        {
                money->setChecked(true);
                currencyLabel->show();
                currency->show();
                if (dlg->bCurrency)
                {
                  QString tmp;
                  if (dlg->cCurrency.type != 1)
                  {
                    Currency curr(dlg->cCurrency.type);
                    bool ok = true;
                    tmp = Currency::getChooseString(dlg->cCurrency.type, ok);
                    if ( !ok )
                      tmp = dlg->cCurrency.symbol;
                  }
                  else
                    tmp = dlg->cCurrency.symbol;
                  currency->setCurrentText( tmp );
                }
        }
        else if ( cellFormatType == Scientific_format )
          scientific->setChecked(true);
        else if ( formatIsDate (cellFormatType) )
                date->setChecked(true);
        else if ( formatIsTime (cellFormatType) )
          time->setChecked(true);
        else if ( formatIsFraction (cellFormatType) )
          fraction->setChecked(true);
	else if (cellFormatType == Text_format)
	  textFormat->setChecked(true);
	else if (cellFormatType == Custom_format)
	  customFormat->setChecked(true);
        }

    connect(generic,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(fraction,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(money,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(date,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(scientific,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(number,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(percent,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(time,SIGNAL(clicked ()),this,SLOT(slotChangeState()));
    connect(textFormat,SIGNAL(clicked()),this,SLOT(slotChangeState()));
    connect(customFormat,SIGNAL(clicked()),this,SLOT(slotChangeState()));

    connect(listFormat,SIGNAL(selectionChanged ()),this,SLOT(makeformat()));
    connect(precision,SIGNAL(valueChanged(int)),this,SLOT(slotChangeValue(int)));
    connect(prefix,SIGNAL(textChanged ( const QString & ) ),this,SLOT(makeformat()));
    connect(postfix,SIGNAL(textChanged ( const QString & ) ),this,SLOT(makeformat()));
    connect(currency,SIGNAL(activated ( const QString & ) ),this, SLOT(currencyChanged(const QString &)));
    connect(format,SIGNAL(activated ( int ) ),this,SLOT(formatChanged(int)));
    slotChangeState();
    m_bFormatColorChanged=false;
    m_bFormatTypeChanged=false;
    this->resize( 400, 400 );
}

void CellFormatPageFloat::formatChanged(int)
{
    m_bFormatColorChanged=true;
}

void CellFormatPageFloat::slotChangeValue(int)
{
    makeformat();
}
void CellFormatPageFloat::slotChangeState()
{
    QStringList list;
    listFormat->clear();
    currency->hide();
    currencyLabel->hide();

    // start with enabled, they get disabled when inappropriate further down
    precision->setEnabled(true);
    prefix->setEnabled(true);
    postfix->setEnabled(true);
    format->setEnabled(true);

    if (generic->isChecked() || number->isChecked() || percent->isChecked() ||
        scientific->isChecked() || textFormat->isChecked())
      listFormat->setEnabled(false);
    else if (money->isChecked())
    {
        listFormat->setEnabled(false);
        precision->setValue(2);
        currency->show();
        currencyLabel->show();
    }
    else if (date->isChecked())
        {
            format->setEnabled(false);
            precision->setEnabled(false);
            prefix->setEnabled(false);
            postfix->setEnabled(false);
            listFormat->setEnabled(true);
            init();
        }
    else if (fraction->isChecked())
        {
            precision->setEnabled(false);
            listFormat->setEnabled(true);
            list+=i18n("Halves 1/2");
            list+=i18n("Quarters 1/4");
            list+=i18n("Eighths 1/8");
            list+=i18n("Sixteenths 1/16");
            list+=i18n("Tenths 1/10");
            list+=i18n("Hundredths 1/100");
            list+=i18n("One digit 5/9");
            list+=i18n("Two digits 15/22");
            list+=i18n("Three digits 153/652");
            listFormat->insertStringList(list);
            if (cellFormatType == fraction_half)
                listFormat->setCurrentItem(0);
            else if (cellFormatType == fraction_quarter)
                listFormat->setCurrentItem(1);
            else if (cellFormatType == fraction_eighth )
                listFormat->setCurrentItem(2);
            else if (cellFormatType == fraction_sixteenth )
                listFormat->setCurrentItem(3);
            else if (cellFormatType == fraction_tenth )
                listFormat->setCurrentItem(4);
            else if (cellFormatType == fraction_hundredth )
                listFormat->setCurrentItem(5);
            else if (cellFormatType == fraction_one_digit )
                listFormat->setCurrentItem(6);
            else if (cellFormatType == fraction_two_digits )
                listFormat->setCurrentItem(7);
            else if (cellFormatType == fraction_three_digits )
                listFormat->setCurrentItem(8);
            else
                listFormat->setCurrentItem(0);
        }
    else if (time->isChecked())
        {
            precision->setEnabled(false);
            prefix->setEnabled(false);
            postfix->setEnabled(false);
            format->setEnabled(false);
            listFormat->setEnabled(true);


            list+=i18n("System: ")+dlg->locale()->formatTime(QTime::currentTime(),false);
            list+=i18n("System: ")+dlg->locale()->formatTime(QTime::currentTime(),true);
            QDateTime tmpTime (QDate (1, 1, 1900), QTime (10, 35, 25));


            KSpread::ValueFormatter *fmt = dlg->getDoc()->formatter();
            list+= fmt->timeFormat(tmpTime, Time_format1);
            list+= fmt->timeFormat(tmpTime, Time_format2);
            list+= fmt->timeFormat(tmpTime, Time_format3);
            list+= fmt->timeFormat(tmpTime, Time_format4);
            list+= fmt->timeFormat(tmpTime, Time_format5);
            list+= ( fmt->timeFormat(tmpTime, Time_format6) + i18n(" (=[mm]::ss)") );
            list+= ( fmt->timeFormat(tmpTime, Time_format7) + i18n(" (=[hh]::mm::ss)") );
            list+= ( fmt->timeFormat(tmpTime, Time_format8) + i18n(" (=[hh]::mm)") );
            listFormat->insertStringList(list);

            if ( cellFormatType == Time_format )
                listFormat->setCurrentItem(0);
            else if (cellFormatType == SecondeTime_format)
                listFormat->setCurrentItem(1);
            else if (cellFormatType == Time_format1)
                listFormat->setCurrentItem(2);
            else if (cellFormatType == Time_format2)
                listFormat->setCurrentItem(3);
            else if (cellFormatType == Time_format3)
                listFormat->setCurrentItem(4);
            else if (cellFormatType == Time_format4)
                listFormat->setCurrentItem(5);
            else if (cellFormatType == Time_format5)
                listFormat->setCurrentItem(6);
            else if (cellFormatType == Time_format6)
                listFormat->setCurrentItem(7);
            else if (cellFormatType == Time_format7)
                listFormat->setCurrentItem(8);
            else if (cellFormatType == Time_format8)
                listFormat->setCurrentItem(9);
            else
                listFormat->setCurrentItem(0);
        }

    if (customFormat->isChecked())
    {
      customFormatEdit->setHidden( false );
      precision->setEnabled(false);
      prefix->setEnabled(false);
      postfix->setEnabled(false);
      format->setEnabled(false);
      listFormat->setEnabled(true);
    }
    else
      customFormatEdit->setHidden( true );

    m_bFormatTypeChanged=true;

    makeformat();
}

void CellFormatPageFloat::init()
{
    QStringList list;
    QString tmp;
    QString tmp2;
    QDate tmpDate( 2000,2,18);
    list+=i18n("System: ")+dlg->locale()->formatDate (QDate::currentDate(), true);
    list+=i18n("System: ")+dlg->locale()->formatDate (QDate::currentDate(), false);

    KSpread::ValueFormatter *fmt = dlg->getDoc()->formatter();

    /*18-Feb-00*/
    list+=fmt->dateFormat( tmpDate, date_format1);
    /*18-Feb-1999*/
    list+=fmt->dateFormat( tmpDate, date_format2);
    /*18-Feb*/
    list+=fmt->dateFormat( tmpDate, date_format3);
    /*18-2*/
    list+=fmt->dateFormat( tmpDate, date_format4);
    /*18/2/00*/
    list+=fmt->dateFormat( tmpDate, date_format5);
    /*18/5/1999*/
    list+=fmt->dateFormat( tmpDate, date_format6);
    /*Feb-99*/
    list+=fmt->dateFormat( tmpDate, date_format7);
    /*February-99*/
    list+=fmt->dateFormat( tmpDate, date_format8);
    /*February-1999*/
    list+=fmt->dateFormat( tmpDate, date_format9);
    /*F-99*/
    list+=fmt->dateFormat( tmpDate, date_format10);
    /*18/Feb*/
    list+=fmt->dateFormat( tmpDate, date_format11);
    /*18/2*/
    list+=fmt->dateFormat( tmpDate, date_format12);
    /*18/Feb/1999*/
    list+=fmt->dateFormat( tmpDate, date_format13);
    /*2000/Feb/18*/
    list+=fmt->dateFormat( tmpDate, date_format14);
    /*2000-Feb-18*/
    list+=fmt->dateFormat( tmpDate, date_format15);
    /*2000-2-18*/
    list+=fmt->dateFormat( tmpDate, date_format16);
    /*2 february 2000*/
    list+=fmt->dateFormat( tmpDate, date_format17);
    list+=fmt->dateFormat( tmpDate, date_format18);
    list+=fmt->dateFormat( tmpDate, date_format19);
    list+=fmt->dateFormat( tmpDate, date_format20);
    list+=fmt->dateFormat( tmpDate, date_format21);
    list+=fmt->dateFormat( tmpDate, date_format22);
    list+=fmt->dateFormat( tmpDate, date_format23);
    list+=fmt->dateFormat( tmpDate, date_format24);
    list+=fmt->dateFormat( tmpDate, date_format25);
    list+=fmt->dateFormat( tmpDate, date_format26);

    listFormat->insertStringList(list);
    if ( cellFormatType == ShortDate_format )
        listFormat->setCurrentItem(0);
    else if (cellFormatType == TextDate_format)
        listFormat->setCurrentItem(1);
    else if (cellFormatType == date_format1)
        listFormat->setCurrentItem(2);
    else if (cellFormatType == date_format2)
        listFormat->setCurrentItem(3);
    else if (cellFormatType == date_format3)
        listFormat->setCurrentItem(4);
    else if (cellFormatType == date_format4)
        listFormat->setCurrentItem(5);
    else if (cellFormatType == date_format5)
        listFormat->setCurrentItem(6);
    else if (cellFormatType == date_format6)
        listFormat->setCurrentItem(7);
    else if (cellFormatType == date_format7)
        listFormat->setCurrentItem(8);
    else if (cellFormatType == date_format8)
        listFormat->setCurrentItem(9);
    else if (cellFormatType == date_format9)
        listFormat->setCurrentItem(10);
    else if (cellFormatType == date_format10)
        listFormat->setCurrentItem(11);
    else if (cellFormatType == date_format11)
        listFormat->setCurrentItem(12);
    else if (cellFormatType == date_format12)
        listFormat->setCurrentItem(13);
    else if (cellFormatType == date_format13)
        listFormat->setCurrentItem(14);
    else if (cellFormatType == date_format14)
        listFormat->setCurrentItem(15);
    else if (cellFormatType == date_format15)
        listFormat->setCurrentItem(16);
    else if (cellFormatType == date_format16)
        listFormat->setCurrentItem(17);
    else if (cellFormatType == date_format17)
        listFormat->setCurrentItem(18);
    else if (cellFormatType == date_format18)
        listFormat->setCurrentItem(19);
    else if (cellFormatType == date_format19)
        listFormat->setCurrentItem(20);
    else if (cellFormatType == date_format20)
        listFormat->setCurrentItem(21);
    else if (cellFormatType == date_format21)
        listFormat->setCurrentItem(22);
    else if (cellFormatType == date_format22)
        listFormat->setCurrentItem(23);
    else if (cellFormatType == date_format23)
        listFormat->setCurrentItem(24);
    else if (cellFormatType == date_format24)
        listFormat->setCurrentItem(25);
    else if (cellFormatType == date_format25)
        listFormat->setCurrentItem(26);
    else if (cellFormatType == date_format26)
        listFormat->setCurrentItem(27);
    else
        listFormat->setCurrentItem(0);

}

void CellFormatPageFloat::currencyChanged(const QString &)
{
  int index = currency->currentItem();
  if (index > 0)
    ++index;
  dlg->cCurrency.symbol = Currency::getDisplaySymbol(index);
  dlg->cCurrency.type   = index;

  makeformat();
}

void CellFormatPageFloat::updateFormatType ()
{
  if (generic->isChecked())
    newFormatType = Generic_format;
  else if (number->isChecked())
    newFormatType = Number_format;
  else if (percent->isChecked())
    newFormatType = Percentage_format;
  else if (date->isChecked())
  {
    newFormatType=ShortDate_format;
    switch (listFormat->currentItem())
    {
      case 0: newFormatType=ShortDate_format; break;
      case 1: newFormatType=TextDate_format; break;
      case 2: newFormatType=date_format1; break; /*18-Feb-99*/
      case 3: newFormatType=date_format2; break; /*18-Feb-1999*/
      case 4: newFormatType=date_format3; break; /*18-Feb*/
      case 5: newFormatType=date_format4; break; /*18-05*/
      case 6: newFormatType=date_format5; break; /*18/05/00*/
      case 7: newFormatType=date_format6; break; /*18/05/1999*/
      case 8: newFormatType=date_format7; break;/*Feb-99*/
      case 9: newFormatType=date_format8; break; /*February-99*/
      case 10: newFormatType=date_format9; break; /*February-1999*/
      case 11: newFormatType=date_format10; break; /*F-99*/
      case 12: newFormatType=date_format11; break; /*18/Feb*/
      case 13: newFormatType=date_format12; break; /*18/02*/
      case 14: newFormatType=date_format13; break; /*18/Feb/1999*/
      case 15: newFormatType=date_format14; break; /*2000/Feb/18*/
      case 16: newFormatType=date_format15; break;/*2000-Feb-18*/
      case 17: newFormatType=date_format16; break;/*2000-02-18*/
      case 18: newFormatType=date_format17; break; /*2000-02-18*/
      case 19: newFormatType=date_format18; break;
      case 20: newFormatType=date_format19; break;
      case 21: newFormatType=date_format20; break;
      case 22: newFormatType=date_format21; break;
      case 23: newFormatType=date_format22; break;
      case 24: newFormatType=date_format23; break;
      case 25: newFormatType=date_format24; break;
      case 26: newFormatType=date_format25; break;
      case 27: newFormatType=date_format26; break;
    }
  }
  else if (money->isChecked())
    newFormatType = Money_format;
  else if (scientific->isChecked())
    newFormatType = Scientific_format;
  else if (fraction->isChecked())
  {
    newFormatType=fraction_half;
    switch (listFormat->currentItem())
    {
      case 0: newFormatType=fraction_half; break;
      case 1: newFormatType=fraction_quarter; break;
      case 2: newFormatType=fraction_eighth; break;
      case 3: newFormatType=fraction_sixteenth; break;
      case 4: newFormatType=fraction_tenth; break;
      case 5: newFormatType=fraction_hundredth; break;
      case 6: newFormatType=fraction_one_digit; break;
      case 7: newFormatType=fraction_two_digits; break;
      case 8: newFormatType=fraction_three_digits; break;
    }
  }
  else if (time->isChecked())
  {
    newFormatType=Time_format;
    switch (listFormat->currentItem())
    {
      case 0: newFormatType=Time_format; break;
      case 1: newFormatType=SecondeTime_format; break;
      case 2: newFormatType=Time_format1; break;
      case 3: newFormatType=Time_format2; break;
      case 4: newFormatType=Time_format3; break;
      case 5: newFormatType=Time_format4; break;
      case 6: newFormatType=Time_format5; break;
      case 7: newFormatType=Time_format6; break;
      case 8: newFormatType=Time_format7; break;
      case 9: newFormatType=Time_format8; break;
    }
  }
  else if (textFormat->isChecked())
    newFormatType = Text_format;
  else if (customFormat->isChecked())
    newFormatType = Custom_format;
}

void CellFormatPageFloat::makeformat()
{
  m_bFormatTypeChanged=true;
  QString tmp;

  updateFormatType();
  KSpread::ValueFormatter *fmt = dlg->getDoc()->formatter();
  tmp = fmt->formatText (dlg->value, newFormatType,
      precision->value());
  if (tmp.length() > 50)
    tmp = tmp.left (50);
  exampleLabel->setText (tmp);
}

void CellFormatPageFloat::apply( CustomStyle * style )
{
  if ( postfix->text() != dlg->postfix )
  {
    if ( postfix->text() != "########" )
    {
      if ( postfix->isEnabled())
        style->changePostfix( postfix->text() );
      else
        style->changePostfix( "" );
    }
  }
  if ( prefix->text() != dlg->prefix )
  {
    if ( prefix->text() != "########" )
    {
      if (prefix->isEnabled())
        style->changePrefix( prefix->text() );
      else
        style->changePrefix( "" );
    }
  }

  if ( dlg->precision != precision->value() )
    style->changePrecision( precision->value() );

  if ( m_bFormatColorChanged )
  {
    switch( format->currentItem() )
    {
     case 0:
      style->changeFloatFormat( Cell::OnlyNegSigned );
      style->changeFloatColor( Cell::AllBlack );
      break;
     case 1:
      style->changeFloatFormat( Cell::OnlyNegSigned );
      style->changeFloatColor( Cell::NegRed );
      break;
     case 2:
      style->changeFloatFormat( Cell::AlwaysUnsigned );
      style->changeFloatColor( Cell::NegRed );
      break;
     case 3:
      style->changeFloatFormat( Cell::AlwaysSigned );
      style->changeFloatColor( Cell::AllBlack );
      break;
     case 4:
      style->changeFloatFormat( Cell::AlwaysSigned );
      style->changeFloatColor( Cell::NegRed );
      break;
    }
  }
  if ( m_bFormatTypeChanged )
  {
    style->changeFormatType (newFormatType);
    if ( money->isChecked() )
    {
      Cell::Currency cur;
      int index = currency->currentItem();
      if (index == 0)
      {
        if ( currency->currentText() == i18n( "Automatic" ) )
        {
          cur.symbol = dlg->locale()->currencySymbol();
          cur.type   = 0;
        }
        else
        {
          cur.type   = 1;
          cur.symbol = currency->currentText();
        }
      }
      else
      {
        cur.type   = ++index;
        cur.symbol = Currency::getDisplaySymbol( index );
      }

      style->changeCurrency( cur );
    }
  }
}

void CellFormatPageFloat::applyFormat( Format *_obj )
{
  if ( postfix->text() != dlg->postfix )
    if ( postfix->text() != "########" && postfix->isEnabled())
    {
      // If we are in here it *never* can be disabled - FIXME (Werner)!
      if ( postfix->isEnabled())
        _obj->setPostfix( postfix->text() );
      else
        _obj->setPostfix( "" );
    }
  if ( prefix->text() != dlg->prefix )
    if ( prefix->text() != "########" )
    {
      if (prefix->isEnabled())
        _obj->setPrefix( prefix->text() );
      else
        _obj->setPrefix( "" );

    }

  if ( dlg->precision != precision->value() )
      _obj->setPrecision( precision->value() );

  if (m_bFormatColorChanged)
  {
    switch( format->currentItem() )
      {
      case 0:
        _obj->setFloatFormat( Cell::OnlyNegSigned );
        _obj->setFloatColor( Cell::AllBlack );
        break;
      case 1:
        _obj->setFloatFormat( Cell::OnlyNegSigned );
        _obj->setFloatColor( Cell::NegRed );
        break;
      case 2:
        _obj->setFloatFormat( Cell::AlwaysUnsigned );
        _obj->setFloatColor( Cell::NegRed );
        break;
      case 3:
        _obj->setFloatFormat( Cell::AlwaysSigned );
        _obj->setFloatColor( Cell::AllBlack );
        break;
      case 4:
        _obj->setFloatFormat( Cell::AlwaysSigned );
        _obj->setFloatColor( Cell::NegRed );
        break;
      }
  }
  if (m_bFormatTypeChanged)
  {
    _obj->setFormatType (newFormatType);
    if (money->isChecked())
    {
      Cell::Currency cur;
      int index = currency->currentItem();
      if (index == 0)
      {
        if ( currency->currentText() == i18n( "Automatic" ) )
        {
          cur.symbol = dlg->locale()->currencySymbol();
          cur.type   = 0;
        }
        else
        {
          cur.type   = 1;
          cur.symbol = currency->currentText();
        }
      }
      else
      {
        cur.type   = ++index;
        cur.symbol = Currency::getDisplaySymbol( index );
      }

      _obj->setCurrency( cur.type, cur.symbol );
    }
  }
}

void CellFormatPageFloat::apply( Cell *_obj )
{
    applyFormat(_obj);
}

void CellFormatPageFloat::apply( RowFormat *_obj )
{
  Sheet* sheet = dlg->getSheet();
  Cell* c = NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for ( c = sheet->getFirstCellRow(row); c != NULL;
         c = sheet->getNextCellRight(c->column(), c->row()) )
    {
      if ( dlg->precision != precision->value() )
      {
        c->clearProperty(Cell::PPrecision);
        c->clearNoFallBackProperties( Cell::PPrecision );
      }
      if ( postfix->text() != dlg->postfix )
      {
        if ( postfix->text() != "########" )
        {
          c->clearProperty(Cell::PPostfix);
          c->clearNoFallBackProperties( Cell::PPostfix );
        }
      }
      if ( prefix->text() != dlg->prefix )
      {
        if ( postfix->text() != "########" )
        {
          c->clearProperty(Cell::PPrefix);
          c->clearNoFallBackProperties( Cell::PPrefix );
        }
      }
      if (m_bFormatColorChanged)
      {
        c->clearProperty(Cell::PFloatFormat);
        c->clearNoFallBackProperties( Cell::PFloatFormat );
        c->clearProperty(Cell::PFloatColor);
        c->clearNoFallBackProperties( Cell::PFloatColor );
      }
      if (m_bFormatTypeChanged)
      {
        c->clearProperty(Cell::PFormatType);
        c->clearNoFallBackProperties( Cell::PFormatType );
      }
    }
  }
  applyFormat(_obj);
}

void CellFormatPageFloat::apply( ColumnFormat *_obj )
{
  Sheet *sheet = dlg->getSheet();
  Cell* c = NULL;
  for (int col = dlg->left; col <= dlg->right; col++)
  {
    for ( c = sheet->getFirstCellColumn(col); c != NULL;
         c = sheet->getNextCellDown(c->column(), c->row()) )
    {
      if ( dlg->precision != precision->value() )
      {
        c->clearProperty(Cell::PPrecision);
        c->clearNoFallBackProperties( Cell::PPrecision );
      }
      if ( postfix->text() != dlg->postfix )
      {
        if ( postfix->text() != "########" )
        {
          c->clearProperty(Cell::PPostfix);
          c->clearNoFallBackProperties( Cell::PPostfix );
        }
      }
      if ( prefix->text() != dlg->prefix )
      {
        if ( prefix->text() != "########" )
        {
          c->clearProperty(Cell::PPrefix);
          c->clearNoFallBackProperties( Cell::PPrefix );
        }
      }
      if (m_bFormatColorChanged)
      {
        c->clearProperty(Cell::PFloatFormat);
        c->clearNoFallBackProperties( Cell::PFloatFormat );
        c->clearProperty(Cell::PFloatColor);
        c->clearNoFallBackProperties( Cell::PFloatColor );
      }
      if (m_bFormatTypeChanged)
      {
        c->clearProperty(Cell::PFormatType);
        c->clearNoFallBackProperties( Cell::PFormatType );
      }
    }
  }
  applyFormat(_obj);

  RowFormat* rw =dlg->getSheet()->firstRow();
  for ( ; rw; rw = rw->next() )
  {
    if ( !rw->isDefault() &&
         (rw->hasProperty(Cell::PPrecision) ||
          rw->hasProperty(Cell::PPostfix) ||
          rw->hasProperty(Cell::PPrefix) ||
          rw->hasProperty(Cell::PFloatFormat) ||
          rw->hasProperty(Cell::PFloatColor) ||
          rw->hasProperty(Cell::PFormatType) ))
    {
      for ( int i=dlg->left;i<=dlg->right;i++)
      {
        Cell *cell = dlg->getSheet()->nonDefaultCell( i, rw->row() );
        applyFormat(cell );
      }
    }
  }
}

CellFormatPageProtection::CellFormatPageProtection( QWidget* parent, CellFormatDialog * _dlg )
  : QWidget( parent ),
    m_dlg( _dlg )
{
  QVBoxLayout * Form1Layout     = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint(), "Form1Layout");

  QGroupBox   * groupBox1       = new QGroupBox( 0, Qt::Vertical, i18n( "Protection" ), this, "groupBox1" );
  groupBox1->layout()->setSpacing( KDialog::spacingHint() );
  groupBox1->layout()->setMargin( KDialog::marginHint() );
  QVBoxLayout * groupBox1Layout = new QVBoxLayout( groupBox1->layout() );

  m_bIsProtected = new QCheckBox( i18n( "&Protected" ), groupBox1, "m_bIsProtected" );
  m_bIsProtected->setChecked( true );
  groupBox1Layout->addWidget( m_bIsProtected );

  m_bHideFormula = new QCheckBox( i18n( "&Hide formula" ), groupBox1, "m_bHideFormula" );
  groupBox1Layout->addWidget( m_bHideFormula );

  m_bHideAll = new QCheckBox( i18n( "Hide &all" ), groupBox1, "m_bHideAll" );
  groupBox1Layout->addWidget( m_bHideAll );
  Form1Layout->addWidget( groupBox1 );

  QGroupBox   * groupBox2       = new QGroupBox( 0, Qt::Vertical, i18n( "Printing" ), this, "groupBox2" );
  groupBox2->layout()->setSpacing( KDialog::spacingHint() );
  groupBox2->layout()->setMargin( KDialog::marginHint() );
  QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );

  m_bDontPrint = new QCheckBox( i18n( "&Do not print text" ), groupBox2, "m_bDontPrint" );
  groupBox2Layout->addWidget( m_bDontPrint );
  Form1Layout->addWidget( groupBox2 );
  QSpacerItem * spacer_4 = new QSpacerItem( 20, 90, QSizePolicy::Minimum, QSizePolicy::Expanding );
  Form1Layout->addItem( spacer_4 );

  m_bDontPrint->setChecked( m_dlg->bDontPrintText );
  m_bHideAll->setChecked( m_dlg->bHideAll );
  m_bHideFormula->setChecked( m_dlg->bHideFormula );
  m_bIsProtected->setChecked( m_dlg->bIsProtected );
}

CellFormatPageProtection::~CellFormatPageProtection()
{
}

void CellFormatPageProtection::apply( CustomStyle * style )
{
  if ( m_dlg->bDontPrintText != m_bDontPrint->isChecked() )
  {
    if ( m_bDontPrint->isChecked() )
      style->addProperty( Style::PDontPrintText );
    else
      style->removeProperty( Style::PDontPrintText );
  }

  if ( m_dlg->bIsProtected != m_bIsProtected->isChecked() )
  {
    if ( !m_bIsProtected->isChecked() )
      style->addProperty( Style::PNotProtected );
    else
      style->removeProperty( Style::PNotProtected );
  }

  if ( m_dlg->bHideAll != m_bHideAll->isChecked() )
  {
    if ( m_bHideAll->isChecked() )
      style->addProperty( Style::PHideAll );
    else
      style->removeProperty( Style::PHideAll );
  }

  if ( m_dlg->bHideFormula != m_bHideFormula->isChecked() )
  {
    if ( m_bHideFormula->isChecked() )
      style->addProperty( Style::PHideFormula );
    else
      style->removeProperty( Style::PHideFormula );
  }
}

void CellFormatPageProtection::apply( Cell * _cell )
{
  applyFormat( _cell );
}

void CellFormatPageProtection::apply( ColumnFormat * _col )
{
  Sheet * sheet = m_dlg->getSheet();
  Cell  * c     = 0;
  for (int col = m_dlg->left; col <= m_dlg->right; col++)
  {
    for ( c = sheet->getFirstCellColumn( col ); c != 0;
         c = sheet->getNextCellDown( c->column(), c->row() ) )
    {
      if ( m_dlg->bDontPrintText != m_bDontPrint->isChecked() )
      {
        c->clearProperty( Cell::PDontPrintText );
        c->clearNoFallBackProperties( Cell::PDontPrintText );
      }
      if ( m_dlg->bIsProtected != m_bIsProtected->isChecked() )
      {
        c->clearProperty( Cell::PNotProtected );
        c->clearNoFallBackProperties( Cell::PNotProtected );
      }
      if ( m_dlg->bHideFormula != m_bHideFormula->isChecked() )
      {
        c->clearProperty( Cell::PHideFormula );
        c->clearNoFallBackProperties( Cell::PHideFormula );
      }
      if ( m_dlg->bHideAll != m_bHideAll->isChecked() )
      {
        c->clearProperty( Cell::PHideAll );
        c->clearNoFallBackProperties( Cell::PHideAll );
      }
    }
  }

  applyFormat( _col );
}

void CellFormatPageProtection::apply( RowFormat * _row )
{
  Sheet * sheet = m_dlg->getSheet();
  Cell  * c = 0;
  for ( int row = m_dlg->top; row <= m_dlg->bottom; ++row)
  {
    for ( c = sheet->getFirstCellRow( row ); c != 0;
         c = sheet->getNextCellRight( c->column(), c->row() ) )
    {
      if ( m_dlg->bDontPrintText != m_bDontPrint->isChecked() )
      {
        c->clearProperty( Cell::PDontPrintText );
        c->clearNoFallBackProperties( Cell::PDontPrintText );
      }
      if ( m_dlg->bIsProtected != m_bIsProtected->isChecked() )
      {
        c->clearProperty( Cell::PNotProtected );
        c->clearNoFallBackProperties( Cell::PNotProtected );
      }
      if ( m_dlg->bHideFormula != m_bHideFormula->isChecked() )
      {
        c->clearProperty( Cell::PHideFormula );
        c->clearNoFallBackProperties( Cell::PHideFormula );
      }
      if ( m_dlg->bHideAll != m_bHideAll->isChecked() )
      {
        c->clearProperty( Cell::PHideAll );
        c->clearNoFallBackProperties( Cell::PHideAll );
      }
    }
  }

  applyFormat( _row );
}

void CellFormatPageProtection::applyFormat( Format * _obj )
{
  if ( m_dlg->bDontPrintText != m_bDontPrint->isChecked())
    _obj->setDontPrintText( m_bDontPrint->isChecked() );

  if ( m_dlg->bIsProtected != m_bIsProtected->isChecked())
    _obj->setNotProtected( !m_bIsProtected->isChecked() );

  if ( m_dlg->bHideAll != m_bHideAll->isChecked())
    _obj->setHideAll( m_bHideAll->isChecked() );

  if ( m_dlg->bHideFormula != m_bHideFormula->isChecked())
    _obj->setHideFormula( m_bHideFormula->isChecked() );
}


CellFormatPageMisc::CellFormatPageMisc( QWidget* parent, CellFormatDialog *_dlg ) : QWidget( parent )
{
    dlg = _dlg;


    QGridLayout* layout = new QGridLayout( this, 2,2,KDialog::marginHint(), KDialog::spacingHint() );
    QGroupBox *box = new QGroupBox( this, "Box");

    QGridLayout *grid = new QGridLayout(box,2,4,KDialog::marginHint(), KDialog::spacingHint());
    QLabel *tmpQLabel;

    tmpQLabel = new QLabel( box, "Label_3" );
    grid->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText( i18n("Functionality") );

    styleButton = new QComboBox( box, "ComboBox_2" );

    grid->addWidget(styleButton,1,0);

    idStyleNormal = 0; styleButton->insertItem( i18n("Normal"), 0 );
    idStyleButton = 1; styleButton->insertItem( i18n("Button"), 1 );
    idStyleSelect = 2; styleButton->insertItem( i18n("Select"), 2 );
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
        actionText->setEnabled( false );
    }
    else
      actionText->setEnabled( false );

      styleButton->setCurrentItem( idStyleNormal );

    layout->addWidget(box,0,0);

    box = new QGroupBox( this, "Box1");
    grid = new QGridLayout(box,4,1,KDialog::marginHint(), KDialog::spacingHint());
    dontPrintText= new QCheckBox(i18n("Do not print text"),box);
    dontPrintText->setChecked(dlg->bDontPrintText);

    grid->addWidget(dontPrintText,0,0);
    layout->addWidget(box,1,0);


    this->resize( 400, 400 );
}

void CellFormatPageMisc::apply( CustomStyle * /* style */ )
{
  // TODO
}

void CellFormatPageMisc::apply( Cell *_obj )
{
  applyFormat( _obj );
}

void CellFormatPageMisc::applyColumn( )
{
  Sheet* sheet = dlg->getSheet();
  Cell* c = NULL;
  for (int col = dlg->left; col <= dlg->right; col++)
  {
    for ( c = sheet->getFirstCellColumn(col); c != NULL;
         c = sheet->getNextCellDown(c->column(), c->row()) )
      {
        applyFormat(c);
      }
  }
}

void CellFormatPageMisc::applyRow( )
{
  Sheet* sheet = dlg->getSheet();
  Cell* c= NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for ( c = sheet->getFirstCellRow(row); c != NULL;
         c = sheet->getNextCellRight(c->column(), c->row()) )
    {
      applyFormat(c);
    }
  }
}

void CellFormatPageMisc::applyFormat( Cell *_obj )
{
    if ( dlg->bDontPrintText!=dontPrintText->isChecked())
      _obj->setDontPrintText(dontPrintText->isChecked());
}

void CellFormatPageMisc::slotStyle( int _i )
{
  if ( dlg->isSingleCell() && _i != idStyleNormal && _i != idStyleUndef )
    actionText->setEnabled( true );
  else
    actionText->setEnabled( false );
}


CellFormatPageFont::CellFormatPageFont( QWidget* parent, CellFormatDialog *_dlg ) : QWidget ( parent )
{
  dlg = _dlg;

  bTextColorUndefined = !dlg->bTextColor;

  QVBoxLayout* grid = new QVBoxLayout( this, 6 );

  box1 = new QGroupBox( this, "Box1");
  box1->setTitle(i18n("Requested Font"));
  QGridLayout *grid2 = new QGridLayout(box1,7,3,KDialog::marginHint(), KDialog::spacingHint());
  int fHeight = box1->fontMetrics().height();
  grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

  family_label = new QLabel(box1,"family");
  family_label->setText(i18n("Family:"));
  grid2->addWidget(family_label,1,0);

  size_label = new QLabel(box1,"size");
  size_label->setText(i18n("Size:"));
  grid2->addWidget(size_label,1,2);

  weight_label = new QLabel(box1,"weight");
  weight_label->setText(i18n("Weight:"));
  grid2->addWidget(weight_label,3,1);

  QLabel *tmpQLabel = new QLabel( box1, "Label_1" );
  tmpQLabel->setText( i18n("Text color:") );
  grid2->addWidget(tmpQLabel,5,1);

  textColorButton = new KColorButton( box1, "textColor" );
  grid2->addWidget(textColorButton,6,1);

  connect( textColorButton, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotSetTextColor( const QColor & ) ) );



  style_label = new QLabel(box1,"style");
  style_label->setText(i18n("Style:"));
  grid2->addWidget(style_label,1,1);

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

        if ( !family_combo->findItem(dlg->textFontFamily))
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

  grid2->addMultiCellWidget(family_combo,3,6,0,0);

  connect( family_combo, SIGNAL(highlighted(const QString &)),
           SLOT(family_chosen_slot(const QString &)) );

  searchFont=new KLineEdit(box1);
  grid2->addWidget(searchFont,2,0);
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
  grid2->addWidget(size_combo,2,2);
  connect( size_combo, SIGNAL(activated(const QString &)),
           SLOT(size_chosen_slot(const QString &)) );
  connect( size_combo ,SIGNAL( textChanged(const QString &)),
        this,SLOT(size_chosen_slot(const QString &)));

  weight_combo = new QComboBox( box1, "Weight" );
  weight_combo->insertItem( "", 0 );
  weight_combo->insertItem( i18n("Normal") );
  weight_combo->insertItem( i18n("Bold") );

  weight_combo->setInsertionPolicy(QComboBox::NoInsertion);
  grid2->addWidget(weight_combo,4,1);
  connect( weight_combo, SIGNAL(activated(const QString &)),
           SLOT(weight_chosen_slot(const QString &)) );

  style_combo = new QComboBox( box1, "Style" );
  style_combo->insertItem( "", 0 );
  style_combo->insertItem( i18n("Roman") );
  style_combo->insertItem( i18n("Italic"), 2 );
  grid2->addWidget(style_combo,2,1);
  style_combo->setInsertionPolicy(QComboBox::NoInsertion);
  connect( style_combo, SIGNAL(activated(const QString &)),
           SLOT(style_chosen_slot(const QString &)) );

  strike = new QCheckBox(i18n("Strike out"),box1);
  grid2->addWidget(strike,6,2);
  strike->setChecked(dlg->strike);
  connect( strike, SIGNAL( clicked()),
           SLOT(strike_chosen_slot()) );

  underline = new QCheckBox(i18n("Underline"),box1);
  grid2->addWidget(underline,4,2);
  underline->setChecked(dlg->underline);
  connect( underline, SIGNAL( clicked()),
           SLOT(underline_chosen_slot()) );


  grid->addWidget(box1);

  box1 = new QGroupBox(this, "Box2");
  box1->setTitle(i18n("Actual Font"));
  grid2 = new QGridLayout(box1,4,5,KDialog::marginHint(), KDialog::spacingHint());
  grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

  actual_family_label = new QLabel(box1,"afamily");
  actual_family_label->setText(i18n("Family:"));
  grid2->addWidget(actual_family_label,1,0);

  actual_family_label_data = new QLabel(box1,"afamilyd");
  grid2->addWidget(actual_family_label_data,1,1);

  actual_size_label = new QLabel(box1,"asize");
  actual_size_label->setText(i18n("Size:"));
  grid2->addWidget(actual_size_label,2,0);

  actual_size_label_data = new QLabel(box1,"asized");
  grid2->addWidget(actual_size_label_data,2,1);

  actual_weight_label = new QLabel(box1,"aweight");
  actual_weight_label->setText(i18n("Weight:"));
  grid2->addWidget(actual_weight_label,3,0);

  actual_weight_label_data = new QLabel(box1,"aweightd");
  grid2->addWidget(actual_weight_label_data,3,1);

  actual_style_label = new QLabel(box1,"astyle");
  actual_style_label->setText(i18n("Style:"));
  grid2->addWidget(actual_style_label,4,0);

  actual_style_label_data = new QLabel(box1,"astyled");
  grid2->addWidget(actual_style_label_data,4,1);


  example_label = new QLabel(box1,"examples");
  example_label->setFont(selFont);
  example_label->setAlignment(AlignCenter);
  example_label->setBackgroundColor(colorGroup().base());
  example_label->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
  example_label->setLineWidth( 1 );
  example_label->setText(i18n("Dolor Ipse"));
  //  example_label->setAutoResize(true);
  grid2->addMultiCellWidget(example_label,1,4,2,3);
  connect(this,SIGNAL(fontSelected( const QFont&  )),
          this,SLOT(display_example( const QFont&)));


  grid->addWidget(box1);
  setCombos();
  display_example( selFont );
  fontChanged=false;
  this->resize( 400, 400 );
}

void CellFormatPageFont::slotSearchFont(const QString &_text)
{
  QString result;
  result = listFont.makeCompletion( _text );
  if ( !result.isNull() )
    family_combo->setCurrentItem( family_combo->index( family_combo->findItem( result ) ) );
}

void CellFormatPageFont::slotSetTextColor( const QColor &_color )
{
  textColor = _color;
  bTextColorUndefined = false;
}

void CellFormatPageFont::apply( CustomStyle * style )
{
  if ( !bTextColorUndefined && textColor != dlg->textColor )
    style->changeTextColor( textColor );

  if ( ( size_combo->currentItem() != 0 )
       && ( dlg->textFontSize != selFont.pointSize() ) )
    style->changeFontSize( selFont.pointSize() );

  if ( ( selFont.family() != dlg->textFontFamily )
       && !family_combo->currentText().isEmpty() )
    style->changeFontFamily( selFont.family() );

  uint flags = 0;

  if ( weight_combo->currentItem() != 0 && selFont.bold() )
    flags |= Style::FBold;
  else
    flags &= ~(uint) Style::FBold;

  if ( style_combo->currentItem() != 0 && selFont.italic() )
    flags |= Style::FItalic;
  else
    flags &= ~(uint) Style::FItalic;

  if ( strike->isChecked() )
    flags |= Style::FStrike;
  else
    flags &= ~(uint) Style::FStrike;

  if ( underline->isChecked() )
    flags |= Style::FUnderline;
  else
    flags &= ~(uint) Style::FUnderline;

  style->changeFontFlags( flags );
}

void CellFormatPageFont::apply( ColumnFormat *_obj)
{
  Sheet* sheet = dlg->getSheet();
  Cell* c= NULL;
  for (int col = dlg->left; col <= dlg->right; col++)
  {
    for ( c = sheet->getFirstCellColumn(col); c != NULL;
         c = sheet->getNextCellDown(c->column(), c->row()) )
    {
      if ( !bTextColorUndefined )
      {
        c->clearProperty(Cell::PTextPen);
        c->clearNoFallBackProperties( Cell::PTextPen );
      }
      if (fontChanged)
      {
        c->clearProperty(Cell::PFont);
        c->clearNoFallBackProperties( Cell::PFont );
      }
    }
  }

  applyFormat(_obj);
  RowFormat* rw =dlg->getSheet()->firstRow();
  for ( ; rw; rw = rw->next() )
  {
    if ( !rw->isDefault() && (rw->hasProperty(Cell::PFont) ))
    {
      for ( int i=dlg->left;i<=dlg->right;i++)
      {
        Cell *cell = dlg->getSheet()->nonDefaultCell( i, rw->row() );
        applyFormat(cell );
      }
    }
  }
}

void CellFormatPageFont::apply( RowFormat *_obj)
{
  Sheet* sheet = dlg->getSheet();
  Cell* c= NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for ( c = sheet->getFirstCellRow(row); c != NULL;
         c = sheet->getNextCellRight(c->column(), c->row()) )
    {
      if ( !bTextColorUndefined )
      {
        c->clearProperty(Cell::PTextPen);
        c->clearNoFallBackProperties( Cell::PTextPen );
      }
      if (fontChanged)
      {
        c->clearProperty(Cell::PFont);
        c->clearNoFallBackProperties( Cell::PFont );
      }
    }
  }
  applyFormat(_obj);
}


void CellFormatPageFont::apply( Cell *_obj )
{
  applyFormat(_obj);
}

void CellFormatPageFont::applyFormat( Format *_obj )
{
  if ( !bTextColorUndefined && textColor != dlg->textColor )
    _obj->setTextColor( textColor );
  if (fontChanged)
  {
    if ( ( size_combo->currentItem() != 0 )
         && ( dlg->textFontSize != selFont.pointSize() ) )
      _obj->setTextFontSize( selFont.pointSize() );
    if ( ( selFont.family() != dlg->textFontFamily ) && ( !family_combo->currentText().isEmpty() ) )
      _obj->setTextFontFamily( selFont.family() );
    if ( weight_combo->currentItem() != 0 )
      _obj->setTextFontBold( selFont.bold() );
    if ( style_combo->currentItem() != 0 )
      _obj->setTextFontItalic( selFont.italic() );
    _obj->setTextFontStrike( strike->isChecked() );
    _obj->setTextFontUnderline(underline->isChecked() );
  }
}

void CellFormatPageFont::underline_chosen_slot()
{
   selFont.setUnderline( underline->isChecked() );
   emit fontSelected(selFont);
}

void CellFormatPageFont::strike_chosen_slot()
{
   selFont.setStrikeOut( strike->isChecked() );
   emit fontSelected(selFont);
}

void CellFormatPageFont::family_chosen_slot(const QString & family)
{
  selFont.setFamily(family);
  emit fontSelected(selFont);
}

void CellFormatPageFont::size_chosen_slot(const QString & size)
{
  QString size_string = size;

  selFont.setPointSize(size_string.toInt());
  emit fontSelected(selFont);
}

void CellFormatPageFont::weight_chosen_slot(const QString & weight)
{
  QString weight_string = weight;

  if ( weight_string == i18n("Normal"))
    selFont.setBold(false);
  if ( weight_string == i18n("Bold"))
    selFont.setBold(true);
  emit fontSelected(selFont);
}

void CellFormatPageFont::style_chosen_slot(const QString & style)
{
  QString style_string = style;

  if ( style_string == i18n("Roman"))
    selFont.setItalic(false);
  if ( style_string == i18n("Italic"))
    selFont.setItalic(true);
  emit fontSelected(selFont);
}


void CellFormatPageFont::display_example(const QFont& font)
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
    actual_style_label_data->setText(i18n("Italic"));
  else
    actual_style_label_data->setText(i18n("Roman"));
}

void CellFormatPageFont::setCombos()
{
 QString string;
 QComboBox* combo;
 int number_of_entries;
 bool found;

 if ( dlg->bTextColor )
   textColor = dlg->textColor;
 else
   textColor = colorGroup().text();

 if ( !textColor.isValid() )
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

CellFormatPagePosition::CellFormatPagePosition( QWidget* parent, CellFormatDialog *_dlg )
  : QWidget( parent ),
    dlg( _dlg )
{
    QGridLayout *grid3 = new QGridLayout(this, 4, 2, KDialog::marginHint(), KDialog::spacingHint());
    QButtonGroup *grp = new QButtonGroup( i18n("Horizontal"), this);
    grp->setRadioButtonExclusive( true );

    QGridLayout *grid2 = new QGridLayout(grp, 4, 2, KDialog::marginHint(), KDialog::spacingHint());
    int fHeight = grp->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title
    standard = new QRadioButton( i18n("Standard"), grp );
    grid2->addWidget(standard, 2, 0);
    left = new QRadioButton( i18n("Left"), grp );
    grid2->addWidget(left, 1, 1);
    center = new QRadioButton( i18n("Center"), grp );
    grid2->addWidget(center, 2, 1);
    right = new QRadioButton( i18n("Right"), grp );
    grid2->addWidget(right, 3, 1);
    grid3->addWidget(grp, 0, 0);

    if ( dlg->alignX == Cell::Left )
        left->setChecked( true );
    else if ( dlg->alignX == Cell::Center )
        center->setChecked( true );
    else if ( dlg->alignX == Cell::Right )
        right->setChecked( true );
    else if ( dlg->alignX == Cell::Undefined )
        standard->setChecked( true );

    connect(grp,  SIGNAL(clicked(int)), this, SLOT(slotStateChanged(int)));

    grp = new QButtonGroup( i18n("Vertical"), this);
    grp->setRadioButtonExclusive( true );

    grid2 = new QGridLayout(grp, 4, 1, KDialog::marginHint(), KDialog::spacingHint());
    fHeight = grp->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title
    top = new QRadioButton( i18n("Top"), grp );
    grid2->addWidget(top, 1, 0);
    middle = new QRadioButton( i18n("Middle"), grp );
    grid2->addWidget(middle, 2, 0);
    bottom = new QRadioButton( i18n("Bottom"), grp );
    grid2->addWidget(bottom, 3, 0);
    grid3->addWidget(grp, 0, 1);

    if ( dlg->alignY ==Cell::Top )
        top->setChecked( true );
    else if ( dlg->alignY ==Cell::Middle )
        middle->setChecked(true );
    else if ( dlg->alignY ==Cell::Bottom )
        bottom->setChecked( true );

    grp = new QButtonGroup( i18n("Text Option"), this);
    //grp->setRadioButtonExclusive( false );
    grid2 = new QGridLayout(grp, 3, 1, KDialog::marginHint(), KDialog::spacingHint());
    fHeight = grp->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title
    multi = new QCheckBox( i18n("Wrap text"), grp);

    grid2->addWidget(multi, 1, 0);
    multi->setChecked(dlg->bMultiRow);

    vertical = new QCheckBox( i18n("Vertical text"), grp);
    grid2->addWidget(vertical, 2, 0);
    vertical->setChecked(dlg->bVerticalText);

    grid3->addWidget(grp, 1, 0);

    grp = new QButtonGroup(i18n("Rotation"), this);

    grid2 = new QGridLayout(grp, 2, 1, KDialog::marginHint(), KDialog::spacingHint());
    fHeight = grp->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title
    angleRotation=new KIntNumInput((-dlg->textRotation), grp, 10);
    angleRotation->setLabel(i18n("Angle:"));
    angleRotation->setRange(-90, 90, 1);
    angleRotation->setSuffix(QString::fromUtf8("°"));

    grid2->addWidget(angleRotation, 1, 0);
    grid3->addWidget(grp, 1, 1);
    if ( dlg->textRotation != 0 )
    {
        multi->setEnabled(false );
	vertical->setEnabled(false);
    }

    grp = new QButtonGroup( i18n("Merge Cells"), this);
    grid2 = new QGridLayout(grp, 2, 1, KDialog::marginHint(), KDialog::spacingHint());

    fHeight = grp->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

    mergeCell=new QCheckBox(i18n("Merge cells"), grp);
    mergeCell->setChecked(dlg->isMerged);
    mergeCell->setEnabled(!dlg->oneCell && ((!dlg->isRowSelected) && (!dlg->isColumnSelected)));
    grid2->addWidget(mergeCell, 1, 0);
    grid3->addWidget(grp, 2, 0);

    grp = new QButtonGroup( i18n("Indent"), this);
    grid2 = new QGridLayout(grp, 2, 1, KDialog::marginHint(), KDialog::spacingHint());
    fHeight = grp->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

    m_indent = new KoUnitDoubleSpinBox( grp, 0.0,  400.0, 10.0,dlg->indent,dlg->getDoc()->unit() );
    grid2->addWidget(m_indent, 1, 0);
    grid3->addWidget(grp, 2, 1);

    grp = new QButtonGroup( i18n("Size of Cell"), this);
    grid2 = new QGridLayout(grp, 3, 4, KDialog::marginHint(), KDialog::spacingHint());
    fHeight = grp->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

    QLabel *tmpLabel=new QLabel(grp, "label");
    tmpLabel->setText(i18n("Width:"));
    grid2->addWidget(tmpLabel, 1, 0);

    width = new KoUnitDoubleSpinBox( grp );
    width->setValue ( dlg->widthSize );
    width->setUnit( dlg->getDoc()->unit() );
    //to ensure, that we don't get rounding problems, we store the displayed value (for later check for changes)
    dlg->widthSize = width->value();

    if ( dlg->isRowSelected )
        width->setEnabled(false);

    grid2->addWidget(width, 1, 1);
    defaultWidth=new QCheckBox(i18n("Default width (%1 %2)").arg(KoUnit::toUserValue( 60, dlg->getDoc()->unit())).arg(dlg->getDoc()->unitName()), grp);
    if ( dlg->isRowSelected )
        defaultWidth->setEnabled(false);

    grid2->addMultiCellWidget(defaultWidth, 2, 2, 0, 1);

    tmpLabel=new QLabel(grp, "label1");
    tmpLabel->setText(i18n("Height:"));
    grid2->addWidget(tmpLabel, 1, 2);

    height=new KoUnitDoubleSpinBox( grp );
    height->setValue( dlg->heightSize );
    height->setUnit(  dlg->getDoc()->unit() );
    //to ensure, that we don't get rounding problems, we store the displayed value (for later check for changes)
    dlg->heightSize = height->value();

    if ( dlg->isColumnSelected )
        height->setEnabled(false);

    grid2->addWidget(height, 1, 3);

    defaultHeight=new QCheckBox(i18n("Default height (%1 %2)").arg(KoUnit::toUserValue(  20 , dlg->getDoc()->unit())).arg(dlg->getDoc()->unitName()), grp);
    if ( dlg->isColumnSelected )
        defaultHeight->setEnabled(false);

    grid2->addMultiCellWidget(defaultHeight, 2, 2, 2, 3);

    grid3->addMultiCellWidget(grp, 3, 3, 0, 1);

    connect(defaultWidth , SIGNAL(clicked() ),this, SLOT(slotChangeWidthState()));
    connect(defaultHeight , SIGNAL(clicked() ),this, SLOT(slotChangeHeightState()));
    connect(vertical , SIGNAL(clicked() ),this, SLOT(slotChangeVerticalState()));
    connect(multi , SIGNAL(clicked() ), this, SLOT(slotChangeMultiState()));
    connect(angleRotation, SIGNAL(valueChanged(int)), this, SLOT(slotChangeAngle(int)));

    slotStateChanged( 0 );
    m_bOptionText = false;
    this->resize( 400, 400 );
}

void CellFormatPagePosition::slotChangeMultiState()
{
    m_bOptionText = true;
    if (vertical->isChecked())
    {
        vertical->setChecked(false);
    }
}

void CellFormatPagePosition::slotChangeVerticalState()
{
    m_bOptionText=true;
    if (multi->isChecked())
    {
        multi->setChecked(false);
    }

}

void CellFormatPagePosition::slotStateChanged(int)
{
    if (right->isChecked() || center->isChecked())
        m_indent->setEnabled(false);
    else
        m_indent->setEnabled(true);

}
bool CellFormatPagePosition::getMergedCellState() const
{
    return  mergeCell->isChecked();
}

void CellFormatPagePosition::slotChangeWidthState()
{
    if ( defaultWidth->isChecked())
        width->setEnabled(false);
    else
        width->setEnabled(true);
}

void CellFormatPagePosition::slotChangeHeightState()
{
    if ( defaultHeight->isChecked())
        height->setEnabled(false);
    else
        height->setEnabled(true);
}

void CellFormatPagePosition::slotChangeAngle(int _angle)
{
  if ( _angle == 0 )
  {
    multi->setEnabled( true );
    vertical->setEnabled( true );
  }
  else
  {
    multi->setEnabled( false );
    vertical->setEnabled( false );
  }
}

void CellFormatPagePosition::apply( CustomStyle * style )
{
  if ( top->isChecked() && dlg->alignY != Cell::Top )
    style->changeAlignY( Cell::Top );
  else if ( bottom->isChecked() && dlg->alignY != Cell::Bottom )
    style->changeAlignY( Cell::Bottom );
  else if ( middle->isChecked() && dlg->alignY != Cell::Middle )
    style->changeAlignY( Cell::Middle );

  if ( left->isChecked() && dlg->alignX != Cell::Left )
    style->changeAlignX( Cell::Left );
  else if ( right->isChecked() && dlg->alignX != Cell::Right )
    style->changeAlignX( Cell::Right );
  else if ( center->isChecked() && dlg->alignX != Cell::Center )
    style->changeAlignX( Cell::Center );
  else if ( standard->isChecked() && dlg->alignX != Cell::Undefined )
    style->changeAlignX( Cell::Undefined );

  if ( m_bOptionText )
  {
    if ( multi->isEnabled() )
    {
      if ( multi->isChecked() )
        style->addProperty( Style::PMultiRow );
      else
        style->removeProperty( Style::PMultiRow );
    }
  }

  if ( m_bOptionText )
  {
    if ( vertical->isEnabled() )
    {
      if ( vertical->isChecked() )
        style->addProperty( Style::PVerticalText );
      else
        style->removeProperty( Style::PVerticalText );
    }
  }

  if ( dlg->textRotation != angleRotation->value() )
    style->changeRotateAngle( (-angleRotation->value()) );

  if ( m_indent->isEnabled()
       && dlg->indent != m_indent->value() )
    style->changeIndent( m_indent->value() );
}

void CellFormatPagePosition::apply( ColumnFormat *_obj )
{
  Format::Align  ax;
  Format::AlignY ay;

  if ( top->isChecked() )
    ay = Cell::Top;
  else if ( bottom->isChecked() )
    ay = Cell::Bottom;
  else if ( middle->isChecked() )
    ay = Cell::Middle;
  else
    ay = Cell::Middle; //Default, just in case

  if ( left->isChecked() )
    ax = Cell::Left;
  else if ( right->isChecked() )
    ax = Cell::Right;
  else if ( center->isChecked() )
    ax = Cell::Center;
  else if ( standard->isChecked() )
    ax = Cell::Undefined;
  else
    ax = Cell::Undefined; //Default, just in case


  Sheet * sheet = dlg->getSheet();
  Cell * c = NULL;
  for ( int col = dlg->left; col <= dlg->right; ++col)
  {
    for ( c = sheet->getFirstCellColumn(col); c != NULL;
         c = sheet->getNextCellDown(c->column(), c->row()) )
    {
      if ( m_indent->isEnabled()
	   && dlg->indent != m_indent->value() )

      {
        c->clearProperty( Cell::PIndent );
        c->clearNoFallBackProperties( Cell::PIndent );
      }
      if ( ax != dlg->alignX )
      {
        c->clearProperty(Cell::PAlign);
        c->clearNoFallBackProperties( Cell::PAlign );
      }
      if ( ay != dlg->alignY )
      {
        c->clearProperty(Cell::PAlignY);
        c->clearNoFallBackProperties( Cell::PAlignY );
      }
      if ( m_bOptionText)
      {
        c->clearProperty(Cell::PMultiRow);
        c->clearNoFallBackProperties( Cell::PMultiRow );
      }
      if ( m_bOptionText)
      {
        c->clearProperty(Cell::PVerticalText);
        c->clearNoFallBackProperties( Cell::PVerticalText );
      }

      if (dlg->textRotation != angleRotation->value())
      {
        c->clearProperty(Cell::PAngle);
        c->clearNoFallBackProperties( Cell::PAngle );
      }
    }
  }

  applyFormat( _obj );

  RowFormat* rw =dlg->getSheet()->firstRow();
  for ( ; rw; rw = rw->next() )
  {
    if ( !rw->isDefault() && ( rw->hasProperty(Cell::PAngle) ||
                               rw->hasProperty(Cell::PVerticalText) ||
                               rw->hasProperty(Cell::PMultiRow) ||
                               rw->hasProperty(Cell::PAlignY) ||
                               rw->hasProperty(Cell::PAlign) ||
                               rw->hasProperty(Cell::PIndent) ) )
    {
      for ( int i = dlg->left; i <= dlg->right; ++i )
      {
        Cell * cell = dlg->getSheet()->nonDefaultCell( i, rw->row() );
        applyFormat( cell );
      }
    }
  }
}

void CellFormatPagePosition::apply( RowFormat *_obj )
{
  Format::Align  ax;
  Format::AlignY ay;

  if ( top->isChecked() )
    ay = Cell::Top;
  else if ( bottom->isChecked() )
    ay = Cell::Bottom;
  else if ( middle->isChecked() )
    ay = Cell::Middle;
  else
    ay = Cell::Middle; //Default, just in case

  if ( left->isChecked() )
    ax = Cell::Left;
  else if ( right->isChecked() )
    ax = Cell::Right;
  else if ( center->isChecked() )
    ax = Cell::Center;
  else if ( standard->isChecked() )
    ax = Cell::Undefined;
  else
    ax = Cell::Undefined; //Default, just in case

  Sheet* sheet = dlg->getSheet();
  Cell* c= NULL;
  for (int row = dlg->top; row <= dlg->bottom; row++)
  {
    for ( c = sheet->getFirstCellRow(row); c != NULL;
         c = sheet->getNextCellRight(c->column(), c->row()) )
    {
      if ( m_indent->isEnabled()
	   && dlg->indent != m_indent->value() )
      {
        c->clearProperty(Cell::PIndent);
        c->clearNoFallBackProperties( Cell::PIndent );
      }
      if ( ax != dlg->alignX )
      {
        c->clearProperty(Cell::PAlign);
        c->clearNoFallBackProperties( Cell::PAlign );
      }
      if ( ay != dlg->alignY )
      {
        c->clearProperty(Cell::PAlignY);
        c->clearNoFallBackProperties( Cell::PAlignY );
      }
      if ( m_bOptionText)
      {
        c->clearProperty(Cell::PMultiRow);
        c->clearNoFallBackProperties( Cell::PMultiRow );
      }
      if ( m_bOptionText)
      {
        c->clearProperty(Cell::PVerticalText);
        c->clearNoFallBackProperties( Cell::PVerticalText );
      }
      if (dlg->textRotation!=angleRotation->value())
      {
        c->clearProperty(Cell::PAngle);
        c->clearNoFallBackProperties( Cell::PAngle );
      }
    }
  }

  applyFormat( _obj );
}


void CellFormatPagePosition::apply( Cell *_obj )
{
  applyFormat( _obj );
}

void CellFormatPagePosition::applyFormat( Format * _obj )
{
  Format::Align  ax;
  Format::AlignY ay;

  if ( top->isChecked() )
    ay = Cell::Top;
  else if ( bottom->isChecked() )
    ay = Cell::Bottom;
  else if ( middle->isChecked() )
    ay = Cell::Middle;
  else
    ay = Cell::Middle; // Default, just in case

  if ( left->isChecked() )
    ax = Cell::Left;
  else if ( right->isChecked() )
    ax = Cell::Right;
  else if ( center->isChecked() )
    ax = Cell::Center;
  else if ( standard->isChecked() )
    ax = Cell::Undefined;
  else
    ax = Cell::Undefined; //Default, just in case

  if ( top->isChecked() && ay != dlg->alignY )
    _obj->setAlignY( Cell::Top );
  else if ( bottom->isChecked() && ay != dlg->alignY )
    _obj->setAlignY( Cell::Bottom );
  else if ( middle->isChecked() && ay != dlg->alignY )
    _obj->setAlignY( Cell::Middle );

  if ( left->isChecked() && ax != dlg->alignX )
    _obj->setAlign( Cell::Left );
  else if ( right->isChecked() && ax != dlg->alignX )
    _obj->setAlign( Cell::Right );
  else if ( center->isChecked() && ax != dlg->alignX )
    _obj->setAlign( Cell::Center );
  else if ( standard->isChecked() && ax != dlg->alignX )
    _obj->setAlign( Cell::Undefined );

  if ( m_bOptionText )
  {
    if ( multi->isEnabled() )
      _obj->setMultiRow( multi->isChecked() );
    else
      _obj->setMultiRow( false );
  }

  if ( m_bOptionText )
  {
    if ( vertical->isEnabled() )
      _obj->setVerticalText( vertical->isChecked() );
    else
      _obj->setVerticalText( false );
  }

  if ( dlg->textRotation!=angleRotation->value() )
    _obj->setAngle( (-angleRotation->value() ) );
  if ( m_indent->isEnabled()
       && dlg->indent != m_indent->value() )
    _obj->setIndent( m_indent->value() );
}

double CellFormatPagePosition::getSizeHeight() const
{
  if ( defaultHeight->isChecked() )
      return 20.0;
  else
      return height->value();
}

double CellFormatPagePosition::getSizeWidth() const
{
  if ( defaultWidth->isChecked() )
        return 60.0;
  else
        return width->value();
}

BorderButton::BorderButton( QWidget *parent, const char *_name ) : QPushButton(parent,_name)
{
  penStyle = Qt::NoPen;
  penWidth = 1;
  penColor = colorGroup().text();
  setToggleButton( true );
  setOn( false);
  setChanged(false);
}
void BorderButton::mousePressEvent( QMouseEvent * )
{

  this->setOn(!isOn());
  emit clicked( this );
}

void BorderButton::setUndefined()
{
 setPenStyle(SolidLine );
 setPenWidth(1);
 setColor(colorGroup().midlight());
}


void BorderButton::unselect()
{
  setOn(false);
  setPenWidth(1);
  setPenStyle(Qt::NoPen);
  setColor( colorGroup().text() );
  setChanged(true);
}

Border::Border( QWidget *parent, const char *_name,bool _oneCol, bool _oneRow )
    : QFrame( parent, _name )
{
  oneCol=_oneCol;
  oneRow=_oneRow;
}


#define OFFSETX 5
#define OFFSETY 5
void Border::paintEvent( QPaintEvent *_ev )
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
  if (oneCol==false)
  {
        painter.drawLine( width()/2, OFFSETY-5, width()/2 , OFFSETY );
        painter.drawLine( width()/2-5, OFFSETY, width()/2+5 , OFFSETY );
        painter.drawLine( width()/2, height()-OFFSETY, width()/2 , height() );
        painter.drawLine( width()/2-5, height()-OFFSETY, width()/2+5 , height()-OFFSETY );
  }
  if (oneRow==false)
  {
        painter.drawLine( OFFSETX-5, height()/2, OFFSETX , height()/2 );
        painter.drawLine( OFFSETX, height()/2-5, OFFSETX , height()/2+5 );
        painter.drawLine( width()-OFFSETX, height()/2, width(), height()/2 );
        painter.drawLine( width()-OFFSETX, height()/2-5, width()-OFFSETX , height()/2+5 );
  }
  painter.end();
  emit redraw();
}

void Border::mousePressEvent( QMouseEvent* _ev )
{
  emit choosearea(_ev);
}

CellFormatPageBorder::CellFormatPageBorder( QWidget* parent, CellFormatDialog *_dlg )
  : QWidget( parent ),
    dlg( _dlg )
{
  sheet = dlg->getSheet();

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

void CellFormatPageBorder::InitializeGrids()
{
  QGridLayout *grid = new QGridLayout(this,5,2,KDialog::marginHint(), KDialog::spacingHint());
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
  grid2 = new QGridLayout(tmpQGroupBox,6,5,KDialog::marginHint(), KDialog::spacingHint());
  int fHeight = tmpQGroupBox->fontMetrics().height();
  grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

  area=new Border(tmpQGroupBox,"area",dlg->oneCol,dlg->oneRow);
  grid2->addMultiCellWidget(area,2,4,1,3);
  area->setBackgroundColor( colorGroup().base() );

  /* initailize the buttons that are in this box */
  for (int i=BorderType_Top; i < BorderType_END; i++)
  {
    borderButtons[i] = new BorderButton(tmpQGroupBox,
                                               borderButtonNames[i]);
    loadIcon(borderButtonIconNames[i], borderButtons[i]);
    grid2->addWidget(borderButtons[i], borderButtonPositions[i][0] + 1,
                     borderButtonPositions[i][1]);
  }

  grid->addMultiCellWidget(tmpQGroupBox,0,2,0,0);

  /* the remove, all, and outline border buttons are in a second box down
     below.*/

  tmpQGroupBox = new QGroupBox( this, "GroupBox_3" );
  tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
  tmpQGroupBox->setTitle( i18n("Preselect") );
  tmpQGroupBox->setAlignment( AlignLeft );

  grid2 = new QGridLayout(tmpQGroupBox,1,3,KDialog::marginHint(), KDialog::spacingHint());

  /* the "all" button is different depending on what kind of region is currently
     selected */
  if ((dlg->oneRow==true)&&(dlg->oneCol==false))
  {
    shortcutButtonIconNames[BorderShortcutType_All] = "border_vertical";
  }
  else if ((dlg->oneRow==false)&&(dlg->oneCol==true))
  {
    shortcutButtonIconNames[BorderShortcutType_All] = "border_horizontal";
  }
  else
  {
    shortcutButtonIconNames[BorderShortcutType_All] = "border_inside";
  }

  for (int i=BorderShortcutType_Remove; i < BorderShortcutType_END; i++)
  {
    shortcutButtons[i] = new BorderButton(tmpQGroupBox,
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

  grid2 = new QGridLayout(tmpQGroupBox,7,2,KDialog::marginHint(), KDialog::spacingHint());
  fHeight = tmpQGroupBox->fontMetrics().height();
  grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

  char name[] = "PatternXX";
  Q_ASSERT(NUM_BORDER_PATTERNS < 100);

  for (int i=0; i < NUM_BORDER_PATTERNS; i++)
  {
    name[7] = '0' + (i+1) / 10;
    name[8] = '0' + (i+1) % 10;
    pattern[i] = new PatternSelect( tmpQGroupBox, name );
    pattern[i]->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(pattern[i], i % 5 + 1, i / 5);
    /* this puts them in the pattern:
       1  6
       2  7
       3  8
       4  9
       5  10
    */
  }

  color = new KColorButton (tmpQGroupBox, "PushButton_1" );
  grid2->addWidget(color,7,1);

  QLabel *tmpQLabel = new QLabel( tmpQGroupBox, "Label_6" );
  tmpQLabel->setText( i18n("Color:") );
  grid2->addWidget(tmpQLabel,7,0);

  /* tack on the 'customize' border pattern selector */
  QGridLayout *grid3 = new QGridLayout( this, 2, 2, KDialog::marginHint(), KDialog::spacingHint() );
  customize  = new QCheckBox(i18n("Customize"),tmpQGroupBox);
  grid3->addWidget(customize,0,0);
  connect( customize, SIGNAL( clicked()), SLOT(cutomize_chosen_slot()) );

  size=new QComboBox(true,tmpQGroupBox);
  grid3->addWidget(size,1,1);
  size->setValidator(new KIntValidator( size ));
  QString tmp;
  for ( int i=0;i<10;i++)
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

  grid2->addMultiCell(grid3,6,6,0,1);
  grid->addMultiCellWidget(tmpQGroupBox,0,3,1,1);

  /* Now the preview box is put together */
  tmpQGroupBox = new QGroupBox(this, "GroupBox_4" );
  tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
  tmpQGroupBox->setTitle( i18n("Preview") );
  tmpQGroupBox->setAlignment( AlignLeft );

  grid2 = new QGridLayout(tmpQGroupBox,1,1,KDialog::marginHint(), KDialog::spacingHint());
  fHeight = tmpQGroupBox->fontMetrics().height();
  grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

  preview = new PatternSelect( tmpQGroupBox, "Pattern_preview" );
  preview->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  grid2->addWidget(preview,1,0);

  grid->addWidget(tmpQGroupBox,4,1);
}

void CellFormatPageBorder::InitializeBorderButtons()
{
  for (int i=BorderType_Top; i < BorderType_END; i++)
  {
    if (dlg->borders[i].style != Qt::NoPen ||
       !dlg->borders[i].bStyle )
    {
      /* the horozontil and vertical buttons might be disabled depending on what
         kind of area is selected so check that first. */
      if ((dlg->oneRow == true && i == BorderType_Horizontal) ||
          (dlg->oneCol == true && i == BorderType_Vertical))
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

void CellFormatPageBorder::InitializePatterns()
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

void CellFormatPageBorder::SetConnections()
{
  connect( color, SIGNAL( changed( const QColor & ) ),
           this, SLOT( slotSetColorButton( const QColor & ) ) );

  for (int i=0; i < NUM_BORDER_PATTERNS; i++)
  {
    connect( pattern[i], SIGNAL( clicked( PatternSelect* ) ),
             this, SLOT( slotUnselect2( PatternSelect* ) ) );
  }

  for (int i = BorderType_Top; i < BorderType_END; i++)
  {
    connect( borderButtons[i], SIGNAL( clicked (BorderButton *) ),
             this, SLOT( changeState( BorderButton *) ) );
  }

  for (int i = BorderShortcutType_Remove; i < BorderShortcutType_END; i++)
  {
    connect( shortcutButtons[i], SIGNAL( clicked(BorderButton *) ),
             this, SLOT( preselect(BorderButton *) ) );
  }

  connect( area ,SIGNAL( redraw()),this,SLOT(draw()));
  connect( area ,SIGNAL( choosearea(QMouseEvent * )),
           this,SLOT( slotPressEvent(QMouseEvent *)));

  connect( style, SIGNAL( activated(int)), this, SLOT(slotChangeStyle(int)));
  connect( size, SIGNAL( textChanged(const QString &)),
           this, SLOT(slotChangeStyle(const QString &)));
  connect( size ,SIGNAL( activated(int)), this, SLOT(slotChangeStyle(int)));
}

void CellFormatPageBorder::cutomize_chosen_slot()
{
  if ( customize->isChecked() )
  {
    style->setEnabled( true );
    size->setEnabled( true );
    slotUnselect2( preview );
  }
  else
  {
    style->setEnabled( false );
    size->setEnabled( false );
    pattern[2]->slotSelect();
    preview->setPattern( black , 1, SolidLine );
  }
}

void CellFormatPageBorder::slotChangeStyle(const QString &)
{
  /* if they try putting text in the size box, then erase the line */
  slotChangeStyle(0);
}

void CellFormatPageBorder::slotChangeStyle(int)
{
  int index = style->currentItem();
  QString tmp;
  int penSize = size->currentText().toInt();
  if ( !penSize)
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

QPixmap CellFormatPageBorder::paintFormatPixmap(PenStyle _style)
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

void CellFormatPageBorder::loadIcon( QString _pix, BorderButton *_button)
{
  _button->setPixmap( QPixmap( KSBarIcon(_pix) ) );
}

void CellFormatPageBorder::applyOutline()
{
  if (borderButtons[BorderType_Horizontal]->isChanged())
    applyHorizontalOutline();

  if (borderButtons[BorderType_Vertical]->isChanged())
    applyVerticalOutline();

  if ( borderButtons[BorderType_Left]->isChanged() )
    applyLeftOutline();

  if ( borderButtons[BorderType_Right]->isChanged() )
    applyRightOutline();

  if ( borderButtons[BorderType_Top]->isChanged() )
    applyTopOutline();

  if ( borderButtons[BorderType_Bottom]->isChanged() )
    applyBottomOutline();

  if ( borderButtons[BorderType_RisingDiagonal]->isChanged() ||
       borderButtons[BorderType_FallingDiagonal]->isChanged() )
    applyDiagonalOutline();
}

void CellFormatPageBorder::applyTopOutline()
{
  BorderButton * top = borderButtons[BorderType_Top];

  QPen tmpPen( top->getColor(), top->getPenWidth(), top->getPenStyle());

  if ( dlg->getStyle() )
  {
    dlg->getStyle()->changeTopBorderPen( tmpPen );
  }
  else if ( !dlg->isRowSelected )
  {
    /* if a column is selected then _top will just be row 1 so there's no special
       handling */

    for ( int x = dlg->left; x <= dlg->right; x++ )
    {
      Cell *obj = dlg->getSheet()->nonDefaultCell( x, dlg->top );
      if ( obj->isObscuringForced() /* && dlg->isSingleCell() */ )
        obj = obj->obscuringCells().first();
      obj->setTopBorderPen( tmpPen );
    }
  }
  else if ( dlg->isRowSelected )
  {
    Cell* c = NULL;
    for ( c = sheet->getFirstCellRow(dlg->top); c != NULL;
         c = sheet->getNextCellRight(c->column(), c->row()) )
    {
      c->clearProperty(Cell::PTopBorder);
      c->clearNoFallBackProperties( Cell::PTopBorder );
    }

    RowFormat *obj=dlg->getSheet()->nonDefaultRowFormat(dlg->top-1);
    obj->setBottomBorderPen( tmpPen );
  }
}

void CellFormatPageBorder::applyBottomOutline()
{
  Sheet * sheet = dlg->getSheet();
  BorderButton * bottom = borderButtons[BorderType_Bottom];

  QPen tmpPen( bottom->getColor(), bottom->getPenWidth(), bottom->getPenStyle() );

  if ( dlg->getStyle() )
    dlg->getStyle()->changeBottomBorderPen( tmpPen );
  else if ( !dlg->isRowSelected && !dlg->isColumnSelected )
  {
    for ( int x = dlg->left; x <= dlg->right; x++ )
    {
      Cell *obj = dlg->getSheet()->nonDefaultCell( x, dlg->bottom );
      if ( obj->isObscuringForced() /* && dlg->isSingleCell() */ )
        obj = obj->obscuringCells().first();
      obj->setBottomBorderPen( tmpPen );
    }
  }
  else if ( dlg->isRowSelected )
  {
    Cell* c = NULL;
    for ( c = sheet->getFirstCellRow(dlg->bottom); c != NULL;
         c = sheet->getNextCellRight(c->column(), c->row()) )
    {
      c->clearProperty(Cell::PBottomBorder);
      c->clearNoFallBackProperties( Cell::PBottomBorder );
    }

    RowFormat *obj=dlg->getSheet()->nonDefaultRowFormat(dlg->bottom);
    obj->setBottomBorderPen( tmpPen );
  }
}

void CellFormatPageBorder::applyLeftOutline()
{
  BorderButton * left = borderButtons[BorderType_Left];
  QPen tmpPen( left->getColor(), left->getPenWidth(), left->getPenStyle() );

  if ( dlg->getStyle() )
    dlg->getStyle()->changeLeftBorderPen( tmpPen );
  else if ( !dlg->isColumnSelected )
  {
    for ( int y = dlg->top; y <= dlg->bottom; y++ )
    {
      Cell *obj = dlg->getSheet()->nonDefaultCell( dlg->left, y );
      if ( obj->isObscuringForced() /* && dlg->isSingleCell() */ )
        continue;
      obj->setLeftBorderPen( tmpPen );
    }
  }
  else
  {
    Cell* c = NULL;
    for ( c = sheet->getFirstCellColumn(dlg->left); c != NULL;
         c = sheet->getNextCellDown(c->column(), c->row()) )
    {
      c->clearProperty(Cell::PLeftBorder);
      c->clearNoFallBackProperties( Cell::PLeftBorder );
    }
    ColumnFormat *obj=dlg->getSheet()->nonDefaultColumnFormat(dlg->left);
    obj->setLeftBorderPen( tmpPen );

    RowFormat* rw =dlg->getSheet()->firstRow();
    for ( ; rw; rw = rw->next() )
    {
      if (rw->row()==dlg->left&& !rw->isDefault() &&
          (rw->hasProperty(Cell::PLeftBorder)  ))
      {
        for ( int i=dlg->left;i<=dlg->right;i++)
        {
          Cell *cell =
            dlg->getSheet()->nonDefaultCell( i, rw->row() );
          if ( cell->isObscuringForced() && dlg->isSingleCell() )
            continue;
          cell->setLeftBorderPen( tmpPen );
        }
      }
    }
  }
}

void CellFormatPageBorder::applyRightOutline()
{
  BorderButton* right = borderButtons[BorderType_Right];
  QPen tmpPen( right->getColor(), right->getPenWidth(), right->getPenStyle() );

  if ( dlg->getStyle() )
    dlg->getStyle()->changeRightBorderPen( tmpPen );
  else if ( (!dlg->isRowSelected) && (!dlg->isColumnSelected) )
  {
    for ( int y = dlg->top; y <= dlg->bottom; y++ )
    {
      Cell * obj = dlg->getSheet()->nonDefaultCell( dlg->right, y );
      if ( obj->isObscuringForced() /* && dlg->isSingleCell() */ )
        obj = obj->obscuringCells().first();
      obj->setRightBorderPen( tmpPen );
    }
  }
  else if (  dlg->isColumnSelected )
  {
    Cell* c = NULL;
    for ( c = sheet->getFirstCellColumn(dlg->right); c != NULL;
         c = sheet->getNextCellDown(c->column(), c->row()) )
    {
      c->clearProperty(Cell::PRightBorder);
      c->clearNoFallBackProperties( Cell::PRightBorder );
    }

    ColumnFormat *obj=dlg->getSheet()->nonDefaultColumnFormat(dlg->right);
    obj->setRightBorderPen(tmpPen);

    RowFormat* rw =dlg->getSheet()->firstRow();
    for ( ; rw; rw = rw->next() )
    {
      if (rw->row()==dlg->right&& !rw->isDefault() &&
          (rw->hasProperty(Cell::PRightBorder)  ))
      {
        for ( int i=dlg->left;i<=dlg->right;i++)
        {
          Cell *cell =
            dlg->getSheet()->nonDefaultCell( i, rw->row() );
          if ( cell->isObscuringForced() /* && dlg->isSingleCell() */ )
            cell = cell->obscuringCells().first();
          cell->setRightBorderPen( tmpPen );
        }
      }
    }
  }
}

void CellFormatPageBorder::applyDiagonalOutline()
{
  BorderButton * fallDiagonal = borderButtons[BorderType_FallingDiagonal];
  BorderButton * goUpDiagonal = borderButtons[BorderType_RisingDiagonal];
  QPen tmpPenFall( fallDiagonal->getColor(), fallDiagonal->getPenWidth(),
                   fallDiagonal->getPenStyle());
  QPen tmpPenGoUp( goUpDiagonal->getColor(), goUpDiagonal->getPenWidth(),
                   goUpDiagonal->getPenStyle());

  if ( dlg->getStyle() )
  {
    if ( fallDiagonal->isChanged() )
      dlg->getStyle()->changeFallBorderPen( tmpPenFall );
    if ( goUpDiagonal->isChanged() )
      dlg->getStyle()->changeGoUpBorderPen( tmpPenGoUp );
  }
  else if ( (!dlg->isRowSelected) && (!dlg->isColumnSelected) )
  {
    for ( int x = dlg->left; x <= dlg->right; x++ )
    {
      for ( int y = dlg->top; y <= dlg->bottom; y++ )
      {
        Cell *obj = dlg->getSheet()->nonDefaultCell( x, y );
        if ( fallDiagonal->isChanged() )
          obj->setFallDiagonalPen( tmpPenFall );
        if ( goUpDiagonal->isChanged() )
          obj->setGoUpDiagonalPen( tmpPenGoUp );
      }
    }
  }
  else if ( dlg->isColumnSelected )
  {
    Cell* c = NULL;
    for (int col = dlg->left; col <= dlg->right; col++)
    {
      for (c = sheet->getFirstCellColumn(col); c != NULL;
           c = sheet->getNextCellDown(c->column(), c->row()))
      {
        if ( fallDiagonal->isChanged() )
        {
          c->clearProperty(Cell::PFallDiagonal);
          c->clearNoFallBackProperties( Cell::PFallDiagonal );
        }
        if ( goUpDiagonal->isChanged() )
        {
          c->clearProperty(Cell::PGoUpDiagonal);
          c->clearNoFallBackProperties( Cell::PGoUpDiagonal);
        }
      }

      ColumnFormat *obj=dlg->getSheet()->nonDefaultColumnFormat(col);
      if ( fallDiagonal->isChanged() )
        obj->setFallDiagonalPen( tmpPenFall );
      if ( goUpDiagonal->isChanged() )
        obj->setGoUpDiagonalPen( tmpPenGoUp );
    }


    RowFormat* rw =dlg->getSheet()->firstRow();
    for ( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(Cell::PFallDiagonal)
                                ||rw->hasProperty(Cell::PGoUpDiagonal) ))
      {
        for ( int i=dlg->left;i<=dlg->right;i++)
        {
          Cell *cell =
            dlg->getSheet()->nonDefaultCell( i, rw->row() );
          if ( cell->isObscuringForced() && dlg->isSingleCell() )
            continue;
          cell->setFallDiagonalPen( tmpPenFall );
          cell->setGoUpDiagonalPen( tmpPenGoUp );
        }
      }
    }
  }
  else if ( dlg->isRowSelected )
  {
    Cell* c = NULL;
    for (int row = dlg->top; row <= dlg->bottom; row++)
    {
      for (c = sheet->getFirstCellRow(row); c != NULL;
           c = sheet->getNextCellRight(c->column(), c->row()))
      {
        if ( fallDiagonal->isChanged() )
        {
          c->clearProperty(Cell::PFallDiagonal);
          c->clearNoFallBackProperties( Cell::PFallDiagonal );
        }
        if ( goUpDiagonal->isChanged() )
        {
          c->clearProperty(Cell::PGoUpDiagonal);
          c->clearNoFallBackProperties( Cell::PGoUpDiagonal);
        }
      }

      RowFormat *obj=dlg->getSheet()->nonDefaultRowFormat(row);
      if ( fallDiagonal->isChanged() )
        obj->setFallDiagonalPen( tmpPenFall );
      if ( goUpDiagonal->isChanged() )
        obj->setGoUpDiagonalPen( tmpPenGoUp );
    }
  }
}

void CellFormatPageBorder::applyHorizontalOutline()
{
  QPen tmpPen( borderButtons[BorderType_Horizontal]->getColor(),
               borderButtons[BorderType_Horizontal]->getPenWidth(),
               borderButtons[BorderType_Horizontal]->getPenStyle());

  if ( dlg->getStyle() )
  {
    dlg->getStyle()->changeTopBorderPen( tmpPen );
  }
  else if ( (!dlg->isRowSelected) && (!dlg->isColumnSelected) )
  {
    for ( int x = dlg->left; x <= dlg->right; x++ )
    {
      for ( int y = dlg->top + 1; y <= dlg->bottom; y++ )
      {
        Cell * obj = dlg->getSheet()->nonDefaultCell( x, y );
        obj->setTopBorderPen( tmpPen );
      }
    }
  }
  else if ( dlg->isColumnSelected )
  {
    Cell* c = NULL;
    for (int col = dlg->left; col <= dlg->right; col++)
    {
      for (c = sheet->getFirstCellColumn(col); c != NULL;
           c = sheet->getNextCellDown(c->column(), c->row()))
      {
        c->clearProperty(Cell::PTopBorder);
        c->clearNoFallBackProperties( Cell::PTopBorder );
      }

      ColumnFormat *obj=dlg->getSheet()->nonDefaultColumnFormat(col);
      obj->setTopBorderPen(tmpPen);
    }

    RowFormat* rw =dlg->getSheet()->firstRow();
    for ( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(Cell::PTopBorder)  ))
      {
        for ( int i=dlg->left;i<=dlg->right;i++)
        {
          Cell *cell =
            dlg->getSheet()->nonDefaultCell( i, rw->row() );
          cell->setTopBorderPen(tmpPen);
        }
      }
    }
  }
  else if ( dlg->isRowSelected )
  {
    Cell* c = NULL;
    for (int row = dlg->top + 1; row <= dlg->bottom; row++)
    {
      for (c = sheet->getFirstCellRow(row); c != NULL;
           c = sheet->getNextCellRight(c->column(), c->row()))
      {
        c->clearProperty(Cell::PTopBorder);
        c->clearNoFallBackProperties( Cell::PTopBorder );
      }

      RowFormat *obj = dlg->getSheet()->nonDefaultRowFormat(row);
      obj->setTopBorderPen(tmpPen);
    }
  }
}

void CellFormatPageBorder::applyVerticalOutline()
{
  BorderButton* vertical = borderButtons[BorderType_Vertical];
  QPen tmpPen( vertical->getColor(), vertical->getPenWidth(),
               vertical->getPenStyle());

  if ( dlg->getStyle() )
    dlg->getStyle()->changeLeftBorderPen( tmpPen );
  else if ( (!dlg->isRowSelected) && (!dlg->isColumnSelected) )
  {
    for ( int x = dlg->left+1; x <= dlg->right; x++ )
    {
      for ( int y = dlg->top; y <= dlg->bottom; y++ )
      {
        Cell *obj = dlg->getSheet()->nonDefaultCell( x, y );
        obj->setLeftBorderPen( tmpPen );
      }
    }
  }
  else if ( dlg->isColumnSelected )
  {
    Cell* c = NULL;
    for (int col = dlg->left + 1; col <= dlg->right; ++col)
    {
      for (c = sheet->getFirstCellColumn(col); c != NULL;
           c = sheet->getNextCellDown(c->column(), c->row()))
      {
        c->clearProperty(Cell::PLeftBorder);
        c->clearNoFallBackProperties( Cell::PLeftBorder );
      }
      ColumnFormat *obj=dlg->getSheet()->nonDefaultColumnFormat(col);
      obj->setLeftBorderPen( tmpPen );
    }

    RowFormat * rw = dlg->getSheet()->firstRow();
    for ( ; rw; rw = rw->next() )
    {
      if ( !rw->isDefault() && (rw->hasProperty(Cell::PLeftBorder)  ))
      {
        for ( int i = dlg->left + 1; i <= dlg->right; ++i )
        {
          Cell * cell =
            dlg->getSheet()->nonDefaultCell( i, rw->row() );
          cell->setLeftBorderPen( tmpPen );
        }
      }
    }
  }
  else if ( dlg->isRowSelected )
  {
    Cell* c = NULL;
    for (int row = dlg->top; row <= dlg->bottom; row++)
    {
      for (c = sheet->getFirstCellRow(row); c != NULL;
           c = sheet->getNextCellRight(c->column(), c->row()))
      {
        c->clearProperty(Cell::PLeftBorder);
        c->clearNoFallBackProperties( Cell::PLeftBorder );
      }
      RowFormat *obj=dlg->getSheet()->nonDefaultRowFormat(row);
      obj->setLeftBorderPen( tmpPen );
    }
  }
}


void CellFormatPageBorder::slotSetColorButton( const QColor &_color )
{
    currentColor = _color;

    for ( int i = 0; i < NUM_BORDER_PATTERNS; ++i )
    {
      pattern[i]->setColor( currentColor );
    }
    preview->setColor( currentColor );
}

void CellFormatPageBorder::slotUnselect2( PatternSelect *_p )
{
    for ( int i = 0; i < NUM_BORDER_PATTERNS; ++i )
    {
      if ( pattern[i] != _p )
      {
        pattern[i]->slotUnselect();
      }
    }
    preview->setPattern( _p->getColor(), _p->getPenWidth(), _p->getPenStyle() );
}

void CellFormatPageBorder::preselect( BorderButton *_p )
{
  BorderButton* top = borderButtons[BorderType_Top];
  BorderButton* bottom = borderButtons[BorderType_Bottom];
  BorderButton* left = borderButtons[BorderType_Left];
  BorderButton* right = borderButtons[BorderType_Right];
  BorderButton* vertical = borderButtons[BorderType_Vertical];
  BorderButton* horizontal = borderButtons[BorderType_Horizontal];
  BorderButton* remove = shortcutButtons[BorderShortcutType_Remove];
  BorderButton* outline = shortcutButtons[BorderShortcutType_Outline];
  BorderButton* all = shortcutButtons[BorderShortcutType_All];

  _p->setOn(false);
  if (_p == remove)
  {
    for (int i=BorderType_Top; i < BorderType_END; i++)
    {
      if (borderButtons[i]->isOn())
      {
        borderButtons[i]->unselect();
      }
    }
  }
  if (_p==outline)
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
  if (_p==all)
  {
    if (dlg->oneRow==false)
    {
      horizontal->setOn(true);
      horizontal->setPenWidth(preview->getPenWidth());
      horizontal->setPenStyle(preview->getPenStyle());
      horizontal->setColor( currentColor );
      horizontal->setChanged(true);
    }
    if (dlg->oneCol==false)
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

void CellFormatPageBorder::changeState( BorderButton *_p)
{
  _p->setChanged(true);

  if (_p->isOn())
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

void CellFormatPageBorder::draw()
{
  BorderButton* top = borderButtons[BorderType_Top];
  BorderButton* bottom = borderButtons[BorderType_Bottom];
  BorderButton* left = borderButtons[BorderType_Left];
  BorderButton* right = borderButtons[BorderType_Right];
  BorderButton* risingDiagonal = borderButtons[BorderType_RisingDiagonal];
  BorderButton* fallingDiagonal = borderButtons[BorderType_FallingDiagonal];
  BorderButton* vertical = borderButtons[BorderType_Vertical];
  BorderButton* horizontal = borderButtons[BorderType_Horizontal];
  QPen pen;
  QPainter painter;
  painter.begin( area );

  if ((bottom->getPenStyle())!=Qt::NoPen)
  {
    pen=QPen( bottom->getColor(), bottom->getPenWidth(),bottom->getPenStyle());
    painter.setPen( pen );
    painter.drawLine( OFFSETX, area->height()-OFFSETY, area->width()-OFFSETX , area->height()-OFFSETY );
  }
  if ((top->getPenStyle())!=Qt::NoPen)
  {
    pen=QPen( top->getColor(), top->getPenWidth(),top->getPenStyle());
    painter.setPen( pen );
    painter.drawLine( OFFSETX, OFFSETY, area->width() -OFFSETX, OFFSETY );
  }
 if ((left->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( left->getColor(), left->getPenWidth(),left->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( OFFSETX, OFFSETY, OFFSETX , area->height()-OFFSETY );
 }
 if ((right->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( right->getColor(), right->getPenWidth(),right->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( area->width()-OFFSETX, OFFSETY, area->width()-OFFSETX,
                     area->height()-OFFSETY );

 }
 if ((fallingDiagonal->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( fallingDiagonal->getColor(), fallingDiagonal->getPenWidth(),
             fallingDiagonal->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( OFFSETX, OFFSETY, area->width()-OFFSETX,
                     area->height()-OFFSETY );
   if (dlg->oneCol==false&& dlg->oneRow==false)
   {
     painter.drawLine( area->width()/2, OFFSETY, area->width()-OFFSETX,
                       area->height()/2 );
     painter.drawLine( OFFSETX,area->height()/2 , area->width()/2,
                       area->height()-OFFSETY );
   }
 }
 if ((risingDiagonal->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( risingDiagonal->getColor(), risingDiagonal->getPenWidth(),
             risingDiagonal->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( OFFSETX, area->height()-OFFSETY , area->width()-OFFSETX ,
                     OFFSETY );
   if (dlg->oneCol==false&& dlg->oneRow==false)
   {
     painter.drawLine( area->width()/2, OFFSETY, OFFSETX, area->height()/2 );
     painter.drawLine( area->width()/2,area->height()-OFFSETY ,
                       area->width()-OFFSETX, area->height()/2 );
   }

 }
 if ((vertical->getPenStyle())!=Qt::NoPen)
    {
      pen=QPen( vertical->getColor(), vertical->getPenWidth(),
                vertical->getPenStyle());
      painter.setPen( pen );
      painter.drawLine( area->width()/2, 5 , area->width()/2 , area->height()-5 );
    }
 if ((horizontal->getPenStyle())!=Qt::NoPen)
 {
   pen=QPen( horizontal->getColor(), horizontal->getPenWidth(),
             horizontal->getPenStyle());
   painter.setPen( pen );
   painter.drawLine( OFFSETX,area->height()/2,area->width()-OFFSETX,
                     area->height()/2 );
 }
 painter.end();
}

void CellFormatPageBorder::invertState(BorderButton *_p)
{
  if (_p->isOn())
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

void CellFormatPageBorder::slotPressEvent(QMouseEvent *_ev)
{
  BorderButton* top = borderButtons[BorderType_Top];
  BorderButton* bottom = borderButtons[BorderType_Bottom];
  BorderButton* left = borderButtons[BorderType_Left];
  BorderButton* right = borderButtons[BorderType_Right];
  BorderButton* vertical = borderButtons[BorderType_Vertical];
  BorderButton* horizontal = borderButtons[BorderType_Horizontal];


  QRect rect(OFFSETX,OFFSETY-8,area->width()-OFFSETX,OFFSETY+8);
  if (rect.contains(QPoint(_ev->x(),_ev->y())))
  {
    if (((top->getPenWidth()!=preview->getPenWidth()) ||
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
  if (rect.contains(QPoint(_ev->x(),_ev->y())))
  {
    if (((bottom->getPenWidth()!=preview->getPenWidth()) ||
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
  if (rect.contains(QPoint(_ev->x(),_ev->y())))
  {
    if (((left->getPenWidth()!=preview->getPenWidth()) ||
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
  if (rect.contains(QPoint(_ev->x(),_ev->y())))
  {
    if (((right->getPenWidth()!=preview->getPenWidth()) ||
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
if (rect.contains(QPoint(_ev->x(),_ev->y())))
        {
         invertState(fallDiagonal);
        }
rect.setCoords(OFFSETX,YHEI-OFFSETY,XLEN-OFFSETX,OFFSETY);
if (rect.contains(QPoint(_ev->x(),_ev->y())))
        {
         invertState(goUpDiagonal);
        } */

  if (dlg->oneCol==false)
  {
    rect.setCoords(area->width()/2-8,OFFSETY,area->width()/2+8,
                   area->height()-OFFSETY);

    if (rect.contains(QPoint(_ev->x(),_ev->y())))
    {
      if (((vertical->getPenWidth()!=preview->getPenWidth()) ||
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
  if (dlg->oneRow==false)
  {
    rect.setCoords(OFFSETX,area->height()/2-8,area->width()-OFFSETX,
                   area->height()/2+8);
    if (rect.contains(QPoint(_ev->x(),_ev->y())))
    {
      if (((horizontal->getPenWidth()!=preview->getPenWidth()) ||
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

BrushSelect::BrushSelect( QWidget *parent, const char * ) : QFrame( parent )
{
    brushStyle = Qt::NoBrush;
    brushColor = Qt::red;
    selected = false;
}

void BrushSelect::setPattern( const QColor &_color,BrushStyle _style )
{
    brushStyle = _style;
    brushColor = _color;
    repaint();
}


void BrushSelect::paintEvent( QPaintEvent *_ev )
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

void BrushSelect::mousePressEvent( QMouseEvent * )
{
    slotSelect();

    emit clicked( this );
}

void BrushSelect::slotUnselect()
{
    selected = false;

    setLineWidth( 1 );
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    repaint();
}

void BrushSelect::slotSelect()
{
    selected = true;

    setLineWidth( 2 );
    setFrameStyle( QFrame::Panel | QFrame::Plain );
    repaint();
}


CellFormatPagePattern::CellFormatPagePattern( QWidget* parent, CellFormatDialog *_dlg ) : QWidget( parent )
{
    dlg = _dlg;

    bBgColorUndefined = !dlg->bBgColor;

    QGridLayout *grid = new QGridLayout(this,5,2,KDialog::marginHint(), KDialog::spacingHint());

    QGroupBox* tmpQGroupBox;
    tmpQGroupBox = new QGroupBox( this, "GroupBox_20" );
    tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
    tmpQGroupBox->setTitle( i18n("Pattern") );
    tmpQGroupBox->setAlignment( AlignLeft );

    QGridLayout *grid2 = new QGridLayout(tmpQGroupBox,8,3,KDialog::marginHint(), KDialog::spacingHint());
    int fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title


    brush1 = new BrushSelect( tmpQGroupBox, "Frame_1" );
    brush1->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush1,1,0);

    brush2 = new BrushSelect( tmpQGroupBox, "Frame_2" );
    brush2->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush2,1,1);

    brush3 = new BrushSelect( tmpQGroupBox, "Frame_3" );
    brush3->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush3,1,2);

    brush4 = new BrushSelect( tmpQGroupBox, "Frame_4" );
    brush4->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush4,2,0);

    brush5 = new BrushSelect( tmpQGroupBox, "Frame_5" );
    brush5->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush5,2,1);

    brush6 = new BrushSelect( tmpQGroupBox, "Frame_6" );
    brush6->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush6,2,2);

    brush7 = new BrushSelect( tmpQGroupBox, "Frame_7" );
    brush7->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush7,3,0);

    brush8 = new BrushSelect( tmpQGroupBox, "Frame_8" );
    brush8->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush8,3,1);

    brush9 = new BrushSelect( tmpQGroupBox, "Frame_9" );
    brush9->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush9,3,2);

    brush10 = new BrushSelect( tmpQGroupBox, "Frame_10" );
    brush10->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush10,4,0);

    brush11 = new BrushSelect( tmpQGroupBox, "Frame_11" );
    brush11->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush11,4,1);

    brush12 = new BrushSelect( tmpQGroupBox, "Frame_12" );
    brush12->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush12,4,2);

    brush13 = new BrushSelect( tmpQGroupBox, "Frame_13" );
    brush13->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush13,5,0);

    brush14 = new BrushSelect( tmpQGroupBox, "Frame_14" );
    brush14->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush14,5,1);

    brush15 = new BrushSelect( tmpQGroupBox, "Frame_15" );
    brush15->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(brush15,5,2);

    QGridLayout *grid3 = new QGridLayout( 1, 2 );
    color = new KColorButton (tmpQGroupBox, "ColorButton_1" );
    grid3->addWidget(color,0,1);

    QLabel *tmpQLabel = new QLabel( tmpQGroupBox, "Label_1" );
    tmpQLabel->setText( i18n("Color:") );
    grid3->addWidget(tmpQLabel,0,0);

    grid2->addMultiCell(grid3,6,6,0,2);

    grid3 = new QGridLayout( 1, 3 );
    grid3->setSpacing(KDialog::spacingHint());

    tmpQLabel = new QLabel( tmpQGroupBox, "Label_2" );
    grid3->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText( i18n("Background color:") );

    bgColorButton = new KColorButton( tmpQGroupBox, "ColorButton" );
    grid3->addWidget(bgColorButton,0,1);
    if ( dlg->bBgColor )
        bgColor = dlg->bgColor;
    else
        bgColor = colorGroup().base();

    if (!bgColor.isValid())
        bgColor = colorGroup().base();

    bgColorButton->setColor( bgColor );
    connect( bgColorButton, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotSetBackgroundColor( const QColor & ) ) );

    notAnyColor=new QPushButton(i18n("No Color"),tmpQGroupBox);
    grid3->addWidget(notAnyColor,0,2);
    connect( notAnyColor, SIGNAL( clicked( ) ),
             this, SLOT( slotNotAnyColor(  ) ) );
    b_notAnyColor=false;

    grid2->addMultiCell(grid3,7,7,0,2);

    grid->addMultiCellWidget(tmpQGroupBox,0,3,0,0);

    tmpQGroupBox = new QGroupBox( this, "GroupBox1" );
    tmpQGroupBox->setTitle( i18n("Preview") );
    tmpQGroupBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
    tmpQGroupBox->setAlignment( AlignLeft );

    grid2 = new QGridLayout(tmpQGroupBox,2,1,KDialog::marginHint(), KDialog::spacingHint());
    fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

    current = new BrushSelect( tmpQGroupBox, "Current" );
    current->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    grid2->addWidget(current,1,0);
    grid->addWidget( tmpQGroupBox,4,0);

    connect( brush1, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush2, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush3, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush4, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush5, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush6, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush7, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush8, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush9, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush10, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush11, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush12, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush13, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush14, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );
    connect( brush15, SIGNAL( clicked( BrushSelect* ) ),
             this, SLOT( slotUnselect2( BrushSelect* ) ) );

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
    current->setBackgroundColor( bgColor );

    connect( color, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotSetColorButton( const QColor & ) ) );

    slotSetColorButton( dlg->brushColor );
    init();
    this->resize( 400, 400 );
}

void CellFormatPagePattern::slotNotAnyColor()
{
  b_notAnyColor = true;
  bgColorButton->setColor( colorGroup().base() );
  current->setBackgroundColor( colorGroup().base() );
}

void CellFormatPagePattern::slotSetBackgroundColor( const QColor &_color )
{
  bgColor =_color;
  current->setBackgroundColor( bgColor );
  bBgColorUndefined = false;
  b_notAnyColor = false;
}

void CellFormatPagePattern::init()
{
  if (dlg->brushStyle == Qt::VerPattern)
  {
    brush1->slotSelect();
  }
  else if (dlg->brushStyle == Qt::HorPattern)
  {
    brush2->slotSelect();
  }
  else if (dlg->brushStyle == Qt::Dense1Pattern)
  {
    brush3->slotSelect();
  }
  else if (dlg->brushStyle == Qt::Dense2Pattern)
  {
    brush4->slotSelect();
  }
  else if (dlg->brushStyle == Qt::Dense3Pattern)
  {
    brush5->slotSelect();
  }
  else if (dlg->brushStyle == Qt::Dense4Pattern)
  {
    brush6->slotSelect();
  }
  else if (dlg->brushStyle == Qt::Dense5Pattern)
  {
    brush7->slotSelect();
  }
  else if (dlg->brushStyle == Qt::Dense6Pattern)
  {
    brush8->slotSelect();
  }
  else if (dlg->brushStyle == Qt::Dense7Pattern)
  {
    brush9->slotSelect();
  }
  else if (dlg->brushStyle == Qt::CrossPattern)
  {
    brush10->slotSelect();
  }
  else if (dlg->brushStyle == Qt::BDiagPattern)
  {
    brush11->slotSelect();
  }
  else if (dlg->brushStyle == Qt::FDiagPattern)
  {
    brush12->slotSelect();
  }
  else if (dlg->brushStyle == Qt::VerPattern)
  {
    brush13->slotSelect();
  }
  else if (dlg->brushStyle == Qt::DiagCrossPattern)
  {
    brush14->slotSelect();
  }
  else if (dlg->brushStyle == Qt::NoBrush)
  {
    brush15->slotSelect();
  }
  else
    kdDebug(36001) << "Error in brushStyle" << endl;
}

void CellFormatPagePattern::slotSetColorButton( const QColor &_color )
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

void CellFormatPagePattern::slotUnselect2( BrushSelect *_p )
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

    current->setBrushStyle( selectedBrush->getBrushStyle() );
}

void CellFormatPagePattern::apply( CustomStyle * style )
{
  if ( selectedBrush != 0L
    && ( dlg->brushStyle != selectedBrush->getBrushStyle()
         || dlg->brushColor != selectedBrush->getBrushColor() ) )
    style->changeBackGroundBrush( QBrush( selectedBrush->getBrushColor(), selectedBrush->getBrushStyle() ) );

  /*
    TODO: check...
  if ( b_notAnyColor)
    style->changeBgColor( QColor() );
  else
  */
  if ( bgColor != dlg->getStyle()->bgColor() )
    style->changeBgColor( bgColor );
}

void CellFormatPagePattern::apply( ColumnFormat *_obj )
{
  Sheet * sheet = dlg->getSheet();
  Cell  * c = NULL;
  for ( int col = dlg->left; col <= dlg->right; ++col )
  {
    for (c = sheet->getFirstCellColumn(col); c != NULL;
         c = sheet->getNextCellDown(c->column(), c->row()))
    {
      if ( selectedBrush != 0L
           && ( dlg->brushStyle != selectedBrush->getBrushStyle()
                || dlg->brushColor != selectedBrush->getBrushColor() ) )
      {
        c->clearProperty(Cell::PBackgroundBrush);
        c->clearNoFallBackProperties( Cell::PBackgroundBrush );
      }
      if ( ( !bBgColorUndefined || b_notAnyColor )
           && bgColor != dlg->bgColor )
      {
        c->clearProperty(Cell::PBackgroundColor);
        c->clearNoFallBackProperties( Cell::PBackgroundColor );
      }
    }
  }
  applyFormat( _obj );

  RowFormat * rw = dlg->getSheet()->firstRow();
  for ( ; rw; rw = rw->next() )
  {
    if ( !rw->isDefault() && (rw->hasProperty(Cell::PBackgroundColor) || rw->hasProperty(Cell::PBackgroundBrush)))
    {
      for ( int i = dlg->left; i <= dlg->right; ++i )
      {
        Cell * cell =
          dlg->getSheet()->nonDefaultCell( i, rw->row() );
        applyFormat(cell );
      }
    }
  }

}

void CellFormatPagePattern::apply( RowFormat *_obj )
{
  Sheet * sheet = dlg->getSheet();
  Cell * c = NULL;
  for ( int row = dlg->top; row <= dlg->bottom; ++row)
  {
    for (c = sheet->getFirstCellRow(row); c != NULL;
         c = sheet->getNextCellRight(c->column(), c->row()))
    {
      if ( selectedBrush != 0L
           && ( dlg->brushStyle != selectedBrush->getBrushStyle()
                || dlg->brushColor != selectedBrush->getBrushColor() ) )
      {
        c->clearProperty(Cell::PBackgroundBrush);
        c->clearNoFallBackProperties( Cell::PBackgroundBrush );
      }
      if ( ( !bBgColorUndefined || b_notAnyColor )
           && bgColor != dlg->bgColor )
      {
        c->clearProperty(Cell::PBackgroundColor);
        c->clearNoFallBackProperties( Cell::PBackgroundColor );
      }
    }
  }
  applyFormat( _obj );
}


void CellFormatPagePattern::apply( Cell *_obj )
{
  applyFormat( _obj );
}

void CellFormatPagePattern::applyFormat( Format *_obj )
{
  if ( selectedBrush != 0L
       && ( dlg->brushStyle != selectedBrush->getBrushStyle()
            || dlg->brushColor != selectedBrush->getBrushColor() ) )
    _obj->setBackGroundBrush( QBrush( selectedBrush->getBrushColor(), selectedBrush->getBrushStyle() ) );

  if ( bgColor == dlg->bgColor )
    return;

  if ( b_notAnyColor)
    _obj->setBgColor( QColor() );
  else if ( !bBgColorUndefined )
    _obj->setBgColor( bgColor );
}

#include "kspread_dlg_layout.moc"

