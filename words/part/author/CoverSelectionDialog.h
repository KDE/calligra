#ifndef COVERSELECTIONDIALOG_H
#define COVERSELECTIONDIALOG_H

#include "CAuView.h"
#include <QDialog>
#include <QLabel>

namespace Ui {
class CoverSelectionDialog;
}

class CoverSelectionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CoverSelectionDialog(CAuView *au, QWidget *parent = 0);
    ~CoverSelectionDialog();

private slots:
    void open();
    void reset();
    void ok();

private:
    Ui::CoverSelectionDialog *ui;

    void refresh();
    void createActions();

    CAuView *view;
    QPair<QString, QByteArray> img;

    QAction *openAct;
    QAction *exitAct;
};

#endif // COVERSELECTIONDIALOG_H
