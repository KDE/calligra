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
#include "kdiagramm_doc.h"
#include "kdiagramm_shell.h"

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>
#include <komlMime.h>

#include <fstream>
#include <string>

#include <unistd.h>
#include <kapp.h>
#include <qdatetm.h>

#include "sheetdlg.h"

#define MM_TO_POINT 2.83465
#define POINT_TO_MM 0.3527772388

/*****************************************************************************
 *
 * KDiagrammDoc
 *
 *****************************************************************************/

KDiagrammDoc::KDiagrammDoc()
{
  // Demo values
  m_table.xDesc.append( "Torben" );
  m_table.xDesc.append( "Claudia" );
  m_table.xDesc.append( "Lars" );
  m_table.xDesc.append( "Matthias" );

  m_table.yDesc.append( "Januar" );
  m_table.yDesc.append( "Februar" );

  line_t l;
  l.push_back( 6.0 );
  l.push_back( 7.0 );
  l.push_back( 3.0 );
  l.push_back( 1.5 );
  m_table.data.push_back( l );

  l.clear();
  l.push_back( 5.0 );
  l.push_back( 3.0 );
  l.push_back( 2.0 );
  l.push_back( 2.5 );
  m_table.data.push_back( l );

  ADD_INTERFACE( "IDL:Chart/SimpleChart:1.0" );
  ADD_INTERFACE("IDL:KOffice/Print:1.0");

  m_bEmpty = true;
  m_bModified = FALSE;

  m_type = KoDiagramm::DT_SAEULEN;

  m_leftBorder = 20.0;
  m_rightBorder = 20.0;
  m_topBorder = 20.0;
  m_bottomBorder = 20.0;
  m_paperFormat = PG_DIN_A4;
  m_paperWidth = PG_A4_WIDTH;
  m_paperHeight = PG_A4_HEIGHT;
  calcPaperSize();
  m_orientation = PG_PORTRAIT;

  m_lstViews.setAutoDelete( false );
}

CORBA::Boolean KDiagrammDoc::initDoc()
{
  return true;
}

void KDiagrammDoc::cleanUp()
{
  cerr << "CLeanUp KDiagrammDoc" << endl;

  if ( m_bIsClean )
    return;

  assert( m_lstViews.count() == 0 );

  KoDocument::cleanUp();
}

KOffice::MainWindow_ptr KDiagrammDoc::createMainWindow()
{
  KDiagrammShell* shell = new KDiagrammShell;
  shell->show();
  shell->setDocument( this );

  return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

void KDiagrammDoc::removeView( KDiagrammView* _view )
{
  m_lstViews.removeRef( _view );
}

KDiagrammView* KDiagrammDoc::createDiagrammView(QWidget *_parent)
{
  KDiagrammView *p = new KDiagrammView( _parent, 0L, this );
  //p->QWidget::show();
  m_lstViews.append( p );

  return p;
}

OpenParts::View_ptr KDiagrammDoc::createView()
{
  return OpenParts::View::_duplicate( createDiagrammView() );
}

void KDiagrammDoc::viewList( OpenParts::Document::ViewList*& _list )
{
  (*_list).length( m_lstViews.count() );

  int i = 0;
  QListIterator<KDiagrammView> it( m_lstViews );
  for( ; it.current(); ++it )
  {
    (*_list)[i++] = OpenParts::View::_duplicate( it.current() );
  }
}

int KDiagrammDoc::viewCount()
{
  return m_lstViews.count();
}

bool KDiagrammDoc::save( ostream& out, const char* /* format */ )
{
  out << "<?xml version=\"1.0\"?>" << endl;
  out << otag << "<DOC author=\"" << "Torben Weis" << "\" email=\"" << "weis@kde.org" << "\" editor=\"" << "kdiagramm"
      << "\" mime=\"" << "application/x-kdiagramm" << "\" >" << endl;

  out << etag << "</DOC>" << endl;

  setModified( FALSE );

  return true;
}

bool KDiagrammDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr /*_store*/ )
{
  cerr << "------------------------ LOADING --------------------" << endl;

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  // DOC
  if ( !parser.open( "DOC", tag ) )
  {
    cerr << "Missing DOC" << endl;
    return false;
  }

  KOMLParser::parseTag( tag.c_str(), name, lst );
  vector<KOMLAttrib>::const_iterator it = lst.begin();
  for( ; it != lst.end(); it++ )
  {
    if ( (*it).m_strName == "mime" )
    {
      if ( (*it).m_strValue != "application/x-kdiagramm" )
      {
	cerr << "Unknown mime type " << (*it).m_strValue << endl;
	return false;
      }
    }
  }

  parser.close( tag );

  cerr << "------------------------ LOADING DONE --------------------" << endl;

  return true;
}

