#ifndef GUIDESSETUPDIALOG_H
#define GUIDESSETUPDIALOG_H

#include "guidessetupdialogbase.h"
#include "tkunits.h"

#include <qlistview.h>

class KivioView;
class KivioGuideLineData;

class GuidesListViewItem : public QListViewItem
{
public:
  GuidesListViewItem(QListView* parent, KivioGuideLineData*, bool isTwoColumn);
  ~GuidesListViewItem();

  void setUnit(int u);
  KivioGuideLineData* guideData() { return d; }

private:
  KivioGuideLineData* d;
  bool twoColumn;
};
/****************************************************************************************/
class GuidesSetupDialog : public GuidesSetupDialogBase
{ Q_OBJECT
public:
  GuidesSetupDialog(KivioView* view, QWidget* parent=0, const char* name=0);
  ~GuidesSetupDialog();

public slots:
  void apply(QWidget*);
};

#endif
