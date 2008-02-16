#ifndef KCHART_NEW_AXIS_DIALOG_H
#define KCHART_NEW_AXIS_DIALOG_H

#include "ui_NewAxisDialog.h"

namespace KChart {

class NewAxisDialog : public QDialog, public Ui::NewAxisDialog
{
public:
    NewAxisDialog( QWidget *parent = 0 );
    ~NewAxisDialog();
};

} // Namespace KChart

#endif // KCHART_NEW_AXIS_DIALOG
