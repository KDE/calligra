/****************************************************************************
** Form interface generated from reading ui file 'textdialog.ui'
**
** Created: Sun Jul 16 20:35:26 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <klocale.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class KColorButton;
class QComboBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;

class TextDialog : public QDialog
{ 
    Q_OBJECT

public:
    TextDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~TextDialog();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    KColorButton* colorBullet1;
    KColorButton* colorBullet2;
    QLabel* TextLabel1_2;
    KColorButton* colorBullet3;
    QLabel* TextLabel1_3;
    QLabel* TextLabel1_4;
    KColorButton* colorBullet4;
    QComboBox* comboBullet1;
    QComboBox* comboBullet2;
    QComboBox* comboBullet4;
    QComboBox* comboBullet3;
    QGroupBox* GroupBox2;
    QLabel* TextLabel1_5;
    QComboBox* comboEnumType;
    KColorButton* enumColor;
    QLabel* TextLabel2;
    QGroupBox* GroupBox3;
    QLabel* TextLabel1_6;
    QSpinBox* spinLineSpacing;
    QLabel* TextLabel1_6_2;
    QSpinBox* spinParagSpacing;
    QLabel* TextLabel1_6_2_2;
    QSpinBox* spinMargin;
    QPushButton* PushButton2;
    QPushButton* PushButton1;

protected:
    QHBoxLayout* hbox;
    QVBoxLayout* vbox;
    QGridLayout* grid;
    QGridLayout* grid_2;
    QGridLayout* grid_3;
};

#endif // TEXTDIALOG_H
