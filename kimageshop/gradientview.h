#ifndef __GRADIENTVIEW_H__
#define __GRADIENTVIEW_H__

#include <qlist.h>
#include <qwidget.h>
#include <qcolor.h>

#warning "TODO: convert QColor -> KColor"

struct GradientItem
{
  QColor leftColor;
  QColor rightColor;

  double middle;
  double right;
};

class GradientView : public QWidget
{
  Q_OBJECT

public:

  GradientView( QWidget *_parent = 0 , const char *_name = 0 );
  ~GradientView();

  void addItem( QColor, QColor, float, float );

protected:

  virtual void paintEvent( QPaintEvent *_event );

private:

  QList<GradientItem> m_lstGradientItems;
};

#endif

