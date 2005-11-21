/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

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

#ifndef KEXIMIGRATIONIMPORTWIZARD_H
#define KEXIMIGRATIONIMPORTWIZARD_H

#include <kwizard.h>
#include <kprogress.h>
#include <kapplication.h>


class QHBoxLayout;
class QVBoxLayout;
class KComboBox;
class KListView;
class KLineEdit;
class QLabel;
class KexiConnSelectorWidget;
class KexiProjectSelectorWidget;
class KexiProjectSet;
class KexiDBTitlePage;
class KexiDBDriverComboBox;
class QVButtonGroup;

namespace KexiMigration {

//! GUI for importing non-native databases.
/**
@author Adam Pigg
*/
class KEXIMIGR_EXPORT ImportWizard : public KWizard
{
Q_OBJECT
public:
	/*! Creates wizard's instance. \a result will be set to imported project's filename, on success
	 and to null value on failure or cancellation. */
	ImportWizard(QWidget *parent = 0, QVariant* result = 0);
	virtual ~ImportWizard();

public slots:
	void progressUpdated(int percent);

protected slots:
	virtual void next();
	virtual void back();
	void pageSelected(const QString &);
	virtual void accept();
	virtual void reject();
	void helpClicked();

private:
	void setupintro();
	void setupsrcType();
	void setupsrcconn();
	void setupsrcdb();
	void setupdstType();
	void setupdstTitle();
	void setupdst();
	void setupfinish();
	void setupImportType();
	bool checkUserInput();
		void acceptImport();

	void checkIfSrcTypeFileBased(const QString& srcType);
	void checkIfDstTypeFileBased(const QString& dstType);

	void arriveSrcConnPage();
	void arriveSrcDBPage();
	void arriveDstTitlePage();
	void arriveDstPage();
	void arriveFinishPage();

	QWidget *introPage, *srcTypePage, *srcConnPage, *srcdbPage, *dstTypePage, *dstPage, *importTypePage, *finishPage;

	QVButtonGroup *importTypeButtonGroup ;
	KexiDBTitlePage* dstTitlePage;

	KComboBox *srcTypeCombo;
	KexiDBDriverComboBox *dstTypeCombo;

	KexiConnSelectorWidget *srcConn, *dstConn;
	KLineEdit *dstNewDBName;
	KexiProjectSelectorWidget *srcdbname;

	QLabel *lblfinishTxt;
	bool fileBasedSrc, fileBasedDst, fileBasedDstWasPresented, setupFileBasedSrcNeeded;
	KexiProjectSet* m_prjSet;
	KProgress *progress;
	QVariant *m_result;
};

}

#endif
