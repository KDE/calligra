/* This file is part of the KDE project

   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 1999-2004 David Faure <faure@kde.org>
   Copyright 2002-2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Hamish Rodda <rodda@kde.org>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Daniel Naber <daniel.naber@t-online.de>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 1999-2000 Stephan Kulow <coolo@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Michael Reiher <michael.reiher.gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kspread_util.h"
#include "kspread_editors.h"
#include "kspread_map.h"
#include "kspread_undo.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_global.h"
#include "kspread_view.h"
#include "kspread_selection.h"
#include "kspread_locale.h"

#include <kmessagebox.h>
#include <kcursor.h>
#include <kdebug.h>
#include <krun.h>

#include <assert.h>
#include <stdlib.h>
#include <qbuffer.h>
#include <qlabel.h>
#include <qdrawutil.h>
#include <qbutton.h>
#include <qapplication.h>
#include <qtimer.h>
#include <qpoint.h>
#include <qscrollbar.h>
#include <float.h>


KSpreadComboboxLocationEditWidget::KSpreadComboboxLocationEditWidget( QWidget * _parent,
                                                      KSpreadView * _view )
    : KComboBox( _parent, "KSpreadComboboxLocationEditWidget" )
{
    m_locationWidget = new KSpreadLocationEditWidget( _parent, _view );
    setLineEdit( m_locationWidget );
    insertItem( "" );

    QValueList<Reference>::Iterator it;
    QValueList<Reference> area = _view->doc()->listArea();
    for ( it = area.begin(); it != area.end(); ++it )
        slotAddAreaName( (*it).ref_name);
}

void KSpreadComboboxLocationEditWidget::slotAddAreaName( const QString &_name)
{
    insertItem( _name );
}

void KSpreadComboboxLocationEditWidget::slotRemoveAreaName( const QString &_name )
{
    for ( int i = 0; i<count(); i++ )
        if ( text(i)==_name )
        {
            removeItem( i );
            return;
        }
}

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
            if ( pos !=- 1 )
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
            setText( KSpreadCell::columnName( m_pView->canvasWidget()->markerColumn() )
                     + QString::number( m_pView->canvasWidget()->markerRow() ) );
        } else {
            setText( KSpreadCell::columnName( m_pView->selection().left() )
                     + QString::number( m_pView->selection().top() )
                     + ":"
                     + KSpreadCell::columnName( m_pView->selection().right() )
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

  if ( !m_pCanvas->doc()->isReadWrite() || !m_pCanvas->activeTable() )
    setEnabled( false );

  QObject::connect( m_pCancelButton, SIGNAL( clicked() ),
                    this, SLOT( slotAbortEdit() ) );
  QObject::connect( m_pOkButton, SIGNAL( clicked() ),
                    this, SLOT( slotDoneEdit() ) );

  setEditMode( false ); // disable buttons
}

void KSpreadEditWidget::showEditWidget(bool _show)
{
    if (_show)
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
         || ( _ev->key() == Key_Shift )
         || ( _ev->key() == Key_Control ) )
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
  : QWidget( _parent, "", /*WNorthWestGravity*/ WStaticContents| WResizeNoErase | WRepaintNoErase ),
    m_dragStart( -1, -1 ),
    m_dragging( false )
{
  length_namecell = 0;
  m_chooseStartTable = NULL;
  m_pEditor = 0;
  m_bChoose = FALSE;
  m_validationInfo = 0L;

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
  setAcceptDrops( true );
}

