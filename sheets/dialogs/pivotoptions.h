#ifndef PIVOTOPTIONS_H
#define PIVOTOPTIONS_H

#include <QDialog>

namespace Ui {
    class PivotOptions;
}

class PivotOptions : public QDialog
{
    //Q_OBJECT

public:
    explicit PivotOptions(QWidget *parent = 0);
    ~PivotOptions();

private:
    Ui::PivotOptions *ui;
};

#endif // PIVOTOPTIONS_H
