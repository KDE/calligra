#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <qpainter.h>
#include <qdrawutl.h>
#include <qkeycode.h>
#include <qregexp.h>
#include <qpoint.h>
#include <qprinter.h>
#include <qcursor.h>

#include "kspread_table.h"
#include "kspread_view.h"
#include "kspread_python.h"
#include "kspread_map.h"
#include "kspread_doc.h"

#define UPDATE_BEGIN bool b_update_begin = m_bDisplayDirtyFlag; m_bDisplayDirtyFlag = true;
#define UPDATE_END if ( !b_update_begin && m_bDisplayDirtyFlag ) m_pTable->emit_updateCell( this, m_iColumn, m_iRow );

/*****************************************************************************
 *
 * KSpreadCell
 *
 *****************************************************************************/

KSpreadCell::KSpreadCell( KSpreadTable *_table, int _column, int _row, const char* _text ) : KSpreadLayout( _table )
{   
    m_lstDepends.setAutoDelete( TRUE );

    if ( _text != 0L )
      m_strText = _text;

    m_bLayoutDirtyFlag= FALSE;

    QFont font( "Times", 12 );
    m_textFont = font;

    m_iRow = _row;
    m_iColumn = _column;
    
    m_bFormular = FALSE;
    m_bCalcDirtyFlag = FALSE;
    m_bValue = FALSE;
    m_bProgressFlag = FALSE;
    m_bDisplayDirtyFlag = false;
    
    m_bForceExtraCells = FALSE;
    m_iExtraXCells = 0;
    m_iExtraYCells = 0;
    m_pObscuringCell = 0;
}

void KSpreadCell::copyLayout( int _column, int _row )
{
  KSpreadCell *o = m_pTable->cellAt( _column, _row );
    
  setAlign( o->align() );
  setTextFont( o->textFont() );
  setTextColor( o->textColor() );
  setBgColor( o->bgColor( _column, _row) );
  setLeftBorderWidth( o->leftBorderWidth( _column, _row ) );
  setLeftBorderStyle( o->leftBorderStyle( _column, _row ) );
  setLeftBorderColor( o->leftBorderColor( _column, _row ) );
  setTopBorderWidth( o->topBorderWidth( _column, _row ) );
  setTopBorderStyle( o->topBorderStyle( _column, _row ) );
  setTopBorderColor( o->topBorderColor( _column, _row ) );
  setBottomBorderWidth( o->bottomBorderWidth( _column, _row ) );
  setBottomBorderStyle( o->bottomBorderStyle( _column, _row ) );
  setBottomBorderColor( o->bottomBorderColor( _column, _row ) );
  setRightBorderWidth( o->rightBorderWidth( _column, _row ) );
  setRightBorderStyle( o->rightBorderStyle( _column, _row ) );
  setRightBorderColor( o->rightBorderColor( _column, _row ) );
  setPrecision( o->precision() );
  setPrefix( o->prefix() );
  setPostfix( o->postfix() );
  setFloatFormat( o->floatFormat() );
  setFloatColor( o->floatColor() );
  setFaktor( o->faktor() );
  setMultiRow( o->multiRow() );
}

void KSpreadCell::forceExtraCells( int _col, int _row, int _x, int _y )
{
  // Unobscure the objects we obscure right now
  for( int x = _col; x <= _col + m_iExtraXCells; x++ )
    for( int y = _row; y <= _row + m_iExtraYCells; y++ )
      if ( x != _col || y != _row )
      {
	KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
	cell->unobscure();
      }

  // disable forcing ?
  if ( _x == 0 && _y == 0 )
  {
    m_bForceExtraCells = FALSE;
    m_iExtraXCells = 0;
    m_iExtraYCells = 0;
	return;
    }

    m_bForceExtraCells = TRUE;
    m_iExtraXCells = _x;
    m_iExtraYCells = _y;

    // Obscure the cells
    for( int x = _col; x <= _col + _x; x++ )
	for( int y = _row; y <= _row + _y; y++ )
	    if ( x != _col || y != _row )
	    {
		KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
		cell->obscure( this, _col, _row );
	    }
    
    // Refresh the layout
    // QPainter painter;
    // painter.begin( m_pTable->gui()->canvasWidget() );

    m_bLayoutDirtyFlag = TRUE;
    makeLayout( m_pTable->painter(), _col, _row );
}
    
void KSpreadCell::setLayoutDirtyFlag()
{
    m_bLayoutDirtyFlag= TRUE;
	
    if ( m_pObscuringCell )
    {
	m_pObscuringCell->setLayoutDirtyFlag();
	return;
    }
}

bool KSpreadCell::isEmpty()
{
    if ( isDefault() )
	return TRUE;
    
    if ( m_strText.isNull() )
	return TRUE;
    
    if ( m_strText.data()[0] == 0 )
	return TRUE;
    
    return FALSE;
}

bool KSpreadCell::isObscuringForced()
{
    if ( !m_pObscuringCell )
	return FALSE;
    
    return m_pObscuringCell->isForceExtraCells();
}

void KSpreadCell::obscure( KSpreadCell *_cell, int _col, int _row )
{
  m_pObscuringCell = _cell;
  m_iObscuringCellsColumn = _col;
  m_iObscuringCellsRow = _row;
}

void KSpreadCell::unobscure()
{
  m_pObscuringCell = 0L;
  m_bLayoutDirtyFlag= TRUE;
}

