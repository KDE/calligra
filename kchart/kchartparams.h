/* $Id$ */

#ifndef _KCHARTPARAMETERS_H
#define _KCHARTPARAMETERS_H

#include <qcolor.h>
#include <qpen.h>
#include <qvector.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlist.h>
#include <qstrlist.h>

#include <kpixmap.h>

#include "engine.h"

class KConfig;

enum KChartPercentType {
    KCHARTPCTTYPE_NONE,
    KCHARTPCTTYPE_ABOVE,
    KCHARTPCTTYPE_BELOW,
    KCHARTPCTTYPE_RIGHT,
    KCHARTPCTTYPE_LEFT
};

enum KChartHLCStyle {
    KCHARTHLCSTYLE_DIAMOND = 1,
    KCHARTHLCSTYLE_CLOSECONNECTED = 2,
    KCHARTHLCSTYLE_CONNECTING = 4,
    KCHARTHLCSTYLE_ICAP = 8
};

enum KChartType {
    KCHARTTYPE_LINE,
    KCHARTTYPE_AREA,
    KCHARTTYPE_BAR,
    KCHARTTYPE_HILOCLOSE,
    KCHARTTYPE_COMBO_LINE_BAR,/* aka, VOL[ume] */
    KCHARTTYPE_COMBO_HLC_BAR,
    KCHARTTYPE_COMBO_LINE_AREA,
    KCHARTTYPE_COMBO_HLC_AREA,
    KCHARTTYPE_3DHILOCLOSE,
    KCHARTTYPE_3DCOMBO_LINE_BAR,
    KCHARTTYPE_3DCOMBO_LINE_AREA,
    KCHARTTYPE_3DCOMBO_HLC_BAR,
    KCHARTTYPE_3DCOMBO_HLC_AREA,
    KCHARTTYPE_3DBAR,
    KCHARTTYPE_3DAREA,
    KCHARTTYPE_3DLINE,
    KCHARTTYPE_3DPIE,
    KCHARTTYPE_2DPIE
};


enum KChartStackType {
    KCHARTSTACKTYPE_DEPTH,/* "behind" (even non-3D) */
    KCHARTSTACKTYPE_SUM,
    KCHARTSTACKTYPE_BESIDE,
    KCHARTSTACKTYPE_LAYER,
    KCHARTSTACKTYPE_PERCENT
};


enum KChartScatterIndType {
    KCHARTSCATTER_TRIANGLE_DOWN,
    KCHARTSCATTER_TRIANGLE_UP
};


enum KChartJustifyType {
    KCHARTJUSTIFYRIGHT,
    KCHARTJUSTIFYCENTER,
    KCHARTJUSTIFYLEFT
};


struct KChartScatterType {
    double point;		/* 0 <= point < num_points */
    double val;
    unsigned short width;		/* % (1-100) */
    QColor color;
    KChartScatterIndType ind;
};

struct KChartAnnotationType {
    double point;/* 0 <= point < num_points */
    QColor color;
    QString note;/* NLs ok here */
};


typedef QVector<QColor> _KChartColorArray;

class KChartColorArray : protected _KChartColorArray
{
public:
    KChartColorArray() :
	_KChartColorArray( 0 ) { setAutoDelete( true ); }

    void setColor( uint pos, QColor );
    QColor color( uint pos ) const;
    uint count() const { return _KChartColorArray::count(); }
};

inline void KChartColorArray::setColor( uint pos, QColor color )
{
    if( pos >= size() )
	resize( pos + 1 );

    insert( pos, new QColor( color ) );
}


inline QColor KChartColorArray::color( uint pos ) const
{
    return *( at( pos ) );
}


struct KChartParameters
{
    KChartParameters();

    bool do_vol();
    bool do_bar();
    bool threeD();
    bool has_hlc_sets();
    bool has_yaxis2();
    bool isPie();

