#ifndef __kspread_dlg_cons__
#define __kspread_dlg_cons__

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstrlist.h>
#include <qlistbox.h>

class KSpreadView;

class KSpreadConsolidate : public QDialog
{
  Q_OBJECT
public:
  KSpreadConsolidate( KSpreadView* parent, const char* name );
  
  QStrList refs();
  
public slots:
  void slotOk();
  void slotClose();
  void slotAdd();
  void slotRemove();
  
protected:
  KSpreadView* m_pView;
  
  QLineEdit* m_pRef;
  QListBox* m_pRefs;
  QComboBox* m_pFunction;
  QPushButton* m_pOk;
  QPushButton* m_pClose;
  QPushButton* m_pAdd;
  QPushButton* m_pRemove;

  int m_idSumme;
};

#endif