KSpreadCanvas::~KSpreadCanvas()
{
  delete m_scrollTimer;
  delete m_validationInfo;
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

KSpreadSelection* KSpreadCanvas::selectionInfo() const
{
  return m_pView->selectionInfo();
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

double KSpreadCanvas::zoom() const
{
  return m_pView->zoom();
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

  length_namecell = 0;
  m_bChoose = FALSE;

  KSpreadSheet *table=m_pView->doc()->map()->findTable(m_chooseStartTable->tableName());
  if (table)
    m_pView->setActiveTable(table);

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

void KSpreadCanvas::gotoLocation( QPoint const & location, KSpreadSheet* table,
                                  bool extendSelection)
{
    //  kdDebug() << "GotoLocation: " << location.x() << ", " << location.x() << endl;

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
        QPoint topLeft(location);
        KSpreadCell* cell = table->cellAt(location);
        if ( cell->isObscured() && cell->isObscuringForced() )
        {
            cell = cell->obscuringCells().first();
            topLeft = QPoint(cell->column(), cell->row());
        }

        if (m_bChoose)
        {
            updateChooseRect(topLeft, topLeft);
            if( m_pEditor )
            {
                if( m_chooseStartTable != table )
                    m_pEditor->hide();
                else
                    m_pEditor->show();
            }
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

    if ( selectionInfo()->singleCellSelection() )
    {
        int col = selectionInfo()->marker().x();
        int row = selectionInfo()->marker().y();
        KSpreadCell * cell = table->cellAt( col,row );
        if ( cell && cell->getValidity(0) && cell->getValidity()->displayValidationInformation)
        {
            QString title = cell->getValidity(0)->titleInfo;
            QString message = cell->getValidity(0)->messageInfo;
            if ( title.isEmpty() && message.isEmpty() )
                return;

            if ( !m_validationInfo )
                m_validationInfo = new QLabel(  this );
            kdDebug()<<" display info validation\n";
            double u = cell->dblWidth( col );
            double v = cell->dblHeight( row );
            double xpos = table->dblColumnPos( markerColumn() ) - xOffset();
            double ypos = table->dblRowPos( markerRow() ) - yOffset();
            // Special treatment for obscured cells.
            if ( cell->isObscured() && cell->isObscuringForced() )
            {
                cell = cell->obscuringCells().first();
                int moveX = cell->column();
                int moveY = cell->row();

                // Use the obscuring cells dimensions
                u = cell->dblWidth( moveX );
                v = cell->dblHeight( moveY );
                xpos = table->dblColumnPos( moveX );
                ypos = table->dblRowPos( moveY );
            }
            //m_validationInfo->setGeometry( 3, y + 3, len + 2, hei + 2 );
            m_validationInfo->setAlignment( Qt::AlignVCenter );
            QPainter painter;
            painter.begin( this );
            int len = 0.0;
            int hei = 0.0;
            QString resultText;
            if ( !title.isEmpty() )
            {
                len = painter.fontMetrics().width( title );
                hei = painter.fontMetrics().height();
                resultText = title + "\n";
            }
            if ( !message.isEmpty() )
            {
                int i = 0;
                int pos = 0;
                QString t;
                do
                {
                    i = message.find( "\n", pos );
                    if ( i == -1 )
                        t = message.mid( pos, message.length() - pos );
                    else
                    {
                        t = message.mid( pos, i - pos );
                        pos = i + 1;
                    }
                    hei += painter.fontMetrics().height();
                    len = QMAX( len, painter.fontMetrics().width( t ) );
                }
                while ( i != -1 );
                resultText += message;
            }
            painter.end();
            m_validationInfo->setText( resultText );

            KoRect unzoomedMarker( xpos - xOffset()+u,
                                   ypos - yOffset()+v,
                                   len,
                                   hei );
            QRect marker( doc()->zoomRect( unzoomedMarker ) );

            m_validationInfo->setGeometry( marker );
            m_validationInfo->show();
        }
        else
        {
            delete m_validationInfo;
            m_validationInfo = 0L;
        }
    }
    else
    {
        delete m_validationInfo;
        m_validationInfo = 0L;
    }
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

  double unzoomedWidth = doc()->unzoomItX( width() );
  double unzoomedHeight = doc()->unzoomItY( height() );

  double xpos;
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
    xpos = unzoomedWidth - table->dblColumnPos( location.x() ) - xOffset();
  else
    xpos = table->dblColumnPos( location.x() ) - xOffset();
  double ypos = table->dblRowPos( location.y() ) - yOffset();

  double minY = 40.0;
  double maxY = unzoomedHeight - 40.0;
  //kdDebug(36001) << "KSpreadCanvas::gotoLocation : height=" << height() << endl;
  //kdDebug(36001) << "KSpreadCanvas::gotoLocation : width=" << width() << endl;

  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    double minX = 100.0; // less than that, we scroll
    double maxX = unzoomedWidth - 100.0; // more than that, we scroll

    kdDebug() << "rtl: XPos: " << xpos << ", min: " << minX << ", maxX: " << maxX << endl;

    minX = unzoomedWidth - 100.0; // less than that, we scroll
    maxX = 100.0; // more than that, we scroll

    kdDebug() << "rtl2: XPos: " << xpos << ", min: " << minX << ", maxX: " << maxX << ", Offset: " << xOffset() << endl;

    // do we need to scroll left
    if ( xpos > minX )
      horzScrollBar()->setValue( doc()->zoomItX( xOffset() - xpos + minX ) );

    //do we need to scroll right
    else if ( xpos < maxX )
    {
      double horzScrollBarValue = xOffset() + xpos + maxX;
      double horzScrollBarValueMax = table->sizeMaxX() + unzoomedWidth;

      //We don't want to display any area > KS_colMax widths
      if ( horzScrollBarValue > horzScrollBarValueMax )
        horzScrollBarValue = horzScrollBarValueMax;

      horzScrollBar()->setValue( doc()->zoomItX( horzScrollBarValue ) );
    }
  }
  else
  {
    double minX = 100.0; // less than that, we scroll
    double maxX = unzoomedWidth - 100.0; // more than that, we scroll

    // kdDebug() << "ltr: XPos: " << xpos << ", min: " << minX << ", maxX: " << maxX << endl;

    // do we need to scroll left
    if ( xpos < minX )
      horzScrollBar()->setValue( doc()->zoomItX( xOffset() + xpos - minX ) );

    //do we need to scroll right
    else if ( xpos > maxX )
    {
      double horzScrollBarValue = xOffset() + xpos - maxX;
      double horzScrollBarValueMax = table->sizeMaxX() - unzoomedWidth;

      //We don't want to display any area > KS_colMax widths
      if ( horzScrollBarValue > horzScrollBarValueMax )
        horzScrollBarValue = horzScrollBarValueMax;

      horzScrollBar()->setValue( doc()->zoomItX( horzScrollBarValue ) );
    }
  }

  // do we need to scroll up
  if ( ypos < minY )
    vertScrollBar()->setValue( doc()->zoomItY( yOffset() + ypos - minY ) );

  // do we need to scroll down
  else if ( ypos > maxY )
  {
    double vertScrollBarValue = yOffset() + ypos - maxY;
    double vertScrollBarValueMax = table->sizeMaxY() - unzoomedHeight;

    //We don't want to display any area > KS_rowMax heights
    if ( vertScrollBarValue > vertScrollBarValueMax )
      vertScrollBarValue = vertScrollBarValueMax;

    vertScrollBar()->setValue( doc()->zoomItY( vertScrollBarValue ) );
  }
}

void KSpreadCanvas::slotScrollHorz( int _value )
{
  KSpreadSheet * sheet = activeTable();

  if ( sheet == 0L )
    return;

  double unzoomedValue = doc()->unzoomItX( _value );
  double dwidth = doc()->unzoomItX( width() );

  doc()->emitBeginOperation(false);

  if ( unzoomedValue < 0.0 ) {
    unzoomedValue = 0.0;
    kdDebug (36001) << "KSpreadCanvas::slotScrollHorz: value out of range (unzoomedValue: " <<
                       unzoomedValue << ")" << endl;
  }

  double xpos = sheet->dblColumnPos( QMIN( KS_colMax, m_pView->activeTable()->maxColumn()+10 ) ) - m_dXOffset;
  if ( unzoomedValue > ( xpos + m_dXOffset ) )
    unzoomedValue = xpos + m_dXOffset;

  sheet->enableScrollBarUpdates( false );

  // Relative movement
  int dx = doc()->zoomItX( m_dXOffset - unzoomedValue );


  /* what cells will need painted now? */
  QRect area = visibleCells();
  double tmp;
  if (dx > 0)
  {
    area.setRight( area.left() );
    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
      area.setLeft( sheet->leftColumn( dwidth - unzoomedValue, tmp ) );
    else
      area.setLeft( sheet->leftColumn( unzoomedValue, tmp ) );
  }
  else
  {
    area.setLeft( area.right() );
    if ( sheet->layoutDirection()==KSpreadSheet::RightToLeft )
      area.setRight( sheet->rightColumn( unzoomedValue ) );
    else
      area.setRight( sheet->rightColumn( doc()->unzoomItX( width() ) +
                                         unzoomedValue ) );
  }

  sheet->setRegionPaintDirty(area);

  // New absolute position
  m_dXOffset = unzoomedValue;

  scroll( dx, 0 );

  hBorderWidget()->scroll( dx, 0 );

  sheet->enableScrollBarUpdates( true );

  doc()->emitEndOperation( sheet->visibleRect( this ) );
}

void KSpreadCanvas::slotScrollVert( int _value )
{
  if ( activeTable() == 0L )
    return;

  doc()->emitBeginOperation(false);
  double unzoomedValue = doc()->unzoomItY( _value );

  if ( unzoomedValue < 0 )
  {
    unzoomedValue = 0;
    kdDebug (36001) << "KSpreadCanvas::slotScrollVert: value out of range (unzoomedValue: " <<
                       unzoomedValue << ")" << endl;
  }

  double ypos = activeTable()->dblRowPos( QMIN( KS_rowMax, m_pView->activeTable()->maxRow()+10 ) );
  if ( unzoomedValue > ypos )
      unzoomedValue = ypos;

  activeTable()->enableScrollBarUpdates( false );

  // Relative movement
  int dy = doc()->zoomItY( m_dYOffset - unzoomedValue );


  /* what cells will need painted now? */
  QRect area = visibleCells();
  double tmp;
  if (dy > 0)
  {
    area.setBottom(area.top());
    area.setTop(activeTable()->topRow(unzoomedValue, tmp));
  }
  else
  {
    area.setTop(area.bottom());
    area.setBottom(activeTable()->bottomRow(doc()->unzoomItY(height()) +
                                            unzoomedValue));
  }

  activeTable()->setRegionPaintDirty( area );

  // New absolute position
  m_dYOffset = unzoomedValue;
  scroll( 0, dy );
  vBorderWidget()->scroll( 0, dy );

  activeTable()->enableScrollBarUpdates( true );

  doc()->emitEndOperation( activeTable()->visibleRect( this ) );
}

void KSpreadCanvas::slotMaxColumn( int _max_column )
{
  double xpos = activeTable()->dblColumnPos( QMIN( KS_colMax, _max_column + 10 ) ) - xOffset();
  double unzoomWidth = doc()->unzoomItX( width() );

  //Don't go beyond the maximum column range (KS_colMax)
  double sizeMaxX = activeTable()->sizeMaxX();
  if ( xpos > sizeMaxX - xOffset() - unzoomWidth )
    xpos = sizeMaxX - xOffset() - unzoomWidth;

  horzScrollBar()->setRange( 0, doc()->zoomItX( xpos + xOffset() ) );
}

void KSpreadCanvas::slotMaxRow( int _max_row )
{
  double ypos = activeTable()->dblRowPos( QMIN( KS_rowMax, _max_row + 10 ) ) - yOffset();
  double unzoomHeight = doc()->unzoomItY( height() );

  //Don't go beyond the maximum row range (KS_rowMax)
  double sizeMaxY = activeTable()->sizeMaxY();
  if ( ypos > sizeMaxY - yOffset() - unzoomHeight )
    ypos = sizeMaxY - yOffset() - unzoomHeight;

  vertScrollBar()->setRange( 0, doc()->zoomItY( ypos + yOffset() ) );
}

void KSpreadCanvas::mouseMoveEvent( QMouseEvent * _ev )
{
  // Dont allow modifications if document is readonly. Selecting is no modification
  if ( (!m_pView->koDocument()->isReadWrite()) && (m_eMouseAction!=Mark))
    return;

  if ( m_dragging )
    return;

  if ( m_dragStart.x() != -1 )
  {
    QPoint p ( (int) _ev->pos().x() + xOffset(),
               (int) _ev->pos().y() + yOffset() );

    if ( ( m_dragStart - p ).manhattanLength() > 4 )
    {
      m_dragging = true;
      startTheDrag();
      m_dragStart.setX( -1 );
    }
    m_dragging = false;
    return;
  }

  // Working on this table ?
  KSpreadSheet *table = activeTable();
  if ( !table )
    return;

  // Special handling for choose mode.
  if ( m_bChoose )
  {
    chooseMouseMoveEvent( _ev );
    return;
  }

  double ev_PosX = doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  double ev_PosY = doc()->unzoomItY( _ev->pos().y() ) + yOffset();
  double dwidth = doc()->unzoomItX( width() );

  double xpos;
  double ypos;
  int col;
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
    col = table->leftColumn( dwidth - ev_PosX, xpos );
  else
    col = table->leftColumn( ev_PosX, xpos );
  int row  = table->topRow( ev_PosY, ypos );

  if ( col > KS_colMax || row > KS_rowMax )
  {
    return;
  }

  QRect rct( selectionInfo()->selection() );

  QRect r1;
  QRect r2;

  double lx = table->dblColumnPos( rct.left() );
  double rx = table->dblColumnPos( rct.right() + 1 );
  double ty = table->dblRowPos( rct.top() );
  double by = table->dblRowPos( rct.bottom() + 1 );

  r1.setLeft( lx - 1 );
  r1.setTop( ty - 1 );
  r1.setRight( rx + 1 );
  r1.setBottom( by + 1 );

  r2.setLeft( lx + 1 );
  r2.setTop( ty + 1 );
  r2.setRight( rx - 1 );
  r2.setBottom( by - 1 );

  QRect selectionHandle = m_pView->selectionInfo()->selectionHandleArea();

  // Test whether the mouse is over some anchor
  {
    KSpreadCell *cell = table->visibleCellAt( col, row );
    QString anchor = cell->testAnchor( doc()->zoomItX( ev_PosX - xpos ),
                                       doc()->zoomItY( ev_PosY - ypos ) );
    if ( !anchor.isEmpty() && anchor != m_strAnchor )
      setCursor( KCursor::handCursor() );

    m_strAnchor = anchor;
  }

  if ( selectionHandle.contains( QPoint( doc()->zoomItX( ev_PosX ),
                                         doc()->zoomItY( ev_PosY ) ) ) )
  {
    //If the cursor is over the hanlde, than it might be already on the next cell.
    //Recalculate the cell!
    if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
      col = table->leftColumn( dwidth - ev_PosX - doc()->unzoomItX( 2 ), xpos );
    else
      col  = table->leftColumn( ev_PosX - doc()->unzoomItX( 2 ), xpos );
    row  = table->topRow( ev_PosY - doc()->unzoomItY( 2 ), ypos );

    if ( !table->isProtected() )
      setCursor( sizeFDiagCursor );
  }
  else if ( !m_strAnchor.isEmpty() )
  {
    if ( !table->isProtected() )
      setCursor( KCursor::handCursor() );
  }
  else if ( r1.contains( QPoint( ev_PosX, ev_PosY ) )
            && !r2.contains( QPoint( ev_PosX, ev_PosY ) ) )
    setCursor( KCursor::handCursor() );
  else
    setCursor( arrowCursor );

  // No marking, selecting etc. in progess? Then quit here.
  if ( m_eMouseAction == NoAction )
    return;

  // Set the new extent of the selection
  gotoLocation( QPoint( col, row ), table, true );
}

void KSpreadCanvas::mouseReleaseEvent( QMouseEvent* _ev )
{
  if ( m_scrollTimer->isActive() )
    m_scrollTimer->stop();

  m_bMousePressed = false;

  if ( m_bChoose )
  {
    chooseMouseReleaseEvent( _ev );
    return;
  }

  KSpreadSheet *table = activeTable();
  if ( !table )
    return;

  KSpreadSelection* selectionInfo = m_pView->selectionInfo();
  QRect s( selection() );

  if ( selectionInfo->singleCellSelection() )
  {
    KSpreadCell* cell = table->cellAt( selectionInfo->marker() );
    cell->clicked( this );
  }

  // The user started the drag in the lower right corner of the marker ?
  if ( m_eMouseAction == ResizeCell && !table->isProtected() )
  {
    QPoint selectionAnchor = selectionInfo->selectionAnchor();
    int x = selectionAnchor.x();
    int y = selectionAnchor.y();
    if ( x > s.left())
        x = s.left();
    if ( y > s.top() )
        y = s.top();
    KSpreadCell *cell = table->nonDefaultCell( x, y );
    if ( !m_pView->doc()->undoLocked() )
    {
        KSpreadUndoMergedCell *undo = new KSpreadUndoMergedCell( m_pView->doc(),
                        table, x, y, cell->extraXCells(), cell->extraYCells() );
        m_pView->doc()->addCommand( undo );
    }
    cell->forceExtraCells( x, y,
                           abs( s.right() - s.left() ),
                           abs( s.bottom() - s.top() ) );

    m_pView->updateEditWidget();
    if ( table->getAutoCalc() ) table->recalc();
  }
  else if ( m_eMouseAction == AutoFill && !table->isProtected() )
  {
    QRect dest = s;
    table->autofill( m_rctAutoFillSrc, dest );

    m_pView->updateEditWidget();
  }
  // The user started the drag in the middle of a cell ?
  else if ( m_eMouseAction == Mark )
  {
    m_pView->updateEditWidget();
  }

  m_eMouseAction = NoAction;
  m_dragging = false;
  m_dragStart.setX( -1 );
}

void KSpreadCanvas::processClickSelectionHandle( QMouseEvent *event )
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


void KSpreadCanvas::extendCurrentSelection( QPoint cell )
{
  KSpreadSheet* table = activeTable();
  QPoint chooseAnchor = selectionInfo()->getChooseAnchor();
//  KSpreadCell* destinationCell = table->cellAt(cell);

  if ( m_bChoose )
  {
    if ( chooseAnchor.x() == 0 )
    {
      updateChooseRect( cell, cell );
    }
    else
    {
      updateChooseRect( cell, chooseAnchor );
    }
  }
  else
  {

    /* the selection simply becomes a box with the anchor and given cell as
       opposite corners
    */
    selectionInfo()->setSelection( cell, selectionInfo()->selectionAnchor(),
                                   table );
  }
}

void KSpreadCanvas::processLeftClickAnchor()
{
  bool isLink = (m_strAnchor.find("http://") == 0 || m_strAnchor.find("mailto:") == 0
                 || m_strAnchor.find("ftp://") == 0 || m_strAnchor.find("file:") == 0 );
  bool isLocalLink = (m_strAnchor.find("file:") == 0);
  if ( isLink )
  {
    QString question = i18n("Do you want to open this link to '%1'?\n").arg(m_strAnchor);
    if ( isLocalLink )
    {
      question += i18n("Note that opening a link to a local file may "
                       "compromise your system's security.");
    }

    // this will also start local programs, so adding a "don't warn again"
    // checkbox will probably be too dangerous
    int choice = KMessageBox::warningYesNo(this, question, i18n("Open Link?"));
    if ( choice == KMessageBox::Yes )
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
  if ( m_bChoose )
  {
    chooseMousePressEvent( _ev );
    return;
  }

  KSpreadSheet *table = activeTable();

  if ( !table )
    return;

  double dwidth = 0.0;
  double ev_PosX;
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    dwidth = doc()->unzoomItX( width() );
    ev_PosX = dwidth - doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  }
  else
    ev_PosX = doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  double ev_PosY = doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pEditor )
  {
    deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  // Remember current values.
  QRect s( selection() );

  // Did we click in the lower right corner of the marker/marked-area ?
  if ( selectionInfo()->selectionHandleArea().contains( QPoint( doc()->zoomItX( ev_PosX ),
                                                                doc()->zoomItY( ev_PosY ) ) ) )
  {
    processClickSelectionHandle( _ev );
    return;
  }

  // In which cell did the user click ?
  double xpos;
  double ypos;
  int col  = table->leftColumn( ev_PosX, xpos );
  int row  = table->topRow( ev_PosY, ypos );

  {
    // start drag ?
    QRect rct( selectionInfo()->selection() );

    QRect r1;
    QRect r2;
    {
      double lx = table->dblColumnPos( rct.left() );
      double rx = table->dblColumnPos( rct.right() + 1 );
      double ty = table->dblRowPos( rct.top() );
      double by = table->dblRowPos( rct.bottom() + 1 );

      r1.setLeft( lx - 1 );
      r1.setTop( ty - 1 );
      r1.setRight( rx + 1 );
      r1.setBottom( by + 1 );

      r2.setLeft( lx + 1 );
      r2.setTop( ty + 1 );
      r2.setRight( rx - 1 );
      r2.setBottom( by - 1 );
    }

    m_dragStart.setX( -1 );

    if ( r1.contains( QPoint( ev_PosX, ev_PosY ) )
         && !r2.contains( QPoint( ev_PosX, ev_PosY ) ) )
    {
      m_dragStart.setX( (int) ev_PosX );
      m_dragStart.setY( (int) ev_PosY );

      return;
    }
  }

  //  kdDebug() << "Clicked in cell " << col << ", " << row << endl;

  //you cannot move marker when col > KS_colMax or row > KS_rowMax
  if ( col > KS_colMax || row > KS_rowMax)
  {
    kdDebug(36001) << "KSpreadCanvas::mousePressEvent: col or row is out of range: col: " << col << " row: " << row << endl;
    return;
  }

  // Extending an existing selection with the shift button ?
  if ( m_pView->koDocument()->isReadWrite() && s.right() != KS_colMax &&
       s.bottom() != KS_rowMax && _ev->state() & ShiftButton )
  {
    gotoLocation( QPoint( col, row ), activeTable(), true );
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
    updatePosWidget();
  }
  else if ( _ev->button() == LeftButton )
  {
    m_eMouseAction = Mark;
    gotoLocation( QPoint( col, row ), activeTable(), false );
  }
  else if ( _ev->button() == RightButton &&
            !s.contains( QPoint( col, row ) ) )
  {
    // No selection or the mouse press was outside of an existing selection ?
    gotoLocation( QPoint( col, row ), activeTable(), false );
  }

  // Paste operation with the middle button ?
  if ( _ev->button() == MidButton )
  {
    if ( m_pView->koDocument()->isReadWrite() && !table->isProtected() )
    {
      selectionInfo()->setMarker( QPoint( col, row ), table );
      table->paste( QRect(marker(), marker()) );
      table->setRegionPaintDirty(QRect(marker(), marker()));
    }
    updatePosWidget();
  }

  // Update the edit box
  m_pView->updateEditWidgetOnPress();

  // Context menu ?
  if ( _ev->button() == RightButton )
  {
    updatePosWidget();
    // TODO: Handle anchor
    QPoint p = mapToGlobal( _ev->pos() );
    m_pView->openPopupMenu( p );
  }
}

void KSpreadCanvas::startTheDrag()
{
  KSpreadSheet * table = activeTable();
  if ( !table )
    return;

  // right area for start dragging
  KSpreadTextDrag * d = new KSpreadTextDrag( this );
  setCursor( KCursor::handCursor() );

  QRect rct( selectionInfo()->selection() );
  QDomDocument doc = table->saveCellRect( rct );

  // Save to buffer
  QBuffer buffer;
  buffer.open( IO_WriteOnly );
  QTextStream str( &buffer );
  str.setEncoding( QTextStream::UnicodeUTF8 );
  str << doc;
  buffer.close();

  d->setPlain( table->copyAsText( selectionInfo() ) );
  d->setKSpread( buffer.buffer() );

  d->dragCopy();
  setCursor( KCursor::arrowCursor() );
}

void KSpreadCanvas::chooseMouseMoveEvent( QMouseEvent * _ev )
{
  if ( !m_bMousePressed )
    return;

  KSpreadSheet * table = activeTable();
  if ( !table )
    return;

  double tmp;
  double ev_PosX = doc()->unzoomItX( _ev->pos().x() );
  double ev_PosY = doc()->unzoomItY( _ev->pos().y() );
  int col = table->leftColumn( (ev_PosX + xOffset()), tmp ); // TODO
  int row = table->topRow( (ev_PosY + yOffset()), tmp );

  if ( col > KS_colMax || row > KS_rowMax )
  {
    return;
  }

  QPoint chooseMarker = selectionInfo()->getChooseMarker();

  // Nothing changed ?
  if ( row == chooseMarker.y() && col == chooseMarker.x() )
  {
    return;
  }

  gotoLocation( QPoint( col, row ), table, ( m_eMouseAction != NoAction ) );
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


  double ev_PosX = doc()->unzoomItX( _ev->pos().x() );
  double ev_PosY = doc()->unzoomItY( _ev->pos().y() );
  double ypos, xpos;
  int col = table->leftColumn( (ev_PosX + xOffset()), xpos ); // TODO rtl
  int row = table->topRow( (ev_PosY + yOffset()), ypos );

  if ( col > KS_colMax || row > KS_rowMax )
  {
    return;
  }

  bool extend = ( ( ( !util_isColumnSelected(selection() ) ) &&
                    ( !util_isRowSelected(selection() ) ) ) &&
                  ( _ev->state() & ShiftButton ) );

  gotoLocation( QPoint( col, row ), activeTable(), extend );

  if ( _ev->button() == LeftButton )
  {
    m_eMouseAction = Mark;
  }
  return;
}

void KSpreadCanvas::mouseDoubleClickEvent( QMouseEvent*  )
{
  if ( m_pView->koDocument()->isReadWrite() && activeTable() )
    createEditor();
}

void KSpreadCanvas::wheelEvent( QWheelEvent* _ev )
{
  if ( _ev->orientation() == Qt::Vertical )
  {
    if ( vertScrollBar() )
      QApplication::sendEvent( vertScrollBar(), _ev );
  }
  else if ( horzScrollBar() )
  {
    QApplication::sendEvent( horzScrollBar(), _ev );
  }
}

void KSpreadCanvas::paintEvent( QPaintEvent* _ev )
{
  if ( m_pDoc->isLoading() )
    return;

  KSpreadSheet* table = activeTable();
  if ( !table )
    return;

  // ElapsedTime et( "KSpreadCanvas::paintEvent" );

  double dwidth = doc()->unzoomItX( width() );
  KoRect rect = doc()->unzoomRect( _ev->rect() & QWidget::rect() );
  rect.moveBy( xOffset(), yOffset() );

  KoPoint tl = rect.topLeft();
  KoPoint br = rect.bottomRight();

  double tmp;
  int left_col;
  int right_col;
  //Philipp: I don't know why we need the +1, but otherwise we don't get it correctly
  //Testcase: Move a dialog slowly up left. Sometimes the top/left most points are not painted
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    right_col = table->leftColumn( dwidth - tl.x(), tmp );
    left_col  = table->rightColumn( dwidth - br.x() + 1.0 );
  }
  else
  {
    left_col  = table->leftColumn( tl.x(), tmp );
    right_col = table->rightColumn( br.x() + 1.0 );
  }
  int top_row = table->topRow( tl.y(), tmp );
  int bottom_row = table->bottomRow( br.y() + 1.0 );

  QRect vr( QPoint(left_col, top_row),
            QPoint(right_col, bottom_row) );
  m_pView->doc()->emitBeginOperation( false );
  table->setRegionPaintDirty( vr );
  m_pView->doc()->emitEndOperation( vr );
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
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();
    m_bMousePressed = false;
}

void KSpreadCanvas::dragMoveEvent( QDragMoveEvent * _ev )
{
  KSpreadSheet * table = activeTable();
  if ( !table )
  {
    _ev->ignore();
    return;
  }

  _ev->accept( KSpreadTextDrag::canDecode( _ev ) );

  double xpos = table->dblColumnPos( selectionInfo()->selection().left() );
  double ypos = table->dblRowPos( selectionInfo()->selection().top() );
  double width  = table->columnFormat( selectionInfo()->selection().left() )->dblWidth( this );
  double height = table->rowFormat( selectionInfo()->selection().top() )->dblHeight( this );

  QRect r1( xpos - 1, ypos - 1, width + 3, height + 3 );
  double ev_PosX = doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  double ev_PosY = doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  if ( r1.contains( QPoint( ev_PosX, ev_PosY ) ) )
    _ev->ignore( r1 );
}

void KSpreadCanvas::dragLeaveEvent( QDragLeaveEvent * )
{
  if ( m_scrollTimer->isActive() )
    m_scrollTimer->stop();
}

void KSpreadCanvas::dropEvent( QDropEvent * _ev )
{
  m_dragging = false;
  KSpreadSheet * table = activeTable();
  if ( !table || table->isProtected() )
  {
    _ev->ignore();
    return;
  }

  double xpos = table->dblColumnPos( selectionInfo()->selection().left() );
  double ypos = table->dblRowPos( selectionInfo()->selection().top() );
  double width  = table->columnFormat( selectionInfo()->selection().left() )->dblWidth( this );
  double height = table->rowFormat( selectionInfo()->selection().top() )->dblHeight( this );

  QRect r1( xpos - 1, ypos - 1, width + 3, height + 3 );
  double ev_PosX = doc()->unzoomItX( _ev->pos().x() ) + xOffset();
  double ev_PosY = doc()->unzoomItY( _ev->pos().y() ) + yOffset();

  if ( r1.contains( QPoint( ev_PosX, ev_PosY ) ) )
  {
    _ev->ignore( );
    return;
  }
  else
    _ev->accept( );

  double tmp;
  int col = table->leftColumn( ev_PosX, tmp );
  int row = table->topRow( ev_PosY, tmp );

  if ( !KSpreadTextDrag::canDecode( _ev ) )
  {
    _ev->ignore();
    return;
  }

  QByteArray b;

  bool makeUndo = true;

  if ( _ev->provides( KSpreadTextDrag::selectionMimeType() ) )
  {
    if ( KSpreadTextDrag::target() == _ev->source() )
    {
      if ( !m_pDoc->undoLocked() )
      {
        KSpreadUndoDragDrop * undo
          = new KSpreadUndoDragDrop( m_pDoc, table, selectionInfo()->selection(),
                                     QRect( col, row, selectionInfo()->selection().width(),
                                            selectionInfo()->selection().height() ) );
        m_pDoc->addCommand( undo );
        makeUndo = false;
      }
      table->deleteSelection( selectionInfo(), false );
    }


    b = _ev->encodedData( KSpreadTextDrag::selectionMimeType() );
    table->paste( b, QRect( col, row, 1, 1 ), makeUndo );

    if ( _ev->source() == this )
      _ev->acceptAction();
    _ev->accept();
  }
  else
  {
    QString text;
    if ( !QTextDrag::decode( _ev, text ) )
    {
      _ev->ignore();
      return;
    }
    //    if ( KSpreadTextDrag::target() == _ev->source() )
    //      table->deleteSelection( selectionInfo() );

    table->pasteTextPlain( text, QRect( col, row, 1, 1 ) );
    _ev->accept();
    if ( _ev->source() == this )
      _ev->acceptAction();

    return;
  }
}

void KSpreadCanvas::resizeEvent( QResizeEvent* _ev )
{
    double ev_Width = doc()->unzoomItX( _ev->size().width() );
    double ev_Height = doc()->unzoomItY( _ev->size().height() );

    // If we rise horizontally, then check if we are still within the valid area (KS_colMax)
    if ( _ev->size().width() > _ev->oldSize().width() )
    {
        if ( ( xOffset() + ev_Width ) >
               doc()->zoomItX( activeTable()->sizeMaxX() ) )
        {
            horzScrollBar()->setRange( 0, doc()->zoomItX( activeTable()->sizeMaxX() - ev_Width ) );
        }
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().width() < _ev->oldSize().width() )
    {
        if ( horzScrollBar()->maxValue() ==
             int( doc()->zoomItX( activeTable()->sizeMaxX() ) - ev_Width ) )
        {
            horzScrollBar()->setRange( 0, doc()->zoomItX( activeTable()->sizeMaxX() - ev_Width ) );
        }
    }

    // If we rise vertically, then check if we are still within the valid area (KS_rowMax)
    if ( _ev->size().height() > _ev->oldSize().height() )
    {
        if ( ( yOffset() + ev_Height ) >
             doc()->zoomItY( activeTable()->sizeMaxY() ) )
        {
            vertScrollBar()->setRange( 0, doc()->zoomItY( activeTable()->sizeMaxY() - ev_Height ) );
        }
    }
    // If we lower vertically, then check if the range should represent the maximum range
    else if ( _ev->size().height() < _ev->oldSize().height() )
    {
        if ( vertScrollBar()->maxValue() ==
             int( doc()->zoomItY( activeTable()->sizeMaxY() ) - ev_Height ) )
        {
            vertScrollBar()->setRange( 0, doc()->zoomItY( activeTable()->sizeMaxY() - ev_Height ) );
        }
    }
}

QRect KSpreadCanvas::moveDirection( KSpread::MoveTo direction, bool extendSelection )
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
  RowFormat *rl = NULL;
  ColumnFormat *cl = NULL;
  switch (direction)
    /* for each case, figure out how far away the next cell is and then keep
       going one row/col at a time after that until a visible row/col is found

       NEVER use cell->column() or cell->row() -- it might be a default cell
    */
  {
    case KSpread::Bottom:
      offset = cell->mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = activeTable()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHide())
      {
        offset++;
        rl = activeTable()->rowFormat( cursor.y() + offset );
      }

      destination = QPoint(cursor.x(), QMIN(cursor.y() + offset, KS_rowMax));
      break;
    case KSpread::Top:
      offset = (cellCorner.y() - cursor.y()) - 1;
      rl = activeTable()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) >= 1) && rl->isHide())
      {
        offset--;
        rl = activeTable()->rowFormat( cursor.y() + offset );
      }
      destination = QPoint(cursor.x(), QMAX(cursor.y() + offset, 1));
      break;
    case KSpread::Left:
      offset = (cellCorner.x() - cursor.x()) - 1;
      cl = activeTable()->columnFormat( cursor.x() + offset );
      while ( ((cursor.x() + offset) >= 1) && cl->isHide())
      {
        offset--;
        cl = activeTable()->columnFormat( cursor.x() + offset );
      }
      destination = QPoint(QMAX(cursor.x() + offset, 1), cursor.y());
      break;
    case KSpread::Right:
      offset = cell->mergedXCells() - (cursor.x() - cellCorner.x()) + 1;
      cl = activeTable()->columnFormat( cursor.x() + offset );
      while ( ((cursor.x() + offset) <= KS_colMax) && cl->isHide())
      {
        offset++;
        cl = activeTable()->columnFormat( cursor.x() + offset );
      }
      destination = QPoint(QMIN(cursor.x() + offset, KS_colMax), cursor.y());
      break;
    case KSpread::BottomFirst:
      offset = cell->mergedYCells() - (cursor.y() - cellCorner.y()) + 1;
      rl = activeTable()->rowFormat( cursor.y() + offset );
      while ( ((cursor.y() + offset) <= KS_rowMax) && rl->isHide())
      {
        ++offset;
        rl = activeTable()->rowFormat( cursor.y() + offset );
      }

      destination = QPoint( 1, QMIN( cursor.y() + offset, KS_rowMax ) );
      break;
  }

  gotoLocation(destination, activeTable(), extendSelection);
  m_pView->updateEditWidget();

  return QRect( cursor, destination );
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
  if (event->state() & Qt::ShiftButton)
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
      break;
     case KSpread::BottomFirst:
      direction = KSpread::BottomFirst;
      break;
    }
  }

  /* never extend a selection with the enter key -- the shift key reverses
     direction, not extends the selection
  */
  QRect r( moveDirection( direction, false ) );
  m_pDoc->emitEndOperation( r );
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
    if (activeTable()->layoutDirection()==KSpreadSheet::RightToLeft)
      direction = KSpread::Right;
    else
      direction = KSpread::Left;
    break;
  case Key_Right:
    if (activeTable()->layoutDirection()==KSpreadSheet::RightToLeft)
      direction = KSpread::Left;
    else
      direction = KSpread::Right;
    break;
  case Key_Tab:
      direction = KSpread::Right;
      break;
  default:
    Q_ASSERT(false);
    break;
  }

  QRect r( moveDirection( direction, makingSelection ) );
  m_pDoc->emitEndOperation( r );
}

