/****************************************************************************
** Form interface generated from reading ui file 'searchdia.ui'
**
** Created: Mon Jul 17 02:37:07 2000
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

    QLabel* TextLabel1;
    QLineEdit* lineEdit;
    QCheckBox* cs;
    QCheckBox* back;
    QCheckBox* wo;
    QPushButton* buttonFind;
    QPushButton* PushButton1;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QVBoxLayout* vbox;
};

#endif // SEARCHDIALOG_H
