#ifndef KIVIOBIRDEYEPANEL_H
#define KIVIOBIRDEYEPANEL_H
#include "kivio_birdeye_panel_base.h"

class QPixmap;

class KivioView;
class KivioCanvas;
class KivioDoc;
class KivioPage;

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
  void canvasZoomChanged(int);
  void slotUpdateView(KivioPage*);
  void togglePageBorder(bool);
  void togglePageOnly(bool);
  void doAutoResizeMin();
  void doAutoResizeMax();

protected:
  void updateView();

private:
  KivioView* m_pView;
  KivioCanvas* m_pCanvas;
  KivioDoc* m_pDoc;

  QPixmap* m_buffer;
  bool m_bShowPageBorders;
  bool m_bPageOnly;

  QSize cMinSize;
  QSize cMaxSize;
};

#endif
