/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#include <qprinter.h>

#include "kspread_dlg_cons.h"
#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_util.h"

#include <kapp.h>
#include <qmessagebox.h>
#include <klocale.h>

#include <list>

KSpreadConsolidate::KSpreadConsolidate( KSpreadView* parent, const char* name )
	: QDialog( 0L, name )
{
  m_pView = parent;
  
  setCaption( i18n("Consolidate") );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 10, 20, 150, 20 );
  tmpQLabel->setText( i18n("Function") );

  m_pFunction = new QComboBox( this );
  m_pFunction->setGeometry( 10, 40, 150, 30 );
  m_idSumme = 0; m_pFunction->insertItem( i18n("sum"), m_idSumme );
  m_idAverage = 1; m_pFunction->insertItem( i18n("average"), m_idAverage );
    
  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 10, 80, 150, 30 );
  tmpQLabel->setText( i18n("Reference") );
  
  m_pRef = new QLineEdit( this );
  m_pRef->setGeometry( 10, 110, 150, 30 );
  
  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 10, 140, 150, 30 );
  tmpQLabel->setText( i18n("Entered References") );

  m_pRefs = new QListBox( this );
  m_pRefs->setGeometry( 10, 170, 150, 80 );
  
  m_pOk = new QPushButton( i18n("Ok"), this );
  m_pOk->setGeometry( 180, 20, 100, 30 );
  m_pOk->setEnabled( false );
  m_pClose = new QPushButton( i18n("Cancel"), this );
  m_pClose->setGeometry( 180, 60, 100, 30 );
  
  m_pAdd = new QPushButton( i18n("Add"), this );
  m_pAdd->setGeometry( 180, 120, 100, 30 );
  m_pRemove = new QPushButton( i18n("Remove"), this );
  m_pRemove->setGeometry( 180, 160, 100, 30 );

  m_pRow = new QCheckBox( i18n("Description in Rows"), this );
  m_pRow->setGeometry( 10, 270, 300, 30 );
  m_pCol = new QCheckBox( i18n("Description in Columns"), this );
  m_pCol->setGeometry( 10, 300, 300, 30 );
  m_pCopy = new QCheckBox( i18n("Copy data"), this );
  m_pCopy->setGeometry( 10, 330, 300, 30 );

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( m_pAdd, SIGNAL( clicked() ), this, SLOT( slotAdd() ) );
  connect( m_pRemove, SIGNAL( clicked() ), this, SLOT( slotRemove() ) );
  connect( m_pRef, SIGNAL( returnPressed() ), this, SLOT( slotReturnPressed() ) );
  
  connect( m_pView, SIGNAL( sig_selectionChanged( KSpreadTable*, const QRect& ) ),
	   this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );
}

enum Function { F_SUM, F_AVERAGE };
enum Description { D_ROW, D_COL, D_NONE, D_BOTH };

struct st_cell
{
  QString xdesc;
  QString ydesc;
  KSpreadCell* cell;
  QString table;
  int x;
  int y;
};

