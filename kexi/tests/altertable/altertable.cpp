/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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
#include <qclipboard.h>

#include <kdebug.h>

#include <main/KexiMainWindow.h>
#include <core/kexiaboutdata.h>
#include <core/KexiWindow.h>
#include <core/KexiView.h>
#include <core/kexipartitem.h>
#include <core/kexitabledesignerinterface.h>
#include <core/kexiinternalpart.h>
#include <kexiutils/utils.h>
#include <koproperty/set.h>
#include <kexidb/connection.h>
#include <kexidb/utils.h>

QString testFilename;
QFile testFile;
QTextStream testFileStream;
QStringList testFileLine;
uint testLineNumber = 0;
QString origDbFilename, dbFilename;
int variableI = 1; // simple variable 'i' support
int newArgc;
char** newArgv;
KexiMainWindow* win = 0;
KexiProject* prj = 0;

void showError(const QString& msg)
{
    QString msg_(msg);
    msg_.prepend(QString("Error at line %1: ").arg(testLineNumber));
    kDebug() << msg_;
}

/* Reads a single line from testFileStream, fills testFileLine, updates testLineNumber
 text in quotes is extracted, e.g. \"ab c\" is treat as one item "ab c"
 Returns flas on failure (e.g. end of file).
 Empty lines and lines or parts of lines with # (comments) are omitted. */
