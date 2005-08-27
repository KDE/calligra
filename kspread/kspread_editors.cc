/* This file is part of the KDE project

   Copyright 1999-2004 The KSpread Team <koffice-devel@mail.kde.org>

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


#include "kspread_editors.h"
#include "kspread_canvas.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_selection.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

#include <klineedit.h>
#include <qapplication.h>
#include <qbutton.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qregexp.h>
#include <kdebug.h>

/********************************************
 *
 * KSpreadCellEditor
 *
 ********************************************/

KSpreadCellEditor::KSpreadCellEditor( KSpreadCell* _cell, KSpreadCanvas* _parent, const char* _name )
  : QWidget( _parent, _name )
{
  m_pCell = _cell;
  m_pCanvas = _parent;
  setFocusPolicy( QWidget::StrongFocus );
}

KSpreadCellEditor::~KSpreadCellEditor()
{
}

/********************************************
 *
 * KSpreadTextEditor
 *
 ********************************************/

KSpreadTextEditor::KSpreadTextEditor( KSpreadCell* _cell, KSpreadCanvas* _parent, const char* _name )
  : KSpreadCellEditor( _cell, _parent, _name ),
    m_sizeUpdate(false),
    m_length(0),
    m_fontLength(0)
{
  m_pEdit = new KLineEdit( this );
  m_pEdit->installEventFilter( this );
  m_pEdit->setFrame( FALSE );
  m_pEdit->setCompletionMode((KGlobalSettings::Completion)canvas()->view()->doc()->completionMode()  );
  m_pEdit->setCompletionObject( &canvas()->view()->doc()->completion(),true );
  setFocusProxy( m_pEdit );

  connect( m_pEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotTextChanged( const QString& ) ) );
  connect( m_pEdit, SIGNAL(completionModeChanged( KGlobalSettings::Completion )),this,SLOT (slotCompletionModeChanged(KGlobalSettings::Completion)));

  // A choose should always start at the edited cell
//  canvas()->setChooseMarkerRow( canvas()->markerRow() );
//  canvas()->setChooseMarkerColumn( canvas()->markerColumn() );

  m_blockCheck = FALSE;

  // set font size according to zoom factor
  QFont font( _cell->font() );
  font.setPointSizeFloat( 0.01 * _parent->doc()->zoom() * font.pointSizeFloat() );
  m_pEdit->setFont( font );

  if (m_fontLength == 0)
  {
    QFontMetrics fm( m_pEdit->font() );
    m_fontLength = fm.width('x');
  }
}

KSpreadTextEditor::~KSpreadTextEditor()
{
  canvas()->endChoose();
}

void KSpreadTextEditor::cut()
{
    if(m_pEdit)
        m_pEdit->cut();
}

void KSpreadTextEditor::paste()
{
    if( m_pEdit)
        m_pEdit->paste();
}

void KSpreadTextEditor::copy()
{
    if( m_pEdit)
        m_pEdit->copy();
}

void KSpreadTextEditor::setEditorFont(QFont const & font, bool updateSize)
{
  if (!m_pEdit)
    return;

  QFont tmpFont( font );
  tmpFont.setPointSizeFloat( 0.01 * canvas()->doc()->zoom() * tmpFont.pointSizeFloat() );
  m_pEdit->setFont( tmpFont );

  if (updateSize)
  {
    QFontMetrics fm( m_pEdit->font() );
    m_fontLength = fm.width('x');

    int mw = fm.width( m_pEdit->text() ) + m_fontLength;
    // don't make it smaller: then we would have to repaint the obscured cells
    if (mw < width())
      mw = width();

    int mh = fm.height();
    if (mh < height())
      mh = height();

    setGeometry(x(), y(), mw, mh);
    m_sizeUpdate = true;
  }
}

void KSpreadTextEditor::slotCompletionModeChanged(KGlobalSettings::Completion _completion)
{
  canvas()->view()->doc()->setCompletionMode( _completion );
}

