#ifndef KCHARTENGINE_H
#define KCHARTENGINE_H

class KChartEngine {
public:
  KChartParameters* params;
  KoChart::Data const * data;
  QPainter *p;
  int imagewidth;
  int imageheight;
  int doLabels();
  /*int minmaxValues( int num_points,
		  int num_sets,
		  float *uvol,
		  float &highest,
		  float &lowest,
		  float &vhighest,
		  float &vlowest);*/
  int out_graph();

  int compute_yintervals();
  void titleText();
  void drawBorder();
  void drawAnnotation();
  void drawThumbnails();
  void drawScatter();
  void drawBackgroundImage();
  void prepareColors();

  // functions to resize the chart
  // implementation in kchartEngine_ComputeSize.cc
  void computeSize();
  void computeMinMaxValues();

  // grid plotting functions
  // implementation in kchartEngine_Grid.cc
  void drawGridAndLabels(bool do_ylbl_fractions);
  void drawYAxisTitle();
  void drawVYAxisTitle();
  void draw3DGrids();
  void drawShelfGrids();
  void drawXTicks();
  void drawVolumeGrids();
  void draw3DAnnotation();
  void draw3DShelf();

  // data plotting functions
  // implementation in kchartEngine_DataDraw.cc
  void drawData();
  void drawBars();
  void drawLines();
  void drawCross();
  void draw3DLines();
  void drawArea();
  void drawCombo();
  void draw3DCombo();
  /*try chart pie*/
  void drawPie();

  // other stuff

  inline bool CELLEXISTS( int row, int col ) {
    return  data->cell(row,col).isValid();
  };
  inline double CELLVALUE(int row, int col) {
    return data->cell(row,col).toDouble();
  }
  
private:
  int num_sets;
  int num_hlc_sets;
  int num_points;
  int graphwidth;
  int graphheight;
  float	xorig, yorig, vyorig;
  float	yscl;
  float	vyscl;
  float	xscl;
  float vhighest;
  float	vlowest;
  float	highest;
  float	lowest;
  float	ylbl_interval;
  int num_lf_xlbls;
  int xdepth_3Dtotal;
  int ydepth_3Dtotal;
  int xdepth_3D;		// affects PX()
  int ydepth_3D;		// affects PY() and PV()
  int hlf_barwdth;		// half bar widths
  int hlf_hlccapwdth;		// half cap widths for HLC_I_CAP and DIAMOND
  int annote_len, annote_hgt;
  bool do_ylbl_fractions;

  /* args */
  int setno;// affects PX() and PY()
  // PENDING(kalle) Get rid of this
  //float *uvol;
#define MAXNUMPOINTS 100
  QColor BGColor, LineColor, PlotColor, GridColor, VolColor;
  QColor ExtVolColor[100];
  //  QColor ExtVolColor[num_points];  
  QColor ThumbDColor, ThumbLblColor, ThumbUColor;
    // ArrowDColor,ArrowUColor,
  QColor AnnoteColor;
  //  QColor ExtColor[num_sets][num_points];
  QColor ExtColor[100][100];
  // shade colors only with 3D
  //	intExtColorShd[threeD?1:num_sets][threeD?1:num_points]; // compiler limitation
  QColor ExtColorShd[100][100];
  //  QColor ExtColorShd[num_sets][num_points];
  // bool hasxlabels;
  //  QPointArray volpoly( 4 );

  int init();
};
#endif // KCHARTENGINE_H