bool KDiagrammDoc::completeLoading( KOStore::Store_ptr /* _store */ )
{
  cerr << "------------------------ COMPLETION DONE --------------------" << endl;

  m_bModified = false;

  return true;
}

void KDiagrammDoc::showWizard()
{
  // TODO
}

void KDiagrammDoc::fill( const Chart::Range& range, const Chart::Matrix& matrix )
{
  cout << "Got w=" << matrix.columns << " h=" << matrix.rows << endl;

  cerr << "1" << endl;

  m_table.yDesc.clear();
  int l = matrix.rowDescription.length();
  for ( int i = 0; i < l; i++ )
  {
    m_table.yDesc.append( matrix.rowDescription[i].in() );
  }

  m_table.xDesc.clear();
  l = matrix.columnDescription.length();
  for ( int i = 0; i < l; i++ )
  {
    m_table.xDesc.append( matrix.columnDescription[i].in() );
  }

  cerr << "2" << endl;

  m_table.data.clear();
  for ( int y = 0; y < matrix.rows; y++ )
  {
    line_t l;
    for ( int x = 0; x < matrix.columns; x++ )
      l.push_back( matrix.matrix[ y * matrix.columns + x ] );
    m_table.data.push_back( l );
  }

  cerr << "4" << endl;

  m_range = range;

  cerr << "6" << endl;

  emit sig_updateView();
}

void KDiagrammDoc::editData()
{
  // create dialog
  QDialog *dlg = new QDialog(0,"SheetDlg",true);
  SheetDlg *widget = new SheetDlg( dlg, "SheetWidget" );
  widget->setGeometry(0,0,520,400);
  widget->show();
  dlg->resize(520,400);
  dlg->setMaximumSize( dlg->size() );
  dlg->setMinimumSize( dlg->size() );

  // fill cells
  int col = 0,row = 0;
  for( data_t::iterator it1 = m_table.data.begin(); it1 != m_table.data.end(); ++it1 )
  {
    col = 0;
    for( line_t::iterator it2 = it1->begin(); it2 != it1->end(); ++it2 )
    {
      widget->fillCell( row, col++, *it2 );
    }
    row++;
  }

  const char *s;
  col = 0;
  for( s = m_table.xDesc.first(); s != 0L; s = m_table.xDesc.next() )
    widget->fillX( col++, s );

  row = 0;
  for( s = m_table.yDesc.first(); s != 0L; s = m_table.yDesc.next() )
    widget->fillY( row++, s );

  // OK pressed
  if ( dlg->exec() == QDialog::Accepted )
  {
    m_table.xDesc.clear();
    for ( col = 0; col < widget->cols(); col++ )
      m_table.xDesc.append( widget->getX( col ) );

    m_table.yDesc.clear();
    for ( row = 0; row < widget->rows(); row++ )
      m_table.yDesc.append( widget->getY( row ) );

    m_table.data.clear();
    for ( row = 0; row < widget->rows(); row++ )
    {
      line_t line;
      for ( col = 0; col < widget->cols(); col++ )
	line.push_back( widget->getCell( row, col ) );
      m_table.data.push_back( line );
    }

    emit sig_updateView();
  }

  // delete dialog
  delete widget; widget = 0;
  delete dlg; dlg = 0;
}


