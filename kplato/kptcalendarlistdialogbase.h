/****************************************************************************
** Form interface generated from reading ui file './kptcalendarsdialogbase.ui'
**
** Created: Fri Jan 16 09:28:56 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KPTCALENDARLISTDIALOGBASE_H
#define KPTCALENDARLISTDIALOGBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QListView;
class QListViewItem;
class QLineEdit;
class QPushButton;
class KPTCalendarEdit;
class QComboBox;
class QLabel;

class KPTCalendarListDialogBase : public QWidget
{
    Q_OBJECT

public:
    KPTCalendarListDialogBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KPTCalendarListDialogBase();

    QListView* calendarList;
    QLineEdit* editName;
    QPushButton* bDelete;
    QPushButton* bAdd;
    QLabel* textLabel2;
    QComboBox* baseCalendar;
   
    KPTCalendarEdit* calendar;

protected:
    QHBoxLayout* KPTCalendarListDialogBaseLayout;
    QGridLayout* layout6;
    QHBoxLayout* layout10;
    QVBoxLayout* layout1;

protected slots:
    virtual void languageChange();

};

#endif // KPTCALENDARLISTDIALOGBASE_H
