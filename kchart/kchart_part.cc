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
#include "kchart_factory.h"
#include "kchartWizard.h"
#include <kstddirs.h>

#include <kglobal.h>
#include <kdebug.h> // "ported" to kdDebug(35001)

#include "kdchart/KDChartParams.h"
#include "kdchart/KDChart.h"

using namespace std;

// Some hardcoded data for a chart

/* ----- set some data ----- */
// float   a[6]  = { 0.5, 0.09, 0.6, 0.85, 0.0, 0.90 },
// b[6]  = { 1.9, 1.3,  0.6, 0.75, 0.1, -2.0 };
/* ----- X labels ----- */
// char    *t[6] = { "Chicago", "New York", "L.A.", "Atlanta", "Paris, MD\n(USA) ", "London" };
/* ----- data set colors (RGB) ----- */
// QColor   sc[2]    = { QColor( 255, 128, 128 ), QColor( 128, 128, 255 ) };


#include <qpainter.h>

KChartPart::KChartPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoChart::Part( parentWidget, widgetName, parent, name, singleViewMode ),
      _params( 0 ),
      _parentWidget( parentWidget )
{
    m_bLoading = false;
    kdDebug(35001) << "Constructor started!" << endl;

    setInstance( KChartFactory::global(), false );

    (void)new WizardExt( this );
    initDoc();
    // hack
    setModified(true);
}

KChartPart::~KChartPart()
{
    kdDebug(35001) << "Part is going to be destroyed now!!!" << endl;
    if( _params != 0 )
        delete _params;
}


bool KChartPart::initDoc()
{
    // Initialize the parameter set for this chart document
    kdDebug(35001) << "InitDOC" << endl;
    _params = new KDChartParams();
    _params->setThreeDBars( true );

    return TRUE;
}

void KChartPart::initRandomData()
{
    // fill cells
    int col,row;
    // initialize some data, if there is none
    if (currentData.rows() == 0) {
        kdDebug(35001) << "Initialize with some data!!!" << endl;
        currentData.expand(4,4);
	currentData.setUsedRows( 4 );
	currentData.setUsedCols( 4 );
        for (row = 0;row < 4;row++)
            for (col = 0;col < 4;col++) {
                KoChart::Value t( (double)row+col );
                // kdDebug(35001) << "Set cell for " << row << "," << col << endl;
                currentData.setCell(row,col,t);
            }
    }

}



KoView* KChartPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KChartView( this, parent, name );
}

void KChartPart::paintContent( QPainter& painter, const QRect& rect, bool transparent, double /*zoomX*/, double /*zoomY*/ )
{
    if (isLoading()) {
        kdDebug(35001) << "Loading... Do not paint!!!..." << endl;
        return;
    }
    // if params is 0, initDoc() has not been called
    ASSERT( _params != 0 );

    // ####### handle transparency
    if( !transparent )
        painter.eraseRect( rect );

    // ## TODO: support zooming

    // kdDebug(35001) << "KChartPart::paintContent called, rows = "
    //                << currentData.usedRows() << ", cols = "
    //                << currentData.usedCols() << endl;

    // Need to draw only the document rectangle described in the parameter rect.
    //  return;

    KDChart::paint( &painter, _params, &currentData );
}


void KChartPart::setData( const KoChart::Data& data )
{
    currentData = data;
    //  initLabelAndLegend();
    emit docChanged();
}


void KChartPart::showWizard()
{
    KChartWizard* wizard = new KChartWizard( this, _parentWidget, "wizard" );
    (void)wizard->exec();
    delete wizard;
}

void KChartPart::initLabelAndLegend()
{
    // Labels and legends are automatically initialized to reasonable
    // default values in KDChart
}

