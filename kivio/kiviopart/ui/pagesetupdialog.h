#ifndef PAGESETUPDIALOG_H
#define PAGESETUPDIALOG_H

#include "pagesetupdialogbase.h"

class KivioView;

class PageSetupDialog : public PageSetupDialogBase
{ Q_OBJECT
public:
  PageSetupDialog(KivioView* view, QWidget* parent=0, const char* name=0);
  ~PageSetupDialog();

protected slots:
  void update();
};

#endif
