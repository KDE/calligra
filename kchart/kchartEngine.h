class kchartEngine {
public:
  KChartParameters* params;
  KChartData* data;
  QPainter *p;
  int imagewidth;
  int imageheight;  
  int doLabels();
  int minmaxValues( int num_points,
		  int num_sets,
		  float *uvol,
		  float &highest, 
		  float &lowest,
		  float &vhighest,
		  float &vlowest);
  int out_graph();
  
  int compute_yintervals();
  void titleText();
  void drawBorder();
  void drawAnnotation();
  void drawThumbnails();
  void drawScatter();
  void drawBackgroundImage();
  void prepareColors();
  void drawGridAndLabels(bool do_ylbl_fractions);

  inline bool CELLEXISTS( int row, int col ) {
    return  data->cell(row,col).exists;
  };
  inline double CELLVALUE(int row, int col) {
    return data->cell(row,col).value.doubleValue();
  }
  
private:
  int num_sets;
  int num_hlc_sets;
  int num_points;
  int graphwidth;
  int grapheight;
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
  
  /* args */
  int			setno;				// affects PX() and PY()
  // PENDING(kalle) Get rid of this
  float		*uvol;
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
  bool hasxlabels;
  //  QPointArray volpoly( 4 );

  int init();
};
