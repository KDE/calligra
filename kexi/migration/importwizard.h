//
// C++ Interface: importwizard
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KEXIMIGRATIONIMPORTWIZARD_H
#define KEXIMIGRATIONIMPORTWIZARD_H

#include <kwizard.h>

class KexiConnSelectorWidget;
class KexiProjectSelectorWidget;
class QHBox;
class QVBox;
class KComboBox;
class KListView;
class KLineEdit;
class QLabel;

namespace KexiMigration {

/**
@author Adam Pigg
*/
class importWizard : public KWizard
{
Q_OBJECT
private:
    QHBox *introPage, *srcTypePage, *srcConnPage, *srcdbPage, *dstTypePage, *dstPage, *finishPage;
    QVBox *srcdbControls;
    KComboBox *srcTypeCombo, *dstTypeCombo;
    KexiConnSelectorWidget *srcConn, *dstConn;
    KLineEdit *dstNewDBName;
    KexiProjectSelectorWidget *srcdbname;
    QLabel *lblfinishTxt;
    
    void setupintro();
    void setupsrcType();
    void setupsrcconn();
    void setupsrcdb();
    void setupdstType();
    void setupdst();
    void setupfinish();
    void createBlankPages();
    bool checkUserInput();
private slots:
    void nextClicked(const QString &);
    virtual void accept();
public:
    importWizard(QWidget *parent = 0, const char *name = 0);

    ~importWizard();

};

};

#endif
