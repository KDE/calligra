#ifndef __koFrame_h__
#define __koFrame_h__

#include <qwidget.h>
#include <qpoint.h>

class KoView;
class KoFramePrivate;

class KoFrame : public QWidget
{
  Q_OBJECT
public:
  enum State { Inactive, Selected, Active };

  KoFrame( QWidget *parent, const char *name = 0 );
  virtual ~KoFrame();

  virtual void setView( KoView *view );
  virtual KoView *view() const;

  virtual void setState( State s );
  virtual State state() const;

  virtual int leftBorder() const;
  virtual int rightBorder() const;
  virtual int topBorder() const;
  virtual int bottomBorder() const;

  virtual int border() const;

protected:
  virtual void paintEvent( QPaintEvent* );
  virtual void mousePressEvent( QMouseEvent* );
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
  virtual void resizeEvent( QResizeEvent* );
  virtual bool eventFilter( QObject*, QEvent* );

private:
  KoFramePrivate *d;
};

#endif
