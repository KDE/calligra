#include "kivio_guidelines.h"
#include "kivio_canvas.h"
#include "kivio_page.h"
#include "kivio_doc.h"
#include "kivio_map.h"
#include "kivio_common.h"

#include <qpainter.h>
#include <qwmatrix.h>
#include <kdebug.h>
#include <koPoint.h>

QPixmap* KivioGuideLines::vGuideLines = 0;
QPixmap* KivioGuideLines::hGuideLines = 0;
QPixmap* KivioGuideLines::pattern     = 0;

QPixmap* KivioGuideLines::vGuideLinesSelected = 0;
QPixmap* KivioGuideLines::hGuideLinesSelected = 0;
QPixmap* KivioGuideLines::patternSelected     = 0;

QSize KivioGuideLines::size;

KivioGuideLineData::KivioGuideLineData(Qt::Orientation o)
{
  hasBuffer = false;
  pos = 0.0;
  orient = o;
  selected = false;
}

KivioGuideLineData::KivioGuideLineData(KivioGuideLineData& d)
{
  hasBuffer = false;
  pos = d.position();
  orient = d.orientation();
  selected = false;
}

void KivioGuideLineData::setOrientation(Qt::Orientation o)
{
  orient = o;
}

void KivioGuideLineData::setPosition(double p)
{
  pos = p;
}

/**************************************************************************/

KivioGuideLines::KivioGuideLines(KivioPage* page)
{
  if (!vGuideLines) {
    static const char* const image_data[] = {
    "15 1 2 1",
    "  c None",
    "# c blue",
    "## ## ## ## ## "};
    vGuideLines = new QPixmap();
    hGuideLines = new QPixmap();
    pattern = new QPixmap((const char**)image_data);

    static const char* const image_data_selected[] = {
    "15 1 2 1",
    "  c None",
    "# c red",
    "## ## ## ## ## "};
    vGuideLinesSelected = new QPixmap();
    hGuideLinesSelected = new QPixmap();
    patternSelected = new QPixmap((const char**)image_data_selected);
  }

  m_pPage = page;

  lines.setAutoDelete(true);
}

KivioGuideLines::~KivioGuideLines()
{
  unselectAll();
}

void KivioGuideLines::resize(QSize s,KivioDoc* doc)
{
  resizeLinesPixmap(s,vGuideLines,hGuideLines,pattern);
  resizeLinesPixmap(s,vGuideLinesSelected,hGuideLinesSelected,patternSelected);

  size = s;

  KivioMap* map = doc->map();
  QPtrList<KivioPage> pageList = map->pageList();
  for (KivioPage* page = pageList.first(); page; page = pageList.next())
    page->guideLines()->resize();
}

void KivioGuideLines::resizeLinesPixmap(QSize s, QPixmap* vLine, QPixmap* hLine, QPixmap* linePattern)
{
  if(!vLine || !hLine || !linePattern) {
    return;
  }

  int d = linePattern->width();
  hLine->resize(s.width() + d,1);
  vLine->resize(1,s.height() + d);

  QPainter p;
  p.begin(hLine);
  p.drawTiledPixmap(0,0,hLine->width(),1,*linePattern);
  p.end();

  const QBitmap* mask = linePattern->mask();
  QBitmap* hmask = new QBitmap(hLine->size());
  p.begin(hmask);
  p.drawTiledPixmap(0,0,hmask->width(),1,*mask);
  p.end();
  hLine->setMask(*hmask);

  delete hmask;

  QWMatrix m;
  m.rotate(90);
  QPixmap rpattern = linePattern->xForm(m);

  p.begin(vLine);
  p.drawTiledPixmap(0,0,1,vLine->height(),rpattern);
  p.end();

  const QBitmap* rmask = rpattern.mask();
  QBitmap* vmask = new QBitmap(vLine->size());
  p.begin(vmask);
  p.drawTiledPixmap(0,0,1,vmask->height(),*rmask);
  p.end();
  vLine->setMask(*vmask);
  delete vmask;
}

void KivioGuideLines::resize()
{
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next()) {
    if (g->orientation() == Qt::Vertical)
      g->buffer.resize(1,size.height());
    else
      g->buffer.resize(size.width(),1);
    g->hasBuffer = false;
  }
}

void KivioGuideLines::erase(QPaintDevice* buffer, KivioCanvas* c)
{
  KoPoint p;
  for (KivioGuideLineData* g = lines.last(); g; g = lines.prev()) {
    if (g->hasBuffer){
      if (g->orientation() == Qt::Vertical) {
        p.setCoords(g->position(), 0);
        int x = c->mapToScreen(p).x();
        if (x >= 0 && x < c->width() ) {
          bitBlt(buffer, x, 0, &g->buffer);
        }
      } else {
        p.setCoords(0, g->position());
        int y = c->mapToScreen(p).y();
        if (y >= 0 && y < c->height() ) {
          bitBlt(buffer, 0, y, &g->buffer);
        }
      }
    }
    g->hasBuffer = false;
  }
}

