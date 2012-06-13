#include "pivotmain.h"
#include "ui_pivotmain.h"
#include "pivotoptions.h"
#include "pivotfilters.h"


PivotMain::PivotMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PivotMain)
{
    ui->setupUi(this);
    ui->TotalRows->setChecked(true);
    ui->TotalColumns->setChecked(true);

}

PivotMain::~PivotMain()
{
    delete ui;
}

void PivotMain::on_Options_clicked()
{
    PivotOptions pOptions;
    pOptions.setModal(true);
    pOptions.exec();
}

void PivotMain::on_AddFilter_clicked()
{
    if(ui->AddFilter->isChecked())
    {
        PivotFilters pFilters;
        pFilters.setModal(true);
        pFilters.exec();
    }
}
