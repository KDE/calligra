/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 2000-2001 Laurent Montel <montel@kde.org>
             (C) 1999-2002 David Faure <faure@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1999 Reginald Stadlbauer <reggie@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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

#include <assert.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QCloseEvent>
#include <Q3ListBox>
#include <kpushbutton.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kmessagebox.h>

#include "kspread_dlg_cons.h"

#include <kspread_canvas.h>
#include <kspread_doc.h>
#include <kspread_global.h>
#include <kspread_locale.h>
#include "selection.h"
#include <kspread_sheet.h>
#include <kspread_util.h>
#include <kspread_view.h>

#include <formula.h>
#include <valueconverter.h>

using namespace KSpread;

ConsolidateDialog::ConsolidateDialog( View* parent, const char* name )
  : KDialogBase( KDialogBase::Tabbed, Qt::Dialog, parent, name, false, i18n("Consolidate"), Ok|Cancel )
{
  m_pView = parent;

  QWidget* page = new QWidget( this );
  setMainWidget( page );

  QGridLayout *grid1 = new QGridLayout( page );
  grid1->setMargin(KDialog::marginHint());
  grid1->setSpacing(KDialog::spacingHint());

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( page );
  grid1->addWidget(tmpQLabel,0,0);
  tmpQLabel->setText( i18n("&Function:") );

  m_pFunction = new QComboBox( page );
  grid1->addWidget(m_pFunction,1,0);
  tmpQLabel->setBuddy(m_pFunction);

  m_pFunction->insertItem( i18n("Sum"), Sum );
  m_pFunction->insertItem( i18n("Average"), Average );
  m_pFunction->insertItem( i18n("Count"), Count );
  m_pFunction->insertItem( i18n("Max"), Max );
  m_pFunction->insertItem( i18n("Min"), Min );
  m_pFunction->insertItem( i18n("Product"), Product );
  m_pFunction->insertItem( i18n("Standard Deviation"), StdDev );
  m_pFunction->insertItem( i18n("Variance"), Var );

  tmpQLabel = new QLabel( page );
  tmpQLabel->setText( i18n("Re&ference:") );
  grid1->addWidget(tmpQLabel,2,0);

  m_pRef = new QLineEdit( page );
  grid1->addWidget(m_pRef,3,0);
  tmpQLabel->setBuddy(m_pRef);

  tmpQLabel = new QLabel( page );
  grid1->addWidget(tmpQLabel,4,0);
  tmpQLabel->setText( i18n("&Entered references:") );

  m_pRefs = new Q3ListBox( page );
  grid1->addWidget( m_pRefs,5,8,0,0);
  tmpQLabel->setBuddy(m_pRefs);

  m_pRow = new QCheckBox( i18n("&Description in row"), page );
  grid1->addWidget( m_pRow,9,0);
  m_pCol = new QCheckBox( i18n("De&scription in column"), page );
  grid1->addWidget(m_pCol,10,0);
  m_pCopy = new QCheckBox( i18n("Co&py data"), page );
  grid1->addWidget(m_pCopy,11,0);

  m_pAdd = new QPushButton( i18n("&Add"), page );
  grid1->addWidget(m_pAdd,2,1);
  m_pRemove = new QPushButton( i18n("&Remove"), page );
  grid1->addWidget(m_pRemove,3,1);


  connect( m_pAdd, SIGNAL( clicked() ), this, SLOT( slotAdd() ) );
  connect( m_pRemove, SIGNAL( clicked() ), this, SLOT( slotRemove() ) );
  connect( m_pRef, SIGNAL( returnPressed() ), this, SLOT( slotReturnPressed() ) );

  connect(m_pView->selectionInfo(), SIGNAL(changed(const Region&)),
          this, SLOT(slotSelectionChanged()));
}

ConsolidateDialog::~ConsolidateDialog()
{
    kDebug(36001)<<"Consolidate::~Consolidate()\n";
}

