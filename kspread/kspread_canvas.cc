#include "kspread_canvas.h"
#include "kspread_util.h"
#include "kspread_editors.h"
#include "kspread_map.h"
#include "kspread_undo.h"

#include "kspread_doc.h"

#include <kmessagebox.h>
#include <kcursor.h>
#include <kdebug.h>
#include <krun.h>

#include <assert.h>
#include <stdlib.h>
#include <qlabel.h>
#include <qdrawutil.h>
#include <qbutton.h>
#include <qapplication.h>
#include <qtimer.h>

KSpreadLocationEditWidget::KSpreadLocationEditWidget( QWidget * _parent,
                                                      KSpreadView * _view )
    : QLineEdit( _parent, "KSpreadLocationEditWidget" ),
      m_pView(_view)
{
  m_pView = _view;
}

void KSpreadLocationEditWidget::keyPressEvent( QKeyEvent * _ev )
{
    // Do not handle special keys and accelerators. This is
    // done by QLineEdit.
    if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
    {
        QLineEdit::keyPressEvent( _ev );
        // Never allow that keys are passed on to the parent.
        _ev->accept();

        return;
    }

    // Handle some special keys here. Eve
    switch( _ev->key() )
    {
    case Key_Return:
    case Key_Enter:
        {
	    QString tmp;
            QString ltext = text();
            tmp = ltext.lower();
            QValueList<Reference>::Iterator it;
	    QValueList<Reference> area = m_pView->doc()->listArea();
	    for ( it = area.begin(); it != area.end(); ++it )
            {
                if ((*it).ref_name == tmp)
                {
                    QString tmp;
                    tmp = (*it).table_name;
                    tmp += "!";
                    tmp += util_rangeName((*it).rect);
                    m_pView->canvasWidget()->gotoLocation( KSpreadRange(tmp, m_pView->doc()->map()));
                    return;
                }
            }

            int pos;

            // Set the cell component to uppercase:
            // Table1!a1 -> Table1!A2
            pos = ltext.find('!');
            if( pos !=- 1 )
                tmp = ltext.left(pos)+ltext.mid(pos).upper();
            else
                tmp = ltext.upper();

            // Selection entered in location widget
            if ( ltext.contains( ':' ) )
                m_pView->canvasWidget()->gotoLocation( KSpreadRange( tmp, m_pView->doc()->map() ) );
            // Location entered in location widget
            else
            {
                KSpreadPoint point( tmp, m_pView->doc()->map());
                bool validName = true;
                for (unsigned int i = 0; i < ltext.length(); ++i)
                {
                    if (!ltext[i].isLetter())
                    {
                        validName = false;
                        break;
                    }
                }
                if ( !point.isValid() && validName)
                {
                    QRect rect( m_pView->activeTable()->selection() );
                    KSpreadTable * t = m_pView->activeTable();
                    // set area name on current selection/cell

                    m_pView->doc()->addAreaName(rect, ltext.lower(),
                                                t->tableName());
                }

                if (!validName)
                    m_pView->canvasWidget()->gotoLocation( point );
            }

            // Set the focus back on the canvas.
            m_pView->canvasWidget()->setFocus();
            _ev->accept();
        }
        break;
    // Escape pressed, restore original value
    case Key_Escape:
        // #### Torben says: This is duplicated code. Bad.
        if ( m_pView->activeTable()->singleCellSelection() ) {
            setText( util_encodeColumnLabelText( m_pView->canvasWidget()->markerColumn() )
                     + QString::number( m_pView->canvasWidget()->markerRow() ) );
        } else {
            setText( util_encodeColumnLabelText( m_pView->activeTable()->selection().left() )
                     + QString::number( m_pView->activeTable()->selection().top() )
                     + ":"
                     + util_encodeColumnLabelText( m_pView->activeTable()->selection().right() )
                     + QString::number( m_pView->activeTable()->selection().bottom() ) );
        }
        m_pView->canvasWidget()->setFocus();
        _ev->accept();
        break;
    default:
        QLineEdit::keyPressEvent( _ev );
        // Never allow that keys are passed on to the parent.
        _ev->accept();
    }
}

/****************************************************************
 *
 * KSpreadEditWidget
 * The line-editor that appears above the table and allows to
 * edit the cells content.
 *
 ****************************************************************/

KSpreadEditWidget::KSpreadEditWidget( QWidget *_parent, KSpreadCanvas *_canvas,
                                      QButton *cancelButton, QButton *okButton )
  : QLineEdit( _parent, "KSpreadEditWidget" )
{
  m_pCanvas = _canvas;
  // Those buttons are created by the caller, so that they are inserted
  // properly in the layout - but they are then managed here.
  m_pCancelButton = cancelButton;
  m_pOkButton = okButton;
  if ( !m_pCanvas->doc()->isReadWrite() )
    setEnabled( false );
  else
  {
    QObject::connect( m_pCancelButton, SIGNAL( clicked() ),
                      this, SLOT( slotAbortEdit() ) );
    QObject::connect( m_pOkButton, SIGNAL( clicked() ),
                      this, SLOT( slotDoneEdit() ) );
  }
  setEditMode( false ); // disable buttons
}


void KSpreadEditWidget::showEditWidget(bool _show)
{
    if(_show)
	{
	    m_pCancelButton->show();
	    m_pOkButton->show();
	    show();
	}
    else
	{
	    m_pCancelButton->hide();
	    m_pOkButton->hide();
	    hide();
	}
}

void KSpreadEditWidget::slotAbortEdit()
{
    m_pCanvas->deleteEditor( false /*discard changes*/ );
    // will take care of the buttons
}

void KSpreadEditWidget::slotDoneEdit()
{
    m_pCanvas->deleteEditor( true /*keep changes*/ );
    // will take care of the buttons
}

void KSpreadEditWidget::keyPressEvent ( QKeyEvent* _ev )
{
    // Dont handle special keys and accelerators
    if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
    {
        QLineEdit::keyPressEvent( _ev );
        return;
    }

  if ( !m_pCanvas->doc()->isReadWrite() )
    return;

  if ( !m_pCanvas->editor() )
      {
        // Start editing the current cell
        m_pCanvas->createEditor( KSpreadCanvas::CellEditor,false );
      }
  KSpreadTextEditor* cellEditor = (KSpreadTextEditor*) m_pCanvas->editor();

  switch ( _ev->key() )
  {
    case Key_Down:
    case Key_Up:
    case Key_Return:
    case Key_Enter:
      cellEditor->setText( text());
      // Don't allow to start a chooser when pressing the arrow keys
      // in this widget, since only up and down would work anyway.
      // This is why we call slotDoneEdit now, instead of sending
      // to the canvas.
      //QApplication::sendEvent( m_pCanvas, _ev );
      slotDoneEdit();
      m_pCanvas->view()->updateEditWidget();
      _ev->accept();
      break;
    case Key_F2:
      cellEditor->setFocus();
      cellEditor->setText( text());
      cellEditor->setCursorPosition(cursorPosition());
      break;
    default:

      QLineEdit::keyPressEvent( _ev );

      setFocus();
      cellEditor->blockCheckChoose( TRUE );
      cellEditor->setText( text() );
      cellEditor->blockCheckChoose( FALSE );
      cellEditor->setCursorPosition( cursorPosition() );
  }
}

void KSpreadEditWidget::setEditMode( bool mode )
{
  m_pCancelButton->setEnabled(mode);
  m_pOkButton->setEnabled(mode);
}

void KSpreadEditWidget::focusOutEvent( QFocusEvent* ev )
{
  //kdDebug(36001) << "EditWidget lost focus" << endl;
  // See comment about setLastEditorWithFocus
  m_pCanvas->setLastEditorWithFocus( KSpreadCanvas::EditWidget );

  QLineEdit::focusOutEvent( ev );
}

void KSpreadEditWidget::setText( const QString& t )
{
  if ( t == text() ) // Why this? (David)
    return;

  QLineEdit::setText( t );
}

/****************************************************************
 *
 * KSpreadCanvas
 *
 ****************************************************************/

KSpreadCanvas::KSpreadCanvas( QWidget *_parent, KSpreadView *_view, KSpreadDoc* _doc )
    : QWidget( _parent, "", /*WNorthWestGravity*/ WStaticContents| WResizeNoErase | WRepaintNoErase )
{
  length_namecell = 0;
  m_chooseStartTable = 0;
  m_pEditor = 0;
  m_bChoose = FALSE;

  QWidget::setFocusPolicy( QWidget::StrongFocus );

  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );

  m_iXOffset = 0;
  m_iYOffset = 0;
  m_pView = _view;
  m_pDoc = _doc;
  // m_eAction = DefaultAction;
  m_eMouseAction = NoAction;
  m_bGeometryStarted = false;
  // m_bEditDirtyFlag = false;

  //Now built afterwards(David)
  //m_pEditWidget = m_pView->editWidget();
  m_pPosWidget = m_pView->posWidget();

  setBackgroundMode( PaletteBase );

  setMouseTracking( TRUE );
  m_bMousePressed = false;

  m_scrollTimer = new QTimer( this );
  connect (m_scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );

  choose_visible = false;
  setFocus();
  installEventFilter( this );
  (void)new KSpreadToolTip( this );
}

KSpreadCanvas::~KSpreadCanvas()
{
    delete m_scrollTimer;
}


bool KSpreadCanvas::eventFilter( QObject *o, QEvent *e )
{
    if ( !o || !e )
        return TRUE;
    switch ( e->type() )
    {
    case QEvent::AccelOverride:
    {
        QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
        if (keyev->key()==Key_Tab && !m_pEditor)
        {
            keyPressEvent ( keyev );
            return true;
        }
    }
    default:
      break;
    }
    return false;
}

bool KSpreadCanvas::focusNextPrevChild( bool )
{
    return TRUE; // Don't allow to go out of the canvas widget by pressing "Tab"
}

QPoint KSpreadCanvas::marker() const
{
    return activeTable()->marker();
}

int KSpreadCanvas::markerColumn() const
{
    return activeTable()->marker().x();
}

int KSpreadCanvas::markerRow() const
{
    return activeTable()->marker().y();
}

void KSpreadCanvas::startChoose()
{
  if ( m_bChoose )
    return;

  // Clear a selection if there is any
  m_i_chooseMarkerColumn = markerColumn();
  m_i_chooseMarkerRow = markerRow();
  activeTable()->setChooseRect( QRect( 0, 0, 0, 0 ) );

  // It is important to enable this AFTER we set the rect!
  m_bChoose = TRUE;
  m_chooseStartTable = activeTable();
}

void KSpreadCanvas::startChoose( const QRect& rect )
{
  activeTable()->setChooseRect( rect );

  m_i_chooseMarkerColumn = rect.right();
  m_i_chooseMarkerRow = rect.bottom();

  // It is important to enable this AFTER we set the rect!
  m_bChoose = TRUE;
  m_chooseStartTable = activeTable();
}

void KSpreadCanvas::endChoose()
{
  if ( !m_bChoose )
    return;

  activeTable()->setChooseRect( QRect( 0, 0, 0, 0 ) );
  //m_pView->setActiveTable( m_chooseStartTable );
  KSpreadTable *table=m_pView->doc()->map()->findTable(m_chooseStartTable->tableName());
  if(table)
        table->setActiveTable();

  //kdDebug(36001) << "endChoose len=0" << endl;
  length_namecell = 0;
  m_bChoose = FALSE;
  m_chooseStartTable = 0;
}

KSpreadHBorder* KSpreadCanvas::hBorderWidget()
{
  return m_pView->hBorderWidget();
}

KSpreadVBorder* KSpreadCanvas::vBorderWidget()
{
  return m_pView->vBorderWidget();
}

QScrollBar* KSpreadCanvas::horzScrollBar()
{
  return m_pView->horzScrollBar();
}

QScrollBar* KSpreadCanvas::vertScrollBar()
{
  return m_pView->vertScrollBar();
}

KSpreadTable* KSpreadCanvas::findTable( const QString& _name )
{
  return m_pDoc->map()->findTable( _name );
}

const KSpreadTable* KSpreadCanvas::activeTable() const
{
  return m_pView->activeTable();
}

KSpreadTable* KSpreadCanvas::activeTable()
{
  return m_pView->activeTable();
}

void KSpreadCanvas::gotoLocation( const KSpreadRange & _range )
{
        if ( !_range.isValid() )
        {
                KMessageBox::error( this, i18n( "Invalid cell reference" ) );
                return;
        }
        KSpreadTable * table = activeTable();
        if ( _range.isTableKnown() )
                table = _range.table;
        if ( !table )
        {
            KMessageBox::error( this, i18n("Unknown table name %1" ).arg( _range.tableName ) );
            return;
        }

        gotoLocation( _range.range.left(), _range.range.top(), table, false );
        gotoLocation( _range.range.right(), _range.range.bottom(), table, true );
}


void KSpreadCanvas::gotoLocation( const KSpreadPoint& _cell )
{
  if ( !_cell.isValid() )
  {
    KMessageBox::error( this, i18n("Invalid cell reference") );
    return;
  }

  KSpreadTable* table = activeTable();
  if ( _cell.isTableKnown() )
    table = _cell.table;
  if ( !table )
  {
    KMessageBox::error( this, i18n("Unknown table name %1").arg( _cell.tableName ) );
    return;
  }

  gotoLocation( _cell.pos.x(), _cell.pos.y(), table );
}

