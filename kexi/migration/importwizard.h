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
class QHBox;
class KComboBox;

namespace KexiMigration {

/**
@author Adam Pigg
*/
class importWizard : public KWizard
{
Q_OBJECT
private:
    QHBox *page1, *page2, *page3, *page4;
    KComboBox *srcCombo, *dstCombo;
    
    void setupPage1();
    void setupPage2();
    void setupPage3();
    void setupPage4();
    void doImport();
public:
    importWizard(QWidget *parent = 0, const char *name = 0);

    ~importWizard();

};

};

#endif