    void saveConfig(KConfig *conf);
    void loadConfig(KConfig *conf);
    void defaultConfig();
    QString title;
    QString xtitle;
    QString ytitle;
    QString ytitle2;
    QString ylabel_fmt;
    QString ylabel2_fmt;
    short xlabel_spacing;
    short ylabel_density;
    double requested_ymin;
    double requested_ymax;
    double requested_yinterval;
    bool shelf;
    bool grid;
    bool xaxis;
    bool yaxis;
    bool yaxis2;
    bool cross;
    bool llabel;
    int yval_style;
    KChartStackType stack_type;
    double _3d_depth;
    short _3d_angle;
    short bar_width;
    KChartHLCStyle hlc_style;
    short hlc_cap_width;
    KChartAnnotationType* annotation;
    int num_scatter_pts;
    KChartScatterType* scatter;
    bool thumbnail;
    QString thumblabel;
    double thumbval;
    bool border;
    QColor BGColor;
    QColor GridColor;
    QColor LineColor;
    QColor PlotColor;
    QColor VolColor;
    QColor TitleColor;
    QColor XTitleColor;
    QColor YTitleColor;
    QColor YTitle2Color;
    QColor XLabelColor;
    QColor YLabelColor;
    QColor YLabel2Color;
    QColor EdgeColor; // pies
    KChartColorArray SetColor;
    KChartColorArray ExtColor;
    KChartColorArray ExtVolColor;
    bool transparent_bg;
    bool hasxlabel;
    QStringList xlbl;
    QStringList legend;
    QArray<int> explode; // pies
    QArray<bool> missing; // pies
    KChartPercentType percent_labels; // pies
    int label_dist; // pies
    bool label_line; // pies
    KChartType type;
    short other_threshold; // pies
    int colPie; //pies
    int offsetCol;
    bool hard_size;
    int hard_graphheight;
    int hard_graphwidth;
    int hard_xorig;
    int hard_yorig;
    int label_width;

  // background pixmap stuff
  KPixmap backgroundPixmap;
  QString backgroundPixmapName;
  bool backgroundPixmapIsDirty;
  bool backgroundPixmapScaled;
  bool backgroundPixmapCentered;
  float backgroundPixmapIntensity;

    void setTitleFont( QFont f ) {
	titlefont = f;
	QFontMetrics fm( f );
	titlefontwidth = fm.maxWidth();
	titlefontheight = fm.height();
    }
    QFont titleFont() const { return titlefont; }
    int titleFontWidth() const { return titlefontwidth; }
    int titleFontHeight() const { return titlefontheight; }

    void setYTitleFont( QFont f ) {
	ytitlefont = f;
	QFontMetrics fm( f );
	ytitlefontwidth = fm.maxWidth();
	ytitlefontheight = fm.height();
    }
    QFont yTitleFont() const { return ytitlefont; }
    int yTitleFontWidth() const { return ytitlefontwidth; }
    int yTitleFontHeight() const { return ytitlefontheight; }

    void setXTitleFont( QFont f ) {
	xtitlefont = f;
	QFontMetrics fm( f );
	xtitlefontwidth = fm.maxWidth();
	xtitlefontheight = fm.height();
    }
    QFont xTitleFont() const { return xtitlefont; }
    int xTitleFontWidth() const { return xtitlefontwidth; }
    int xTitleFontHeight() const { return xtitlefontheight; }

    void setYAxisFont( QFont f ) {
	yaxisfont = f;
	QFontMetrics fm( f );
	yaxisfontwidth = fm.maxWidth();
	yaxisfontheight = fm.height();
    }
    QFont yAxisFont() const { return yaxisfont; }
    int yAxisFontWidth() const { return yaxisfontwidth; }
    int yAxisFontHeight() const { return yaxisfontheight; }
	
    void setXAxisFont( QFont f ) {
	xaxisfont = f;
	QFontMetrics fm( f );
	xaxisfontwidth = fm.maxWidth();
	xaxisfontheight = fm.height();
    }
    QFont xAxisFont() const { return xaxisfont; }
    int xAxisFontWidth() const { return xaxisfontwidth; }
    int xAxisFontHeight() const { return xaxisfontheight; }

    void setLabelFont( QFont f ) {
	labelfont = f;
	QFontMetrics fm( f );
	labelfontwidth = fm.maxWidth();
	labelfontheight = fm.height();
    }
    QFont labelFont() const { return labelfont; }
    int labelFontWidth() const { return labelfontwidth; }
    int labelFontHeight() const { return labelfontheight; }

    void setAnnotationFont( QFont f ) {
	annotationfont = f;
	QFontMetrics fm( f );
	annotationfontwidth = fm.maxWidth();
	annotationfontheight = fm.height();
    }
    QFont annotationFont() const { return annotationfont; }
    int annotationFontWidth() const { return annotationfontwidth; }
    int annotationFontHeight() const { return annotationfontheight; }

private:
    QFont titlefont;
    int titlefontwidth;
    int titlefontheight;
    QFont ytitlefont;
    int ytitlefontwidth;
    int ytitlefontheight;
    QFont xtitlefont;
    int xtitlefontwidth;
    int xtitlefontheight;
    QFont yaxisfont;
    int yaxisfontwidth;
    int yaxisfontheight;
    QFont xaxisfont;
    int xaxisfontwidth;
    int xaxisfontheight;
    QFont labelfont; // pies
    int labelfontwidth;
    int labelfontheight;
    QFont annotationfont;
    int annotationfontwidth;
    int annotationfontheight;

    // helper functions
    void loadColorArray(KConfig *conf, 
			KChartColorArray *arr,
			QString name);
    void saveColorArray(KConfig *conf, 
			KChartColorArray *arr,
			QString name);
};


#endif