void KSpreadCanvas::gotoLocation( int x, int y, KSpreadTable* table, bool make_select, bool move_into_area, bool keyPress )
{
  if ( table )
    table->setActiveTable();
  else
    table = activeTable();

  QRect extraArea;
  QRect tmpArea;
  KSpreadCell* cell = table->cellAt( x, y );
  if ( cell->isObscured() && cell->isObscuringForced() )
  {
    cell = cell->obscuringCells().getFirst();
    int moveX=cell->column();
    int moveY=cell->row();
    QRect extraCell;
    extraCell.setCoords( moveX, moveY, moveX+cell->extraXCells(),
                         moveY+cell->extraYCells() );
    if( (x - markerColumn()) != 0 &&
        extraCell.contains(QPoint(markerColumn(),markerRow())))
    {
      extraArea.setCoords( markerColumn(), 1,
                           QMIN(KS_colMax, cell->extraXCells()+x-1), KS_rowMax );
      if(keyPress)
      {
        tmpArea.setCoords( 1, markerRow(), KS_colMax,
                           QMIN(KS_rowMax, cell->extraYCells()+markerRow()) );
        if(!extraArea.contains(table->getOldPos().x(), 1) &&
           tmpArea.contains(1, table->getOldPos().y()))
        {
          y = table->getOldPos().y();
        }
        else if( extraArea.contains(table->getOldPos().x(), 1) &&
                 table->getOldPos().y()==(cell->extraYCells()+y+1))
        {
          y = table->getOldPos().y()-1;
        }
        x = cell->extraXCells()+x;
      }
    }
    else if( (y - markerRow()) != 0 &&
             extraCell.contains( QPoint(markerColumn(), markerRow()) ) )
    {
      extraArea.setCoords( 1, markerRow(), KS_colMax,
                           QMIN(KS_rowMax, cell->extraYCells()+y-1) );

      tmpArea.setCoords( markerColumn(), 1,
                         QMIN(KS_colMax, cell->extraXCells()+markerColumn()),
                         KS_rowMax );
      if(keyPress)
      {
        if( !extraArea.contains(1, table->getOldPos().y()) &&
            tmpArea.contains(table->getOldPos().x(), 1))
        {
          x=table->getOldPos().x();
        }
        else if( table->getOldPos().x() ==
                 (cell->extraXCells() + markerColumn() + 1))
        {
          x=table->getOldPos().x()-1;
        }
      }
      y=cell->extraYCells()+y;
    }
    else
    {
      y = moveY;
      x = moveX;
    }
  }
  else
  {
    cell = table->cellAt( table->marker().x(), table->marker().y() );
    if ( cell->isForceExtraCells() )
    {
      if(keyPress && (x-markerColumn())!=0)
      {
        extraArea.setCoords( markerColumn(), 1,
                             QMIN(KS_colMax, cell->extraXCells() +
                                  markerColumn()), KS_rowMax );
        tmpArea.setCoords( 1, markerRow(), KS_colMax, QMIN(KS_rowMax, cell->extraYCells()+markerRow()) );
        if( !extraArea.contains(QPoint(table->getOldPos().x(),1))&& tmpArea.contains(1,table->getOldPos().y()))
          y=table->getOldPos().y();
        else if( extraArea.contains(table->getOldPos().x(),1)&& table->getOldPos().y()==(cell->extraYCells()+markerRow()+1))
          y=table->getOldPos().y()-1;
      }
      else if(keyPress && (y-markerRow())!=0 )
      {
        tmpArea.setCoords( markerColumn(), 1, QMIN(KS_colMax, cell->extraXCells()+markerColumn()), KS_rowMax );
        extraArea.setCoords( 1, markerRow(), KS_colMax, QMIN(KS_rowMax, cell->extraYCells()+markerRow()) );
        if( !extraArea.contains(QPoint(1,table->getOldPos().y()))&&tmpArea.contains(table->getOldPos().x(),1))
          x=table->getOldPos().x();
        else if( /*extraArea.contains(QPoint(1,table->getOldPos().y()))&&*/ table->getOldPos().x()==(cell->extraXCells()+markerColumn()+1))
          x=table->getOldPos().x()-1;
      }
    }
  }
  cell= table->cellAt( x, y, true /* update scrollbar when necessary */ );
  if( cell->isObscured() && cell->isObscuringForced() )
  {
    x = cell->obscuringCells().getFirst()->column();
    y = cell->obscuringCells().getFirst()->row();
  }

  int xpos = table->columnPos( x, this );
  int ypos = table->rowPos( y, this );

  //kdDebug(36001) << "KSpreadCanvas::gotoLocation : zoom=" << zoom() << endl;
  int minX = (int) (100 * zoom()); // less than that, we scroll
  int minY = (int) (50 * zoom());
  int maxX = (int) (width() - 100 * zoom()); // more than that, we scroll
  int maxY = (int) (height() - 50 * zoom());
  //kdDebug(36001) << "KSpreadCanvas::gotoLocation : height=" << height() << endl;
  //kdDebug(36001) << "KSpreadCanvas::gotoLocation : width=" << width() << endl;

  // do we need to scroll left
  if ( xpos < minX )
    horzScrollBar()->setValue( xOffset() + xpos - minX );

  //do we need to scroll right
  else if ( xpos > maxX ) {
    int horzScrollBarValue;
    unsigned long horzScrollBarValueMax = table->sizeMaxX() - width();
    horzScrollBarValue = xOffset() + xpos - maxX;

    //We don't want to display any area > KS_colMax widths
    if ( (unsigned long)horzScrollBarValue > horzScrollBarValueMax )
      horzScrollBarValue = horzScrollBarValueMax;

    horzScrollBar()->setValue( horzScrollBarValue );
  }

  // do we need to scroll up
  if ( ypos < minY )
    vertScrollBar()->setValue( yOffset() + ypos - minY );

  // do we need to scroll down
  else if ( ypos > maxY ) {
    int vertScrollBarValue;
    unsigned long vertScrollBarValueMax = table->sizeMaxY() - height();
    vertScrollBarValue = yOffset() + ypos - maxY;

    //We don't want to display any area > KS_rowMax heights
    if ( (unsigned long)vertScrollBarValue > vertScrollBarValueMax )
      vertScrollBarValue = vertScrollBarValueMax;

    vertScrollBar()->setValue( vertScrollBarValue );
  }

  QRect selection = activeTable()->selection();

  if ( !make_select )
  {
    if ( !move_into_area)
      activeTable()->setMarker( QPoint( x, y ) );
    else
      activeTable()->setSelection(selection,QPoint( x, y ),this);
  }
  else
  {
    if ( markerColumn() == selection.left() )
      selection.setLeft( x );
    else
      selection.setRight( x );

    if ( markerRow() == selection.top() )
      selection.setTop( y );
    else
      selection.setBottom( y );
    selection = selection.normalize();

    // m_iMarkerColumn = x;
    // m_iMarkerRow = y;
    activeTable()->setSelection( selection, QPoint( x, y ) );
  }

  // Perhaps the user is entering a value in the cell.
  // In this case we may not touch the EditWidget
  if ( !m_pEditor )
    m_pView->updateEditWidget();
  updatePosWidget();
}

void KSpreadCanvas::chooseGotoLocation( int x, int y, KSpreadTable* table, bool make_select )
{
  if ( table )
    table->setActiveTable( );
  else
    table = activeTable();

  KSpreadCell* cell = table->cellAt( x, y, true /* update scrollbar when necessary */ );
  if ( cell->isObscured() && cell->isObscuringForced() )
  {
    cell = cell->obscuringCells().getFirst();
    int moveX=cell->column();
    int moveY=cell->row();
    cell = table->cellAt( moveX, moveY, true /* update scrollbar when necessary */ );
    QRect extraCell;
    extraCell.setCoords( moveX, moveY, moveX+cell->extraXCells(), moveY+cell->extraYCells() );
    if( (x-chooseMarkerColumn())!=0 && extraCell.contains(chooseMarker()))
        x=cell->extraXCells()+x;
    else if((y-chooseMarkerRow())!=0 && extraCell.contains(chooseMarker()))
        y=cell->extraYCells()+y;
    else
        {
        y = moveY;
        x = moveX;
        }
  }

  int xpos = table->columnPos( x, this );
  int ypos = table->rowPos( y, this );

  int minX = (int) (100 * zoom()); // less than that, we scroll
  int minY = (int) (50 * zoom());
  int maxX = (int) (width() - 100 * zoom()); // more than that, we scroll
  int maxY = (int) (height() - 50 * zoom());

  // do we need to scroll left
  if ( xpos < minX )
    horzScrollBar()->setValue( xOffset() + xpos - minX );

  //do we need to scroll right
  else if ( xpos > maxX ) {
    int horzScrollBarValue;
    int horzScrollBarValueMax = table->sizeMaxX() - width();

    horzScrollBarValue = xOffset() + xpos - maxX;

    //We don't want to display any area > KS_colMax value
    if ( horzScrollBarValue > horzScrollBarValueMax )
      horzScrollBarValue = horzScrollBarValueMax;

    horzScrollBar()->setValue( horzScrollBarValue );
  }

  // do we need to scroll up
  if ( ypos < minY )
    vertScrollBar()->setValue( yOffset() + ypos - minY );

  // do we need to scroll down
  else if ( ypos > maxY ) {
    int vertScrollBarValue;
    unsigned long vertScrollBarValueMax = table->sizeMaxY() - height();
    vertScrollBarValue = yOffset() + ypos - maxY;

    //We don't want to display any area > KS_rowMax value
    if ( (unsigned long)vertScrollBarValue > vertScrollBarValueMax )
      vertScrollBarValue = vertScrollBarValueMax;

    vertScrollBar()->setValue( vertScrollBarValue );
  }

  if ( !make_select )
    setChooseMarker( QPoint( x, y ) );
  else
  {
    QRect selection = activeTable()->chooseRect();
    if ( chooseMarkerColumn() == selection.left() )
      selection.setLeft( x );
    else
      selection.setRight( x );

    if ( chooseMarkerRow() == selection.top() )
      selection.setTop( y );
    else
      selection.setBottom( y );
    selection = selection.normalize();
    m_i_chooseMarkerColumn = x;
    m_i_chooseMarkerRow = y;
    activeTable()->setChooseRect( selection );
  }
}

void KSpreadCanvas::highlight( const QString &/*text*/, int /*matchingIndex*/, int /*matchedLength*/, const QRect &cellRect )
{
    // Which cell was this again?
    //KSpreadCell *cell = cellAt( cellRect.left(), cellRect.top() );

    // ...now I remember, update it!
    // TBD: highlight it!
    gotoLocation( cellRect.left(), cellRect.top(), activeTable() );
}

// Used by replace() logic to modify a cell.
void KSpreadCanvas::replace( const QString &newText, int /*index*/, int /*replacedLength*/, int /*searchWordLenght*/,const QRect &cellRect )
{
    // Which cell was this again?
    KSpreadCell *cell = activeTable()->cellAt( cellRect.left(), cellRect.top() );

    // ...now I remember, update it!
    cell->setDisplayDirtyFlag();
    cell->setCellText( newText );
    cell->clearDisplayDirtyFlag();
    activeTable()->updateCell( cell, cellRect.left(), cellRect.top() );
}

void KSpreadCanvas::slotScrollHorz( int _value )
{
  if ( activeTable() == 0L )
    return;
  if ( _value < 0 ) {
    _value = 0;
    kdDebug (36001) << "KSpreadCanvas::slotScrollHorz: value out of range (_value: " << _value << ")" << endl;
  }

  int xpos = activeTable()->columnPos( QMIN( KS_colMax, m_pView->activeTable()->maxColumn()+10 ), this );
  if(_value>(xpos + m_iXOffset))
      _value=xpos + m_iXOffset;

  activeTable()->enableScrollBarUpdates( false );

  // Relative movement
  int dx = m_iXOffset - _value;
  // New absolute position
  m_iXOffset = _value;

  scroll( dx, 0 );

  hBorderWidget()->scroll( dx, 0 );

  activeTable()->enableScrollBarUpdates( true );

}

void KSpreadCanvas::slotScrollVert( int _value )
{
  if ( activeTable() == 0L )
    return;

  if ( _value < 0 ) {
    _value = 0;
    kdDebug (36001) << "KSpreadCanvas::slotScrollVert: value out of range (_value: " << _value << ")" << endl;
  }

  int ypos = activeTable()->rowPos( QMIN( KS_rowMax, m_pView->activeTable()->maxRow()+10 ) , this );
  if(_value>(ypos + m_iYOffset))
      _value=ypos + m_iYOffset;

  activeTable()->enableScrollBarUpdates( false );

  // Relative movement
  int dy = m_iYOffset - _value;
  // New absolute position
  m_iYOffset = _value;
  scroll( 0, dy );
  vBorderWidget()->scroll( 0, dy );

  activeTable()->enableScrollBarUpdates( true );
}

void KSpreadCanvas::slotMaxColumn( int _max_column )
{
  int xpos = activeTable()->columnPos( QMIN( KS_colMax, _max_column + 10 ), this );

  //Don't go beyond the maximum column range (KS_colMax)
  const int _sizeMaxX = activeTable()->sizeMaxX();
  if ( ( xOffset() + xpos ) > ( _sizeMaxX - width() ) )
    xpos = _sizeMaxX - width() - xOffset();

  horzScrollBar()->setRange( 0, xpos + xOffset() );
}

void KSpreadCanvas::slotMaxRow( int _max_row )
{
  int ypos = activeTable()->rowPos( QMIN( KS_rowMax, _max_row + 10 ), this );

  //Don't go beyond the maximum row range (KS_rowMax)
  unsigned long _sizeMaxY = activeTable()->sizeMaxY();
  if ( (unsigned long)( yOffset() + ypos ) > ( _sizeMaxY - height() ) )
    ypos = _sizeMaxY - height() - yOffset();

  vertScrollBar()->setRange( 0, ypos + yOffset() );
}

void KSpreadCanvas::mouseMoveEvent( QMouseEvent * _ev )
{
  // Dont allow modifications if document is readonly.
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  // Special handling for choose mode.
  if( m_bChoose )
  {
    chooseMouseMoveEvent( _ev );
    return;
  }

  // Working on this table ?
  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  int xpos;
  int ypos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  if( col > KS_colMax || row > KS_rowMax ) {
    kdDebug(36001) << "KSpreadCanvas::mouseMoveEvent: col or row is out of range: col: " << col << " row: " << row << endl;
    return;
  }

  QRect selectionHandle = table->getSelectionHandleArea(this);

  // Test whether the mouse is over some anchor
  {
    KSpreadCell *cell = table->visibleCellAt( col, row );
    QString anchor = cell->testAnchor( _ev->pos().x() - xpos,
                                       _ev->pos().y() - ypos );
    if ( !anchor.isEmpty() && anchor != m_strAnchor )
      setCursor( KCursor::handCursor() );
    m_strAnchor = anchor;
  }

  //
  // Now set the cursor correctly.
  //
  if ( selectionHandle.contains( _ev->pos() ) )
    setCursor( sizeFDiagCursor );
  else if ( !m_strAnchor.isEmpty() )
    setCursor( KCursor::handCursor() );
  else
    setCursor( arrowCursor );

  // No marking, selecting etc. in progess? Then quit here.
  if ( m_eMouseAction == NoAction )
    return;

  // Set the new extent of the selection
  extendCurrentSelection(QPoint(col, row));

  // Scroll the table if necessary
  if ( _ev->pos().x() < 0 )
    horzScrollBar()->setValue( xOffset() + xpos );
  else if ( _ev->pos().x() > width() )
  {
    if ( col < KS_colMax )
    {
      ColumnLayout *cl = table->columnLayout( col + 1 );
      xpos = table->columnPos( col + 1, this );
      horzScrollBar()->setValue( xOffset() + ( xpos + cl->width( this ) - width() ) );
    }
  }
  if ( _ev->pos().y() < 0 )
    vertScrollBar()->setValue( yOffset() + ypos );
  else if ( _ev->pos().y() > height() )
  {
    if ( row < KS_rowMax )
    {
      RowLayout *rl = table->rowLayout( row + 1 );
      ypos = table->rowPos( row + 1, this );
      vertScrollBar()->setValue( yOffset() + ( ypos + rl->height( this ) - height() ) );
    }
  }

  // Show where we are now.
  updatePosWidget();
}

void KSpreadCanvas::mouseReleaseEvent( QMouseEvent* _ev )
{
  if ( m_scrollTimer->isActive() )
    m_scrollTimer->stop();

  m_bMousePressed = false;

  if( m_bChoose )
  {
    chooseMouseReleaseEvent( _ev );
    return;
  }

  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  QRect selection( table->selection() );

  if (table->singleCellSelection())
  {
    KSpreadCell* cell = table->cellAt(table->marker());
    cell->clicked(this);
  }

  // The user started the drag in the lower right corner of the marker ?
  if ( m_eMouseAction == ResizeCell )
  {
    int x = m_selectionAnchor.x();
    int y = m_selectionAnchor.y();
    if( x > selection.left())
        x = selection.left();
    if( y > selection.top() )
        y =selection.top();
    KSpreadCell *cell = table->nonDefaultCell( x, y );
    if ( !m_pView->doc()->undoBuffer()->isLocked() )
    {
        KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( m_pView->doc(), table, x, y,cell->extraXCells() ,cell->extraYCells());
        m_pView->doc()->undoBuffer()->appendUndo( undo );
    }
    cell->forceExtraCells( x,y,
                           abs(selection.right() - selection.left()),
                           abs(selection.bottom() - selection.top()) );

    m_pView->updateEditWidget();
    if(table->getAutoCalc()) table->recalc();
  }
  else if ( m_eMouseAction == AutoFill )
  {
    QRect dest = table->selection();
    table->autofill( m_rctAutoFillSrc, dest );

    m_pView->updateEditWidget();
  }
  // The user started the drag in the middle of a cell ?
  else if ( m_eMouseAction == Mark )
  {
    m_pView->updateEditWidget();
  }

  m_eMouseAction = NoAction;

}

void KSpreadCanvas::processClickSelectionHandle(QMouseEvent *event)
{
  KSpreadTable *table = activeTable();
  QRect selection = table->selection();

  // Auto fill ? That is done using the left mouse button.
  if ( event->button() == LeftButton )
  {
    m_eMouseAction = AutoFill;
    m_rctAutoFillSrc = selection;
    m_selectionAnchor.setX(QMIN(markerColumn(),selection.left()));
    m_selectionAnchor.setY(QMIN(markerRow(),selection.top()));
  }
  // Resize a cell (done with the right mouse button) ?
  // But for that to work there must not be a selection.
  else if ( event->button() == MidButton && table->singleCellSelection())
  {
    m_eMouseAction = ResizeCell;
    m_selectionAnchor.setX(markerColumn());
    m_selectionAnchor.setY(markerRow());
  }

  return;
}

void KSpreadCanvas::extendCurrentSelection(QPoint cell)
{
  KSpreadTable* table = activeTable();

  /* the selection simply becomes a box with the anchor and given cell as opposite corners*/
  int left, top, right, bottom;
  left = QMIN(m_selectionAnchor.x(), cell.x());
  top = QMIN(m_selectionAnchor.y(), cell.y());
  right = QMAX(m_selectionAnchor.x(), cell.x());
  bottom = QMAX(m_selectionAnchor.y(), cell.y());
  QRect newSelection(QPoint(left, top), QPoint(right, bottom));
  newSelection.normalize();

  /* account for merged cell problems */
  newSelection = table->selectionCellMerged(newSelection);

  /* keep the marker at the anchor */
  table->setSelection(newSelection, cell, this);

}

