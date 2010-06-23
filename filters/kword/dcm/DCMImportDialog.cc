#include "DCMImportDialog.h"
#include "ui_DCMImportDialog.h"

DCMImportDialog::DCMImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DCMImportDialog)
{
    ui->setupUi(this);
}

DCMImportDialog::~DCMImportDialog()
{
    delete ui;
}

void DCMImportDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool DCMImportDialog::getDS() {
	return ui->readDS->checkState();
}

bool DCMImportDialog::getIRC() {
	return ui->readIRC->checkState();
}

bool DCMImportDialog::getICIE() {
	return ui->readICIE->checkState();
}

bool DCMImportDialog::getSICI() {
	return ui->readSICI->checkState();
}
