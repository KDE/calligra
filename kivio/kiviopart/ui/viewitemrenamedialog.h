#ifndef VIEWITEMRENAMEDIALOG_H
#define VIEWITEMRENAMEDIALOG_H
#include "enternamedialogbase.h"

class ViewItemRenameDialog : public EnterNameDialogBase
{ Q_OBJECT
public:
  ViewItemRenameDialog(QWidget* parent=0, const char* name=0);
  ~ViewItemRenameDialog();

  void setText(const QString&);
  QString text();

protected slots:
  void slotOk();
};

#endif

