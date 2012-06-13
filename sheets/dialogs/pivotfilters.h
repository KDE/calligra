#ifndef PIVOTFILTERS_H
#define PIVOTFILTERS_H

#include <QDialog>

namespace Ui {
    class PivotFilters;
}

class PivotFilters : public QDialog
{
    //Q_OBJECT

public:
    explicit PivotFilters(QWidget *parent = 0);
    ~PivotFilters();

private:
    Ui::PivotFilters *ui;
};

#endif // PIVOTFILTERS_H