QString KSpreadCell::encodeFormular( int _col, int _row )
{
    if ( _col == -1 )
	_col = m_iColumn;
    if ( _row == -1 )
	_row = m_iRow;

    QString erg = "";
    
    char *p = m_strText.data();
    if ( p == 0L )
	return QString();
    
    char buf[ 2 ];
    buf[ 1 ] = 0;
    
    bool fix1 = FALSE;
    bool fix2 = FALSE;
    
    while ( *p != 0 )
    {
	if ( *p == '\"' )
	{
	    while ( *p && *p != '\"' )
	    {
		buf[ 0 ] = *p++;
		erg += buf;
		if ( *p == '\\' )
		{
		    buf[ 0 ] = *p++;
		    erg += buf;
		}
	    }
	    p++;
	}
	else if ( *p != '$' && !isalpha( *p ) )
	{
	    buf[0] = *p++;
	    erg += buf;
	    fix1 = fix2 = FALSE;
	}
	else
	{
	    QString tmp = "";
	    if ( *p == '$' )
	    {
		tmp = "$";
		p++;
		fix1 = TRUE;
	    }
	    if ( isalpha( *p ) )
	    {
		char buffer[ 1024 ];
		char *p2 = buffer;
		while ( *p && isalpha( *p ) )
		{
		    buf[ 0 ] = *p;
		    tmp += buf;
		    *p2++ = *p++;
		}
		*p2 = 0;
		if ( *p == '$' )
		{
		    tmp += "$";
		    p++;
		    fix2 = TRUE;
		}
		if ( isdigit( *p ) )
		{
		    char *p3 = p;
		    int row = atoi( p );
		    while ( *p != 0 && isdigit( *p ) ) p++;
		    // Is it a table
		    if ( *p == '!' )
		    {
			erg += tmp.data();
			fix1 = fix2 = FALSE;
			p = p3;
		    }
		    else // It must be a cell identifier
		    {
			int col = 0;
			if ( strlen( buffer ) >= 2 )
			{
			    col += 26 * ( buffer[0] - 'A' + 1 );
			    col += buffer[1] - 'A' + 1;
			}
			else
			    col += buffer[0] - 'A' + 1;
			if ( fix1 )
			    sprintf( buffer, "$%i", col );
			else
			    sprintf( buffer, "#%i", col - _col );
			erg += buffer;
			if ( fix2 )
			    sprintf( buffer, "$%i#", row );
			else
			    sprintf( buffer, "#%i#", row - _row );
			erg += buffer;		    
		    }
		}
		else
		{
		    erg += tmp.data();
		    fix1 = fix2 = FALSE;
		}
	    }
	    else
	    {
		erg += tmp.data();
		fix1 = FALSE;
	    }
	}
    }

    return QString( erg.data() );
}

QString KSpreadCell::decodeFormular( const char *_text, int _col, int _row )
{
    if ( _col == -1 )
	_col = m_iColumn;
    if ( _row == -1 )
	_row = m_iRow;
    
    QString erg = "";
    const char *p = _text;
    
    if ( p == 0L )
	return QString();
    
    char buf[ 2 ];
    buf[ 1 ] = 0;
    
    while ( *p != 0 )
    {
	if ( *p == '\"' )
	{
	    while ( *p && *p != '\"' )
	    {
		buf[ 0 ] = *p++;
		erg += buf;
		if ( *p == '\\' )
		{
		    buf[ 0 ] = *p++;
		    erg += buf;
		}
	    }
	    p++;
	}
	if ( *p == '#' || *p == '$' )
	{
	    bool fix1 = FALSE;
	    bool fix2 = FALSE;
	    if ( *p++ == '$' )
		fix1 = TRUE;
	    int col = atoi( p );
	    if ( !fix1 )
		col += _col;
	    if ( *p == '-' ) p++;
	    while ( *p != 0 && isdigit( *p ) ) p++;
	    // Skip '#' or '$'
	    if ( *p++ == '$' )
		fix2 = TRUE;
	    int row = atoi( p );
	    if ( !fix2 )
		row += _row;
	    if ( *p == '-' ) p++;
	    while ( *p != 0 && isdigit( *p ) ) p++;
	    // Skip '#' or '$'
	    p++;
	    if ( row <= 0 || col <= 0 )
	    {
		printf("ERROR: out of range\n");
		return QString( _text );
	    }
	    if ( fix1 )
		erg += "$";
	    char buffer[ 20 ];
	    char *p2 = buffer;
	    if ( col > 26 )
		*p2++ = 'A' + ( col / 26 ) - 1;
	    *p2++ = 'A' + ( col % 26 ) - 1;
	    *p2 = 0;
	    erg += buffer;
	    if ( fix2 )
		erg += "$";
	    sprintf( buffer, "%i", row );
	    erg += buffer;
	}
	else
	{
	    buf[ 0 ] = *p++;
	    erg += buf;
	}
    }

    return QString( erg.data() );
}

