#include "kspread_editors.h"
#include "kspread_canvas.h"
#include "kspread_view.h"
#include "kspread_cell.h"
#include "kspread_doc.h"

#include <qlineedit.h>
#include <qlayout.h>
#include <qapplication.h>

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
  : KSpreadCellEditor( _cell, _parent, _name )
{
  m_pEdit = new KLineEdit( this );
  m_pEdit->installEventFilter( this );
  m_pEdit->setFrame( FALSE );
  m_pEdit->setCompletionMode((KGlobalSettings::Completion)canvas()->view()->doc()->completionMode()  );
  m_pEdit->setCompletionObject( &canvas()->view()->doc()->completion(),true );
  setFocusProxy( m_pEdit );
  setFontPropagation( AllChildren );
  setPalettePropagation( AllChildren );
  m_pEdit->setBackgroundColor(_cell->bgColor(_cell->column(),_cell->row()));
  connect( m_pEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotTextChanged( const QString& ) ) );
  connect( m_pEdit, SIGNAL(completionModeChanged( KGlobalSettings::Completion )),this,SLOT (slotCompletionModeChanged(KGlobalSettings::Completion)));

  // A choose should always start at the edited cell
  canvas()->setChooseMarkerRow( canvas()->markerRow() );
  canvas()->setChooseMarkerColumn( canvas()->markerColumn() );

  m_blockCheck = FALSE;
}

KSpreadTextEditor::~KSpreadTextEditor()
{
    canvas()->endChoose();
}

void KSpreadTextEditor::slotCompletionModeChanged(KGlobalSettings::Completion _completion)
{
  canvas()->view()->doc()->setCompletionMode( _completion );
}

void KSpreadTextEditor::slotTextChanged( const QString& t )
{
    // if ( canvas->chooseCursorPosition() >= 0 )
    // m_pEdit->setCursorPosition( canvas->chooseCursorPosition() );
    checkChoose();

  if((cell()->getFormatNumber(cell()->column(),cell()->row()))==KSpreadCell::Percentage)
        {
        if((t.length()==1) && t[0].isDigit())
                {
                QString tmp=t;
                tmp=t+"%";
                m_pEdit->setText(tmp);
                m_pEdit->setCursorPosition(1);
                }
        }

    canvas()->view()->editWidget()->setText( t );
    // canvas()->view()->editWidget()->setCursorPosition( m_pEdit->cursorPosition() );
}

void KSpreadTextEditor::checkChoose()
{
    if ( m_blockCheck )
        return;

    QString t = m_pEdit->text();
    if ( t[0] != '=' )
        canvas()->endChoose();
    else
    {
      QChar r = t[ m_pEdit->cursorPosition() - 1 - canvas()->chooseTextLen() ];
      kdDebug(36001) << "r='" << QString(r) << "'" << endl;
      if ( ( r == '*' || r == '|' || r == '&' || r == '-' ||
             r == '+' || r == '/' || r == '!' || r == '(' ||
             r == '^' || r == ',' || r == '%' || r == '[' ||
             r == '{' || r == '~' || r == '=' || r == ';' ||
	     r == '>' || r == '<') )
      {
          kdDebug(36001) << "Start CHOOSE" << endl;
          canvas()->startChoose();
      }
      else
      {
          kdDebug(36001) << "End CHOOSE" << endl;
          canvas()->endChoose();
      }
    }
}

void KSpreadTextEditor::resizeEvent( QResizeEvent* )
{
    m_pEdit->setGeometry( 0, 0, width(), height() );
}

void KSpreadTextEditor::handleKeyPressEvent( QKeyEvent* _ev )
{
    // Send the key event to the QLineEdit
    QApplication::sendEvent( m_pEdit, _ev );
}

QString KSpreadTextEditor::text() const
{
    return m_pEdit->text();
}

void KSpreadTextEditor::setText(QString text)
{
    if(m_pEdit !=0)
        m_pEdit->setText(text);
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
    // Only interested in QLineEdit
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
        if ( k->key() == Key_Right || k->key() == Key_Left || k->key() == Key_Up ||
             k->key() == Key_Down || k->key() == Key_Next || k->key() == Key_Prior ||
             k->key() == Key_Escape )
        {
            // Send directly to canvas
            QApplication::sendEvent( parent(), e );
            return TRUE;
        }
        else if( k->key() == Key_Tab )
            return true;

        // End choosing. May be restarted by KSpreadTextEditor::slotTextChanged
        if ( e->type() == QEvent::KeyPress && !k->text().isEmpty() )
        {
            kdDebug(36001) << "eventFilter End Choose" << endl;
            canvas()->endChoose();
            kdDebug(36001) << "Cont" << endl;
        }
    }

    return FALSE;
}



#include "kspread_editors.moc"
