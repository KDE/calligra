/****************************************************************************
** Form interface generated from reading ui file './vtransformdlg_stub.ui'
**
** Created: Thu Jul 19 20:14:21 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef FORM3_H
#define FORM3_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QTabWidget;

class VTransformDlg : public QWidget
{ 
    Q_OBJECT

public:
    VTransformDlg( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~VTransformDlg();

    QLabel* PixmapLabel6_2;
    QTabWidget* TabWidget2;
    QWidget* tab;
    QLineEdit* xpos;
    QLineEdit* width;
    QLineEdit* ypos;
    QLineEdit* height;
    QFrame* Line1;
    QLabel* rotatepix;
    QLabel* skewpix;
    QComboBox* rotatecombo;
    QComboBox* skewcombo;
    QLabel* xpospix;
    QLabel* ypospix;
    QLabel* widthpix;
    QLabel* heightpix;
    QWidget* tab_2;
    QWidget* tab_3;

};

#endif // FORM3_H
