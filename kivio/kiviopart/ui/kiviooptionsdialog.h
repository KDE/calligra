#ifndef KIVIOOPTIONSDIALOG_H
#define KIVIOOPTIONSDIALOG_H

#include "kiviooptionsdialogbase.h"

class KivioView;
class QListViewItem;

class KivioOptionsDialog : public KivioOptionsDialogBase
{ Q_OBJECT
public:
  KivioOptionsDialog(KivioView* view, QWidget* parent=0, const char* name=0);
  ~KivioOptionsDialog();

protected slots:
  void apply();
  void slotCurrentChanged(QListViewItem*);
  void slotOk();

private:
  KivioView* m_pView;
};

#endif