void KSpreadCanvas::processEscapeKey(QKeyEvent * event)
{
  if ( m_pEditor )
    deleteEditor( false );

  event->accept(); // ?
  QPoint cursor;

  if (m_bChoose)
  {
    cursor = selectionInfo()->getChooseCursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = selectionInfo()->cursorPosition();
  }
  else
    cursor = selectionInfo()->cursorPosition();

  m_pDoc->emitEndOperation( QRect( cursor, cursor ) );
}

bool KSpreadCanvas::processHomeKey(QKeyEvent* event)
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

    return false;
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
      destination = QPoint( 1, 1 );
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

    if ( selectionInfo()->marker() == destination )
    {
      m_pDoc->emitEndOperation( QRect( destination, destination ) );
      return false;
    }

    gotoLocation( destination, activeTable(), makingSelection );
  }
  return true;
}

bool KSpreadCanvas::processEndKey( QKeyEvent *event )
{
  bool makingSelection = event->state() & ShiftButton;
  KSpreadSheet* table = activeTable();
  KSpreadCell* cell = NULL;
  QPoint marker = m_bChoose ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();


  // move to the last used cell in the row
  // We are in edit mode -> go beginning of line
  if ( m_pEditor )
  {
    // (David) Do this for text editor only, not formula editor...
    // Don't know how to avoid this hack (member var for editor type ?)
    if ( m_pEditor->inherits("KSpreadTextEditor") )
      QApplication::sendEvent( m_pEditWidget, event );
    // TODO: What to do for a formula editor ?
    m_pDoc->emitEndOperation( QRect( marker, marker ) );
    return false;
  }
  else
  {
    int col = 1;

    cell = table->getLastCellRow(marker.y());
    while (cell != NULL && cell->column() > markerColumn() && cell->isEmpty())
    {
      cell = table->getNextCellLeft(cell->column(), cell->row());
    }

    col = (cell == NULL) ? KS_colMax : cell->column();

    QPoint destination( col, marker.y() );
    if ( destination == marker )
    {
      m_pDoc->emitEndOperation( QRect( destination, destination ) );
      return false;
    }

    gotoLocation( destination, activeTable(), makingSelection );
  }
  return true;
}