void KSpreadConsolidate::slotOk()
{
  KSpreadMap *map = m_pView->doc()->map();

  KSpreadTable* table = m_pView->activeTable();
  int dx = m_pView->markerColumn();
  int dy = m_pView->markerRow();

  Function f = F_SUM;
  if ( m_pFunction->currentItem() == m_idAverage )
    f = F_AVERAGE;
  
  QStrList r = refs();
  list<KSpread::Range> ranges;
  const char *s = r.first();
  for( ; s != 0L; s = r.next() )
  {
    // TODO: check for exceptions
    ranges.push_back( util_parseRange2( s, map ) );
    if ( strlen( ranges.back().table.in() ) == 0 )
      ranges.back().table = CORBA::string_dup( table->name() );
  }

  Description desc;
  if ( m_pRow->isChecked() && m_pCol->isChecked() )
    desc = D_BOTH;
  else if ( m_pRow->isChecked() )
    desc = D_ROW;
  else if ( m_pCol->isChecked() )
    desc = D_COL;
  else
    desc = D_NONE;
  
  // Check wether all ranges have same size
  assert( ranges.size() > 0 );
  list<KSpread::Range>::iterator it = ranges.begin();
  int w = it->right - it->left + 1;
  int h = it->bottom - it->top + 1;
  if ( w <= ( ( desc == D_BOTH || desc == D_COL ) ? 1 : 0 ) ||
       h <= ( ( desc == D_BOTH || desc == D_ROW ) ? 1 : 0 ) )
  {
    QString tmp;
    tmp.sprintf( i18n( "The range\n%s\nis too small" ), r.first() );
    QMessageBox::critical( 0L, i18n("Error"), tmp );
    return;
  }
  
  ++it;
  int i = 1;
  for( ; it != ranges.end(); ++it, i++ )
  {
    int w2 = it->right - it->left + 1;
    int h2 = it->bottom - it->top + 1;
    if ( ( desc == D_NONE && ( w != w2 || h != h2 ) ) ||
	 ( desc == D_ROW && h != h2 ) ||
	 ( desc == D_COL && w != w2 ) )
    {
      QString tmp;
      tmp.sprintf( i18n( "The ranges\n%s\nand\n%s\nhave different size" ),
		   r.first(), r.at( i ) );
      QMessageBox::critical( 0L, i18n("Error"), tmp );
      return;
    }
  }
  
  // Create the consolidation table
  if ( desc == D_NONE )
  {    
    // Check wether the destination is part of the source ...
    QRect dest;
    dest.setCoords( dx, dy, dx + w - 1, dy + h - 1 );
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      QRect r;
      r.setCoords( it->left, it->top, it->right, it->bottom );
      if ( t == table && r.intersects( dest ) )
      {
	QString tmp( i18n("The source tables intersect with the destination table") );
	QMessageBox::critical( 0L, i18n("Error"), tmp );
	return;
      }
    }
    
    for( int x = 0; x < w; x++ )
    {
      for( int y = 0; y < h; y++ )
      {
	double dbl = 0.0;
	QString formel;
	if ( f == F_AVERAGE )
	  formel = "=(";
	else if ( f == F_SUM )
	  formel = "=";
	else
	  assert( 0 );

	it = ranges.begin();
	for( ; it != ranges.end(); ++it )
        {
	  // Calculate value directly
	  KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
	  assert( t );
	  KSpreadCell *c = t->cellAt( x + it->left, y + it->top );
	  if ( c && c->isValue() )
	  {
	    if ( f == F_SUM || f == F_AVERAGE )
	      dbl += c->valueDouble();
	    else
	      assert( 0 );
	  }
	  
	  // Built formular
	  cerr << "RANGE: l=" << it->left << " r=" << it->right << endl;
	  if ( f == F_SUM || f == F_AVERAGE )
	  {    
	    if ( formel.length() > 2 )
	      formel += "+";
	    formel += it->table.in();
	    formel += "!";
	    formel += util_cellName( x + it->left, y + it->top );
	  }
	  else
	    assert( 0 );
	}
	
	if ( f == F_AVERAGE )
	{
	  char buffer[ 100 ];
	  sprintf( buffer, "%i", h );
	  formel += ") / ";
	  formel += buffer;
	  dbl /= (double)h;
	}
	
	if ( m_pCopy->isChecked() )
	  formel.sprintf( "%f", dbl );
	cerr << "Setting (" << dx + x << "|" << dy + y << ") = " << formel << endl;
	table->setText( dy + y, dx + x, formel );
      }
    }
  }
  else if ( desc == D_ROW )
  {
    // Get list of all descriptions in the rows
    QStrList lst;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      for( unsigned int x = it->left; x <= it->right ; ++x )
      {
	KSpreadCell *c = t->cellAt( x, it->top );
	if ( c )
	{
	  const char *s = c->valueString();
	  if ( s != 0L && lst.find( s ) == -1 )
	    lst.inSort( s );
	}
      }
    }

    // Check wether the destination is part of the source ...
    QRect dest;
    dest.setCoords( dx, dy, dx + lst.count() - 1, dy + h - 1 );
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      QRect r;
      r.setCoords( it->left, it->top, it->right, it->bottom );
      if ( t == table && r.intersects( dest ) )
      {
	QString tmp( i18n("The source tables intersect with the destination table") );
	QMessageBox::critical( 0L, i18n("Error"), tmp );
	return;
      }
    }

    // Now create the consolidation table
    int x = 0;
    const char *s;
    for( s = lst.first(); s != 0L; s = lst.next(), x++ )
    {
      table->setText( dy, dx + x, s );

      for( int y = 1; y < h; ++y )
      {
	int count = 0;
	double dbl = 0.0;
	QString formel;
	if ( f == F_AVERAGE )
	  formel = "=(";
	else if ( f == F_SUM )
	  formel = "=";
	else
	  assert( 0 );

	it = ranges.begin();
	for( ; it != ranges.end(); ++it )
        {
	  for( unsigned int i = it->left; i <= it->right; i++ )
	  {    
	    KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
	    assert( t );
	    KSpreadCell *c = t->cellAt( i, it->top );
	    if ( c )
	    {
	      const char *v = c->valueString();
	      if ( v != 0L && strcmp( s, v ) == 0L )
	      {
		KSpreadCell *c2 = t->cellAt( i, y + it->top );
		count++;
		// Calculate value
		if ( c2 && c2->isValue() )
		{  
		  if ( f == F_SUM || f == F_AVERAGE )
		    dbl += c2->valueDouble();
		  else
		    assert( 0 );
		}
		// Create formular
		if ( f == F_SUM || f == F_AVERAGE )
		{    
		  if ( formel != "=" )
		    formel += "+";
		  formel += it->table.in();
		  formel += "!";
		  formel += util_cellName( i, y + it->top );
		}
		else
		  assert( 0 );
	      }
	    }
	  }
	}

	if ( f == F_AVERAGE )
	{
	  char buffer[ 100 ];
	  sprintf( buffer, "%i", count );
	  formel += ") / ";
	  formel += buffer;
	  dbl /= (double)count;
	}

	if ( m_pCopy->isChecked() )
	  formel.sprintf( "%f", dbl );	
	table->setText( dy + y, dx + x, formel );
      }
    }
  }
  else if ( desc == D_COL )
  {
    // Get list of all descriptions in the columns
    QStrList lst;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      for( unsigned int y = it->top; y <= it->bottom ; ++y )
      {
	KSpreadCell *c = t->cellAt( it->left, y );
	if ( c )
	{
	  const char *s = c->valueString();
	  if ( s != 0L && lst.find( s ) == -1 )
	    lst.inSort( s );
	}
      }
    }

    // Check wether the destination is part of the source ...
    QRect dest;
    dest.setCoords( dx, dy, dx + w - 1, dy + lst.count() - 1 );
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      QRect r;
      r.setCoords( it->left, it->top, it->right, it->bottom );
      if ( t == table && r.intersects( dest ) )
      {
	QString tmp( i18n("The source tables intersect with the destination table") );
	QMessageBox::critical( 0L, i18n("Error"), tmp );
	return;
      }
    }

    // Now create the consolidation table
    int y = 0;
    const char *s;
    for( s = lst.first(); s != 0L; s = lst.next(), y++ )
    {
      table->setText( dy + y, dx, s );

      for( int x = 1; x < w; ++x )
      {
	int count = 0;
	double dbl = 0.0;
	QString formel;
	if ( f == F_AVERAGE )
	  formel = "=(";
	else if ( f == F_SUM )
	  formel = "=";
	else
	  assert( 0 );

	it = ranges.begin();
	for( ; it != ranges.end(); ++it )
        {
	  for( unsigned int i = it->top; i <= it->bottom; i++ )
	  {    
	    KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
	    assert( t );
	    KSpreadCell *c = t->cellAt( it->left, i );
	    if ( c )
	    {
	      const char *v = c->valueString();
	      if ( v != 0L && strcmp( s, v ) == 0L )
	      {
		KSpreadCell *c2 = t->cellAt( x + it->left, i );
		count++;
		// Calculate value
		if ( c2 && c2->isValue() )
		{  
		  if ( f == F_SUM || f == F_AVERAGE )
		    dbl += c2->valueDouble();
		  else
		    assert( 0 );
		}
		// Create formular
		if ( f == F_SUM || f == F_AVERAGE )
		{    
		  if ( formel != "=" )
		    formel += "+";
		  formel += it->table.in();
		  formel += "!";
		  formel += util_cellName( x + it->left, i );
		}
		else
		  assert( 0 );
	      }
	    }
	  }
	}

	if ( f == F_AVERAGE )
	{
	  char buffer[ 100 ];
	  sprintf( buffer, "%i", count );
	  formel += ") / ";
	  formel += buffer;
	  dbl /= (double)count;
	}

	if ( m_pCopy->isChecked() )
	  formel.sprintf( "%f", dbl );	
	table->setText( dy + y, dx + x, formel );
      }
    }
  }
  else if ( desc == D_BOTH )
  {
    // Get list of all descriptions in the columns
    QStrList cols;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      for( unsigned int y = it->top + 1; y <= it->bottom ; ++y )
      {
	KSpreadCell *c = t->cellAt( it->left, y );
	if ( c )
	{
	  const char *s = c->valueString();
	  if ( s != 0L && cols.find( s ) == -1 )
	    cols.inSort( s );
	}
      }
    }

    // Get list of all descriptions in the rows
    QStrList rows;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      for( unsigned int x = it->left + 1; x <= it->right ; ++x )
      {
	KSpreadCell *c = t->cellAt( x, it->top );
	if ( c )
	{
	  const char *s = c->valueString();
	  if ( s != 0L && rows.find( s ) == -1 )
	    rows.inSort( s );
	}
      }
    }

    // Check wether the destination is part of the source ...
    QRect dest;
    dest.setCoords( dx, dy, dx + cols.count(), dy + rows.count() );
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      QRect r;
      r.setCoords( it->left, it->top, it->right, it->bottom );
      if ( t == table && r.intersects( dest ) )
      {
	QString tmp( i18n("The source tables intersect with the destination table") );
	QMessageBox::critical( 0L, i18n("Error"), tmp );
	return;
      }
    }
  
    // Fill the list with all interesting cells
    list<st_cell> lst;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      KSpreadTable *t = m_pView->doc()->map()->findTable( it->table.in() );
      assert( t );
      for( unsigned int x = it->left + 1; x <= it->right ; ++x )
      {
	KSpreadCell *c = t->cellAt( x, it->top );
	if ( c )
	{
	  const char *ydesc = c->valueString();
	  for( unsigned int y = it->top + 1; y <= it->bottom ; ++y )
	  {
	    KSpreadCell *c2 = t->cellAt( it->left, y );
	    if ( c2 )
	    {
	      const char *xdesc = c2->valueString();
	      KSpreadCell *c3 = t->cellAt( x, y );
	      if ( c3 && c3->isValue() )
	      {  
		st_cell k;
		k.xdesc = xdesc;
		k.ydesc = ydesc;
		k.cell = c3;
		k.table = it->table.in();
		k.x = x;
		k.y = y;
		lst.push_back( k );
	      }
	    }
	  }
	}
      }
    }
    
    // Draw the row description
    int i = 1;
    const char* s = rows.first();
    for( ; s != 0L; s = rows.next(), i++ )
      table->setText( dy, dx + i, s );

    // Draw the column description
    i = 1;
    s = cols.first();
    for( ; s != 0L; s = cols.next(), i++ )
      table->setText( dy + i, dx, s );
    
    // Draw the data
    int x = 1;
    const char* ydesc = rows.first();
    for( ; ydesc != 0L; ydesc = rows.next(), x++ )
    {
      int y = 1;
      const char *xdesc = cols.first();
      for( ; xdesc != 0L; xdesc = cols.next(), y++ )
      {
	int count = 0;
	double dbl = 0.0;
	QString formel;
	if ( f == F_AVERAGE )
	  formel = "=(";
	else if ( f == F_SUM )
	  formel = "=";
	else
	  assert( 0 );
	
	list<st_cell>::iterator lit = lst.begin();
	for( ; lit != lst.end(); ++lit )
	{
	  cerr << "Comparing " << xdesc << " with " << lit->xdesc << endl;
	  cerr << "      and " << ydesc << " with " << lit->ydesc << endl;
	  if ( lit->xdesc == xdesc && lit->ydesc == ydesc )
	  {
	    count++;
	    // Calculate value
	    if ( f == F_SUM || f == F_AVERAGE )
	      dbl += lit->cell->valueDouble();
	    else
	      assert( 0 );
	    // Create formular
	    if ( f == F_SUM || f == F_AVERAGE )
	    {    
	      if ( formel != "=" )
		formel += "+";
	      formel += lit->table;
	      formel += "!";
	      formel += util_cellName( lit->x, lit->y );
	    }
	    else
	      assert( 0 );
	  }
	}
	
	if ( f == F_AVERAGE )
	{
	  char buffer[ 100 ];
	  sprintf( buffer, "%i", count );
	  formel += ") / ";
	  formel += buffer;
	  dbl /= (double)count;
	}

	if ( m_pCopy->isChecked() )
	  formel.sprintf( "%f", dbl );	
	table->setText( dy + y, dx + x, formel );
      }
    }
  }
  
  accept();
}