enum Description { D_ROW, D_COL, D_NONE, D_BOTH };

struct st_cell
{
  QString xdesc;
  QString ydesc;
  Cell* cell;
  QString sheet;
  int x;
  int y;
};

void ConsolidateDialog::slotOk()
{
  m_pView->doc()->emitBeginOperation( false );

  Map *map = m_pView->doc()->map();

  Sheet* sheet = m_pView->activeSheet();
  int dx = m_pView->selectionInfo()->selection().left();
  int dy = m_pView->selectionInfo()->selection().top();

  QString function;

  switch( m_pFunction->currentIndex() )
  {
    case Sum:      function = "SUM"; break;
    case Average:  function = "AVERAGE"; break;
    case Count:    function = "COUNT"; break;
    case Max:      function = "MAX"; break;
    case Min:      function = "MIN"; break;
    case Product:  function = "PRODUCT"; break;
    case StdDev:   function = "STDDEV"; break;
    case Var:      function = "VARIANCE"; break;
    default: break; // bad bad !
  }

  QStringList r = refs();
  QLinkedList<Range> ranges;
  QStringList::Iterator s = r.begin();
  for( ; s != r.end(); ++s )
  {
    Range r( *s, map );
    // TODO: Check for valid
    Q_ASSERT( r.isValid() );

    if ( r.sheet() == 0 )
    {
      r.setSheet(sheet);
      r.setSheetName(sheet->sheetName());
    }
    ranges.append( r  );
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

  // Check whether all ranges have same size
  Q_ASSERT( ranges.count() > 0 );
  QLinkedList<Range>::Iterator it = ranges.begin();
  int w = (*it).range().right() - (*it).range().left() + 1;
  int h = (*it).range().bottom() - (*it).range().top() + 1;
  if ( w <= ( ( desc == D_BOTH || desc == D_COL ) ? 1 : 0 ) ||
       h <= ( ( desc == D_BOTH || desc == D_ROW ) ? 1 : 0 ) )
  {
    m_pView->slotUpdateView( m_pView->activeSheet() );
    KMessageBox::error( this, i18n( "The range\n%1\nis too small" ).arg( *( r.begin() ) ));
    return;
  }

  if( (*it).range().bottom()==KS_rowMax || (*it).range().right()== KS_colMax )
  {
    m_pView->slotUpdateView( m_pView->activeSheet() );
    KMessageBox::error( this, i18n( "The range\n%1\nis too large" ).arg( *( r.begin() ) ));
    return;
  }

  ++it;
  int i = 1;
  for( ; it != ranges.end(); ++it, i++ )
  {
    QRect currentRange=(*it).range();

    int w2 = currentRange.right() - currentRange.left() + 1;
    int h2 = currentRange.bottom() - currentRange.top() + 1;

    if(currentRange.bottom()==KS_rowMax || currentRange.right()== KS_colMax)
    {
      m_pView->slotUpdateView( m_pView->activeSheet() );
      KMessageBox::error( this, i18n( "The range\n%1\nis too large" ).arg( r[i]));
      return;
    }
    if ( ( desc == D_NONE && ( w != w2 || h != h2 ) ) ||
	 ( desc == D_ROW && h != h2 ) ||
	 ( desc == D_COL && w != w2 ) )
    {
      m_pView->slotUpdateView( m_pView->activeSheet() );
      QString tmp = i18n( "The ranges\n%1\nand\n%2\nhave different size").arg( *( r.begin() ) ).arg( r[i] );
      KMessageBox::error( this, tmp);
      return;
    }
  }

  // Create the consolidation sheet
  if ( desc == D_NONE )
  {
    // Check whether the destination is part of the source ...
    QRect dest;
    dest.setCoords( dx, dy, dx + w - 1, dy + h - 1 );
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      Q_ASSERT( t );
      QRect r;

      QRect currentRange=(*it).range();

      r.setCoords( currentRange.left(), currentRange.top(), currentRange.right(), currentRange.bottom() );
      if ( t == sheet && r.intersects( dest ) )
      {
        m_pView->slotUpdateView( m_pView->activeSheet() );
	QString tmp( i18n("The source tables intersect with the destination table") );
	KMessageBox::error( this, tmp);
	return;
      }
    }

    for( int x = 0; x < w; x++ )
    {
      for( int y = 0; y < h; y++ )
      {
        bool novalue=true;
        QString formula = "=" + function + "(";
	it = ranges.begin();
	for( ; it != ranges.end(); ++it )
        {
	  Sheet *t = (*it).sheet();
	  assert( t );
	  Cell *c = t->cellAt( x + (*it).range().left(), y + (*it).range().top() );
          if(!c->isDefault())
                novalue=false;
	  if ( it != ranges.begin() )
	    formula += ";";
	  formula += (*it).sheetName() + "!";
	  formula += c->name();
	}
	formula += ")";

        if(!novalue)
	  sheet->setText( dy + y, dx + x,
            m_pCopy->isChecked() ? evaluate( formula, sheet ) : formula );
      }
    }
  }
  else if ( desc == D_ROW )
  {
    // Get list of all descriptions in the rows
    QStringList lst;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      assert( t );
//      kDebug(36001) << "FROM " << (*it).range.left() << " to " << (*it).range.right() << endl;
      for( int x = (*it).range().left(); x <= (*it).range().right() ; ++x )
      {
	Cell *c = t->cellAt( x, (*it).range().top() );
	if ( c )
	{
	  QString s = c->value().asString();
	  if ( !lst.contains( s ) )
	    lst.append( s );
	}
      }
    }
    lst.sort();

    // Check whether the destination is part of the source ...
    QRect dest;
    dest.setCoords( dx, dy, dx + lst.count() - 1, dy + h - 1 );
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      assert( t );
      QRect r;
      QRect currentRange=(*it).range();
      r.setCoords( currentRange.left(), currentRange.top(), currentRange.right(), currentRange.bottom() );
      if ( t == sheet && r.intersects( dest ) )
      {
        m_pView->slotUpdateView( m_pView->activeSheet() );
	QString tmp( i18n("The source tables intersect with the destination table") );
	KMessageBox::error( this, tmp);
	return;
      }
    }

    // Now create the consolidation sheet
    int x = 0;
    QStringList::Iterator s = lst.begin();
    for( ; s != lst.end(); ++s, ++x )
    {
      sheet->setText( dy, dx + x, *s );

      for( int y = 1; y < h; ++y )
      {
	int count = 0;
        QString formula = "=" + function + "(";
	it = ranges.begin();
	for( ; it != ranges.end(); ++it )
        {
	  for( int i = (*it).range().left(); i <= (*it).range().right(); ++i )
	  {
	    Sheet *t = (*it).sheet();
	    assert( t );
	    Cell *c = t->cellAt( i, (*it).range().top() );
	    if ( c )
	    {
	      if ( c->value().asString() == *s )
	      {
//		Cell *c2 = t->cellAt( i, y + (*it).range.top() );
		count++;
		if ( it != ranges.begin() )
		  formula += ";";
		formula += (*it).sheetName() + "!";
		formula += Cell::name( i, y + (*it).range().top() );
	      }
	    }
	  }
	}
	formula += ")";

	sheet->setText( dy + y, dx + x,
          m_pCopy->isChecked() ? evaluate( formula, sheet ) : formula );
      }
    }
  }
  else if ( desc == D_COL )
  {
    // Get list of all descriptions in the columns
    QStringList lst;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      assert( t );
      for( int y = (*it).range().top(); y <= (*it).range().bottom() ; ++y )
      {
	Cell *c = t->cellAt( (*it).range().left(), y );
	if ( c )
	{
	  QString s = c->value().asString();
	  if ( !s.isEmpty() && lst.indexOf( s ) == -1 )
	    lst.append( s );
	}
      }
    }
    lst.sort();

    // Check whether the destination is part of the source ...
    QRect dest;
    dest.setCoords( dx, dy, dx + w - 1, dy + lst.count() - 1 );
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      assert( t );
      QRect r;
      QRect currentRange=(*it).range();
      r.setCoords( currentRange.left(), currentRange.top(), currentRange.right(), currentRange.bottom() );
      if ( t == sheet && r.intersects( dest ) )
      {
        m_pView->slotUpdateView( m_pView->activeSheet() );
	QString tmp( i18n("The source tables intersect with the destination table") );
	KMessageBox::error( this, tmp);
	return;
      }
    }

    // Now create the consolidation sheet
    int y = 0;
    QStringList::Iterator s = lst.begin();
    for( ; s != lst.end(); ++s, ++y )
    {
      sheet->setText( dy + y, dx, *s );

      for( int x = 1; x < w; ++x )
      {
	int count = 0;
        QString formula = "=" + function + "(";
	it = ranges.begin();
	for( ; it != ranges.end(); ++it )
        {
	  for( int i = (*it).range().top(); i <= (*it).range().bottom(); i++ )
	  {
	    Sheet *t = (*it).sheet();
	    assert( t );
	    Cell *c = t->cellAt( (*it).range().left(), i );
	    if ( c )
	    {
	      QString v = c->value().asString();
	      if ( !v.isEmpty() && *s == v )
	      {
//		Cell *c2 = t->cellAt( x + (*it).range.left(), i );
		count++;
		if ( it != ranges.begin() ) formula += ";";
		formula += (*it).sheetName() + "!";
		formula += Cell::name( i, y + (*it).range().top() );
	      }
	    }
	  }
	}

	formula += ")";

	sheet->setText( dy + y, dx + x,
          m_pCopy->isChecked() ? evaluate( formula, sheet ) : formula );
      }
    }
  }
  else if ( desc == D_BOTH )
  {
    // Get list of all descriptions in the columns
    QStringList cols;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      assert( t );
      for( int y = (*it).range().top() + 1; y <= (*it).range().bottom() ; ++y )
      {
	Cell *c = t->cellAt( (*it).range().left(), y );
	if ( c )
	{
	  QString s = c->value().asString();
	  if ( !s.isEmpty() && cols.indexOf( s ) == -1 )
	    cols.append( s );
	}
      }
    }
    cols.sort();

    // Get list of all descriptions in the rows
    QStringList rows;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      assert( t );
      for( int x = (*it).range().left() + 1; x <= (*it).range().right() ; ++x )
      {
	Cell *c = t->cellAt( x, (*it).range().top() );
	if ( c )
	{
	  QString s = c->value().asString();
	  if ( !s.isEmpty() && rows.indexOf( s ) == -1 )
	    rows.append( s );
	}
      }
    }
    rows.sort();

    // Check whether the destination is part of the source ...
    QRect dest;
    dest.setCoords( dx, dy, dx + cols.count(), dy + rows.count() );
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      assert( t );
      QRect r;
      QRect currentRange=(*it).range();
      r.setCoords( currentRange.left(), currentRange.top(), currentRange.right(), currentRange.bottom() );
      if ( t == sheet && r.intersects( dest ) )
      {
        m_pView->slotUpdateView( m_pView->activeSheet() );
	QString tmp( i18n("The source tables intersect with the destination table") );
	KMessageBox::error( this, tmp);
	return;
      }
    }

    // Fill the list with all interesting cells
    QLinkedList<st_cell> lst;
    it = ranges.begin();
    for( ; it != ranges.end(); ++it )
    {
      Sheet *t = (*it).sheet();
      assert( t );
      for( int x = (*it).range().left() + 1; x <= (*it).range().right() ; ++x )
      {
	Cell *c = t->cellAt( x, (*it).range().top() );
	if ( c )
	{
	  QString ydesc = c->value().asString();
	  for( int y = (*it).range().top() + 1; y <= (*it).range().bottom() ; ++y )
	  {
	    Cell *c2 = t->cellAt( (*it).range().left(), y );
	    if ( c2 )
	    {
	      QString xdesc = c2->value().asString();
	      Cell *c3 = t->cellAt( x, y );
	      if ( c3 && c3->value().isNumber() )
	      {
		st_cell k;
		k.xdesc = xdesc;
		k.ydesc = ydesc;
		k.cell = c3;
		k.sheet = (*it).sheetName();
		k.x = x;
		k.y = y;
		lst.append( k );
	      }
	    }
	  }
	}
      }
    }

    // Draw the row description
    int i = 1;
    QStringList::Iterator s = rows.begin();
    for( ; s != rows.end(); ++s, ++i )
      sheet->setText( dy, dx + i, *s );

    // Draw the column description
    i = 1;
    s = cols.begin();
    for( ; s != cols.end(); ++s, ++i )
      sheet->setText( dy + i, dx, *s );

    // Draw the data
    int x = 1;
    QStringList::Iterator ydesc = rows.begin();
    for( ; ydesc != rows.end(); ++ydesc, x++ )
    {
      int y = 1;
      QStringList::Iterator xdesc = cols.begin();
      for( ; xdesc != cols.end(); ++xdesc, y++ )
      {
	int count = 0;
        QString formula = "=" + function + "(";
	QLinkedList<st_cell>::Iterator lit = lst.begin();
	for( ; lit != lst.end(); ++lit )
	{
	  if ( (*lit).xdesc == *xdesc && (*lit).ydesc == *ydesc )
	  {
	    count++;
  	    if ( it != ranges.begin() ) formula += ";";
	    formula += (*it).sheetName() + "!";
	    formula += Cell::name( i, y + (*it).range().top() );
	  }
	}
	formula += ")";

	sheet->setText( dy + y, dx + x,
          m_pCopy->isChecked() ? evaluate( formula, sheet ) : formula );
      }
    }
  }
  m_pView->updateEditWidget();
  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
  delete this;
}