bool KSpreadCanvas::processPriorKey(QKeyEvent *event)
{
  bool makingSelection = event->state() & ShiftButton;
  if (!m_bChoose)
  {
    deleteEditor( true );
  }

  QPoint marker = m_bChoose ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();

  QPoint destination(marker.x(), QMAX(1, marker.y() - 10));
  if ( destination == marker )
  {
    m_pDoc->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  gotoLocation(destination, activeTable(), makingSelection);

  return true;
}

bool KSpreadCanvas::processNextKey(QKeyEvent *event)
{
  bool makingSelection = event->state() & ShiftButton;

  if (!m_bChoose)
  {
    deleteEditor( true /*save changes*/ );
  }

  QPoint marker = m_bChoose ?
    selectionInfo()->getChooseMarker() : selectionInfo()->marker();
  QPoint destination(marker.x(), QMAX(1, marker.y() + 10));

  if ( marker == destination )
  {
    m_pDoc->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  gotoLocation(destination, activeTable(), makingSelection);

  return true;
}

void KSpreadCanvas::processDeleteKey(QKeyEvent* /* event */)
{
  activeTable()->clearTextSelection( selectionInfo() );
  m_pView->editWidget()->setText( "" );

  QPoint cursor;

  if ( m_bChoose )
  {
    cursor = selectionInfo()->getChooseCursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = selectionInfo()->cursorPosition();
  }
  else
    cursor = selectionInfo()->cursorPosition();

  m_pDoc->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void KSpreadCanvas::processF2Key(QKeyEvent* /* event */)
{
  m_pView->editWidget()->setFocus();
  if ( m_pEditor )
    m_pView->editWidget()->setCursorPosition( m_pEditor->cursorPosition() - 1 );
  m_pView->editWidget()->cursorForward( false );

  QPoint cursor;

  if ( m_bChoose )
  {
    cursor = selectionInfo()->getChooseCursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = selectionInfo()->cursorPosition();
  }
  else
    cursor = selectionInfo()->cursorPosition();

  m_pDoc->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void KSpreadCanvas::processF4Key(QKeyEvent* event)
{
  /* I have no idea what this is doing.  But it was in the code so I'm leaving it
   */
  if ( m_pEditor )
  {
    m_pEditor->handleKeyPressEvent( event );
    m_pView->editWidget()->setFocus();
    m_pView->editWidget()->setCursorPosition( m_pEditor->cursorPosition() );
  }
  QPoint cursor;

  if ( m_bChoose )
  {
    cursor = selectionInfo()->getChooseCursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = selectionInfo()->cursorPosition();
  }
  else
    cursor = selectionInfo()->cursorPosition();

  m_pDoc->emitEndOperation( QRect( cursor, cursor ) );
  return;
}

void KSpreadCanvas::processOtherKey(QKeyEvent *event)
{
  // No null character ...
  if ( event->text().isEmpty() || !m_pView->koDocument()->isReadWrite()
       || !activeTable() || activeTable()->isProtected() )
  {
    event->accept();
  }
  else
  {
    if ( !m_pEditor && !m_bChoose )
    {
      // Switch to editing mode
      createEditor( CellEditor );
      m_pEditor->handleKeyPressEvent( event );
    }
    else if ( m_pEditor )
      m_pEditor->handleKeyPressEvent( event );
  }

  QPoint cursor;

  if ( m_bChoose )
  {
    cursor = selectionInfo()->getChooseCursor();
    /* if the cursor is unset, pretend we're starting at the regular cursor */
    if (cursor.x() == 0 || cursor.y() == 0)
      cursor = selectionInfo()->cursorPosition();
  }
  else
    cursor = selectionInfo()->cursorPosition();

  m_pDoc->emitEndOperation( QRect( cursor, cursor ) );

  return;
}

bool KSpreadCanvas::processControlArrowKey( QKeyEvent *event )
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
  switch ( event->key() )
  {
    //Ctrl+Key_Up
   case Key_Up:

    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.y() != 1))
    {
      lastCell = cell;
      row = marker.y()-1;
      cell = table->cellAt(cell->column(), row);
      while ((cell != NULL) && (row > 0) && (!cell->isEmpty()) )
      {
        if (!(table->rowFormat(cell->row())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = FALSE;
        }
        row--;
        if ( row > 0 )
          cell = table->cellAt(cell->column(), row);
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = table->getNextCellUp(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (table->rowFormat(cell->row())->isHide())))
      {
        cell = table->getNextCellUp(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      row = 1;
    else
      row = cell->row();

    while ( table->rowFormat(row)->isHide() )
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
      while ((cell != NULL) && (row < KS_rowMax) && (!cell->isEmpty()) )
      {
        if (!(table->rowFormat(cell->row())->isHide()))
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

      while ((cell != NULL) &&
            (cell->isEmpty() || (table->rowFormat(cell->row())->isHide())))
      {
        cell = table->getNextCellDown(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      row = marker.y();
    else
      row = cell->row();

    while ( table->rowFormat(row)->isHide() )
    {
      row--;
    }

    destination.setX(marker.x());
    destination.setY(row);
    break;

  //Ctrl+Key_Left
  case Key_Left:

  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = table->cellAt(col, cell->row());
      while ((cell != NULL) && (col < KS_colMax) && (!cell->isEmpty()) )
      {
        if (!(table->columnFormat(cell->column())->isHide()))
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

      while ((cell != NULL) &&
            (cell->isEmpty() || (table->columnFormat(cell->column())->isHide())))
      {
        cell = table->getNextCellRight(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = marker.x();
    else
      col = cell->column();

    while ( table->columnFormat(col)->isHide() )
    {
      col--;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
  else
  {
    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = table->cellAt(col, cell->row());
      while ((cell != NULL) && (col > 0) && (!cell->isEmpty()) )
      {
        if (!(table->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = FALSE;
        }
        col--;
        if ( col > 0 )
            cell = table->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = table->getNextCellLeft(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (table->columnFormat(cell->column())->isHide())))
      {
        cell = table->getNextCellLeft(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = 1;
    else
      col = cell->column();

    while ( table->columnFormat(col)->isHide() )
    {
      col++;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
    break;

  //Ctrl+Key_Right
  case Key_Right:

  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != 1))
    {
      lastCell = cell;
      col = marker.x()-1;
      cell = table->cellAt(col, cell->row());
      while ((cell != NULL) && (col > 0) && (!cell->isEmpty()) )
      {
        if (!(table->columnFormat(cell->column())->isHide()))
        {
          lastCell = cell;
          searchThroughEmpty = FALSE;
        }
        col--;
        if ( col > 0 )
            cell = table->cellAt(col, cell->row());
      }
      cell = lastCell;
    }
    if (searchThroughEmpty)
    {
      cell = table->getNextCellLeft(marker.x(), marker.y());

      while ((cell != NULL) &&
            (cell->isEmpty() || (table->columnFormat(cell->column())->isHide())))
      {
        cell = table->getNextCellLeft(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = 1;
    else
      col = cell->column();

    while ( table->columnFormat(col)->isHide() )
    {
      col++;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
  else
  {
    cell = table->cellAt( marker.x(), marker.y() );
    if ( (cell != NULL) && (!cell->isEmpty()) && (marker.x() != KS_colMax))
    {
      lastCell = cell;
      col = marker.x()+1;
      cell = table->cellAt(col, cell->row());
      while ((cell != NULL) && (col < KS_colMax) && (!cell->isEmpty()) )
      {
        if (!(table->columnFormat(cell->column())->isHide()))
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

      while ((cell != NULL) &&
            (cell->isEmpty() || (table->columnFormat(cell->column())->isHide())))
      {
        cell = table->getNextCellRight(cell->column(), cell->row());
      }
    }

    if (cell == NULL)
      col = marker.x();
    else
      col = cell->column();

    while ( table->columnFormat(col)->isHide() )
    {
      col--;
    }

    destination.setX(col);
    destination.setY(marker.y());
  }
    break;

  }

  if ( marker == destination )
  {
    m_pDoc->emitEndOperation( QRect( destination, destination ) );
    return false;
  }

  gotoLocation( destination, table, makingSelection );
  return true;
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

  m_pDoc->emitBeginOperation(false);
  switch( _ev->key() )
  {
   case Key_Return:
   case Key_Enter:
    processEnterKey( _ev );
    return;
    break;
   case Key_Down:
   case Key_Up:
   case Key_Left:
   case Key_Right:
   case Key_Tab: /* a tab behaves just like a right arrow */
    if (_ev->state() & ControlButton)
    {
      if ( !processControlArrowKey( _ev ) )
        return;
    }
    else
    {
      processArrowKey( _ev );
      return;
    }
    break;

   case Key_Escape:
    processEscapeKey( _ev );
    return;
    break;

   case Key_Home:
    if ( !processHomeKey( _ev ) )
      return;
    break;

   case Key_End:
    if ( !processEndKey( _ev ) )
      return;
    break;

   case Key_Prior:  /* Page Up */
    if ( !processPriorKey( _ev ) )
      return;
    break;

   case Key_Next:   /* Page Down */
    if ( !processNextKey( _ev ) )
      return;
    break;

   case Key_Delete:
    processDeleteKey( _ev );
    return;
    break;

   case Key_F2:
    processF2Key( _ev );
    return;
    break;

   case Key_F4:
    processF4Key( _ev );
    return;
    break;

   default:
    processOtherKey( _ev );
    return;
    break;
  }
  m_pDoc->emitEndOperation( table->visibleRect( this ) );
  return;
}

double KSpreadCanvas::getDouble( KSpreadCell * cell )
{
  cell->setFactor( 1.0 );
  if ( cell->isDate() )
  {
    QDate date = cell->value().asDate();
    QDate dummy(1900, 1, 1);
    return (dummy.daysTo( date ) + 1);
  }
  if ( cell->isTime() )
  {
    QTime time  = cell->value().asTime();
    QTime dummy;
    return dummy.secsTo( time );
  }
  if ( cell->value().isNumber() )
    return cell->value().asFloat();

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
  cell->setFormatType( KSpreadFormat::SecondeTime );
  QTime time = cell->value().asDateTime().time();
  int msec = (int) ( (cell->value().asFloat() - (int) cell->value().asFloat())* 1000 );
  time = time.addMSecs( msec );
  cell->setCellText( time.toString() );
}

void KSpreadCanvas::convertToDate( KSpreadCell * cell )
{
  if ( cell->isDefault() || cell->isEmpty() )
    return;
  if ( cell->isTime() )
    cell->setValue( getDouble( cell ) );
  cell->setFormatType( KSpreadFormat::ShortDate );
  cell->setFactor( 1.0 );

  QDate date(1900, 1, 1);
  date = date.addDays( (int) cell->value().asFloat() - 1 );
  date = cell->value().asDateTime().date();
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

  m_pDoc->emitBeginOperation(false);
  table->setRegionPaintDirty( rect );
  int right  = rect.right();
  int bottom = rect.bottom();

  if ( !m_pDoc->undoLocked() )
  {
    QString dummy;
    KSpreadUndoCellFormat * undo = new KSpreadUndoCellFormat( m_pDoc, table, rect, dummy );
    m_pDoc->addCommand( undo );
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
           m_pDoc->emitEndOperation( rect );
          return false;
        } // switch

        cell = table->getNextCellRight( cell->column(), r );
      } // while (cell)
      RowFormat * rw = table->nonDefaultRowFormat( r );
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
        rw->setFormatType( KSpreadFormat::SecondeTime );
        rw->setFactor( 1.0 );
        break;

       case Key_NumberSign:
        rw->setFormatType( KSpreadFormat::ShortDate );
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
         m_pDoc->emitEndOperation( rect );
        return false;
      }
      table->emit_updateRow( rw, r );
    }

    m_pDoc->emitEndOperation( rect );
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
           m_pDoc->emitEndOperation( rect );
          return false;
        }
        cell = table->getNextCellDown( c, cell->row() );
      }

      ColumnFormat * cw = table->nonDefaultColumnFormat( c );
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
        cw->setFormatType( KSpreadFormat::SecondeTime );
        cw->setFactor( 1.0 );
        break;

       case Key_NumberSign:
        cw->setFormatType( KSpreadFormat::ShortDate );
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
         m_pDoc->emitEndOperation( rect );
         return false;
      }
      table->emit_updateColumn( cw, c );
    }
    m_pDoc->emitEndOperation( rect );
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
         m_pDoc->emitEndOperation( rect );
        return false;
      } // switch
    } // for left .. right
  } // for top .. bottom
  _ev->accept();

  m_pDoc->emitEndOperation( rect );
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

    //Provide progressive scrolling depending on the mouse position
    if ( pos.y() < 0 )
    {
        vertScrollBar()->setValue( vertScrollBar()->value() -
                                   autoScrollAccelerationY( - pos.y() ) );
        select = true;
    }
    else if ( pos.y() > height() )
    {
        vertScrollBar()->setValue( vertScrollBar()->value() +
                                   autoScrollAccelerationY( pos.y() - height() ) );
        select = true;
    }

    if ( pos.x() < 0 )
    {
        horzScrollBar()->setValue( horzScrollBar()->value() -
                                   autoScrollAccelerationX( - pos.x() ) );
        select = true;
    }
    else if ( pos.x() > width() )
    {
        horzScrollBar()->setValue( horzScrollBar()->value() +
                                   autoScrollAccelerationX( pos.x() - width() ) );
        select = true;
    }

    if ( select )
    {
        QMouseEvent * event = new QMouseEvent(QEvent::MouseMove, pos, 0, 0);
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    m_scrollTimer->start( 50 );
}

double KSpreadCanvas::autoScrollAccelerationX( int offset )
{
    switch( static_cast<int>( offset / 20 ) )
    {
        case 0: return 5.0;
        case 1: return 20.0;
        case 2: return doc()->unzoomItX( width() );
        case 3: return doc()->unzoomItX( width() );
        default: return doc()->unzoomItX( width() * 5.0 );
    }
}

double KSpreadCanvas::autoScrollAccelerationY( int offset )
{
    switch( static_cast<int>( offset / 20 ) )
    {
        case 0: return 5.0;
        case 1: return 20.0;
        case 2: return doc()->unzoomItY( height() );
        case 3: return doc()->unzoomItY( height() );
        default: return doc()->unzoomItY( height() * 5.0 );
    }
}

void KSpreadCanvas::deleteEditor( bool saveChanges )
{
  if ( !m_pEditor )
    return;
  // We need to set the line-edit out of edit mode,
  // but only if we are using it (text editor)
  // A bit of a hack - perhaps we should store the editor mode ?
  bool textEditor = true;
  if ( m_pEditor->inherits("KSpreadTextEditor") )
      m_pEditWidget->setEditMode( false );
  else
      textEditor = false;

  QString t = m_pEditor->text();
  // Delete the cell editor first and after that update the document.
  // That means we get a synchronous repaint after the cell editor
  // widget is gone. Otherwise we may get painting errors.
  delete m_pEditor;
  m_pEditor = 0;

  if ( saveChanges && textEditor )
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

  setFocus();
}

void KSpreadCanvas::createEditor()
{
  KSpreadCell * cell = activeTable()->nonDefaultCell( markerColumn(), markerRow(), false );

  if ( !createEditor( CellEditor ) )
      return;
  if ( cell )
      m_pEditor->setText( cell->text() );
}

bool KSpreadCanvas::createEditor( EditorType ed, bool addFocus )
{
  KSpreadSheet * table = activeTable();
  if ( !m_pEditor )
  {
    KSpreadCell * cell = table->nonDefaultCell( marker().x(), marker().y(), false );

    if ( table->isProtected() && !cell->notProtected( marker().x(), marker().y() ) )
      return false;

    if ( ed == CellEditor )
    {
      m_pEditWidget->setEditMode( true );

      m_pEditor = new KSpreadTextEditor( cell, this );
    }

    double w, h;
    double min_w = cell->dblWidth( markerColumn() );
    double min_h = cell->dblHeight( markerRow() );
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

    double xpos;
    if ( table->layoutDirection() == KSpreadSheet::RightToLeft )
    {
      double dwidth = doc()->unzoomItX( width() );
      xpos = dwidth - min_w - table->dblColumnPos( markerColumn() ) - xOffset();
    }
    else
      xpos = table->dblColumnPos( markerColumn() ) - xOffset();

    double ypos = table->dblRowPos( markerRow() ) - yOffset();
    QPalette p = m_pEditor->palette();
    QColorGroup g( p.active() );

    QColor color = cell->textColor( markerColumn(), markerRow() );
    if ( !color.isValid() )
        color = QApplication::palette().active().text();
    g.setColor( QColorGroup::Text, color);

    color = cell->bgColor( markerColumn(), markerRow() );
    if ( !color.isValid() )
        color = g.base();
    g.setColor( QColorGroup::Background, color );

    m_pEditor->setPalette( QPalette( g, p.disabled(), g ) );
    QFont tmpFont = cell->textFont( markerColumn(), markerRow() );
    tmpFont.setPointSizeFloat( 0.01 * doc()->zoom() * tmpFont.pointSizeFloat() );
    m_pEditor->setFont( tmpFont );

    KoRect rect( xpos, ypos, w, h ); //needed to circumvent rounding issue with height/width
    m_pEditor->setGeometry( doc()->zoomRect( rect ) );
    m_pEditor->setMinimumSize( QSize( doc()->zoomItX( min_w ), doc()->zoomItY( min_h ) ) );
    m_pEditor->show();
    //kdDebug(36001) << "FOCUS1" << endl;
    //Laurent 2001-12-05
    //Don't add focus when we create a new editor and
    //we select text in edit widget otherwise we don't delete
    //selected text.
    if ( addFocus )
        m_pEditor->setFocus();
    //kdDebug(36001) << "FOCUS2" << endl;
  }

  return true;
}

void KSpreadCanvas::closeEditor()
{
  if ( m_bChoose )
    return;

  if ( m_pEditor )
  {
    deleteEditor( true ); // save changes
  }
}


void KSpreadCanvas::updateChooseRect(const QPoint &newMarker, const QPoint &newAnchor)
{
  if( !m_bChoose )
    return;

  KSpreadSheet* table = activeTable();

  if ( ! table )
      return;

  QPoint oldAnchor = selectionInfo()->getChooseAnchor();
  QPoint oldMarker = selectionInfo()->getChooseMarker();
  QPoint chooseCursor = selectionInfo()->getChooseCursor();
  QRect oldChooseRect = selectionInfo()->getChooseRect();


  if ( newMarker == oldMarker && newAnchor == oldAnchor )
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
  if ( !newChooseRect.contains(chooseCursor) )
  {
    selectionInfo()->setChooseCursor(table, newMarker);
  }

  m_pDoc->emitBeginOperation();
  setSelectionChangePaintDirty(table, oldChooseRect, newChooseRect);
  repaint();
  m_pDoc->emitEndOperation();

  /* this signal is used in the formula editor to update the text display */
  emit m_pView->sig_chooseSelectionChanged(activeTable(), newChooseRect);

  if ( !m_pEditor )
  {
    length_namecell = 0;
    return;
  }

  /* the rest of this function updates the text showing the choose rect */
  /***** TODO - should this be here? */

  if (newMarker.x() != 0 && newMarker.y() != 0)
    /* don't update the text if we are removing the marker */
  {
    QString name_cell;

    if ( m_chooseStartTable != table )
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


void KSpreadCanvas::setSelectionChangePaintDirty(KSpreadSheet* sheet,
                                                 QRect area1, QRect area2)
{
  QValueList<QRect> cellRegions;

  /* first of all, let's not get confused by an unset region at 0,0,0,0
     Just reset to region to something ridiculous that will be ignored by a paint call
   */
  if (area1.contains(QPoint(0,0)))
  {
    area1.setLeft(-100);
    area1.setRight(-100);
  }

  if (area2.contains(QPoint(0,0)))
  {
    area2.setLeft(-50);
    area2.setRight(-50);
  }

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

  QValueList<QRect>::iterator it = cellRegions.begin();

  while (it != cellRegions.end())
  {
    sheet->setRegionPaintDirty(*it);
    it++;
  }
}

void KSpreadCanvas::ExtendRectBorder(QRect& area)
{
  ColumnFormat *cl;
  RowFormat *rl;
  //look at if column is hiding.
  //if it's hiding refreshing column+1 (or column -1 )
  int left = area.left();
  int right = area.right();
  int top = area.top();
  int bottom = area.bottom();

  //Maybe the case for ridiculous settings, see setSelectionChangePaintDirty
  //No need to extend then, avoids warnings
  if ( left < 1 && right < 1 )
      return;

  if ( right < KS_colMax )
  {
    do
    {
      right++;
      cl = activeTable()->nonDefaultColumnFormat( right );
    } while ( cl->isHide() && right != KS_colMax );
  }
  if ( left > 1 )
  {
    do
    {
      left--;
      cl = activeTable()->nonDefaultColumnFormat( left );
    } while ( cl->isHide() && left != 1);
  }

  if ( bottom < KS_rowMax )
  {
    do
    {
      bottom++;
      rl = activeTable()->nonDefaultRowFormat( bottom );
    } while ( rl->isHide() && bottom != KS_rowMax );
  }

  if ( top > 1 )
  {
    do
    {
      top--;
      rl = activeTable()->nonDefaultRowFormat( top );
    } while ( rl->isHide() && top != 1);
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
        if (activeTable()->getLcMode())
        {
            buffer = "L" + QString::number( markerRow() ) +
		"C" + QString::number( markerColumn() );
        }
        else
        {
            buffer = KSpreadCell::columnName( markerColumn() ) +
		QString::number( markerRow() );
        }
    }
    else
    {
        if (activeTable()->getLcMode())
        {
            buffer = QString::number( (selection().bottom()-selection().top()+1) )+"Lx";
            if ( util_isRowSelected( selection() ) )
                buffer+=QString::number((KS_colMax-selection().left()+1))+"C";
            else
                buffer+=QString::number((selection().right()-selection().left()+1))+"C";
        }
        else
        {
                //encodeColumnLabelText return @@@@ when column >KS_colMax
                //=> it's not a good display
                //=> for the moment I display pos of marker
                buffer=KSpreadCell::columnName( selection().left() ) +
		    QString::number(selection().top()) + ":" +
		    KSpreadCell::columnName( QMIN( KS_colMax, selection().right() ) ) +
		    QString::number(selection().bottom());
                //buffer=activeTable()->columnLabel( m_iMarkerColumn );
                //buffer+=tmp.setNum(m_iMarkerRow);
        }
  }

    if (buffer != m_pPosWidget->lineEdit()->text())
      m_pPosWidget->lineEdit()->setText(buffer);
}




void KSpreadCanvas::adjustArea(bool makeUndo)
{
  QRect s( selection() );
  if (activeTable()->areaIsEmpty(s))
        return;

  if (makeUndo)
  {
        if ( !doc()->undoLocked() )
        {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( doc(),activeTable() , s );
                doc()->addCommand( undo );
        }
  }
  // Columns selected
  if ( util_isColumnSelected(s) )
  {
    for (int x=s.left(); x <= s.right(); x++ )
    {
      hBorderWidget()->adjustColumn(x,false);
    }
  }
  // Rows selected
  else if ( util_isRowSelected(s) )
  {
    for(int y = s.top(); y <= s.bottom(); y++ )
    {
      vBorderWidget()->adjustRow(y,false);
    }
  }
  // No selection
  // Selection of a rectangular area
  else
  {
    for (int x=s.left(); x <= s.right(); x++ )
    {
      hBorderWidget()->adjustColumn(x,false);
    }
    for(int y = s.top(); y <= s.bottom(); y++ )
    {
      vBorderWidget()->adjustRow(y,false);
    }
  }
}

void KSpreadCanvas::equalizeRow()
{
  QRect s( selection() );
  RowFormat *rl = m_pView->activeTable()->rowFormat(s.top());
  int size=rl->height(this);
  if ( s.top() == s.bottom() )
      return;
  for(int i=s.top()+1;i<=s.bottom();i++)
  {
      KSpreadSheet *table = activeTable();
      if ( !table )
          return;
      size=QMAX(m_pView->activeTable()->rowFormat(i)->height(this),size);
  }
  m_pView->vBorderWidget()->equalizeRow(size);
}

void KSpreadCanvas::equalizeColumn()
{
  QRect s( selection() );
  ColumnFormat *cl = m_pView->activeTable()->columnFormat(s.left());
  int size=cl->width(this);
  if ( s.left() == s.right() )
      return;

  for(int i=s.left()+1;i<=s.right();i++)
  {
    size=QMAX(m_pView->activeTable()->columnFormat(i)->width(this),size);
  }
  m_pView->hBorderWidget()->equalizeColumn(size);
}

QRect KSpreadCanvas::visibleCells()
{
  KoRect unzoomedRect = doc()->unzoomRect( QRect( 0, 0, width(), height() ) );
  unzoomedRect.moveBy( xOffset(), yOffset() );

  double tmp;
  int left_col = activeTable()->leftColumn( unzoomedRect.left(), tmp );
  int right_col = activeTable()->rightColumn( unzoomedRect.right() );
  int top_row = activeTable()->topRow( unzoomedRect.top(), tmp );
  int bottom_row = activeTable()->bottomRow( unzoomedRect.bottom() );

  return QRect( left_col, top_row,
                right_col - left_col + 1, bottom_row - top_row + 1 );
}


//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void KSpreadCanvas::paintUpdates()
{
  if (activeTable() == NULL)
    return;

  QPainter painter(this);

  //Save clip region
  QRegion rgnComplete( painter.clipRegion() );
  QWMatrix matrix;
  if ( m_pView )
  {
    matrix = m_pView->matrix();
  }
  else
  {
    matrix = painter.worldMatrix();
  }

  painter.save();
  clipoutChildren( painter, matrix );

  KoRect unzoomedRect = doc()->unzoomRect( QRect( 0, 0, width(), height() ) );
  // unzoomedRect.moveBy( xOffset(), yOffset() );


  /* paint any visible cell that has the paintDirty flag */
  QRect range = visibleCells();
  KSpreadCell* cell = NULL;

  double topPos = activeTable()->dblRowPos(range.top());
  double leftPos = activeTable()->dblColumnPos(range.left());

  KoPoint dblCorner( leftPos - xOffset(), topPos - yOffset() );

  int x;
  int y;

  int right  = range.right();
  int bottom = range.bottom();
  KSpreadSheet * sheet = activeTable();

  for ( x = range.left(); x <= right; ++x )
  {
    for ( y = range.top(); y <= bottom; ++y )
    {
      if ( sheet->cellIsPaintDirty( QPoint( x, y ) ) )
      {
        cell = sheet->cellAt( x, y );
        
        // recalc and relayout only for non default cells
        if( !cell->isDefault() )
        {
          cell->calc();
          cell->makeLayout( painter, x, y );
        }

        bool paintBordersBottom = false;
        bool paintBordersRight = false;
        bool paintBordersLeft = false;
        bool paintBordersTop = false;

        QPen bottomPen( cell->effBottomBorderPen( x, y ) );
        QPen rightPen( cell->effRightBorderPen( x, y ) );
        QPen leftPen( cell->effLeftBorderPen( x, y ) );
        QPen topPen( cell->effTopBorderPen( x, y ) );

        // paint right border if rightmost cell or if the pen is more "worth" than the left border pen
        // of the cell on the left or if the cell on the right is not painted. In the latter case get
        // the pen that is of more "worth"
        if ( x >= KS_colMax )
          paintBordersRight = true;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x + 1, y ) ) )
          {
            paintBordersRight = true;
            if ( cell->effRightBorderValue( x, y ) < sheet->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
              rightPen = sheet->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
          }
        else
        {
          paintBordersRight = true;
          if ( cell->effRightBorderValue( x, y ) < sheet->cellAt( x + 1, y )->effLeftBorderValue( x + 1, y ) )
            rightPen = sheet->cellAt( x + 1, y )->effLeftBorderPen( x + 1, y );
        }

        // similiar for other borders...
        // bottom border:
        if ( y >= KS_rowMax )
          paintBordersBottom = true;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x, y + 1 ) ) )
          {
            if ( cell->effBottomBorderValue( x, y ) > sheet->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1 ) )
              paintBordersBottom = true;
          }
        else
        {
          paintBordersBottom = true;

          if ( cell->effBottomBorderValue( x, y ) < sheet->cellAt( x, y + 1 )->effTopBorderValue( x, y + 1 ) )
            bottomPen = sheet->cellAt( x, y + 1 )->effTopBorderPen( x, y + 1 );
        }

        // left border:
        if ( x == 1 )
          paintBordersLeft = true;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x - 1, y ) ) )
          {
            paintBordersLeft = true;
            if ( cell->effLeftBorderValue( x, y ) < sheet->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
              leftPen = sheet->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
          }
        else
        {
          paintBordersLeft = true;
          if ( cell->effLeftBorderValue( x, y ) < sheet->cellAt( x - 1, y )->effRightBorderValue( x - 1, y ) )
            leftPen = sheet->cellAt( x - 1, y )->effRightBorderPen( x - 1, y );
        }

        // top border:
        if ( y == 1 )
          paintBordersTop = true;
        else
          if ( sheet->cellIsPaintDirty( QPoint( x, y - 1 ) ) )
          {
            paintBordersTop = true;
            if ( cell->effTopBorderValue( x, y ) < sheet->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
              topPen = sheet->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
          }
        else
        {
          paintBordersTop = true;
          if ( cell->effTopBorderValue( x, y ) < sheet->cellAt( x, y - 1 )->effBottomBorderValue( x, y - 1 ) )
            topPen = sheet->cellAt( x, y - 1 )->effBottomBorderPen( x, y - 1 );
        }

        cell->paintCell( unzoomedRect, painter, m_pView, dblCorner,
                         QPoint( x, y ), paintBordersRight, paintBordersBottom, paintBordersLeft, paintBordersTop,
                         rightPen, bottomPen, leftPen, topPen );

      }
      dblCorner.setY( dblCorner.y() + sheet->rowFormat( y )->dblHeight( ) );
    }
    dblCorner.setY( topPos - yOffset() );
    dblCorner.setX( dblCorner.x() + sheet->columnFormat( x )->dblWidth( ) );
  }

  /* now paint the selection and choose selection */
  paintChooseRect(painter, unzoomedRect);
  paintNormalMarker(painter, unzoomedRect);

  //restore clip region with children area
  painter.restore();
  painter.setClipRegion( rgnComplete );
  paintChildren( painter, matrix );
}

void KSpreadCanvas::clipoutChildren( QPainter& painter, QWMatrix& matrix )
{
  QRegion rgn = painter.clipRegion();
  if ( rgn.isEmpty() )
    rgn = QRegion( QRect( 0, 0, width(), height() ) );

  QPtrListIterator<KoDocumentChild> itChild( m_pDoc->children() );
  for( ; itChild.current(); ++itChild )
  {
//    if ( ((KSpreadChild*)it.current())->table() == table &&
//         !m_pView->hasDocumentInWindow( it.current()->document() ) )
    if ( ( ( KSpreadChild*)itChild.current() )->table() == activeTable() )
    {
      rgn -= itChild.current()->region( matrix );
    }
  }
  painter.setClipRegion( rgn );
}

void KSpreadCanvas::paintChildren( QPainter& painter, QWMatrix& matrix )
{
  painter.setWorldMatrix( matrix );
  QPtrListIterator<KoDocumentChild> itChild( m_pDoc->children() );
  itChild.toFirst();
  for( ; itChild.current(); ++itChild )
  {
    if ( ( ( KSpreadChild*)itChild.current() )->table() == activeTable() &&
         ( m_pView && !m_pView->hasDocumentInWindow( itChild.current()->document() ) ) )
    {
      // #### todo: paint only if child is visible inside rect
      painter.save();
      m_pDoc->paintChild( itChild.current(), painter, m_pView,
        m_pDoc->zoomedResolutionX(), m_pDoc->zoomedResolutionY() );
      painter.restore();
    }
  }
}

void KSpreadCanvas::paintChooseRect(QPainter& painter, const KoRect &viewRect)
{
  double positions[4];
  bool paintSides[4];

  QRect chooseRect = m_pView->selectionInfo()->getChooseRect();

  if ( chooseRect.left() != 0 )
  {
    QPen pen;
    pen.setWidth( 2 );
    pen.setStyle( DashLine );

    retrieveMarkerInfo( chooseRect, viewRect, positions, paintSides );

    double left =   positions[0];
    double top =    positions[1];
    double right =  positions[2];
    double bottom = positions[3];

    bool paintLeft =   paintSides[0];
    bool paintTop =    paintSides[1];
    bool paintRight =  paintSides[2];
    bool paintBottom = paintSides[3];

    RasterOp rop = painter.rasterOp();
    painter.setRasterOp( NotROP );
    painter.setPen( pen );

    if ( paintTop )
    {
      painter.drawLine( doc()->zoomItX( left ),  doc()->zoomItY( top ),
                        doc()->zoomItX( right ), doc()->zoomItY( top ) );
    }
    if ( paintLeft )
    {
      painter.drawLine( doc()->zoomItX( left ), doc()->zoomItY( top ),
                        doc()->zoomItX( left ), doc()->zoomItY( bottom ) );
    }
    if ( paintRight )
    {
      painter.drawLine( doc()->zoomItX( right ), doc()->zoomItY( top ),
                        doc()->zoomItX( right ), doc()->zoomItY( bottom ) );
    }
    if ( paintBottom )
    {
      painter.drawLine( doc()->zoomItX( left ),  doc()->zoomItY( bottom ),
                        doc()->zoomItX( right ), doc()->zoomItY( bottom ) );
    }

    /* restore the old raster mode */
    painter.setRasterOp( rop );
  }
  return;
}

void KSpreadCanvas::paintNormalMarker(QPainter& painter, const KoRect &viewRect)
{

  if( m_bChoose )
    return;

  double positions[4];
  bool paintSides[4];

  QRect marker = selection();

  QPen pen( Qt::black, 3 );
  painter.setPen( pen );

  retrieveMarkerInfo( marker, viewRect, positions, paintSides );

  painter.setPen( pen );

  double left =   positions[0];
  double top =    positions[1];
  double right =  positions[2];
  double bottom = positions[3];

  bool paintLeft =   paintSides[0];
  bool paintTop =    paintSides[1];
  bool paintRight =  paintSides[2];
  bool paintBottom = paintSides[3];

  /* the extra '-1's thrown in here account for the thickness of the pen.
     want to look like this:                     not this:
                            * * * * * *                     * * * *
                            *         *                   *         *
     .                      *         *                   *         *
  */
  int l = 1;

  if ( paintTop )
  {
    painter.drawLine( doc()->zoomItX( left ) - l,      doc()->zoomItY( top ),
                      doc()->zoomItX( right ) + 2 * l, doc()->zoomItY( top ) );
  }
  if ( activeTable()->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    if ( paintRight )
    {
      painter.drawLine( doc()->zoomItX( right ), doc()->zoomItY( top ),
                        doc()->zoomItX( right ), doc()->zoomItY( bottom ) );
    }
    if ( paintLeft && paintBottom )
    {
      /* then the 'handle' in the bottom left corner is visible. */
      painter.drawLine( doc()->zoomItX( left ), doc()->zoomItY( top ),
                        doc()->zoomItX( left ), doc()->zoomItY( bottom ) - 3 );
      painter.drawLine( doc()->zoomItX( left ) + 4,  doc()->zoomItY( bottom ),
                        doc()->zoomItX( right ) + l + 1, doc()->zoomItY( bottom ) );
      painter.fillRect( doc()->zoomItX( left ) - 2, doc()->zoomItY( bottom ) -2, 5, 5,
                        painter.pen().color() );
    }
    else
    {
      if ( paintLeft )
      {
        painter.drawLine( doc()->zoomItX( left ), doc()->zoomItY( top ),
                          doc()->zoomItX( left ), doc()->zoomItY( bottom ) );
      }
      if ( paintBottom )
      {
        painter.drawLine( doc()->zoomItX( left ) - l,  doc()->zoomItY( bottom ),
                          doc()->zoomItX( right ) + l + 1, doc()->zoomItY( bottom ));
      }
    }
  }
  else
  {
    if ( paintLeft )
    {
      painter.drawLine( doc()->zoomItX( left ), doc()->zoomItY( top ),
                        doc()->zoomItX( left ), doc()->zoomItY( bottom ) );
    }
    if ( paintRight && paintBottom )
    {
      /* then the 'handle' in the bottom right corner is visible. */
      painter.drawLine( doc()->zoomItX( right ), doc()->zoomItY( top ),
                        doc()->zoomItX( right ), doc()->zoomItY( bottom ) - 3 );
      painter.drawLine( doc()->zoomItX( left ) - l,  doc()->zoomItY( bottom ),
                        doc()->zoomItX( right ) - 3, doc()->zoomItY( bottom ) );
      painter.fillRect( doc()->zoomItX( right ) - 2, doc()->zoomItY( bottom ) - 2, 5, 5,
                        painter.pen().color() );
    }
    else
    {
      if ( paintRight )
      {
        painter.drawLine( doc()->zoomItX( right ), doc()->zoomItY( top ),
                          doc()->zoomItX( right ), doc()->zoomItY( bottom ) );
      }
      if ( paintBottom )
      {
        painter.drawLine( doc()->zoomItX( left ) - l,  doc()->zoomItY( bottom ),
                          doc()->zoomItX( right ) + l, doc()->zoomItY( bottom ) );
      }
    }
  }
}


void KSpreadCanvas::retrieveMarkerInfo( const QRect &marker,
                                        const KoRect &viewRect,
                                        double positions[],
                                        bool paintSides[] )
{
  KSpreadSheet * table = activeTable();
  if ( !table )
    return;

  double dWidth = doc()->unzoomItX( width() );

  double xpos;
  double x;
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    xpos = dWidth - table->dblColumnPos( marker.right() ) + xOffset();
    x    = dWidth - table->dblColumnPos( marker.left() ) + xOffset();
  }
  else
  {
    xpos = table->dblColumnPos( marker.left() ) - xOffset();
    x    = table->dblColumnPos( marker.right() ) - xOffset();
  }
  double ypos = table->dblRowPos( marker.top() ) - yOffset();

  const ColumnFormat *columnFormat = table->columnFormat( marker.right() );
  double tw = columnFormat->dblWidth( );
  double w = x - xpos + tw;

  double y = table->dblRowPos( marker.bottom() ) - yOffset();
  const RowFormat* rowFormat = table->rowFormat( marker.bottom() );
  double th = rowFormat->dblHeight( );
  double h = ( y - ypos ) + th;

  /* left, top, right, bottom */
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    positions[0] = xpos - tw;
    positions[2] = xpos - tw + w;
  }
  else
  {
    positions[0] = xpos;
    positions[2] = xpos + w;
  }
  positions[1] = ypos;
  positions[3] = ypos + h;

  /* these vars are used for clarity, the array for simpler function arguments  */
  double left = positions[0];
  double top = positions[1];
  double right = positions[2];
  double bottom = positions[3];

  /* left, top, right, bottom */
  paintSides[0] = (viewRect.left() <= left) && (left <= viewRect.right()) &&
                (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  paintSides[1] = (viewRect.top() <= top) && (top <= viewRect.bottom())
               && (right >= viewRect.left()) && (left <= viewRect.right());
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
    paintSides[2] = (viewRect.left() <= right ) &&
                    (right - 1 <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  else
    paintSides[2] = (viewRect.left() <= right ) &&
                    (right <= viewRect.right()) &&
                    (bottom >= viewRect.top()) && (top <= viewRect.bottom());
  paintSides[3] = (viewRect.top() <= bottom) && (bottom <= viewRect.bottom())
               && (right >= viewRect.left()) && (left <= viewRect.right());

  positions[0] = QMAX( left,   viewRect.left() );
  positions[1] = QMAX( top,    viewRect.top() );
  positions[2] = QMIN( right,  viewRect.right() );
  positions[3] = QMIN( bottom, viewRect.bottom() );
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
  m_bMousePressed = FALSE;

  m_scrollTimer = new QTimer( this );
  connect (m_scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );
}


KSpreadVBorder::~KSpreadVBorder()
{
    delete m_scrollTimer;
}

QSize KSpreadVBorder::sizeHint() const
{
  return QSize( 40, 10 );
}


void KSpreadVBorder::mousePressEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  if ( _ev->button() == LeftButton )
    m_bMousePressed = true;

  const KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );

  double ev_PosY = m_pCanvas->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();
  double dHeight = m_pCanvas->doc()->unzoomItY( height() );
  m_bResize = FALSE;
  m_bSelection = FALSE;

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
  {
    m_pCanvas->deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  // Find the first visible row and the y position of this row.
  double y;
  int row = table->topRow( m_pCanvas->yOffset(), y );

  // Did the user click between two rows?
  while ( y < ( dHeight + m_pCanvas->yOffset() ) && ( !m_bResize ) )
  {
    double h = table->rowFormat( row )->dblHeight();
    row++;
    if ( row > KS_rowMax )
      row = KS_rowMax;
    if ( ( ev_PosY >= y + h - 2 ) &&
         ( ev_PosY <= y + h + 1 ) &&
         !( table->rowFormat( row )->isHide() && row == 1 ) )
      m_bResize = TRUE;
    y += h;
  }

  //if row is hide and it's the first row
  //you mustn't resize it.
  double tmp2;
  int tmpRow = table->topRow( ev_PosY - 1, tmp2 );
  if ( table->rowFormat( tmpRow )->isHide() && tmpRow == 1 )
      m_bResize = false;

  // So he clicked between two rows ?
  if ( m_bResize )
  {
    // Determine row to resize
    double tmp;
    m_iResizedRow = table->topRow( ev_PosY - 1, tmp );
    if ( !table->isProtected() )
      paintSizeIndicator( _ev->pos().y(), true );
  }
  else
  {
    m_bSelection = TRUE;

    double tmp;
    int hit_row = table->topRow( ev_PosY, tmp );
    if ( hit_row > KS_rowMax )
        return;

    m_iSelectionAnchor = hit_row;

    QRect rect = m_pView->selection();
    if ( !rect.contains( QPoint(1, hit_row) ) ||
        !( _ev->button() == RightButton ) ||
        ( !util_isRowSelected( rect ) ) )
    {
      QPoint newMarker( 1, hit_row );
      QPoint newAnchor( KS_colMax, hit_row );
      m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                              m_pView->activeTable() );
    }

    if ( _ev->button() == RightButton )
    {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupRowMenu( p );
      m_bSelection = FALSE;
    }
    m_pView->updateEditWidget();
  }
}

void KSpreadVBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();

    m_bMousePressed = false;

    if ( !m_pView->koDocument()->isReadWrite() )
        return;

    KSpreadSheet *table = m_pCanvas->activeTable();
    assert( table );

    double ev_PosY = m_pCanvas->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();

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
        if ( util_isRowSelected( m_pView->selection() ) )
        {
            if ( m_pView->selection().contains( QPoint( 1, m_iResizedRow ) ) )
            {
                start = m_pView->selection().top();
                end = m_pView->selection().bottom();
                rect = m_pView->selection();
            }
        }

        double height = 0.0;
        double y = table->dblRowPos( m_iResizedRow );
        if ( ev_PosY - y <= 0.0 )
            height = 0.0;
        else
            height = ev_PosY - y;

        if ( !table->isProtected() )
        {
          if ( !m_pCanvas->doc()->undoLocked() )
          {
            //just resize
            if ( height != 0.0 )
            {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
                m_pCanvas->doc()->addCommand( undo );
            }
            else
            {
                //hide row
                KSpreadUndoHideRow *undo = new KSpreadUndoHideRow( m_pCanvas->doc(), m_pCanvas->activeTable(),
                                                                   rect.top(), ( rect.bottom() - rect.top() ) );
                m_pCanvas->doc()->addCommand( undo );
            }
          }

          for( int i = start; i <= end; i++ )
          {
            RowFormat *rl = table->nonDefaultRowFormat( i );
            if ( height != 0.0 )
            {
              if ( !rl->isHide() )
                rl->setDblHeight( height );
            }
            else
              rl->setHide( true );
          }

          if ( height == 0.0 )
            table->emitHideColumn();

          delete m_lSize;
          m_lSize = 0;
        }
    }
    else if ( m_bSelection )
    {
        QRect rect = m_pView->selection();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if ( m_frozen )
        {
            kdDebug(36001) << "selected: T " << rect.top() << " B " << rect.bottom() << endl;

            int i;
            RowFormat * row;
            QValueList<int>hiddenRows;

            for ( i = rect.top(); i <= rect.bottom(); ++i )
            {
                row = m_pView->activeTable()->rowFormat( i );
                if ( row->isHide() )
                {
                    hiddenRows.append(i);
                }
            }

            if ( hiddenRows.count() > 0 )
                m_pView->activeTable()->showRow( 0, -1, hiddenRows );
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
            RowFormat * rl = table->nonDefaultRowFormat( select );

            if ( kAbs( rl->dblHeight() - adjust ) < DBL_EPSILON )
                return;
        }

        if ( makeUndo && !m_pCanvas->doc()->undoLocked() )
        {
            QRect rect;
            rect.setCoords( 1, select, KS_colMax, select);
            KSpreadUndoResizeColRow * undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),
                                                                          m_pCanvas->activeTable(), rect );
            m_pCanvas->doc()->addCommand( undo );
        }
        RowFormat * rl = table->nonDefaultRowFormat( select );
        rl->setDblHeight( QMAX( 2.0, adjust ) );
    }
}

