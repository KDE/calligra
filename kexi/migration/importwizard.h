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


class QLabel;
class QCheckBox;
class QHBoxLayout;
class QVBoxLayout;
class QVButtonGroup;
class KComboBox;
class KListView;
class KLineEdit;
class KActiveLabel;
class KexiConnSelectorWidget;
class KexiProjectSelectorWidget;
class KexiProjectSet;
class KexiDBTitlePage;
class KexiDBDriverComboBox;

namespace KexiMigration {

//! GUI for importing external databases (file-based and server-based)
class KEXIMIGR_EXPORT ImportWizard : public KWizard
{
Q_OBJECT
public:
	/*! Creates wizard's instance. 
	 \a args contains arguments that can be parsed by parseArguments().
	 \a *arg will be also set to imported project's filename on success
	 and to null value on failure or cancellation. */
	ImportWizard(QWidget *parent = 0, QMap<QString,QString>* args = 0);
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
	void parseArguments();
	void setupIntro();
//	void setupSrcType();
	void setupSrcConn();
	void setupSrcDB();
	void setupDstType();
	void setupDstTitle();
	void setupDst();
	void setupFinish();
	void setupImportType();
	void setupImporting();
	bool checkUserInput();
	bool acceptImport();

	bool fileBasedSrcSelected() const;
	bool fileBasedDstSelected() const;
	QString driverNameForSelectedSource();
//	void checkIfSrcTypeFileBased(const QString& srcType);
//	void checkIfDstTypeFileBased(const QString& dstType);

	void arriveSrcConnPage();
	void arriveSrcDBPage();
	void arriveDstTitlePage();
	void arriveDstPage();
	void arriveFinishPage();
	void arriveImportingPage();

	QWidget *m_introPage, /* *m_srcTypePage,*/ *m_srcConnPage, *m_srcDBPage, 
		*m_dstTypePage, *m_dstPage, *m_importTypePage, *m_importingPage, *m_finishPage;

	QVButtonGroup *m_importTypeButtonGroup;
	KexiDBTitlePage* m_dstTitlePage;

	KComboBox *m_srcTypeCombo;
	KexiDBDriverComboBox *m_dstTypeCombo;

	KexiConnSelectorWidget *m_srcConn, *m_dstConn;
	KLineEdit *m_dstNewDBNameLineEdit;
	KexiProjectSelectorWidget *m_srcDBName;

	QLabel *m_lblImportingTxt;
	KActiveLabel *m_lblImportingErrTxt, *m_finishLbl;
	QCheckBox *m_openImportedProjectCheckBox;
	bool m_fileBasedDstWasPresented, m_setupFileBasedSrcNeeded, 
		m_acceptImportExecuted; //!< used in acceptImport()
	KexiProjectSet* m_prjSet;
	KProgress *m_progress;
	QMap<QString,QString> *m_args;
	QString m_predefinedFileName, m_predefinedMimeType;
};

}

#endif
