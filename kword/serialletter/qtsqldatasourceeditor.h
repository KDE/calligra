/****************************************************************************
** Form interface generated from reading ui file './qtsqldatasourceeditor.ui'
**
** Created: Mit Nov 7 20:07:32 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QTSQLDATASOURCEEDITOR_H
#define QTSQLDATASOURCEEDITOR_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QDataTable;
class QFrame;
class QLabel;
class QPushButton;
class QSqlRecord;

class QTSQLDataSourceEditor : public QWidget
{ 
    Q_OBJECT

public:
    QTSQLDataSourceEditor( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~QTSQLDataSourceEditor();

    QLabel* TextLabel1;
    QComboBox* tableCombo;
    QCheckBox* filterCheckBox;
    QPushButton* editFilter;
    QFrame* Line1;
    QLabel* TextLabel2;
    QDataTable* DataTable;


public slots:
    virtual void filterCheckBox_toggled( bool fcb_state );
    void polish();

protected:
    QVBoxLayout* QTSQLDataSourceEditorLayout;
    QHBoxLayout* Layout5;
    QHBoxLayout* Layout6;
};

#endif // QTSQLDATASOURCEEDITOR_H
