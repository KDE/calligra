#ifndef KIVIOBIRDEYEPANEL_H
#define KIVIOBIRDEYEPANEL_H
#include "kivio_birdeye_panel_base.h"

class QPixmap;

class KivioView;
class KivioCanvas;
class KivioDoc;
class KivioPage;
class KAction;
class KoZoomHandler;

class KivioBirdEyePanel : public KivioBirdEyePanelBase
{ Q_OBJECT
public:
  KivioBirdEyePanel(KivioView* view, QWidget* parent=0, const char* name=0);
  ~KivioBirdEyePanel();

  bool eventFilter(QObject*, QEvent*);

public slots:
  void show();

  void zoomChanged(int);
  void zoomMinus();
  void zoomPlus();

protected slots:
  void updateVisibleArea();
  void canvasZoomChanged();
  void slotUpdateView(KivioPage*);
  void togglePageBorder(bool);
  void doAutoResizeMin();
  void doAutoResizeMax();

protected:
  void updateView();
  void handleMouseMove(QPoint);
  void handleMouseMoveAction(QPoint);
  void handleMousePress(QPoint);

private:
  KivioView* m_pView;
  KivioCanvas* m_pCanvas;
  KivioDoc* m_pDoc;

  KAction* zoomIn;
  KAction* zoomOut;
  QPixmap* m_buffer;
  bool m_bShowPageBorders;

  QSize cMinSize;
  QSize cMaxSize;

  QRect varea;
  AlignmentFlags apos;
  bool handlePress;
  QPoint lastPos;
  KoZoomHandler* m_zoomHandler;
};

#endif
