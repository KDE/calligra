#ifndef KPTPROJECTDIALOG_H
#define KPTPROJECTDIALOG_H

#include <kdialogbase.h>

class KPTProject;
class KLineEdit;
class QTextEdit;


class KPTProjectDialog : public KDialogBase {
    Q_OBJECT
public:
    KPTProjectDialog(KPTProject &project, QWidget *parent=0,
		     const char *name=0);

protected slots:
    void slotOk();

private:
    KPTProject &project;
    KLineEdit *namefield;
    KLineEdit *leaderfield;
    QTextEdit *descriptionfield;
};


#endif // KPTPROJECTDIALOG_H