void KSpreadCanvas::processLeftClickAnchor()
{
  bool isLink = (m_strAnchor.find("http://") == 0 || m_strAnchor.find("mailto:") == 0
                 || m_strAnchor.find("ftp://") == 0 || m_strAnchor.find("file:") == 0 );
  bool isLocalLink = (m_strAnchor.find("file:") == 0);
  if( isLink )
  {
    QString question = i18n("Do you want to open this link to '%1'?\n").arg(m_strAnchor);
    if( isLocalLink )
    {
      question += i18n("Note that opening a link to a local file may "
                       "compromise your system's security!");
    }

    // this will also start local programs, so adding a "don't warn again"
    // checkbox will probably be too dangerous
    int choice = KMessageBox::warningYesNo(this, question, i18n("Open Link?"));
    if( choice == KMessageBox::Yes )
    {
      (void) new KRun( m_strAnchor );
    }
  }
  else
  {
    gotoLocation( KSpreadPoint( m_strAnchor, m_pDoc->map() ) );
  }
}

void KSpreadCanvas::mousePressEvent( QMouseEvent * _ev )
{
  if ( _ev->button() == LeftButton )
    m_bMousePressed = true;

  // If in choose mode, we handle the mouse differently.
  if( m_bChoose )
  {
    chooseMousePressEvent( _ev );
    return;
  }

  KSpreadTable *table = activeTable();

  if ( !table )
    return;

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pEditor )
  {
    deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  // Remember current values.
  QRect selection( table->selection() );

  // Did we click in the lower right corner of the marker/marked-area ?
  if (table->getSelectionHandleArea(this).contains(_ev->pos()))
  {
    processClickSelectionHandle(_ev);
    return;
  }

  // In which cell did the user click ?
  int xpos, ypos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  //you cannot move marker when col > KS_colMax or row > KS_rowMax
  if( col > KS_colMax || row > KS_rowMax){
    kdDebug(36001) << "KSpreadCanvas::mousePressEvent: col or row is out of range: col: " << col << " row: " << row << endl;
    return;
  }

  // Unselect a selection ?
  if ( _ev->button() == LeftButton || !selection.contains( QPoint( col, row ) ) )
    table->unselect();

  // Extending an existing selection with the shift button ?
  if ( m_pView->koDocument()->isReadWrite() && selection.right() != KS_colMax &&
       selection.bottom() != KS_rowMax && _ev->state() & ShiftButton )
  {
    extendCurrentSelection(QPoint(col, row));

    return;
  }

  KSpreadCell *cell = table->cellAt( col, row );

  // Go to the upper left corner of the obscuring object if cells are merged
  if (cell->isObscuringForced())
  {
    cell = cell->obscuringCells().getFirst();
    col = cell->column();
    row = cell->row();
  }

  // Start a marking action ?
  if ( !m_strAnchor.isEmpty() && _ev->button() == LeftButton )
  {
    processLeftClickAnchor();
  }
  else if ( _ev->button() == LeftButton )
  {
    m_eMouseAction = Mark;
    selection.setCoords( col, row,
                         col + cell->extraXCells(),
                         row + cell->extraYCells() );
    table->setSelection( selection, this );
    m_selectionAnchor.setX(col);
    m_selectionAnchor.setY(row);
  }
  else if ( _ev->button() == RightButton )
  {
    // No selection or the mouse press was outside of an existing selection ?
    if ( !selection.contains( QPoint(col, row )) )
      table->setMarker( QPoint( col, row ) );
  }

  // Paste operation with the middle button ?
  if( _ev->button() == MidButton )
  {
    table->setMarker( QPoint( col, row ) );
    table->paste( QPoint( markerColumn(), markerRow() ) );
    table->cellAt( markerColumn(), markerRow() )->update();
  }

  // Update the edit box
  m_pView->updateEditWidgetOnPress();

  updatePosWidget();

  // Context menu ?
  if ( _ev->button() == RightButton )
  {
    // TODO: Handle anchor
    QPoint p = mapToGlobal( _ev->pos() );
    m_pView->openPopupMenu( p );
  }
}

void KSpreadCanvas::chooseMouseMoveEvent( QMouseEvent * _ev )
{
  if ( !m_bMousePressed )
    return;

  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  int ypos, xpos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  /*if ( col < m_iMouseStartColumn )
    col = m_iMouseStartColumn;
  if ( row < m_iMouseStartRow )
    row = m_iMouseStartRow;*/
  if( col > KS_colMax || row > KS_rowMax ) {
    kdDebug(36001) << "KSpreadCanvas::chooseMouseMoveEvent: col or row is out of range: col: " << col << " row: " << row << endl;
    return;
  }

  // Noting changed ?
  QRect selection( table->chooseRect() );
  /*if ( row == selection.bottom() && col == selection.right() )
    return;
  */
  // Set the new lower right corner of the selection
  /*selection.setRight( col );
  selection.setBottom( row );*/
  if ( col <= m_iMouseStartColumn )
     {
     selection.setLeft( col );
     selection.setRight( m_iMouseStartColumn );
     }
  else
     selection.setRight( col );
  if ( row <= m_iMouseStartRow )
     {
     selection.setTop( row );
     selection.setBottom( m_iMouseStartRow);
     }
  else
     selection.setBottom( row );

  table->setChooseRect( selection );

  // Scroll the table if neccessary
  if ( _ev->pos().x() < 0 )
    horzScrollBar()->setValue( xOffset() + xpos );
  else if ( _ev->pos().x() > width() )
  {
    if ( col < KS_colMax )
    {
      ColumnLayout *cl = table->columnLayout( col + 1 );
      xpos = table->columnPos( col + 1, this );
      horzScrollBar()->setValue( xOffset() + ( xpos + cl->width( this ) - width() ) );
    }
  }

  if ( _ev->pos().y() < 0 )
    vertScrollBar()->setValue( yOffset() + ypos );
  else if ( _ev->pos().y() > height() )
  {
    if ( row < KS_rowMax )
    {
      RowLayout *rl = table->rowLayout( row + 1 );
      ypos = table->rowPos( row + 1, this );
      vertScrollBar()->setValue( yOffset() + ( ypos + rl->height( this ) - height() ) );
    }
  }
}

void KSpreadCanvas::chooseMouseReleaseEvent( QMouseEvent* )
{
    // gets done in mouseReleaseEvent
    //  m_bMousePressed = FALSE;
}

void KSpreadCanvas::chooseMousePressEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = activeTable();
  if ( !table )
    return;
  QRect selection = table->chooseRect();

  int ypos, xpos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  if( col > KS_colMax || row > KS_rowMax ) {
    kdDebug(36001) << "KSpreadCanvas::chooseMousePressEvent: col or row is out of range: col: " << col << " row: " << row << endl;
    return;
  }

  if ( ( (!table->isColumnSelected()) && (!table->isRowSelected())) &&
      ( _ev->state() & ShiftButton ) )
  {
    if ( col != m_iMouseStartColumn || row != m_iMouseStartRow )
    {
      if ( selection.left() !=0 && selection.right() !=0
          && selection.top() !=0 && selection.bottom() !=0 )
      {
        if ( col < m_iMouseStartColumn )
          col = m_iMouseStartColumn;
        if ( row < m_iMouseStartRow )
          row = m_iMouseStartRow;

        if ( row == selection.bottom() && col == selection.right() )
          return;
      }
      selection.setLeft(m_iMouseStartColumn);
      selection.setRight( col );
      selection.setTop(m_iMouseStartRow);
      selection.setBottom( row );
      table->setChooseRect( selection );
      return;
    }
  }

  setChooseMarkerColumn( col );
  setChooseMarkerRow( row );
  KSpreadCell *cell = table->cellAt( chooseMarkerColumn(), chooseMarkerRow() );

  // Go to the upper left corner of the obscuring object if it is a merged cell
  if ( cell->isObscuringForced() )
  {
    cell = cell->obscuringCells().getFirst();
    setChooseMarkerRow( cell->row() );
    setChooseMarkerColumn( cell->column() );
  }

  if ( cell->isForceExtraCells() )
  {
      selection.setCoords( chooseMarkerColumn(), chooseMarkerRow(),
                           chooseMarkerColumn() + cell->extraXCells(),
                           chooseMarkerRow() + cell->extraYCells() );
  }
  else
  {
      selection.setCoords( chooseMarkerColumn(), chooseMarkerRow(),
                           chooseMarkerColumn(), chooseMarkerRow() );
  }

  table->setChooseRect( selection );
  m_iMouseStartColumn = chooseMarkerColumn();
  m_iMouseStartRow = chooseMarkerRow();
}

void KSpreadCanvas::mouseDoubleClickEvent( QMouseEvent*  )
{
  if ( m_pView->koDocument()->isReadWrite() )
    createEditor();
}

void KSpreadCanvas::wheelEvent( QWheelEvent* _ev )
{
  if ( vertScrollBar() )
    QApplication::sendEvent( vertScrollBar(), _ev );
}

void KSpreadCanvas::paintEvent( QPaintEvent* _ev )
{
  if ( m_pDoc->isLoading() )
    return;

  if ( !activeTable() )
    return;

  // printf("PAINT EVENT %i %i %i %i\n", _ev->rect().x(), _ev->rect().y(), _ev->rect().width(), _ev->rect().height() );

  QRect rect( _ev->rect() );
  if ( rect.left() < 0 )
      rect.rLeft() = 0;
  if ( rect.right() > width() )
      rect.rRight() = width();
  if ( rect.top() < 0 )
      rect.rTop() = 0;
  if ( rect.bottom() > height() )
      rect.rBottom() = height();

  // printf("PAINT EVENT %i %i %i %i\n", rect.x(), rect.y(), rect.width(), rect.height() );

  QPainter painter;
  painter.begin( this );

  QWMatrix m = m_pView->matrix();
  painter.setWorldMatrix( m );
  m = m.invert();
  QPoint tl = m.map( rect.topLeft() );
  QPoint br = m.map( rect.bottomRight() );

  //kdDebug(36001) << "Mapped topleft to " << tl.x() << ":" << tl.y() << endl;

  painter.save();

  // Clip away children
  QRegion rgn = painter.clipRegion();
  if ( rgn.isEmpty() )
    rgn = QRegion( rect );
  QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
  for( ; it.current(); ++it )
  {
    if ( ((KSpreadChild*)it.current())->table() == activeTable() &&
         !m_pView->hasDocumentInWindow( it.current()->document() ) )
    {
        rgn -= it.current()->region( painter.worldMatrix() );
    }
  }
  painter.setClipRegion( rgn );

  // Draw content
  m_pDoc->paintContent( painter, QRect( tl, br ), FALSE, activeTable() );

  painter.restore();

  // Draw children
  // QPtrListIterator<PartChild> it( m_pDoc->children() );
  it.toFirst();
  for( ; it.current(); ++it )
  {
      if ( ((KSpreadChild*)it.current())->table() == activeTable() &&
           !m_pView->hasDocumentInWindow( it.current()->document() ) )
    {
      // #### todo: paint only if child is visible inside rect
      painter.save();
      m_pDoc->paintChild( it.current(), painter, m_pView );
      painter.restore();
    }
  }

  painter.end();

  if( choose_visible )
    drawChooseMarker( );
}

void KSpreadCanvas::focusInEvent( QFocusEvent* )
{
  if ( !m_pEditor )
    return;

  //kdDebug(36001) << "m_bChoose : " << ( m_bChoose ? "true" : "false" ) << endl;
  // If we are in editing mode, we redirect the
  // focus to the CellEditor or EditWidget
  // And we know which, using lastEditorWithFocus.
  // This screws up <Tab> though (David)
  if ( lastEditorWithFocus() == EditWidget )
  {
    m_pView->editWidget()->setFocus();
    //kdDebug(36001) << "Focus to EditWidget" << endl;
    return;
  }

  //kdDebug(36001) << "Redirecting focus to editor" << endl;
  m_pEditor->setFocus();
}

void KSpreadCanvas::focusOutEvent( QFocusEvent* )
{
    if (m_scrollTimer->isActive())
        m_scrollTimer->stop();
    m_bMousePressed = false;
}

void KSpreadCanvas::resizeEvent( QResizeEvent* _ev )
{
    // If we rise horizontally, then check if we are still within the valid area (KS_colMax)
    if ( _ev->size().width() > _ev->oldSize().width() ){
	if ( (unsigned long)( xOffset() + _ev->size().width() ) >
             activeTable()->sizeMaxX() )
        {
	    horzScrollBar()->setRange( 0, activeTable()->sizeMaxX() - _ev->size().width() );
	}
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().width() < _ev->oldSize().width() ){
	if ( (unsigned long)(horzScrollBar()->maxValue()) ==
             ( activeTable()->sizeMaxX() - _ev->oldSize().width() ) )
        {
	    horzScrollBar()->setRange( 0, activeTable()->sizeMaxX() - _ev->size().width() );
	}
    }

    // If we rise vertically, then check if we are still within the valid area (KS_rowMax)
    if ( _ev->size().height() > _ev->oldSize().height() ){
	if ( (unsigned long)( yOffset() + _ev->size().height() ) >
             activeTable()->sizeMaxY() )
        {
	    vertScrollBar()->setRange( 0, activeTable()->sizeMaxY() - _ev->size().height() );
	}
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().height() < _ev->oldSize().height() ){
	if ( (unsigned long)vertScrollBar()->maxValue() ==
             ( activeTable()->sizeMaxY() - _ev->oldSize().height() ) )
        {
	    vertScrollBar()->setRange( 0, activeTable()->sizeMaxY() - _ev->size().height() );
	}
    }
}