void ConsolidateDialog::slotCancel()
{
  reject();
  delete this;
}

void ConsolidateDialog::slotAdd()
{
  slotReturnPressed();
}

void ConsolidateDialog::slotRemove()
{
  int i = m_pRefs->currentItem();
  if ( i < 0 )
    return;

  m_pRefs->removeItem( i );

  if ( m_pRefs->count() == 0 )
    actionButton( Ok )->setEnabled( false );
}

QStringList ConsolidateDialog::refs()
{
  QStringList list;
  int c = m_pRefs->count();

  for( int i = 0; i < c; i++ )
    list.append( m_pRefs->text( i ) );

  return list;
}

void ConsolidateDialog::slotSelectionChanged()
{
  if (!m_pView->selectionInfo()->isValid())
  {
    m_pRef->setText( "" );
    return;
  }

  QString area = m_pView->selectionInfo()->name();
  m_pRef->setText( area );
  m_pRef->setSelection( 0, area.length() );
}

void ConsolidateDialog::slotReturnPressed()
{
  QString txt = m_pRef->text();

  Range r( txt, m_pView->doc()->map() );
  if ( !r.isValid() )
  {
    KMessageBox::error( this, i18n("The range\n%1\n is malformed").arg( txt ));
    return;
  }

  if ( !txt.isEmpty() )
  {
    m_pRefs->insertItem( txt );
    actionButton( Ok )->setEnabled( true );
  }
}

void ConsolidateDialog::closeEvent ( QCloseEvent * )
{
    delete this;
}

QString ConsolidateDialog::evaluate( const QString& formula, Sheet* sheet )
{
  QString result = "###";
  Formula *f = new Formula (sheet);
  f->setExpression (formula);
  if (!f->isValid()) {
    delete f;
    return result;
  }

  Value res = f->eval ();
  delete f;
  result = sheet->doc()->converter()->asString (res).asString ();
  return result;
}

#include "kspread_dlg_cons.moc"
