#ifndef EXPORT_PAGE_DIALOG_H
#define EXPORT_PAGE_DIALOG_H

#include "export_page_dialog_base.h"


class ExportPageDialog : public ExportPageDialogBase
{ Q_OBJECT
public:
    ExportPageDialog(QWidget* parent=0, const char* name=0);
    ~ExportPageDialog();

    int quality();
    int border();

    bool crop();
    bool fullPage();
    bool selectedStencils();
};

#endif
