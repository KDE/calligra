#ifndef GRIDSETUPDIALOG_H
#define GRIDSETUPDIALOG_H

#include "gridsetupdialogbase.h"

class KivioView;
class KivioDoc;

class GridSetupDialog : public GridSetupDialogBase
{ Q_OBJECT
public:
  GridSetupDialog(KivioView* view, QWidget* parent=0, const char* name=0);
  ~GridSetupDialog();

public slots:
  void apply(QWidget*);

private:
  KivioDoc* m_pDoc;
};

#endif