void KSpreadTextEditor::slotTextChanged( const QString& t )
{
  // if ( canvas->chooseCursorPosition() >= 0 )
  // m_pEdit->setCursorPosition( canvas->chooseCursorPosition() );
  if (!checkChoose())
    return;

  if (t.length() > m_length)
  {
    // allocate more space than needed. Otherwise it might be too slow
    m_length = t.length() + 5;

    // Too slow for long texts
    // QFontMetrics fm( m_pEdit->font() );
    //  int mw = fm.width( t ) + fm.width('x');
    int mw = m_fontLength * m_length;

    if (mw < width())
      mw = width();

    if (t.isRightToLeft())
      setGeometry(x() - mw + width(), y(), mw, height());
    else
      setGeometry(x(), y(), mw, height());

    m_length -= 2;
  }

  if ( (cell()->formatType()) == Percentage_format )
  {
    if ( (t.length() == 1) && t[0].isDigit() )
    {
      QString tmp = t + " %";
      m_pEdit->setText(tmp);
      m_pEdit->setCursorPosition(1);
      return;
    }
  }

  canvas()->view()->editWidget()->setText( t );
  // canvas()->view()->editWidget()->setCursorPosition( m_pEdit->cursorPosition() );
}

bool KSpreadTextEditor::checkChoose()
{
    if ( m_blockCheck )
        return false;

    QString t = m_pEdit->text();
    if ( t[0] != '=' )
        canvas()->endChoose();
    else
    {
      QChar r = t[ m_pEdit->cursorPosition() - 1 - canvas()->chooseTextLen() ];
      if ( ( r == '*' || r == '|' || r == '&' || r == '-' ||
             r == '+' || r == '/' || r == '!' || r == '(' ||
             r == '^' || r == ',' || r == '%' || r == '[' ||
             r == '{' || r == '~' || r == '=' || r == ';' ||
	     r == '>' || r == '<') )
      {
          canvas()->startChoose();
      }
      else
      {
          canvas()->endChoose();
      }
    }
    return true;
}

void KSpreadTextEditor::resizeEvent( QResizeEvent* )
{
    m_pEdit->setGeometry( 0, 0, width(), height() );
}

void KSpreadTextEditor::handleKeyPressEvent( QKeyEvent * _ev )
{
  if (_ev->key() == Qt::Key_F4)
  {
    if (m_pEdit == 0)
    {
      QApplication::sendEvent( m_pEdit, _ev );
      return;
    }

    QRegExp exp("(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)$");

    int cur = m_pEdit->cursorPosition();
    QString tmp, tmp2;
    int n = -1;

    // this is ugly, and sort of hack
    // FIXME rewrite to use the real KSpreadTokenizer
    unsigned i;
    for( i = 0; i < 10; i++ )
    {
      tmp =  m_pEdit->text().left( cur+i );
      tmp2 = m_pEdit->text().right( m_pEdit->text().length() - cur - i );

      n = exp.search(tmp);
      if( n >= 0 ) break;
    }

    if (n == -1) return;

    QString newPart;
    if ((exp.cap(1) == "$") && (exp.cap(3) == "$"))
      newPart = "$" + exp.cap(2) + exp.cap(4);
    else if ((exp.cap(1) != "$") && (exp.cap(3) != "$"))
      newPart = "$" + exp.cap(2) + "$" + exp.cap(4);
    else if ((exp.cap(1) == "$") && (exp.cap(3) != "$"))
      newPart = exp.cap(2) + "$" + exp.cap(4);
    else if ((exp.cap(1) != "$") && (exp.cap(3) == "$"))
      newPart = exp.cap(2) + exp.cap(4);

    QString newString = tmp.left(n);
    newString += newPart;
    cur = newString.length() - i;
    newString += tmp2;

    m_pEdit->setText(newString);
    m_pEdit->setCursorPosition( cur );

    _ev->accept();

    return;
  }

  // Send the key event to the KLineEdit
  QApplication::sendEvent( m_pEdit, _ev );
}