void KSpreadCanvas::keyPressEvent ( QKeyEvent * _ev )
{
  KSpreadTable * table = activeTable();
  QString tmp;
  if ( !table )
    return;

  if ( formatKeyPress( _ev ) )
    return;

  // Dont handle the remaining special keys.
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) &&
                      (_ev->key() != Key_Down) &&
                      (_ev->key() != Key_Up) &&
                      (_ev->key() != Key_Right) &&
                      (_ev->key() != Key_Left) &&
                      (_ev->key() != Key_Home) )
      {
	  QWidget::keyPressEvent( _ev );
	  return;
      }

  // Always accept so that events are not
  // passed to the parent.
  _ev->accept();

  // Find out about the current selection/choose.
  QRect selection;
  if ( m_bChoose )
    selection = activeTable()->chooseRect();
  else
    selection = activeTable()->selection();

  // Is this a key that involves goint to another cell ? If yes, do the
  // "end of editing" stuff here, instead of for each case - this bool is
  // also used for make_select (extending selection), so it doesn't include Return
  bool bChangingCells = ( _ev->key() == Key_Down || _ev->key() == Key_Up ||
                          _ev->key() == Key_Left || _ev->key() == Key_Right ||
                          _ev->key() == Key_Prior || _ev->key() == Key_Next );

  // End of editing a cell
  if ( ( bChangingCells || _ev->key() == Key_Return || _ev->key() == Key_Enter )
       && m_pEditor && !m_bChoose )
  {
    deleteEditor( true /*save changes*/ );
  }

  // Are we making a selection right now ? Go thru this only if no selection is made
  // or if we neither selected complete rows nor columns.
 bool make_select = m_pView->koDocument()->isReadWrite() &&
                    ((( _ev->state() & ShiftButton ) == ShiftButton ||
                      (( _ev->state() & ShiftButton ) == ShiftButton ) &&
                      ( _ev->state() & ControlButton ) == ControlButton) &&
                     ( bChangingCells || _ev->key() == Key_Home || _ev->key() == Key_End ));

    ColumnLayout *cl;
    RowLayout *rl;
    int moveHide=0;


    if( !((( _ev->state() & ShiftButton ) == ShiftButton)&&( _ev->state() & ControlButton ) == ControlButton) && (_ev->state() != Qt::ControlButton) )
	{
	    KSpread::MoveTo tmpMoveTo=m_pView->doc()->getMoveToValue();
	    //if shift Button clicked inverse move direction
	    if(_ev->state()==Qt::ShiftButton)
		{
		    switch( tmpMoveTo)
			{
			case KSpread::Bottom:
			    tmpMoveTo=KSpread::Top;
			    break;
			case KSpread::Top:
			    tmpMoveTo=KSpread::Bottom;
			    break;
			case KSpread::Left:
			    tmpMoveTo=KSpread::Right;
			    break;
			case KSpread::Right:
			    tmpMoveTo=KSpread::Left;
			}
		}

//if( _ev->state() != Qt::ControlButton ){
      switch( _ev->key() )
      {
      case Key_Return:
      case Key_Enter:
	  switch( tmpMoveTo)
	      {
	      case KSpread::Bottom :
		  {
		      if ( !m_bChoose && markerRow() == KS_rowMax )
			  return;
		      if ( m_bChoose && chooseMarkerRow() == KS_rowMax )
			  return;

		      if ( m_bChoose )
			  chooseGotoLocation( chooseMarkerColumn(), QMIN( KS_rowMax, chooseMarkerRow() + 1 ), 0, make_select );
	      else
			  {
		              QRect selection = activeTable()->selection();
			      if( selection.left() == 0 )
				  gotoLocation( markerColumn(), QMIN( KS_rowMax, markerRow() + 1 ), 0, make_select,false,true  );
			      else
				  {
				      if( markerColumn()<selection.right() && markerRow()<selection.bottom() )
					  gotoLocation( markerColumn(), QMIN( KS_rowMax, markerRow() + 1 ), 0, make_select,true ,true);
				      else if( markerRow()==selection.bottom() && markerColumn()<selection.right() )
					  gotoLocation( QMIN(KS_colMax, markerColumn()+1), QMIN(KS_rowMax, selection.top()), 0, make_select,true, true );
				      else if( markerRow()==selection.bottom() && markerColumn()==selection.right())
					  gotoLocation( selection.left(), QMIN( KS_rowMax, selection.top() ), 0, make_select,true,true );
				      else if(markerColumn()==selection.right() && markerRow()<selection.bottom())
					  gotoLocation( markerColumn(), QMIN( KS_rowMax, markerRow() + 1 ), 0, make_select,true,true );
				  }
			  }
		      return;
		  }
	      case KSpread::Top :
		  {
		      /*if ( !m_bChoose && markerRow() == 1 )
			return;*/
		      if ( m_bChoose && chooseMarkerRow() ==1 )
			  return;
		      if ( m_bChoose )
			  chooseGotoLocation( chooseMarkerColumn(), QMIN( KS_rowMax, chooseMarkerRow() + 1 ), 0, make_select );
		      else
			  {
		              QRect selection = activeTable()->selection();
			      if( selection.left() == 0 )
				  gotoLocation( markerColumn(), QMAX( 1, markerRow() - 1 ), 0, make_select,false,true  );
			      else
				  {
				      if( markerRow()==selection.top() && markerColumn()==selection.left())
					  gotoLocation( selection.right(), QMAX( 1, selection.bottom() ), 0, make_select,true,true );
				      else if(markerColumn()==selection.right() && markerRow()>selection.top())
					  gotoLocation( markerColumn(), QMAX( 1, markerRow() - 1 ), 0, make_select,true,true );
				      else if(markerColumn()<selection.right()&&markerRow()>selection.top() )
					  gotoLocation( markerColumn(), QMAX( 1, markerRow() - 1 ), 0, make_select,true ,true);
				      else if( markerRow()==selection.top() && markerColumn()<=selection.right())
					  gotoLocation( markerColumn()-1, QMAX( 1, selection.bottom() ), 0, make_select,true, true );

				  }
			  }
		      return;
		  }
	      case KSpread::Left :
		  {
		      /*if ( !m_bChoose && markerColumn() == 1 )
			return;*/
		      if ( m_bChoose && chooseMarkerColumn() == 1 )
			  return;

		      if ( m_bChoose )
			  chooseGotoLocation( QMAX(chooseMarkerColumn()-1,1),  chooseMarkerRow() , 0, make_select );
		      else
			  {
		              QRect selection = activeTable()->selection();
			      if( selection.left() == 0 )
				  gotoLocation( QMAX(markerColumn()-1,1),  markerRow() , 0, make_select,false,true  );
			      else
				  {
				      if( markerRow()==selection.top() && markerColumn()==selection.left())
					  gotoLocation( selection.right(),selection.bottom() , 0, make_select,true,true );
				      else if(markerColumn()>selection.left() && markerRow()<=selection.bottom())
					  gotoLocation( QMAX(markerColumn()-1,1),  markerRow() , 0, make_select,true,true );
				      else if(markerColumn()==selection.right()&&markerRow()==selection.bottom() && markerColumn()!=selection.left())
					  gotoLocation( QMAX(markerColumn()-1,1), markerRow(), 0, make_select,true ,true);
				      else if(markerColumn()==selection.right()&&markerRow()==selection.bottom() && markerColumn()==selection.left())
					  gotoLocation( markerColumn(),QMAX( markerRow()-1,1), 0, make_select,true ,true);
				      else if( markerColumn()==selection.left() && markerRow()<=selection.bottom())
					  gotoLocation( selection.right(),QMAX( 1, markerRow()-1 ) , 0, make_select,true, true );

				  }
			  }
		      return;
		  }
	      case KSpread::Right :
		  {
		      /*if ( !m_bChoose && markerColumn() == KS_colMax)
			return;*/
		      if ( m_bChoose && chooseMarkerColumn() == KS_colMax )
			  return;
		      if ( m_bChoose )
			  chooseGotoLocation( QMIN( KS_colMax, chooseMarkerColumn()+1 ),chooseMarkerRow() , 0, make_select );
		      else
			  {
		              QRect selection = activeTable()->selection();
			      if( selection.left() == 0 )
				  gotoLocation( QMIN( KS_colMax, markerColumn()+1 ),  markerRow() , 0, make_select,false,true );
			      else
				  {
				      if( markerRow()==selection.top() && markerColumn()==selection.left() && markerColumn()!=selection.right())
					  gotoLocation( QMIN(markerColumn()+1, KS_colMax), markerRow(), 0, make_select,true,true );
				      else if( markerRow()==selection.top() && markerColumn()==selection.left() && markerColumn()==selection.right())
					  gotoLocation( markerColumn(), QMIN(markerRow()+1, KS_rowMax), 0, make_select,true,true );
				      else if(markerColumn()<selection.right() && markerRow()<=selection.bottom())
					  gotoLocation(QMIN(markerColumn()+1, KS_colMax),  markerRow() , 0, make_select,true,true );
				      else if(markerColumn()==selection.right() && markerRow()==selection.bottom() )
					  gotoLocation( selection.left(), selection.top(), 0, make_select,true ,true);
				      else if( markerColumn()==selection.right() && markerRow()<=selection.bottom())
					  gotoLocation( selection.left(), QMIN( KS_rowMax, markerRow()+1 ), 0, make_select,true, true );
				  }
			  }
		      return;
		  }
		  return;
	      }
      case Key_Down:

	  if ( !m_bChoose && markerRow() == KS_rowMax )
	      return;
	  if ( m_bChoose && chooseMarkerRow() == KS_rowMax )
	      return;

                   if(m_bChoose)
                        moveHide=chooseMarkerRow();
                   else
                        moveHide=markerRow();
                   do
                        {
                                moveHide++;
                                rl= activeTable()->nonDefaultRowLayout( moveHide );
                        }
                   while( rl->isHide());

	  if ( m_bChoose )
	      chooseGotoLocation( chooseMarkerColumn(), QMIN( KS_rowMax, /*chooseMarkerRow() + 1*/moveHide ), 0, make_select);
	  else
	      gotoLocation( markerColumn(), QMIN( KS_rowMax, /*markerRow() + 1*/ moveHide), 0, make_select,false,true  );

	  return;

      case Key_Up:

	  if ( !m_bChoose && markerRow() == 1 )
	      return;
	  if ( m_bChoose && chooseMarkerRow() == 1 )
	      return;

                   if(m_bChoose)
                        moveHide=chooseMarkerRow();
                   else
                        moveHide=markerRow();
                   do
                        {
                                moveHide--;
                                rl= activeTable()->nonDefaultRowLayout( moveHide );
                        }
                   while( rl->isHide() && moveHide!=0);
                   if(moveHide==0)
                        return;
	  if ( m_bChoose )
              chooseGotoLocation( chooseMarkerColumn(), QMAX( 1, /*chooseMarkerRow() - 1*/moveHide ), 0, make_select );
	  else
	      gotoLocation( markerColumn(), QMAX( 1, /*markerRow() - 1*/ moveHide ), 0, make_select,false,true );

	  return;

      case Key_Right:
      case Key_Tab:
	  if ( !m_bChoose && markerColumn() >= KS_colMax )
	      return;
	  if ( m_bChoose && chooseMarkerColumn() >= KS_colMax )
	      return;
	  if(m_bChoose)
	      moveHide=chooseMarkerColumn();
	  else
	      moveHide=markerColumn() ;

	  do
	  {
	      moveHide++;
	      cl= activeTable()->nonDefaultColumnLayout( moveHide );
	  }
	  while( cl->isHide() );

	  if ( m_bChoose )
	      chooseGotoLocation( QMIN( KS_colMax, /*chooseMarkerColumn() + 1*/moveHide ), chooseMarkerRow(), 0, make_select );
	  else
	      gotoLocation( QMIN( KS_colMax, /*markerColumn() + 1*/moveHide ), markerRow(), 0, make_select,false,true );

	  return;

      case Key_Left:

	  if ( !m_bChoose && markerColumn() == 1 )
	      return;
	  if ( m_bChoose && chooseMarkerColumn() == 1 )
	      return;
                  if(m_bChoose)
                        moveHide=chooseMarkerColumn();
                   else
                        moveHide=markerColumn();
                   do
                        {
                                moveHide--;
                                cl= activeTable()->nonDefaultColumnLayout( moveHide );
                        }
                   while( cl->isHide() && moveHide!=0);
                   // if column==0 return;
                   if(moveHide==0)
                        return;
	  if ( m_bChoose )
	      chooseGotoLocation( QMAX( 1, /*chooseMarkerColumn() - 1*/moveHide ), chooseMarkerRow(), 0, make_select );
	  else
	      gotoLocation( QMAX( 1, /*markerColumn() - 1*/ moveHide), markerRow(), 0, make_select,false,true );

	  return;

      case Key_Escape:

	  if ( m_pEditor )
	      deleteEditor( false );

	  _ev->accept(); // ?
	  return;

      case Key_Home:

	  // We are in edit mode -> go beginning of line
	  if ( m_pEditor )
	      {
		  // (David) Do this for text editor only, not formula editor...
		  // Don't know how to avoid this hack (member var for editor type ?)
		  if ( m_pEditor->inherits("KSpreadTextEditor") )
		      QApplication::sendEvent( m_pEditWidget, _ev );
		  // What to do for a formula editor ?
	      }
	  else
	      {
		  if ( !m_bChoose && markerColumn() == 1 )
		      return;
		  if ( m_bChoose && chooseMarkerColumn() == 1 )
		      return;
                  KSpreadCell * cell = table->getFirstCellRow(markerRow());
                  int col = ( cell ? cell->column() : 1 );
                  if (col > 1)
                  {
                    cell = table->cellAt(col, markerRow());
                    while ( cell && cell->isEmpty() && col > 1 )
                    {
                      col = cell->column();
                      cell = table->getNextCellLeft( col, markerRow() );
                    }
                  }
                  if ( col == markerColumn() )
                    col = 1;
		  if ( m_bChoose )
		      chooseGotoLocation( col, markerRow(), 0, make_select );
		  else
		      gotoLocation( col, markerRow(), 0, make_select,false,true );
	      }
	  return;

      case Key_End:

          // move to the last used cell in the row
	  // We are in edit mode -> go beginning of line
	  if ( m_pEditor )
          {
              // (David) Do this for text editor only, not formula editor...
              // Don't know how to avoid this hack (member var for editor type ?)
              if ( m_pEditor->inherits("KSpreadTextEditor") )
                  QApplication::sendEvent( m_pEditWidget, _ev );
              // TODO: What to do for a formula editor ?
          }
	  else
          {
              int maxCol = table->maxColumn();
              int row    = markerRow();
              int max    = markerColumn();
              int i;

              // we have to check every cell, cause there might
              // be unused cells in the middle
              for (i = 1; i < maxCol; ++i)
              {
                  KSpreadCell * cell = table->cellAt( i, row );

                  if (cell != table->defaultCell())
                  {
                      if (!cell->isObscured())
                          max = i;
                  }
              }

              if ( max == markerColumn() )
              {
                  gotoLocation( KS_colMax, row, 0, make_select, true, true );
                  return;
              }

              if ( m_bChoose )
                  chooseGotoLocation( max, markerRow(), 0, make_select );
              else
                  gotoLocation( max, markerRow(), 0, make_select,false,true );
          }
	  return;

      case Key_Prior:

	  if( !m_bChoose && markerRow() == 1 )
	      return;
	  if( m_bChoose && chooseMarkerRow() == 1 )
	      return;

	  if ( m_bChoose )
	      chooseGotoLocation( chooseMarkerColumn(), QMAX( 1, chooseMarkerRow() - 10 ), 0, make_select );
	  else
	      gotoLocation( markerColumn(), QMAX( 1, markerRow() - 10 ), 0, make_select,false,true );

	  return;

      case Key_Next:

	  if( !m_bChoose && markerRow() == KS_rowMax )
	      return;
	  if( m_bChoose && chooseMarkerRow() == KS_rowMax )
	      return;

	  if ( m_bChoose )
	      chooseGotoLocation( chooseMarkerColumn(), QMIN( KS_rowMax, chooseMarkerRow() + 10 ), 0, make_select );
	  else
	      gotoLocation( markerColumn(), QMIN( KS_rowMax, markerRow() + 10 ), 0, make_select,false,true );

	  return;

      case Key_Delete:

	  activeTable()->clearTextSelection( QPoint( markerColumn(), markerRow() ) );
	  m_pView->editWidget()->setText( "" );
	  return;
      case Key_F2:
	  m_pView->editWidget()->setFocus();
	  if(m_pEditor)
	      m_pView->editWidget()->setCursorPosition(m_pEditor->cursorPosition()-1);
	  m_pView->editWidget()->cursorForward(false);
	  return;
      default:

          if (m_pEditor && (_ev->key() == Key_F4))
          {
              m_pEditor->handleKeyPressEvent( _ev );
              _ev->accept();
              return;
          }

	  // No null character ...
	  if ( _ev->text().isEmpty() || !m_pView->koDocument()->isReadWrite() )
	      {
		  _ev->accept();
		  return;
	      }

	  if ( !m_pEditor && !m_bChoose )
	      {
                  // Switch to editing mode
                  createEditor( CellEditor );
                  m_pEditor->handleKeyPressEvent( _ev );
	      }
	  else if ( m_pEditor )
	      m_pEditor->handleKeyPressEvent( _ev );

	  return;

      } // control  button not pressed
  }
  else
  { //control button pressed
      if(!make_select)
	  table->unselect();
      int x, y;

      switch(_ev->key()){

      //Ctrl+Key_Up
      case Key_Up:
	  //If we are already at the end, we skip
	  if ( !m_bChoose && markerRow() <= 1 )
	      return;
	  //If we are already at the end, we skip
	  if ( m_bChoose && chooseMarkerRow() <= 1 )
	      return;

	  if ( m_bChoose )
	      //If we are in choose mode, we only go 1 up
	      chooseGotoLocation( chooseMarkerColumn(), QMAX( 1, chooseMarkerRow() - 1 ), 0, make_select );
	  else{

	      x = markerColumn();
	      y = markerRow();

	      //If we are at the end of a filled or empty block, then move one up
	      if ( activeTable()->cellAt( x, y )->isEmpty() != activeTable()->cellAt( x, y-1 )->isEmpty() )
		  y --;
	      else
		  {
		  // if this is a filled cell
		  if(!activeTable()->cellAt( x, y )->isEmpty()){
		      //Then we search as long as the previous pervios field is filled
		      while ( (y-1 >= 1) && !(activeTable()->cellAt( x, y-1 ))->isEmpty() )
		      {
			  y --;
		      }
		  }
		  else{
		      //Otherwise we search as long as the previous pervios field is empty
		      KSpreadCell * _c = activeTable()->getNextCellUp( x, y );

		      //Found an existing cell, but does it contain something?
		      while ( _c && _c->isEmpty() ) {
			  _c = activeTable()->getNextCellUp( x, _c->row() );
		      }

		      //If there is a filled one, use the next
		      if ( _c )
		          y = _c->row() + 1;
		      //Otherwise go to the first row
		      else
			  y = 1;
		  }
	      }

	      gotoLocation( x, QMAX( 1, y ), 0, make_select, true, true );
	  }

	  return;

      //Ctrl+Key_Down
      case Key_Down:

	  //If we are already at the end, we skip
	  if ( !m_bChoose && markerRow() >= KS_rowMax )
	      return;

	  //If we are already at the end, we skip
	  if ( m_bChoose && chooseMarkerRow() >= KS_rowMax )
	      return;

	  if ( m_bChoose )
	      //If we are in choose mode, we only go 1 down
	      chooseGotoLocation( chooseMarkerColumn(), QMIN( KS_rowMax, chooseMarkerRow() + 1 ), 0, make_select );
	  else{

	      x = markerColumn();
	      y = markerRow();

	      //If we are at the end of a filled or empty block, then move one down
	      if ( ( y < KS_rowMax ) && ( activeTable()->cellAt( x, y )->isEmpty() != activeTable()->cellAt( x, y+1 )->isEmpty() ) )
		  y ++;
	      else
		  {
		  // if this is a filled cell
		  if(!activeTable()->cellAt( x, y )->isEmpty()){
		      //Then we search as long as the next field is filled
		      while ( (y+1 <= KS_rowMax) && !(activeTable()->cellAt( x, y+1 ))->isEmpty() )
		      {
			  y ++;
		      }
		  }
		  else{
		      // If this already the last used field then jump to the end
		      if ( y >= activeTable()->maxRow() ) {
			  y = KS_rowMax;
		      }
		      else {
			  //Otherwise we search for the next filled field and use the last empty one
			  KSpreadCell * _c = activeTable()->getNextCellDown( x, y );

			  //Found an existing cell, but does it contain something?
			  while ( _c && _c->isEmpty() ) {
			      _c = activeTable()->getNextCellDown( x, _c->row() );
			  }

			  //If there is a filled one, use the previous
			  if ( _c )
		              y = _c->row() - 1;
			  //Otherwise go to the end
			  else
			      y = KS_rowMax;
		      }
		  }
	      }
	      gotoLocation( x, QMIN( KS_rowMax, y ), 0, make_select, true, true );
	  }

	  return;

      //Ctrl+Key_Right
      case Key_Right:

	  //If we are already at the end, we skip
          if ( !m_bChoose && markerColumn() >= KS_colMax )
	      return;
	  //If we are already at the end, we skip
	  if ( m_bChoose && chooseMarkerColumn() >= KS_colMax )
	      return;

	  if ( m_bChoose )
	      //If we are in choose mode, we only go 1 right
	      chooseGotoLocation( QMIN( KS_colMax, chooseMarkerColumn() + 1 ), chooseMarkerRow(), 0, make_select );
	  else{

	      x = markerColumn();
	      y = markerRow();

	      //If we are at the end of a filled or empty block, then move one right
	      if ( activeTable()->cellAt( x, y )->isEmpty() != activeTable()->cellAt( x+1, y )->isEmpty() )
		  x ++;
	      else
		  {
		  // if this is a filled cell
		  if(!activeTable()->cellAt( x, y )->isEmpty()){
		      //Then we search as long as the next field is filled
		      while ( (x < KS_colMax) && !(activeTable()->cellAt( x+1, y ))->isEmpty() ){
			  x ++;
		      }
		  }
		  else{
		      // If this already the last used field then jump to the end
		      if ( x >= activeTable()->maxColumn() ) {
			  x = KS_colMax;
		      }
		      else {
			  //Otherwise we search for the next filled field and use the last empty one
			  KSpreadCell * _c = activeTable()->getNextCellRight( x, y );

			  //Found an existing cell, but does it contain something?
			  while ( _c && _c->isEmpty() ) {
			      _c = activeTable()->getNextCellRight( _c->column(), y );
			  }

			  //If there is a filled one, use the previous
			  if ( _c )
		              x = _c->column() - 1;
			  //Otherwise go to the end
			  else
			      x = KS_colMax;
		      }
		  }
	      }

	      gotoLocation( QMIN( KS_colMax, x ), y, 0, make_select, true, true );
	  }

	  return;

      //Ctrl+Key_Left
      case Key_Left:
	  //If we are already at the end, we skip
	  if ( !m_bChoose && markerColumn() <= 1 )
	      return;
	  //If we are already at the end, we skip
	  if ( m_bChoose && chooseMarkerColumn() <= 1 )
	      return;

	  if ( m_bChoose )
	      //If we are in choose mode, we only go 1 left
	      chooseGotoLocation( QMAX( 1, chooseMarkerColumn() - 1 ), chooseMarkerRow(), 0, make_select );
	  else{

	      x = markerColumn();
	      y = markerRow();

	      //If we are at the end of a filled or empty block, then move one left
	      if ( activeTable()->cellAt( x, y )->isEmpty() != activeTable()->cellAt( x-1, y )->isEmpty() )
		  x --;
	      else
		  {
		  // if this is a filled cell
		  if(!activeTable()->cellAt( x, y )->isEmpty()){
		      //Then we search as long as the previous field is filled
		      while ( (x-1 >= 1) && !(activeTable()->cellAt( x-1, y ))->isEmpty() ){
			  x --;
		      }
		  }
		  else{
		      //Otherwise we search as long as the previous pervios field is empty
		      KSpreadCell * _c = activeTable()->getNextCellLeft( x, y );

		      //Found an existing cell, but does it contain something?
		      while ( _c && _c->isEmpty() ) {
			  _c = activeTable()->getNextCellLeft( _c->column(), y );
		      }

		      //If there is a filled one, use the next
		      if ( _c )
		          x = _c->column() + 1;
		      //Otherwise go to the first row
		      else
			  x = 1;
		  }
	      }

	      gotoLocation( QMAX( 1, x ), y, 0, make_select, true, true );
	  }

	  return;

      //Ctrl+Key_Home
      case Key_Home:

	  gotoLocation( 1, 1, 0, make_select, true, true );

	  return;

      }
  }

  /**
   * Tell the KSpreadView event handler and enable
   * makro recording by the way.
   */
  // _ev->accept();

  // m_pView->eventKeyPressed( _ev, m_bChoose );
}

