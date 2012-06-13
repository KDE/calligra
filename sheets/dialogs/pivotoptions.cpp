#include "pivotoptions.h"
#include "ui_pivotoptions.h"

PivotOptions::PivotOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PivotOptions)
{
    ui->setupUi(this);
}

PivotOptions::~PivotOptions()
{
    delete ui;
}