void KSpreadTextEditor::handleIMEvent( QIMEvent * _ev )
{
    // send the IM event to the KLineEdit
    QApplication::sendEvent( m_pEdit, _ev );
}

QString KSpreadTextEditor::text() const
{
    return m_pEdit->text();
}

void KSpreadTextEditor::setText(QString text)
{
    if (m_pEdit != 0)
        m_pEdit->setText(text);

    if (m_fontLength == 0)
    {
      QFontMetrics fm( m_pEdit->font() );
      m_fontLength = fm.width('x');
    }
}

int KSpreadTextEditor::cursorPosition() const
{
    return m_pEdit->cursorPosition();
}

void KSpreadTextEditor::setCursorPosition( int pos )
{
    m_pEdit->setCursorPosition(pos);
    canvas()->view()->editWidget()->setCursorPosition( pos );
    checkChoose();
}

void KSpreadTextEditor::insertFormulaChar(int /*c*/)
{
}

bool KSpreadTextEditor::eventFilter( QObject* o, QEvent* e )
{
    // Only interested in KLineEdit
    if ( o != m_pEdit )
        return FALSE;
    if ( e->type() == QEvent::FocusOut )
    {
        canvas()->setLastEditorWithFocus( KSpreadCanvas::CellEditor );
        return FALSE;
    }

    if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease )
    {
        QKeyEvent* k = (QKeyEvent*)e;
        if ( !( k->state() & Qt::ShiftButton )|| canvas()->chooseFormulaArea())
        {
            if ( k->key() == Key_Up || k->key() == Key_Down || 
                 k->key() == Key_Next || k->key() == Key_Prior || 
                 k->key() == Key_Escape || k->key() == Key_Tab )
            {
                // Send directly to canvas
                QApplication::sendEvent( parent(), e );
                return TRUE;
            }
        }
        // End choosing. May be restarted by KSpreadTextEditor::slotTextChanged
        if ( e->type() == QEvent::KeyPress && !k->text().isEmpty() )
        {
            //kdDebug(36001) << "eventFilter End Choose" << endl;
            canvas()->endChoose();
            //kdDebug(36001) << "Cont" << endl;
        }
    }

    return FALSE;
}

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
                    QString tmp = (*it).sheet_name;
                    tmp += "!";
                    tmp += util_rangeName((*it).rect);
                    m_pView->canvasWidget()->gotoLocation( KSpreadRange(tmp, m_pView->doc()->map()));
                    return;
                }
            }

            // Set the cell component to uppercase:
            // Sheet1!a1 -> Sheet1!A2
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
                    KSpreadSheet * t = m_pView->activeSheet();
                    // set area name on current selection/cell

                    m_pView->doc()->addAreaName(rect, ltext.lower(),
                                                t->sheetName());
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
 * The line-editor that appears above the sheet and allows to
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
  isArray = false;

  installEventFilter(m_pCanvas);

  if ( !m_pCanvas->doc()->isReadWrite() || !m_pCanvas->activeSheet() )
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
  m_pCanvas->deleteEditor( true /*keep changes*/, isArray);
  isArray = false;
  // will take care of the buttons
}

void KSpreadEditWidget::keyPressEvent ( QKeyEvent* _ev )
{
    // Dont handle special keys and accelerators, except Enter ones
    if (( ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
         || ( _ev->state() & Qt::ShiftButton )
         || ( _ev->key() == Key_Shift )
         || ( _ev->key() == Key_Control ) )
      && (_ev->key() != Key_Return) && (_ev->key() != Key_Enter))
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
      isArray = (_ev->state() & Qt::AltButton) &&
          (_ev->state() & Qt::ControlButton);
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



#include "kspread_editors.moc"