double KSpreadCanvas::getDouble( KSpreadCell * cell )
{
  cell->setFactor( 1.0 );
  if ( cell->isDate() )
  {
    QDate date = cell->valueDate();
    QDate dummy(1900, 1, 1);
    return (dummy.daysTo( date ) + 1);
  }
  if ( cell->isTime() )
  {
    QTime time  = cell->valueTime();
    QTime dummy;
    return dummy.secsTo( time );
  }
  if ( cell->isNumeric() )
    return cell->valueDouble();

  return 0.0;
}

void KSpreadCanvas::convertToDouble( KSpreadCell * cell )
{
  if ( cell->isTime() || cell->isDate() )
    cell->setValue( getDouble( cell ) );
  cell->setFactor( 1.0 );
}

void KSpreadCanvas::convertToPercent( KSpreadCell * cell )
{
  if ( cell->isTime() || cell->isDate() )
    cell->setValue( getDouble( cell ) );

  cell->setFactor( 100.0 );
  cell->setFormatType( KSpreadCell::Percentage );
}

void KSpreadCanvas::convertToMoney( KSpreadCell * cell )
{
  if ( cell->isTime() || cell->isDate() )
    cell->setValue( getDouble( cell ) );

  cell->setFormatType( KSpreadCell::Money );
  cell->setFactor( 1.0 );
  cell->setPrecision( m_pDoc->locale()->fracDigits() );
}

void KSpreadCanvas::convertToTime( KSpreadCell * cell )
{
  if ( cell->isDefault() || cell->isEmpty() )
    return;
  if ( cell->isDate() )
    cell->setValue( getDouble( cell ) );

  cell->setFormatType( KSpreadLayout::SecondeTime );

  QTime time(0, 0, 0);
  time = time.addSecs( (int) cell->valueDouble() );
  int msec = (int) ( (cell->valueDouble() - (int) cell->valueDouble())* 1000 );
  time = time.addMSecs( msec );
  cell->setCellText( time.toString() );
}

void KSpreadCanvas::convertToDate( KSpreadCell * cell )
{
  if ( cell->isDefault() || cell->isEmpty() )
    return;
  if ( cell->isTime() )
    cell->setValue( getDouble( cell ) );

  cell->setFormatType( KSpreadLayout::ShortDate );
  cell->setFactor( 1.0 );

  QDate date(1900, 1, 1);

  date = date.addDays( (int) cell->valueDouble() - 1 );
  cell->setCellText( util_dateFormat(m_pDoc->locale(), date, KSpreadCell::ShortDate) );
}

bool KSpreadCanvas::formatKeyPress( QKeyEvent * _ev )
{
  if (!(_ev->state() & ControlButton ))
    return false;

  int key = _ev->key();
  if ( key != Key_Exclam && key != Key_At && key != Key_Ampersand
       && key != Key_Dollar && key != Key_Percent && key != Key_AsciiCircum
       && key != Key_NumberSign )
    return false;

  KSpreadCell  * cell = 0L;
  KSpreadTable * table = activeTable();
  QRect rect = table->selection();

  int right  = rect.right();
  int bottom = rect.bottom();

  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    QString dummy;
    KSpreadUndoCellLayout * undo = new KSpreadUndoCellLayout( m_pDoc, table, rect, dummy );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }

  if ( table->isRowSelected() )
  {
    for ( int r = rect.top(); r <= bottom; ++r )
    {
      cell = table->getFirstCellRow( r );
      while ( cell )
      {
        if ( cell->isObscuringForced() )
        {
          cell = table->getNextCellRight( cell->column(), r );
          continue;
        }

        QPen pen;

        switch ( _ev->key() )
        {
         case Key_Exclam:
          convertToDouble( cell );
          cell->setFormatType( KSpreadCell::Number );
          cell->setPrecision( 2 );
          break;

         case Key_Dollar:
          convertToMoney( cell );
          break;

         case Key_Percent:
          convertToPercent( cell );
          break;

         case Key_At:
          convertToTime( cell );
          break;

         case Key_NumberSign:
          convertToDate( cell );
          break;

         case Key_AsciiCircum:
          cell->setFormatType( KSpreadCell::Scientific );
          convertToDouble( cell );
          cell->setFactor( 1.0 );
          break;

         case Key_Ampersand:
          if ( r == rect.top() )
          {
            pen = QPen( m_pView->borderColor(), 1, SolidLine);
            cell->setTopBorderPen( pen );
          }
          else if ( r == rect.bottom() )
          {
            pen = QPen( m_pView->borderColor(), 1, SolidLine);
            cell->setBottomBorderPen( pen );
          }
          break;

         default:
          return false;
        } // switch

        cell = table->getNextCellRight( cell->column(), r );
      } // while (cell)
      RowLayout * rw = table->nonDefaultRowLayout( r );
      QPen pen;
      switch ( _ev->key() )
      {
       case Key_Exclam:
        rw->setFormatType( KSpreadCell::Number );
        rw->setPrecision( 2 );
        break;

       case Key_Dollar:
        rw->setFormatType( KSpreadCell::Money );
        rw->setFactor( 1.0 );
        rw->setPrecision( m_pDoc->locale()->fracDigits() );
        break;

       case Key_Percent:
        rw->setFactor( 100.0 );
        rw->setFormatType( KSpreadCell::Percentage );
        break;

       case Key_At:
        rw->setFormatType( KSpreadLayout::SecondeTime );
        rw->setFactor( 1.0 );
        break;

       case Key_NumberSign:
        rw->setFormatType( KSpreadLayout::ShortDate );
        rw->setFactor( 1.0 );
        break;

       case Key_AsciiCircum:
        rw->setFormatType( KSpreadCell::Scientific );
        rw->setFactor( 1.0 );
        break;

       case Key_Ampersand:
        if ( r == rect.top() )
        {
          pen = QPen( m_pView->borderColor(), 1, SolidLine);
          rw->setTopBorderPen( pen );
        }
        if ( r == rect.bottom() )
        {
          pen = QPen( m_pView->borderColor(), 1, SolidLine);
          rw->setBottomBorderPen( pen );
        }
        break;

       default:
        return false;
      }
      table->emit_updateRow( rw, r );
    }

    return true;
  }

  if ( table->isColumnSelected() )
  {
    for ( int c = rect.left(); c <= right; ++c )
    {
      cell = table->getFirstCellColumn( c );
      while ( cell )
      {
        if ( cell->isObscuringForced() )
        {
          cell = table->getNextCellDown( c, cell->row() );
          continue;
        }

        QPen pen;
        switch ( _ev->key() )
        {
         case Key_Exclam:
          convertToDouble( cell );
          cell->setFormatType( KSpreadCell::Number );
          cell->setPrecision( 2 );
          break;

         case Key_Dollar:
          convertToMoney( cell );
          break;

         case Key_Percent:
          convertToPercent( cell );
          break;

         case Key_At:
          convertToTime( cell );
          break;

         case Key_NumberSign:
          convertToDate( cell );
          break;

         case Key_AsciiCircum:
          cell->setFormatType( KSpreadCell::Scientific );
          convertToDouble( cell );
          cell->setFactor( 1.0 );
          break;

         case Key_Ampersand:
          if ( c == rect.left() )
          {
            pen = QPen( m_pView->borderColor(), 1, SolidLine);
            cell->setLeftBorderPen( pen );
          }
          else if ( c == rect.right() )
          {
            pen = QPen( m_pView->borderColor(), 1, SolidLine);
            cell->setRightBorderPen( pen );
          }
          break;

         default:
          return false;
        }
        cell = table->getNextCellDown( c, cell->row() );
      }

      ColumnLayout * cw = table->nonDefaultColumnLayout( c );
      QPen pen;
      switch ( _ev->key() )
      {
       case Key_Exclam:
        cw->setFormatType( KSpreadCell::Number );
        cw->setPrecision( 2 );
        break;

       case Key_Dollar:
        cw->setFormatType( KSpreadCell::Money );
        cw->setFactor( 1.0 );
        cw->setPrecision( m_pDoc->locale()->fracDigits() );
        break;

       case Key_Percent:
        cw->setFactor( 100.0 );
        cw->setFormatType( KSpreadCell::Percentage );
        break;

       case Key_At:
        cw->setFormatType( KSpreadLayout::SecondeTime );
        cw->setFactor( 1.0 );
        break;

       case Key_NumberSign:
        cw->setFormatType( KSpreadLayout::ShortDate );
        cw->setFactor( 1.0 );
        break;

       case Key_AsciiCircum:
        cw->setFactor( 1.0 );
        cw->setFormatType( KSpreadCell::Scientific );
        break;

       case Key_Ampersand:
        if ( c == rect.left() )
        {
          pen = QPen( m_pView->borderColor(), 1, SolidLine);
          cw->setLeftBorderPen( pen );
        }
        if ( c == rect.right() )
        {
          pen = QPen( m_pView->borderColor(), 1, SolidLine);
          cw->setRightBorderPen( pen );
        }
        break;

       default:
        return false;
      }
      table->emit_updateColumn( cw, c );
    }
    return true;
  }

  for ( int row = rect.top(); row <= bottom; ++row )
  {
    for ( int col = rect.left(); col <= right; ++ col )
    {
      cell = table->nonDefaultCell( col, row );

      if ( cell->isObscuringForced() )
        continue;

      QPen  pen;
      switch ( _ev->key() )
      {
       case Key_Exclam:
        convertToDouble( cell );
        cell->setFormatType( KSpreadCell::Number );
        cell->setPrecision( 2 );
        break;

       case Key_Dollar:
        convertToMoney( cell );
        break;

       case Key_Percent:
        convertToPercent( cell );
        break;

       case Key_At:
        convertToTime( cell );
        break;

       case Key_NumberSign:
        convertToDate( cell );
        break;

       case Key_AsciiCircum:
        cell->setFormatType( KSpreadCell::Scientific );
        convertToDouble( cell );
        cell->setFactor( 1.0 );
        break;

       case Key_Ampersand:
        if ( row == rect.top() )
        {
          pen = QPen( m_pView->borderColor(), 1, SolidLine);
          cell->setTopBorderPen( pen );
        }
        if ( row == rect.bottom() )
        {
          pen = QPen( m_pView->borderColor(), 1, SolidLine);
          cell->setBottomBorderPen( pen );
        }
        if ( col == rect.left() )
        {
          pen = QPen( m_pView->borderColor(), 1, SolidLine);
          cell->setLeftBorderPen( pen );
        }
        if ( col == rect.right() )
        {
          pen = QPen( m_pView->borderColor(), 1, SolidLine);
          cell->setRightBorderPen( pen );
        }
        break;

       default:
        return false;
      } // switch
    } // for left .. right
  } // for top .. bottom
  table->updateView(rect);
  _ev->accept();

  return true;
}

void KSpreadCanvas::doAutoScroll()
{
    if ( !m_bMousePressed)
    {
        m_scrollTimer->stop();
        return;
    }

    bool select = false;

    QPoint pos( mapFromGlobal( QCursor::pos() ) );
    if ( pos.y() < 0 )
    {
        vertScrollBar()->setValue(vertScrollBar()->value() - 20);
        select = true;
    }
    else if ( pos.y() > height() )
    {
        vertScrollBar()->setValue(vertScrollBar()->value() + 20);
        select = true;
    }

    if ( pos.x() < 0 )
    {
        horzScrollBar()->setValue(horzScrollBar()->value() - 20);
        select = true;
    }
    else if ( pos.x() > width() )
    {
        horzScrollBar()->setValue(horzScrollBar()->value() + 20);
        select = true;
    }

    if (select)
    {
        QMouseEvent * event = new QMouseEvent(QEvent::MouseMove, pos, 0, 0);
        mouseMoveEvent(event);
        delete event;
    }
}

