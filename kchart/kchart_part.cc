/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include <qdom.h>
#include <qtextstream.h>
#include <qbuffer.h>
#include "kchart_part.h"
#include "kchart_view.h"
#include "kchart_shell.h"
#include "kchart_factory.h"
#include <kstddirs.h>

#include <engine.h>
#include <kchartparams.h>
#include <kglobal.h>

// Some hardcoded data for a chart

/* ----- set some data ----- */
// float   a[6]  = { 0.5, 0.09, 0.6, 0.85, 0.0, 0.90 },
// b[6]  = { 1.9, 1.3,  0.6, 0.75, 0.1, -2.0 };
/* ----- X labels ----- */
// char    *t[6] = { "Chicago", "New York", "L.A.", "Atlanta", "Paris, MD\n(USA) ", "London" };
/* ----- data set colors (RGB) ----- */
// QColor   sc[2]    = { QColor( 255, 128, 128 ), QColor( 128, 128, 255 ) };


#include <qpainter.h>

KChartPart::KChartPart( KoDocument* parent, const char* name )
  : KoDocument( parent, name ),
    _params( 0 )
{
}

KChartPart::~KChartPart()
{
}


bool KChartPart::initDoc()
{
  // Initialize the parameter set for this chart document
  // PENDING(kalle,torben) Where to delete this?
  _params = new KChartParameters;
  initRandomData();
  // PENDING(lotzi) This is where to start the wizard and fill the
  // params struct with the data the users enters there.
  
  return TRUE;
}

void KChartPart::initRandomData() {
     // fill cells
    int col,row;

    // initialize some data, if there is none
    if (currentData.rows() == 0) {
      cerr << "Initialize with some data!!!\n";
      currentData.expand(4,4);
      for (row = 0;row < 4;row++)
	for (col = 0;col < 4;col++) {
	  KChartValue t; 
	  t.exists= true;
	  t.value.setValue((double)row+col);
	  // cerr << "Set cell for " << row << "," << col << "\n";
	  currentData.setCell(row,col,t);
	}
    }
   
}



QCString KChartPart::mimeType() const
{
    return "application/x-kchart";
}

View* KChartPart::createView( QWidget* parent, const char* name )
{
    KChartView* view = new KChartView( this, parent, name );
    addView( view );

    return view;
}

Shell* KChartPart::createShell()
{
    Shell* shell = new KChartShell;
    shell->setRootPart( this );
    shell->show();

    return shell;
}


void KChartPart::paintContent( QPainter& painter, const QRect& rect, bool transparent )
{
	// if params is 0, initDoc() has not been called
	ASSERT( _params != 0 );
	
    // ####### handle transparency
    if( !transparent )
        painter.eraseRect( rect );

    // debug( "KChartPart::paintContent called, rows = %d, cols = %d", currentData.rows(), currentData.cols() );

    // Need to draw only the document rectangle described in the parameter rect.
    out_graph( rect.width(),
               rect.height(),      /* short       width, height */
               &painter,        // Paint into this painter
			   _params,			// the parameters of the chart,
								// including the type
               currentData );
}

QString KChartPart::configFile() const
{
  //    return readConfigFile( "kchart.rc" );
    return readConfigFile( locate( "data", "kchart/kchart.rc", 
			  KChartFactory::global() ) );
}


void KChartPart::setPart( const KChartData& data )
{
  currentData = data;

  emit docChanged();
}


void KChartPart::loadConfig( KConfig *conf ) {
    _params->loadConfig(conf);
}


void KChartPart::saveConfig( KConfig *conf ) {
    _params->saveConfig(conf);
}

bool KChartPart::save( ostream& out, const char *_format ) {
  cerr << "save kchart called!\n";
  QDomDocument doc( "chart" );
  doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
  QDomElement chart = doc.createElement( "chart" );
  chart.setAttribute( "author", "Kalle Dalheimer, Laszlo Boloni" );
  chart.setAttribute( "email", "kalle@dalheimer.org, boloni@cs.purdue.edu" );
  chart.setAttribute( "editor", "KChart" );
  chart.setAttribute( "mime", "application/x-kchart" );
  doc.appendChild( chart );
  // now save the data
  QDomElement data = doc.createElement("data");
  data.setAttribute("rows", currentData.rows());
  data.setAttribute("cols", currentData.cols());
  for (int row = 0;row < currentData.rows();row++) {
      for (int col = 0;col < currentData.cols();col++) {
	// later we need a value 
	cerr << "Row " << row << "\n";
	KChartValue t = currentData.cell(row, col);
	QDomElement e = doc.createElement("cell");
	e.setAttribute("value", t.value.doubleValue());
	  /*
	  if ( e.isNull() )
	      return e;
	  */      	  
	data.appendChild(e);
      }
  }
  // now save the parameters
  chart.appendChild(data);
  cerr << "Ok, till here!!!";
  QBuffer buffer;
  buffer.open( IO_WriteOnly );
  QTextStream str( &buffer );
  str << doc;
  buffer.close();

  out.write( buffer.buffer().data(), buffer.buffer().size() );

  //  setModified( false );
  return true;
};

