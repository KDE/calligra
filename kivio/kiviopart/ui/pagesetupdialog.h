#ifndef PAGESETUPDIALOG_H
#define PAGESETUPDIALOG_H

#include "pagesetupdialogbase.h"
#include "tkpagelayout.h"

class KivioPage;
class KivioView;

class PageSetupDialog : public PageSetupDialogBase
{ Q_OBJECT
public:
  PageSetupDialog(KivioView* view, QWidget* parent=0, const char* name=0);
  ~PageSetupDialog();

public slots:
  void apply(QWidget*);

protected slots:
  void update();

private:
  KivioPage* m_pPage;
};

#endif