void KSpreadVBorder::equalizeRow( double resize )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  Q_ASSERT( table );

  QRect selection( m_pView->selection() );
  if ( !m_pCanvas->doc()->undoLocked() )
  {
     KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), selection );
     m_pCanvas->doc()->addCommand( undo );
  }
  RowFormat *rl;
  for ( int i = selection.top(); i <= selection.bottom(); i++ )
  {
     rl = table->nonDefaultRowFormat( i );
     resize = QMAX( 2.0, resize);
     rl->setDblHeight( resize );
  }
}

void KSpreadVBorder::resizeRow( double resize, int nb, bool makeUndo )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  Q_ASSERT( table );

  if ( nb == -1 ) // I don't know, where this is the case
  {
    if ( makeUndo && !m_pCanvas->doc()->undoLocked() )
    {
        QRect rect;
        rect.setCoords( 1, m_iSelectionAnchor, KS_colMax, m_iSelectionAnchor );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->addCommand( undo );
    }
    RowFormat *rl = table->nonDefaultRowFormat( m_iSelectionAnchor );
    rl->setDblHeight( QMAX( 2.0, resize ) );
  }
  else
  {
    QRect selection( m_pView->selection() );
    if ( m_pView->selectionInfo()->singleCellSelection() )
    {
      if ( makeUndo && !m_pCanvas->doc()->undoLocked() )
      {
        QRect rect;
        rect.setCoords( 1, m_pCanvas->markerRow(), KS_colMax, m_pCanvas->markerRow() );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->addCommand( undo );
      }
      RowFormat *rl = table->nonDefaultRowFormat( m_pCanvas->markerRow() );
      rl->setDblHeight( QMAX( 2.0, resize ) );
    }
    else
    {
      if ( makeUndo && !m_pCanvas->doc()->undoLocked() )
      {
          KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), selection );
          m_pCanvas->doc()->addCommand( undo );
      }
      RowFormat *rl;
      for ( int i = selection.top(); i<=selection.bottom(); i++ )
      {
        rl = table->nonDefaultRowFormat( i );
        rl->setDblHeight( QMAX( 2.0, resize ) );
      }
    }
  }
}


