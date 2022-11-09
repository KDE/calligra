#include "autocorrectiondialog.h"

AutoCorrectionDialog::AutoCorrectionDialog(AutoCorrection *autocorrection, QWidget *parent)
    : QDialog{parent}
{
    ui.setupUi(this);
    ui.widget->setAutoCorrection(autocorrection);

    connect(this, &QDialog::accepted, this, [this]() {
        ui.widget->writeConfig();
    });
}
