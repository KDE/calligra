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
#include <float.h>

KSpreadLocationEditWidget::KSpreadLocationEditWidget( QWidget * _parent,
                                                      KSpreadView * _view )
    : QLineEdit( _parent, "KSpreadLocationEditWidget" ),
      m_pView(_view)
{
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
            QString ltext = text();
            QString tmp = ltext.lower();
            QValueList<Reference>::Iterator it;
	    QValueList<Reference> area = m_pView->doc()->listArea();
	    for ( it = area.begin(); it != area.end(); ++it )
            {
                if ((*it).ref_name == tmp)
                {
                    QString tmp = (*it).table_name;
                    tmp += "!";
                    tmp += util_rangeName((*it).rect);
                    m_pView->canvasWidget()->gotoLocation( KSpreadRange(tmp, m_pView->doc()->map()));
                    return;
                }
            }

            // Set the cell component to uppercase:
            // Table1!a1 -> Table1!A2
            int pos = ltext.find('!');
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
                    QRect rect( m_pView->selection() );
                    KSpreadSheet * t = m_pView->activeTable();
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
        if ( m_pView->selectionInfo()->singleCellSelection() ) {
            setText( util_encodeColumnLabelText( m_pView->canvasWidget()->markerColumn() )
                     + QString::number( m_pView->canvasWidget()->markerRow() ) );
        } else {
            setText( util_encodeColumnLabelText( m_pView->selection().left() )
                     + QString::number( m_pView->selection().top() )
                     + ":"
                     + util_encodeColumnLabelText( m_pView->selection().right() )
                     + QString::number( m_pView->selection().bottom() ) );
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
  Q_ASSERT(m_pCanvas != NULL);
  // Those buttons are created by the caller, so that they are inserted
  // properly in the layout - but they are then managed here.
  m_pCancelButton = cancelButton;
  m_pOkButton = okButton;

  installEventFilter(m_pCanvas);

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
    if ( ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
         || ( _ev->state() & Qt::ShiftButton )
         || ( _ev->key() == Key_Shift ) )
    {
        QLineEdit::keyPressEvent( _ev );
        _ev->accept();
        return;
    }

  if ( !m_pCanvas->doc()->isReadWrite() )
    return;

  if ( !m_pCanvas->editor() )
  {
    // Start editing the current cell
    m_pCanvas->createEditor( KSpreadCanvas::CellEditor,false );
  }
  KSpreadTextEditor * cellEditor = (KSpreadTextEditor*) m_pCanvas->editor();

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
  m_chooseStartTable = NULL;
  m_pEditor = 0;
  m_bChoose = FALSE;

  QWidget::setFocusPolicy( QWidget::StrongFocus );

  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );

  m_dXOffset = 0.0;
  m_dYOffset = 0.0;
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
  /* this canvas event filter acts on events sent to the line edit as well
     as events to this filter itself.
  */
  if ( !o || !e )
    return TRUE;
  switch ( e->type() )
  {
  case QEvent::KeyPress:
  {
    QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
    if (keyev->key()==Key_Tab)
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

QRect KSpreadCanvas::selection() const
{
  return m_pView->selectionInfo()->selection();
}

QPoint KSpreadCanvas::marker() const
{
    return m_pView->selectionInfo()->marker();
}

int KSpreadCanvas::markerColumn() const
{
    return m_pView->selectionInfo()->marker().x();
}

int KSpreadCanvas::markerRow() const
{
    return m_pView->selectionInfo()->marker().y();
}

void KSpreadCanvas::startChoose()
{
  if ( m_bChoose )
    return;

  updateChooseRect(QPoint(0,0), QPoint(0,0));

  // It is important to enable this AFTER we set the rect!
  m_bChoose = TRUE;
  m_chooseStartTable = activeTable();
}

void KSpreadCanvas::startChoose( const QRect& rect )
{
  if (m_bChoose)
    return;

  updateChooseRect(rect.bottomRight(), rect.topLeft());

  // It is important to enable this AFTER we set the rect!
  m_bChoose = TRUE;
  m_chooseStartTable = activeTable();
}

void KSpreadCanvas::endChoose()
{
  if ( !m_bChoose )
    return;

  updateChooseRect(QPoint(0,0), QPoint(0,0));

  KSpreadSheet *table=m_pView->doc()->map()->findTable(m_chooseStartTable->tableName());
  if(table)
        m_pView->setActiveTable(table);

  length_namecell = 0;
  m_bChoose = FALSE;
  m_chooseStartTable = 0;
}

KSpreadHBorder* KSpreadCanvas::hBorderWidget() const
{
  return m_pView->hBorderWidget();
}

KSpreadVBorder* KSpreadCanvas::vBorderWidget() const
{
  return m_pView->vBorderWidget();
}

QScrollBar* KSpreadCanvas::horzScrollBar() const
{
  return m_pView->horzScrollBar();
}

QScrollBar* KSpreadCanvas::vertScrollBar() const
{
  return m_pView->vertScrollBar();
}

KSpreadSheet* KSpreadCanvas::findTable( const QString& _name ) const
{
  return m_pDoc->map()->findTable( _name );
}

KSpreadSheet* KSpreadCanvas::activeTable() const
{
  return m_pView->activeTable();
}

bool KSpreadCanvas::gotoLocation( const KSpreadRange & _range )
{
  if ( !_range.isValid() )
  {
    KMessageBox::error( this, i18n( "Invalid cell reference" ) );
    return false;
  }
  KSpreadSheet * table = activeTable();
  if ( _range.isTableKnown() )
    table = _range.table;
  if ( !table )
  {
    KMessageBox::error( this, i18n("Unknown table name %1" ).arg( _range.tableName ) );
    return false;
  }

  gotoLocation( _range.range.topLeft(), table, false );
  gotoLocation( _range.range.bottomRight(), table, true );
  return true;
}


bool KSpreadCanvas::gotoLocation( const KSpreadPoint& _cell )
{
  if ( !_cell.isValid() )
  {
    KMessageBox::error( this, i18n("Invalid cell reference") );
    return false;
  }

  KSpreadSheet* table = activeTable();
  if ( _cell.isTableKnown() )
    table = _cell.table;
  if ( !table )
  {
    KMessageBox::error( this, i18n("Unknown table name %1").arg( _cell.tableName ) );
    return false;
  }

  gotoLocation( _cell.pos, table );
  return true;
}

void KSpreadCanvas::gotoLocation( QPoint location, KSpreadSheet* table,
                                  bool extendSelection)
{
  if ( table && (table != activeTable() ))
    m_pView->setActiveTable(table);
  else
    table = activeTable();

  if (extendSelection)
  {
    extendCurrentSelection(location);
  }
  else
  {
    QRect newSelection;
    QPoint topLeft(location);
    QPoint bottomRight;
    KSpreadCell* cell = table->cellAt(location);
    if ( cell->isObscured() && cell->isObscuringForced() )
    {
      cell = cell->obscuringCells().first();
      topLeft = QPoint(cell->column(), cell->row());
    }
    bottomRight.setX(topLeft.x() + cell->extraXCells());
    bottomRight.setY(topLeft.y() + cell->extraYCells());
    newSelection = QRect(topLeft, bottomRight);

    if (m_bChoose)
    {
      updateChooseRect(topLeft, topLeft);
    }
    else
    {
      /* anchor and marker should be on the same cell here */
      selectionInfo()->setSelection(topLeft, topLeft, table);
    }
  }

  scrollToCell(location);

  // Perhaps the user is entering a value in the cell.
  // In this case we may not touch the EditWidget
  if ( !m_pEditor && !m_bChoose )
    m_pView->updateEditWidgetOnPress();

  updatePosWidget();

}


void KSpreadCanvas::scrollToCell(QPoint location)
{
  KSpreadSheet* table = activeTable();
  if (table == NULL)
    return;

  /* we don't need this cell ptr, but this call is necessary to update the
     scroll bar correctly.  I don't like having that as part of the cellAt function
     but I suppose that's ok for now.
  */
  KSpreadCell* cell = table->cellAt(location.x(), location.y(), true);
  Q_UNUSED(cell);

  double xpos = table->dblColumnPos( location.x(), this );
  double ypos = table->dblRowPos( location.y(), this );

  //kdDebug(36001) << "KSpreadCanvas::gotoLocation : zoom=" << zoom() << endl;
  int minX = (int) (100 * zoom()); // less than that, we scroll
  int minY = (int) (50 * zoom());
  int maxX = (int) (width() - 100 * zoom()); // more than that, we scroll
  int maxY = (int) (height() - 50 * zoom());
  //kdDebug(36001) << "KSpreadCanvas::gotoLocation : height=" << height() << endl;
  //kdDebug(36001) << "KSpreadCanvas::gotoLocation : width=" << width() << endl;

  // do we need to scroll left
  if ( xpos < minX )
    horzScrollBar()->setValue( int( xOffset() + xpos ) - minX );

  //do we need to scroll right
  else if ( xpos > maxX )
  {
    double horzScrollBarValue = xOffset() + xpos - maxX;
    double horzScrollBarValueMax = table->sizeMaxX() * zoom() - width();

    //We don't want to display any area > KS_colMax widths
    if ( horzScrollBarValue > horzScrollBarValueMax )
      horzScrollBarValue = horzScrollBarValueMax;

    horzScrollBar()->setValue( int( horzScrollBarValue ) );
  }

  // do we need to scroll up
  if ( ypos < minY )
    vertScrollBar()->setValue( int( yOffset() + ypos - minY ) );

  // do we need to scroll down
  else if ( ypos > maxY )
  {
    double vertScrollBarValue = yOffset() + ypos - maxY;
    double vertScrollBarValueMax = table->sizeMaxY() * zoom() - height();

    //We don't want to display any area > KS_rowMax heights
    if ( vertScrollBarValue > vertScrollBarValueMax )
      vertScrollBarValue = vertScrollBarValueMax;

    vertScrollBar()->setValue( int( vertScrollBarValue ) );
  }
}


void KSpreadCanvas::highlight( const QString &/*text*/, int /*matchingIndex*/, int /*matchedLength*/, const QRect &cellRect )
{
    // Which cell was this again?
    //KSpreadCell *cell = cellAt( cellRect.left(), cellRect.top() );

    // ...now I remember, update it!
    // TBD: highlight it!
    gotoLocation( cellRect.topLeft(), activeTable() );
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

  double xpos = activeTable()->dblColumnPos( QMIN( KS_colMax, m_pView->activeTable()->maxColumn()+10 ), this );
  if( _value > ( xpos + m_dXOffset ) )
      _value = int( xpos + m_dXOffset );

  activeTable()->enableScrollBarUpdates( false );

  // Relative movement
  int dx = int( m_dXOffset ) - _value;
  // New absolute position
  m_dXOffset = _value;

  scroll( dx, 0 );

  hBorderWidget()->scroll( dx, 0 );

  activeTable()->enableScrollBarUpdates( true );

}

void KSpreadCanvas::slotScrollVert( int _value )
{
  if ( activeTable() == 0L )
    return;

  if ( _value < 0 )
  {
    _value = 0;
    kdDebug (36001) << "KSpreadCanvas::slotScrollVert: value out of range (_value: " << _value << ")" << endl;
  }

  double ypos = activeTable()->dblRowPos( QMIN( KS_rowMax, m_pView->activeTable()->maxRow()+10 ), this );
  if( _value > ypos + m_dYOffset )
      _value = int( ypos + m_dYOffset );

  activeTable()->enableScrollBarUpdates( false );

  // Relative movement
  int dy = int( m_dYOffset - _value );
  // New absolute position
  m_dYOffset = _value;
  scroll( 0, dy );
  vBorderWidget()->scroll( 0, dy );

  activeTable()->enableScrollBarUpdates( true );
}

void KSpreadCanvas::slotMaxColumn( int _max_column )
{
  double xpos = activeTable()->dblColumnPos( QMIN( KS_colMax, _max_column + 10 ), this );

  //Don't go beyond the maximum column range (KS_colMax)
  double sizeMaxX = activeTable()->sizeMaxX() * zoom();
  if ( xpos > sizeMaxX - xOffset() - width() )
    xpos = sizeMaxX - xOffset() - width();

  horzScrollBar()->setRange( 0, int( xpos + xOffset() ) );
}

void KSpreadCanvas::slotMaxRow( int _max_row )
{
  double ypos = activeTable()->dblRowPos( QMIN( KS_rowMax, _max_row + 10 ), this );

  //Don't go beyond the maximum row range (KS_rowMax)
  double sizeMaxY = activeTable()->sizeMaxY() * zoom();
  if ( ypos > sizeMaxY - yOffset() - height() )
    ypos = sizeMaxY - yOffset() - height();

  vertScrollBar()->setRange( 0, int( ypos + yOffset() ) );
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
  KSpreadSheet *table = activeTable();
  if ( !table )
    return;

  double xpos;
  double ypos;
  int row  = table->topRow( _ev->pos().y(), ypos, this );
  int col  = table->leftColumn( _ev->pos().x(), xpos, this );

  if( col > KS_colMax || row > KS_rowMax )
  {
    return;
  }

  QRect selectionHandle = m_pView->selectionInfo()->selectionHandleArea(this);

  // Test whether the mouse is over some anchor
  {
    KSpreadCell *cell = table->visibleCellAt( col, row );
    QString anchor = cell->testAnchor( _ev->pos().x() - int( xpos ),
                                       _ev->pos().y() - int( ypos ) );
    if ( !anchor.isEmpty() && anchor != m_strAnchor )
      setCursor( KCursor::handCursor() );
    m_strAnchor = anchor;
  }

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
  gotoLocation(QPoint(col, row), table, true);
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

  KSpreadSheet *table = activeTable();
  if ( !table )
    return;

  KSpreadSelection* selectionInfo = m_pView->selectionInfo();
  QRect selection( selection() );

  if (selectionInfo->singleCellSelection())
  {
    KSpreadCell* cell = table->cellAt(selectionInfo->marker());
    cell->clicked(this);
  }

  // The user started the drag in the lower right corner of the marker ?
  if ( m_eMouseAction == ResizeCell )
  {
    QPoint selectionAnchor = selectionInfo->selectionAnchor();
    int x = selectionAnchor.x();
    int y = selectionAnchor.y();
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
    QRect dest = selection;
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
  // Auto fill ? That is done using the left mouse button.
  if ( event->button() == LeftButton )
  {
    m_eMouseAction = AutoFill;
    m_rctAutoFillSrc = selection();
  }
  // Resize a cell (done with the right mouse button) ?
  // But for that to work there must not be a selection.
  else if ( event->button() == MidButton && selectionInfo()->singleCellSelection())
  {
    m_eMouseAction = ResizeCell;
  }

  return;
}


void KSpreadCanvas::extendCurrentSelection(QPoint cell)
{
  KSpreadSheet* table = activeTable();
  QPoint chooseAnchor = selectionInfo()->getChooseAnchor();
  QRect newCellArea;
//  KSpreadCell* destinationCell = table->cellAt(cell);

  newCellArea = QRect(cell, cell);

  if (m_bChoose)
  {
    if (chooseAnchor.x() == 0)
    {
      updateChooseRect(cell, cell);
    }
    else
    {
      updateChooseRect(cell, chooseAnchor);
    }
  }
  else
  {

    /* the selection simply becomes a box with the anchor and given cell as
       opposite corners
    */
    selectionInfo()->setSelection(cell, selectionInfo()->selectionAnchor(),
                                  table);
  }
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

  KSpreadSheet *table = activeTable();

  if ( !table )
    return;

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pEditor )
  {
    deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  // Remember current values.
  QRect selection( selection() );

  // Did we click in the lower right corner of the marker/marked-area ?
  if (selectionInfo()->selectionHandleArea(this).contains(_ev->pos()))
  {
    processClickSelectionHandle(_ev);
    return;
  }

  // In which cell did the user click ?
  double xpos, ypos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  //you cannot move marker when col > KS_colMax or row > KS_rowMax
  if( col > KS_colMax || row > KS_rowMax){
    kdDebug(36001) << "KSpreadCanvas::mousePressEvent: col or row is out of range: col: " << col << " row: " << row << endl;
    return;
  }

  // Extending an existing selection with the shift button ?
  if ( m_pView->koDocument()->isReadWrite() && selection.right() != KS_colMax &&
       selection.bottom() != KS_rowMax && _ev->state() & ShiftButton )
  {
    gotoLocation(QPoint(col, row), activeTable(), true);
    return;
  }

  KSpreadCell *cell = table->cellAt( col, row );

  // Go to the upper left corner of the obscuring object if cells are merged
  if (cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
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
    gotoLocation(QPoint(col, row), activeTable(), false);
  }
  else if ( _ev->button() == RightButton &&
            !selection.contains(QPoint(col, row)) )
  {
    // No selection or the mouse press was outside of an existing selection ?
    gotoLocation(QPoint(col, row), activeTable(), false);
  }

  // Paste operation with the middle button ?
  if( _ev->button() == MidButton )
  {
      if ( m_pView->koDocument()->isReadWrite() )
      {
          selectionInfo()->setMarker( QPoint( col, row ), table );
          table->paste( QRect(marker(), marker()) );
          table->cellAt( marker() )->update();
      }
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

  KSpreadSheet *table = activeTable();
  if ( !table )
    return;

  double ypos, xpos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  if( col > KS_colMax || row > KS_rowMax )
  {
    return;
  }

  QPoint chooseMarker = selectionInfo()->getChooseMarker();

  // Nothing changed ?
  if (row == chooseMarker.y() && col == chooseMarker.x())
  {
    return;
  }

  gotoLocation(QPoint(col, row), table, ( m_eMouseAction != NoAction));
}

void KSpreadCanvas::chooseMouseReleaseEvent( QMouseEvent* )
{
    // gets done in mouseReleaseEvent
    //  m_bMousePressed = FALSE;
  m_eMouseAction = NoAction;
}

void KSpreadCanvas::chooseMousePressEvent( QMouseEvent * _ev )
{
  KSpreadSheet *table = activeTable();
  if ( !table )
    return;


  double ypos, xpos;
  int row = table->topRow( _ev->pos().y(), ypos, this );
  int col = table->leftColumn( _ev->pos().x(), xpos, this );

  if( col > KS_colMax || row > KS_rowMax )
  {
    return;
  }

  bool extend = ( ( (!util_isColumnSelected(selection())) &&
                    (!util_isRowSelected(selection()))) &&
                  ( _ev->state() & ShiftButton ) );

  gotoLocation(QPoint(col, row), activeTable(), extend);

  if ( _ev->button() == LeftButton )
  {
    m_eMouseAction = Mark;
  }
  return;
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

  const KSpreadSheet* table = activeTable();
  if ( !table )
    return;

  // printf("PAINT EVENT %i %i %i %i\n", _ev->rect().x(), _ev->rect().y(), _ev->rect().width(), _ev->rect().height() );

  QRect rect = _ev->rect() & QWidget::rect();

  // printf("PAINT EVENT %i %i %i %i\n", rect.x(), rect.y(), rect.width(), rect.height() );



  QWMatrix m = m_pView->matrix();
  m = m.invert();
  QPoint tl = m.map( rect.topLeft() );
  QPoint br = m.map( rect.bottomRight() );

  //kdDebug(36001) << "Mapped topleft to " << tl.x() << ":" << tl.y() << endl;

  double xpos, ypos;
  int left_col = table->leftColumn( tl.x(), xpos );
  int right_col = table->rightColumn( br.x() );
  int top_row = table->topRow( tl.y(), ypos );
  int bottom_row = table->bottomRow( br.y() );

  updateCellRect(QRect(left_col, top_row, right_col - left_col + 1,
                       bottom_row - top_row + 1));
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
    if ( _ev->size().width() > _ev->oldSize().width() )
    {
        if ( ( xOffset() + _ev->size().width() ) >
               activeTable()->sizeMaxX() * zoom() )
        {
            horzScrollBar()->setRange( 0, int( activeTable()->sizeMaxX() * zoom() ) - _ev->size().width() );
        }
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().width() < _ev->oldSize().width() )
    {
        if ( horzScrollBar()->maxValue() ==
             ( int( activeTable()->sizeMaxX() * zoom() ) - _ev->oldSize().width() ) )
        {
            horzScrollBar()->setRange( 0, int( activeTable()->sizeMaxX() * zoom() ) - _ev->size().width() );
        }
    }

    // If we rise vertically, then check if we are still within the valid area (KS_rowMax)
    if ( _ev->size().height() > _ev->oldSize().height() )
    {
        if ( ( yOffset() + _ev->size().height() ) >
             activeTable()->sizeMaxY() * zoom() )
        {
            vertScrollBar()->setRange( 0, int( activeTable()->sizeMaxY() * zoom() ) - _ev->size().height() );
        }
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().height() < _ev->oldSize().height() )
    {
        if ( vertScrollBar()->maxValue() ==
             ( int( activeTable()->sizeMaxY() * zoom() ) - _ev->oldSize().height() ) )
        {
            vertScrollBar()->setRange( 0, int( activeTable()->sizeMaxY() * zoom() ) - _ev->size().height() );
        }
    }
}

void KSpreadCanvas::moveDirection(KSpread::MoveTo direction, bool extendSelection)
{
  QPoint destination;
  QPoint cursor;

  if (m_bChoose)
  {
    cursor = selectionInfo()->getChooseCursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
    {
      cursor = selectionInfo()->cursorPosition();
    }
  }
  else
  {
    cursor = selectionInfo()->cursorPosition();
  }

  QPoint cellCorner = cursor;
  KSpreadCell* cell = activeTable()->cellAt(cursor.x(), cursor.y());

  /* cell is either the same as the marker, or the cell that is forced obscuring
     the marker cell
  */
  if (cell->isObscuringForced())
  {
    cell = cell->obscuringCells().first();
    cellCorner = QPoint(cell->column(), cell->row());
  }

  /* how many cells must we move to get to the next cell? */
  int offset = 0;
  RowLayout *rl = NULL;
  ColumnLayout *cl = NULL;
  switch (direction)
    /* for each case, figure out how far away the next cell is and then keep
       going one row/col at a time after that until a visible row/col is found

       NEVER use cell->column() or cell->row() -- it might be a default cell
    */
  {
    case KSpread::Bottom:
      offset = cell->mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = activeTable()->rowLayout( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHide())
      {
        offset++;
        rl = activeTable()->rowLayout( cursor.y() + offset );
      }

      destination = QPoint(cursor.x(), QMIN(cursor.y() + offset, KS_rowMax));
      break;
    case KSpread::Top:
      offset = (cellCorner.y() - cursor.y()) - 1;
      rl = activeTable()->rowLayout( cursor.y() + offset );
      while ( ((cursor.y() + offset) >= 1) && rl->isHide())
      {
        offset--;
        rl = activeTable()->rowLayout( cursor.y() + offset );
      }
      destination = QPoint(cursor.x(), QMAX(cursor.y() + offset, 1));
      break;
    case KSpread::Left:
      offset = (cellCorner.x() - cursor.x()) - 1;
      cl = activeTable()->columnLayout( cursor.x() + offset );
      while ( ((cursor.x() + offset) >= 1) && cl->isHide())
      {
        offset--;
        cl = activeTable()->columnLayout( cursor.x() + offset );
      }
      destination = QPoint(QMAX(cursor.x() + offset, 1), cursor.y());
      break;
    case KSpread::Right:
      offset = cell->mergedXCells() - (cursor.x() - cellCorner.x()) + 1;
      cl = activeTable()->columnLayout( cursor.x() + offset );
      while ( ((cursor.x() + offset) <= KS_colMax) && cl->isHide())
      {
        offset++;
        cl = activeTable()->columnLayout( cursor.x() + offset );
      }
      destination = QPoint(QMIN(cursor.x() + offset, KS_colMax), cursor.y());
      break;
  }

  gotoLocation(destination, activeTable(), extendSelection);
  if (m_bChoose)
  {
    selectionInfo()->setChooseCursor(activeTable(), destination);
  }
  else
  {
    selectionInfo()->setCursorPosition(destination);
  }
  m_pView->updateEditWidget();
}

void KSpreadCanvas::processEnterKey(QKeyEvent* event)
{
  /* save changes to the current editor */
  if (!m_bChoose)
  {
    deleteEditor( true );
  }

  /* use the configuration setting to see which direction we're supposed to move
     when enter is pressed.
  */
  KSpread::MoveTo direction = m_pView->doc()->getMoveToValue();

  //if shift Button clicked inverse move direction
  if(event->state() & Qt::ShiftButton)
  {
    switch( direction )
    {
    case KSpread::Bottom:
      direction = KSpread::Top;
      break;
    case KSpread::Top:
      direction = KSpread::Bottom;
      break;
    case KSpread::Left:
      direction = KSpread::Right;
      break;
    case KSpread::Right:
      direction = KSpread::Left;
    }
  }

  /* never extend a selection with the enter key -- the shift key reverses
     direction, not extends the selection
  */
    moveDirection(direction, false);
  return;
}

void KSpreadCanvas::processArrowKey( QKeyEvent *event)
{
  /* NOTE:  hitting the tab key also calls this function.  Don't forget
     to account for it
  */

  /* save changes to the current editor */
  if (!m_bChoose)
  {
    deleteEditor( true );
  }

  KSpread::MoveTo direction = KSpread::Bottom;
  bool makingSelection = event->state() & ShiftButton;

  switch (event->key())
  {
  case Key_Down:
    direction = KSpread::Bottom;
    break;
  case Key_Up:
    direction = KSpread::Top;
    break;
  case Key_Left:
    direction = KSpread::Left;
    break;
  case Key_Right:
  case Key_Tab:
    direction = KSpread::Right;
    break;
  default:
    Q_ASSERT(false);
    break;
  }

  moveDirection(direction, makingSelection);
}

void KSpreadCanvas::processEscapeKey(QKeyEvent * event)
{
  if ( m_pEditor )
    deleteEditor( false );

  event->accept(); // ?
}

void KSpreadCanvas::processHomeKey(QKeyEvent* event)
{
  bool makingSelection = event->state() & ShiftButton;
  KSpreadSheet* table = activeTable();

  if ( m_pEditor )
  // We are in edit mode -> go beginning of line
  {
    // (David) Do this for text editor only, not formula editor...
    // Don't know how to avoid this hack (member var for editor type ?)
    if ( m_pEditor->inherits("KSpreadTextEditor") )
      QApplication::sendEvent( m_pEditWidget, event );
    // What to do for a formula editor ?
  }
  else
  {
    QPoint destination;
    /* start at the first used cell in the row and cycle through the right until
       we find a cell that has some output text.  But don't look past the current
       marker.
       The end result we want is to move to the left to the first cell with text,
       or just to the first column if there is no more text to the left.

       But why?  In excel, home key sends you to the first column always.
       We might want to change to that behavior.
    */

    if (event->state() & ControlButton)
    {
      /* ctrl + Home will always just send us to location (1,1) */
      destination = QPoint(1,1);
    }
    else
    {
      QPoint marker = m_bChoose ?
        selectionInfo()->getChooseMarker() : selectionInfo()->marker();

      KSpreadCell * cell = table->getFirstCellRow(marker.y());
      while (cell != NULL && cell->column() < marker.x() && cell->isEmpty())
      {
        cell = table->getNextCellRight(cell->column(), cell->row());
      }

      int col = ( cell ? cell->column() : 1 );
      if ( col == marker.x())
        col = 1;
      destination = QPoint(col, marker.y());
    }

    gotoLocation(destination, activeTable(), makingSelection);
  }
  return;
}

void KSpreadCanvas::processEndKey( QKeyEvent *event )
{
  bool makingSelection = event->state() & ShiftButton;
  KSpreadSheet* table = activeTable();
  KSpreadCell* cell = NULL;


  // move to the last used cell in the row
  // We are in edit mode -> go beginning of line
  if ( m_pEditor )
  {
    // (David) Do this for text editor only, not formula editor...
    // Don't know how to avoid this hack (member var for editor type ?)
    if ( m_pEditor->inherits("KSpreadTextEditor") )
      QApplication::sendEvent( m_pEditWidget, event );
    // TODO: What to do for a formula editor ?
  }
  else
  {
    QPoint marker = m_bChoose ?
      selectionInfo()->getChooseMarker() : selectionInfo()->marker();

    int col = 1;

    cell = table->getLastCellRow(marker.y());
    while (cell != NULL && cell->column() > markerColumn() && cell->isEmpty())
    {
      cell = table->getNextCellLeft(cell->column(), cell->row());
    }

    col = (cell == NULL) ? KS_colMax : cell->column();

    gotoLocation(QPoint(col, marker.y()), activeTable(), makingSelection);
  }
}

void KSpreadCanvas::processPriorKey(QKeyEvent *event)
{
  bool makingSelection = event->state() & ShiftButton;
  if (!m_bChoose)
  {
    deleteEditor( true );
  }

  QPoint marker = m_bChoose ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();

  QPoint destination(marker.x(), QMAX(1, marker.y() - 10));

  gotoLocation(destination, activeTable(), makingSelection);

  return;
}

void KSpreadCanvas::processNextKey(QKeyEvent *event)
{
  bool makingSelection = event->state() & ShiftButton;

  if (!m_bChoose)
  {
    deleteEditor( true /*save changes*/ );
  }

  QPoint marker = m_bChoose ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();
  QPoint destination(marker.x(), QMAX(1, marker.y() + 10));

  gotoLocation(destination, activeTable(), makingSelection);

  return;
}

void KSpreadCanvas::processDeleteKey(QKeyEvent* /* event */)
{
  activeTable()->clearTextSelection( selectionInfo() );
  m_pView->editWidget()->setText( "" );
  return;
}

void KSpreadCanvas::processF2Key(QKeyEvent* /* event */)
{
  m_pView->editWidget()->setFocus();
  if(m_pEditor)
    m_pView->editWidget()->setCursorPosition(m_pEditor->cursorPosition()-1);
  m_pView->editWidget()->cursorForward(false);
  return;
}

void KSpreadCanvas::processF4Key(QKeyEvent* event)
{
  /* I have no idea what this is doing.  But it was in the code so I'm leaving it
   */
  if (m_pEditor)
  {
    m_pEditor->handleKeyPressEvent( event );
    event->accept();
  }
  return;
}

void KSpreadCanvas::processOtherKey(QKeyEvent *event)
{
  // No null character ...
  if ( event->text().isEmpty() || !m_pView->koDocument()->isReadWrite() )
  {
    event->accept();
    return;
  }

  if ( !m_pEditor && !m_bChoose )
  {
    // Switch to editing mode
    createEditor( CellEditor );
    m_pEditor->handleKeyPressEvent( event );
  }
  else if ( m_pEditor )
    m_pEditor->handleKeyPressEvent( event );

  return;

}

void KSpreadCanvas::processControlArrowKey( QKeyEvent *event )
{
  bool makingSelection = event->state() & ShiftButton;

  KSpreadSheet* table = activeTable();
  KSpreadCell* cell = NULL;
  KSpreadCell* lastCell;
  QPoint destination;
  bool searchThroughEmpty = TRUE;
  int row;
  int col;

  QPoint marker = m_bChoose ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();

  /* here, we want to move to the first or last cell in the given direction that is
     actually being used.  Ignore empty cells and cells on hidden rows/columns */
  switch(event->key())
  {

  //Ctrl+Key_Up
  case Key_Up:

    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.y() != 1))
    {
      lastCell = cell;
      row = marker.y()-1;
      cell = table->cellAt(cell->column(), row);
      while((cell != NULL) && (row > 1) && (!cell->isEmpty()) )
      {
        if(!(table->rowLayout(cell->row())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = FALSE;
        }
        row--;
        cell = table->cellAt(cell->column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = table->getNextCellUp(marker.x(), marker.y());

      while((cell != NULL) &&
            (cell->isEmpty() || (table->rowLayout(cell->row())->isHide())))
      {
        cell = table->getNextCellUp(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      row = 1;
    else
      row = cell->row();

    while ( table->rowLayout(row)->isHide() )
    {
      row++;
    }

    destination.setX(marker.x());
    destination.setY(row);
    break;

  //Ctrl+Key_Down
  case Key_Down:

    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.y() != KS_rowMax))
    {
      lastCell = cell;
      row = marker.y()+1;
      cell = table->cellAt(cell->column(), row);
      while((cell != NULL) && (row < KS_rowMax) && (!cell->isEmpty()) )
      {
        if(!(table->rowLayout(cell->row())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = FALSE;
        }
        row++;
        cell = table->cellAt(cell->column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = table->getNextCellDown(marker.x(), marker.y());

      while((cell != NULL) &&
            (cell->isEmpty() || (table->rowLayout(cell->row())->isHide())))
      {
        cell = table->getNextCellDown(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      row = KS_rowMax;
    else
      row = cell->row();

    while ( table->rowLayout(row)->isHide() )
    {
      row--;
    }

    destination.setX(marker.x());
    destination.setY(row);
    break;

  //Ctrl+Key_Left
  case Key_Left:

    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = table->cellAt(col, cell->row());
      while((cell != NULL) && (col > 1) && (!cell->isEmpty()) )
      {
        if(!(table->columnLayout(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = FALSE;
        }
        col--;
        cell = table->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = table->getNextCellLeft(marker.x(), marker.y());

      while((cell != NULL) &&
            (cell->isEmpty() || (table->columnLayout(cell->column())->isHide())))
      {
        cell = table->getNextCellLeft(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = 1;
    else
      col = cell->column();

    while ( table->columnLayout(col)->isHide() )
    {
      col++;
    }

    destination.setX(col);
    destination.setY(marker.y());
    break;

  //Ctrl+Key_Right
  case Key_Right:

    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = table->cellAt(col, cell->row());
      while((cell != NULL) && (col < KS_colMax) && (!cell->isEmpty()) )
      {
        if(!(table->columnLayout(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = FALSE;
        }
        col++;
        cell = table->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = table->getNextCellRight(marker.x(), marker.y());

      while((cell != NULL) &&
            (cell->isEmpty() || (table->columnLayout(cell->column())->isHide())))
      {
        cell = table->getNextCellRight(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = KS_colMax;
    else
      col = cell->column();

    while ( table->columnLayout(col)->isHide() )
    {
      col--;
    }

    destination.setX(col);
    destination.setY(marker.y());
    break;

  }

  gotoLocation(destination, table, makingSelection);
  return;
}


void KSpreadCanvas::keyPressEvent ( QKeyEvent * _ev )
{
  KSpreadSheet * table = activeTable();

  if ( !table || formatKeyPress( _ev ))
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

  switch( _ev->key() )
  {
  case Key_Return:
  case Key_Enter:
    processEnterKey( _ev );
    break;;
  case Key_Down:
  case Key_Up:
  case Key_Left:
  case Key_Right:
  case Key_Tab: /* a tab behaves just like a right arrow */
    if (_ev->state() & ControlButton)
    {
      processControlArrowKey( _ev );
    }
    else
    {
      processArrowKey( _ev );
    }
    break;
  case Key_Escape:
    processEscapeKey( _ev );
    break;

  case Key_Home:
    processHomeKey( _ev );
    break;

  case Key_End:
    processEndKey( _ev );
    break;

  case Key_Prior:  /* Page Up */
    processPriorKey( _ev );
    break;

  case Key_Next:   /* Page Down */
    processNextKey( _ev );
    break;

  case Key_Delete:
    processDeleteKey( _ev );
    break;

  case Key_F2:
    processF2Key( _ev );
    break;

  case Key_F4:
    processF4Key( _ev );
    break;

  default:
    processOtherKey( _ev );
    break;
  }
  return;
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
  KSpreadSheet * table = activeTable();
  QRect rect = selection();

  int right  = rect.right();
  int bottom = rect.bottom();

  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    QString dummy;
    KSpreadUndoCellLayout * undo = new KSpreadUndoCellLayout( m_pDoc, table, rect, dummy );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }

  if ( util_isRowSelected(selection()) )
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

  if ( util_isColumnSelected(selection()) )
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
    if ( !m_bMousePressed )
    {
        m_scrollTimer->stop();
        return;
    }

    bool select = false;
    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    //The switch defines progressive scrolling. The farer you out, the more steps you scroll
    if ( pos.y() < 0 )
    {
        int step;
        switch( - pos.y() / 20 )
        {
            case 0: step = 5;
                    break;
            case 1: step = 20;
                    break;
            case 2: step = height();
                    break;
            case 3: step = height();
                    break;
            default: step = height() * 5;
        }
        vertScrollBar()->setValue( vertScrollBar()->value() - step );
        select = true;
    }
    else if ( pos.y() > height() )
    {
        int step;
        switch( ( pos.y() - height() ) / 20 )
        {
            case 0: step = 5;
                    break;
            case 1: step = 20;
                    break;
            case 2: step = height();
                    break;
            case 3: step = height();
                    break;
            default: step = height() * 5;
        }
        vertScrollBar()->setValue( vertScrollBar()->value() + step );
        select = true;
    }

    if ( pos.x() < 0 )
    {
        int step;
        switch( - pos.x() / 20 )
        {
            case 0: step = 5;
                    break;
            case 1: step = 20;
                    break;
            case 2: step = width();
                    break;
            case 3: step = width();
                    break;
            default: step = width() * 5;
        }
        horzScrollBar()->setValue( horzScrollBar()->value() - step );
        select = true;
    }
    else if ( pos.x() > width() )
    {
        int step;
        switch( ( pos.x() - width() ) / 20 )
        {
            case 0: step = 5;
                    break;
            case 1: step = 20;
                    break;
            case 2: step = width();
                    break;
            case 3: step = width();
                    break;
            default: step = width() * 5;
        }
        horzScrollBar()->setValue( horzScrollBar()->value() + step );
        select = true;
    }

    if (select)
    {
        QMouseEvent * event = new QMouseEvent(QEvent::MouseMove, pos, 0, 0);
        mouseMoveEvent(event);
        delete event;
    }

    //Restart timer
    m_scrollTimer->start( 50 );
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
  {
      if ( t.at(0)=='=' )
      {
          //a formula
          int openParenthese = t.contains('(' );
          int closeParenthese = t.contains(')' );
          int diff = QABS( openParenthese - closeParenthese );
          if ( openParenthese > closeParenthese )
          {
              for (int i=0; i < diff;i++)
              {
                  t=t+')';
              }
          }
      }
    m_pView->setText( t );
  }
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
  KSpreadSheet *table = activeTable();
  if ( !m_pEditor )
  {
    KSpreadCell* cell = activeTable()->cellAt( marker() );
    if ( ed == CellEditor )
    {
      m_pEditWidget->setEditMode( true );

      m_pEditor = new KSpreadTextEditor( cell, this );
    }

    double w, h;
    double min_w = cell->dblWidth( markerColumn(), this );
    double min_h = cell->dblHeight( markerRow(), this );
    if ( cell->isDefault() )
    {
      w = min_w;
      h = min_h;
      //kdDebug(36001) << "DEFAULT" << endl;
    }
    else
    {
      w = cell->extraWidth();
      h = cell->extraHeight();
      //kdDebug(36001) << "HEIGHT=" << min_h << " EXTRA=" << h << endl;
    }
    double xpos = table->dblColumnPos( markerColumn(), this );
    double ypos = table->dblRowPos( markerRow(), this );
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
    m_pEditor->setGeometry( int( xpos ), int( ypos ), int( w ), int( h ) );
    m_pEditor->setMinimumSize( QSize( int( min_w ), int( min_h ) ) );
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


void KSpreadCanvas::updateChooseRect(const QPoint &newMarker, const QPoint &newAnchor)
{
  KSpreadSheet* table = activeTable();

  QPoint oldAnchor = selectionInfo()->getChooseAnchor();
  QPoint oldMarker = selectionInfo()->getChooseMarker();
  QPoint chooseCursor = selectionInfo()->getChooseCursor();
  QRect oldChooseRect = selectionInfo()->getChooseRect();


  if (newMarker == oldMarker && newAnchor == oldAnchor)
  {
    return;
  }

  selectionInfo()->setChooseMarker(newMarker);
  selectionInfo()->setChooseAnchor(newAnchor);

  QRect newChooseRect = selectionInfo()->getChooseRect();

  /* keep the choose cursor updated.  If you don't know what the 'cursor' is
     supposed to represent, check the comments of the regular selection cursor
     in kspread_selection.h (KSpreadSelection::m_cursorPosition).  It's the
     same thing here except for the choose selection.
  */
  if ( !newChooseRect.contains(chooseCursor))
  {
    selectionInfo()->setChooseCursor(table, newMarker);
  }

  /* make sure the old selection was valid before we try to paint it */
  if (oldChooseRect.left() <= 0 || oldChooseRect.top() <= 0)
  {
    updateCellRect(newChooseRect);
  }
  else
  {
    paintSelectionChange(oldChooseRect, newChooseRect);
  }

  /* this signal is used in the formula editor to update the text display */
  emit m_pView->sig_chooseSelectionChanged(activeTable(), newChooseRect);

  if ( !m_pEditor )
  {
    length_namecell = 0;
    return;
  }

  /* the rest of this function updates the text showing the choose rect */

  if (newMarker.x() != 0 && newMarker.y() != 0)
    /* don't update the text if we are removing the marker */
  {
    QString name_cell;

    if( m_chooseStartTable != table )
    {
      if ( newMarker == newAnchor )
        name_cell = KSpreadCell::fullName( table, newChooseRect.left(), newChooseRect.top() );
      else
        name_cell = util_rangeName( table, newChooseRect );
    }
    else
    {
      if ( newMarker == newAnchor )
        name_cell = KSpreadCell::name( newChooseRect.left(), newChooseRect.top() );
      else
        name_cell = util_rangeName( newChooseRect );
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
    editWidget()->setText( res );
    //kdDebug(36001) << "old=" << old << " len=" << length_namecell << " pos=" << pos << endl;
  }
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void KSpreadCanvas::updateCellRect( const QRect &_rect )
{
    updateSelection( _rect, marker() );
}

void KSpreadCanvas::updateSelection( const QRect & oldSelection,
                                     const QPoint& /*oldMarker*/ )
  /* for now oldMarker is unused.  Maybe we can just remove it? */
{
  paintSelectionChange(oldSelection, selection());
}

void KSpreadCanvas::paintSelectionChange(QRect area1, QRect area2)
{
  const KSpreadSheet *table = activeTable();
  if ( !table )
    return;

  QValueList<QRect> cellRegions;

  /* let's try to only paint where the selection is actually changing*/
  bool newLeft   = area1.left() != area2.left();
  bool newTop    = area1.top() != area2.top();
  bool newRight  = area1.right() != area2.right();
  bool newBottom = area1.bottom() != area2.bottom();
  bool topLeftSame = !newLeft && !newTop;
  bool topRightSame = !newTop && !newRight;
  bool bottomLeftSame = !newLeft && !newBottom;
  bool bottomRightSame = !newBottom && !newRight;

  if (!topLeftSame && !topRightSame && !bottomLeftSame && !bottomRightSame)
  {
    /* the two areas are not related. */
    /* since the marker/selection border extends into neighboring cells, we
       want to calculate all the cells bordering these regions.
    */
    ExtendRectBorder(area1);
    ExtendRectBorder(area2);
    cellRegions.append(area1);
    cellRegions.append(area2);
  }
  else
  {
    /* at least one corner is the same -- let's only paint the extension
       on corners that are not the same
    */

    /* first, calculate some numbers that we'll use a few times */
    int farLeft = QMIN(area1.left(), area2.left());
    if (farLeft != 1) farLeft--;
    int innerLeft = QMAX(area1.left(), area2.left());
    if (innerLeft != KS_colMax) innerLeft++;

    int farTop = QMIN(area1.top(), area2.top());
    if (farTop != 1) farTop--;
    int innerTop = QMAX(area1.top(), area2.top());
    if (innerTop != KS_rowMax) innerTop++;

    int farRight = QMAX(area1.right(), area2.right());
    if (farRight != KS_colMax) farRight++;
    int innerRight = QMIN(area1.right(), area2.right());
    if (innerRight != 1) innerRight--;

    int farBottom = QMAX(area1.bottom(), area2.bottom());
    if (farBottom!= KS_rowMax) farBottom++;
    int innerBottom = QMIN(area1.bottom(), area2.bottom());
    if (innerBottom != 1) innerBottom--;

    if (newLeft)
    {
      cellRegions.append(QRect(QPoint(farLeft, farTop),
                               QPoint(innerLeft, farBottom)));
    }

    if (newTop)
    {
      cellRegions.append(QRect(QPoint(farLeft, farTop),
                               QPoint(farRight, innerTop)));
    }

    if (newRight)
    {
      cellRegions.append(QRect(QPoint(innerRight, farTop),
                               QPoint(farRight, farBottom)));
    }

    if (newBottom)
    {
      cellRegions.append(QRect(QPoint(farLeft, innerBottom),
                               QPoint(farRight, farBottom)));
    }
  }

  /* Prepare the painter */
  QPainter painter( this );
  painter.save();

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

  //TODO: Fix following condition, shouldn't be necessary: Used when zoomed
  if ( view.width() < width() )
    view.setWidth( width() );

  if ( view.height() < height() )
    view.setHeight( height() );


  m_pDoc->paintCellRegions(painter, view, m_pView, cellRegions, table, true);
  painter.restore();

  //TODO: remove hack: The painter is saved, restored and "scaled" with the matrix again,
  //      because within paintCellRegions the region for the children is clipped out.

  // Draw children
  m = m_pView->matrix();
  painter.setWorldMatrix( m );

  QPtrListIterator<KoDocumentChild> it( m_pDoc->children() );
  it.toFirst();

  for( ; it.current(); ++it )
  {
    if ( ((KSpreadChild*)it.current())->table() == table &&
         !m_pView->hasDocumentInWindow( it.current()->document() ) )
    {
      // #### todo: paint only if child is visible inside rect
      painter.save();
      m_pDoc->paintChild( it.current(), painter, m_pView );
      painter.restore();
    }
  }

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


void KSpreadCanvas::updatePosWidget()
{
    QString buffer;
    // No selection, or only one cell merged selected
    if ( selectionInfo()->singleCellSelection() )
    {
        if(activeTable()->getLcMode())
        {
            buffer = "L" + QString::number( markerRow() ) +
		"C" + QString::number( markerColumn() );
        }
        else
        {
            buffer = util_encodeColumnLabelText( markerColumn() ) +
		QString::number( markerRow() );
        }
    }
    else
    {
        if(activeTable()->getLcMode())
        {
            buffer = QString::number( (selection().bottom()-selection().top()+1) )+"Lx";
            if( util_isRowSelected( selection() ) )
                buffer+=QString::number((KS_colMax-selection().left()+1))+"C";
            else
                buffer+=QString::number((selection().right()-selection().left()+1))+"C";
        }
        else
        {
                //encodeColumnLabelText return @@@@ when column >KS_colMax
                //=> it's not a good display
                //=> for the moment I display pos of marker
                buffer=util_encodeColumnLabelText( selection().left() ) +
		    QString::number(selection().top()) + ":" +
		    util_encodeColumnLabelText( QMIN( KS_colMax, selection().right() ) ) +
		    QString::number(selection().bottom());
                //buffer=activeTable()->columnLabel( m_iMarkerColumn );
                //buffer+=tmp.setNum(m_iMarkerRow);
        }
  }

    if (buffer != m_pPosWidget->text())
      m_pPosWidget->setText(buffer);
}




void KSpreadCanvas::adjustArea(bool makeUndo)
{
  QRect selection( selection() );
  if(activeTable()->areaIsEmpty(selection))
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
  if( util_isColumnSelected(selection) )
  {
    for (int x=selection.left(); x <= selection.right(); x++ )
    {
      hBorderWidget()->adjustColumn(x,false);
    }
  }
  // Rows selected
  else if( util_isRowSelected(selection) )
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
  QRect selection( selection() );
  RowLayout *rl = m_pView->activeTable()->rowLayout(selection.top());
  int size=rl->height(this);
  if ( selection.top() == selection.bottom() )
      return;
  for(int i=selection.top()+1;i<=selection.bottom();i++)
  {
      KSpreadSheet *table = activeTable();
      if ( !table )
          return;
      size=QMAX(m_pView->activeTable()->rowLayout(i)->height(this),size);
  }
  m_pView->vBorderWidget()->equalizeRow(size);
}

void KSpreadCanvas::equalizeColumn()
{
  QRect selection( selection() );
  ColumnLayout *cl = m_pView->activeTable()->columnLayout(selection.left());
  int size=cl->width(this);
  if ( selection.left() == selection.right() )
      return;

  for(int i=selection.left()+1;i<=selection.right();i++)
  {
    size=QMAX(m_pView->activeTable()->columnLayout(i)->width(this),size);
  }
  m_pView->hBorderWidget()->equalizeColumn(size);
}

QRect KSpreadCanvas::visibleCells()
{
  QWMatrix m = m_pView->matrix();

  // Which part of the document is visible ? To determine this
  // just transform the viewport rectangle with the inverse
  // matrix, since this matrix usually transforms from document
  // coordinates to view coordinates.
  m = m.invert();
  QPoint tl = m.map( QPoint( 0, 0 ) );
  QPoint br = m.map( QPoint( width(), height() ) );

  double xpos, ypos;
  int left_col = activeTable()->leftColumn( tl.x(), xpos );
  int right_col = activeTable()->rightColumn( br.x() );
  int top_row = activeTable()->topRow( tl.y(), ypos );
  int bottom_row = activeTable()->bottomRow( br.y() );

  return QRect(left_col, top_row, right_col - left_col + 1,
               bottom_row - top_row + 1);
}

void KSpreadCanvas::paintUpdates()
{
  QWMatrix m = m_pView->matrix();

  QPainter painter(this);
  painter.save();

  // Do the view transformation.
  painter.setWorldMatrix( m );

  // Which part of the document is visible ? To determine this
  // just transform the viewport rectangle with the inverse
  // matrix, since this matrix usually transforms from document
  // coordinates to view coordinates.
  m = m.invert();
  QPoint tl = m.map( QPoint( 0, 0 ) );
  QPoint br = m.map( QPoint( width(), height() ) );
  QRect view( tl, br );


  /* paint any visible cell that has the paintDirty flag */
  QRect range = visibleCells();
  KSpreadRangeIterator it(range, activeTable());
  KSpreadCell* cell = NULL;

  double topPos = activeTable()->dblRowPos(range.top());
  QPair<double, double> dblCorner =
    qMakePair(activeTable()->dblColumnPos(range.left()), topPos);

  int x;
  int y;

  for (x = range.left(); x <= range.right(); x++)
  {
    for (y = range.top(); y <= range.bottom(); y++)
    {
      if (activeTable()->cellIsPaintDirty(QPoint(x,y)))
      {
        cell = activeTable()->cellAt(x, y);
        cell->calc();
        cell->makeLayout(painter, x, y);
        cell->paintCell(view, painter, m_pView, dblCorner,
                        QPoint(cell->column(), cell->row()));

      }
      dblCorner.second += activeTable()->rowLayout(y)->dblHeight(this);
    }
    dblCorner.second = topPos;
    dblCorner.first += activeTable()->columnLayout(x)->dblWidth(this);
  }

  painter.restore();
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

  const KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );
  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
  {
    m_pCanvas->deleteEditor( true ); // save changes
  }
  // Find the first visible row and the y position of this row.
  double y;
  int row = table->topRow( 0, y, m_pCanvas );

  // Did the user click between two rows ?
  while ( y < height() )
  {
    double h = table->rowLayout( row )->dblHeight( m_pCanvas );
    row++;
    if ( row > KS_rowMax )
	row = KS_rowMax;
    if ( _ev->pos().y() >= int( y + h ) - 1 && _ev->pos().y() <= int( y + h ) + 1
	&& !(table->rowLayout( row )->isHide() && row == 1 ) )
      m_bResize = TRUE;
    y += h;
  }
  double tmp2;
  int tmpRow = table->topRow( _ev->pos().y() - 1, tmp2, m_pCanvas );
  if( table->rowLayout(tmpRow  )->isHide() && tmpRow == 1 )
      m_bResize = false;
  // So he clicked between two rows ?
  if ( m_bResize )
  {
    // Determine row to resize
    double tmp;
    m_iResizedRow = table->topRow( _ev->pos().y() - 1, tmp, m_pCanvas );

    paintSizeIndicator( _ev->pos().y(), true );
  }
  else
  {
    m_bSelection = TRUE;

    double tmp;
    int hit_row = table->topRow( _ev->pos().y(), tmp, m_pCanvas );
    if(hit_row > KS_rowMax)
	return;

    m_iSelectionAnchor = hit_row;

    QRect rect = m_pView->selection();
    if(!rect.contains( QPoint(1,hit_row)) || !(_ev->button() == RightButton)
                || (!util_isRowSelected(rect)) )
        {
          QPoint newMarker(1, hit_row);
          QPoint newAnchor(KS_colMax, hit_row);
          m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                                  m_pView->activeTable() );
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
    KSpreadSheet *table = m_pCanvas->activeTable();
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
        QRect rect;
        rect.setCoords( 1, m_iResizedRow, KS_colMax, m_iResizedRow );
        if( util_isRowSelected(m_pView->selection()) )
        {
            if( m_pView->selection().contains( QPoint( 1, m_iResizedRow ) ) )
            {
                start=m_pView->selection().top();
                end=m_pView->selection().bottom();
                rect=m_pView->selection();
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
        QRect rect = m_pView->selection();

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
    double adjust = -1.0;
    int select;
    if ( _row == -1 )
    {
        adjust = m_pCanvas->activeTable()->adjustRow( m_pView->selectionInfo() );
        select = m_iSelectionAnchor;
    }
    else
    {
        adjust = m_pCanvas->activeTable()->adjustRow( m_pView->selectionInfo(), _row );
        select = _row;
    }

    if ( adjust != -1.0 )
    {
        KSpreadSheet * table = m_pCanvas->activeTable();
        assert( table );
        if ( _row == -1 )
        {
            RowLayout * rl = table->nonDefaultRowLayout( select );

            if ( kAbs( rl->dblHeight() - adjust ) < DBL_EPSILON )
                return;
        }

        if ( makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
        {
            QRect rect;
            rect.setCoords( 1, select, KS_colMax, select);
            KSpreadUndoResizeColRow * undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),
                                                                          m_pCanvas->activeTable(), rect );
            m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
        }
        RowLayout * rl = table->nonDefaultRowLayout( select );
        //    adjust = QMAX( (int)(2.0 * m_pCanvas->zoom()), adjust * m_pCanvas->zoom() );
        adjust = QMAX( 2.0, adjust );
        rl->setDblHeight(adjust * m_pCanvas->zoom(), m_pCanvas);
    }
}

void KSpreadVBorder::equalizeRow( int resize )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  Q_ASSERT( table );
  QRect selection( m_pView->selection() );
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

void KSpreadVBorder::resizeRow( double resize, int nb, bool makeUndo )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
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
    rl->setDblHeight( resize, m_pCanvas );
  }
  else
  {
    QRect selection( m_pView->selection() );
    if( m_pView->selectionInfo()->singleCellSelection() )
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
      rl->setDblHeight( resize, m_pCanvas );
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
        rl->setDblHeight( resize, m_pCanvas );
      }
    }
  }
}

void KSpreadVBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  if(!m_pView->koDocument()->isReadWrite())
    return;

  KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );
  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    paintSizeIndicator( _ev->pos().y(), false );
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    double y;
    int row = table->topRow( _ev->pos().y(), y, m_pCanvas );
    if( row > KS_rowMax )
	return;

    QPoint newAnchor = m_pView->selectionInfo()->selectionAnchor();
    QPoint newMarker = m_pView->selectionInfo()->marker();
    newMarker.setY( row );
    newAnchor.setY( m_iSelectionAnchor );

    m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                            m_pView->activeTable() );

    if ( _ev->pos().y() < 0 )
      m_pCanvas->vertScrollBar()->setValue( int( m_pCanvas->yOffset() + y ) );
    else if ( _ev->pos().y() > m_pCanvas->height() )
    {
      if ( row < KS_rowMax )
      {
        RowLayout *rl = table->rowLayout( row + 1 );
        y = table->dblRowPos( row + 1, m_pCanvas );
        m_pCanvas->vertScrollBar()->setValue( int( m_pCanvas->yOffset() + y
                                              + rl->dblHeight( m_pCanvas ) )
                                              - m_pCanvas->height() );
      }
    }
  }
  // No button is pressed and the mouse is just moved
  else
  {
    double tmp;
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
  KSpreadSheet *table = m_pCanvas->activeTable();
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
    KSpreadSheet *table = m_pCanvas->activeTable();
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

void KSpreadVBorder::updateRows( int from, int to )
{
    KSpreadTable *table = m_pCanvas->activeTable();
    if ( !table )
	return;
    int y0 = table->rowPos( from, m_pCanvas );
    int y1 = table->rowPos( to+1, m_pCanvas );
    update( 0, y0, width(), y1-y0 );
}

void KSpreadVBorder::paintEvent( QPaintEvent* _ev )
{
  //Notes on zoom-support from Philipp:
  //Scaling here is not good (fonts and AA), so we need a different approach.
  //As long as the canvas is zoomed with scale, we need to simulate scaling here too.
  //Scaling means, we have at the end a qRound function.
  //This means this ugly formula qRound(int(dblheight)*zoom) simulates this scaling.

  KSpreadSheet *table = m_pCanvas->activeTable();
  if ( !table )
    return;

  QPainter painter( this );
  QPen pen( Qt::black, 1 );
  painter.setPen( pen );
  // painter.setBackgroundColor( colorGroup().base() );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  painter.setClipRect( _ev->rect() );

  double dblYpos;
  //Get the top row and the current y-position
  int y = table->topRow( qRound( int( (_ev->rect().y() + m_pCanvas->yOffset() ) / m_pCanvas->zoom() ) ), dblYpos );
  //Align to the unzoomed offset
  dblYpos = dblYpos - qRound( m_pCanvas->yOffset() / m_pCanvas->zoom() );
  //Simulate the scaling
  int scaledYpos = qRound( dblYpos * m_pCanvas->zoom() );

  QFont normalFont = painter.font();
  if (m_pCanvas->zoom() < 1)
  {
    normalFont.setPointSize( (int) ((double) normalFont.pointSize() * m_pCanvas->zoom()) );
  }
  QFont boldFont = normalFont;
  boldFont.setBold( TRUE );

  //several cells selected but not just a cell merged
  bool area= !(m_pView->selectionInfo()->singleCellSelection());

  //Loop through the rows, until we are out of range
  while ( scaledYpos <= _ev->rect().bottom() )
  {
    bool highlighted = (area && y >= m_pView->selection().top() &&
                        y <= m_pView->selection().bottom() );
    bool selected = ( highlighted && (util_isRowSelected(m_pView->selection())) );
    bool current  = ( !highlighted && y == m_pView->selection().top() );

    const RowLayout *row_lay = table->rowLayout( y );

    //The height is the height of a column plus the the rest of the double ypos values, rounded because of scaling
    int scaledHeight = qRound( int( dblYpos + row_lay->dblHeight() ) * m_pCanvas->zoom() ) - scaledYpos;

    if ( selected )
    {
      QBrush fillSelected( colorGroup().brush( QColorGroup::Highlight ) );
      qDrawShadePanel( &painter, 0, scaledYpos, YBORDER_WIDTH, scaledHeight,
                       colorGroup(), FALSE, 1, &fillSelected );
    }
    else if ( highlighted )
    {
      QBrush fillHighlighted( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, 0, scaledYpos, YBORDER_WIDTH, scaledHeight,
                       colorGroup(), true, 1, &fillHighlighted );
    }
    else
    {
      QBrush fill( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, 0, scaledYpos, YBORDER_WIDTH, scaledHeight, colorGroup(), FALSE, 1, &fill );
    }

    QString rowText = QString::number( y );

    // Reset painter
    painter.setFont( normalFont );
    painter.setPen( colorGroup().text() );

    if ( selected )
      painter.setPen( colorGroup().highlightedText() );
    else if ( highlighted || current )
      painter.setFont( boldFont );
    int len = painter.fontMetrics().width( rowText );
    if(!row_lay->isHide())
        painter.drawText( ( YBORDER_WIDTH-len )/2, scaledYpos +
                          ( scaledHeight + painter.fontMetrics().ascent() -
                            painter.fontMetrics().descent() ) / 2, rowText );

    dblYpos += row_lay->dblHeight();
    scaledYpos = qRound( int( dblYpos ) * m_pCanvas->zoom() );
    y++;
  }
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
  const KSpreadSheet *table = m_pCanvas->activeTable();
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

  double x;
  int col = table->leftColumn( 0, x, m_pCanvas );

  while ( x < width() && !m_bResize )
  {
    double w = table->columnLayout( col )->dblWidth( m_pCanvas );
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
  double tmp2;
  int tmpCol=table->leftColumn( _ev->pos().x() - 1, tmp2, m_pCanvas );
  if ( table->columnLayout(tmpCol  )->isHide() && tmpCol == 1)
      m_bResize = false;

  QRect rect = m_pView->selection();
  if ( m_bResize )
  {
    // Determine the column to resize
    double tmp;
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
    double tmp;
    int hit_col = table->leftColumn( _ev->pos().x(), tmp, m_pCanvas );
    if( hit_col > KS_colMax)
	return;
    m_iSelectionAnchor = hit_col;

    if(!rect.contains( QPoint(hit_col,1)) || !(_ev->button() == RightButton)
                                          || !(util_isRowSelected(m_pView->selection())) )
    {
      QPoint newMarker(hit_col, 1);
      QPoint newAnchor(hit_col, KS_rowMax);
      m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                              m_pView->activeTable() );
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
    KSpreadSheet *table = m_pCanvas->activeTable();
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
        QRect rect;
        rect.setCoords( m_iResizedColumn, 1, m_iResizedColumn, KS_rowMax );
        if( util_isColumnSelected(m_pView->selection()) )
        {
            if(m_pView->selection().contains(QPoint(m_iResizedColumn,1)))
            {
                start=m_pView->selection().left();
                end=m_pView->selection().right();
                rect=m_pView->selection();
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
        QRect rect = m_pView->selection();

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
  double adjust = -1.0;
  int select;

  if ( _col == -1 )
  {
    adjust = m_pCanvas->activeTable()->adjustColumn(m_pView->selectionInfo());
    select = m_iSelectionAnchor;
  }
  else
  {
    adjust = m_pCanvas->activeTable()->adjustColumn(m_pView->selectionInfo(), _col);
    select = _col;
  }

  if (adjust != -1.0)
  {
    KSpreadSheet * table = m_pCanvas->activeTable();
    assert( table );

    if ( _col == -1 )
    {
        ColumnLayout * cl = table->nonDefaultColumnLayout( select );
        if ( kAbs( cl->dblWidth() - adjust ) < DBL_EPSILON )
            return;

    }
    if ( makeUndo && !m_pCanvas->doc()->undoBuffer()->isLocked() )
    {
        QRect rect;
        rect.setCoords( select, 1, select, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),
                                                                     m_pCanvas->activeTable() , rect );
        m_pCanvas->doc()->undoBuffer()->appendUndo( undo );
    }

    ColumnLayout * cl = table->nonDefaultColumnLayout( select );

    //    adjust = QMAX( (int)(2.0 * m_pCanvas->zoom()), adjust * m_pCanvas->zoom() );
    adjust = QMAX( 2.0, adjust );
    cl->setDblWidth( adjust * m_pCanvas->zoom(), m_pCanvas );
  }
}

void KSpreadHBorder::equalizeColumn( int resize )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  Q_ASSERT( table );
  QRect selection( m_pView->selection() );
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

void KSpreadHBorder::resizeColumn( double resize, int nb, bool makeUndo )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
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
    cl->setDblWidth( resize, m_pCanvas );
  }
  else
  {
    QRect selection( m_pView->selection() );
    if( m_pView->selectionInfo()->singleCellSelection() )
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
      cl->setDblWidth( resize, m_pCanvas );
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
        cl->setDblWidth( resize, m_pCanvas );
      }
    }
  }
}

void KSpreadHBorder::mouseDoubleClickEvent( QMouseEvent * /*_ev */)
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );
  if(!m_pView->koDocument()->isReadWrite())
    return;
  adjustColumn();
}

void KSpreadHBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );
  if(!m_pView->koDocument()->isReadWrite())
    return;

  if ( m_bResize )
  {
    paintSizeIndicator( _ev->pos().x(), false );
  }
  else if ( m_bSelection )
  {
    double x;
    int col = table->leftColumn( _ev->pos().x(), x, m_pCanvas );
    if( col > KS_colMax )
	return;

    QPoint newMarker = m_pView->selectionInfo()->marker();
    QPoint newAnchor = m_pView->selectionInfo()->selectionAnchor();

    newMarker.setX(col);
    newAnchor.setX(m_iSelectionAnchor);

    m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                            m_pView->activeTable() );

    if ( _ev->pos().x() < 0 )
      m_pCanvas->horzScrollBar()->setValue( int( m_pCanvas->xOffset() + x ) );
    else if ( _ev->pos().x() > m_pCanvas->width() )
    {
      if ( col < KS_colMax )
      {
        ColumnLayout *cl = table->columnLayout( col + 1 );
        x = table->dblColumnPos( col + 1, m_pCanvas );
        m_pCanvas->horzScrollBar()->setValue( int( m_pCanvas->xOffset() +
                                              x + cl->dblWidth( m_pCanvas ) ) - m_pCanvas->width() );
      }
    }

  }
  // Perhaps we have to modify the cursor
  else
  {
    //if col is hide and it's the first column
    //you mustn't resize it.
    double tmp2;
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
    KSpreadSheet *table = m_pCanvas->activeTable();
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

void KSpreadHBorder::updateColumns( int from, int to )
{
    KSpreadTable *table = m_pCanvas->activeTable();
    if ( !table )
	return;
    int x0 = table->columnPos( from, m_pCanvas );
    int x1 = table->columnPos( to+1, m_pCanvas );
    update( x0, 0, x1-x0, height() );
}

void KSpreadHBorder::paintEvent( QPaintEvent* _ev )
{
  //Notes on zoom-support from Philipp:
  //Scaling here is not good (fonts and AA), so we need a different approach.
  //As long as the canvas is zoomed with scale, we need to simulate scaling here too.
  //Scaling means, we have at the end a qRound function.
  //This means this ugly formula qRound(int(dblheight)*zoom) simulates this scaling.

  KSpreadSheet *table = m_pCanvas->activeTable();
  if ( !table )
    return;

  QPainter painter( this );
  QPen pen( Qt::black, 1 );
  painter.setPen( pen );
  painter.setBackgroundColor( white );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  // Determine which columns need painting
  double dblXpos;
  //Get the left column and the current x-position
  int x = table->leftColumn( qRound( int(_ev->rect().x() + m_pCanvas->xOffset() ) / m_pCanvas->zoom() ), dblXpos );
  //Align to the unzoomed offset
  dblXpos = dblXpos - qRound( m_pCanvas->xOffset() / m_pCanvas->zoom() );
  //Simulate the scaling
  int scaledXpos = qRound( dblXpos * m_pCanvas->zoom() );

  QFont normalFont = painter.font();
  QFont boldFont = normalFont;
  boldFont.setBold( TRUE );
  KSpreadCell *cell=table->cellAt( m_pView->marker() );
  QRect extraCell;
  extraCell.setCoords(m_pCanvas->markerColumn(),m_pCanvas->markerRow(),
  m_pCanvas->markerColumn()+cell->extraXCells(),m_pCanvas->markerRow()+cell->extraYCells());

  //several cells selected but not just a cell merged
  bool area=( m_pView->selection().left()!=0 &&
              extraCell != m_pView->selection() );

  //Loop through the columns, until we are out of range
  while ( scaledXpos <= _ev->rect().right() )
  {
    bool highlighted = ( area && x >= m_pView->selection().left() &&
                         x <= m_pView->selection().right());
    bool selected = ( highlighted && util_isColumnSelected(m_pView->selection()) &&
                      (!util_isRowSelected(m_pView->selection())) );
    bool current = ( !highlighted && x == m_pView->selection().left() );

    const ColumnLayout *col_lay = table->columnLayout( x );

    //The width is the width of a column plus the delta between dblXpos and xpos
    int scaledWidth = qRound( int( dblXpos + col_lay->dblWidth() ) * m_pCanvas->zoom() ) - scaledXpos;

    if ( selected )
    {
      QBrush fillSelected( colorGroup().brush( QColorGroup::Highlight ) );
      qDrawShadePanel( &painter, scaledXpos, 0, scaledWidth, XBORDER_HEIGHT,
                       colorGroup(), FALSE, 1, &fillSelected );
    }
    else if ( highlighted )
    {
      QBrush fillHighlighted( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, scaledXpos, 0, scaledWidth, XBORDER_HEIGHT,
                       colorGroup(), true, 1, &fillHighlighted );
    }
    else
    {
      QBrush fill( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter, scaledXpos, 0, scaledWidth,
                       XBORDER_HEIGHT, colorGroup(), FALSE, 1, &fill );
    }

    // Reset painter
    painter.setFont( normalFont );
    painter.setPen( colorGroup().text() );

    if ( selected )
      painter.setPen( colorGroup().highlightedText() );
    else if ( highlighted || current )
      painter.setFont( boldFont );
    if(!m_pView->activeTable()->getShowColumnNumber())
    {
        QString colText = util_encodeColumnLabelText( x );
        int len = painter.fontMetrics().width( colText );
        if(!col_lay->isHide())
            painter.drawText( scaledXpos + ( scaledWidth - len ) / 2,
                  ( XBORDER_HEIGHT + painter.fontMetrics().ascent() -
                    painter.fontMetrics().descent() ) / 2, colText );
    }
    else
    {
        QString tmp;
        int len = painter.fontMetrics().width( tmp.setNum(x) );
        if(!col_lay->isHide())
            painter.drawText( scaledXpos + ( scaledWidth - len ) / 2,
                  ( XBORDER_HEIGHT + painter.fontMetrics().ascent() -
                    painter.fontMetrics().descent() ) / 2,
                    tmp.setNum(x) );
    }
    dblXpos += col_lay->dblWidth();
    scaledXpos = qRound( int( dblXpos ) * m_pCanvas->zoom() );
    x++;
  }
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
    KSpreadSheet *table = m_canvas->activeTable();
    if ( !table )
        return;

    // Over which cell is the mouse ?
    double ypos, xpos;
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
      cell = cell->obscuringCells().first();
      int moveX = cell->column();
      int moveY = cell->row();

      // Use the obscuring cells dimensions
      u = cell->width( moveX, m_canvas );
      xpos = table->columnPos( moveX, m_canvas );
      ypos = table->rowPos( moveY, m_canvas );
    }

    // Is the cursor over the comment marker (if there is any) then
    // show the comment.
    double zoom = m_canvas->zoom();
    QRect marker( int(xpos) + u - (int)(6*zoom), int(ypos) -(int)(1*zoom), (int)(7*zoom),(int)( 7*zoom) );
    if ( marker.contains( p ) )
    {
        tip( marker, comment );
    }
}

#include "kspread_canvas.moc"