void KSpreadCanvas::deleteEditor( bool saveChanges )
{
  if( !m_pEditor )
    return;
  // We need to set the line-edit out of edit mode,
  // but only if we are using it (text editor)
  // A bit of a hack - perhaps we should store the editor mode ?
  bool textEditor=true;
  int newHeight = -1;
  int row = -1;
  if ( m_pEditor->inherits("KSpreadTextEditor") )
  {
      if ( m_pEditor->cell()->height() < m_pEditor->height() )
      {
          if (((KSpreadTextEditor * ) m_pEditor)->sizeUpdate())
          {
              newHeight = m_pEditor->height();
              row = m_pEditor->cell()->row();
          }
      }
      m_pEditWidget->setEditMode( false );
  }
  else
    textEditor=false;

  QString t = m_pEditor->text();
  // Delete the cell editor first and after that update the document.
  // That means we get a synchronous repaint after the cell editor
  // widget is gone. Otherwise we may get painting errors.
  delete m_pEditor;
  m_pEditor = 0;

  if (saveChanges && textEditor)
    m_pView->setText( t );
  else
    m_pView->updateEditWidget();

  if (newHeight != -1)
      m_pView->vBorderWidget()->resizeRow(newHeight, row, true);

  setFocus();
}

void KSpreadCanvas::createEditor()
{
  KSpreadCell* cell = activeTable()->cellAt( markerColumn(), markerRow() );

  createEditor( CellEditor );
  if ( cell )
      m_pEditor->setText(cell->text());
}

void KSpreadCanvas::createEditor( EditorType ed, bool addFocus)
{
  KSpreadTable *table = activeTable();
  if ( !m_pEditor )
  {
    KSpreadCell* cell = activeTable()->cellAt( marker() );
    if ( ed == CellEditor )
    {
      m_pEditWidget->setEditMode( true );

      m_pEditor = new KSpreadTextEditor( cell, this );
    }

    int w, h;
    int min_w = cell->width( markerColumn(), this );
    int min_h = cell->height( markerRow(), this );
    if ( cell->isDefault() )
    {
      w = min_w;
      h = min_h;
      //kdDebug(36001) << "DEFAULT" << endl;
    }
    else
    {
      w = cell->extraWidth() + 1;
      h = cell->extraHeight() + 1;
      //kdDebug(36001) << "HEIGHT=" << min_h << " EXTRA=" << h << endl;
    }
    int xpos = table->columnPos( markerColumn(), this );
    int ypos = table->rowPos( markerRow(), this );
    QPalette p = m_pEditor->palette();
    QColorGroup g( p.active() );
    QColor color=cell->textColor( markerColumn(), markerRow() );
    if(!color.isValid())
        color=QApplication::palette().active().text();
    g.setColor( QColorGroup::Text, color);
    color=cell->bgColor( markerColumn(), markerRow() );
    if(!color.isValid())
        color=g.base();
    g.setColor( QColorGroup::Background, color );
    m_pEditor->setPalette( QPalette( g, p.disabled(), g ) );
    m_pEditor->setFont( cell->textFont( markerColumn(), markerRow() ) );
    m_pEditor->setGeometry( xpos, ypos, w, h );
    m_pEditor->setMinimumSize( QSize( min_w, min_h ) );
    m_pEditor->show();
    //kdDebug(36001) << "FOCUS1" << endl;
    //Laurent 2001-12-05
    //Don't add focus when we create a new editor and
    //we select text in edit widget otherwise we don't delete
    //selected text.
    if(addFocus)
        m_pEditor->setFocus();
    //kdDebug(36001) << "FOCUS2" << endl;
  }
}

void KSpreadCanvas::closeEditor()
{
   if(m_bChoose)
        return;

   if ( m_pEditor )
   {
        deleteEditor( true ); // save changes
   }
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void KSpreadCanvas::updateCellRect( const QRect &_rect )
{
    updateSelection( _rect, activeTable()->marker() );
}

void KSpreadCanvas::updateSelection( const QRect & oldSelection,
                                     const QPoint& /*oldMarker*/ )
  /* for now oldMarker is unused.  Maybe we can just remove it? */
{
  KSpreadTable *table = activeTable();
  if ( !table )
    return;

  /* two areas that indicate the areas needing repainting */
  QRect oldSelectionArea;
  QRect newSelectionArea;

  oldSelectionArea = oldSelection;
  newSelectionArea = table->selection();

  /* since the marker/selection border extends into neighboring cells, we
     want to calculate all the cells bordering these regions.
  */
  ExtendRectBorder(oldSelectionArea);
  ExtendRectBorder(newSelectionArea);

  /* Prepare the painter */
  QPainter painter;
  painter.begin( this );

  // Do the view transformation.
  QWMatrix m = m_pView->matrix();
  painter.setWorldMatrix( m );

  // Which part of the document is visible ? To determine this
  // just transform the viewport rectangle with the inverse
  // matrix, since this matrix usually transforms from document
  // coordinates to view coordinates.
  m = m.invert();
  QPoint tl = m.map( QPoint( 0, 0 ) );
  QPoint br = m.map( QPoint( width(), height() ) );
  QRect view( tl, br );

  //
  // Clip away children
  //

  QRegion rgn = painter.clipRegion();
  if ( rgn.isEmpty() )
    rgn = QRegion( QRect( 0, 0, width(), height() ) );
  QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
  for( ; it.current(); ++it )
  {
    if ( ((KSpreadChild*)it.current())->table() == activeTable() &&
         !m_pView->hasDocumentInWindow( it.current()->document() ) )
      rgn -= it.current()->region( painter.worldMatrix() );
  }
  painter.setClipRegion( rgn );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );

  PaintRegion(oldSelectionArea, view, painter);
  PaintRegion(newSelectionArea, view, painter);

  painter.end();
  // XIM Position
  int xpos_xim, ypos_xim;
  xpos_xim = table->columnPos( markerColumn(), this );
  ypos_xim = table->rowPos( markerRow(), this );
  setMicroFocusHint(xpos_xim, ypos_xim, 0, 16);
}

void KSpreadCanvas::ExtendRectBorder(QRect& area)
{
  ColumnLayout *cl;
  RowLayout *rl;
  //look at if column is hiding.
  //if it's hiding refreshing column+1 (or column -1 )
  int left = area.left();
  int right = area.right();
  int top = area.top();
  int bottom = area.bottom();

  if ( right < KS_colMax )
  {
    do
    {
      right++;
      cl = activeTable()->nonDefaultColumnLayout( right );
    } while( cl->isHide() && right != KS_colMax );
  }
  if ( left > 1 )
  {
    do
    {
      left--;
      cl = activeTable()->nonDefaultColumnLayout( left );
    } while( cl->isHide() && left != 1);
  }

  if ( bottom < KS_rowMax )
  {
    do
    {
      bottom++;
      rl = activeTable()->nonDefaultRowLayout( bottom );
    } while( rl->isHide() && bottom != KS_rowMax );
  }

  if ( top > 1 )
  {
    do
    {
      top--;
      rl = activeTable()->nonDefaultRowLayout( top );
    } while( rl->isHide() && top != 1);
  }

  area.setLeft(left);
  area.setRight(right);
  area.setTop(top);
  area.setBottom(bottom);
}

void KSpreadCanvas::PaintRegion(QRect paintRegion, QRect viewRegion,
                                QPainter &painter)
{
  /* paint region has cell coordinates (col,row) while viewRegion has world
     coordinates.  paintRegion is the cells to update and viewRegion is the
     area actually onscreen.
  */
  KSpreadTable *table = activeTable();

  /* get the world coordinates of the upper left corner of the paintRegion */
  QPoint corner(table->columnPos(paintRegion.left()),
                table->rowPos(paintRegion.top()));

  QPoint currentCellPos = corner;

  for ( int y = paintRegion.top();
        y <= paintRegion.bottom() && currentCellPos.y() <= viewRegion.bottom();
        y++ )
  {
    RowLayout *row_lay = table->rowLayout( y );
    currentCellPos.setX(corner.x());

    for ( int x = paintRegion.left();
          x <= paintRegion.right() && currentCellPos.x() <= viewRegion.right();
          x++ )
    {
      ColumnLayout *col_lay = table->columnLayout( x );
      KSpreadCell *cell = table->cellAt( x, y );

      QPoint cellCoordinate( x, y );
      cell->paintCell( viewRegion, painter, currentCellPos, cellCoordinate);

      currentCellPos.setX(currentCellPos.x() + col_lay->width());
    }
    currentCellPos.setY(currentCellPos.y() + row_lay->height());
  }
}

//---------------------------------------------
//
// Choose Marker
//
//---------------------------------------------

void KSpreadCanvas::updateChooseMarker( const QRect& _old, const QRect& _new )
{
  if ( isChooseMarkerVisible() )
  {
    drawChooseMarker( _old );
    drawChooseMarker( _new );
  }
  else
  {
    choose_visible = TRUE;
    drawChooseMarker( _new );
  }

  if ( _new.left() == 0 || !m_bChoose || !m_pEditor )
  {
    //kdDebug(36001) << "updateChooseMarker len=0" << endl;
    length_namecell = 0;
    return;
  }

  KSpreadTable* table = activeTable();

  // ##### Torben: Clean up here!
  QString name_cell;

  //kdDebug(36001) << m_chooseStartTable->tableName() << ", "
  //               << table->tableName() << endl;
  if( m_chooseStartTable != table )
  {
    if ( _new.left() >= _new.right() && _new.top() >= _new.bottom() )
      name_cell = util_cellName( table, _new.left(), _new.top() );
    else
      name_cell = util_rangeName( table, _new );
  }
  else
  {
    if ( _new.left() >= _new.right() && _new.top() >= _new.bottom() )
      name_cell = util_cellName( _new.left(), _new.top() );
    else
      name_cell = util_rangeName( _new );
  }

  int old = length_namecell;
  length_namecell= name_cell.length();
  length_text = m_pEditor->text().length();
  //kdDebug(36001) << "updateChooseMarker2 len=" << length_namecell << endl;

  QString text = m_pEditor->text();
  QString res = text.left( m_pEditor->cursorPosition() - old ) + name_cell + text.right( text.length() - m_pEditor->cursorPosition() );
  int pos = m_pEditor->cursorPosition() - old;

  ((KSpreadTextEditor*)m_pEditor)->blockCheckChoose( TRUE );
  m_pEditor->setText( res );
  ((KSpreadTextEditor*)m_pEditor)->blockCheckChoose( FALSE );
  m_pEditor->setCursorPosition( pos + length_namecell );
  //kdDebug(36001) << "old=" << old << " len=" << length_namecell << " pos=" << pos << endl;
}

void KSpreadCanvas::updatePosWidget()
{
    QRect selection = m_pView->activeTable()->selection();
    QString buffer;
    QString tmp;
    // No selection, or only one cell merged selected
    if ( activeTable()->singleCellSelection() )
    {
        if(activeTable()->getLcMode())
        {
            buffer="L"+tmp.setNum( markerRow() );
            buffer+="C"+tmp.setNum( markerColumn() );
        }
        else
        {
            buffer=util_encodeColumnLabelText( markerColumn() );
            buffer+=tmp.setNum( markerRow() );
        }
    }
  /*else if((!activeTable->isRowSelected( selection.) && (!activeTable->isColumnSelected( selection.))
        {
        if(activeTable()->getLcMode())
                {
                buffer=tmp.setNum( (selection.bottom()-m_iMarkerRow+1) )+"Lx";
                buffer+=tmp.setNum((selection.right()-m_iMarkerColumn+1))+"C";
                }
        else
                {
                buffer=util_encodeColumnLabelText( m_iMarkerColumn );
                buffer+=tmp.setNum(m_iMarkerRow);
                buffer+=":";
                buffer+=util_encodeColumnLabelText( selection.right() );
                buffer+=tmp.setNum(selection.bottom());
                }
        }*/
  else
  {
        if(activeTable()->getLcMode())
        {
            buffer=tmp.setNum( (selection.bottom()-selection.top()+1) )+"Lx";
            if( activeTable()->isRowSelected( selection ) )
                buffer+=tmp.setNum((KS_colMax-selection.left()+1))+"C";
            else
                buffer+=tmp.setNum((selection.right()-selection.left()+1))+"C";
        }
        else
        {
                //encodeColumnLabelText return @@@@ when column >KS_colMax
                //=> it's not a good display
                //=> for the moment I display pos of marker
                buffer=util_encodeColumnLabelText( selection.left() );
                buffer+=tmp.setNum(selection.top());
                buffer+=":";
                buffer+=util_encodeColumnLabelText( QMIN( KS_colMax, selection.right() ) );
                buffer+=tmp.setNum(selection.bottom());
                //buffer=activeTable()->columnLabel( m_iMarkerColumn );
                //buffer+=tmp.setNum(m_iMarkerRow);
        }
  }

  m_pPosWidget->setText(buffer);
}


void KSpreadCanvas::drawChooseMarker()
{
  drawChooseMarker( activeTable()->chooseRect() );
}

void KSpreadCanvas::drawChooseMarker( const QRect& selection )
{
  // Draw nothing if the selection is empty
  if ( selection.left() == 0 )
    return;

  QPainter painter;
  painter.begin( this );

  int xpos;
  int ypos;
  int w, h;

  if ( selection.left() == 0 || activeTable()->isRowSelected( selection ) || activeTable()->isColumnSelected( selection ) )
  {
    xpos = activeTable()->columnPos( chooseMarkerColumn(), this );
    ypos = activeTable()->rowPos( chooseMarkerRow(), this );
    KSpreadCell *cell = activeTable()->cellAt( chooseMarkerColumn(), chooseMarkerRow() );
    w = cell->width( chooseMarkerColumn(), this );
    h = cell->height( chooseMarkerRow(), this );
  }
  else
  {
    xpos = activeTable()->columnPos( selection.left(), this );
    ypos = activeTable()->rowPos( selection.top(), this );
    int x = activeTable()->columnPos( selection.right(), this );
    KSpreadCell *cell = activeTable()->cellAt( selection.right(), selection.top() );
    int tw = cell->width( selection.right(), this );
    w = ( x - xpos ) + tw;
    cell = activeTable()->cellAt( selection.left(), selection.bottom() );
    int y = activeTable()->rowPos( selection.bottom(), this );
    int th = cell->height( selection.bottom(), this );
    h = ( y - ypos ) + th;
  }

  RasterOp rop = painter.rasterOp();

  painter.setRasterOp( NotROP );
  QPen pen;
  pen.setWidth( 2 );
  pen.setStyle(DashLine);
  painter.setPen( pen );

  painter.drawLine( xpos - 2, ypos - 1, xpos + w + 2, ypos - 1 );
  painter.drawLine( xpos - 1, ypos + 1, xpos - 1, ypos + h + 3 );
  painter.drawLine( xpos + 1, ypos + h + 1, xpos + w - 3, ypos + h + 1 );
  painter.drawLine( xpos + w, ypos + 1, xpos + w, ypos + h - 2 );
  // painter.fillRect( xpos + w - 2, ypos + h - 1, 5, 5, black );
  painter.setRasterOp( rop );

  painter.end();
}

void KSpreadCanvas::setChooseMarker( const QPoint& p )
{
  if ( p.x() == m_i_chooseMarkerColumn && p.y() == m_i_chooseMarkerRow )
    return;

  m_i_chooseMarkerRow = p.y();
  m_i_chooseMarkerColumn = p.x();
  // This will trigger a redraw
  activeTable()->setChooseRect( QRect( p.x(), p.y(), 1, 1 ) );
}

void KSpreadCanvas::adjustArea(bool makeUndo)
{
  QRect selection( activeTable()->selection() );
  if(activeTable()->areaIsEmpty())
        return;

  if(makeUndo)
  {
        if ( !doc()->undoBuffer()->isLocked() )
        {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( doc(),activeTable() , selection );
                doc()->undoBuffer()->appendUndo( undo );
        }
  }
  // Columns selected
  if( activeTable()->isColumnSelected() )
  {
    for (int x=selection.left(); x <= selection.right(); x++ )
    {
      hBorderWidget()->adjustColumn(x,false);
    }
  }
  // Rows selected
  else if( activeTable()->isRowSelected() )
  {
    for(int y = selection.top(); y <= selection.bottom(); y++ )
    {
      vBorderWidget()->adjustRow(y,false);
    }
  }
  // No selection
  // Selection of a rectangular area
  else
  {
    for (int x=selection.left(); x <= selection.right(); x++ )
    {
      hBorderWidget()->adjustColumn(x,false);
    }
    for(int y = selection.top(); y <= selection.bottom(); y++ )
    {
      vBorderWidget()->adjustRow(y,false);
    }
  }
}

