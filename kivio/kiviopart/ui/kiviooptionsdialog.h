#ifndef KIVIOOPTIONSDIALOG_H
#define KIVIOOPTIONSDIALOG_H

#include "kiviooptionsdialogbase.h"

class KivioView;
class QListViewItem;

class KivioOptionsDialog : public KivioOptionsDialogBase
{ Q_OBJECT
public:
  enum Pages {
    PageOption       = 0,
    PageSize         = 1,
    PageGrid         = 2,
    Guides           = 3,
    GuidesHorizontal = 4,
    GuidesVertical   = 5,
    GuidesAll        = 6,
    StencilsBar      = 7
  };

  KivioOptionsDialog(KivioView* view, Pages startPage = PageOption, QWidget* parent=0, const char* name=0);
  ~KivioOptionsDialog();

protected slots:
  void apply();
  void slotCurrentChanged(QListViewItem*);
  void slotOk();

signals:
  void sig_apply(QWidget*);

private:
  KivioView* m_pView;
};

#endif
