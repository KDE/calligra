#ifndef __edithtmldlg_h__
#define __edithtmldlg_h__

#include <qdialog.h>
#include <qmultilinedit.h>

class HTMLEditDlg : public QDialog
{
  Q_OBJECT
public:
  HTMLEditDlg(QString data);
  ~HTMLEditDlg();
  
  QString getText();
  
private:
  QMultiLineEdit *e;
};

#endif