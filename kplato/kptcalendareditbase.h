/****************************************************************************
** Form interface generated from reading ui file './kptcalendardialogbase.ui'
**
** Created: Mon Feb 9 09:28:08 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KPTCALENDAREDITBASE_H
#define KPTCALENDAREDITBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class KPTCalendarPanel;
class QLabel;
class QComboBox;
class QButtonGroup;
class QPushButton;
class QListView;
class QListViewItem;
class QTimeEdit;

class KPTCalendarEditBase : public QWidget
{
    Q_OBJECT

public:
    KPTCalendarEditBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KPTCalendarEditBase();

    QGroupBox* groupBox2;
    KPTCalendarPanel* calendarPanel;
    QButtonGroup* day;
    QComboBox* state;
    QPushButton* bApply;
    QGroupBox* groupBox4;
    QListView* intervalList;
    QTimeEdit* startTime;
    QTimeEdit* endTime;
    QPushButton* bClear;
    QPushButton* bAddInterval;

protected:
    QHBoxLayout* KPTCalendarEditBaseLayout;
    QGridLayout* groupBox2Layout;
    QHBoxLayout* layout10;
    QVBoxLayout* dayLayout;
    QHBoxLayout* layout8;
    QVBoxLayout* groupBox4Layout;
    QHBoxLayout* layout6;
    QHBoxLayout* layout5;

protected slots:
    virtual void languageChange();

};

#endif // KPTCALENDAREDITBASE_H