void KSpreadCell::makeLayout( QPainter &_painter, int _col, int _row )
{
  m_leftBorderPen.setWidth( leftBorderWidth( _col, _row ) );
  m_topBorderPen.setWidth( topBorderWidth( _col, _row ) );

  const char *ptext;
  if ( m_bFormular )
    ptext = m_strFormularOut.data();
  else
    ptext = m_strText.data();

  if ( ptext == 0L )
  {
    m_strOutText = "";
    if ( isDefault() )
      return;
  }
    
  if ( m_bValue )
  {
    QString f2;
    
    char buff[ 1024 ];
    
    double v = m_dValue * m_dFaktor;
	
    if ( floatFormat() == KSpreadCell::AlwaysUnsigned && v < 0.0)
      v *= -1.0;

    f2 = "%";
    if ( floatFormat() == KSpreadCell::AlwaysSigned )
      f2 += "+";
    if ( precision() != -1 )
    {
      sprintf( buff, ".%i", precision() );
      f2 += buff;
    }
    f2 += "f";
    sprintf( buff, f2.data(), v );

    // Remove trailing zeros and the dot if neccessary
    if ( precision() == -1 )
    {
      int i = strlen( buff );
      bool bend = FALSE;
      while ( !bend && i > 0 )
      {
	if ( buff[ i - 1 ] == '0' )
	  i--;
	else if ( buff[ i - 1 ] == '.' )
	{
	  bend = TRUE;
	  i--;
	}
	else
	  bend = TRUE;
      }
      buff[ i ] = 0;
    }

    m_strOutText = "";
    if ( prefix() != 0L )
      m_strOutText += prefix();	
    m_strOutText += buff;
    if ( postfix() != 0L )
      m_strOutText += postfix();
    
    if ( floatColor() == KSpreadCell::NegRed && v < 0.0 )
      m_textPen.setColor( red );
    else
      m_textPen.setColor( m_textColor );
  }
  else
  {
    m_textPen.setColor( m_textColor );
    m_strOutText = ptext;
  }

  _painter.setPen( m_textPen );
  _painter.setFont( m_textFont );    
  QFontMetrics fm = _painter.fontMetrics();
  m_iOutTextWidth = fm.width( m_strOutText.data() );
  m_iOutTextHeight = fm.ascent() + fm.descent();
    
  RowLayout *rl = m_pTable->rowLayout( m_iRow );
  ColumnLayout *cl = m_pTable->columnLayout( m_iColumn );    

  int w = cl->width();
  int h = rl->height();

  // Calculate the extraWidth and extraHeight if needed
  if ( m_bForceExtraCells )
  {
    for ( int x = _col + 1; x <= _col + m_iExtraXCells; x++ )
    {
      ColumnLayout *cl = m_pTable->columnLayout( x );
      w += cl->width() - 1;
    }
    m_iExtraWidth = w;

    for ( int y = _row + 1; y <= _row + m_iExtraYCells; y++ )
    {
      RowLayout *rl = m_pTable->rowLayout( y );
      h += rl->height() - 1;
    }	
    m_iExtraHeight = h;
  }

  int a = m_eAlign;
  if ( a == KSpreadCell::Undefined )
  {
    if ( m_bValue )
      a = KSpreadCell::Right;
    else
      a = KSpreadCell::Left;
  }
    
  switch( a )
  {
  case KSpreadCell::Left:
    m_iTextX = leftBorderWidth( _col, _row) + BORDER_SPACE;
    break;
  case KSpreadCell::Right:
    m_iTextX = w - BORDER_SPACE - m_iOutTextWidth - rightBorderWidth( _col, _row ); 
    break;
  case KSpreadCell::Center:
    m_iTextX = ( w - m_iOutTextWidth ) / 2;
    break;
  }

  // Free all obscured cells
  if ( !m_bForceExtraCells )
  {
    for ( int x = m_iColumn; x <= m_iColumn + m_iExtraXCells; x++ )
      for ( int y = m_iRow; y <= m_iRow + m_iExtraYCells; y++ )
	if ( x != m_iColumn || y != m_iRow )
	{
	  KSpreadCell *cell = m_pTable->cellAt( x, y );
	  cell->unobscure();
	}
    
    m_iExtraXCells = 0;
    m_iExtraYCells = 0;
  }

  // Do we need to break the line into multiple lines and are we allowed to
  // do so?
  if ( m_iOutTextWidth > w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
       rightBorderWidth( _col, _row ) && m_bMultiRow )
  {
    int lines = 1;
    // copy of m_strOutText
    QString o = m_strOutText.data();
    // The stop character
    o += "\n";
    // current word
    QString ws = "";
    m_strOutText = "";
    const char *p = o.data();
    const char *start = o.data();
    // Length of the current word
    int wl = 0;
    // Length of the current line
    int l = 0;
    int spacew = fm.width( ' ' );
    // Loop over all chars
    while ( *p )
    {
      // Separator ?
      if ( *p == ' ' || *p == '\n' )
      {
	// Do we have to print a space or are we
	// at the beginning of the line
	int t = wl;
	if ( start != o.data() )
	  wl += spacew;
	
	// Does the new word fit in this line ?
	if ( t > 0 && l + wl <= w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
	     rightBorderWidth( _col, _row ) )
	{
	  if ( start != o.data() )
	    m_strOutText += " ";
	  m_strOutText += ws.data();
	  start = p + 1;
	  l += t;
	  wl = 0;
	}
	else // The word does not fit in the line
	{
	  // Did we print anything yet ?
	  if ( start != o.data() )
	  {
	    // We printed something yet => start a new line
	    lines++;
	    // New line
	    m_strOutText += "\n";
	  }
	  
	  // Add the current word
	  m_strOutText += ws.data();
	  // Skip this word and the separator
	  start = p + 1;
	  l = wl;
	  wl = 0;
	}
	ws = "";
      }
      // A usual character
      else if ( *p != 0 )
      {
	// Add the character to the current line
	char buf[2];
	buf[0] = *p;
	buf[1] = 0;
	ws += buf;
	wl += fm.width( *p );
      }
      p++;
    }
      
    m_iOutTextHeight *= lines;
    m_iTextX = 0;
    // Calculate the maximum width
    QString t;
    int i;
    int pos = 0;
    m_iOutTextWidth = 0;
    do
    {
      i = m_strOutText.find( "\n", pos );
      if ( i == -1 )
	t = m_strOutText.mid( pos, m_strOutText.length() - pos );
      else
      {
	t = m_strOutText.mid( pos, i - pos );
	pos = i + 1;
      }
      int tw = fm.width( t.data() );
      if ( tw > m_iOutTextWidth )
	m_iOutTextWidth = tw;
    }
    while ( i != -1 );
  }
  
  m_iTextY = ( h - m_iOutTextHeight ) / 2 + fm.ascent();
  
  // Do we have to occupy additional cells right hand ?
  if ( m_iOutTextWidth > w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
       rightBorderWidth( _col, _row ) )
  {
    if ( m_bForceExtraCells )
    {
      m_strOutText = "**";
    }
    else
    {
      int c = m_iColumn + 1;
      int end = 0;
      // Find free cells right hand to this one
      while ( !end && !m_bForceExtraCells )
      {
	ColumnLayout *cl2 = m_pTable->columnLayout( c );
	KSpreadCell *cell = m_pTable->cellAt( c, m_iRow );
	if ( cell->isEmpty() )
	{
	  w += cl2->width() - 1;
	  
	  if ( m_iOutTextWidth <= w - 2 * BORDER_SPACE - leftBorderWidth( _col, _row) -
	       rightBorderWidth( _col, _row ) )
	    end = 1;
	  else
	    c++;		    
	}
	else
	  end = -1;
      }
      
      // Dont occupy additional space for right aligned or centered text or values.
      if ( end == 1 && !m_bFormular && ( m_eAlign == KSpreadCell::Left || m_eAlign == KSpreadCell::Undefined ) )
      {
	m_iExtraWidth = w;
	for( int i = m_iColumn + 1; i <= c; i++ )
	{
	  KSpreadCell *cell = m_pTable->nonDefaultCell( i, m_iRow );
	  cell->obscure( this, m_iColumn, m_iRow );
	}
	m_iExtraXCells = c - m_iColumn;
      }
      else
      {
	m_strOutText = "**";
      }
    }
  }
    
    /* for ( int x = 0; x <= m_iExtraXCells; x++ )
	for ( int y = 0; y <= m_iExtraYCells; y++ )
	    if ( x != 0 || y != 0 )
	    {		
		KSpreadCell *cell = m_pTable->nonDefaultCell( column + x, row + y );
		cell->setKSpreadLayoutDirtyFlag();
		cell->setDisplayDirtyFlag();
	    }  */
	    
  m_bLayoutDirtyFlag = FALSE;
}

