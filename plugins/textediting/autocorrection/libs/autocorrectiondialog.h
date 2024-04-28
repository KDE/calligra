#ifndef AUTOCORRECTIONDIALOG_H
#define AUTOCORRECTIONDIALOG_H

#include "autocorrection_export.h"

#include <QWidget>
#include <ui_autocorrectiondialog.h>

class AUTOCORRECTION_EXPORT AutoCorrectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AutoCorrectionDialog(AutoCorrection *autocorrection, QWidget *parent = nullptr);

private:
    Ui::AutoCorrectionDialog ui;
};

#endif // AUTOCORRECTIONDIALOG_H
