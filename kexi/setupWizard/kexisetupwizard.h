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
#ifndef _KEXI_SETUP_WIZARD_
#define _KEXI_SETUP_WIZARD_

#include "kexisetupwizard_skel.h"

class KexiSetupWizard: public KexiSetupWizardSkel
{

Q_OBJECT
public:
	KexiSetupWizard();
	~KexiSetupWizard();

protected slots:
	virtual void accept();

};

#endif