void KDiagrammDoc::configChart()
{
#ifndef OLDCODE
    // PENDING(Torben) How to transfer configuration from diag._params
    // to document?
    KoDiagramm diag;
    diag.config( 0 );
#endif
}

void KDiagrammDoc::print( QPaintDevice* _dev )
{
  QPainter painter;
  painter.begin( _dev );

  // HACK
  int pagenr = 1;
  QString m_strName = "";

  // print head line
  QFont font( "Times", 10 );
  painter.setFont( font );
  QFontMetrics fm = painter.fontMetrics();
  int w = fm.width( headLeft( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() ),
		      (int)( MM_TO_POINT * 10.0 ), headLeft( pagenr, m_strName ) );
  w = fm.width( headMid( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() +
			     ( MM_TO_POINT * printableWidth() - (float)w ) / 2.0 ),
		      (int)( MM_TO_POINT * 10.0 ), headMid( pagenr, m_strName ) );
  w = fm.width( headRight( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() +
			     MM_TO_POINT * printableWidth() - (float)w ),
		      (int)( MM_TO_POINT * 10.0 ), headRight( pagenr, m_strName ) );

  // print foot line
  w = fm.width( footLeft( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() ),
		      (int)( MM_TO_POINT * ( paperHeight() - 10.0 ) ),
		      footLeft( pagenr, m_strName ) );
  w = fm.width( footMid( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() +
			     ( MM_TO_POINT * printableWidth() - (float)w ) / 2.0 ),
		      (int)( MM_TO_POINT * ( paperHeight() - 10.0 ) ),
		      footMid( pagenr, m_strName ) );
  w = fm.width( footRight( pagenr, m_strName ) );
  if ( w > 0 )
    painter.drawText( (int)( MM_TO_POINT * leftBorder() +
			     MM_TO_POINT * printableWidth() - (float)w ),
		      (int)( MM_TO_POINT * ( paperHeight() - 10.0 ) ),
		      footRight( pagenr, m_strName ) );

  painter.translate( MM_TO_POINT * m_leftBorder, MM_TO_POINT * m_topBorder );

  KoDiagramm diag;
  diag.setData( data(), "", KoDiagramm::DAT_NUMMER, m_type );
  diag.paint( painter, MM_TO_POINT * printableWidth(), MM_TO_POINT * printableHeight() );

  painter.end();
}

void KDiagrammDoc::draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
			 CORBA::Float _scale )
{
  cerr << "DRAWING w=" << _width << " h=" << _height << endl;

  QPainter painter;
  painter.begin( _dev );

  if ( _scale != 1.0 )
    painter.scale( _scale, _scale );

  KoDiagramm diag;
  diag.setData( data(), "", KoDiagramm::DAT_NUMMER, m_type );
  diag.paint( painter, _width, _height );

  painter.end();
}

void KDiagrammDoc::paperLayoutDlg()
{
  KoPageLayout pl;
  pl.format = paperFormat();
  pl.orientation = orientation();
  pl.unit = PG_MM;
  pl.width = m_paperWidth;
  pl.height = m_paperHeight;
  pl.left = leftBorder();
  pl.right = rightBorder();
  pl.top = topBorder();
  pl.bottom = bottomBorder();

  KoHeadFoot hf;
  hf.headLeft = headLeft();
  hf.headRight = headRight();
  hf.headMid = headMid();
  hf.footLeft = footLeft();
  hf.footRight = footRight();
  hf.footMid = footMid();

  if ( !KoPageLayoutDia::pageLayout( pl, hf, FORMAT_AND_BORDERS | HEADER_AND_FOOTER ) )
    return;

  if ( pl.format == PG_CUSTOM )
  {
    m_paperWidth = pl.width;
    m_paperHeight = pl.height;
  }

  setPaperLayout( pl.left, pl.top, pl.right, pl.bottom, pl.format, pl.orientation );

  setHeadFootLine( hf.headLeft, hf.headMid, hf.headRight, hf.footLeft, hf.footMid, hf.footRight );

  emit sig_updateView();
}

