#ifndef DCMIMPORTDIALOG_H
#define DCMIMPORTDIALOG_H

#include <QDialog>

namespace Ui {
    class DCMImportDialog;
}

class DCMImportDialog : public QDialog {
    Q_OBJECT
public:
    DCMImportDialog(QWidget *parent = 0);
    ~DCMImportDialog();

	bool getDS();
	bool getIRC();
	bool getICIE();
	bool getSICI();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DCMImportDialog *ui;
};

#endif // DCMIMPORTDIALOG_H
