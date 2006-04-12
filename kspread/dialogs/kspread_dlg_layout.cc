/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             (C) 2004 Tomas Mecir <mecirt@gmail.com>
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

#include <qbitmap.h>
#include <qcheckbox.h>
#include <q3frame.h>
#include <qlabel.h>
#include <qlayout.h>
#include <q3listbox.h>
#include <qfontdatabase.h>
#include <qradiobutton.h>
#include <qslider.h>

//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <QPixmap>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <knumvalidator.h>

#include <KoUnitWidgets.h>

#include "kspread_canvas.h"
#include "kspread_dlg_layout.h"
#include "kspread_locale.h"
#include "kspread_sheet.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "kspread_util.h"
#include "manipulator.h"
#include "selection.h"
#include "valueformatter.h"

using namespace KSpread;

/***************************************************************************
 *
 * PatternSelect
 *
 ***************************************************************************/

PatternSelect::PatternSelect( QWidget *parent, const char * )
  : Q3Frame( parent )
{
    penStyle = Qt::NoPen;
    penWidth = 1;
    penColor = colorGroup().text();
    selected = false;
    undefined = false;
}

void PatternSelect::setPattern( const QColor &_color, int _width, Qt::PenStyle _style )
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
    Q3Frame::paintEvent( _ev );

    QPainter painter( this );

    if ( !undefined )
    {
        QPen pen( penColor, penWidth, penStyle);
        painter.setPen( pen );
        painter.drawLine( 6, height()/2, width() - 6,height()/2  );
    }
    else
    {
        painter.fillRect( 2, 2, width() - 4, height() - 4, Qt::BDiagPattern );
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
    setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    repaint();
}

void PatternSelect::slotSelect()
{
    selected = true;

    setLineWidth( 2 );
    setFrameStyle( Q3Frame::Panel | Q3Frame::Plain );
    repaint();
}



/***************************************************************************
 *
 * GeneralTab
 *
 ***************************************************************************/

