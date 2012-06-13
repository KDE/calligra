#ifndef PIVOTSUBTOTALS_H
#define PIVOTSUBTOTALS_H

#include <QDialog>

namespace Ui {
    class PivotSubTotals;
}

class PivotSubTotals : public QDialog
{
    Q_OBJECT

public:
    explicit PivotSubTotals(QWidget *parent = 0);
    ~PivotSubTotals();

private:
    Ui::PivotSubTotals *ui;
};

#endif // PIVOTSUBTOTALS_H