bool KChartPart::loadChildren( KoStore* _store ) {
  cerr << "kchart loadChildren called\n";
  return true;
};

bool KChartPart::loadXML( const QDomDocument& doc, KoStore* store ) {
  cerr << "kchart loadXML called\n";
  // <spreadsheet>
  //  m_bLoading = true;
  if ( doc.doctype().name() != "chart" )
  {
    //m_bLoading = false;
    return false;
  }

  cerr << "Ok, it is a chart\n";
  
  QDomElement chart = doc.documentElement();
  if ( chart.attribute( "mime" ) != "application/x-kchart" )
    return false;

  cerr << "Mimetype ok\n";

  QDomElement data = chart.namedItem("data").toElement();
  bool ok;
  int cols = data.attribute("cols").toInt(&ok);
  cerr << "cols readed as:" << cols << "\n";
  if (!ok)  { return false; }
  int rows = data.attribute("rows").toInt(&ok);
  if (!ok)  { return false; }
  cerr << rows << " x " << cols << "\n";
  currentData.expand(rows, cols);

  QDomNode n = data.firstChild();
  while( !n.isNull() )
  {
    QDomElement e = n.toElement();
    if ( !e.isNull() && e.tagName() == "cell" )
    {
      // add the cell to the corresponding place...
      double val = e.attribute("value").toDouble(&ok);
      if (!ok)  {  return false; }
      cerr << val << "\n";      
    }
    n = n.nextSibling();
  }
  //  cerr << 
  return true;
};

bool KChartPart::load( istream& in, KoStore* store ) {
  cerr << "kchart load colled\n";
    _params = new KChartParameters;
    QBuffer buffer;
    buffer.open( IO_WriteOnly );

    char buf[ 4096 ];
    int anz;
    do
    {
	in.read( buf, 4096 );
	anz = in.gcount();
	buffer.writeBlock( buf, anz );
    } while( anz > 0 );

    buffer.close();

    buffer.open( IO_ReadOnly );
    QDomDocument doc( &buffer );

    bool b = loadXML( doc, store );
    buffer.close();
    // init the parameters 

    return b;
};


#include "kchart_part.moc"

/**
 * $Log$
 * Revision 1.6  1999/11/19 05:04:35  boloni
 * more work on saving
 *
 * Revision 1.5  1999/11/17 02:49:53  boloni
 * -started implementing save and load.
 *
 * Revision 1.4  1999/11/16 03:00:56  boloni
 * -enabling grid and label drawing. Some more small reorganizations
 * -one more page in the wizard.
 *
 * Revision 1.3  1999/11/14 18:02:06  boloni
 * auto-initialization for standalone startup
 * separate class for the kchart data editor
 *
 * Revision 1.2  1999/10/25 04:52:52  boloni
 * -ok, the gray rectangle which Reggie got was due to the fact that the
 * rc files were hardcoded so it worked only from the kchart dir.
 * -changed to the "locate" style and now it has menus if started from other dirs, too.
 * -and btw the kchart.rc was not installed anyhow
 *
 * Revision 1.1  1999/10/20 10:07:32  kulow
 * sync with canossa
 *
 * Revision 1.15  1999/10/18 08:15:10  kalle
 * Pulled the colors (and some other stuff) into KChartParameter
 *
 * Revision 1.14  1999/10/16 14:51:08  kalle
 * Accessor for params, pulled the fonts into KChartParameters (finally!)
 *
 * Revision 1.13  1999/10/15 00:54:16  boloni
 * more work
 *
 * Revision 1.11  1999/10/13 20:25:18  kalle
 * chart type is now taken from param struct
 *
 * Revision 1.10  1999/10/13 15:07:58  kalle
 * More parameter work. Compiles.
 *
 */
