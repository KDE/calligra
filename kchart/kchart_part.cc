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
  m_bLoading = false;
  cerr << "Contstructor started!\n";
  initDoc();
  // hack
  setModified(true);
}

KChartPart::~KChartPart()
{
  cerr << "Part is going to be destroyed now!!!";
  if (_params != NULL)
    delete _params;
}


bool KChartPart::initDoc()
{
  // Initialize the parameter set for this chart document
  // PENDING(kalle,torben) Where to delete this?
  cerr << "InitDOC";
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
  if (isLoading()) {
    cerr << "Loading... Do not paint!!!...\n";
    return;
  }
  // if params is 0, initDoc() has not been called
  ASSERT( _params != 0 );

  // ####### handle transparency
  if( !transparent )
    painter.eraseRect( rect );

  // debug( "KChartPart::paintContent called, rows = %d, cols = %d", currentData.rows(), currentData.cols() );

  // Need to draw only the document rectangle described in the parameter rect.
  //  return;
  out_graph( rect.width(),
	     rect.height(), // short width, height 
	     &painter,        // Paint into this painter
	     _params,	      // the parameters of the chart,
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

bool KChartPart::save( ostream& out, const char * /*_format*/ ) {
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
  for (unsigned int row = 0;row < currentData.rows();row++) {
      for (unsigned int col = 0;col < currentData.cols();col++) {
	// later we need a value
	cerr << "Row " << row << "\n";
	KChartValue t = currentData.cell(row, col);
	QDomElement e = doc.createElement("cell");
	e.setAttribute("value", t.value.toDouble());
	  /*
	  if ( e.isNull() )
	      return e;
	  */
	data.appendChild(e);
      }
  }
  // now save the parameters
  chart.appendChild(data);

  QDomElement params = doc.createElement("params");
  chart.appendChild(params);
  params.setAttribute("type",(int)_params->type);

  if(!_params->title.isEmpty())
        {
        QDomElement title = doc.createElement( "title" );
        title.appendChild( doc.createTextNode( _params->title ) );
        params.appendChild( title );
        QDomElement titlefont = doc.createElement("titlefont");
        params.appendChild(titlefont);
        titlefont.appendChild( doc.createElement( "font",_params->titleFont() ) );

        }


  if(!_params->xtitle.isEmpty())
        {
        QDomElement xtitle = doc.createElement( "xtitle" );
        xtitle.appendChild( doc.createTextNode( _params->xtitle ) );
        params.appendChild( xtitle );
        QDomElement xtitlefont = doc.createElement("xtitlefont");
        xtitlefont.appendChild( doc.createElement( "font",_params->xTitleFont()  ) );
        params.appendChild(xtitlefont);
        }
  if(!_params->ytitle.isEmpty())
        {
        QDomElement ytitle = doc.createElement( "ytitle" );
        ytitle.appendChild( doc.createTextNode( _params->ytitle ) );
        params.appendChild( ytitle );
        QDomElement ytitlefont = doc.createElement("ytitlefont");
        ytitlefont.appendChild( doc.createElement( "font",_params->yTitleFont() ) );
        params.appendChild(ytitlefont);
        }
  if(!_params->ytitle2.isEmpty())
        {
        QDomElement ytitle2 = doc.createElement( "ytitle2" );
        ytitle2.appendChild( doc.createTextNode( _params->ytitle2 ) );
        params.appendChild( ytitle2 );
        }
   if((!_params->ylabel_fmt.isEmpty())||(!_params->ylabel2_fmt.isEmpty()))
        {
        if(!_params->ylabel_fmt.isEmpty())
                {
                QDomElement ylabelfmt = doc.createElement( "ylabelfmt" );
                ylabelfmt.appendChild( doc.createTextNode( _params->ylabel_fmt ) );
                params.appendChild( ylabelfmt );
                }
        if(!_params->ylabel2_fmt.isEmpty())
                {
                QDomElement ylabel2fmt = doc.createElement( "ylabel2fmt" );
                ylabel2fmt.appendChild( doc.createTextNode( _params->ylabel2_fmt ) );
                params.appendChild( ylabel2fmt );
                }

        }
  QDomElement labelfont = doc.createElement("labelfont");
  labelfont.appendChild( doc.createElement( "font",_params->labelFont() ) );
  params.appendChild(labelfont);

  QDomElement yaxisfont = doc.createElement("yaxisfont");
  yaxisfont.appendChild( doc.createElement( "font",_params->yAxisFont() ) );
  params.appendChild(yaxisfont);

  QDomElement xaxisfont = doc.createElement("xaxisfont");
  xaxisfont.appendChild( doc.createElement( "font",_params->xAxisFont() ) );
  params.appendChild(xaxisfont);

  QDomElement yaxis = doc.createElement("yaxis");
  yaxis.setAttribute("ymin",_params->requested_ymin);
  yaxis.setAttribute("ymax",_params->requested_ymax);
  yaxis.setAttribute("yinterval",_params->requested_yinterval);
  params.appendChild(yaxis);

  QDomElement graph = doc.createElement("graph");
  graph.setAttribute("grid",(int)_params->grid);
  graph.setAttribute("xaxis",(int)_params->xaxis);
  graph.setAttribute("yaxis",(int)_params->yaxis);
  graph.setAttribute("shelf",(int)_params->shelf);
  graph.setAttribute("yaxis2",(int)_params->yaxis2);
  graph.setAttribute("ystyle",(int)_params->yval_style);
  graph.setAttribute("border",(int)_params->border);
  graph.setAttribute("transbg",(int)_params->transparent_bg);
  params.appendChild(graph);
  //graph params
  QDomElement graphparams = doc.createElement("graphparams");
  graphparams.setAttribute("depth3d",(double)_params->_3d_depth);
  graphparams.setAttribute("angle3d",(short)_params->_3d_angle);
  graphparams.setAttribute("barwidth",(short)_params->bar_width);
  params.appendChild(graphparams);
  //graph color
  QDomElement graphcolor = doc.createElement("graphcolor");
  graphcolor.setAttribute( "bgcolor", _params->BGColor.name() );
  graphcolor.setAttribute( "gridcolor", _params->GridColor.name() );
  graphcolor.setAttribute( "linecolor", _params->LineColor.name() );
  graphcolor.setAttribute( "plotcolor", _params->PlotColor.name() );
  graphcolor.setAttribute( "volcolor", _params->VolColor.name() );
  graphcolor.setAttribute( "titlecolor", _params->TitleColor.name() );
  graphcolor.setAttribute( "xtitlecolor", _params->XTitleColor.name() );
  graphcolor.setAttribute( "ytitlecolor", _params->YTitleColor.name() );
  graphcolor.setAttribute( "ytitle2color", _params->YTitle2Color.name() );
  graphcolor.setAttribute( "xlabelcolor", _params->XLabelColor.name() );
  graphcolor.setAttribute( "ylabelcolor", _params->YLabelColor.name() );
  graphcolor.setAttribute( "ylabel2color", _params->YLabel2Color.name() );
  params.appendChild(graphcolor);

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

bool KChartPart::loadChildren( KoStore* /*_store*/ ) {
  cerr << "kchart loadChildren called\n";
  return true;
};

bool KChartPart::loadXML( const QDomDocument& doc, KoStore* /*store*/ ) {
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
  cerr << "Expanded!";
  QDomNode n = data.firstChild();

  for (int i=0; i!=rows; i++) {
    for (int j=0; j!=cols; j++) {
      if (n.isNull()) {
	qDebug("Some problems, there is less data than it should be!");
	break;
      }
      QDomElement e = n.toElement();
      if ( !e.isNull() && e.tagName() == "cell" ) {
	  // add the cell to the corresponding place...
	  double val = e.attribute("value").toDouble(&ok);
	  if (!ok)  {  return false; }
	  cerr << i << " " << j << "=" << val << "\n";
	  KChartValue t;
	  t.exists= true;
	  t.value.setValue(val);
	  // cerr << "Set cell for " << row << "," << col << "\n";
	  currentData.setCell(i,j,t);
	  n = n.nextSibling();
      }
    }
  }
  QDomElement params = chart.namedItem( "params" ).toElement();
  if ( params.hasAttribute( "type" ) )
        {
	 _params->type = (KChartType)params.attribute("type").toInt( &ok );
	 if ( !ok )
	        return false;
         }
  QDomElement title = params.namedItem( "title" ).toElement();
    if ( !title.isNull())
        {
         QString t = title.text();
         _params->title=t;
        }
  QDomElement titlefont = params.namedItem( "titlefont" ).toElement();
    if ( !titlefont.isNull())
        {
        QDomElement font = titlefont.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		_params->setTitleFont(font.toFont());
        }
  QDomElement xtitle = params.namedItem( "xtitle" ).toElement();
    if ( !xtitle.isNull())
        {
         QString t = xtitle.text();
         _params->xtitle=t;
        }
  QDomElement xtitlefont = params.namedItem( "xtitlefont" ).toElement();
    if ( !xtitlefont.isNull())
        {
        QDomElement font = xtitlefont.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		_params->setXTitleFont(font.toFont());
        }
  QDomElement ytitle = params.namedItem( "ytitle" ).toElement();
    if ( !ytitle.isNull())
        {
         QString t = ytitle.text();
         _params->ytitle=t;
        }
  QDomElement ytitle2 = params.namedItem( "ytitle2" ).toElement();
    if ( !ytitle2.isNull())
        {
         QString t = ytitle2.text();
         _params->ytitle2=t;
        }
  QDomElement ytitlefont = params.namedItem( "ytitlefont" ).toElement();
    if ( !ytitlefont.isNull())
        {
        QDomElement font = ytitlefont.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		_params->setYTitleFont(font.toFont());
        }
  QDomElement ylabelfmt = params.namedItem( "ylabelfmt" ).toElement();
    if ( !ylabelfmt.isNull())
        {
         QString t = ylabelfmt.text();
         _params->ylabel_fmt=t;
        }
  QDomElement ylabel2fmt = params.namedItem( "ylabel2fmt" ).toElement();
    if ( !ylabel2fmt.isNull())
        {
         QString t = ylabel2fmt.text();
         _params->ylabel2_fmt=t;
        }
  QDomElement labelfont = params.namedItem( "labelfont" ).toElement();
    if ( !labelfont.isNull())
        {
        QDomElement font = labelfont.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		_params->setLabelFont(font.toFont());
        }

  QDomElement yaxisfont = params.namedItem( "yaxisfont" ).toElement();
    if ( !yaxisfont.isNull())
        {
        QDomElement font = yaxisfont.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		_params->setYAxisFont(font.toFont());
        }

 QDomElement xaxisfont = params.namedItem( "xaxisfont" ).toElement();
    if ( !xaxisfont.isNull())
        {
        QDomElement font = xaxisfont.namedItem( "font" ).toElement();
	    if ( !font.isNull() )
		_params->setXAxisFont(font.toFont());
        }

  QDomElement yaxis = params.namedItem( "yaxis" ).toElement();
    if ( !yaxis.isNull())
        {
        if(yaxis.hasAttribute( "yinterval" ))
                {
                _params->requested_yinterval= yaxis.attribute("yinterval").toDouble( &ok );
	        if ( !ok ) return false;
                }
        if(yaxis.hasAttribute( "ymin" ))
                {
                _params->requested_ymin= yaxis.attribute("ymin").toDouble( &ok );
	        if ( !ok ) return false;
                }
         if(yaxis.hasAttribute( "ymax" ))
                {
                _params->requested_ymax= yaxis.attribute("ymax").toDouble( &ok );
	        if ( !ok ) return false;
                }
        }
  QDomElement graph = params.namedItem( "graph" ).toElement();
  if(!graph.isNull())
        {
        if(graph.hasAttribute( "grid" ))
                {
                _params->grid=(bool) graph.attribute("grid").toInt( &ok );
                if(!ok) return false;
                }
         if(graph.hasAttribute( "xaxis" ))
                {
                _params->xaxis=(bool) graph.attribute("xaxis").toInt( &ok );
                if(!ok) return false;
                }
         if(graph.hasAttribute( "yaxis" ))
                {
                _params->yaxis=(bool) graph.attribute("yaxis").toInt( &ok );
                if(!ok) return false;
                }
          if(graph.hasAttribute( "shelf" ))
                {
                _params->shelf=(bool) graph.attribute("shelf").toInt( &ok );
                if(!ok) return false;
                }
          if(graph.hasAttribute( "yaxis2" ))
                {
                _params->yaxis2=(bool) graph.attribute("yaxis2").toInt( &ok );
                if(!ok) return false;
                }
          if(graph.hasAttribute( "ystyle" ))
                {
                _params->yval_style=(bool) graph.attribute("ystyle").toInt( &ok );
                if(!ok) return false;
                }
          if(graph.hasAttribute( "border" ))
                {
                _params->border=(bool) graph.attribute("border").toInt( &ok );
                if(!ok) return false;
                }
          if(graph.hasAttribute( "transbg" ))
                {
                _params->transparent_bg=(bool) graph.attribute("transbg").toInt( &ok );
                if(!ok) return false;
                }
        }
  QDomElement graphparams = params.namedItem( "graphparams" ).toElement();
  if(!graphparams.isNull())
        {
         if(graphparams.hasAttribute( "dept3d" ))
                {
                _params->_3d_depth=graphparams.attribute("dept3d").toDouble( &ok );
                if(!ok) return false;
                }
         if(graphparams.hasAttribute( "angle3d" ))
                {
                _params->_3d_angle=graphparams.attribute("angle3d").toShort( &ok );
                if(!ok) return false;
                }
         if(graphparams.hasAttribute( "barwidth" ))
                {
                _params->bar_width=graphparams.attribute("barwidth").toShort( &ok );
                if(!ok) return false;
                }
        }
   QDomElement graphcolor = params.namedItem( "graphcolor" ).toElement();
   if(!graphcolor.isNull())
        {
         if(graphcolor.hasAttribute( "bgcolor" ))
                {
                _params->BGColor= QColor( graphcolor.attribute( "bgcolor" ) );
                }
         if(graphcolor.hasAttribute( "gridcolor" ))
                {
                _params->GridColor= QColor( graphcolor.attribute( "gridcolor" ) );
                }
         if(graphcolor.hasAttribute( "linecolor" ))
                {
                _params->LineColor= QColor( graphcolor.attribute( "linecolor" ) );
                }
         if(graphcolor.hasAttribute( "plotcolor" ))
                {
                _params->PlotColor= QColor( graphcolor.attribute( "plotcolor" ) );
                }
         if(graphcolor.hasAttribute( "volcolor" ))
                {
                _params->VolColor= QColor( graphcolor.attribute( "volcolor" ) );
                }
         if(graphcolor.hasAttribute( "titlecolor" ))
                {
                _params->TitleColor= QColor( graphcolor.attribute( "titlecolor" ) );
                }
         if(graphcolor.hasAttribute( "xtitlecolor" ))
                {
                _params->XTitleColor= QColor( graphcolor.attribute( "xtitlecolor" ) );
                }
         if(graphcolor.hasAttribute( "ytitlecolor" ))
                {
                _params->YTitleColor= QColor( graphcolor.attribute( "ytitlecolor" ) );
                }
         if(graphcolor.hasAttribute( "ytitle2color" ))
                {
                _params->YTitle2Color= QColor( graphcolor.attribute( "ytitle2color" ) );
                }
         if(graphcolor.hasAttribute( "xlabelcolor" ))
                {
                _params->XLabelColor= QColor( graphcolor.attribute( "xlabelcolor" ) );
                }
         if(graphcolor.hasAttribute( "ylabelcolor" ))
                {
                _params->YLabelColor= QColor( graphcolor.attribute( "ylabelcolor" ) );
                }
         if(graphcolor.hasAttribute( "ylabel2color" ))
                {
                _params->YLabel2Color= QColor( graphcolor.attribute( "ylabel2color" ) );
                }
        }
  return true;
};

bool KChartPart::load( istream& in, KoStore* store ) {
  cerr << "kchart load colled\n";
  m_bLoading = true;
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
    //bool b = true;
    //initDoc();

    buffer.close();
    // init the parameters 
    m_bLoading = false;
    return b;
};


#include "kchart_part.moc"

/**
 * $Log$
 * Revision 1.17  2000/01/06 20:32:46  mlaurent
 * Bug fix
 * Now you can choose font and color in kchartWizard
 *
 * Revision 1.16  2000/01/05 20:09:51  mlaurent
 * Improved save parameters
 *
 * Revision 1.15  2000/01/05 07:50:22  mlaurent
 * Improved save parameters
 *
 * Revision 1.14  2000/01/04 21:02:31  mlaurent
 * Start save parameters in file
 *
 * Revision 1.13  2000/01/03 20:26:42  mlaurent
 * Improved kchartWizard and bugfix
 *
 * Revision 1.12  1999/12/21 22:36:17  faure
 * Porting to new QVariant. More like this and I write a script !
 *
 * Revision 1.11  1999/11/29 21:26:14  wtrobin
 * - fixed some ugly warnings
 * - made kchart compile with --enable-final
 *
 * Revision 1.10  1999/11/21 20:26:45  boloni
 * -multidocument view works - but it still freezes at load and new. But this
 * time it is probably my fault somewhere
 *
 * Revision 1.9  1999/11/21 17:43:29  boloni
 * -save and load works but the New and Load mechanisms from KoMainWindow hangs
 * it. Use loading from the command line.
 * -there is something wrong with that mechanism, because KWord is not working in multiple document mode either - it just overwrites the old one for New or Load.
 *
 * Revision 1.8  1999/11/21 16:40:13  boloni
 * save-load works
 * data files can be specified at the command line
 * the is some problem with load, still
 *
 * Revision 1.7  1999/11/21 15:27:14  boloni
 * ok
 *
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