bool KSpreadCell::makeDepend( const char *_p, KSpreadDepend ** _dep, bool _second )
{ 
    KSpreadTable *dest_table;
    
    // Test wethe we have a table name
    int pos = 0;
    while ( isalpha( _p[pos] ) || isdigit( _p[pos] ) )
	pos++;
    
    if ( _p[pos] == '!' )
    {
      QString n = _p;
      n = n.left( pos );
      
      _p += pos + 1;
      KSpreadTable *t = m_pTable->map()->findTable( n.data() );
      if ( t == 0L )
      {
	printf("ERROR: Unknown table name '%s'\n",n.data() );
	return FALSE;
      }
      
      if ( _second )
      {
	printf("ERROR: KSpreadTable name not allowed in 2nd corner of a A1:D5 construct.\n");
	return FALSE;
      }
      
      dest_table = t;
    }
    else
      dest_table = m_pTable;
    
    if ( *_p == '$' )
      _p++;
    
    pos = 0;
    int col = 0;
    while ( isupper( _p[ pos ] ) )
    {
	if ( pos >= 2 )
	    return FALSE;
	else if ( pos == 0 )
	    col = _p[pos] - 'A' + 1;
	else if ( pos == 1 )
	    col += ( _p[pos] - 'A' + 1 ) * 26;
	pos++;
    }
    if ( pos == 0 )
	return FALSE;
    
    _p += pos;
    
    if ( *_p == '$' )
	_p++;
    
    pos = 0;
    int row = atoi( _p );
    while ( isdigit( _p[ pos ] ) )
    {
	if ( pos >= 5 )
	    return FALSE;
	pos++;
    }
    if ( pos == 0 )
	return FALSE;
    
    _p += pos;
    
    if ( isalpha( *_p ) || *_p == '$' || *_p == '!' )
	return FALSE;
    
    KSpreadDepend *dep;
    
    if ( _second )
    {
	dep = m_lstDepends.last();
	if ( dep == 0L )
	    return FALSE;
    }
    else
    {
	dep = new KSpreadDepend();
	m_lstDepends.append( dep );
	dep->m_iColumn2 = -1;
	dep->m_pTable = dest_table;
    }
    *_dep = dep;

    if ( _second )
	dep->m_iColumn2 = col;
    else
	dep->m_iColumn = col;
    if ( _second )
	dep->m_iRow2 = row;
    else
	dep->m_iRow = row;
	
    return TRUE;
}