GeneralTab::GeneralTab( QWidget* parent, CellFormatDialog * dlg )
  : QWidget( parent ),
    m_dlg( dlg )
{
  Q3GridLayout * layout = new Q3GridLayout( this, 1, 1, KDialog::marginHint(), KDialog::spacingHint(), "layout");

  QGroupBox * groupBox = new QGroupBox( this );
  groupBox->setObjectName( "groupBox1" );
// ###  groupBox->setColumnLayout(0, Qt::Vertical );
  groupBox->setTitle( i18n( "Style" ) );
  groupBox->layout()->setSpacing( KDialog::spacingHint() );
  groupBox->layout()->setMargin( KDialog::marginHint() );

  Q3GridLayout * groupBoxLayout = new Q3GridLayout( groupBox->layout() );
  groupBoxLayout->setAlignment( Qt::AlignTop );

  QLabel * label1 = new QLabel( groupBox, "label1" );
  label1->setText( i18n( "Name:" ) );
  groupBoxLayout->addWidget( label1, 0, 0 );

  m_nameEdit = new KLineEdit( groupBox );
  m_nameEdit->setObjectName( "m_nameEdit" );
  m_nameEdit->setText( m_dlg->styleName );
  groupBoxLayout->addWidget( m_nameEdit, 0, 1 );

  QLabel * label2 = new QLabel( groupBox, "label2" );
  label2->setText( i18n( "Inherit style:" ) );
  groupBoxLayout->addWidget( label2, 1, 0 );

  m_parentBox = new KComboBox( false, groupBox );
  m_parentBox->setObjectName( "m_parentBox" );
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
  kDebug() << "New Parent" << endl;
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



/***************************************************************************
 *
 * CellFormatDialog
 *
 ***************************************************************************/

CellFormatDialog::CellFormatDialog( View * _view, Sheet * _sheet )
  : QObject(),
    m_doc( _sheet->doc() ),
    m_sheet( _sheet ),
    m_pView( _view ),
    m_style( 0 )
{
  initMembers();

  //We need both conditions quite often, so store the condition here too
  isRowSelected    = _view->selectionInfo()->isRowSelected();
  isColumnSelected = _view->selectionInfo()->isColumnSelected();

  QRect range = _view->selectionInfo()->selection();
  left = range.left();
  top = range.top();
  right = range.right();
  bottom = range.bottom();

  if ( left == right )
    oneCol = true;
  else
    oneCol = false;

  if ( top == bottom )
    oneRow = true;
  else
    oneRow = false;

  Cell* obj = m_sheet->cellAt( left, top );
  oneCell = (left == right && top == bottom &&
             !obj->doesMergeCells());

  isMerged = ((obj->doesMergeCells() &&
               left + obj->extraXCells() >= right &&
               top + obj->extraYCells() >= bottom));

  // Initialize with the upper left object.
  borders[BorderType_Left].style = obj->format()->leftBorderStyle( left, top );
  borders[BorderType_Left].width = obj->format()->leftBorderWidth( left, top );
  borders[BorderType_Left].color = obj->format()->leftBorderColor( left, top );
  borders[BorderType_Top].style = obj->format()->topBorderStyle( left, top );
  borders[BorderType_Top].width = obj->format()->topBorderWidth( left, top );
  borders[BorderType_Top].color = obj->format()->topBorderColor( left, top );
  borders[BorderType_FallingDiagonal].style =
      obj->format()->fallDiagonalStyle( left, top );
  borders[BorderType_FallingDiagonal].width =
      obj->format()->fallDiagonalWidth( left, top );
  borders[BorderType_FallingDiagonal].color =
      obj->format()->fallDiagonalColor( left, top );
  borders[BorderType_RisingDiagonal].style =
      obj->format()->goUpDiagonalStyle( left, top );
  borders[BorderType_RisingDiagonal].width =
      obj->format()->goUpDiagonalWidth( left, top );
  borders[BorderType_RisingDiagonal].color =
      obj->format()->goUpDiagonalColor( left, top );

  // Look at the upper right one for the right border.
  obj = m_sheet->cellAt( right, top );
  borders[BorderType_Right].style = obj->format()->rightBorderStyle( right, top );
  borders[BorderType_Right].width = obj->format()->rightBorderWidth( right, top );
  borders[BorderType_Right].color = obj->format()->rightBorderColor( right, top );

  // Look at the bottom left cell for the bottom border.
  obj = m_sheet->cellAt( left, bottom );
  borders[BorderType_Bottom].style = obj->format()->bottomBorderStyle( left, bottom );
  borders[BorderType_Bottom].width = obj->format()->bottomBorderWidth( left, bottom );
  borders[BorderType_Bottom].color = obj->format()->bottomBorderColor( left, bottom );

  // Just an assumption
  obj = m_sheet->cellAt( right, top );
  if ( obj->isPartOfMerged() )
  {
    obj = obj->obscuringCells().first();
    int moveX  = obj->column();
    int moveY  = top;
    int moveX2 = right;
    int moveY2 = obj->row();
    borders[BorderType_Vertical].style = obj->format()->leftBorderStyle( moveX, moveY );
    borders[BorderType_Vertical].width = obj->format()->leftBorderWidth( moveX, moveY );
    borders[BorderType_Vertical].color = obj->format()->leftBorderColor( moveX, moveY );

    obj = m_sheet->cellAt( moveX2,  moveY2 );
    borders[BorderType_Horizontal].style = obj->format()->topBorderStyle( moveX2, moveY2 );
    borders[BorderType_Horizontal].width = obj->format()->topBorderWidth( moveX2, moveY2 );
    borders[BorderType_Horizontal].color = obj->format()->topBorderColor( moveX2, moveY2 );
  }
  else
  {
    borders[BorderType_Vertical].style = obj->format()->leftBorderStyle( right, top );
    borders[BorderType_Vertical].width = obj->format()->leftBorderWidth( right, top );
    borders[BorderType_Vertical].color = obj->format()->leftBorderColor( right, top );
    borders[BorderType_Horizontal].style = obj->format()->topBorderStyle(right, bottom);
    borders[BorderType_Horizontal].width = obj->format()->topBorderWidth(right, bottom);
    borders[BorderType_Horizontal].color = obj->format()->topBorderColor(right, bottom);
  }

  obj = m_sheet->cellAt( left, top );
  prefix = obj->format()->prefix( left, top );
  postfix = obj->format()->postfix( left, top );
  precision = obj->format()->precision( left, top );
  floatFormat = obj->format()->floatFormat( left, top );
  floatColor = obj->format()->floatColor( left, top );
  alignX = obj->format()->align( left, top );
  alignY = obj->format()->alignY( left, top );
  textColor = obj->format()->textColor( left, top );
  bgColor = obj->bgColor( left, top );
  textFontSize = obj->format()->textFontSize( left, top );
  textFontFamily = obj->format()->textFontFamily( left, top );
  textFontBold = obj->format()->textFontBold( left, top );
  textFontItalic = obj->format()->textFontItalic( left, top );
  strike=obj->format()->textFontStrike( left, top );
  underline = obj->format()->textFontUnderline( left, top );
  // Needed to initialize the font correctly ( bug in Qt )
  textFont = obj->format()->textFont( left, top );
  obj->format()->currencyInfo( cCurrency );

  brushColor = obj->format()->backGroundBrushColor( left, top );
  brushStyle = obj->format()->backGroundBrushStyle( left,top );

  bMultiRow = obj->format()->multiRow( left, top );
  bVerticalText = obj->format()->verticalText( left, top );
  textRotation = obj->format()->getAngle(left, top);
  formatType = obj->format()->getFormatType(left, top);

  bDontPrintText = obj->format()->getDontprintText( left, top );
  bHideFormula   = obj->format()->isHideFormula( left, top );
  bHideAll       = obj->format()->isHideAll( left, top );
  bIsProtected   = !obj->format()->notProtected( left, top );

  indent = obj->format()->getIndent(left, top);

  value = obj->value();

  RowFormat *rl;
  ColumnFormat *cl;
  widthSize = 0.0;
  heightSize = 0.0;

  if ( !isRowSelected )
  {
    for ( int x = left; x <= right; x++ )
    {
      cl = m_pView->activeSheet()->columnFormat( x );
      widthSize = qMax( cl->dblWidth(), widthSize );
    }
  }

  if ( !isColumnSelected )
  {
    for ( int y = top; y <= bottom; y++ )
    {
      rl = m_pView->activeSheet()->rowFormat(y);
      heightSize = qMax( rl->dblHeight(), heightSize );
    }
  }

  //select column(s)
  if ( isColumnSelected )
  {
    int y = 1;
    Cell* cell = NULL;
    for (int x = left;x <= right; x++)
    {
      ColumnFormat *obj = m_sheet->nonDefaultColumnFormat(x);
      initParameters( obj,x,y);

      for (cell = m_sheet->getFirstCellColumn(x); cell != NULL;
           cell = m_sheet->getNextCellDown(cell->column(), cell->row()))
      {
        initParameters( cell->format(), x, cell->row());
      }
    }

  }
  else if ( isRowSelected )
  {
    int x = 1;
    Cell* c = NULL;
    for ( int y = top;y<=bottom;y++)
    {
      RowFormat *obj = m_sheet->nonDefaultRowFormat(y);
      initParameters( obj,x,y);

      for (c = m_sheet->getFirstCellRow(y); c != NULL;
           c = m_sheet->getNextCellRight(c->column(), c->row()) )
      {
        initParameters( c->format(), c->column(), c->row());
      }
      }
  }
  else
  {
    // Do the other objects have the same values ?
    for ( int x = left; x <= right; x++ )
    {
      for ( int y = top; y <= bottom; y++ )
      {
        Cell *obj = m_sheet->cellAt( x, y );

        if ( obj->isPartOfMerged() )
          continue;

        initParameters( obj->format(),x,y);
      }
    }
  }
  if ( !bTextRotation )
    textRotation = 0;

  if ( isColumnSelected )
  {
    int y=1;
    ColumnFormat *obj=m_sheet->nonDefaultColumnFormat(left);
    checkBorderLeft( obj,left, y);

    Cell* c = NULL;
    for (c = m_sheet->getFirstCellColumn(left); c != NULL;
         c = m_sheet->getNextCellDown(c->column(), c->row()) )
    {
      checkBorderLeft(c->format(), c->column(), c->row());
    }


    obj=m_sheet->nonDefaultColumnFormat(right);
    checkBorderRight(obj,right,y);
    c = NULL;
    for (c = m_sheet->getFirstCellColumn(right); c != NULL;
         c = m_sheet->getNextCellDown(c->column(), c->row()) )
    {
      checkBorderRight(c->format(), c->column(), c->row());
    }

    for ( int x = left; x <= right; x++ )
    {
      Cell *obj = m_sheet->cellAt( x, top );
      checkBorderTop(obj->format(),x, top);
      obj = m_sheet->cellAt( x, bottom );
      checkBorderBottom(obj->format(),x, bottom);
      if ( x > left )
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
    for ( int y = top; y <= bottom; y++ )
    {
      Cell *obj = m_sheet->cellAt( right, y );
      checkBorderRight(obj->format(),right,y);
      obj = m_sheet->cellAt( left, y );
      checkBorderLeft( obj->format(),left, y);
      if ( y > top )
      {
        RowFormat* obj = m_sheet->nonDefaultRowFormat(y);
        checkBorderHorizontal(obj,x, y);
        checkBorderVertical(obj,x, y);
      }
    }

    RowFormat *obj=m_sheet->nonDefaultRowFormat(top);
    checkBorderTop(obj,x, top);
    obj=m_sheet->nonDefaultRowFormat(bottom);
    checkBorderBottom(obj,x, bottom);
  }
  else
  {
    for ( int y = top; y <= bottom; y++ )
    {
      Cell *obj = m_sheet->cellAt( left, y );
      checkBorderLeft( obj->format(),left, y);
      obj = m_sheet->cellAt( right, y );
      checkBorderRight(obj->format(),right,y);
    }

    for ( int x = left; x <= right; x++ )
    {
      Cell *obj = m_sheet->cellAt( x, top );
      checkBorderTop( obj->format(), x, top );
      obj = m_sheet->cellAt( x, bottom );
      checkBorderBottom( obj->format(), x, bottom );
    }

    // Look for the Outline
    for ( int x = left; x <= right; x++ )
    {
      for ( int y = top+1; y <= bottom; y++ )
      {
        Cell *obj = m_sheet->cellAt( x, y );
        checkBorderHorizontal(obj->format(),x, y);
      }
    }

    for ( int x = left+1; x <= right; x++ )
    {
      for ( int y = top; y <= bottom; y++ )
      {
        Cell *obj = m_sheet->cellAt( x, y );
        checkBorderVertical(obj->format(),x,y);
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
  alignX         = m_style->halign();
  alignY         = m_style->valign();
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

  bMultiRow     = m_style->hasProperty( Style::SMultiRow );
  bVerticalText = m_style->hasProperty( Style::SVerticalText );
  textRotation  = m_style->rotateAngle();
  formatType    = m_style->formatType();
  indent        = m_style->indent();

  bDontPrintText = m_style->hasProperty( Style::SDontPrintText );
  bHideFormula   = m_style->hasProperty( Style::SHideFormula );
  bHideAll       = m_style->hasProperty( Style::SHideAll );
  bIsProtected   = !m_style->hasProperty( Style::SNotProtected );

  widthSize  = defaultWidthSize;
  heightSize = defaultHeightSize;
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

  Sheet* sheet = m_pView->activeSheet();
  defaultWidthSize  = sheet ? sheet->columnFormat(0)->dblWidth() : 0;
  defaultHeightSize = sheet ? sheet->rowFormat(0)->dblHeight() : 0;
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

  Style::Currency cur;
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
    QColor Qt::black = colorGroup.text(); // not necessarily black :)
    formatOnlyNegSignedPixmap    = paintFormatPixmap( "123.456", Qt::black, "-123.456", Qt::black );
    formatRedOnlyNegSignedPixmap = paintFormatPixmap( "123.456", Qt::black, "-123.456", Qt::red );
    formatRedNeverSignedPixmap   = paintFormatPixmap( "123.456", Qt::black, "123.456", Qt::red );
    formatAlwaysSignedPixmap     = paintFormatPixmap( "+123.456", Qt::black, "-123.456", Qt::black );
    formatRedAlwaysSignedPixmap  = paintFormatPixmap( "+123.456", Qt::black, "-123.456", Qt::red );
  }

  tab = new Q3TabDialog( (QWidget*)m_pView, 0L, true );
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
  bm.fill( Qt::color0 );
  painter.begin( &bm );
  painter.setPen( Qt::color1 );
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

  borderPage->apply(0);
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

  // (Tomas) TODO: this will be slow !!!
  // We need to create a manipulator that would act as KMacroCommand,
  // but which would also ensure that updates are not painted until everything
  // is updated properly ...
  KMacroCommand* macroCommand = new KMacroCommand( i18n("Change Format") );

  if ( isMerged != positionPage->getMergedCellState() )
  {
    if ( positionPage->getMergedCellState() )
    {
      Manipulator* manipulator = new MergeManipulator();
      manipulator->setSheet(m_pView->activeSheet());
      manipulator->setRegisterUndo(false);
      manipulator->add(*m_pView->selectionInfo());
      macroCommand->addCommand( manipulator );
    }
    else
    {
      //dissociate cells
      Manipulator* manipulator = new MergeManipulator();
      manipulator->setSheet(m_pView->activeSheet());
      manipulator->setReverse(true);
      manipulator->setRegisterUndo(false);
      manipulator->add(*m_pView->selectionInfo());
      macroCommand->addCommand( manipulator );
    }
  }

  FormatManipulator* manipulator = new FormatManipulator();
  manipulator->setSheet(m_pView->activeSheet());
  manipulator->setRegisterUndo(false);
  manipulator->add(*m_pView->selectionInfo());
  borderPage->apply(manipulator);
  floatPage->apply(manipulator);
  fontPage->apply(manipulator);
  positionPage->apply(manipulator);
  patternPage->apply(manipulator);
  protectPage->apply(manipulator);

  if (!manipulator->isEmpty())
  {
    macroCommand->addCommand( manipulator );
  }
  else
  {
    delete manipulator;
  }

  if ( int( positionPage->getSizeHeight() ) != int( heightSize ) )
  {
    ResizeRowManipulator* manipulator = new ResizeRowManipulator();
    manipulator->setSheet(m_pView->activeSheet());
    manipulator->setSize(positionPage->getSizeHeight());
    // TODO Stefan:
    manipulator->setOldSize(heightSize);
    manipulator->add(*m_pView->selectionInfo());
    macroCommand->addCommand( manipulator );
  }
  if ( int( positionPage->getSizeWidth() ) != int( widthSize ) )
  {
    ResizeColumnManipulator* manipulator = new ResizeColumnManipulator();
    manipulator->setSheet(m_pView->activeSheet());
    manipulator->setSize(positionPage->getSizeWidth());
    // TODO Stefan:
    manipulator->setOldSize(widthSize);
    manipulator->add(*m_pView->selectionInfo());
    macroCommand->addCommand( manipulator );
  }

  macroCommand->execute();
  m_doc->addCommand( macroCommand );

  // Update the toolbar (bold/italic/font...)
  m_pView->updateEditWidget();
}



/***************************************************************************
 *
 * CellFormatPageFloat
 *
 ***************************************************************************/

CellFormatPageFloat::CellFormatPageFloat( QWidget* parent, CellFormatDialog *_dlg )
  : QWidget ( parent ),
    dlg( _dlg )
{
    Q3VBoxLayout* layout = new Q3VBoxLayout( this, 6,10 );

    Q3ButtonGroup *grp = new Q3ButtonGroup( i18n("Format"),this);
    Q3GridLayout *grid = new Q3GridLayout(grp,11,2,KDialog::marginHint(), KDialog::spacingHint());

    int fHeight = grp->fontMetrics().height();
    grid->addRowSpacing( 0, fHeight/2 ); // groupbox title

    grp->setRadioButtonExclusive( true );
    generic=new QRadioButton(i18n("Generic"),grp);
    generic->setWhatsThis( i18n( "This is the default format and KSpread autodetects the actual data type depending on the current cell data. By default, KSpread right justifies numbers, dates and times within a cell and left justifies anything else." ) );
    grid->addWidget(generic,1,0);

    number=new QRadioButton(i18n("Number"),grp);
    number->setWhatsThis( i18n( "The number notation uses the notation you globally choose in KControl -> Regional & Accessibility -> Numbers tab. Numbers are right justified by default." ) );
    grid->addWidget(number,2,0);

    percent=new QRadioButton(i18n("Percent"),grp);
    percent->setWhatsThis( i18n( "When you have a number in the current cell and you switch from the dcell format from Generic to Percent, the current cell number will be multiplied by 100%.\nFor example if you enter 12 and set the cell format to Percent, the number will then be 1,200 %. Switching back to Generic cell format will bring it back to 12.\nYou can also use the Percent icon in the Format Toolbar." ) );
    grid->addWidget(percent,3,0);

    money=new QRadioButton(i18n("Money"),grp);
    money->setWhatsThis( i18n( "The Money format converts your number into money notation using the settings globally fixed in KControl in Regional & Accessibility -> Money. The currency symbol will be displayed and the precision will be the one set in KControl.\nYou can also use the Currency icon in the Format Toolbar to set the cell formatting to look like your current currency." ) );
    grid->addWidget(money,4,0);

    scientific=new QRadioButton(i18n("Scientific"),grp);
    scientific->setWhatsThis( i18n( "The scientific format changes your number using the scientific notation. For example, 0.0012 will be changed to 1.2E-03. Going back using Generic cell format will display 0.0012 again." ) );
    grid->addWidget(scientific,5,0);

    fraction=new QRadioButton(i18n("Fraction"),grp);
    fraction->setWhatsThis( i18n( "The fraction format changes your number into a fraction. For example, 0.1 can be changed to 1/8, 2/16, 1/10, etc. You define the type of fraction by choosing it in the field on the right. If the exact fraction is not possible in the fraction mode you choose, the nearest closest match is chosen.\n For example: when we have 1.5 as number, we choose Fraction and Sixteenths 1/16 the text displayed into cell is \"1 8/16\" which is an exact fraction. If you have 1.4 as number in your cell and you choose Fraction and Sixteenths 1/16 then the cell will display \"1 6/16\" which is the nearest closest Sixteenth fraction." ) );
    grid->addWidget(fraction,6,0);

    date=new QRadioButton(i18n("Date format"),grp);
    date->setWhatsThis( i18n( "To enter a date, you should enter it in one of the formats set in KControl in Regional & Accessibility ->Time & Dates. There are two formats set here: the date format and the short date format.\nJust like you can drag down numbers you can also drag down dates and the next cells will also get dates." ) );
    grid->addWidget(date,7,0);

    time=new QRadioButton(i18n("Time format"),grp);
    time->setWhatsThis( i18n( "This formats your cell content as a time. To enter a time, you should enter it in the Time format set in KControl in Regional & Accessibility ->Time & Dates. In the Cell Format dialog box you can set how the time should be displayed by choosing one of the available time format options. The default format is the system format set in KControl. When the number in the cell does not make sense as a time, KSpread will display 00:00 in the global format you have in KControl." ) );
    grid->addWidget(time,8,0);

    textFormat=new QRadioButton(i18n("Text"),grp);
    textFormat->setWhatsThis( i18n( "This formats your cell content as text. This can be useful if you want a number treated as text instead as a number, for example for a ZIP code. Setting a number as text format will left justify it. When numbers are formatted as text, they cannot be used in calculations or formulas. It also change the way the cell is justified." ) );
    grid->addWidget(textFormat,9,0);

    customFormat=new QRadioButton(i18n("Custom"),grp);
    customFormat->setWhatsThis( i18n( "The custom format does not work yet. To be enabled in the next release." ) );
    grid->addWidget(customFormat,10,0);
    customFormat->setEnabled( false );

    Q3GroupBox *box2 = new Q3GroupBox( grp, "Box");
    box2->setTitle(i18n("Preview"));
    Q3GridLayout *grid3 = new Q3GridLayout(box2,1,3,KDialog::marginHint(), KDialog::spacingHint());

    exampleLabel=new QLabel(box2);
    exampleLabel->setWhatsThis( i18n( "This will display a preview of your choice so you can know what it does before clicking the OK button to validate it." ) );
    grid3->addWidget(exampleLabel,0,1);

    grid->addWidget(box2,9,10,1,1);

    customFormatEdit = new QLineEdit( grp );
    grid->addWidget( customFormatEdit, 1, 1, 1, 1 );
    customFormatEdit->setHidden( true );

    listFormat=new Q3ListBox(grp);
    grid->addWidget(listFormat,2,7,1,1);
    listFormat->setWhatsThis( i18n( "Displays choices of format for the fraction, date or time formats." ) );
    layout->addWidget(grp);

    /* *** */

    Q3GroupBox *box = new Q3GroupBox( this, "Box");

    grid = new Q3GridLayout(box,3,4,KDialog::marginHint(), KDialog::spacingHint());

    postfix = new QLineEdit( box, "LineEdit_1" );
    postfix->setWhatsThis( i18n( "You can add here a Postfix such as a $HK symbol to the end of each cell content in the checked format." ) );
    grid->addWidget(postfix,2,1);
    precision = new KIntNumInput( dlg->precision, box, 10 );
    precision->setSpecialValueText(i18n("variable"));
    precision->setRange(-1,10,1,false);
    precision->setWhatsThis( i18n( "You can control how many digits are displayed after the decimal point for numeric values. This can also be changed using the Increase precision or Decrease precision icons in the Format toolbar. " ) );
    grid->addWidget(precision,1,1);

    prefix = new QLineEdit( box, "LineEdit_3" );
    prefix->setWhatsThis( i18n( "You can add here a Prefix such as a $ symbol at the start of each cell content in the checked format." ) );
    grid->addWidget(prefix,0,1);

    format = new QComboBox( box, "ListBox_1" );
    format->setWhatsThis( i18n( "You can choose whether positive values are displayed with a leading + sign and whether negative values are shown in red." ) );
    grid->addWidget(format,0,3);

    QLabel* tmpQLabel;
    tmpQLabel = new QLabel( box, "Label_1" );
    grid->addWidget(tmpQLabel,2,0);
    tmpQLabel->setText( i18n("Postfix:") );

    postfix->setText( dlg->postfix );

    tmpQLabel = new QLabel( box, "Label_2" );
    grid->addWidget(tmpQLabel,0,0);

    tmpQLabel->setText( i18n("Prefix:") );
    tmpQLabel = new QLabel( box, "Label_3" );
    grid->addWidget(tmpQLabel,1,0);
    tmpQLabel->setText( i18n("Precision:") );

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

    currency->setCurrentIndex( 0 );
    currency->hide();
    currencyLabel->hide();

    if ( !dlg->bFloatFormat || !dlg->bFloatColor )
        format->setCurrentIndex( 5 );
    else if ( dlg->floatFormat == Style::OnlyNegSigned && dlg->floatColor == Style::AllBlack )
        format->setCurrentIndex( 0 );
    else if ( dlg->floatFormat == Style::OnlyNegSigned && dlg->floatColor == Style::NegRed )
        format->setCurrentIndex( 1 );
    else if ( dlg->floatFormat == Style::AlwaysUnsigned && dlg->floatColor == Style::NegRed )
        format->setCurrentIndex( 2 );
    else if ( dlg->floatFormat == Style::AlwaysSigned && dlg->floatColor == Style::AllBlack )
        format->setCurrentIndex( 3 );
    else if ( dlg->floatFormat == Style::AlwaysSigned && dlg->floatColor == Style::NegRed )
        format->setCurrentIndex( 4 );
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
    connect(format, SIGNAL(activated(int)), this, SLOT(makeformat()));
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


            ValueFormatter *fmt = dlg->getDoc()->formatter();
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

    ValueFormatter *fmt = dlg->getDoc()->formatter();

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
  int index = currency->currentIndex();
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
  QColor color;
  Style::FloatFormat floatFormat;
  switch( format->currentIndex() )
  {
    case 0:
      floatFormat = Style::OnlyNegSigned;
      color = Qt::black;
      break;
    case 1:
      floatFormat =  Style::OnlyNegSigned;
      color = Qt::red;
      break;
    case 2:
      floatFormat =  Style::AlwaysUnsigned;
      color = Qt::red;
      break;
    case 3:
      floatFormat =  Style::AlwaysSigned;
      color = Qt::black;
      break;
    case 4:
      floatFormat =  Style::AlwaysSigned;
      color = Qt::red;
      break;
  }
  if (!dlg->value.isNumber() || dlg->value.asFloat() >= 0 || !format->isEnabled())
  {
    color = Qt::black;
  }
  ValueFormatter *fmt = dlg->getDoc()->formatter();
  tmp = fmt->formatText(dlg->value, newFormatType, precision->value(),
                        floatFormat,
                        prefix->isEnabled() ? prefix->text() : QString::null,
                        postfix->isEnabled() ? postfix->text() : QString::null,
                        newFormatType == Money_format ? dlg->cCurrency.symbol : QString::null);
  if (tmp.length() > 50)
    tmp = tmp.left (50);
  exampleLabel->setText(tmp.prepend("<font color=" + color.name() + ">").append("</font>"));
}

void CellFormatPageFloat::apply( CustomStyle * style )
{
  if ( postfix->text() != dlg->postfix )
  {
      if ( postfix->isEnabled())
        style->changePostfix( postfix->text() );
      else
        style->changePostfix( "" );
  }
  if ( prefix->text() != dlg->prefix )
  {
      if (prefix->isEnabled())
        style->changePrefix( prefix->text() );
      else
        style->changePrefix( "" );
  }

  if ( dlg->precision != precision->value() )
    style->changePrecision( precision->value() );

  if ( m_bFormatColorChanged )
  {
    switch( format->currentIndex() )
    {
     case 0:
      style->changeFloatFormat( Style::OnlyNegSigned );
      style->changeFloatColor( Style::AllBlack );
      break;
     case 1:
      style->changeFloatFormat( Style::OnlyNegSigned );
      style->changeFloatColor( Style::NegRed );
      break;
     case 2:
      style->changeFloatFormat( Style::AlwaysUnsigned );
      style->changeFloatColor( Style::NegRed );
      break;
     case 3:
      style->changeFloatFormat( Style::AlwaysSigned );
      style->changeFloatColor( Style::AllBlack );
      break;
     case 4:
      style->changeFloatFormat( Style::AlwaysSigned );
      style->changeFloatColor( Style::NegRed );
      break;
    }
  }
  if ( m_bFormatTypeChanged )
  {
    style->changeFormatType (newFormatType);
    if ( money->isChecked() )
    {
      Style::Currency cur;
      int index = currency->currentIndex();
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

void CellFormatPageFloat::apply(FormatManipulator* _obj)
{
  if ( postfix->text() != dlg->postfix )
    if ( postfix->isEnabled())
    {
      // If we are in here it *never* can be disabled - FIXME (Werner)!
      if ( postfix->isEnabled())
        _obj->setPostfix( postfix->text() );
      else
        _obj->setPostfix( "" );
    }
  if ( prefix->text() != dlg->prefix )
      if (prefix->isEnabled())
        _obj->setPrefix( prefix->text() );
      else
        _obj->setPrefix( "" );

  if ( dlg->precision != precision->value() )
      _obj->setPrecision( precision->value() );

  if (m_bFormatColorChanged)
  {
    switch( format->currentIndex() )
      {
      case 0:
        _obj->setFloatFormat( Style::OnlyNegSigned );
        _obj->setFloatColor( Style::AllBlack );
        break;
      case 1:
        _obj->setFloatFormat( Style::OnlyNegSigned );
        _obj->setFloatColor( Style::NegRed );
        break;
      case 2:
        _obj->setFloatFormat( Style::AlwaysUnsigned );
        _obj->setFloatColor( Style::NegRed );
        break;
      case 3:
        _obj->setFloatFormat( Style::AlwaysSigned );
        _obj->setFloatColor( Style::AllBlack );
        break;
      case 4:
        _obj->setFloatFormat( Style::AlwaysSigned );
        _obj->setFloatColor( Style::NegRed );
        break;
      }
  }
  if (m_bFormatTypeChanged)
  {
    _obj->setFormatType (newFormatType);
    if (money->isChecked())
    {
      Style::Currency cur;
      int index = currency->currentIndex();
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



/***************************************************************************
 *
 * CellFormatPageProtection
 *
 ***************************************************************************/

CellFormatPageProtection::CellFormatPageProtection( QWidget* parent, CellFormatDialog * _dlg )
  : ProtectionTab( parent ),
    m_dlg( _dlg )
{
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
      style->addProperty( Style::SDontPrintText );
    else
      style->removeProperty( Style::SDontPrintText );
  }

  if ( m_dlg->bIsProtected != m_bIsProtected->isChecked() )
  {
    if ( !m_bIsProtected->isChecked() )
      style->addProperty( Style::SNotProtected );
    else
      style->removeProperty( Style::SNotProtected );
  }

  if ( m_dlg->bHideAll != m_bHideAll->isChecked() )
  {
    if ( m_bHideAll->isChecked() )
      style->addProperty( Style::SHideAll );
    else
      style->removeProperty( Style::SHideAll );
  }

  if ( m_dlg->bHideFormula != m_bHideFormula->isChecked() )
  {
    if ( m_bHideFormula->isChecked() )
      style->addProperty( Style::SHideFormula );
    else
      style->removeProperty( Style::SHideFormula );
  }
}

void CellFormatPageProtection::apply(FormatManipulator* _obj)
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



/***************************************************************************
 *
 * CellFormatPageFont
 *
 ***************************************************************************/

CellFormatPageFont::CellFormatPageFont( QWidget* parent, CellFormatDialog *_dlg ) : FontTab( parent )
{
  dlg = _dlg;

  bTextColorUndefined = !dlg->bTextColor;

  connect( textColorButton, SIGNAL( changed( const QColor & ) ),
             this, SLOT( slotSetTextColor( const QColor & ) ) );


  QStringList tmpListFont;
  QFontDatabase *fontDataBase = new QFontDatabase();
  tmpListFont = fontDataBase->families();
  delete fontDataBase;

  family_combo->insertStringList( tmpListFont);
  selFont = dlg->textFont;

   if ( dlg->bTextFontFamily )
   {
        selFont.setFamily( dlg->textFontFamily );
        kDebug(36001) << "Family = " << dlg->textFontFamily << endl;

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

  connect( family_combo, SIGNAL(highlighted(const QString &)),
           SLOT(family_chosen_slot(const QString &)) );

  QStringList lst;
  lst.append("");
  for ( unsigned int i = 1; i < 100; ++i )
        lst.append( QString( "%1" ).arg( i ) );

  size_combo->insertStringList( lst );


  size_combo->setInsertPolicy(QComboBox::NoInsert);

  connect( size_combo, SIGNAL(activated(const QString &)),
           SLOT(size_chosen_slot(const QString &)) );
  connect( size_combo ,SIGNAL( textChanged(const QString &)),
        this,SLOT(size_chosen_slot(const QString &)));

  connect( weight_combo, SIGNAL(activated(const QString &)),
           SLOT(weight_chosen_slot(const QString &)) );

  connect( style_combo, SIGNAL(activated(const QString &)),
           SLOT(style_chosen_slot(const QString &)) );

  strike->setChecked(dlg->strike);
  connect( strike, SIGNAL( clicked()),
           SLOT(strike_chosen_slot()) );

  underline->setChecked(dlg->underline);
  connect( underline, SIGNAL( clicked()),
           SLOT(underline_chosen_slot()) );

  example_label->setText(i18n("Dolor Ipse"));

  connect(this,SIGNAL(fontSelected( const QFont&  )),
          this,SLOT(display_example( const QFont&)));

  setCombos();
  display_example( selFont );
  fontChanged=false;
  this->resize( 400, 400 );
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

  if ( ( size_combo->currentIndex() != 0 )
       && ( dlg->textFontSize != selFont.pointSize() ) )
    style->changeFontSize( selFont.pointSize() );

  if ( ( selFont.family() != dlg->textFontFamily )
       && !family_combo->currentText().isEmpty() )
    style->changeFontFamily( selFont.family() );

  uint flags = 0;

  if ( weight_combo->currentIndex() != 0 && selFont.bold() )
    flags |= Style::FBold;
  else
    flags &= ~(uint) Style::FBold;

  if ( style_combo->currentIndex() != 0 && selFont.italic() )
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

void CellFormatPageFont::apply(FormatManipulator* _obj)
{
  if ( !bTextColorUndefined && textColor != dlg->textColor )
    _obj->setTextColor( textColor );
  if (fontChanged)
  {
    if ( ( size_combo->currentIndex() != 0 )
         && ( dlg->textFontSize != selFont.pointSize() ) )
      _obj->setFontSize( selFont.pointSize() );
    if ( ( selFont.family() != dlg->textFontFamily ) && ( !family_combo->currentText().isEmpty() ) )
      _obj->setFontFamily( selFont.family() );
    if ( weight_combo->currentIndex() != 0 )
      _obj->setFontBold( selFont.bold() );
    if ( style_combo->currentIndex() != 0 )
      _obj->setFontItalic( selFont.italic() );
    _obj->setFontStrike( strike->isChecked() );
    _obj->setFontUnderline(underline->isChecked() );
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
     kDebug(36001) << "SIZE=" << dlg->textFontSize << endl;
     selFont.setPointSize( dlg->textFontSize );
     number_of_entries = size_combo->count();
     string.setNum( dlg->textFontSize );
     found = false;

     for (int i = 0; i < number_of_entries ; i++){
         if ( string == (QString) combo->text(i)){
             combo->setCurrentIndex(i);
             found = true;
             // kDebug(36001) << "Found Size " << string.data() << " setting to item " i << endl;
             break;
         }
     }
 }
 else
     combo->setCurrentIndex( 0 );

 if ( !dlg->bTextFontBold )
     weight_combo->setCurrentIndex(0);
 else if ( dlg->textFontBold )
 {
     selFont.setBold( dlg->textFontBold );
     weight_combo->setCurrentIndex(2);
 }
 else
 {
     selFont.setBold( dlg->textFontBold );
     weight_combo->setCurrentIndex(1);
 }

 if ( !dlg->bTextFontItalic )
     weight_combo->setCurrentIndex(0);
 else if ( dlg->textFontItalic )
 {
     selFont.setItalic( dlg->textFontItalic );
     style_combo->setCurrentIndex(2);
 }
 else
 {
     selFont.setItalic( dlg->textFontItalic );
     style_combo->setCurrentIndex(1);
 }
}



/***************************************************************************
 *
 * CellFormatPagePosition
 *
 ***************************************************************************/

CellFormatPagePosition::CellFormatPagePosition( QWidget* parent, CellFormatDialog *_dlg )
  : PositionTab(parent ),
    dlg( _dlg )
{
    if ( dlg->alignX == Style::Left )
        left->setChecked( true );
    else if ( dlg->alignX == Style::Center )
        center->setChecked( true );
    else if ( dlg->alignX == Style::Right )
        right->setChecked( true );
    else if ( dlg->alignX == Style::HAlignUndefined )
        standard->setChecked( true );

    connect(horizontalGroup,  SIGNAL(clicked(int)), this, SLOT(slotStateChanged(int)));

    if ( dlg->alignY ==Style::Top )
        top->setChecked( true );
    else if ( dlg->alignY ==Style::Middle )
        middle->setChecked(true );
    else if ( dlg->alignY ==Style::Bottom )
        bottom->setChecked( true );

    multi->setChecked(dlg->bMultiRow);

    vertical->setChecked(dlg->bVerticalText);

    angleRotation->setValue(-dlg->textRotation);//annma
    spinBox3->setValue(-dlg->textRotation);
    if ( dlg->textRotation != 0 )
    {
        multi->setEnabled(false );
	    vertical->setEnabled(false);
    }

    mergeCell->setChecked(dlg->isMerged);
    mergeCell->setEnabled(!dlg->oneCell && ((!dlg->isRowSelected) && (!dlg->isColumnSelected)));

    Q3GridLayout *grid2 = new Q3GridLayout(indentGroup, 1, 1, KDialog::marginHint(), KDialog::spacingHint());
    grid2->addRowSpacing( 0, indentGroup->fontMetrics().height()/8 ); // groupbox title
    m_indent = new KoUnitDoubleSpinBox( indentGroup, 0.0,  400.0, 10.0,dlg->indent,dlg->getDoc()->unit() );
    grid2->addWidget(m_indent, 0, 0);

    width = new KoUnitDoubleSpinBox( m_widthPanel );
    Q3GridLayout *gridWidth = new Q3GridLayout(m_widthPanel, 1, 1, 0, 0);
    gridWidth->addWidget(width, 0, 0);
    width->setValue ( dlg->widthSize );
    width->setUnit( dlg->getDoc()->unit() );
    //to ensure, that we don't get rounding problems, we store the displayed value (for later check for changes)
    dlg->widthSize = width->value();

    if ( dlg->isRowSelected )
        width->setEnabled(false);

    defaultWidth->setText(i18n("Default width (%1 %2)").arg(KoUnit::toUserValue(dlg->defaultWidthSize, dlg->getDoc()->unit()), 0, 'f', 2).arg(dlg->getDoc()->unitName()));
    if ( dlg->isRowSelected )
        defaultWidth->setEnabled(false);

    height=new KoUnitDoubleSpinBox( m_heightPanel );
    Q3GridLayout *gridHeight = new Q3GridLayout(m_heightPanel, 1, 1, 0, 0);
    gridHeight->addWidget(height, 0, 0);
    height->setValue( dlg->heightSize );
    height->setUnit(  dlg->getDoc()->unit() );
    //to ensure, that we don't get rounding problems, we store the displayed value (for later check for changes)
    dlg->heightSize = height->value();

    if ( dlg->isColumnSelected )
        height->setEnabled(false);

    defaultHeight->setText(i18n("Default height (%1 %2)").arg(KoUnit::toUserValue(dlg->defaultHeightSize, dlg->getDoc()->unit()), 0, 'f', 2).arg(dlg->getDoc()->unitName())); //annma
    if ( dlg->isColumnSelected )
        defaultHeight->setEnabled(false);

    // in case we're editing a style, we disable the cell size settings
    if (dlg->getStyle())
    {
      sizeCellGroup->setEnabled(false);
    }

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
  if ( top->isChecked() && dlg->alignY != Style::Top )
    style->changeVAlign( Style::Top );
  else if ( bottom->isChecked() && dlg->alignY != Style::Bottom )
    style->changeVAlign( Style::Bottom );
  else if ( middle->isChecked() && dlg->alignY != Style::Middle )
    style->changeVAlign( Style::Middle );

  if ( left->isChecked() && dlg->alignX != Style::Left )
    style->changeHAlign( Style::Left );
  else if ( right->isChecked() && dlg->alignX != Style::Right )
    style->changeHAlign( Style::Right );
  else if ( center->isChecked() && dlg->alignX != Style::Center )
    style->changeHAlign( Style::Center );
  else if ( standard->isChecked() && dlg->alignX != Style::HAlignUndefined )
    style->changeHAlign( Style::HAlignUndefined );

  if ( m_bOptionText )
  {
    if ( multi->isEnabled() )
    {
      if ( multi->isChecked() )
        style->addProperty( Style::SMultiRow );
      else
        style->removeProperty( Style::SMultiRow );
    }
  }

  if ( m_bOptionText )
  {
    if ( vertical->isEnabled() )
    {
      if ( vertical->isChecked() )
        style->addProperty( Style::SVerticalText );
      else
        style->removeProperty( Style::SVerticalText );
    }
  }

  if ( dlg->textRotation != angleRotation->value() )
    style->changeRotateAngle( (-angleRotation->value()) );

  if ( m_indent->isEnabled()
       && dlg->indent != m_indent->value() )
    style->changeIndent( m_indent->value() );
}

void CellFormatPagePosition::apply(FormatManipulator* _obj)
{
  Style::HAlign  ax;
  Style::VAlign ay;

  if ( top->isChecked() )
    ay = Style::Top;
  else if ( bottom->isChecked() )
    ay = Style::Bottom;
  else if ( middle->isChecked() )
    ay = Style::Middle;
  else
    ay = Style::Middle; // Default, just in case

  if ( left->isChecked() )
    ax = Style::Left;
  else if ( right->isChecked() )
    ax = Style::Right;
  else if ( center->isChecked() )
    ax = Style::Center;
  else if ( standard->isChecked() )
    ax = Style::HAlignUndefined;
  else
    ax = Style::HAlignUndefined; //Default, just in case

  if ( top->isChecked() && ay != dlg->alignY )
    _obj->setVerticalAlignment( Style::Top );
  else if ( bottom->isChecked() && ay != dlg->alignY )
    _obj->setVerticalAlignment( Style::Bottom );
  else if ( middle->isChecked() && ay != dlg->alignY )
    _obj->setVerticalAlignment( Style::Middle );

  if ( left->isChecked() && ax != dlg->alignX )
    _obj->setHorizontalAlignment( Style::Left );
  else if ( right->isChecked() && ax != dlg->alignX )
    _obj->setHorizontalAlignment( Style::Right );
  else if ( center->isChecked() && ax != dlg->alignX )
    _obj->setHorizontalAlignment( Style::Center );
  else if ( standard->isChecked() && ax != dlg->alignX )
    _obj->setHorizontalAlignment( Style::HAlignUndefined );

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
    return dlg->defaultHeightSize; // guess who calls this!
  else
      return height->value();
}

double CellFormatPagePosition::getSizeWidth() const
{
  if ( defaultWidth->isChecked() )
    return dlg->defaultWidthSize; // guess who calls this!
  else
        return width->value();
}



/***************************************************************************
 *
 * BorderButton
 *
 ***************************************************************************/

BorderButton::BorderButton( QWidget *parent, const char *_name ) : QPushButton(parent,_name)
{
  penStyle = Qt::NoPen;
  penWidth = 1;
  penColor = colorGroup().text();
  setToggleButton( true );
  setChecked( false);
  setChanged(false);
}
void BorderButton::mousePressEvent( QMouseEvent * )
{

  this->setChecked(!isChecked());
  emit clicked( this );
}

void BorderButton::setUndefined()
{
 setPenStyle(Qt::SolidLine );
 setPenWidth(1);
 setColor(colorGroup().midlight());
}


void BorderButton::unselect()
{
  setChecked(false);
  setPenWidth(1);
  setPenStyle(Qt::NoPen);
  setColor( colorGroup().text() );
  setChanged(true);
}



/***************************************************************************
 *
 * Border
 *
 ***************************************************************************/

Border::Border( QWidget *parent, const char *_name,bool _oneCol, bool _oneRow )
    : Q3Frame( parent, _name )
{
  oneCol=_oneCol;
  oneRow=_oneRow;
}


#define OFFSETX 5
#define OFFSETY 5
void Border::paintEvent( QPaintEvent *_ev )
{
  Q3Frame::paintEvent( _ev );
  QPen pen;
  QPainter painter;
  painter.begin( this );
  pen=QPen( colorGroup().midlight(),2,Qt::SolidLine);
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



/***************************************************************************
 *
 * CellFormatPageBorder
 *
 ***************************************************************************/

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
  preview->setPattern( Qt::black , 1, Qt::SolidLine );
  this->resize( 400, 400 );
}

void CellFormatPageBorder::InitializeGrids()
{
  Q3GridLayout *grid = new Q3GridLayout(this,5,2,KDialog::marginHint(), KDialog::spacingHint());
  Q3GridLayout *grid2 = NULL;
  QGroupBox* tmpQGroupBox = 0;

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
  tmpQGroupBox = new Q3GroupBox( this, "GroupBox_1" );
  tmpQGroupBox->setTitle( i18n("Border") );
  tmpQGroupBox->setAlignment( Qt::AlignLeft );
  grid2 = new Q3GridLayout(tmpQGroupBox,6,5,KDialog::marginHint(), KDialog::spacingHint());
  int fHeight = tmpQGroupBox->fontMetrics().height();
  grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

  area=new Border(tmpQGroupBox,"area",dlg->oneCol,dlg->oneRow);
  grid2->addWidget(area,2,4,1,3);
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

  grid->addWidget(tmpQGroupBox,0,2,0,0);

  /* the remove, all, and outline border buttons are in a second box down
     below.*/

  tmpQGroupBox = new Q3GroupBox( this, "GroupBox_3" );
  tmpQGroupBox->setTitle( i18n("Preselect") );
  tmpQGroupBox->setAlignment( Qt::AlignLeft );

  grid2 = new Q3GridLayout(tmpQGroupBox,1,3,KDialog::marginHint(), KDialog::spacingHint());

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

  grid->addWidget(tmpQGroupBox,3,4,0,0);

  /* now set up the group box with the pattern selector */
  tmpQGroupBox = new Q3GroupBox( this, "GroupBox_10" );
#warning "kde4 port it"
  //tmpQGroupBox->setFrameStyle( Q3Frame::Box | Q3Frame::Sunken );
  tmpQGroupBox->setTitle( i18n("Pattern") );
  tmpQGroupBox->setAlignment( Qt::AlignLeft );

  grid2 = new Q3GridLayout(tmpQGroupBox,7,2,KDialog::marginHint(), KDialog::spacingHint());
  fHeight = tmpQGroupBox->fontMetrics().height();
  grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

  char name[] = "PatternXX";
  Q_ASSERT(NUM_BORDER_PATTERNS < 100);

  for (int i=0; i < NUM_BORDER_PATTERNS; i++)
  {
    name[7] = '0' + (i+1) / 10;
    name[8] = '0' + (i+1) % 10;
    pattern[i] = new PatternSelect( tmpQGroupBox, name );
    pattern[i]->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(pattern[i], i % 5 + 1, i / 5);
    /* this puts them in the pattern:
       1  6
       2  7
       3  8
       4  9
       5  10
    */
  }

  color = new KColorButton ( tmpQGroupBox );
  color->setObjectName( "PushButton_1" );
  grid2->addWidget(color,7,1);

  QLabel *tmpQLabel = new QLabel( tmpQGroupBox, "Label_6" );
  tmpQLabel->setText( i18n("Color:") );
  grid2->addWidget(tmpQLabel,7,0);

  /* tack on the 'customize' border pattern selector */
  Q3GridLayout *grid3 = new Q3GridLayout( this, 2, 2, KDialog::marginHint(), KDialog::spacingHint() );
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
  size->setCurrentIndex(1);

  style=new QComboBox(tmpQGroupBox);
  grid3->addWidget(style,1,0);
  style->insertItem(paintFormatPixmap(Qt::DotLine),0 );
  style->insertItem(paintFormatPixmap(Qt::DashLine) ,1);
  style->insertItem(paintFormatPixmap(Qt::DashDotLine),2 );
  style->insertItem(paintFormatPixmap(Qt::DashDotDotLine),3  );
  style->insertItem(paintFormatPixmap(Qt::SolidLine),4);
  style->setBackgroundColor( colorGroup().background() );

  grid2->addItem(grid3,6,6,0,1);
  grid->addWidget(tmpQGroupBox,0,3,1,1);

  /* Now the preview box is put together */
  tmpQGroupBox = new Q3GroupBox(this, "GroupBox_4" );
  tmpQGroupBox->setTitle( i18n("Preview") );
  tmpQGroupBox->setAlignment( Qt::AlignLeft );

  grid2 = new Q3GridLayout(tmpQGroupBox,1,1,KDialog::marginHint(), KDialog::spacingHint());
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
        borderButtons[i]->setChecked(true);
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
  pattern[0]->setPattern( Qt::black, 1, Qt::DotLine );
  pattern[1]->setPattern( Qt::black, 1, Qt::DashLine );
  pattern[2]->setPattern( Qt::black, 1, Qt::SolidLine );
  pattern[3]->setPattern( Qt::black, 1, Qt::DashDotLine );
  pattern[4]->setPattern( Qt::black, 1, Qt::DashDotDotLine );
  pattern[5]->setPattern( Qt::black, 2, Qt::SolidLine );
  pattern[6]->setPattern( Qt::black, 3, Qt::SolidLine );
  pattern[7]->setPattern( Qt::black, 4, Qt::SolidLine );
  pattern[8]->setPattern( Qt::black, 5, Qt::SolidLine );
  pattern[9]->setPattern( Qt::black, 1, Qt::NoPen );

  slotSetColorButton( Qt::black );
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
    preview->setPattern( Qt::black , 1, Qt::SolidLine );
  }
}

void CellFormatPageBorder::slotChangeStyle(const QString &)
{
  /* if they try putting text in the size box, then erase the line */
  slotChangeStyle(0);
}

void CellFormatPageBorder::slotChangeStyle(int)
{
  int index = style->currentIndex();
  QString tmp;
  int penSize = size->currentText().toInt();
  if ( !penSize)
  {
    preview->setPattern( preview->getColor(), penSize, Qt::NoPen );
  }
  else
  {
    switch(index)
    {
    case 0:
      preview->setPattern( preview->getColor(), penSize, Qt::DotLine );
      break;
    case 1:
      preview->setPattern( preview->getColor(), penSize, Qt::DashLine );
      break;
    case 2:
      preview->setPattern( preview->getColor(), penSize, Qt::DashDotLine );
      break;
    case 3:
      preview->setPattern( preview->getColor(), penSize, Qt::DashDotDotLine );
      break;
    case 4:
      preview->setPattern( preview->getColor(), penSize, Qt::SolidLine );
      break;
    default:
      kDebug(36001)<<"Error in combobox\n";
      break;
    }
  }
  slotUnselect2(preview);
}

QPixmap CellFormatPageBorder::paintFormatPixmap(Qt::PenStyle _style)
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

void CellFormatPageBorder::apply(FormatManipulator* obj)
{
  if (borderButtons[BorderType_Horizontal]->isChanged())
    applyHorizontalOutline(obj);

  if (borderButtons[BorderType_Vertical]->isChanged())
    applyVerticalOutline(obj);

  if ( borderButtons[BorderType_Left]->isChanged() )
    applyLeftOutline(obj);

  if ( borderButtons[BorderType_Right]->isChanged() )
    applyRightOutline(obj);

  if ( borderButtons[BorderType_Top]->isChanged() )
    applyTopOutline(obj);

  if ( borderButtons[BorderType_Bottom]->isChanged() )
    applyBottomOutline(obj);

  if ( borderButtons[BorderType_RisingDiagonal]->isChanged() ||
       borderButtons[BorderType_FallingDiagonal]->isChanged() )
    applyDiagonalOutline(obj);
}

void CellFormatPageBorder::applyTopOutline(FormatManipulator* obj)
{
  BorderButton * top = borderButtons[BorderType_Top];

  QPen tmpPen( top->getColor(), top->getPenWidth(), top->getPenStyle());

  if ( dlg->getStyle() )
  {
    dlg->getStyle()->changeTopBorderPen( tmpPen );
  }
  else
  {
    if (borderButtons[BorderType_Top]->isChanged())
      obj->setTopBorderPen( tmpPen );
  }
}

void CellFormatPageBorder::applyBottomOutline(FormatManipulator* obj)
{
  BorderButton * bottom = borderButtons[BorderType_Bottom];

  QPen tmpPen( bottom->getColor(), bottom->getPenWidth(), bottom->getPenStyle() );

  if ( dlg->getStyle() )
  {
    dlg->getStyle()->changeBottomBorderPen( tmpPen );
  }
  else
  {
    if (borderButtons[BorderType_Bottom]->isChanged())
      obj->setBottomBorderPen( tmpPen );
  }
}

void CellFormatPageBorder::applyLeftOutline(FormatManipulator* obj)
{
  BorderButton * left = borderButtons[BorderType_Left];
  QPen tmpPen( left->getColor(), left->getPenWidth(), left->getPenStyle() );

  if ( dlg->getStyle() )
  {
    dlg->getStyle()->changeLeftBorderPen( tmpPen );
  }
  else
  {
    if (borderButtons[BorderType_Left]->isChanged())
      obj->setLeftBorderPen( tmpPen );
  }
}

void CellFormatPageBorder::applyRightOutline(FormatManipulator* obj)
{
  BorderButton* right = borderButtons[BorderType_Right];
  QPen tmpPen( right->getColor(), right->getPenWidth(), right->getPenStyle() );

  if ( dlg->getStyle() )
  {
    dlg->getStyle()->changeRightBorderPen( tmpPen );
  }
  else
  {
    if (borderButtons[BorderType_Right]->isChanged())
      obj->setRightBorderPen( tmpPen );
  }
}

void CellFormatPageBorder::applyDiagonalOutline(FormatManipulator* obj)
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
  else
  {
    if ( fallDiagonal->isChanged() )
      obj->setFallDiagonalPen( tmpPenFall );
    if ( goUpDiagonal->isChanged() )
      obj->setGoUpDiagonalPen( tmpPenGoUp );
  }
}

void CellFormatPageBorder::applyHorizontalOutline(FormatManipulator* obj)
{
  QPen tmpPen( borderButtons[BorderType_Horizontal]->getColor(),
               borderButtons[BorderType_Horizontal]->getPenWidth(),
               borderButtons[BorderType_Horizontal]->getPenStyle());

  if ( dlg->getStyle() )
  {
    dlg->getStyle()->changeTopBorderPen( tmpPen );
  }
  else
  {
    if (borderButtons[BorderType_Horizontal]->isChanged())
      obj->setHorizontalPen( tmpPen );
  }
}

void CellFormatPageBorder::applyVerticalOutline(FormatManipulator* obj)
{
  BorderButton* vertical = borderButtons[BorderType_Vertical];
  QPen tmpPen( vertical->getColor(), vertical->getPenWidth(),
               vertical->getPenStyle());

  if ( dlg->getStyle() )
  {
    dlg->getStyle()->changeLeftBorderPen( tmpPen );
  }
  else
  {
    if (borderButtons[BorderType_Vertical]->isChanged())
      obj->setVerticalPen( tmpPen );
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

  _p->setChecked(false);
  if (_p == remove)
  {
    for (int i=BorderType_Top; i < BorderType_END; i++)
    {
      if (borderButtons[i]->isChecked())
      {
        borderButtons[i]->unselect();
      }
    }
  }
  if (_p==outline)
  {
    top->setChecked(true);
    top->setPenWidth(preview->getPenWidth());
    top->setPenStyle(preview->getPenStyle());
    top->setColor( currentColor );
    top->setChanged(true);
    bottom->setChecked(true);
    bottom->setPenWidth(preview->getPenWidth());
    bottom->setPenStyle(preview->getPenStyle());
    bottom->setColor( currentColor );
    bottom->setChanged(true);
    left->setChecked(true);
    left->setPenWidth(preview->getPenWidth());
    left->setPenStyle(preview->getPenStyle());
    left->setColor( currentColor );
    left->setChanged(true);
    right->setChecked(true);
    right->setPenWidth(preview->getPenWidth());
    right->setPenStyle(preview->getPenStyle());
    right->setColor( currentColor );
    right->setChanged(true);
  }
  if (_p==all)
  {
    if (dlg->oneRow==false)
    {
      horizontal->setChecked(true);
      horizontal->setPenWidth(preview->getPenWidth());
      horizontal->setPenStyle(preview->getPenStyle());
      horizontal->setColor( currentColor );
      horizontal->setChanged(true);
    }
    if (dlg->oneCol==false)
    {
      vertical->setChecked(true);
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

  if (_p->isChecked())
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
  if (_p->isChecked())
  {
    _p->unselect();
  }
  else
  {
    _p->setChecked(true);
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
       && top->isChecked())
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
       && bottom->isChecked())
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
       && left->isChecked())
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
       && right->isChecked())
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
         && vertical->isChecked())
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
         && horizontal->isChecked())
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



/***************************************************************************
 *
 * BrushSelect
 *
 ***************************************************************************/

BrushSelect::BrushSelect( QWidget *parent, const char * ) : Q3Frame( parent )
{
    brushStyle = Qt::NoBrush;
    brushColor = Qt::red;
    selected = false;
}

void BrushSelect::setPattern( const QColor &_color,Qt::BrushStyle _style )
{
    brushStyle = _style;
    brushColor = _color;
    repaint();
}


void BrushSelect::paintEvent( QPaintEvent *_ev )
{
    Q3Frame::paintEvent( _ev );

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
    setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    repaint();
}

void BrushSelect::slotSelect()
{
    selected = true;

    setLineWidth( 2 );
    setFrameStyle( Q3Frame::Panel | Q3Frame::Plain );
    repaint();
}



/***************************************************************************
 *
 * CellFormatPagePattern
 *
 ***************************************************************************/

CellFormatPagePattern::CellFormatPagePattern( QWidget* parent, CellFormatDialog *_dlg ) : QWidget( parent )
{
    dlg = _dlg;

    bBgColorUndefined = !dlg->bBgColor;

    Q3GridLayout *grid = new Q3GridLayout(this,5,2,KDialog::marginHint(), KDialog::spacingHint());

    Q3GroupBox* tmpQGroupBox;
    tmpQGroupBox = new Q3GroupBox( this, "GroupBox_20" );
    tmpQGroupBox->setTitle( i18n("Pattern") );
    tmpQGroupBox->setAlignment( Qt::AlignLeft );

    Q3GridLayout *grid2 = new Q3GridLayout(tmpQGroupBox,8,3,KDialog::marginHint(), KDialog::spacingHint());
    int fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title


    brush1 = new BrushSelect( tmpQGroupBox, "Frame_1" );
    brush1->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush1,1,0);

    brush2 = new BrushSelect( tmpQGroupBox, "Frame_2" );
    brush2->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush2,1,1);

    brush3 = new BrushSelect( tmpQGroupBox, "Frame_3" );
    brush3->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush3,1,2);

    brush4 = new BrushSelect( tmpQGroupBox, "Frame_4" );
    brush4->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush4,2,0);

    brush5 = new BrushSelect( tmpQGroupBox, "Frame_5" );
    brush5->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush5,2,1);

    brush6 = new BrushSelect( tmpQGroupBox, "Frame_6" );
    brush6->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush6,2,2);

    brush7 = new BrushSelect( tmpQGroupBox, "Frame_7" );
    brush7->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush7,3,0);

    brush8 = new BrushSelect( tmpQGroupBox, "Frame_8" );
    brush8->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush8,3,1);

    brush9 = new BrushSelect( tmpQGroupBox, "Frame_9" );
    brush9->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush9,3,2);

    brush10 = new BrushSelect( tmpQGroupBox, "Frame_10" );
    brush10->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush10,4,0);

    brush11 = new BrushSelect( tmpQGroupBox, "Frame_11" );
    brush11->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush11,4,1);

    brush12 = new BrushSelect( tmpQGroupBox, "Frame_12" );
    brush12->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush12,4,2);

    brush13 = new BrushSelect( tmpQGroupBox, "Frame_13" );
    brush13->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush13,5,0);

    brush14 = new BrushSelect( tmpQGroupBox, "Frame_14" );
    brush14->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush14,5,1);

    brush15 = new BrushSelect( tmpQGroupBox, "Frame_15" );
    brush15->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
    grid2->addWidget(brush15,5,2);

    Q3GridLayout *grid3 = new Q3GridLayout( 1, 2 );
    color = new KColorButton ( tmpQGroupBox );
    color->setObjectName( "ColorButton_1" );
    grid3->addWidget(color,0,1);

    QLabel *tmpQLabel = new QLabel( tmpQGroupBox, "Label_1" );
    tmpQLabel->setText( i18n("Color:") );
    grid3->addWidget(tmpQLabel,0,0);

    grid2->addItem(grid3,6,6,0,2);

    grid3 = new Q3GridLayout( 1, 3 );
    grid3->setSpacing(KDialog::spacingHint());

    tmpQLabel = new QLabel( tmpQGroupBox, "Label_2" );
    grid3->addWidget(tmpQLabel,0,0);
    tmpQLabel->setText( i18n("Background color:") );

    bgColorButton = new KColorButton( tmpQGroupBox );
    bgColorButton->setObjectName( "ColorButton" );
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

    grid2->addItem(grid3,7,7,0,2);

    grid->addWidget(tmpQGroupBox,0,3,0,0);

    tmpQGroupBox = new Q3GroupBox( this, "GroupBox1" );
    tmpQGroupBox->setTitle( i18n("Preview") );
