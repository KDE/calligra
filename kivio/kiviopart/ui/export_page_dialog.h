#ifndef EXPORT_PAGE_DIALOG_H
#define EXPORT_PAGE_DIALOG_H

#include <kdialogbase.h>

class ExportPageDialogBase;

class ExportPageDialog : public KDialogBase
{
  Q_OBJECT
  public:
    ExportPageDialog(QWidget* parent=0, const char* name=0);

    int quality();
    int border();

    bool crop();
    bool fullPage();
    bool selectedStencils();
  
  protected slots:
    void slotOk();

  private:
    ExportPageDialogBase* m_view;
};

#endif
