/****************************************************************************
** Form interface generated from reading ui file './latexexportdia.ui'
**
** Created: sam fév 22 22:11:39 2003
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef LATEXEXPORTDIA_H
#define LATEXEXPORTDIA_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class KComboBox;
class KIntNumInput;
class KListBox;
class KURLRequester;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QListBoxItem;
class QPushButton;
class QRadioButton;
class QTabWidget;
class QWidget;

class LatexExportDia : public QDialog
{
    Q_OBJECT

public:
    LatexExportDia( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~LatexExportDia();

    QTabWidget* _tab;
    QWidget* Widget2;
    QButtonGroup* typeGroup;
    QRadioButton* fullDocButton;
    QRadioButton* embededButton;
    QButtonGroup* styleGroup;
    QRadioButton* kwordStyleButton;
    QRadioButton* latexStyleButton;
    QLabel* TextLabel1_2;
    QLabel* TextLabel2;
    QLabel* defaultFontSizeTextZone;
    QComboBox* classComboBox;
    KIntNumInput* defaultFontSize;
    KComboBox* qualityComboBox;
    QWidget* Widget3;
    KURLRequester* pathPictures;
    QCheckBox* pictureCheckBox;
    QLabel* TextLabel1;
    QWidget* tab;
    QButtonGroup* ButtonGroup3;
    KComboBox* encodingComboBox;
    QGroupBox* GroupBox4;
    QPushButton* rmLanguageBtn;
    QPushButton* addLanguageBtn;
    KListBox* languagesList;
    KListBox* langUsedList;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

public slots:
    virtual void addLanguage();
    virtual void removeLanguage();

protected:
    QGridLayout* LatexExportDiaLayout;
    QHBoxLayout* Layout1;

protected slots:
    virtual void languageChange();
};

#endif // LATEXEXPORTDIA_H
