/****************************************************************************
** Form interface generated from reading ui file 'searchdia.ui'
**
** Created: Mon Jul 17 00:09:48 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <klocale.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class SearchDialog : public QDialog
{ 
    Q_OBJECT

public:
    SearchDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SearchDialog();

    QCheckBox* back;
    QLineEdit* lineEdit;
    QPushButton* buttonFind;
    QPushButton* PushButton1;
    QCheckBox* cs;
    QCheckBox* wo;
    QLabel* TextLabel1;

protected:
    QHBoxLayout* hbox;
    QGridLayout* grid;
};

#endif // SEARCHDIALOG_H
