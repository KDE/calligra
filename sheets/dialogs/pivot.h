#ifndef PIVOT_H
#define PIVOT_H

#include <QDialog>

namespace Ui {
    class Pivot;
}

class Pivot : public QDialog
{
    Q_OBJECT

public:
    explicit Pivot(QWidget *parent = 0);
    ~Pivot();

private slots:
    void on_Ok_clicked();

private:
    Ui::Pivot *ui;
};

#endif // PIVOT_H
