#ifndef GUIDESTWOPOSITIONPAGE_H
#define GUIDESTWOPOSITIONPAGE_H

#include "guides2pagebase.h"

class KivioView;
class KivioPage;
class KivioCanvas;
class GuidesSetupDialog;
class KivioGuideLineData;

class GuidesTwoPositionPage : public GuidesTwoPositionPageBase
{ Q_OBJECT
public:
  GuidesTwoPositionPage(KivioView* view, QWidget* parent=0, const char* name=0);
  ~GuidesTwoPositionPage();

  bool eventFilter(QObject*, QEvent*);

public slots:
  void apply(QWidget*);

protected:
  void setCurrent(KivioGuideLineData*);
  void updateListViewColumn();
  void updateListView(bool);
  void updateButton();
protected slots:
  void slotUnitChanged(int);
  void selectionChanged();

  void slotMoveButton();
  void slotMoveByButton();
  void slotDeleteButton();
  void slotDeleteAllButton();
  void slotSelectAllButton();
  void slotClearSelectionButton();

  void slotCurrentChanged(QListViewItem*);

private:
  KivioPage* m_pPage;
  KivioCanvas* m_pCanvas;
};

#endif
