#ifndef PAGEOPTIONSDIALOG_H
#define PAGEOPTIONSDIALOG_H

#include "pageoptionsdialogbase.h"

class KivioView;

class PageOptionsDialog : public PageOptionsDialogBase
{ Q_OBJECT
public:
  PageOptionsDialog(KivioView* view, QWidget* parent=0, const char* name=0);
  ~PageOptionsDialog();

public slots:
  void apply(QWidget*);

private:
  KivioView* m_pView;
};

#endif
