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

#include "altertable.h"

#include <unistd.h>

#include <qapplication.h>
#include <qfile.h>
#include <qdir.h>
#include <qregexp.h>

#include <kdebug.h>

#include <main/keximainwindowimpl.h>
#include <core/kexiaboutdata.h>
#include <core/kexidialogbase.h>
#include <core/kexiviewbase.h>
#include <core/kexipartitem.h>
#include <core/kexitabledesignerinterface.h>
#include <core/kexiinternalpart.h>
#include <koproperty/set.h>

QString testFilename;
QFile testFile;
QTextStream testFileStream;
QStringList testFileLine;
uint testLineNumber = 0;
QString origDbFilename, dbFilename;
int newArgc;
char** newArgv;
KexiMainWindowImpl* win = 0;
KexiProject* prj = 0;

void showError(const QString& msg)
{
	QString msg_(msg);
	msg_.prepend(QString("Error at line %1: ").arg(testLineNumber));
	kdDebug() << msg_ << endl;
}

/* Reads a single line from testFileStream, fills testFileLine, updates testLineNumber
 text in quotes is extracted, e.g. \"ab c\" is treat as one item "ab c"
 Returns flas on failure (e.g. end of file).
 Empty lines and lines or parts of lines with # (comments) are omitted. */
tristate readLineFromTestFile(const QString& expectedCommandName = QString::null)
{
	QString s;
	bool blockComment = false;
	while (true) {
		if (testFileStream.atEnd())
			return cancelled;
		testLineNumber++;
		s = testFileStream.readLine().stripWhiteSpace();
		if (blockComment) {
			if (s.startsWith("*/"))
				blockComment = false;
			continue;
		}
		if (!blockComment && s.startsWith("/*")) {
			blockComment = true;
			continue;
		}
		if (s.startsWith("#"))
			continue; //skip commented line
		if (!s.isEmpty())
			break;
	}
	s.append(" "); //sentinel
	QString item;
	testFileLine.clear();
	const int len = s.length();
	bool skipWhiteSpace = true, quoted = false;
	for (int i=0; i<len; i++) {
		const QChar ch( s.ref(i) );
		if (skipWhiteSpace) {
			if (ch=='#')
				break; //eoln
			if (ch==' ' || ch=='\t')
				continue;
			skipWhiteSpace = false;
			if (ch=='\"') {
				quoted = true;
				continue;
			}
			item.append(ch);
		}
		else {
			if ((quoted && ch=='\"') || (!quoted && (ch==' ' || ch=='\t'))) { //end of item
				skipWhiteSpace = true;
				quoted = false;
				testFileLine.append( item );
				item = QString::null;
				continue;
			}
			item.append(ch);
		}
	}
	if (!expectedCommandName.isEmpty() && testFileLine[0]!=expectedCommandName) {
		showError( QString("Invalid command '%1', expected '%2'")
			.arg(testFileLine[0]).arg(expectedCommandName));
		return false;
	}
	if (quoted) {
		showError( "Invalid contents" );
		return false;
	}
	return true;
}

bool checkItemsNumber(int expectedNumberOfItems)
{
	if (expectedNumberOfItems>0 && expectedNumberOfItems!=testFileLine.count()) {
		showError( QString("Invalid number of args (%1) for command '%2', expected: %3")
			.arg(testFileLine.count()-1).arg(testFileLine[0]).arg(expectedNumberOfItems-1) );
		return false;
	}
	return true;
}

QVariant::Type typeNameToQVariantType(const QCString& name)
{
	if (name=="string")
		return QVariant::String;
	if (name=="int")
		return QVariant::Int;
	if (name=="boolean")
		return QVariant::Bool;
	if (name=="double")
		return QVariant::Double;
//todo more types
	showError(QString("Invalid type '%1'").arg(name));
	return QVariant::Invalid;
}

//---------------------------------------

AlterTableTester::AlterTableTester()
 : QObject()
 , m_finishedCopying(false)
{
	//copy the db file to a temp file
	qInitNetworkProtocols();
	QPtrList<QNetworkOperation> list = m_copyOperator.copy( 
		"file://" + QDir::current().path() + "/" + origDbFilename, 
		"file://" + QDir::current().path() + "/" + dbFilename, false, false );
	connect(&m_copyOperator, SIGNAL(finished(QNetworkOperation*)), 
		this, SLOT(slotFinishedCopying(QNetworkOperation*)));
}