void KSpreadVBorder::mouseDoubleClickEvent( QMouseEvent * /*_ev */)
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );

  if ( !m_pView->koDocument()->isReadWrite() || table->isProtected() )
    return;

  adjustRow();
}


void KSpreadVBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );

  double ev_PosY = m_pCanvas->doc()->unzoomItY( _ev->pos().y() ) + m_pCanvas->yOffset();
  double dHeight = m_pCanvas->doc()->unzoomItY( height() );

  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    if ( !table->isProtected() )
      paintSizeIndicator( _ev->pos().y(), false );
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    double y;
    int row = table->topRow( ev_PosY, y );
    if ( row > KS_rowMax )
      return;

    QPoint newAnchor = m_pView->selectionInfo()->selectionAnchor();
    QPoint newMarker = m_pView->selectionInfo()->marker();
    newMarker.setY( row );
    newAnchor.setY( m_iSelectionAnchor );

    m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                            m_pView->activeTable() );

    if ( _ev->pos().y() < 0 )
      m_pCanvas->vertScrollBar()->setValue( m_pCanvas->doc()->zoomItY( ev_PosY ) );
    else if ( _ev->pos().y() > m_pCanvas->height() )
    {
      if ( row < KS_rowMax )
      {
        RowFormat *rl = table->rowFormat( row + 1 );
        y = table->dblRowPos( row + 1 );
        m_pCanvas->vertScrollBar()->setValue( (m_pCanvas->doc()->zoomItY( ev_PosY
                                                              + rl->dblHeight() )
                                                              - dHeight ));
      }
    }
  }
  // No button is pressed and the mouse is just moved
  else
  {

     //What is the internal size of 1 pixel
    const double unzoomedPixel = m_pCanvas->doc()->unzoomItY( 1 );
    double y;
    int tmpRow = table->topRow( m_pCanvas->yOffset(), y );

    while ( y < m_pCanvas->doc()->unzoomItY( height() ) + m_pCanvas->yOffset() )
    {
      double h = table->rowFormat( tmpRow )->dblHeight();
      //if col is hide and it's the first column
      //you mustn't resize it.
      if ( ev_PosY >= y + h - 2 * unzoomedPixel &&
           ev_PosY <= y + h + unzoomedPixel &&
           !( table->rowFormat( tmpRow )->isHide() && tmpRow == 1 ) )
      {
        setCursor( splitVCursor );
        return;
      }
      y += h;
      tmpRow++;
    }
    setCursor( arrowCursor );
  }
}