void KChartPart::loadConfig( KConfig *conf )
{
  conf->setGroup("ChartParameters");

  // TODO: the fonts
  // PENDING(kalle) Put the applicable ones of these back in
  //   QFont tempfont;
//   tempfont = conf->readFontEntry("titlefont", &titlefont);
//   setTitleFont(tempfont);
//   tempfont = conf->readFontEntry("ytitlefont", &ytitlefont);
//   setYTitleFont(tempfont);
//   tempfont = conf->readFontEntry("xtitlefont", &xtitlefont);
//   setXTitleFont(tempfont);
//   tempfont = conf->readFontEntry("yaxisfont", &yaxisfont);
//   setYAxisFont(tempfont);
//   tempfont = conf->readFontEntry("xaxisfont", &xaxisfont);
//   setXAxisFont(tempfont);
//   tempfont = conf->readFontEntry("labelfont", &labelfont);
//   setLabelFont(tempfont);
//   tempfont = conf->readFontEntry("annotationfont", &annotationfont);
//   setAnnotationFont(tempfont);

//   ylabel_fmt = conf->readEntry("ylabel_fmt", ylabel_fmt );
//   ylabel2_fmt = conf->readEntry("ylabel2_fmt", ylabel2_fmt);
//   xlabel_spacing = conf->readNumEntry("xlabel_spacing");
//   ylabel_density = conf->readNumEntry("ylabel_density", ylabel_density);
//   requested_ymin = conf->readDoubleNumEntry("requested_ymin", requested_ymin);
//   requested_ymax = conf->readDoubleNumEntry("requested_ymax", requested_ymax );
//   requested_yinterval = conf->readDoubleNumEntry("requested_yinterval",
// 					   requested_yinterval);
//   shelf = conf->readBoolEntry("shelf", shelf);
//   grid = conf->readBoolEntry("grid", grid);
//   xaxis = conf->readBoolEntry("xaxis", xaxis);
//   yaxis = conf->readBoolEntry("yaxis", yaxis);
//   yaxis2 = conf->readBoolEntry("yaxis2", yaxis);
//   llabel = conf->readBoolEntry("llabel", llabel);
//   yval_style = conf->readNumEntry("yval_style", yval_style);
//   stack_type = (KChartStackType)conf->readNumEntry("stack_type", stack_type);

  _params->setThreeDBarDepth( conf->readDoubleNumEntry("_3d_depth",
                                                       _params->threeDBarDepth() ) );
  _params->setThreeDBarAngle( conf->readDoubleNumEntry( "_3d_angle",
                                                        _params->threeDBarAngle() ) );

//   hlc_style = (KChartHLCStyle)conf->readNumEntry("hlc_style", hlc_style);
//   hlc_cap_width = conf->readNumEntry("hlc_cap_width", hlc_cap_width);
//   // TODO: Annotation font
//   num_scatter_pts = conf->readNumEntry("num_scatter_pts", num_scatter_pts);
//   // TODO: Scatter type
//   thumbnail = conf->readBoolEntry("thumbnail", thumbnail);
//   thumblabel = conf->readEntry("thumblabel", thumblabel);
//   border = conf->readBoolEntry("border", border);
//   BGColor = conf->readColorEntry("BGColor", &BGColor);
//   GridColor = conf->readColorEntry("GridColor", &GridColor);
//   LineColor = conf->readColorEntry("LineColor", &LineColor);
//   PlotColor = conf->readColorEntry("PlotColor", &PlotColor);
//   VolColor = conf->readColorEntry("VolColor", &VolColor);
//   TitleColor = conf->readColorEntry("TitleColor", &TitleColor);
//   XTitleColor = conf->readColorEntry("XTitleColor", &XTitleColor);
//   YTitleColor = conf->readColorEntry("YTitleColor", &YTitleColor);
//   YTitle2Color = conf->readColorEntry("YTitle2Color", &YTitle2Color);
//   XLabelColor = conf->readColorEntry("XLabelColor", &XLabelColor);
//   YLabelColor = conf->readColorEntry("YLabelColor", &YLabelColor);
//   YLabel2Color = conf->readColorEntry("YLabel2Color", &YLabel2Color);
//   EdgeColor = conf->readColorEntry("EdgeColor", &EdgeColor);
//   loadColorArray(conf, &SetColor, "SetColor");
//   loadColorArray(conf, &ExtColor, "ExtColor");
//   loadColorArray(conf, &ExtVolColor, "ExtVolColor");
//   transparent_bg = conf->readBoolEntry("transparent_bg", transparent_bg);
//   // TODO: explode, missing
//   percent_labels = (KChartPercentType)conf->readNumEntry("percent_labels",
// 							 percent_labels);
//   label_dist = conf->readNumEntry("label_dist", label_dist);
//   label_line = conf->readBoolEntry("label_line", label_line);
  _params->setChartType( (KDChartParams::ChartType)conf->readNumEntry( "type", _params->chartType() ) );
//   other_threshold = conf->readNumEntry("other_threshold", other_threshold);

//   backgroundPixmapName = conf->readEntry( "backgroundPixmapName", QString::null );
//   if( !backgroundPixmapName.isNull() ) {
//     backgroundPixmap.load( locate( "wallpaper", backgroundPixmapName ));
//     backgroundPixmapIsDirty = true;
//   } else
//     backgroundPixmapIsDirty = false;
//   backgroundPixmapScaled = conf->readBoolEntry( "backgroundPixmapScaled", true );
//   backgroundPixmapCentered = conf->readBoolEntry( "backgroundPixmapCentered", true );
//   backgroundPixmapIntensity = conf->readDoubleNumEntry( "backgroundPixmapIntensity", 0.25 );
}