AlterTableTester::~AlterTableTester()
{
	QFile(dbFilename).remove();
}

void AlterTableTester::slotFinishedCopying(QNetworkOperation* oper)
{
	if (oper->operation()==QNetworkProtocol::OpPut)
		m_finishedCopying = true;
}

bool AlterTableTester::changeFieldProperty(KexiTableDesignerInterface* designerIface)
{
	if (!checkItemsNumber(5))
		return false;
	QVariant newValue;
	QCString propertyName( testFileLine[2].latin1() );
	QCString propertyType( testFileLine[3].latin1() );
	QString propertyValueString(testFileLine[4]);
	if (propertyName=="type")
		newValue = (int)KexiDB::Field::typeForString(testFileLine[4]);
	else {
		newValue = propertyValueString;
		QVariant::Type vtype = typeNameToQVariantType( propertyType );
		if (QVariant::Invalid==vtype)
			return false;
		if (!newValue.cast( vtype )) {
			showError( QString("Could not set property '%1' value '%2' of type '%3'")
				.arg(propertyName).arg(propertyValueString).arg(propertyType) );
			return false;
		}
	}
	designerIface->changeFieldPropertyForRow( testFileLine[1].toInt()-1, propertyName, newValue, 0, true );
	if (propertyName=="type") {
		//clean subtype name, e.g. from "longText" to "LongText", because dropdown list is case-sensitive
		QString realSubTypeName;
		if (KexiDB::Field::BLOB == KexiDB::Field::typeForString(testFileLine[4]))
//! @todo hardcoded!
			realSubTypeName = "image";
		else
			realSubTypeName = KexiDB::Field::typeString( KexiDB::Field::typeForString(testFileLine[4]) );
		designerIface->changeFieldPropertyForRow( testFileLine[1].toInt()-1, 
			"subType", realSubTypeName, 0, true );
	}
	return true;
}

//helper
bool AlterTableTester::getSchemaDump(KexiDialogBase* dlg, QString& schemaDebugString)
{
	KexiTableDesignerInterface* designerIface 
		= dynamic_cast<KexiTableDesignerInterface*>( dlg->selectedView() );
	if (!designerIface)
		return false;

	// Get the result
	tristate result;
	schemaDebugString = designerIface->debugStringForCurrentTableSchema(result);
	if (true!=result) {
		showError( QString("Loading modified schema failed. Result: %1")
			.arg(~result ? "cancelled" : "false") );
		return false;
	}
	schemaDebugString.remove(QRegExp(",$")); //no need to have "," at the end of lines
	return true;
}

bool AlterTableTester::showSchema(KexiDialogBase* dlg)
{
	QString schemaDebugString;
	if (!getSchemaDump(dlg, schemaDebugString))
		return false;
	kdDebug() << QString("Schema for '%1' table:\n").arg(dlg->partItem()->name())
		+ schemaDebugString + "\nendSchema" << endl;
	return true;
}

bool AlterTableTester::checkInternal(KexiDialogBase* dlg,
	QString& debugString, const QString& endCommand, bool skipColons)
{
	QTextStream resultStream(&debugString, IO_ReadOnly);
	// Load expected result, compare
	QString expectedLine, resultLine;
	while (true) {
		const bool testFileStreamAtEnd = testFileStream.atEnd();
		if (!testFileStreamAtEnd) {
			testLineNumber++;
			expectedLine = testFileStream.readLine().stripWhiteSpace();
			if (skipColons)
				expectedLine.remove(QRegExp(",$")); //no need to have "," at the end of lines
		}
		if (testFileStreamAtEnd || endCommand==expectedLine) {
			if (!resultStream.atEnd()) {
				showError( "Test file ends unexpectedly." );
				return false;
			}
			break;
		}
		//test line loaded, load result
		if (resultStream.atEnd()) {
			showError( QString("Result ends unexpectedly. There is at least one additinal test line: '")
				+ expectedLine +"'" );
			return false;
		}
		resultLine = resultStream.readLine().stripWhiteSpace();
		if (skipColons)
			resultLine.remove(QRegExp(",$")); //no need to have "," at the end of lines
		if (resultLine!=expectedLine) {
			showError(
				QString("Result differs from the expected:\nExpected: ")
				+expectedLine+"\n????????: "+resultLine+"\n");
			return false;
		}
	}
	return true;
}

