#ifndef NEWAXISDIALOG_H
#define NEWAXISDIALOG_H

#include "ui_NewAxisDialog.h"

class NewAxisDialog : public QDialog, public Ui::NewAxisDialog
{
public:
    NewAxisDialog( QWidget *parent = 0 );
    ~NewAxisDialog();
};

#endif
