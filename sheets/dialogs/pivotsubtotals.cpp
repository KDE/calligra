#include "pivotsubtotals.h"
#include "ui_pivotsubtotals.h"

PivotSubTotals::PivotSubTotals(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PivotSubTotals)
{
    ui->setupUi(this);
}

PivotSubTotals::~PivotSubTotals()
{
    delete ui;
}