bool AlterTableTester::checkSchema(KexiDialogBase* dlg)
{
	QString schemaDebugString;
	if (!getSchemaDump(dlg, schemaDebugString))
		return false;
	bool result = checkInternal(dlg, schemaDebugString, "endSchema", true /*skipColons*/);
	kdDebug() << QString("Schema check for table '%1': %2").arg(dlg->partItem()->name())
		.arg(result ? "OK" : "Failed") << endl;
	return result;
}

bool AlterTableTester::getActionsDump(KexiDialogBase* dlg, QString& actionsDebugString)
{
	KexiTableDesignerInterface* designerIface 
		= dynamic_cast<KexiTableDesignerInterface*>( dlg->selectedView() );
	if (!designerIface)
		return false;
	tristate result = designerIface->simulateAlterTableExecution(&actionsDebugString);
	if (true!=result) {
		showError( QString("Computing simplified actions for table failed.").arg(dlg->partItem()->name()) );
		return false;
	}
	return true;
}

bool AlterTableTester::showActions(KexiDialogBase* dlg)
{
	QString actionsDebugString;
	if (!getActionsDump(dlg, actionsDebugString))
		return false;
	kdDebug() << QString("Simplified actions for altering table '%1':\n").arg(dlg->partItem()->name())
		+actionsDebugString+"\n" << endl;
	return true;
}

bool AlterTableTester::checkActions(KexiDialogBase* dlg)
{
	QString actionsDebugString;
	if (!getActionsDump(dlg, actionsDebugString))
		return false;
	bool result = checkInternal(dlg, actionsDebugString, "endActions", true /*skipColons*/);
		kdDebug() << QString("Actions check for table '%1': %2").arg(dlg->partItem()->name())
		.arg(result ? "OK" : "Failed") << endl;
	return result;
}

bool AlterTableTester::saveTableDesign(KexiDialogBase* dlg)
{
	KexiTableDesignerInterface* designerIface 
		= dynamic_cast<KexiTableDesignerInterface*>( dlg->selectedView() );
	if (!designerIface)
		return false;
	tristate result = designerIface->executeRealAlterTable();
	if (true!=result) {
		showError( QString("Saving design of table '%1' failed.").arg(dlg->partItem()->name()) );
		return false;
	}
	return true;
}

bool AlterTableTester::getTableDataDump(KexiDialogBase* dlg, QString& dataString)
{
	KexiTableDesignerInterface* designerIface 
		= dynamic_cast<KexiTableDesignerInterface*>( dlg->selectedView() );
	if (!designerIface)
		return false;

	QMap<QString,QString> args;
	QTextStream ts( &dataString, IO_WriteOnly );
	args["textStream"] = QString::number((Q_ULLONG)(&ts));
	args["destinationType"]="file";
	args["delimiter"]="\t";
	args["itemId"] = QString::number(dlg->partItem()->identifier());
	if (!KexiInternalPart::executeCommand("csv_importexport", win, "KexiCSVExport", &args)) {
		showError( "Error exporting table contents." );
		return false;
	}
	return true;
}

bool AlterTableTester::showTableData(KexiDialogBase* dlg)
{
	QString dataString;
	if (!getTableDataDump(dlg, dataString))
		return false;
	kdDebug() << QString("Contents of table '%1':\n").arg(dlg->partItem()->name())+dataString+"\n" << endl;
	return true;
}

bool AlterTableTester::checkTableData(KexiDialogBase* dlg)
{
	QString dataString;
	if (!getTableDataDump(dlg, dataString))
		return false;
	bool result = checkInternal(dlg, dataString, "endTableData", false /*!skipColons*/);
		kdDebug() << QString("Table '%1' contents: %2").arg(dlg->partItem()->name())
			.arg(result ? "OK" : "Failed") << endl;
	return result;
}

