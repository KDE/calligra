#ifndef TKVISUALPAGE_H
#define TKVISUALPAGE_H

#include <qwidget.h>
#include <qpixmap.h>

class TKVisualPage : public QWidget
{Q_OBJECT

public:
	TKVisualPage(QWidget* parent=0, const char* name=0);
	~TKVisualPage();

  void setupPage(QSize, int, int, int, int);
	
  QSize sizeHint() const;

protected:
  void paintEvent(QPaintEvent*);
  void resizeEvent(QResizeEvent*);

  QPixmap makeShadow(QSize);
  void update();

private:
  QPixmap buffer;
  QPixmap back;

  QSize psize;
  QRect margin;
  int z;
};

#endif