void KivioGuideLines::paint(QPaintDevice* buffer, KivioCanvas* c)
{
  KoPoint p;
  int d = pattern->width();

  int dx = c->xOffset() % d;
  int dy = c->yOffset() % d;

  if (c->xOffset() < 0 ) {
    dx += d;
  }

  if (c->yOffset() < 0 ) {
    dy += d;
  }

  for (KivioGuideLineData* g = lines.first(); g; g = lines.next()) {
    if (g->orientation() == Qt::Vertical) {
      p.setCoords(g->position(), 0);
      int x = c->mapToScreen(p).x();

      if (x >= 0 && x < c->width() ) {
        bitBlt(&g->buffer,0,0,buffer,x,0,1,g->buffer.height());
        bitBlt(buffer,x,0,g->isSelected() ? vGuideLinesSelected:vGuideLines,0,dy,1,c->height());
        g->hasBuffer = true;
      } else {
        g->hasBuffer = false;
      }
    } else {
      p.setCoords(0, g->position());
      int y = c->mapToScreen(p).y();
      if (y >= 0 && y < c->height() ) {
        bitBlt(&g->buffer, 0, 0, buffer, 0, y, g->buffer.width(), 1);
        bitBlt(buffer,0,y,g->isSelected() ? hGuideLinesSelected:hGuideLines,dx,0,c->width(),1);
        g->hasBuffer = true;
      } else {
        g->hasBuffer = false;
      }
    }
  }
}

KivioGuideLineData* KivioGuideLines::add(double pos, Qt::Orientation o)
{
  KivioGuideLineData* gd;
  gd = new KivioGuideLineData(o);
  gd->pos = pos;
  lines.append(gd);

  if (o == Qt::Vertical)
    gd->buffer.resize(1,size.height());
  else
    gd->buffer.resize(size.width(),1);

  return gd;
}

void KivioGuideLines::remove(KivioGuideLineData* gd)
{
  lines.remove(gd);
}

KivioGuideLineData* KivioGuideLines::find(double x, double y, double d)
{
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next()) {
    if (g->orientation() == Qt::Horizontal && QABS(g->position()-y) < d )
      return g;
    if (g->orientation() == Qt::Vertical && QABS(g->position()-x) < d )
      return g;
  }

  return 0;
}
KivioGuideLineData* KivioGuideLines::findHorizontal(double y, double d)
{
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next()) {
    if (g->orientation() == Qt::Horizontal && QABS(g->position()-y) < d )
      return g;
  }

  return 0;
}

KivioGuideLineData* KivioGuideLines::findVertical(double x, double d)
{
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next()) {
    if (g->orientation() == Qt::Vertical && QABS(g->position()-x) < d )
      return g;
  }

  return 0;
}

void KivioGuideLines::select(KivioGuideLineData* gd)
{
  gd->selected = true;
  slines.append(gd);
}

void KivioGuideLines::unselect(KivioGuideLineData* gd)
{
  gd->selected = false;
  slines.remove(gd);
}

void KivioGuideLines::unselectAll()
{
  while (slines.first())
    unselect(slines.first());
}

void KivioGuideLines::selectAll()
{
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next())
    select(g);
}

bool KivioGuideLines::hasSelected()
{
  return slines.count() > 0;
}

void KivioGuideLines::moveSelectedByX(double d)
{
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next())
    if (g->isSelected() && g->orientation() == Qt::Vertical)
      g->pos = g->pos + d;
}

void KivioGuideLines::moveSelectedByY(double d)
{
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next())
    if (g->isSelected() && g->orientation() == Qt::Horizontal)
      g->pos = g->pos + d;
}

int KivioGuideLines::selectedCount()
{
  return slines.count();
}

void KivioGuideLines::removeSelected()
{
  QPtrList<KivioGuideLineData> rlines;
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next())
    if (g->isSelected())
      rlines.append(g);

  for (KivioGuideLineData* g = rlines.first(); g; g = rlines.next())
      remove(g);
}

void KivioGuideLines::save(QDomElement& element)
{
  for (KivioGuideLineData* g = lines.first(); g; g = lines.next()) {
    QDomElement e = element.ownerDocument().createElement("Guideline");
    element.appendChild(e);
    XmlWriteDouble(e, "pos", g->pos);
    XmlWriteInt(e, "orient", (int)g->orient);
  }
}

void KivioGuideLines::load(const QDomElement& element)
{
  slines.clear();
  lines.clear();

  QDomElement e = element.firstChild().toElement();
  for( ; !e.isNull(); e = e.nextSibling().toElement() )
  {
    double pos = XmlReadDouble(e, "pos", 0.0);
    Qt::Orientation orient = (Qt::Orientation)XmlReadInt(e, "orient", 0);
    add(pos,orient);
  }
}
