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
class KComboBox;
class KListView;
class KLineEdit;

namespace KexiMigration {

/**
@author Adam Pigg
*/
class importWizard : public KWizard
{
Q_OBJECT
private:
    QHBox *intro, *srcType, *srcconn, *srcdb, *dstType, *dst, *finish;
    KComboBox *srcTypeCombo, *dstTypeCombo;
    KexiConnSelectorWidget *srcConn, *dstConn;
    KLineEdit *dstNewDBName;
    KexiProjectSelectorWidget *srcdbname;
    
    void setupintro();
    void setupsrcType();
    void setupsrcconn();
    void setupsrcdb();
    void setupdstType();
    void setupdst();
    void setupfinish();
    void createBlankPages();
private slots:
    void populateSrcDBList(const QString& driverName);
    void nextClicked(const QString &);
    void doImport();
public:
    importWizard(QWidget *parent = 0, const char *name = 0);

    ~importWizard();

};

};

#endif
