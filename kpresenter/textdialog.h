/****************************************************************************
** Form interface generated from reading ui file 'textdialog.ui'
**
** Created: Mon Jul 17 02:41:53 2000
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
    QComboBox* comboBullet1;
    KColorButton* colorBullet1;
    QLabel* TextLabel1_2;
    QComboBox* comboBullet2;
    KColorButton* colorBullet2;
    QLabel* TextLabel1_3;
    QComboBox* comboBullet3;
    KColorButton* colorBullet3;
    QLabel* TextLabel1_4;
    QComboBox* comboBullet4;
    KColorButton* colorBullet4;
    QGroupBox* GroupBox2;
    QLabel* TextLabel1_5;
    QLabel* TextLabel2;
    QComboBox* comboEnumType;
    KColorButton* enumColor;
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
    QHBoxLayout* hbox_2;
    QHBoxLayout* hbox_3;
    QHBoxLayout* hbox_4;
    QHBoxLayout* hbox_5;
    QHBoxLayout* hbox_6;
    QHBoxLayout* hbox_7;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;
    QVBoxLayout* vbox_3;
    QVBoxLayout* vbox_4;
    QVBoxLayout* vbox_5;
    QVBoxLayout* vbox_6;
};

#endif // TEXTDIALOG_H
