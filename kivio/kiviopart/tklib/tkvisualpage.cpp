#include "tkvisualpage.h"

#include <kpixmapeffect.h>
#include <qpainter.h>

TKVisualPage::TKVisualPage(QWidget* parent, const char* name)
: QWidget(parent, name, WResizeNoErase | WRepaintNoErase)
{
  setBackgroundMode(NoBackground);
}

TKVisualPage::~TKVisualPage()
{
}

void TKVisualPage::paintEvent(QPaintEvent*)
{
  back.fill(white);
  if (!buffer.isNull())
    bitBlt(&back,(width()-buffer.width())/2,(height()-buffer.height())/2,&buffer);

  QPainter p;
  p.begin(&back, this);
  p.setPen(QColor(100, 100, 100));
  p.drawText(5, QFontMetrics(font()).lineSpacing() + 5, QString("zoom: 1/%1").arg(z));
  p.end();

  bitBlt(this, 0, 0, &back);
}

void TKVisualPage::resizeEvent(QResizeEvent*)
{
  back.resize(size());
  update();
}

void TKVisualPage::update()
{
  int k = 6;
  float mins = (float)QMIN(width(),height());
  float maxs = QMAX(psize.width(),psize.height());

  z = (((int)(maxs/mins))/k+1)*k;

  int w = (int)(psize.width()/z);
  int h = (int)(psize.height()/z);
  QSize s(w, h);

  int l = (int)(margin.x()/z);
  int r = (int)(margin.width()/z);
  int t = (int)(margin.y()/z);
  int b = (int)(margin.height()/z);

  buffer = makeShadow(s);

  QColor ca(white);
  QColor cb(QColor(255,255,240));

  KPixmap gradient(s);
  KPixmapEffect::gradient(gradient, ca, cb, KPixmapEffect::PyramidGradient);
  bitBlt(&buffer,0,0,&gradient);

  QPainter p;
  p.begin(&buffer);
  p.setPen(black);
  p.drawRect(QRect(QPoint(0,0),s));
  p.setPen(darkBlue);
  p.drawRect(l,t,s.width()-l-r,s.height()-t-b);
  p.end();
}

void TKVisualPage::setupPage(QSize s, int l, int r, int t, int b)
{
  psize = s;
  margin = QRect(l, t, r, b);
  update();
  repaint();
}

QPixmap TKVisualPage::makeShadow(QSize s)
{
  static const char* const shadow_xpm[] = {
  "16 16 25 1",
  " 	c None",
  ".	c #FFFFFF",
  "+	c #FEFEFE",
  "@	c #FDFDFD",
  "#	c #FBFBFB",
  "$	c #F8F8F8",
  "%	c #F7F7F7",
  "&	c #FCFCFC",
  "*	c #F2F2F2",
  "=	c #EEEEEE",
  "-	c #ECECEC",
  ";	c #F6F6F6",
  ">	c #E4E4E4",
  ",	c #DCDCDC",
  "'	c #D8D8D8",
  ")	c #E0E0E0",
  "!	c #D1D1D1",
  "~	c #C4C4C4",
  "{	c #BBBBBB",
  "]	c #BABABA",
  "^	c #A6A6A6",
  "/	c #989898",
  "(	c #898989",
  "_	c #777777",
  ":	c #646464",
  "......+@@+......",
  "....+#$%%$#+....",
  "..+&$*=--=*$&+..",
  "..&;=>,'',>=;&..",
  ".+$=)!~{{~!)=$+.",
  ".#*>!]^//^]!>*#.",
  "+$=,~^(__(^~,=$+",
  "@%-'{/_::_/{'-%@",
  "@%-'{/_::_/{'-%@",
  "+$=,~^(__(^~,=$+",
  ".#*>!]^//^]!>*#.",
  ".+$=)!~{{~!)=$+.",
  "..&;=>,'',>=;&..",
  "..+&$*=--=*$&+..",
  "....+#$%%$#+....",
  "......+@@+......"};

  QPixmap backgroundPix(s.width()+8, s.height()+8);
  QPixmap backgroundTemplate((const char**)shadow_xpm);

  int dx = 8;
  int dy = 8;

  int sw = backgroundPix.width();
  int sh = backgroundPix.height();
  int h = backgroundTemplate.height();
  int w = backgroundTemplate.width();

  int tw;
  int th;

  /* paint top-left segment*/
  bitBlt( &backgroundPix,0,0,&backgroundTemplate,0,0,dx,dy );

  /* paint left segment*/
  tw = dx;
  th = QMAX(h-2*dy,1);
  for ( int y = dy; y < sh-dy; y+=th )
    bitBlt( &backgroundPix,0,y,&backgroundTemplate,0,dy,tw,th );

  /* paint bottom-left segment*/
  bitBlt( &backgroundPix,0,sh-dy,&backgroundTemplate,0,h-dy,dx,dy );

  /* paint top segment and first line*/
  tw = QMAX(w-2*dx,1);
  th = h-dy;
  for ( int x = dx; x < sw-dx; x +=tw )
    bitBlt( &backgroundPix,x,0,&backgroundTemplate,dx,0,tw,th );

  /* repeat first line*/
  tw = sw-2*dx;
  th = QMAX(h-2*dy,1);
  for ( int y1 = h-dy; y1 < sh; y1+=th )
    bitBlt( &backgroundPix,dx,y1,&backgroundPix,dx,dy,tw,th );

  /* paint bottom segment*/
  tw = QMAX(w-2*dx,1);
  th = dy;
  for ( int x1 = dx; x1 < sw-dx; x1 +=tw )
    bitBlt( &backgroundPix,x1,sh-dy,&backgroundTemplate,dx,h-dy,tw,th );

  /* paint top-right segment*/
  bitBlt( &backgroundPix,sw-dx,0,&backgroundTemplate,w-dx,0,dx,dy );

  /* paint right segment*/
  tw = dx;
  th = QMAX(h-2*dy,1);
  for ( int y2 = dy; y2 < sh-dy; y2+=th )
    bitBlt( &backgroundPix,sw-dx,y2,&backgroundTemplate,w-dx,dy,tw,th );

  /* paint bottom-right segment*/
  bitBlt( &backgroundPix,sw-dx,sh-dy,&backgroundTemplate,w-dx,h-dy,dx,dy );

  return backgroundPix;
}

QSize TKVisualPage::sizeHint() const
{
  return QSize(height()*2/3, height());
}

#include "tkvisualpage.moc"