void KSpreadCanvas::equalizeRow()
{
  QRect selection( activeTable()->selection() );
  RowLayout *rl;
  int size;

  rl = m_pView->activeTable()->rowLayout(selection.top());
  size=rl->height(this);
  for(int i=selection.top()+1;i<=selection.bottom();i++)
  {
    size=QMAX(m_pView->activeTable()->rowLayout(i)->height(this),size);
  }
  m_pView->vBorderWidget()->equalizeRow(size);
}

void KSpreadCanvas::equalizeColumn()
{
  ColumnLayout *cl;
  QRect selection( activeTable()->selection() );
  int size;

  cl = m_pView->activeTable()->columnLayout(selection.left());
  size=cl->width(this);
  for(int i=selection.left()+1;i<=selection.right();i++)
  {
    size=QMAX(m_pView->activeTable()->columnLayout(i)->width(this),size);
  }
  m_pView->hBorderWidget()->equalizeColumn(size);
}

/****************************************************************
 *
 * KSpreadVBorder
 *
 ****************************************************************/

KSpreadVBorder::KSpreadVBorder( QWidget *_parent, KSpreadCanvas *_canvas, KSpreadView *_view)
    : QWidget( _parent, "", /*WNorthWestGravity*/WStaticContents | WResizeNoErase | WRepaintNoErase )
{
  m_pView = _view;
  m_pCanvas = _canvas;
  m_lSize = 0L;

  setBackgroundMode( PaletteButton );
  setMouseTracking( TRUE );
  m_bResize = FALSE;
  m_bSelection = FALSE;
  m_iSelectionAnchor=1;
}

void KSpreadVBorder::mousePressEvent( QMouseEvent * _ev )
{
  m_bResize = FALSE;
  m_bSelection = FALSE;
  if(!m_pView->koDocument()->isReadWrite())
    return;

  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );
  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
        {
	    m_pCanvas->deleteEditor( true ); // save changes
        }
  // Find the first visible row and the y position of this row.
  int y = 0;
  int row = table->topRow( 0, y, m_pCanvas );

  // Did the user click between two rows ?
  while ( y < height() )
  {
    int h = table->rowLayout( row )->height( m_pCanvas );
    row++;
    if ( row > KS_rowMax )
	row = KS_rowMax;
    if ( _ev->pos().y() >= y + h - 1 && _ev->pos().y() <= y + h + 1
	&& !(table->rowLayout( row )->isHide()&&row==1) )
      m_bResize = TRUE;
    y += h;
  }
  int tmp2;
  int tmpRow=table->topRow( _ev->pos().y() - 1, tmp2, m_pCanvas );
  if(table->rowLayout(tmpRow  )->isHide()&&tmpRow==1)
      m_bResize = false;
  // So he clicked between two rows ?
  if ( m_bResize )
  {
    // Determine row to resize
    int tmp;
    m_iResizedRow = table->topRow( _ev->pos().y() - 1, tmp, m_pCanvas );

    paintSizeIndicator( _ev->pos().y(), true );
  }
  else
  {
    m_bSelection = TRUE;

    int tmp;
    int hit_row = table->topRow( _ev->pos().y(), tmp, m_pCanvas );
    if(hit_row > KS_rowMax)
	return;

    m_iSelectionAnchor = hit_row;

    QRect rect = m_pCanvas->activeTable()->selection();
    QRect selection;
    if(!rect.contains( QPoint(1,hit_row)) || !(_ev->button() == RightButton)
                || (!m_pCanvas->activeTable()->isRowSelected()) )
        {
        selection.setCoords( 1, hit_row, KS_colMax, hit_row );
        table->setSelection( selection, m_pCanvas );
        }

    if ( _ev->button() == RightButton )
    {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupRowMenu( p );
      m_bSelection=FALSE;
    }
    m_pView->updateEditWidget();
  }
}

void KSpreadVBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
    KSpreadTable *table = m_pCanvas->activeTable();
    assert( table );
    if( !m_pView->koDocument()->isReadWrite() )
        return;

    if ( m_bResize )
    {
        // Remove size indicator painted by paintSizeIndicator
        QPainter painter;
        painter.begin( m_pCanvas );
        painter.setRasterOp( NotROP );
        painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );
        painter.end();

        int start = m_iResizedRow;
        int end = m_iResizedRow;
        QRect selection = m_pCanvas->activeTable()->selection();
        QRect rect;
        rect.setCoords( 1, m_iResizedRow, KS_colMax, m_iResizedRow );
        if( m_pCanvas->activeTable()->isRowSelected() )
        {
            if( selection.contains( QPoint( 1, m_iResizedRow ) ) )
            {
                start=selection.top();
                end=selection.bottom();
                rect=selection;
            }
        }

        int height = 0;
        int y = table->rowPos( m_iResizedRow, m_pCanvas );
        if (( m_pCanvas->zoom() * (float)( _ev->pos().y() - y ) ) <=0 /*(2.0* m_pCanvas->zoom())*/)
            height = 0 /*(int)(2.0* m_pCanvas->zoom())*/;
        else
            height = _ev->pos().y() - y;

        if ( !m_pCanvas->doc()->undoBuffer()->isLocked() )
        {
            if(height!=0)
            {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),m_pCanvas->activeTable() , rect );
                m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
            }
            else
            {
                //hide row
                KSpreadUndoHideRow *undo = new KSpreadUndoHideRow( m_pCanvas->doc(),m_pCanvas->activeTable() ,  rect.top(),(rect.bottom()-rect.top()) );
                m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
            }
        }

        for(int i = start; i <= end; i++ )
        {
            RowLayout *rl = table->nonDefaultRowLayout( i );
            if(height!=0)
            {
                if(!rl->isHide())
                    rl->setHeight( height, m_pCanvas );
            }
            else
                rl->setHide(true );
        }

        if(height==0)
            table->emitHideColumn();

        delete m_lSize;
        m_lSize = 0;
    }
    else if (m_bSelection)
    {
        QRect rect = table->selection();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if ( m_frozen )
        {
            kdDebug(36001) << "selected: T " << rect.top() << " B " << rect.bottom() << endl;

            int i;
            RowLayout * row;
            QValueList<int>hiddenRows;

            for ( i = rect.top(); i <= rect.bottom(); ++i )
            {
                row = m_pView->activeTable()->rowLayout( i );
                if ( row->isHide() )
                {
                    hiddenRows.append(i);
                }
            }

            if (hiddenRows.count() > 0)
                m_pView->activeTable()->showRow(0, -1, hiddenRows);
        }
    }

    m_bSelection = FALSE;
    m_bResize = FALSE;
}

void KSpreadVBorder::adjustRow( int _row, bool makeUndo )
{
  int adjust;
  int select;
  if(_row==-1)
  {
    adjust=m_pCanvas->activeTable()->adjustRow(QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
    select=m_iSelectionAnchor;
  }
  else
  {
    adjust=m_pCanvas->activeTable()->adjustRow(QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),_row);
    select=_row;
  }
  if(adjust!=-1)
  {
    if(makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
    {
        QRect rect;
        rect.setCoords( 1, select, KS_colMax, select);
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),m_pCanvas->activeTable() , rect );
        m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
    }
    KSpreadTable *table = m_pCanvas->activeTable();
    assert( table );
    RowLayout *rl = table->nonDefaultRowLayout( select );
    adjust=QMAX((int)(2.0* m_pCanvas->zoom()),adjust);
    rl->setHeight(adjust,m_pCanvas);
  }
}

void KSpreadVBorder::equalizeRow( int resize )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  Q_ASSERT( table );
  QRect selection( table->selection() );
  if ( !m_pCanvas->doc()->undoBuffer()->isLocked() )
  {
     KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),m_pCanvas->activeTable() , selection );
     m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
  }
  RowLayout *rl;
  for (int i=selection.top();i<=selection.bottom();i++)
  {
     rl= table->nonDefaultRowLayout( i );
     resize=QMAX((int)(2.0* m_pCanvas->zoom()), resize);
     rl->setHeight( resize, m_pCanvas );
  }
}

void KSpreadVBorder::resizeRow( int resize, int nb, bool makeUndo )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  Q_ASSERT( table );
  if( nb == -1 ) // I don't know, where this is the case
  {
    if( makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
    {
        QRect rect;
        rect.setCoords( 1, m_iSelectionAnchor, KS_colMax, m_iSelectionAnchor );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
    }
    RowLayout *rl = table->nonDefaultRowLayout( m_iSelectionAnchor );
    resize = QMAX( (int)(2.0* m_pCanvas->zoom()), resize );
    rl->setHeight( resize, m_pCanvas );
  }
  else
  {
    QRect selection( table->selection() );
    if( table->singleCellSelection() )
    {
      if( makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
      {
        QRect rect;
        rect.setCoords( 1, m_pCanvas->markerRow(), KS_colMax, m_pCanvas->markerRow() );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
      }
      RowLayout *rl = table->nonDefaultRowLayout( m_pCanvas->markerRow() );
      resize=QMAX((int)(2.0* m_pCanvas->zoom()), resize);
      rl->setHeight( resize, m_pCanvas );
    }
    else
    {
      if(makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
      {
          KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), selection );
          m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
      }
      RowLayout *rl;
      for ( int i=selection.top(); i<=selection.bottom(); i++ )
      {
        rl = table->nonDefaultRowLayout( i );
        resize=QMAX((int)(2.0* m_pCanvas->zoom()), resize);
        rl->setHeight( resize, m_pCanvas );
      }
    }
  }
}

void KSpreadVBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  if(!m_pView->koDocument()->isReadWrite())
    return;

  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );
  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    paintSizeIndicator( _ev->pos().y(), false );
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    int y = 0;
    int row = table->topRow( _ev->pos().y(), y, m_pCanvas );
    if( row > KS_rowMax )
	return;
    QRect selection = table->selection();

    if ( row < m_iSelectionAnchor )
    {
      selection.setTop( row );
      selection.setBottom( m_iSelectionAnchor );
    }
    else
    {
      selection.setBottom( row );
      selection.setTop( m_iSelectionAnchor );
    }
    table->setSelection( selection, m_pCanvas );

    if ( _ev->pos().y() < 0 )
      m_pCanvas->vertScrollBar()->setValue( m_pCanvas->yOffset() + y );
    else if ( _ev->pos().y() > m_pCanvas->height() )
    {
      if (row < KS_rowMax)
      {
        RowLayout *rl = table->rowLayout( row + 1 );
        y = table->rowPos( row + 1, m_pCanvas );
        m_pCanvas->vertScrollBar()->setValue( m_pCanvas->yOffset()
                                              + y + rl->height( m_pCanvas )
                                              - m_pCanvas->height() );
      }
    }
  }
  // No button is pressed and the mouse is just moved
  else
  {
    int tmp;
    int tmpRow = table->topRow( _ev->pos().y() - 1, tmp, m_pCanvas );
    int ypos   = _ev->pos().y();

    if ( ( (table->topRow( ( ypos - 1), tmp, m_pCanvas ) != tmpRow)
           || (table->topRow( ( ypos - 1) + 3, tmp, m_pCanvas ) != tmpRow) )
         && !(table->rowLayout(tmpRow)->isHide() && tmpRow == 1) )
    {
        setCursor(splitVCursor);
        return;
    }

    /* Doesn't work correctly, gets removed if it turns out that the new
       version really works, Norbert
    while ( y < height() )
    {
      int h = table->rowLayout( row )->height( m_pCanvas );

      if ( _ev->pos().y() >= y + h - 1 && _ev->pos().y() <= y + h + 1
	   &&!(table->rowLayout(tmpRow)->isHide()&&tmpRow==1))
      {
        setCursor(splitVCursor);
        return;
      }
      y += h;
    }
    */

    setCursor( arrowCursor );
  }
}

void KSpreadVBorder::mouseDoubleClickEvent( QMouseEvent * /*_ev */)
{
  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );
  if(!m_pView->koDocument()->isReadWrite())
    return;
  adjustRow();
}


void KSpreadVBorder::wheelEvent( QWheelEvent* _ev )
{
  if ( m_pCanvas->vertScrollBar() )
    QApplication::sendEvent( m_pCanvas->vertScrollBar(), _ev );
}

void KSpreadVBorder::paintSizeIndicator( int mouseY, bool firstTime )
{
    KSpreadTable *table = m_pCanvas->activeTable();
    assert( table );

    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );
    if (!firstTime)
      painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    m_iResizePos = mouseY;
    // Dont make the row have a height < 2 pixel.
    //int twenty = (int)( 2.0 * m_pCanvas->zoom() );
    int y = table->rowPos( m_iResizedRow, m_pCanvas );
    if ( m_iResizePos < y /*+ twenty*/ )
        m_iResizePos = y /*+ twenty*/;
    painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    painter.end();

    QString tmpSize;
    if(m_iResizePos!=y)
        tmpSize=i18n("Height: %1 %2").arg(KoUnit::ptToUnit(((m_iResizePos-y)/m_pCanvas->zoom()) ,m_pView->doc()->getUnit() )).arg(m_pView->doc()->getUnitName());
    else
        tmpSize=i18n("Hide Row");

    painter.begin(this);
    int len = painter.fontMetrics().width(tmpSize );
    int hei = painter.fontMetrics().height( );
    painter.end();
    if(!m_lSize)
    {
          m_lSize=new QLabel(m_pCanvas);
          m_lSize->setGeometry(3,3+y,len+2, hei+2 ) ;
          m_lSize->setAlignment(Qt::AlignVCenter);
          m_lSize->setText(tmpSize);
          m_lSize->show();
    }
    else
    {
          m_lSize->setGeometry(3,3+y,len+2, hei+2 );
          m_lSize->setText(tmpSize);
    }
}

void KSpreadVBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  if ( !table )
    return;

  QPainter painter;
  painter.begin( this );
  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
  // painter.setBackgroundColor( colorGroup().base() );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  painter.setClipRect( _ev->rect() );

  int ypos;
  int top_row = table->topRow( _ev->rect().y(), ypos, m_pCanvas );
  int bottom_row = table->bottomRow( _ev->rect().bottom(), m_pCanvas );

  QRect selection( table->selection() );

  QFont normalFont = painter.font();
  QFont boldFont = normalFont;
  boldFont.setBold( TRUE );

  //several cells selected but not just a cell merged
  bool area= !(table->singleCellSelection());

  for ( int y = top_row; y <= bottom_row; y++ )
  {
    bool highlighted = (area && y >= selection.top() && y <= selection.bottom() );
    bool selected = ( highlighted && (table->isRowSelected()) );

    RowLayout *row_lay = table->rowLayout( y );

    if ( selected )
    {
      QBrush fillSelected( colorGroup().brush( QColorGroup::Highlight ) );
      qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH,
                  row_lay->height( m_pCanvas ), colorGroup(), FALSE, 1,
                  &fillSelected );
    }
    else if ( highlighted )
    {
      QBrush fillHighlighted( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH,
                  row_lay->height( m_pCanvas ), colorGroup(), true, 1,
                  &fillHighlighted );
    }
    else
    {
      QBrush fill( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH, row_lay->height( m_pCanvas ), colorGroup(), FALSE, 1, &fill );
    }

    char buffer[ 20 ];
    sprintf( buffer, "%i", y );

    // Reset painter
    painter.setFont( normalFont );
    painter.setPen( colorGroup().text() );

    if ( selected )
      painter.setPen( colorGroup().highlightedText() );
    else if ( highlighted )
      painter.setFont( boldFont );
    int len = painter.fontMetrics().width(buffer );
    if(!row_lay->isHide())
        painter.drawText( (YBORDER_WIDTH-len)/2, ypos +
                    ( row_lay->height( m_pCanvas ) + painter.fontMetrics().ascent() - painter.fontMetrics().descent() ) / 2, buffer );

    ypos += row_lay->height( m_pCanvas );
  }
  m_pCanvas->updatePosWidget();
  painter.end();
}

/****************************************************************
 *
 * KSpreadHBorder
 *
 ****************************************************************/

KSpreadHBorder::KSpreadHBorder( QWidget *_parent, KSpreadCanvas *_canvas,KSpreadView *_view )
    : QWidget( _parent, "", /*WNorthWestGravity*/ WStaticContents| WResizeNoErase | WRepaintNoErase )
{
  m_pView = _view;
  m_pCanvas = _canvas;
  m_lSize = 0L;
  setBackgroundMode( PaletteButton );
  setMouseTracking( TRUE );
  m_bResize = FALSE;
  m_bSelection = FALSE;
  m_iSelectionAnchor=1;
}