bool KSpreadCell::makeFormular()
{
    char buffer[ 100 ];
    char *p = m_strText.data();
    int pos = 0;
    int start = -1;
    bool ok = TRUE;
    int len = 0;
    // Difference in the length of 'p' and 'formular'
    int diff = 0;
    KSpreadDepend *dep;
    
    m_strFormular = m_strText.data();
    
    while ( p[pos] != 0 || start != -1 )
    {
	if ( start != -1 && !isalpha( p[pos] ) && !isdigit( p[pos] ) && p[pos] != '$' && p[pos] != '!' )
	{
	    if ( p[pos] != '(' )
	    {
		ok = makeDepend( p + start, &dep );
		if ( ok )
		{
		    if ( p[pos] == ':' )
		    {
			ok = makeDepend( p + pos + 1, &dep, TRUE );			
			if ( ok )
			{
			    pos ++;
			    len ++;
			    while ( isalpha( p[pos] ) || isdigit( p[pos ] ) || p[pos] == '$' || p[pos] == '!' )
			    {
				len++;
				pos++;
			    }
			    
			    // NOT 64 BIT CLEAN !!!
			    sprintf( buffer, "[ %i, %i, %i, %i, %i ]", (long)dep->m_pTable, dep->m_iColumn, dep->m_iRow,
				     dep->m_iColumn2, dep->m_iRow2 );
			    m_strFormular.replace( start + diff, len, buffer );
			    diff -= len - strlen( buffer );
			}
		    }
		    else
		    {
		      // NOT 64 BIT CLEAN !!!
		      sprintf( buffer, "Cell( %i, %i, %i )", (long)dep->m_pTable, dep->m_iColumn, dep->m_iRow );
		      m_strFormular.replace( start + diff, len, buffer );
		      diff -= len - strlen( buffer );
		    }
		}
	    }
	    
	    start = -1;
	    if (p[pos] != 0 )
		pos++;
	}
	else if ( start == -1 && ( isupper( p[pos] ) || p[pos] == '$' ) )
	{
	    len = 1;
	    start = pos;
	    pos++;
	}
	else
	{
	    len++;
	    pos++;
	}
    }

    return true;
}

void KSpreadCell::clearFormular()
{
  m_strFormular = "";
  m_lstDepends.clear();
}

bool KSpreadCell::calc( bool _makedepend )
{
  if ( !m_bFormular )
    return TRUE;
  
  if ( !m_bCalcDirtyFlag )
    return TRUE;
    
  if ( m_bProgressFlag )
  {
    printf("ERROR: Circle\n");
    m_strFormularOut = "##";
    m_bLayoutDirtyFlag= TRUE;    
    return FALSE;
  }
  
  m_bLayoutDirtyFlag= TRUE;    
  m_bProgressFlag = TRUE;
    
  if ( _makedepend )
  {
    KSpreadDepend *dep;
    for ( dep = m_lstDepends.first(); dep != 0L; dep = m_lstDepends.next() )
    {
      bool ok;
      
      if ( dep->m_iColumn2 != -1 )
      {
	int left = dep->m_iColumn < dep->m_iColumn2 ? dep->m_iColumn : dep->m_iColumn2;
	int right = dep->m_iColumn > dep->m_iColumn2 ? dep->m_iColumn : dep->m_iColumn2;
	int top = dep->m_iRow < dep->m_iRow2 ? dep->m_iRow : dep->m_iRow2;
	int bottom = dep->m_iRow > dep->m_iRow2 ? dep->m_iRow : dep->m_iRow2;
	for ( int x = left; x <= right; x++ )
	  for ( int y = top; y <= bottom; y++ )
	  {
	    KSpreadCell *cell = dep->m_pTable->cellAt( x, y );
	    if ( cell == 0L )
	      return FALSE;
	    ok = cell->calc( _makedepend );
	    if ( !ok )
	    {
	      printf("ERROR: Calculating\n");
	      m_bProgressFlag = FALSE;
	      m_bCalcDirtyFlag = FALSE;
	      return FALSE;
	    }
	  }
      }
      else
      {
	KSpreadCell *cell = dep->m_pTable->cellAt( dep->m_iColumn, dep->m_iRow );
	if ( cell == 0L )
	  return FALSE;
	ok = cell->calc( _makedepend );
	if ( !ok )
	{
	  printf("ERROR: Calculating\n");
	  m_bProgressFlag = FALSE;
	  m_bCalcDirtyFlag = FALSE;
	  return FALSE;
	}
      }
    }
  }
    
  if ( !m_pTable->doc()->pythonModule()->eval( m_strFormular.data() + 1, m_strFormularOut ) )
  {
    m_strFormularOut = "##";
    printf("ERROR in python stuff\n");
  }
  
  checkValue();
	
  m_bProgressFlag = FALSE;
  m_bCalcDirtyFlag = FALSE;
  return TRUE;
}

const char* KSpreadCell::valueString()
{
  if ( m_bFormular )
    return m_strFormularOut;
  
  return m_strText;
}
    
void KSpreadCell::paintEvent( KSpreadView *_view, const QRect& _rect, QPainter &_painter,
			      int _col, int _row, QRect *_prect )
{
  RowLayout *rl = m_pTable->rowLayout( _row );
  ColumnLayout *cl = m_pTable->columnLayout( _col );
  int tx = m_pTable->columnPos( _col, _view );
  int ty = m_pTable->rowPos( _row, _view );
  
  paintEvent( _view, _rect, _painter, tx, ty, _col, _row, cl, rl, _prect );
}

