/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Oct  23 22:30:00 CEST 2002
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "kexisetupwizard.h"
#include "kexisetupwizard.moc"
#include <klocale.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kprocess.h>
#include <qradiobutton.h>

KexiSetupWizard::KexiSetupWizard():KexiSetupWizardSkel(0)
{
	setCaption("Kexi Setup Wizard");
	setHelpEnabled(WelcomePage,false);
	setHelpEnabled(WindowModePage,false);
	setHelpEnabled(FinishAndExecPage,false);

	setFinishEnabled(FinishAndExecPage,true);
}


KexiSetupWizard::~KexiSetupWizard()
{
}

void KexiSetupWizard::accept() {
	KConfig *config=kapp->config();

        config->setGroup("views");
        if (multipleWindows->isChecked())
		config->writeEntry("windowMode","multipleWindows" );
	else 
	if (singleWindow->isChecked())
		config->writeEntry("windowMode","singleWindow" );
	else
		config->writeEntry("windowMode","MDIWindows" );

        config->setGroup("setupWizard");
	config->writeEntry("startup","false");

	config->sync();

        KProcess p;
        p<<"kexi";
        p.start(KProcess::DontCare);

	KexiSetupWizardSkel::accept();
}