tristate readLineFromTestFile(const QString& expectedCommandName = QString())
{
    QString s;
    bool blockComment = false;
    while (true) {
        if (testFileStream.atEnd())
            return cancelled;
        testLineNumber++;
        s = testFileStream.readLine().stripWhiteSpace();
        if (blockComment) {
            if (s.endsWith("*/"))
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
    for (int i = 0; i < len; i++) {
        const QChar ch(s.ref(i));
        if (skipWhiteSpace) {
            if (ch == '#')
                break; //eoln
            if (ch == ' ' || ch == '\t')
                continue;
            skipWhiteSpace = false;
            if (ch == '\"') {
                quoted = true;
                continue;
            }
            item.append(ch);
        } else {
            if ((quoted && ch == '\"') || (!quoted && (ch == ' ' || ch == '\t'))) { //end of item
                skipWhiteSpace = true;
                quoted = false;
                testFileLine.append(item);
                item.clear();
                continue;
            }
            item.append(ch);
        }
    }
    if (!expectedCommandName.isEmpty() && testFileLine[0] != expectedCommandName) {
        showError(QString("Invalid command '%1', expected '%2'")
                  .arg(testFileLine[0]).arg(expectedCommandName));
        return false;
    }
    if (quoted) {
        showError("Invalid contents");
        return false;
    }
    return true;
}

bool checkItemsNumber(int expectedNumberOfItems, int optionalNumberOfItems = -1)
{
    bool ok = expectedNumberOfItems == (int)testFileLine.count();
    if (optionalNumberOfItems > 0)
        ok = ok || optionalNumberOfItems == (int)testFileLine.count();
    if (!ok) {
        QString msg = QString("Invalid number of args (%1) for command '%2', expected: %3")
                      .arg(testFileLine.count()).arg(testFileLine[0]).arg(expectedNumberOfItems);
        if (optionalNumberOfItems > 0)
            msg.append(QString(" or %1").arg(optionalNumberOfItems));
        showError(msg);
        return false;
    }
    return true;
}

QVariant::Type typeNameToQVariantType(const QCString& name_)
{
    QCString name(name_.toLower());
    if (name == "string")
        return QVariant::String;
    if (name == "int")
        return QVariant::Int;
    if (name == "bool" || name == "boolean")
        return QVariant::Bool;
    if (name == "double" || name == "float")
        return QVariant::Double;
    if (name == "date")
        return QVariant::Date;
    if (name == "datetime")
        return QVariant::DateTime;
    if (name == "time")
        return QVariant::Time;
    if (name == "bytearray")
        return QVariant::ByteArray;
    if (name == "longlong")
        return QVariant::LongLong;
//todo more types
    showError(QString("Invalid type '%1'").arg(name_));
    return QVariant::Invalid;
}

// casts string to QVariant
bool castStringToQVariant(const QString& string, const QCString& type, QVariant& result)
{
    if (string.toLower() == "<null>") {
        result = QVariant();
        return true;
    }
    if (string == "\"\"") {
        result = QString("");
        return true;
    }
    const QVariant::Type vtype = typeNameToQVariantType(type);
    bool ok;
    result = KexiDB::stringToVariant(string, vtype, ok);
    return ok;
}

// returns a number parsed from argument; if argument is i or i++, variableI is used
// 'ok' is set to false on failure
static int getNumber(const QString& argument, bool& ok)
{
    int result;
    ok = true;
    if (argument == "i" || argument == "i++") {
        result = variableI;
        if (argument == "i++")
            variableI++;
    } else {
        result = argument.toInt(&ok);
        if (!ok) {
            showError(QString("Invalid value '%1'").arg(argument));
            return -1;
        }
    }
    return result;
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
                                           "file://" + QDir::current().path() + "/" + dbFilename, false, false);
    connect(&m_copyOperator, SIGNAL(finished(QNetworkOperation*)),
            this, SLOT(slotFinishedCopying(QNetworkOperation*)));
}

AlterTableTester::~AlterTableTester()
{
    QFile(dbFilename).remove();
}

void AlterTableTester::slotFinishedCopying(QNetworkOperation* oper)
{
    if (oper->operation() == QNetworkProtocol::OpPut)
        m_finishedCopying = true;
}

bool AlterTableTester::changeFieldProperty(KexiTableDesignerInterface* designerIface)
{
    if (!checkItemsNumber(5))
        return false;
    QVariant newValue;
    QCString propertyName(testFileLine[2].toLatin1());
    QCString propertyType(testFileLine[3].toLatin1());
    QString propertyValueString(testFileLine[4]);
    if (propertyName == "type")
        newValue = (int)KexiDB::Field::typeForString(testFileLine[4]);
    else {
        if (!castStringToQVariant(propertyValueString, propertyType, newValue)) {
            showError(QString("Could not set property '%1' value '%2' of type '%3'")
                      .arg(propertyName).arg(propertyValueString).arg(propertyType));
            return false;
        }
    }
    bool ok;
    int row = getNumber(testFileLine[1], ok) - 1;
    if (!ok)
        return false;
    designerIface->changeFieldPropertyForRow(row, propertyName, newValue, 0, true);
    if (propertyName == "type") {
        //clean subtype name, e.g. from "longText" to "LongText", because dropdown list is case-sensitive
        QString realSubTypeName;
        if (KexiDB::Field::BLOB == KexiDB::Field::typeForString(testFileLine[4]))
//! @todo hardcoded!
            realSubTypeName = "image";
        else
            realSubTypeName = KexiDB::Field::typeString(KexiDB::Field::typeForString(testFileLine[4]));
        designerIface->changeFieldPropertyForRow(row, "subType", realSubTypeName, 0, true);
    }
    return true;
}

//helper
bool AlterTableTester::getSchemaDump(KexiWindow* window, QString& schemaDebugString)
{
    KexiTableDesignerInterface* designerIface
    = dynamic_cast<KexiTableDesignerInterface*>(window->selectedView());
    if (!designerIface)
        return false;

    // Get the result
    tristate result;
    schemaDebugString = designerIface->debugStringForCurrentTableSchema(result);
    if (true != result) {
        showError(QString("Loading modified schema failed. Result: %1")
                  .arg(~result ? "cancelled" : "false"));
        return false;
    }
    schemaDebugString.remove(QRegExp(",$")); //no need to have "," at the end of lines
    return true;
}

bool AlterTableTester::showSchema(KexiWindow* window, bool copyToClipboard)
{
    QString schemaDebugString;
    if (!getSchemaDump(window, schemaDebugString))
        return false;
    if (copyToClipboard)
        QApplication::clipboard()->setText(schemaDebugString);
    else
        kDebug() << QString("Schema for '%1' table:\n").arg(window->partItem()->name())
        + schemaDebugString + "\nendSchema";
    return true;
}

bool AlterTableTester::checkInternal(KexiWindow* window,
                                     QString& debugString, const QString& endCommand, bool skipColonsAndStripWhiteSpace)
{
    Q_UNUSED(window);
    QTextStream resultStream(&debugString, IO_ReadOnly);
    // Load expected result, compare
    QString expectedLine, resultLine;
    while (true) {
        const bool testFileStreamAtEnd = testFileStream.atEnd();
        if (!testFileStreamAtEnd) {
            testLineNumber++;
            expectedLine = testFileStream.readLine();
            if (skipColonsAndStripWhiteSpace) {
                expectedLine = expectedLine.stripWhiteSpace();
                expectedLine.remove(QRegExp(",$")); //no need to have "," at the end of lines
            }
        }
        if (testFileStreamAtEnd || endCommand == expectedLine.stripWhiteSpace()) {
            if (!resultStream.atEnd()) {
                showError("Test file ends unexpectedly.");
                return false;
            }
            break;
        }
        //test line loaded, load result
        if (resultStream.atEnd()) {
            showError(QString("Result ends unexpectedly. There is at least one additinal test line: '")
                      + expectedLine + "'");
            return false;
        }
        resultLine = resultStream.readLine();
        if (skipColonsAndStripWhiteSpace) {
            resultLine = resultLine.stripWhiteSpace();
            resultLine.remove(QRegExp(",$")); //no need to have "," at the end of lines
        }
        if (resultLine != expectedLine) {
            showError(
                QString("Result differs from the expected:\nExpected: ")
                + expectedLine + "\n????????: " + resultLine + "\n");
            return false;
        }
    }
    return true;
}

bool AlterTableTester::checkSchema(KexiWindow* window)
{
    QString schemaDebugString;
    if (!getSchemaDump(window, schemaDebugString))
        return false;
    bool result = checkInternal(window, schemaDebugString, "endSchema", true /*skipColonsAndStripWhiteSpace*/);
    kDebug() << QString("Schema check for table '%1': %2").arg(window->partItem()->name())
    .arg(result ? "OK" : "Failed");
    return result;
}

bool AlterTableTester::getActionsDump(KexiWindow* window, QString& actionsDebugString)
{
    KexiTableDesignerInterface* designerIface
    = dynamic_cast<KexiTableDesignerInterface*>(window->selectedView());
    if (!designerIface)
        return false;
    tristate result = designerIface->simulateAlterTableExecution(&actionsDebugString);
    if (true != result) {
        showError(QString("Computing simplified actions for table '%1'  failed.").arg(window->partItem()->name()));
        return false;
    }
    return true;
}

bool AlterTableTester::showActions(KexiWindow* window, bool copyToClipboard)
{
    QString actionsDebugString;
    if (!getActionsDump(window, actionsDebugString))
        return false;
    if (copyToClipboard)
        QApplication::clipboard()->setText(actionsDebugString);
    else
        kDebug() << QString("Simplified actions for altering table '%1':\n").arg(window->partItem()->name())
        + actionsDebugString + "\n";
    return true;
}

bool AlterTableTester::checkActions(KexiWindow* window)
{
    QString actionsDebugString;
    if (!getActionsDump(window, actionsDebugString))
        return false;
    bool result = checkInternal(window, actionsDebugString, "endActions", true /*skipColonsAndStripWhiteSpace*/);
    kDebug() << QString("Actions check for table '%1': %2").arg(window->partItem()->name())
    .arg(result ? "OK" : "Failed");
    return result;
}

bool AlterTableTester::saveTableDesign(KexiWindow* window)
{
    KexiTableDesignerInterface* designerIface
    = dynamic_cast<KexiTableDesignerInterface*>(window->selectedView());
    if (!designerIface)
        return false;
    tristate result = designerIface->executeRealAlterTable();
    if (true != result) {
        showError(QString("Saving design of table '%1' failed.").arg(window->partItem()->name()));
        return false;
    }
    return true;
}

bool AlterTableTester::getTableDataDump(KexiWindow* window, QString& dataString)
{
    KexiTableDesignerInterface* designerIface
    = dynamic_cast<KexiTableDesignerInterface*>(window->selectedView());
    if (!designerIface)
        return false;

    QMap<QString, QString> args;
    QTextStream ts(&dataString, IO_WriteOnly);
    args["textStream"] = KexiUtils::ptrToString<QTextStream>(&ts);
    args["destinationType"] = "file";
    args["delimiter"] = "\t";
    args["textQuote"] = "\"";
    args["itemId"] = QString::number(
                         prj->dbConnection()->tableSchema(window->partItem()->name())->id());
    if (!KexiInternalPart::executeCommand("csv_importexport", win, "KexiCSVExport", &args)) {
        showError("Error exporting table contents.");
        return false;
    }
    return true;
}

bool AlterTableTester::showTableData(KexiWindow* window, bool copyToClipboard)
{
    QString dataString;
    if (!getTableDataDump(window, dataString))
        return false;
    if (copyToClipboard)
        QApplication::clipboard()->setText(dataString);
    else
        kDebug() << QString("Contents of table '%1':\n").arg(window->partItem()->name()) + dataString + "\n";
    return true;
}

bool AlterTableTester::checkTableData(KexiWindow* window)
{
    QString dataString;
    if (!getTableDataDump(window, dataString))
        return false;
    bool result = checkInternal(window, dataString, "endTableData", false /*!skipColonsAndStripWhiteSpace*/);
    kDebug() << QString("Table '%1' contents: %2").arg(window->partItem()->name())
    .arg(result ? "OK" : "Failed");
    return result;
}

bool AlterTableTester::closeWindow(KexiWindow* window)
{
    if (!window)
        return true;
    QString name = window->partItem()->name();
    tristate result = true == win->closeDialog(window, true/*layoutTaskBar*/, true/*doNotSaveChanges*/);
    kDebug() << QString("Closing window for table '%1': %2").arg(name)
    .arg(result == true ? "OK" : (result == false ? "Failed" : "Cancelled"));
    return result == true;
}

//! Processes test file
tristate AlterTableTester::run(bool &closeAppRequested)
{
    closeAppRequested = false;
    while (!m_finishedCopying)
        qApp->processEvents(300);

    kDebug() << "Database copied to temporary: " << dbFilename;

    if (!checkItemsNumber(2))
        return false;

    tristate res = win->openProject(dbFilename, 0);
    if (true != res)
        return res;
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
    KexiWindow* window = win->openObject(item, Kexi::DesignViewMode, openingCancelled);
    if (!window) {
        showError(QString("Could not open table '%1'").arg(item->name()));
        return false;
    }
    KexiTableDesignerInterface* designerIface
    = dynamic_cast<KexiTableDesignerInterface*>(window->selectedView());
    if (!designerIface)
        return false;

    //dramatic speedup: temporary hide the window and propeditor
    QWidget * propeditor
    = KexiUtils::findFirstChild<QWidget>(qApp->mainWidget(), "KexiPropertyEditorView");
    if (propeditor)
        propeditor->hide();
    window->hide();

    bool designTable = true;
    while (!testFileStream.atEnd()) {
        res = readLineFromTestFile();
        if (true != res)
            return ~res;
        QString command(testFileLine[0]);
        if (designTable) {
            //subcommands available within "designTable" commands
            if (command == "endDesign") {
                if (!checkItemsNumber(1))
                    return false;
                //end of the design session: unhide the window and propeditor
                window->show();
                if (propeditor)
                    propeditor->show();
                designTable = false;
                continue;
            } else if (command == "removeField") {
                if (!checkItemsNumber(2))
                    return false;
                bool ok;
                int row = getNumber(testFileLine[1], ok) - 1;
                if (!ok)
                    return false;
                designerIface->deleteRow(row, true);
                continue;
            } else if (command == "insertField") {
                if (!checkItemsNumber(3))
                    return false;
                bool ok;
                int row = getNumber(testFileLine[1], ok) - 1;
                if (!ok)
                    return false;
                designerIface->insertField(row, testFileLine[2], true);
                continue;
            } else if (command == "insertEmptyRow") {
                if (!checkItemsNumber(2))
                    return false;
                bool ok;
                int row = getNumber(testFileLine[1], ok) - 1;
                if (!ok)
                    return false;
                designerIface->insertEmptyRow(row, true);
                continue;
            } else if (command == "changeFieldProperty") {
                if (!checkItemsNumber(5) || !changeFieldProperty(designerIface))
                    return false;
                continue;
            } else if (command.startsWith("i=")) {
                bool ok;
                variableI = command.mid(2).toInt(&ok);
                if (!ok) {
                    showError(QString("Invalid variable initialization '%1'").arg(command));
                    return false;
                }
                continue;
            } else if (command.startsWith("i++")) {
                variableI++;
                continue;
            }
        } else {
            //top-level commands available outside of "designTable"
            if (command == "showSchema") {
                if (!checkItemsNumber(1, 2) || !showSchema(window, testFileLine[1] == "clipboard"))
                    return false;
                continue;
            } else if (command == "checkSchema") {
                if (!checkItemsNumber(1) || !checkSchema(window))
                    return false;
                continue;
            } else if (command == "showActions") {
                if (!checkItemsNumber(1, 2) || !showActions(window, testFileLine[1] == "clipboard"))
                    return false;
                continue;
            } else if (command == "checkActions") {
                if (!checkItemsNumber(1) || !checkActions(window))
                    return false;
                continue;
            } else if (command == "saveTableDesign") {
                if (!checkItemsNumber(1) || !saveTableDesign(window))
                    return false;
                continue;
            } else if (command == "showTableData") {
                if (!checkItemsNumber(1, 2) || !showTableData(window, testFileLine[1] == "clipboard"))
                    return false;
                continue;
            } else if (command == "checkTableData") {
                if (!checkItemsNumber(1) || !checkTableData(window))
                    return false;
                continue;
            }
        }
        //common commands
        if (command == "stop") {
            if (!checkItemsNumber(1))
                return false;
            kDebug() << QString("Test STOPPED at line %1.").arg(testLineNumber);
            break;
        } else if (command == "closeWindow") {
            if (!checkItemsNumber(1) || !closeWindow(window))
                return false;
            else
                window = 0;
            continue;
        } else if (command == "quit") {
            if (!checkItemsNumber(1) || !closeWindow(window))
                return false;
            closeAppRequested = true;
            kDebug() << QString("Quitting the application...");
            break;
        } else {
            showError(QString("No such command '%1'").arg(command));
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

int main(int argc, char *argv[])
{
    // args: <.altertable test filename>
    if (argc < 2) {
        kWarning() << "Please specify test filename.\nOptions: \n"
        "\t-close - closes the main window when test finishes";
        return quit(1);
    }

    // options:
    const bool closeOnFinish = argc > 2 && 0 == qstrcmp(argv[1], "-close");

    // open test file
    testFilename = argv[argc-1];
    testFile.setName(testFilename);
    if (!testFile.open(IO_ReadOnly)) {
        kWarning() << QString("Opening test file %1 failed.").arg(testFilename);
        return quit(1);
    }
    //load db name
    testFileStream.setDevice(&testFile);
    tristate res = readLineFromTestFile("openDatabase");
    if (true != res)
        return quit(~res ? 0 : 1);
    origDbFilename = testFileLine[1];
    dbFilename = origDbFilename + ".tmp";

    newArgc = 2;
    newArgv = new char*[newArgc];
    newArgv[0] = qstrdup(argv[0]);
    newArgv[1] = qstrdup("--skip-startup-dialog");

    KAboutData* aboutdata = Kexi::createAboutData();
    aboutdata->setProgramName("Kexi Alter Table Test");
    int result = KexiMainWindow::create(newArgc, newArgv, aboutdata);
    if (!qApp)
        return quit(result);

    win = KexiMainWindow::self();
    AlterTableTester tester;
    //QObject::connect(win, SIGNAL(projectOpened()), &tester, SLOT(run()));

    bool closeAppRequested;
    res = tester.run(closeAppRequested);
    if (true != res) {
        if (false == res)
            kWarning() << QString("Running test for file '%1' failed.").arg(testFilename);
        return quit(res == false ? 1 : 0);
    }
    kDebug() << QString("Tests from file '%1': OK").arg(testFilename);
    result = (closeOnFinish || closeAppRequested) ? 0 : qApp->exec();
    quit(result);
    return result;
}

#include "altertable.moc"