void KSpreadHBorder::mousePressEvent( QMouseEvent * _ev )
{
    KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );

  if(!m_pView->koDocument()->isReadWrite())
    return;

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
  {
      m_pCanvas->deleteEditor( true ); // save changes
  }
  m_bResize = FALSE;
  m_bSelection = FALSE;

  int x = 0;
  int col = table->leftColumn( 0, x, m_pCanvas );

  while ( x < width() && !m_bResize )
  {
    int w = table->columnLayout( col )->width( m_pCanvas );
    col++;
    if ( col > KS_colMax )
	col = KS_colMax;
    if ( _ev->pos().x() >= x + w - 1 && _ev->pos().x() <= x + w + 1 &&
	 !(table->columnLayout( col )->isHide()&&col==1))
      m_bResize = TRUE;
    x += w;
  }

  //if col is hide and it's the first column
  //you mustn't resize it.
  int tmp2;
  int tmpCol=table->leftColumn( _ev->pos().x() - 1, tmp2, m_pCanvas );
  if ( table->columnLayout(tmpCol  )->isHide() && tmpCol == 1)
      m_bResize = false;

  QRect rect = table->selection();
  if ( m_bResize )
  {
    // Determine the column to resize
    int tmp;
    m_iResizedColumn = table->leftColumn( _ev->pos().x() - /*3*/1, tmp, m_pCanvas );
    paintSizeIndicator( _ev->pos().x(), true );
  }
  else if ( ( rect.left() != rect.right() )
            && ( tmpCol >= rect.left() )
            && ( tmpCol <= rect.right() )
      && _ev->button() == RightButton )
  {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupColumnMenu( p );
  }
  else
  {
    m_bSelection = TRUE;
    int tmp;
    int hit_col = table->leftColumn( _ev->pos().x(), tmp, m_pCanvas );
    if( hit_col > KS_colMax)
	return;
    m_iSelectionAnchor = hit_col;

    QRect r;
    if(!rect.contains( QPoint(hit_col,1)) || !(_ev->button() == RightButton)
                                          || !(table->isRowSelected()) )
    {
        r.setCoords( hit_col, 1, hit_col, KS_rowMax );
        table->setSelection( r, m_pCanvas );
    }
    if ( _ev->button() == RightButton )
    {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupColumnMenu( p );
      m_bSelection=FALSE;
    }
    m_pView->updateEditWidget();
  }
}

void KSpreadHBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
    KSpreadTable *table = m_pCanvas->activeTable();
    assert( table );
    if(!m_pView->koDocument()->isReadWrite())
        return;

    if ( m_bResize )
    {
        // Remove size indicator painted by paintSizeIndicator
        QPainter painter;
        painter.begin( m_pCanvas );
        painter.setRasterOp( NotROP );
        painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );
        painter.end();

        int start=m_iResizedColumn;
        int end=m_iResizedColumn;
        QRect selection = m_pCanvas->activeTable()->selection();
        QRect rect;
        rect.setCoords( m_iResizedColumn, 1, m_iResizedColumn, KS_rowMax );
        if( m_pCanvas->activeTable()->isColumnSelected() )
        {
            if(selection.contains(QPoint(m_iResizedColumn,1)))
            {
                start=selection.left();
                end=selection.right();
                rect=selection;
            }
        }

        int width=0;
        int x = table->columnPos( m_iResizedColumn, m_pCanvas );
        if ( ( m_pCanvas->zoom() * (float)( _ev->pos().x() - x ) ) <=0.0 )
            width= 0 ; /*(int)(2.0* m_pCanvas->zoom());*/
        else
            width=_ev->pos().x() - x;

        if ( !m_pCanvas->doc()->undoBuffer()->isLocked() )
        {
            //juste resize
            if(width!=0)
            {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),m_pCanvas->activeTable() , rect );
                m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
            }
            else
            {//hide column
                KSpreadUndoHideColumn *undo = new KSpreadUndoHideColumn( m_pCanvas->doc(), m_pCanvas->activeTable(),rect.left(),(rect.right()-rect.left()));
                m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
            }
        }

        for(int i=start;i<=end;i++)
        {
            ColumnLayout *cl = table->nonDefaultColumnLayout( i );
            if( width!=0)
            {
                if(!cl->isHide())
                    cl->setWidth( width, m_pCanvas );
            }
            else
                cl->setHide(true);
        }

        if(width==0)
            table->emitHideRow();

        delete m_lSize;
        m_lSize=0;
    }
    else if (m_bSelection)
    {
        QRect rect = table->selection();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if ( m_frozen )
        {
            kdDebug(36001) << "selected: L " << rect.left() << " R " << rect.right() << endl;

            int i;
            ColumnLayout * col;
            QValueList<int>hiddenCols;

            for ( i = rect.left(); i <= rect.right(); ++i )
            {
                col = m_pView->activeTable()->columnLayout( i );
                if ( col->isHide() )
                {
                    hiddenCols.append(i);
                }
            }

            if (hiddenCols.count() > 0)
                m_pView->activeTable()->showColumn(0, -1, hiddenCols);
        }
    }

    m_bSelection = FALSE;
    m_bResize = FALSE;
}

void KSpreadHBorder::adjustColumn( int _col, bool makeUndo )
{
  int adjust;
  int select;
  if( _col==-1 )
  {
    adjust = m_pCanvas->activeTable()->adjustColumn(QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ));
    select=m_iSelectionAnchor;
  }
  else
  {
    adjust=m_pCanvas->activeTable()->adjustColumn(QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ),_col);
    select=_col;
  }

  if(adjust!=-1)
  {
    KSpreadTable *table = m_pCanvas->activeTable();
    assert( table );

    if( makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
    {
        QRect rect;
        rect.setCoords( select, 1, select, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),m_pCanvas->activeTable() , rect );
        m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
    }

    ColumnLayout *cl = table->nonDefaultColumnLayout( select );

    adjust = QMAX( (int)(2.0 * m_pCanvas->zoom()), adjust );
    cl->setWidth( adjust, m_pCanvas );
  }
}

void KSpreadHBorder::equalizeColumn( int resize )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  Q_ASSERT( table );
  QRect selection( table->selection() );
  if ( !m_pCanvas->doc()->undoBuffer()->isLocked() )
  {
      KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),m_pCanvas->activeTable() , selection );
      m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
  }
  ColumnLayout *cl;
  for (int i=selection.left();i<=selection.right();i++)
  {
      cl= table->nonDefaultColumnLayout( i );
      resize = QMAX( (int)(2.0* m_pCanvas->zoom()), resize );
      cl->setWidth( resize, m_pCanvas );
  }

}

void KSpreadHBorder::resizeColumn( int resize, int nb, bool makeUndo )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  Q_ASSERT( table );

  if( nb == -1 )
  {
    if( makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
    {
        QRect rect;
        rect.setCoords( m_iSelectionAnchor, 1, m_iSelectionAnchor, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
    }
    ColumnLayout *cl = table->nonDefaultColumnLayout( m_iSelectionAnchor );
    resize = QMAX( (int)(2.0* m_pCanvas->zoom()), resize );
    cl->setWidth( resize, m_pCanvas );
  }
  else
  {
    QRect selection( table->selection() );
    if( table->singleCellSelection() )
    {
      if( makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
      {
        QRect rect;
        rect.setCoords( m_iSelectionAnchor, 1, m_iSelectionAnchor, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
      }

      ColumnLayout *cl = table->nonDefaultColumnLayout( m_pCanvas->markerColumn() );

      resize = QMAX( (int)(2.0* m_pCanvas->zoom()), resize );
      cl->setWidth( resize, m_pCanvas );
    }
    else
    {
      if( makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
      {
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), selection );
        m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
      }
      ColumnLayout *cl;
      for ( int i=selection.left(); i<=selection.right(); i++ )
      {
        cl = table->nonDefaultColumnLayout( i );

        resize = QMAX( (int)(2.0* m_pCanvas->zoom()), resize );
        cl->setWidth( resize, m_pCanvas );
      }
    }
  }
}

void KSpreadHBorder::mouseDoubleClickEvent( QMouseEvent * /*_ev */)
{
  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );
  if(!m_pView->koDocument()->isReadWrite())
    return;
  adjustColumn();
}

void KSpreadHBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();
  assert( table );
  if(!m_pView->koDocument()->isReadWrite())
    return;

  if ( m_bResize )
  {
    paintSizeIndicator( _ev->pos().x(), false );
  }
  else if ( m_bSelection )
  {
    int x = 0;
    int col = table->leftColumn( _ev->pos().x(), x, m_pCanvas );
    if( col > KS_colMax )
	return;
    QRect r = table->selection();

    if ( col < m_iSelectionAnchor )
    {
      r.setLeft( col );
      r.setRight( m_iSelectionAnchor );
    }
    else
    {
      r.setRight( col );
      r.setLeft( m_iSelectionAnchor );
    }
    table->setSelection( r, m_pCanvas );

    if ( _ev->pos().x() < 0 )
      m_pCanvas->horzScrollBar()->setValue( m_pCanvas->xOffset() + x );
    else if ( _ev->pos().x() > m_pCanvas->width() )
    {
      if ( col < KS_colMax )
      {
        ColumnLayout *cl = table->columnLayout( col + 1 );
        x = table->columnPos( col + 1, m_pCanvas );
        m_pCanvas->horzScrollBar()->setValue( m_pCanvas->xOffset() +
                                            ( x + cl->width( m_pCanvas ) - m_pCanvas->width() ) );
      }
    }

  }
  // Perhaps we have to modify the cursor
  else
  {
    //if col is hide and it's the first column
    //you mustn't resize it.
    int tmp2;
    int tmpCol=table->leftColumn( _ev->pos().x() - 1, tmp2, m_pCanvas );

    /* Doesn't work correctly, gets removed if it turns out that the new
       version really works, Norbert
    while ( x < width() )
    {
    int w = table->columnLayout( col )->width( m_pCanvas );

    if ( _ev->pos().x() >= x + w - 1
    && _ev->pos().x() <= x + w + 1
    &&!(table->columnLayout(tmpCol)->isHide() && tmpCol == 1) )
    {
    setCursor(splitHCursor);
    return;
    }
    x += w;
    }
    */

    int xpos = _ev->pos().x();
    if ( ( (table->leftColumn( ( xpos - 1), tmp2, m_pCanvas ) != tmpCol)
           || (table->leftColumn( ( xpos - 1) + 3, tmp2, m_pCanvas ) != tmpCol) )
         && !(table->columnLayout(tmpCol)->isHide() && tmpCol == 1) )
    {
        setCursor(splitHCursor);
        return;
    }

    setCursor( arrowCursor );
  }
}

void KSpreadHBorder::wheelEvent( QWheelEvent* _ev )
{
  if ( m_pCanvas->horzScrollBar() )
    QApplication::sendEvent( m_pCanvas->horzScrollBar(), _ev );
}

void KSpreadHBorder::paintSizeIndicator( int mouseX, bool firstTime )
{
    KSpreadTable *table = m_pCanvas->activeTable();
    assert( table );

    QPainter painter;
    painter.begin( m_pCanvas );
    painter.setRasterOp( NotROP );
    if (!firstTime)
      painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );

    m_iResizePos = mouseX;

    //int twenty = (int)( 2.0 * m_pCanvas->zoom() );
    // Dont make the column have a width < 2 pixels.
    int x = table->columnPos( m_iResizedColumn, m_pCanvas );
    if ( m_iResizePos <= x /*+ twenty*/ )
        m_iResizePos = x /*+ twenty*/;
    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );
    painter.end();
    QString tmpSize;
    if(m_iResizePos !=x)
        tmpSize=i18n("Width: %1 %2").arg(KoUnit::ptToUnit(((m_iResizePos-x)/m_pCanvas->zoom()), m_pView->doc()->getUnit())).arg(m_pView->doc()->getUnitName());
    else
        tmpSize=i18n("Hide Column");
    painter.begin(this);
    int len = painter.fontMetrics().width(tmpSize );
    int hei = painter.fontMetrics().height( );
    painter.end();
    if(!m_lSize)
    {
        m_lSize=new QLabel(m_pCanvas);
        m_lSize->setGeometry(x+3,3,len+2, hei+2 ) ;
        m_lSize->setAlignment(Qt::AlignVCenter);
        m_lSize->setText(tmpSize);
        m_lSize->show();
    }
    else
    {
        m_lSize->setGeometry(x+3,3,len+2, hei+2 ) ;
        m_lSize->setText(tmpSize);
    }
}

void KSpreadHBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadTable *table = m_pCanvas->activeTable();

  if (!table )
    return;

  QPainter painter;
  painter.begin( this );
  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
  painter.setBackgroundColor( white );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  // Determine which columns need painting
  int xpos;
  int left_col = table->leftColumn( _ev->rect().x(), xpos, m_pCanvas );
  int right_col = table->rightColumn( _ev->rect().right(), m_pCanvas );

  QRect selection( table->selection() );

  QFont normalFont = painter.font();
  QFont boldFont = normalFont;
  boldFont.setBold( TRUE );
  KSpreadCell *cell=table->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
  QRect extraCell;
  extraCell.setCoords(m_pCanvas->markerColumn(),m_pCanvas->markerRow(),
  m_pCanvas->markerColumn()+cell->extraXCells(),m_pCanvas->markerRow()+cell->extraYCells());

  //several cells selected but not just a cell merged
  bool area=( selection.left()!=0 && extraCell!=selection );

  for ( int x = left_col; x <= right_col; x++ )
  {
    bool highlighted = ( area && x >= selection.left() && x <= selection.right());
    bool selected = ( highlighted && table->isColumnSelected() && (!table->isRowSelected()) );

    ColumnLayout *col_lay = table->columnLayout( x );

    if ( selected )
    {
      QBrush fillSelected( colorGroup().brush( QColorGroup::Highlight ) );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pCanvas ),
                  XBORDER_HEIGHT, colorGroup(), FALSE, 1, &fillSelected );
    }
    else if ( highlighted )
    {
      QBrush fillHighlighted( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pCanvas ),
                  XBORDER_HEIGHT, colorGroup(), true, 1, &fillHighlighted );
    }
    else
    {
      QBrush fill( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pCanvas ),
                  XBORDER_HEIGHT, colorGroup(), FALSE, 1, &fill );
    }

    // Reset painter
    painter.setFont( normalFont );
    painter.setPen( colorGroup().text() );

    if ( selected )
      painter.setPen( colorGroup().highlightedText() );
    else if ( highlighted )
      painter.setFont( boldFont );
    if(!m_pView->activeTable()->getShowColumnNumber())
        {
        int len = painter.fontMetrics().width( util_encodeColumnLabelText( x ) );
        if(!col_lay->isHide())
                painter.drawText( xpos + ( col_lay->width( m_pCanvas ) - len ) / 2,
                      ( XBORDER_HEIGHT + painter.fontMetrics().ascent() -
                        painter.fontMetrics().descent() ) / 2,
                        util_encodeColumnLabelText( x ) );
        }
    else
        {
        QString tmp;
        int len = painter.fontMetrics().width( tmp.setNum(x) );
        if(!col_lay->isHide())
                painter.drawText( xpos + ( col_lay->width( m_pCanvas ) - len ) / 2,
                      ( XBORDER_HEIGHT + painter.fontMetrics().ascent() -
                        painter.fontMetrics().descent() ) / 2,
                        tmp.setNum(x) );
        }
    xpos += col_lay->width( m_pCanvas );
  }
  m_pCanvas->updatePosWidget();
  painter.end();
}

/****************************************************************
 *
 * KSpreadToolTip
 *
 ****************************************************************/

KSpreadToolTip::KSpreadToolTip( KSpreadCanvas* canvas )
    : QToolTip( canvas ), m_canvas( canvas )
{
}

void KSpreadToolTip::maybeTip( const QPoint& p )
{
    KSpreadTable *table = m_canvas->activeTable();
    if ( !table )
        return;

    // Over which cell is the mouse ?
    int ypos, xpos;
    int row = table->topRow( p.y(), ypos, m_canvas );
    int col = table->leftColumn( p.x(), xpos, m_canvas );
    KSpreadCell* cell = table->visibleCellAt( col, row );
    if ( !cell )
        return;

    // Get the comment
    QString comment= cell->comment(col,row);

    // Determine position and width of the current cell.
    cell = table->cellAt( col, row );
    int u = cell->width( col, m_canvas );

    // Special treatment for obscured cells.
    if ( cell->isObscured() && cell->isObscuringForced() )
    {
      cell = cell->obscuringCells().getFirst();
      int moveX = cell->column();
      int moveY = cell->row();

      // Use the obscuring cells dimensions
      u = cell->width( moveX, m_canvas );
      xpos = table->columnPos( moveX, m_canvas );
      ypos = table->rowPos( moveY, m_canvas );
    }

    // Is the cursor over the comment marker (if there is any) then
    // show the comment.
    QRect marker( xpos + u - 10, ypos, 10, 10 );
    if ( marker.contains( p ) )
    {
        tip( marker, comment );
    }
}

#include "kspread_canvas.moc"