void KDiagrammDoc::setHeadFootLine( const char *_headl, const char *_headm, const char *_headr,
				    const char *_footl, const char *_footm, const char *_footr )
{
  m_headLeft = _headl;
  m_headRight = _headr;
  m_headMid = _headm;
  m_footLeft = _footl;
  m_footRight = _footr;
  m_footMid = _footm;

  m_bModified = TRUE;
}

void KDiagrammDoc::setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
				   KoFormat _paper, KoOrientation _orientation )
{
  m_leftBorder = _leftBorder;
  m_rightBorder = _rightBorder;
  m_topBorder = _topBorder;
  m_bottomBorder = _bottomBorder;
  m_orientation = _orientation;
  m_paperFormat = _paper;

  calcPaperSize();

  m_bModified = TRUE;
}

QString KDiagrammDoc::completeHeading( const char *_data, int ,
				       const char */*_table*/ )
{
  /* QString page;
    page.sprintf( "%i", _page );
    QString f = m_strFileURL.data();
    if ( f.isNull() )
	f = "";
    QString n = "";
    if ( f != "" )
    {
	KURL u( f.data() );
	n = u.filename();
	} */
    QString t = QTime::currentTime().toString().copy();
    QString d = QDate::currentDate().toString().copy();

    QString tmp = _data;
    int pos = 0;
    /* while ( ( pos = tmp.find( "<file>", pos ) ) != -1 )
       tmp.replace( pos, 6, f.data() ); */
    pos = 0;
    /* while ( ( pos = tmp.find( "<name>", pos ) ) != -1 )
       tmp.replace( pos, 6, n.data() ); */
    pos = 0;
    while ( ( pos = tmp.find( "<time>", pos ) ) != -1 )
	tmp.replace( pos, 6, t.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<date>", pos ) ) != -1 )
	tmp.replace( pos, 6, d.data() );
    pos = 0;
    while ( ( pos = tmp.find( "<author>", pos ) ) != -1 )
	tmp.replace( pos, 8, "??" );
    pos = 0;
    while ( ( pos = tmp.find( "<email>", pos ) ) != -1 )
	tmp.replace( pos, 7, "??" );

    return QString( tmp.data() );
}

void KDiagrammDoc::calcPaperSize()
{
    switch( m_paperFormat )
    {
    case PG_DIN_A5:
        m_paperWidth = PG_A5_WIDTH;
	m_paperHeight = PG_A5_HEIGHT;
	break;
    case PG_DIN_A4:
	m_paperWidth = PG_A4_WIDTH;
	m_paperHeight = PG_A4_HEIGHT;
	break;
    case PG_DIN_A3:
	m_paperWidth = PG_A3_WIDTH;
	m_paperHeight = PG_A3_HEIGHT;
	break;
    case PG_DIN_B5:
	m_paperWidth = PG_B5_WIDTH;
	m_paperHeight = PG_B5_HEIGHT;
	break;
    case PG_US_EXECUTIVE:
	m_paperWidth = PG_US_EXECUTIVE_WIDTH;
	m_paperHeight = PG_US_EXECUTIVE_HEIGHT;
	break;
    case PG_US_LETTER:
	m_paperWidth = PG_US_LETTER_WIDTH;
	m_paperHeight = PG_US_LETTER_HEIGHT;
	break;
    case PG_US_LEGAL:
	m_paperWidth = PG_US_LEGAL_WIDTH;
	m_paperHeight = PG_US_LEGAL_HEIGHT;
	break;
    case PG_SCREEN:
        m_paperWidth = PG_SCREEN_WIDTH;
        m_paperHeight = PG_SCREEN_HEIGHT;
    case PG_CUSTOM:
        return;
    }
}

void KDiagrammDoc::setDiaType( KoDiagramm::dia_type _type )
{
  m_type = _type;
  emit sig_updateView();
}

KDiagrammDoc::~KDiagrammDoc()
{
}

#include "kdiagramm_doc.moc"
