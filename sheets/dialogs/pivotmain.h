#ifndef PIVOTMAIN_H
#define PIVOTMAIN_H

#include <QDialog>

namespace Ui {
    class PivotMain;
}

class PivotMain : public QDialog
{
    Q_OBJECT

public:
    explicit PivotMain(QWidget *parent = 0);
    ~PivotMain();

private slots:

    void on_Options_clicked();

    void on_AddFilter_clicked();

private:
    Ui::PivotMain *ui;
};

#endif // PIVOTMAIN_H