void KChartPart::defaultConfig(  )
{
    delete _params;
    _params = new KDChartParams();
}

void KChartPart::saveConfig( KConfig *conf )
{
  conf->setGroup("ChartParameters");

  // PENDING(kalle) Put some of these back in
  // the fonts
//   conf->writeEntry("titlefont", titlefont);
//   conf->writeEntry("ytitlefont", ytitlefont);
//   conf->writeEntry("xtitlefont", xtitlefont);
//   conf->writeEntry("yaxisfont", yaxisfont);
//   conf->writeEntry("xaxisfont", xaxisfont);
//   conf->writeEntry("labelfont", labelfont);

//   conf->writeEntry("ylabel_fmt", ylabel_fmt);
//   conf->writeEntry("ylabel2_fmt", ylabel2_fmt);
//   conf->writeEntry("xlabel_spacing", xlabel_spacing);
//   conf->writeEntry("ylabel_density", ylabel_density);
//   conf->writeEntry("requested_ymin", requested_ymin);
//   conf->writeEntry("requested_ymax", requested_ymax);
//   conf->writeEntry("requested_yinterval", requested_yinterval);

//   conf->writeEntry("shelf", shelf);
//   conf->writeEntry("grid", grid );
//   conf->writeEntry("xaxis", xaxis);
//   conf->writeEntry("yaxis", yaxis);
//   conf->writeEntry("yaxis2", yaxis2);
//   conf->writeEntry("llabel", llabel);
//   conf->writeEntry("yval_style", yval_style );
//   conf->writeEntry("stack_type", (int)stack_type);

  conf->writeEntry( "_3d_depth", _params->threeDBarDepth() );
  conf->writeEntry( "_3d_angle", _params->threeDBarAngle() );

//   conf->writeEntry("hlc_style", (int)hlc_style);
//   conf->writeEntry("hlc_cap_width", hlc_cap_width );
//   // TODO: Annotation type!!!
//   conf->writeEntry("annotationfont", annotationfont);
//   conf->writeEntry("num_scatter_pts", num_scatter_pts);
//   // TODO: Scatter type!!!
//   conf->writeEntry("thumbnail", thumbnail);
//   conf->writeEntry("thumblabel", thumblabel);
//   conf->writeEntry("thumbval", thumbval);
//   conf->writeEntry("border", border);
//   conf->writeEntry("BGColor", BGColor);
//   conf->writeEntry("GridColor", GridColor);
//   conf->writeEntry("LineColor", LineColor);
//   conf->writeEntry("PlotColor", PlotColor);
//   conf->writeEntry("VolColor", VolColor);
//   conf->writeEntry("TitleColor", TitleColor);
//   conf->writeEntry("XTitleColor", XTitleColor);
//   conf->writeEntry("YTitleColor", YTitleColor );
//   conf->writeEntry("YTitle2Color", YTitle2Color);
//   conf->writeEntry("XLabelColor", XLabelColor );
//   conf->writeEntry("YLabelColor", YLabelColor);
//   conf->writeEntry("YLabel2Color", YLabel2Color);
//   conf->writeEntry("EdgeColor", EdgeColor);
//   saveColorArray(conf, &SetColor, "SetColor");
//   saveColorArray(conf, &ExtColor, "ExtColor");
//   saveColorArray(conf, &ExtVolColor, "ExtVolColor");


//   conf->writeEntry("transparent_bg", transparent_bg);
//   // TODO: explode, missing
//   conf->writeEntry("percent_labels",(int) percent_labels );
//   conf->writeEntry("label_dist", label_dist);
//   conf->writeEntry("label_line", label_line);
  conf->writeEntry( "type", (int)_params->chartType() );
//   conf->writeEntry("other_threshold", other_threshold);

  // background pixmap stuff
//   if( !backgroundPixmapName.isNull() )
// 	conf->writeEntry( "backgroundPixmapName", backgroundPixmapName );
//   conf->writeEntry( "backgroundPixmapIsDirty", backgroundPixmapIsDirty );
//   conf->writeEntry( "backgroundPixmapScaled", backgroundPixmapScaled );
//   conf->writeEntry( "backgroundPixmapCentered", backgroundPixmapCentered );
//   conf->writeEntry( "backgroundPixmapIntensity", backgroundPixmapIntensity );
}