void KSpreadCell::paintEvent( KSpreadView *_view, const QRect& _rect, QPainter &_painter, int _tx, int _ty,
			      int _col, int _row, ColumnLayout *cl, RowLayout *rl, QRect *_prect )
{
  bool selected = m_pTable->selection().contains( QPoint( _col, _row ) );
    
  if ( m_pObscuringCell )
  {
    _view->drawCell( _painter, m_pObscuringCell, m_iObscuringCellsColumn, m_iObscuringCellsRow );
    m_bLayoutDirtyFlag = FALSE;
    return;
  }
    
  if ( m_bCalcDirtyFlag )
    calc();

  if ( m_bLayoutDirtyFlag)
    makeLayout( _painter, _col, _row );

  int w = cl->width( _view );
  int h = rl->height( _view );
  if ( m_iExtraXCells )
    w = (int)( (float)m_iExtraWidth * _view->zoom() );
  if ( m_iExtraYCells )
    h = (int)( (float)m_iExtraHeight * _view->zoom() );
  
  // Do we really need to display this cell ?
  QRect r2( _tx, _ty, w, h );
  if ( !r2.intersects( _rect ) )
    return;
    
  if ( _prect )
    _prect->setRect( _tx, _ty, w, h );
  
  if ( selected )
    _painter.setBackgroundColor( black );
  else
    _painter.setBackgroundColor( m_bgColor );
  _painter.eraseRect( _tx, _ty, w, h );

  // Draw the border
  if ( m_leftBorderPen.style() == NoPen )
    _painter.setPen( _view->defaultGridPen() );
  else
    _painter.setPen( m_leftBorderPen );
  // Fix a 'bug' in the pens width setting. We still need the upper left corner
  // of the line but a width > 1 wont work for us.
  int dx = (int)ceil( (double)( m_leftBorderPen.width() - 1) / 2.0 * _view->zoom() );
  int dy = (int)ceil( (double)( m_topBorderPen.width() - 1) / 2.0 * _view->zoom() );    
  _painter.drawLine( _tx + dx, _ty, _tx + dx, _ty + h );
  if ( m_topBorderPen.style() == NoPen )
    _painter.setPen( _view->defaultGridPen() );
  else
    _painter.setPen( m_topBorderPen );
  _painter.drawLine( _tx, _ty + dy, _tx + w, _ty + dy );
    
  if ( !m_strOutText.isNull() && m_strOutText.length() > 0 )
  {
    if ( selected )
    {
      QPen p( m_textPen );
      p.setColor( white );
      _painter.setPen( p );
    }
    else
      _painter.setPen( m_textPen );
    _painter.setFont( m_textFont );
    if ( !m_bMultiRow )
      _painter.drawText( _tx + m_iTextX, _ty + m_iTextY, m_strOutText.data() );
    else
    {
      QString t;
      int i;
      int pos = 0;
      int dy = 0;
      int dx = 0;
      QFontMetrics fm = _painter.fontMetrics();
      do
      {
	i = m_strOutText.find( "\n", pos );
	if ( i == -1 )
	  t = m_strOutText.mid( pos, m_strOutText.length() - pos );
	else
	{
	  t = m_strOutText.mid( pos, i - pos );
	  pos = i + 1;
	}
	
	int a = m_eAlign;
	if ( a == KSpreadCell::Undefined )
	{
	  if ( m_bValue )
	    a = KSpreadCell::Right;
	  else
	    a = KSpreadCell::Left;
	}
	
	switch( a )
	{
	case KSpreadCell::Left:
	  m_iTextX = leftBorderWidth( _col, _row) + BORDER_SPACE;
	  break;
	case KSpreadCell::Right:
	  m_iTextX = w - BORDER_SPACE - fm.width( t.data() )
	    - rightBorderWidth( _col, _row );
	  break;
	case KSpreadCell::Center:
	  m_iTextX = ( w - fm.width( t.data() ) ) / 2;
	}

	_painter.drawText( _tx + m_iTextX + dx, _ty + m_iTextY + dy, t.data() );
	dy += fm.descent() + fm.ascent();
      }
      while ( i != -1 );
    }
  }
  
  if ( m_pTable->isShowPageBorders() )
  {
    if ( m_pTable->isOnNewPageY( _row ) )
    {
      _painter.setPen( red );
      _painter.drawLine( _tx, _ty, _tx + w, _ty );
    }
    if ( m_pTable->isOnNewPageX( _col ) )
    {
      _painter.setPen( red );
      _painter.drawLine( _tx, _ty, _tx, _ty + h );
    }
  }
}

void KSpreadCell::print( QPainter &_painter, int _tx, int _ty,
			 int _col, int _row, ColumnLayout *cl, RowLayout *rl, bool _only_left, bool _only_top )
{    
  if ( m_bCalcDirtyFlag )
    calc();
  
  if ( m_bLayoutDirtyFlag)
    makeLayout( _painter, _col, _row );
        
  if ( !_only_left && !_only_top )
  {
    _painter.setBackgroundColor( m_bgColor );
    //	_painter.eraseRect( _tx, _ty, cl->width() - 1, rl->height() - 1 );
  }
  
  // Draw the border
  if ( !_only_top )
  {
    //_painter.setPen( leftBorderPen );
    // Fix a 'bug' in the pens width setting. We still need the upper left corner
    // of the line but a width > 1 wont work for us.
    QPen pen;
    pen.setColor( leftBorderColor( _col, _row) );
    pen.setStyle( m_leftBorderPen.style() );
    _painter.setPen( pen );
    int dx = (int)ceil( (double)( m_leftBorderPen.width() - 1) / 2.0 );
    _painter.drawLine( _tx + dx, _ty, _tx + dx, _ty + rl->height() - 1 );
  }
  if ( !_only_left )
  {
    //_painter.setPen( topBorderPen );
    QPen pen;
    pen.setColor( topBorderColor( _col, _row ) );
    pen.setStyle( m_topBorderPen.style() );
    _painter.setPen( pen );
    int dy = (int)ceil( (double)( m_topBorderPen.width() - 1) / 2.0 );    
    _painter.drawLine( _tx, _ty + dy, _tx + cl->width() - 1, _ty + dy );
  }
    
  if ( !_only_top && !_only_left )
    if ( !m_strOutText.isNull() )
      if ( *(m_strOutText.data()) != 0 )
      {
	_painter.setPen( m_textPen );
	_painter.setFont( m_textFont );
	_painter.drawText( _tx + m_iTextX, _ty + m_iTextY, m_strOutText.data() );
      }
}

