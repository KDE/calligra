#include "pivot.h"
#include "ui_pivot.h"
#include "pivotmain.h"
#include "ui_pivotmain.h"

Pivot::Pivot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Pivot)
{
    ui->setupUi(this);
    ui->Current->setChecked(true);
}

Pivot::~Pivot()
{
    delete ui;
}

void Pivot::on_Ok_clicked()
{
    if(ui->Current->isChecked())
    {
        PivotMain pMain;
        pMain.setModal(true);
        pMain.exec();
    }
}
