#include "kspread_editors.h"
#include "kspread_canvas.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_view.h"

#include <klineedit.h>
#include <qapplication.h>
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
    m_pEdit->setFocus();
    m_pEdit->setCursorPosition( cur );

    _ev->accept();

    return;
  }

  // Send the key event to the QLineEdit
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



#include "kspread_editors.moc"