void KSpreadCell::setRightBorderStyle( PenStyle s )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column() + 1, row() );
    cell->setLeftBorderStyle( s );
}

void KSpreadCell::setBottomBorderStyle( PenStyle s )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column(), row() + 1 );
    cell->setTopBorderStyle( s );
}

void KSpreadCell::setRightBorderColor( const QColor & _c )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column() + 1, row() );
    cell->setLeftBorderColor( _c );
}

void KSpreadCell::setBottomBorderColor( const QColor & _c )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column(), row() + 1 );
    cell->setTopBorderColor( _c );
}

void KSpreadCell::setRightBorderWidth( int _w )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column() + 1, row() );
    cell->setLeftBorderWidth( _w );
}

void KSpreadCell::setBottomBorderWidth( int _w )
{
    KSpreadCell * cell = m_pTable->nonDefaultCell( column(), row() + 1 );
    cell->setTopBorderWidth( _w );
}

int KSpreadCell::width( int _col, KSpreadView *_view )
{
  if ( _view )
  {
    if ( m_bForceExtraCells )
      return (int)( m_iExtraWidth * _view->zoom() );
    
    ColumnLayout *cl = m_pTable->columnLayout( _col );
    return cl->width( _view );
  }
  
  if ( m_bForceExtraCells )
    return m_iExtraWidth;
    
  ColumnLayout *cl = m_pTable->columnLayout( _col );
  return cl->width();
}

int KSpreadCell::height( int _row, KSpreadView *_view )
{
  if ( _view )
  {
    if ( m_bForceExtraCells )
      return (int)( m_iExtraHeight * _view->zoom() );
    
    RowLayout *rl = m_pTable->rowLayout( _row );
    return rl->height( _view );
  }
  
  if ( m_bForceExtraCells )
    return m_iExtraHeight;
    
  RowLayout *rl = m_pTable->rowLayout( _row );
  return rl->height();
}

const QColor& KSpreadCell::bgColor( int _col, int _row )
{
  if ( isDefault() )
  {
    RowLayout *rl = m_pTable->rowLayout( _row );
    ColumnLayout *cl = m_pTable->columnLayout( _col );
    
    if ( rl->time() > cl->time() )
      return rl->bgColor();
    else
      return cl->bgColor();
  }
  else
    return m_bgColor;
}

int KSpreadCell::leftBorderWidth( int _col, int _row, KSpreadView *_view )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderWidth( _view );
	else
	    return cl->leftBorderWidth( _view );
    }
    
    if ( _view )
	return (int) ( m_iLeftBorderWidth * _view->zoom() ); 
    else
	return m_iLeftBorderWidth;
}

int KSpreadCell::topBorderWidth( int _col, int _row, KSpreadView *_view )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderWidth( _view );
	else
	    return cl->topBorderWidth( _view );
    }

    if ( _view )
	return (int) ( m_iTopBorderWidth * _view->zoom() ); 
    else
	return m_iTopBorderWidth;
}

int KSpreadCell::rightBorderWidth( int _col, int _row, KSpreadView *_view )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col + 1 );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderWidth( _view );
	else
	    return cl->leftBorderWidth( _view );
    }

    KSpreadCell * cell = m_pTable->cellAt( column() + 1, row() );
    return cell->leftBorderWidth( column() + 1, row(), _view );
}

int KSpreadCell::bottomBorderWidth( int _col, int _row, KSpreadView *_view )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row + 1 );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderWidth( _view );
	else
	    return cl->topBorderWidth( _view );
    }

    KSpreadCell * cell = m_pTable->cellAt( column(), row() + 1 );
    return cell->topBorderWidth( column() + 1, row(), _view );
}

PenStyle KSpreadCell::leftBorderStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderStyle();
	else
	    return cl->leftBorderStyle();
    }

    return m_leftBorderPen.style();
}

PenStyle KSpreadCell::topBorderStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderStyle();
	else
	    return cl->topBorderStyle();
    }

    return m_topBorderPen.style();
}

PenStyle KSpreadCell::rightBorderStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col + 1 );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderStyle();
	else
	    return cl->leftBorderStyle();
    }

    KSpreadCell * cell = m_pTable->cellAt( column() + 1, row() );
    return cell->leftBorderStyle( column() + 1, row() );
}

PenStyle KSpreadCell::bottomBorderStyle( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row + 1 );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderStyle();
	else
	    return cl->topBorderStyle();
    }

    KSpreadCell * cell = m_pTable->cellAt( column(), row() + 1 );
    return cell->topBorderStyle( column(), row() + 1 );
}

const QColor& KSpreadCell::leftBorderColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderColor();
	else
	    return cl->leftBorderColor();
    }

    return m_leftBorderPen.color();
}

const QColor& KSpreadCell::topBorderColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderColor();
	else
	    return cl->topBorderColor();
    }

    return m_topBorderPen.color();
}

const QColor& KSpreadCell::rightBorderColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row );
	ColumnLayout *cl = m_pTable->columnLayout( _col + 1 );
	
	if ( rl->time() > cl->time() )
	    return rl->leftBorderColor();
	else
	    return cl->leftBorderColor();
    }

    KSpreadCell * cell = m_pTable->cellAt( column() + 1, row() );
    return cell->leftBorderColor( column() + 1, row() );
}