void KSpreadConsolidate::slotClose()
{
  reject();
}

void KSpreadConsolidate::slotAdd()
{
  slotReturnPressed();
}

void KSpreadConsolidate::slotRemove()
{
  int i = m_pRefs->currentItem();
  if ( i < 0 )
    return;
  
  m_pRefs->removeItem( i );

  if ( m_pRefs->count() == 0 )
    m_pOk->setEnabled( false );
}

QStrList KSpreadConsolidate::refs()
{
  QStrList list;
  int c = m_pRefs->count();
  
  for( int i = 0; i < c; i++ )
    list.append( m_pRefs->text( i ) );
  
  return list;
}

void KSpreadConsolidate::slotSelectionChanged( KSpreadTable* _table, const QRect& _selection )
{
  if ( _selection.left() == 0 || _selection.top() == 0 ||
       _selection.right() == 0 || _selection.bottom() == 0 )
  {
    m_pRef->setText( "" );
    return;
  }
  
  QString area = util_rangeName( _table, _selection );
  m_pRef->setText( area );
  m_pRef->setSelection( 0, area.length() );
}

void KSpreadConsolidate::slotReturnPressed()
{
  QString txt = m_pRef->text();

  try
  {    
    KSpread::Range r = util_parseRange2( txt, m_pView->doc()->map() );
  }
  catch( KSpread::MalformedExpression_var &_ex )
  {
    QString tmp;
    tmp.sprintf( i18n("The range\n%s\n is malformed"), txt.data() );
    QMessageBox::critical( 0L, i18n("Error"), tmp );
    return;
  }
  
  if ( !txt.isEmpty() )
  {    
    m_pRefs->insertItem( txt );
    m_pOk->setEnabled( true );
  }
}

#include "kspread_dlg_cons.moc"