QDomDocument KChartPart::saveXML()
{
    return _params->saveXML();
}


bool KChartPart::loadXML( QIODevice*, const QDomDocument& doc )
{
    return _params->loadXML( doc );
}


#include "kchart_part.moc"

/**
 * $Log$
 * Revision 1.56  2001/06/11 07:20:26  kalle
 * Resurrected most of the config dialog and the data editor (not everything
 * is connected yet)
 *
 * Revision 1.55  2001/06/10 16:02:52  kalle
 * Replacement of the old charting engine with the new KDChart charting
 * engine.
 *
 * The wizard and the configuration dialog are still disabled and will
 * be turned on again one after the other now.
 *
 * Revision 1.54  2001/04/18 11:32:02  faure
 * paintContent API fix
 *
 * Revision 1.53  2001/04/11 09:36:33  mlaurent
 * Apply patch from Toshitaka Fujioka <toshitaka@kde.gr.jp>
 * Now you can see legend in top-left.
 * Thanks a lot.
   *
   * Fix load none image in kchartBackgroundPixmapConfigPage.cc
   *
   * Revision 1.52  2000/10/20 07:20:38  kalle
   * Fixes bug that you cannot have more than 4x4 charts when entering data in the data editor
   * (I wonder whether this is important enough to go into 2.0.1, can anybody help me decide?)
   *
   * Revision 1.51  2000/10/14 15:30:40  faure
   * Patch from Laurent (background pixmap, and some cleanup).
   *
   * Revision 1.50  2000/10/11 14:54:33  faure
   * +  setInstance( KChartFactory::global(), false );
   * This fixes saving a kchart child (it was saved with mime="application/x-kspread"),
   * bug reported by Laurent Montel.
   * One line missing, and I lost an hour....
   *
   * Revision 1.49  2000/09/23 21:37:36  hausmann
   * - removed all component specific shells. they are not needed anymore.
   *   (as discussed with David)
   *
   * Revision 1.48  2000/09/22 20:59:48  hausmann
   * - don't link kspread against kchart. instead use some shared interface
   *   (which probably needs some finetuning :-}
   *   Posted to koffice-devel last week, approved by David (locally in
   *   Erlangen ;-)
   *   no translations/messages touched!
   *
   * Revision 1.47  2000/08/10 09:40:39  kalle
   * Fixed #7834 and #7380
   *
   * Revision 1.46  2000/07/19 00:51:08  kalle
   * Brushed up the wizard. Still not very nice, but sort of working.
   * Fixed the font config page. Legend color box needs either rework or removal.
   *
   * Revision 1.45  2000/07/18 23:02:17  kalle
   * implemented loading/saving of missing parameters, removed unnecessary field from param struct
   *
   * Revision 1.44  2000/07/18 22:19:43  kalle
   * now even compiles :-o
   *
   * Revision 1.43  2000/07/18 22:12:26  kalle
   * added showWizard()
   *
   * Revision 1.42  2000/07/18 21:48:59  kalle
   * renamed kchartWizard* to KChartWizard* for consistency with the rest
   *
   * Revision 1.41  2000/07/16 01:52:40  faure
   * Add support for applying a "pre-processing filter" before loading an
   * XML document. Used in KPresenter to apply a perl script. Currently
   * not activated, will be when Reggie commits the new text object.
 *
 * Revision 1.40  2000/07/14 12:31:01  faure
 * Ported to new loadXML stuff
 *
 * Revision 1.39  2000/07/13 11:48:08  faure
 * Don't do setRootDocument twice
 *
 * Revision 1.38  2000/06/20 06:40:24  wtrobin
 * - removed the createDoc implementation of all the childs
 *   and implemented The Right Solution(tm) in koMainWindow.cc
 *   (We're now using the mimetype to create a KoDocumentEntry
 *   and use it to create a doc). This fixes the "libkword closed" bug
 *   (as discussed with Simon - Thanks once again :)
 * - made createView non-virtual and added a protected pure virtual
 *   createViewInstance (David, what do you think of the name? :).
 * - Adapted all apps to these changes
 * - removed the topmostParentDocument() hack
 *
 * Will test that now more accurately, but it seems to work nicely
 *
 * Revision 1.37  2000/06/18 10:03:11  wtrobin
 * Still hunting bugs :)
 *
 * Revision 1.36  2000/06/18 08:32:40  wtrobin
 * first steps - don't update, it's broken :)
 *
 * Revision 1.35  2000/06/14 19:44:02  wtrobin
 * even more QDom
 *
 * Revision 1.34  2000/06/14 18:37:24  wtrobin
 * QDom fun
 *
 * Revision 1.33  2000/06/12 19:46:22  faure
 * Ported to new kofficecore
 *
 * Revision 1.32  2000/04/21 06:34:21  wtrobin
 * qDebug, debug, cerr, cout,... -> kdDebug(35001)
 *
 * Revision 1.31  2000/04/08 16:43:13  mlaurent
 * Now "Chart Combo*" works
 * Bug fix
 *
 * Revision 1.30  2000/04/02 16:41:46  mlaurent
 * Now you can add an annotation
 * Bug fix
 * Improved "Config Dialog"
 *
 * Revision 1.29  2000/02/23 05:50:06  mlaurent
 * Bug fix
 *
 * Revision 1.28  2000/02/20 22:39:54  mueller
 * compilation fix with new Qt
 *
 * Revision 1.27  2000/02/13 19:32:03  mlaurent
 * bug fix
 *
 * Revision 1.26  2000/02/13 14:34:18  hausmann
 * - fixed segfault in KoView destructor (use a QGuardedPtr on the manager)
 * - hacked KoDocument to hold a list of associated shells and kill them
 *   upon destruction of the document
 * - fixed KoMainWindow and all the apps to let each shell have a *correct*
 *   KInstance assigned
 * - ignore generated Makefiles in doc/katabase
 *
 * Revision 1.25  2000/02/08 13:12:30  coolo
 * some fixes for ANSI C++ (I gave up on kchart ;(
 *
 * Revision 1.24  2000/02/05 18:12:33  shausman
 * the port of KOffice to use kparts instead of koparts. What I basically did
 * was:
 * - merged the container/view functionality from koparts into kofficecore
 * - made KoDocument inherits KParts::ReadWritePart
 * - added support for single-view-mode to KoDocument and reimplemented
 *   some virtual methods of KParts::Part to redirect requests to the view
 *   (like action requests for example)
 * - ported the nested child activation/deactivation code to
 *   KParts::PartManager (which was a pain, as the old code just deleted
 *   child views upon deactivation, while in kparts we first have to
 *   deactivate a views GUI before we can delete it)
 * - removed the m_bDoPartActivation and partDoubleClick hacks from the
 *   KWord/KPresenter activation stuff ;-) and replaced with a more simple
 *   mechanism of adding the child parts to the tree just right before the
 *   activation
 *   (there are still bugs in the nested part activation/deactivation stuff
 *   though, but I'll look into that)
 * - got rid of all the duplicated shell xml documents and used a common
 *   shared shell gui
 * - added support for read-write state changing (as supported by kparts)
 *   I currently only implemented it in KSpread and added big TODO warnings
 *   to the other apps ;-) . In those methods just deactivate all actions
 *   and widgets which could possibly modify the document, in case we
 *   enter readonly state (this is *VERY* important, *please* support it
 *   properly)
 * - ported all the .rc files to the kparts standard and ported the
 *   calculator plugin of kspread to a real kparts plugin
 * - I *removed* support for inplace editing! (important, Torben, please read
 *   :-)
 *   (I commented out the inplace xml stuff in the .rc files)
 *   The reason for the disabled support is:
 *   I can't see how this is supposed to work at all!
 *   Imaginge I embed a KSpread document into KWord, rotate it and activate
 *   it. This is when the inplace-editing GUI is meant to be used, instead of
 *   the "usual" kspread gui. BUT: This cannot work, as the actions are
 *   only available (allocated, implemented, used, etc..) in a KoView.
 *   HOWEVER a KoView is *not* created in this case, so I cannot see how this
 *   is supposed to work?
 *   Perhaps I'm missing something, but the only situation I see where it
 *   worked was in kspread itself ;-) , as if I embed a kspread table into
 *   another kspread table, then the shell (in koparts) considered the
 *   embedded document to be active, but the active view (as this is where
 *   the user clicked) is *still* the parent KoView (kspread view). So
 *   the shell was able to find the actions, apparently from the completely
 *   wrong view though. Perhaps Torben can help here? :-)
 *   (perhaps we should move all actions from the view to the document?)
 * - added check if the global application object is a KoApplication, so that
 *   the filter stuff is only used inside KOffice (otherwise it used
 *   to crash horribly ;)
 *
 * Revision 1.23  2000/01/28 20:00:37  mlaurent
 * Improved save and load parameters (label,legend,extcolor)
 *
 * Revision 1.22  2000/01/23 14:48:48  mlaurent
 * Improved pie chart
 *
 * Revision 1.21  2000/01/22 19:55:19  mlaurent
 * Add new page for config pie chart
 * Now pie chart works (a lot :( there are against bug)
 *
 * Revision 1.20  2000/01/21 16:04:03  mlaurent
 * Add legend and label
 * Add new page for config font
 * bug fix
 *
 * Revision 1.19  2000/01/08 20:36:21  mlaurent
 * Now you can choose a "sub type chart"
 * Now there are a color for same think
 *
 * Revision 1.18  2000/01/07 20:37:42  mlaurent
 * *** empty log message ***
 *
 * Revision 1.17  2000/01/06 20:32:46  mlaurent
 * Bug fix
 * Now you can choose font and color in KChartWizard
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
 * Improved KChartWizard and bugfix
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
