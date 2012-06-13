#include "pivotfilters.h"
#include "ui_pivotfilters.h"

PivotFilters::PivotFilters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PivotFilters)
{
    ui->setupUi(this);
}

PivotFilters::~PivotFilters()
{
    delete ui;
}