//! Processes test file
bool AlterTableTester::run()
{
	while (!m_finishedCopying)
		qApp->processEvents(300);

	kdDebug() << "Database copied to temporary: " << dbFilename << endl;

	tristate res = win->openProject( dbFilename, 0 );
	if (true != res || !checkItemsNumber(2))
		return false;
	prj = win->project();

	//open table in design mode
	res = readLineFromTestFile("designTable");
	if (true != res)
		return ~res;
	
	QString tableName(testFileLine[1]);
	KexiPart::Item *item = prj->itemForMimeType("kexi/table", tableName);
	if (!item) {
		showError(QString("No such table '%1'").arg(tableName));
		return false;
	}
	bool openingCancelled;
	KexiDialogBase* dlg = win->openObject(item, Kexi::DesignViewMode, openingCancelled);
	if (!dlg) {
		showError(QString("Could not open table '%1'").arg(item->name()));
		return false;
	}
	KexiTableDesignerInterface* designerIface 
		= dynamic_cast<KexiTableDesignerInterface*>( dlg->selectedView() );
	if (!designerIface)
		return false;

	while (!testFileStream.atEnd()) {
		res = readLineFromTestFile();
		if (true != res)
			return ~res;
		QString command( testFileLine[0] );
		if (command=="removeField") {
			if (!checkItemsNumber(2))
				return false;
			designerIface->deleteRow( testFileLine[1].toInt()-1, true );
		}
		else if (command=="insertField") {
			if (!checkItemsNumber(3))
				return false;
			designerIface->insertField( testFileLine[1].toInt()-1, testFileLine[2], true );
		}
		else if (command=="insertEmptyRow") {
			if (!checkItemsNumber(2))
				return false;
			designerIface->insertEmptyRow( testFileLine[1].toInt()-1, true );
		}
		else if (command=="changeFieldProperty") {
			if (!checkItemsNumber(5) || !changeFieldProperty(designerIface))
				return false;
		}
		else if (command=="showSchema") {
			if (!checkItemsNumber(1) || !showSchema(dlg))
				return false;
		}
		else if (command=="checkSchema") {
			if (!checkItemsNumber(1) || !checkSchema(dlg))
				return false;
		}
		else if (command=="showActions") {
			if (!checkItemsNumber(1) || !showActions(dlg))
				return false;
		}
		else if (command=="checkActions") {
			if (!checkItemsNumber(1) || !checkActions(dlg))
				return false;
		}
		else if (command=="saveTableDesign") {
			if (!checkItemsNumber(1) || !saveTableDesign(dlg))
				return false;
		}
		else if (command=="showTableData") {
			if (!checkItemsNumber(1) || !showTableData(dlg))
				return false;
		}
		else if (command=="checkTableData") {
			if (!checkItemsNumber(1) || !checkTableData(dlg))
				return false;
		}
		else if (command=="stop") {
			if (!checkItemsNumber(1))
				return false;
			kdDebug() << QString("Test STOPPED at line %1.").arg(testLineNumber) << endl;
			break;
		}
		else {
			showError( QString("No such command '%1'").arg(command) );
			return false;
		}
	}
	return true;
}

//---------------------------------------

int quit(int result)
{
	testFile.close();
	delete qApp;
	if (newArgv)
		delete [] newArgv;
	return result;
}

extern "C" int kdemain(int argc, char *argv[])
{
	// args: <.altertable test filename>
	if (argc < 2)
		kdWarning() << "Please specify test filename.\nOptions: \n"
		"\t-close - closes the main window when test finishes" << endl;

	// options:
	const bool closeOnFinish = argc > 2 && 0==qstrcmp(argv[1], "-close");
		
	// open test file
	testFilename = argv[argc-1];
	testFile.setName(testFilename);
	if (!testFile.open(IO_ReadOnly)) {
		kdWarning() << QString("Opening test file %1 failed.").arg(testFilename) << endl;
	}
	//load db name
	testFileStream.setDevice( &testFile );
	tristate res = readLineFromTestFile("openDatabase");
	if (true != res)
		return quit( ~res ? 0 : 1 );
	origDbFilename = testFileLine[1];
	dbFilename = origDbFilename + ".tmp";

	newArgc = 2;
	newArgv = new char*[newArgc];
	newArgv[0] = qstrdup(argv[0]);
	newArgv[1] = qstrdup( "--skip-startup-dialog" );

	KAboutData* aboutdata = Kexi::createAboutData();
	aboutdata->setProgramName( "Kexi Alter Table Test" );
	int result = KexiMainWindowImpl::create(newArgc, newArgv, aboutdata);
	if (!qApp)
		return quit(result);

	win = KexiMainWindowImpl::self();
	AlterTableTester tester;
	//QObject::connect(win, SIGNAL(projectOpened()), &tester, SLOT(run()));

	if (!tester.run()) {
		kdWarning() << QString("Running test for file '%1' failed.").arg(testFilename) << endl;
		return quit(1);
	}
	kdDebug() << QString("Tests from file '%1': OK").arg(testFilename) << endl;
	result = closeOnFinish ? 0 : qApp->exec();
	quit(result);
	return result;
}

#include "altertable.moc"