void KSpreadVBorder::doAutoScroll()
{
    if ( !m_bMousePressed )
    {
        m_scrollTimer->stop();
        return;
    }

    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    if ( pos.y() < 0 || pos.y() > height() )
    {
        QMouseEvent * event = new QMouseEvent( QEvent::MouseMove, pos, 0, 0 );
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    m_scrollTimer->start( 50 );
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

    if ( !firstTime )
      painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    m_iResizePos = mouseY;

    // Dont make the row have a height < 2 pixel.
    int y = m_pCanvas->doc()->zoomItY( table->dblRowPos( m_iResizedRow ) - m_pCanvas->yOffset() );
    if ( m_iResizePos < y + 2 )
        m_iResizePos = y;

    painter.drawLine( 0, m_iResizePos, m_pCanvas->width(), m_iResizePos );

    painter.end();

    QString tmpSize;
    if ( m_iResizePos != y )
        tmpSize = i18n("Height: %1 %2").arg( KoUnit::toUserValue( m_pCanvas->doc()->unzoomItY( m_iResizePos - y ),
                                                               m_pView->doc()->getUnit() ) )
                                       .arg( m_pView->doc()->getUnitName() );
    else
        tmpSize = i18n( "Hide Row" );

    painter.begin( this );
    int len = painter.fontMetrics().width( tmpSize );
    int hei = painter.fontMetrics().height();
    painter.end();

    if ( !m_lSize )
    {
          m_lSize = new QLabel( m_pCanvas );
          m_lSize->setGeometry( 3, y + 3, len + 2, hei + 2 );
          m_lSize->setAlignment( Qt::AlignVCenter );
          m_lSize->setText( tmpSize );
          m_lSize->show();
    }
    else
    {
          m_lSize->setGeometry( 3, y + 3, len + 2, hei + 2 );
          m_lSize->setText( tmpSize );
    }
}

void KSpreadVBorder::updateRows( int from, int to )
{
    KSpreadSheet *table = m_pCanvas->activeTable();
    if ( !table )
        return;

    int y0 = table->rowPos( from, m_pCanvas );
    int y1 = table->rowPos( to+1, m_pCanvas );
    update( 0, y0, width(), y1-y0 );
}

void KSpreadVBorder::paintEvent( QPaintEvent* _ev )
{
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

  double yPos;
  //Get the top row and the current y-position
  int y = table->topRow( (m_pCanvas->doc()->unzoomItY( _ev->rect().y() ) + m_pCanvas->yOffset()), yPos );
  //Align to the offset
  yPos = yPos - m_pCanvas->yOffset();
  int width = m_pCanvas->doc()->zoomItX( YBORDER_WIDTH );

  QFont normalFont = painter.font();
  if ( m_pCanvas->doc()->zoom() < 100 )
  {
    normalFont.setPointSizeFloat( 0.01 * m_pCanvas->doc()->zoom() *
                                  normalFont.pointSizeFloat() );
  }
  QFont boldFont = normalFont;
  boldFont.setBold( TRUE );

  //several cells selected but not just a cell merged
  bool area = !( m_pView->selectionInfo()->singleCellSelection() );

  //Loop through the rows, until we are out of range
  while ( yPos <= m_pCanvas->doc()->unzoomItY( _ev->rect().bottom() ) )
  {
    bool highlighted = ( area && y >= m_pView->selection().top() &&
                         y <= m_pView->selection().bottom() );
    bool selected = ( highlighted && (util_isRowSelected(m_pView->selection())) );
    bool current  = ( !highlighted && y == m_pView->selection().top() );

    const RowFormat *row_lay = table->rowFormat( y );
    int zoomedYPos = m_pCanvas->doc()->zoomItY( yPos );
    int height = m_pCanvas->doc()->zoomItY( yPos + row_lay->dblHeight() ) - zoomedYPos;


    if ( selected )
    {
      QBrush fillSelected( colorGroup().brush( QColorGroup::Highlight ) );
      qDrawShadePanel( &painter,
                       0, zoomedYPos, width, height,
                       colorGroup(), FALSE, 1, &fillSelected );
    }
    else if ( highlighted )
    {
      QBrush fillHighlighted( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter,
                       0, zoomedYPos, width, height,
                       colorGroup(), true, 1, &fillHighlighted );
    }
    else
    {
      QBrush fill( colorGroup().brush( QColorGroup::Background ) );
      qDrawShadePanel( &painter,
                       0, zoomedYPos, width, height,
                       colorGroup(), FALSE, 1, &fill );
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
    if (!row_lay->isHide())
        painter.drawText( ( width-len )/2, zoomedYPos +
                          ( height + painter.fontMetrics().ascent() -
                            painter.fontMetrics().descent() ) / 2, rowText );

    yPos += row_lay->dblHeight();
    y++;
  }
}


void KSpreadVBorder::focusOutEvent( QFocusEvent* )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();
    m_bMousePressed = false;
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
  m_bMousePressed = FALSE;

  m_scrollTimer = new QTimer( this );
  connect ( m_scrollTimer, SIGNAL( timeout() ), this, SLOT( doAutoScroll() ) );
}


KSpreadHBorder::~KSpreadHBorder()
{
    delete m_scrollTimer;
}

QSize KSpreadHBorder::sizeHint() const
{
  return QSize( 40, 10 );
}

void KSpreadHBorder::mousePressEvent( QMouseEvent * _ev )
{
  if (!m_pView->koDocument()->isReadWrite())
    return;

  if ( _ev->button() == LeftButton )
    m_bMousePressed = true;

  const KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );

  // We were editing a cell -> save value and get out of editing mode
  if ( m_pCanvas->editor() )
  {
      m_pCanvas->deleteEditor( true ); // save changes
  }

  m_scrollTimer->start( 50 );

  double ev_PosX;
  double dWidth = m_pCanvas->doc()->unzoomItX( width() );
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
    ev_PosX = dWidth - m_pCanvas->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
  else
    ev_PosX = m_pCanvas->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
  m_bResize = FALSE;
  m_bSelection = FALSE;

  // Find the first visible column and the x position of this column.
  double x;

  const double unzoomedPixel = m_pCanvas->doc()->unzoomItX( 1 );
  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    int tmpCol = table->leftColumn( m_pCanvas->xOffset(), x );

    kdDebug() << "evPos: " << ev_PosX << ", x: " << x << ", COL: " << tmpCol << endl;
    while ( ev_PosX > x && ( !m_bResize ) )
    {
      double w = table->columnFormat( tmpCol )->dblWidth();

      kdDebug() << "evPos: " << ev_PosX << ", x: " << x << ", w: " << w << ", COL: " << tmpCol << endl;

      ++tmpCol;
      if ( tmpCol > KS_colMax )
        tmpCol = KS_colMax;
      //if col is hide and it's the first column
      //you mustn't resize it.

      if ( ev_PosX >= x + w - unzoomedPixel &&
           ev_PosX <= x + w + unzoomedPixel &&
           !( table->columnFormat( tmpCol )->isHide() && tmpCol == 1 ) )
      {
        m_bResize = true;
      }
      x += w;
    }

    //if col is hide and it's the first column
    //you mustn't resize it.
    double tmp2;
    tmpCol = table->leftColumn( dWidth - ev_PosX + 1, tmp2 );
    if ( table->columnFormat( tmpCol )->isHide() && tmpCol == 0 )
    {
      kdDebug() << "No resize: " << tmpCol << ", " << table->columnFormat( tmpCol )->isHide() << endl;
      m_bResize = false;
    }

    kdDebug() << "Resize: " << m_bResize << endl;
  }
  else
  {
    int col = table->leftColumn( m_pCanvas->xOffset(), x );

    // Did the user click between two columns?
    while ( x < ( dWidth + m_pCanvas->xOffset() ) && ( !m_bResize ) )
    {
      double w = table->columnFormat( col )->dblWidth();
      col++;
      if ( col > KS_colMax )
        col = KS_colMax;
      if ( ( ev_PosX >= x + w - unzoomedPixel ) &&
         ( ev_PosX <= x + w + unzoomedPixel ) &&
           !( table->columnFormat( col )->isHide() && col == 1 ) )
        m_bResize = TRUE;
      x += w;
    }

    //if col is hide and it's the first column
    //you mustn't resize it.
    double tmp2;
    int tmpCol = table->leftColumn( ev_PosX - 1, tmp2 );
    if ( table->columnFormat( tmpCol )->isHide() && tmpCol == 1 )
      m_bResize = false;
  }

  // So he clicked between two rows ?
  if ( m_bResize )
  {
    // Determine the column to resize
    double tmp;
    if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      m_iResizedColumn = table->leftColumn( ev_PosX - 1, tmp );
      // kdDebug() << "RColumn: " << m_iResizedColumn << ", PosX: " << ev_PosX << endl;

      if ( !table->isProtected() )
        paintSizeIndicator( _ev->pos().x(), true );
    }
    else
    {
      m_iResizedColumn = table->leftColumn( ev_PosX - 1, tmp );

      if ( !table->isProtected() )
        paintSizeIndicator( _ev->pos().x(), true );
    }

    // kdDebug() << "Column: " << m_iResizedColumn << endl;
  }
  else
  {
    m_bSelection = TRUE;

    double tmp;
    int hit_col = table->leftColumn( ev_PosX, tmp );
    if ( hit_col > KS_colMax )
        return;

    m_iSelectionAnchor = hit_col;

    QRect rect = m_pView->selection();
    if ( !rect.contains( QPoint( hit_col, 1 ) ) ||
        !( _ev->button() == RightButton ) ||
        ( !util_isColumnSelected( rect ) ) )
    {
      QPoint newMarker( hit_col, 1 );
      QPoint newAnchor( hit_col, KS_rowMax );
      m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                              m_pView->activeTable() );
    }

    if ( _ev->button() == RightButton )
    {
      QPoint p = mapToGlobal( _ev->pos() );
      m_pView->popupColumnMenu( p );
      m_bSelection = FALSE;
    }
    m_pView->updateEditWidget();
  }
}

void KSpreadHBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();

    m_bMousePressed = false;

    if ( !m_pView->koDocument()->isReadWrite() )
      return;

    KSpreadSheet * table = m_pCanvas->activeTable();
    assert( table );

    if ( m_bResize )
    {
        double dWidth = m_pCanvas->doc()->unzoomItX( width() );
        double ev_PosX;

        // Remove size indicator painted by paintSizeIndicator
        QPainter painter;
        painter.begin( m_pCanvas );
        painter.setRasterOp( NotROP );
        painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );
        painter.end();

        int start = m_iResizedColumn;
        int end   = m_iResizedColumn;
        QRect rect;
        rect.setCoords( m_iResizedColumn, 1, m_iResizedColumn, KS_rowMax );
        if ( util_isColumnSelected(m_pView->selection()) )
        {
            if ( m_pView->selection().contains( QPoint( m_iResizedColumn, 1 ) ) )
            {
                start = m_pView->selection().left();
                end   = m_pView->selection().right();
                rect  = m_pView->selection();
            }
        }

        double width = 0.0;
        double x;

        if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
        {
          ev_PosX = dWidth - m_pCanvas->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
          x = table->dblColumnPos( m_iResizedColumn );

          if ( dWidth - ev_PosX - x <= 0.0 )
            width = 0.0;
          else
            width = ev_PosX - x;
        }
        else
        {
          ev_PosX = m_pCanvas->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();
          x = table->dblColumnPos( m_iResizedColumn );

          if ( ev_PosX - x <= 0.0 )
            width = 0.0;
          else
            width = ev_PosX - x;
        }

        if ( !table->isProtected() )
        {
          if ( !m_pCanvas->doc()->undoLocked() )
          {
            //just resize
            if ( width != 0.0 )
            {
                KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
                m_pCanvas->doc()->addCommand( undo );
            }
            else
            {//hide column
                KSpreadUndoHideColumn *undo = new KSpreadUndoHideColumn( m_pCanvas->doc(), m_pCanvas->activeTable(), rect.left(), (rect.right()-rect.left()));
                m_pCanvas->doc()->addCommand( undo );
            }
          }

          for( int i = start; i <= end; i++ )
          {
            ColumnFormat *cl = table->nonDefaultColumnFormat( i );
            if ( width != 0.0 )
            {
                if ( !cl->isHide() )
                    cl->setDblWidth( width );
            }
            else
                cl->setHide( true );
          }

          if ( width == 0.0 )
            table->emitHideRow();

          delete m_lSize;
          m_lSize = 0;
        }
    }
    else if ( m_bSelection )
    {
        QRect rect = m_pView->selection();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if ( m_frozen )
        {
            kdDebug(36001) << "selected: L " << rect.left() << " R " << rect.right() << endl;

            int i;
            ColumnFormat * col;
            QValueList<int>hiddenCols;

            for ( i = rect.left(); i <= rect.right(); ++i )
            {
                col = m_pView->activeTable()->columnFormat( i );
                if ( col->isHide() )
                {
                    hiddenCols.append(i);
                }
            }

            if ( hiddenCols.count() > 0 )
                m_pView->activeTable()->showColumn( 0, -1, hiddenCols );
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
    adjust = m_pCanvas->activeTable()->adjustColumn( m_pView->selectionInfo() );
    select = m_iSelectionAnchor;
  }
  else
  {
    adjust = m_pCanvas->activeTable()->adjustColumn( m_pView->selectionInfo(), _col );
    select = _col;
  }

  if ( adjust != -1.0 )
  {
    KSpreadSheet * table = m_pCanvas->activeTable();
    assert( table );

    if ( _col == -1 )
    {
        ColumnFormat * cl = table->nonDefaultColumnFormat( select );
        if ( kAbs( cl->dblWidth() - adjust ) < DBL_EPSILON )
            return;

    }
    if ( makeUndo && !m_pCanvas->doc()->undoLocked() )
    {
        QRect rect;
        rect.setCoords( select, 1, select, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(),
                                                                     m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->addCommand( undo );
    }

    ColumnFormat * cl = table->nonDefaultColumnFormat( select );
    cl->setDblWidth( QMAX( 2.0, adjust ) );
  }
}

void KSpreadHBorder::equalizeColumn( double resize )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  Q_ASSERT( table );

  QRect selection( m_pView->selection() );
  if ( !m_pCanvas->doc()->undoLocked() )
  {
      KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), selection );
      m_pCanvas->doc()->addCommand( undo );
  }
  ColumnFormat *cl;
  for ( int i = selection.left(); i <= selection.right(); i++ )
  {
      cl = table->nonDefaultColumnFormat( i );
      resize = QMAX( 2.0, resize );
      cl->setDblWidth( resize );
  }

}

