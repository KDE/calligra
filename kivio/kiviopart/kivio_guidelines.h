#ifndef KIVIO_GUIDELINES_DATA_H
#define KIVIO_GUIDELINES_DATA_H

#include <qdom.h>
#include <qbitmap.h>
#include <qlist.h>

class KivioPage;
class KivioCanvas;
class KivioDoc;

class KivioGuideLineData
{
friend class KivioGuideLines;
public:
  KivioGuideLineData(Qt::Orientation);
  ~KivioGuideLineData();

  Qt::Orientation orientation() { return orient; }
  double position(){ return pos; }
  bool isSelected() { return selected; }

private:
  double pos;
  QPixmap buffer;
  bool hasBuffer;
  bool selected;
  Qt::Orientation orient;
};
/*****************************************************************/
class KivioGuidesList : public QList<KivioGuideLineData>
{
public:
  KivioGuidesList() {};

protected:
  int compareItems(QCollection::Item i1, QCollection::Item i2)
  {
    KivioGuideLineData* s1 = (KivioGuideLineData*)i1;
    KivioGuideLineData* s2 = (KivioGuideLineData*)i2;
    if ((s1->orientation() == Qt::Vertical) && (s2->orientation() == Qt::Horizontal))
      return -1;
    if ((s1->orientation() == Qt::Horizontal) && (s2->orientation() == Qt::Vertical))
      return 1;

    if (s1->position() > s2->position())
      return -1;

    if (s1->position() < s2->position())
      return 1;

    return 0;
  }
};
/*****************************************************************/
class KivioGuideLines
{
public:
  KivioGuideLines(KivioPage*);
  ~KivioGuideLines();

  void save(QDomElement&);
  void load(const QDomElement&);

  KivioGuidesList guides() { return lines; }

  KivioGuideLineData* add(double, Qt::Orientation);
  void remove(KivioGuideLineData*);
  KivioGuideLineData* find(double x, double y, double d);

  bool hasSelected();
  int selectedCount();

  void select(KivioGuideLineData*);
  void unselect(KivioGuideLineData*);
  void selectAll();
  void unselectAll();

  void resize();

  void erase(QPaintDevice* buffer, KivioCanvas*);
  void paint(QPaintDevice* buffer, KivioCanvas*);

  void moveSelectedByX(double);
  void moveSelectedByY(double);

  void removeSelected();

  static void resize(QSize,KivioDoc*);

protected:
  static void resizeLinesPixmap(QSize, QPixmap* vLine, QPixmap* hLine, QPixmap* linePattern);

private:
  KivioPage* m_pPage;

  static QSize size;

  static QPixmap* vGuideLines;
  static QPixmap* hGuideLines;
  static QPixmap* pattern;

  static QPixmap* vGuideLinesSelected;
  static QPixmap* hGuideLinesSelected;
  static QPixmap* patternSelected;

  KivioGuidesList lines;
  KivioGuidesList slines;
};

#endif