const QColor& KSpreadCell::bottomBorderColor( int _col, int _row )
{
    if ( isDefault() )
    {
	RowLayout *rl = m_pTable->rowLayout( _row + 1 );
	ColumnLayout *cl = m_pTable->columnLayout( _col );
	
	if ( rl->time() > cl->time() )
	    return rl->topBorderColor();
	else
	    return cl->topBorderColor();
    }

    KSpreadCell * cell = m_pTable->cellAt( column(), row() + 1 );
    return cell->topBorderColor( column(), row() + 1 );
}

void KSpreadCell::setPrefix( const char * _prefix )
{
  m_strPrefix = _prefix;
  m_bLayoutDirtyFlag= TRUE; 
}

void KSpreadCell::setPostfix( const char * _postfix )
{
  m_strPostfix = _postfix;
  m_bLayoutDirtyFlag= TRUE;
}

/*
void KSpreadCell::initAfterLoading()
{
    if ( text.isNull() )
	return;
    setText( text.data() );
}
*/

void KSpreadCell::setText( const char *_text )
{
  if ( m_pTable->isLoading() )
  {
    m_strText = _text;
    return;
  }
  
  UPDATE_BEGIN;

  m_lstDepends.clear();
    
  if ( *_text == '=' )
  {
    m_bCalcDirtyFlag = TRUE;
    m_bLayoutDirtyFlag= TRUE;
    m_bFormular = TRUE;
    m_strText = _text;

    if ( !makeFormular() )
      printf("ERROR: Syntax ERROR\n");
	// A Hack!!!! For testing only
	// QString ret = encodeFormular( column, row );
	// decodeFormular( ret, column, row );
  }
  else
  {
    if ( m_bFormular )
      clearFormular();
    
    m_strText = _text;

    checkValue();
		
    m_bLayoutDirtyFlag= TRUE;
    m_bFormular = FALSE;
  }
    
  QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
  for( ; it.current(); ++it )
  {
    QIntDictIterator<KSpreadCell> it3( it.current()->m_dctCells );
    for ( ; it3.current(); ++it3 )
      if ( it3.current() != this )
	it3.current()->setCalcDirtyFlag( m_pTable, m_iColumn, m_iRow );
  }
  
  QListIterator<KSpreadTable> it2( m_pTable->map()->tableList() );
  for( ; it2.current(); ++it2 )
  {    
    QIntDictIterator<KSpreadCell> it4( it2.current()->m_dctCells );
    for ( ; it4.current(); ++it4 ) 
      it4.current()->calc( TRUE );
  }

  // Update a chart for example if it depends on this cell.
  if ( m_iRow != 0 && m_iColumn != 0 )
  {
    CellBinding *bind;
    for ( bind = m_pTable->firstCellBinding(); bind != 0L; bind = m_pTable->nextCellBinding() )
    {
      if ( bind->contains( m_iColumn, m_iRow ) )
	bind->cellChanged( this );
    }
  }

  UPDATE_END;
}

void KSpreadCell::checkValue()
{
    bool old_value = m_bValue;

    // If the input is empty, we dont have a value
    if ( m_strText.data() == 0 || m_strText.data()[0] == 0 )
    {
      m_bValue = FALSE;
      return;
    }
    
    const char *p = m_strText.data();	   
    if ( m_bFormular )
      p = m_strFormularOut.data();
    const char *ptext = p;
    
    if ( p == 0L )
    {
      m_bValue = FALSE;
      return;
    }
    
    // Test wether it is a numeric value
    m_bValue = TRUE;
    bool point = FALSE;
    if ( *p == '+' || *p == '-' )
	p++;
    while ( *p != 0 && m_bValue )
    {
	if ( *p == ',' || *p == '.' )
	{
	    // Only one point or comma is allowed
	    if ( point )
	      m_bValue = FALSE;
	    else
	      point = TRUE;
	}
	else if ( !isdigit( *p ) )
	  m_bValue = FALSE;
	p++;
    }

    if ( m_bValue )
	m_dValue = atof( ptext );

    /* if ( old_value != bValue )
	displayDirtyFlag = TRUE; */
}

void KSpreadCell::setCalcDirtyFlag( KSpreadTable *_table, int _column, int _row )
{
    bool isdep = FALSE;
    
    KSpreadDepend *dep;
    for ( dep = m_lstDepends.first(); dep != 0L; dep = m_lstDepends.next() )
    {
	if ( dep->m_iColumn2 != -1 )
	{
	    int left = dep->m_iColumn < dep->m_iColumn2 ? dep->m_iColumn : dep->m_iColumn2;
	    int right = dep->m_iColumn > dep->m_iColumn2 ? dep->m_iColumn : dep->m_iColumn2;
	    int top = dep->m_iRow < dep->m_iRow2 ? dep->m_iRow : dep->m_iRow2;
	    int bottom = dep->m_iRow > dep->m_iRow2 ? dep->m_iRow : dep->m_iRow2;
	    if ( _table == dep->m_pTable )
		if ( left <= _column && _column <= right )
		    if ( top <= _row && _row <= bottom )
			isdep = TRUE;
	}
	else if ( dep->m_iColumn == _column && dep->m_iRow == _row && dep->m_pTable == _table )
	    isdep = TRUE;
    }
    
    if ( isdep )
    {
	m_bCalcDirtyFlag = TRUE;

	QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
	for( ; it.current(); ++it )
	{
	    QIntDictIterator<KSpreadCell> it2( it.current()->m_dctCells );
	    for ( ; it2.current(); ++it2 ) 
		it2.current()->setCalcDirtyFlag( m_pTable, m_iColumn, m_iRow );
	}
    }
}

KSpreadCell::~KSpreadCell()
{
}