#warning "kde4: port it"
    //tmpQGroupBox->setFrameStyle( Q3Frame::Box | Q3Frame::Sunken );
    tmpQGroupBox->setAlignment( Qt::AlignLeft );

    grid2 = new Q3GridLayout(tmpQGroupBox,2,1,KDialog::marginHint(), KDialog::spacingHint());
    fHeight = tmpQGroupBox->fontMetrics().height();
    grid2->addRowSpacing( 0, fHeight/2 ); // groupbox title

    current = new BrushSelect( tmpQGroupBox, "Current" );
    current->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
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
    kDebug(36001) << "Error in brushStyle" << endl;
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

void CellFormatPagePattern::apply(FormatManipulator *_obj)
{
  if ( selectedBrush != 0L
       && ( dlg->brushStyle != selectedBrush->getBrushStyle()
            || dlg->brushColor != selectedBrush->getBrushColor() ) )
    _obj->setBackgroundBrush( QBrush( selectedBrush->getBrushColor(), selectedBrush->getBrushStyle() ) );

  if ( bgColor == dlg->bgColor )
    return;

  if ( b_notAnyColor)
    _obj->setBackgroundColor( QColor() );
  else if ( !bBgColorUndefined )
    _obj->setBackgroundColor( bgColor );
}

#include "kspread_dlg_layout.moc"

