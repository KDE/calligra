/* This file is part of the KDE project
   Copyright (C) 2001 Robert JACOLIN <rjacolin@ifrance.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/*
   This file is based on the file :
    koffice/filters/kword/html/htmlexportdia.h
	Copyright (C) 2001 Nicolas Goutte <goutte@kde.org>

   which was based on the old file:
    /home/kde/koffice/filters/kspread/csv/csvfilterdia.h

   The old file was copyrighted by
    Copyright (C) 1999 David Faure <faure@kde.org>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#ifndef __LATEXIMPORTDIA_H__
#define __LATEXIMPORTDIA_H__

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QRadioButton>
#include <qvbuttongroup.h>

#include <klocale.h>
#include <dcopobject.h>

#include <kdialogbase.h>
#include <kdebug.h>
#include <KoStore.h>

class LATEXImportDia : public KDialogBase, public DCOPObject
{
	K_DCOP
	
	Q_OBJECT
	
	QString _fileIn;
	QByteArray _arrayIn;
	KoStore* _out;	/* the zipped file containing all pictures, part, ... */

	public:
		LATEXImportDia(KoStore*, QWidget *parent=0L, const char *name=0L);

		virtual ~LATEXImportDia() {}
		void createDialog();

		virtual void state();
		void setInputFile(QString file)  { _fileIn = file; }
		//void setOutputFile(QString file) { _fileOut = file; }

	private:
		Q3VButtonGroup* styleBox,       *langBox,        *docBox;
		QRadioButton*  latexStyleRBtn, *kwordStyleRBtn;	/* Document style */
		QRadioButton*  unicodeRBtn,    *latin1RBtn;		/* Language       */
		QRadioButton*  newDocRBtn,     *embededRBtn;	/* Latex file     */
	
	k_dcop:
		void useDefaultConfig() { slotOk(); }

	public slots:
		virtual void slotOk();
};

#endif /* __LATEXIMPORTDIA_H__ */

