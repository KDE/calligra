/***************************************************************************
                          kexiformbase.h  -  description
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by lucijan busch
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEXIFORMBASE_H
#define KEXIFORMBASE_H

#include "kexidialogbase.h"

/**
  *@author lucijan busch
  */

class KexiFormBase : public KexiDialogBase  {

Q_OBJECT
   
public: 
	KexiFormBase(QWidget *parent=0, const char *name=0, QString datasource=0);
	~KexiFormBase();
	
	void addWidgetLineEdit();

protected:
	void paintEvent(QPaintEvent *ev);

	int	m_dotSpacing;
};

#endif
