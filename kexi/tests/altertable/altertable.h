/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef AlterTableTester_H
#define AlterTableTester_H

#include <qurloperator.h>
#include <qnetwork.h>
#include <qnetworkprotocol.h>
#include <kexiutils/tristate.h>

class KexiTableDesignerInterface;
class KexiDialogBase;

class AlterTableTester : public QObject
{
	Q_OBJECT
	public:
		AlterTableTester();
		~AlterTableTester();

		tristate run(bool &closeAppRequested);

	protected slots:
		void slotFinishedCopying(QNetworkOperation*);

	private:
		bool changeFieldProperty(KexiTableDesignerInterface* designerIface);
		bool getSchemaDump(KexiDialogBase* dlg, QString& schemaDebugString);
		bool showSchema(KexiDialogBase* dlg, bool copyToClipboard);
		bool checkSchema(KexiDialogBase* dlg);
		bool getActionsDump(KexiDialogBase* dlg, QString& actionsDebugString);
		bool showActions(KexiDialogBase* dlg, bool copyToClipboard);
		bool checkActions(KexiDialogBase* dlg);
		bool checkInternal(KexiDialogBase* dlg, QString& debugString, 
			const QString& endCommand, bool skipColons);
		bool saveTableDesign(KexiDialogBase* dlg);
		bool getTableDataDump(KexiDialogBase* dlg, QString& dataString);
		bool showTableData(KexiDialogBase* dlg, bool copyToClipboard);
		bool checkTableData(KexiDialogBase* dlg);
		bool closeWindow(KexiDialogBase* dlg);

		QUrlOperator m_copyOperator;
		bool m_finishedCopying;
};

#endif
