#include "kspread_editors.h"

#include <qlineedit.h>
#include <qlayout.h>
#include <qapplication.h>

/********************************************
 *
 * KSpreadCellEditor
 *
 ********************************************/

KSpreadCellEditor::KSpreadCellEditor( KSpreadCell* _cell, QWidget* _parent, const char* _name )
  : QWidget( _parent, _name )
{
  m_pCell = _cell;

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

KSpreadTextEditor::KSpreadTextEditor( KSpreadCell* _cell, QWidget* _parent, const char* _name )
  : KSpreadCellEditor( _cell, _parent, _name )
{
  m_pEdit = new QLineEdit( this );
  m_pEdit->installEventFilter( this );

  setFocusProxy( m_pEdit );
  setFontPropagation( AllChildren );
  setPalettePropagation( AllChildren );
}

KSpreadTextEditor::~KSpreadTextEditor()
{
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

void KSpreadTextEditor::setCursorPosition(int pos)
{
    m_pEdit->setCursorPosition(pos);
}

void KSpreadTextEditor::insertFormulaChar(int c)
{
}

bool KSpreadTextEditor::eventFilter( QObject* o, QEvent* e )
{
  if ( o != m_pEdit )
    return FALSE;

  if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease )
  {
    QKeyEvent* k = (QKeyEvent*)e;
    if ( k->key() == Key_Escape )
    {
      QApplication::sendEvent( parent(), e );
      return TRUE;
    }
    else if ( k->key() == Key_Right )
    {
	if ( text().length() > cursorPosition() )
	    return FALSE;
	
	QApplication::sendEvent( parent(), e );
	return TRUE;	    
    }
  }
  
  return FALSE;
}


/*********************************************************
 *
 * KSpreadFormulaEditor
 *
 *********************************************************/

KSpreadFormulaEditor::KSpreadFormulaEditor( KSpreadCell* _cell, QWidget* _parent, const char* _name )
  : KSpreadCellEditor( _cell, _parent, _name )
{
  // m_pEdit = new KFormulaEdit( this, 0, 0, TRUE ); // make the formula restricted
  // m_pEdit->setExtraChars(QString("$,"));  // extra characters which are allowed in formula

  m_pEdit = new KFormulaEdit( this );
  m_pEdit->enableSizeHintSignal( true );

  m_pEdit->installEventFilter( this );
  connect( m_pEdit, SIGNAL( sizeHint( QSize ) ), this, SLOT( slotSizeHint( QSize ) ) );

  setFocusProxy( m_pEdit );
  setFontPropagation( AllChildren );
  setPalettePropagation( AllChildren );
}

KSpreadFormulaEditor::~KSpreadFormulaEditor()
{
}

void KSpreadFormulaEditor::resizeEvent( QResizeEvent* )
{
    qDebug("FORMULA w=%i h=%i", width(), height() );
  m_pEdit->setGeometry( 0, 0, width(), height() );
}

void KSpreadFormulaEditor::handleKeyPressEvent( QKeyEvent* _ev )
{
  // Send the key event to the KFormulaEdit
  QApplication::sendEvent( m_pEdit, _ev );
}

QString KSpreadFormulaEditor::text() const
{
    QString tmp( "*" );
    tmp += m_pEdit->text();
    return tmp;
}

void KSpreadFormulaEditor::setText(QString text)
{
    if(m_pEdit !=0)
	m_pEdit->setText(text);
}	
int KSpreadFormulaEditor::cursorPosition() const
{
    // function cursorPosition() no implanted in libkformula
    //m_pEdit->cursorPosition();
    return 0;
}

void KSpreadFormulaEditor::setCursorPosition( int pos )
{
    //no implanted
    //m_pEdit->setCursorPosition(pos);
}
void KSpreadFormulaEditor::insertFormulaChar( int c )
{
    m_pEdit->insertChar( c );
}

bool KSpreadFormulaEditor::eventFilter( QObject* o, QEvent* e )
{
  if ( o != m_pEdit )
    return FALSE;

  if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease )
  {
    QKeyEvent* k = (QKeyEvent*)e;
    if ( k->key() == Key_Escape )
    {
      QApplication::sendEvent( parent(), e );
      return TRUE;
    }
  }

  return FALSE;
}

void KSpreadFormulaEditor::slotSizeHint( QSize _s )
{
    int w = QMAX( _s.width(), width() );
    int h = QMAX( _s.height(), height() );
    resize( w, h );
}

#include "kspread_editors.moc"