void KSpreadHBorder::resizeColumn( double resize, int nb, bool makeUndo )
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  Q_ASSERT( table );

  if ( nb == -1 )
  {
    if ( makeUndo && !m_pCanvas->doc()->undoLocked() )
    {
        QRect rect;
        rect.setCoords( m_iSelectionAnchor, 1, m_iSelectionAnchor, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->addCommand( undo );
    }
    ColumnFormat *cl = table->nonDefaultColumnFormat( m_iSelectionAnchor );
    cl->setDblWidth( QMAX( 2.0, resize ) );
  }
  else
  {
    QRect selection( m_pView->selection() );
    if ( m_pView->selectionInfo()->singleCellSelection() )
    {
      if ( makeUndo && !m_pCanvas->doc()->undoLocked() )
      {
        QRect rect;
        rect.setCoords( m_iSelectionAnchor, 1, m_iSelectionAnchor, KS_rowMax );
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), rect );
        m_pCanvas->doc()->addCommand( undo );
      }

      ColumnFormat *cl = table->nonDefaultColumnFormat( m_pCanvas->markerColumn() );
      cl->setDblWidth( QMAX( 2.0, resize ) );
    }
    else
    {
      if ( makeUndo && !m_pCanvas->doc()->undoLocked() )
      {
        KSpreadUndoResizeColRow *undo = new KSpreadUndoResizeColRow( m_pCanvas->doc(), m_pCanvas->activeTable(), selection );
        m_pCanvas->doc()->addCommand( undo );
      }
      ColumnFormat *cl;
      for ( int i = selection.left(); i <= selection.right(); i++ )
      {
        cl = table->nonDefaultColumnFormat( i );
        cl->setDblWidth( QMAX( 2.0, resize ) );
      }
    }
  }
}

void KSpreadHBorder::mouseDoubleClickEvent( QMouseEvent * /*_ev */)
{
  KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );

  if ( !m_pView->koDocument()->isReadWrite() || table->isProtected() )
    return;

  adjustColumn();
}

void KSpreadHBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  if ( !m_pView->koDocument()->isReadWrite() )
    return;

  KSpreadSheet *table = m_pCanvas->activeTable();
  assert( table );

  double dWidth = m_pCanvas->doc()->unzoomItX( width() );
  double ev_PosX = m_pCanvas->doc()->unzoomItX( _ev->pos().x() ) + m_pCanvas->xOffset();

  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    if ( !table->isProtected() )
        paintSizeIndicator( _ev->pos().x(), false );
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    double x;
    int col;

    if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
      col = table->leftColumn( dWidth - ev_PosX, x );
    else
      col = table->leftColumn( ev_PosX, x );
    if ( col > KS_colMax )
      return;

    QPoint newMarker = m_pView->selectionInfo()->marker();
    QPoint newAnchor = m_pView->selectionInfo()->selectionAnchor();

    newMarker.setX( col );
    newAnchor.setX( m_iSelectionAnchor );

    m_pView->selectionInfo()->setSelection( newMarker, newAnchor,
                                            m_pView->activeTable() );

    if ( _ev->pos().x() < 0 ) // TODO rtl
      m_pCanvas->horzScrollBar()->setValue( m_pCanvas->doc()->zoomItX( ev_PosX ) );
    else if ( _ev->pos().x() > m_pCanvas->width() )
    {
      if ( col < KS_colMax )
      {
        ColumnFormat *cl = table->columnFormat( col + 1 );
        x = table->dblColumnPos( col + 1 );
        m_pCanvas->horzScrollBar()->setValue( (m_pCanvas->doc()->zoomItX( ev_PosX
                                              + cl->dblWidth() ) - dWidth) );
      }
    }

  }
  // No button is pressed and the mouse is just moved
  else
  {
     //What is the internal size of 1 pixel
    const double unzoomedPixel = m_pCanvas->doc()->unzoomItX( 1 );
    double x;

    if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
    {
      int tmpCol = table->leftColumn( m_pCanvas->xOffset(), x );

      x = dWidth - x;
      while ( ev_PosX < x )
      {
        double w = table->columnFormat( tmpCol )->dblWidth();

        //if col is hide and it's the first column
        //you mustn't resize it.
        if ( ev_PosX >= x - w - unzoomedPixel &&
             ev_PosX <= x - w + unzoomedPixel &&
             !( table->columnFormat( tmpCol )->isHide() && tmpCol == 0 ) )
        {
          setCursor( splitHCursor );
          return;
        }
        x -= w;
        tmpCol++;
      }
      setCursor( arrowCursor );
    }
    else
    {
      int tmpCol = table->leftColumn( m_pCanvas->xOffset(), x );

      while ( x < m_pCanvas->doc()->unzoomItY( width() ) + m_pCanvas->xOffset() )
      {
        double w = table->columnFormat( tmpCol )->dblWidth();
        //if col is hide and it's the first column
        //you mustn't resize it.
        if ( ev_PosX >= x + w - unzoomedPixel &&
             ev_PosX <= x + w + unzoomedPixel &&
             !( table->columnFormat( tmpCol )->isHide() && tmpCol == 1 ) )
        {
          setCursor( splitHCursor );
          return;
        }
        x += w;
        tmpCol++;
      }
      setCursor( arrowCursor );
    }
  }
}

void KSpreadHBorder::doAutoScroll()
{
    if ( !m_bMousePressed )
    {
        m_scrollTimer->stop();
        return;
    }

    QPoint pos( mapFromGlobal( QCursor::pos() ) );

    if ( pos.x() < 0 || pos.x() > width() )
    {
        QMouseEvent * event = new QMouseEvent( QEvent::MouseMove, pos, 0, 0 );
        mouseMoveEvent( event );
        delete event;
    }

    //Restart timer
    m_scrollTimer->start( 50 );
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

    if ( !firstTime )
      painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );

    m_iResizePos = mouseX;

    // Dont make the column have a width < 2 pixels.
    int x = m_pCanvas->doc()->zoomItX( table->dblColumnPos( m_iResizedColumn ) - m_pCanvas->xOffset() );
    if ( m_iResizePos < x + 2 )
        m_iResizePos = x;

    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pCanvas->height() );

    painter.end();

    QString tmpSize;
    if ( m_iResizePos != x )
        tmpSize = i18n("Width: %1 %2")
                  .arg( KGlobal::locale()->formatNumber( KoUnit::toUserValue( m_pCanvas->doc()->unzoomItX( m_iResizePos - x ),
                                                                           m_pView->doc()->getUnit() )))
                  .arg( m_pView->doc()->getUnitName() );
    else
        tmpSize = i18n( "Hide Column" );

    painter.begin( this );
    int len = painter.fontMetrics().width( tmpSize );
    int hei = painter.fontMetrics().height();
    painter.end();

    if ( !m_lSize )
    {
        m_lSize = new QLabel( m_pCanvas );
        m_lSize->setGeometry( x + 3, 3, len + 2, hei + 2 );
        m_lSize->setAlignment( Qt::AlignVCenter );
        m_lSize->setText( tmpSize );
        m_lSize->show();
    }
    else
    {
        m_lSize->setGeometry( x + 3, 3, len + 2, hei + 2 );
        m_lSize->setText( tmpSize );
    }
}

void KSpreadHBorder::updateColumns( int from, int to )
{
    KSpreadSheet *table = m_pCanvas->activeTable();
    if ( !table )
        return;

    int x0 = table->columnPos( from, m_pCanvas );
    int x1 = table->columnPos( to+1, m_pCanvas );
    update( x0, 0, x1-x0, height() );
}

void KSpreadHBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadSheet * table = m_pCanvas->activeTable();
  if ( !table )
    return;

  QPainter painter( this );
  QPen pen( Qt::black, 1 );
  painter.setPen( pen );
  painter.setBackgroundColor( white );

  painter.setClipRect( _ev->rect() );

  // painter.eraseRect( _ev->rect() );

  //QFontMetrics fm = painter.fontMetrics();
  // Matthias Elter: This causes a SEGFAULT in ~QPainter!
  // Only god and the trolls know why ;-)
  // bah...took me quite some time to track this one down...

  double xPos;
  //Get the left column and the current x-position
  int x = table->leftColumn( int( m_pCanvas->doc()->unzoomItX( _ev->rect().x() ) + m_pCanvas->xOffset() ), xPos );

  //Align to the offset
  xPos = xPos - m_pCanvas->xOffset();
  int height = m_pCanvas->doc()->zoomItY( KSpreadFormat::globalRowHeight() + 2 );

  QFont normalFont = painter.font();
  if ( m_pCanvas->doc()->zoom() < 100 )
  {
    normalFont.setPointSizeFloat( 0.01 * m_pCanvas->doc()->zoom() *
                                  normalFont.pointSizeFloat() );
  }
  QFont boldFont = normalFont;
  boldFont.setBold( TRUE );

  KSpreadCell *cell = table->cellAt( m_pView->marker() );
  QRect extraCell;
  extraCell.setCoords( m_pCanvas->markerColumn(),
                       m_pCanvas->markerRow(),
                       m_pCanvas->markerColumn() + cell->extraXCells(),
                       m_pCanvas->markerRow() + cell->extraYCells());

  //several cells selected but not just a cell merged
  bool area = ( m_pView->selection().left()!=0 && extraCell != m_pView->selection() );

  if ( table->layoutDirection()==KSpreadSheet::RightToLeft )
  {
    xPos = m_pCanvas->doc()->unzoomItX( _ev->rect().right() ) - xPos;

    //Loop through the columns, until we are out of range
    while ( xPos >= m_pCanvas->doc()->unzoomItX( _ev->rect().left() ) )
    {
      bool highlighted = ( area && x >= m_pView->selection().left() && x <= m_pView->selection().right());
      bool selected = ( highlighted && util_isColumnSelected( m_pView->selection() ) &&
                        ( !util_isRowSelected( m_pView->selection() ) ) );
      bool current = ( !highlighted && x == m_pView->selection().left() );

      const ColumnFormat * col_lay = table->columnFormat( x );
      xPos -= col_lay->dblWidth();
      int zoomedXPos = m_pCanvas->doc()->zoomItX( xPos );
      int width = m_pCanvas->doc()->zoomItX( xPos + col_lay->dblWidth() ) - zoomedXPos;

      if ( selected )
      {
        QBrush fillSelected( colorGroup().brush( QColorGroup::Highlight ) );
        qDrawShadePanel( &painter,
                         zoomedXPos, 0, width, height,
                         colorGroup(), FALSE, 1, &fillSelected );
      }
      else if ( highlighted )
      {
        QBrush fillHighlighted( colorGroup().brush( QColorGroup::Background ) );
        qDrawShadePanel( &painter,
                         zoomedXPos, 0, width, height,
                         colorGroup(), true, 1, &fillHighlighted );
      }
      else
      {
        QBrush fill( colorGroup().brush( QColorGroup::Background ) );
        qDrawShadePanel( &painter,
                         zoomedXPos, 0, width, height,
                         colorGroup(), FALSE, 1, &fill );
      }

      // Reset painter
      painter.setFont( normalFont );
      painter.setPen( colorGroup().text() );

      if ( selected )
        painter.setPen( colorGroup().highlightedText() );
      else if ( highlighted || current )
        painter.setFont( boldFont );
      if ( !m_pView->activeTable()->getShowColumnNumber() )
      {
        QString colText = KSpreadCell::columnName( x );
        int len = painter.fontMetrics().width( colText );
        if ( !col_lay->isHide() )
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2, colText );
      }
      else
      {
        QString tmp;
        int len = painter.fontMetrics().width( tmp.setNum(x) );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2,
                            tmp.setNum(x) );
      }
      ++x;
    }
  }
  else
  {
    //Loop through the columns, until we are out of range
    while ( xPos <= m_pCanvas->doc()->unzoomItX( _ev->rect().right() ) )
    {
      bool highlighted = ( area && x >= m_pView->selection().left() && x <= m_pView->selection().right());
      bool selected = ( highlighted && util_isColumnSelected( m_pView->selection() ) &&
                        ( !util_isRowSelected( m_pView->selection() ) ) );
      bool current = ( !highlighted && x == m_pView->selection().left() );

      const ColumnFormat *col_lay = table->columnFormat( x );
      int zoomedXPos = m_pCanvas->doc()->zoomItX( xPos );
      int width = m_pCanvas->doc()->zoomItX( xPos + col_lay->dblWidth() ) - zoomedXPos;

      if ( selected )
      {
        QBrush fillSelected( colorGroup().brush( QColorGroup::Highlight ) );
        qDrawShadePanel( &painter,
                         zoomedXPos, 0, width, height,
                         colorGroup(), FALSE, 1, &fillSelected );
      }
      else if ( highlighted )
      {
        QBrush fillHighlighted( colorGroup().brush( QColorGroup::Background ) );
        qDrawShadePanel( &painter,
                         zoomedXPos, 0, width, height,
                         colorGroup(), true, 1, &fillHighlighted );
      }
      else
      {
        QBrush fill( colorGroup().brush( QColorGroup::Background ) );
        qDrawShadePanel( &painter,
                         zoomedXPos, 0, width, height,
                         colorGroup(), FALSE, 1, &fill );
      }

      // Reset painter
      painter.setFont( normalFont );
      painter.setPen( colorGroup().text() );

      if ( selected )
        painter.setPen( colorGroup().highlightedText() );
      else if ( highlighted || current )
        painter.setFont( boldFont );
      if ( !m_pView->activeTable()->getShowColumnNumber() )
      {
        QString colText = KSpreadCell::columnName( x );
        int len = painter.fontMetrics().width( colText );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2, colText );
      }
      else
      {
        QString tmp;
        int len = painter.fontMetrics().width( tmp.setNum(x) );
        if (!col_lay->isHide())
          painter.drawText( zoomedXPos + ( width - len ) / 2,
                            ( height + painter.fontMetrics().ascent() -
                              painter.fontMetrics().descent() ) / 2,
                            tmp.setNum(x) );
      }
      xPos += col_lay->dblWidth();
      ++x;
    }
  }
}


void KSpreadHBorder::focusOutEvent( QFocusEvent* )
{
    if ( m_scrollTimer->isActive() )
        m_scrollTimer->stop();
    m_bMousePressed = false;
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
    int col = table->leftColumn( (m_canvas->doc()->unzoomItX( p.x() ) +
                                       m_canvas->xOffset()), xpos );
    int row = table->topRow( (m_canvas->doc()->unzoomItY( p.y() ) +
                                   m_canvas->yOffset()), ypos );

    KSpreadCell* cell = table->visibleCellAt( col, row );
    if ( !cell )
        return;

    // Get the comment
    QString comment = cell->comment( col, row );

    //If the cell is too short, get the content
    QString content;
    if ( cell->testFlag( KSpreadCell::Flag_CellTooShortX ) ||
        cell->testFlag( KSpreadCell::Flag_CellTooShortY ) )
        content = cell->strOutText();

    if ( content.isEmpty() && comment.isEmpty() )
        return;

    //Append the content text
    if ( !content.isEmpty() )
    {
        //Add 2 extra lines and a text, when both should be in the tooltip
        if ( !comment.isEmpty() )
            comment = "\n\n" + i18n("Comment:") + "\n" + comment;

        comment = content + comment;
    }

    // Determine position and width of the current cell.
    cell = table->cellAt( col, row );
    double u = cell->dblWidth( col );
    double v = cell->dblHeight( row );

    // Special treatment for obscured cells.
    if ( cell->isObscured() && cell->isObscuringForced() )
    {
      cell = cell->obscuringCells().first();
      int moveX = cell->column();
      int moveY = cell->row();

      // Use the obscuring cells dimensions
      u = cell->dblWidth( moveX );
      v = cell->dblHeight( moveY );
      xpos = table->dblColumnPos( moveX );
      ypos = table->dblRowPos( moveY );
    }

    // Get the cell dimensions
    KoRect unzoomedMarker( xpos - m_canvas->xOffset(),
                           ypos - m_canvas->yOffset(),
                           u,
                           v );
    QRect marker( m_canvas->doc()->zoomRect( unzoomedMarker ) );

    if ( marker.contains( p ) )
    {
        tip( marker, comment );
    }
}

#include "kspread_canvas.moc"
