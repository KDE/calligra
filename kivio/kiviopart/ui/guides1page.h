#ifndef GUIDESONEPOSITIONPAGE_H
#define GUIDESONEPOSITIONPAGE_H

#include "guides1pagebase.h"

class KivioView;
class KivioPage;
class KivioCanvas;
class GuidesSetupDialog;
class KivioGuideLineData;

class GuidesOnePositionPage : public GuidesOnePositionPageBase
{ Q_OBJECT
public:
  GuidesOnePositionPage(Orientation o, KivioView* view, QWidget* parent=0, const char* name=0);
  ~GuidesOnePositionPage();

  bool eventFilter(QObject*, QEvent*);

protected:
  void setCurrent(KivioGuideLineData*);
  void updateListViewColumn();
  void updateListView(bool);

public slots:
  void slotUnitChanged(int);
  void selectionChanged();

  void slotAddButton();
  void slotMoveButton();
  void slotMoveByButton();
  void slotDeleteButton();
  void slotDeleteAllButton();
  void slotSelectAllButton();
  void slotClearSelectionButton();

  void slotCurrentChanged(QListViewItem*);

signals:
  void change(bool);

private:
  KivioPage* m_pPage;
  KivioCanvas* m_pCanvas;
  Orientation orientation;
};

#endif
