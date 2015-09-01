/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#include "KexiCommandLineOptions.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KUser>

#include <QCoreApplication>
#include <QCommandLineParser>

KexiCommandLineOptions::KexiCommandLineOptions(QCommandLineParser *parser)
  : /*help(parser->addHelpOption()),
    version(parser->addVersionOption()),*/

    // NOTE: REMEMBER TO ADD NEW OPTIONS IN KexiStartupData::parseOptions()

    // Options related to entire projects:
    createDb("createdb",
        xi18nc("'createdb' command line option",
               "Create a new, blank project using specified database driver and database "
               "name and exit immediately. You will be asked for confirmation if "
               "overwriting is needed.")),
    createAndOpenDb("create-opendb",
        xi18nc("'create-opendb' command line option",
               "Like --createdb, but also open newly created database.")),
    dropDb("dropdb",
        xi18nc("'dropdb' command line option",
               "Drop (remove) a project using specified database driver and database name. "
               "You will be asked for confirmation.")),
    dbDriver(QStringList() << "drv" << "dbdriver",
       xi18nc("'dbdriver' command line option",
              "Name of a database driver to be used when connecting to a database project "
              "(\"sqlite\" by default). Ignored if a shortcut filename is provided. "
              "Complete KDb-specific globally unique identifier can be used, "
              "e.g. \"org.kde.kdb.sqlite\" to specify exact vendor of the driver."),
       "name_or_id"),
    fileType(QStringList() << "t" << "type",
        xi18nc("'type' command line option",
               "Specify the type of file provided as an argument. This option is only "
               "useful if the filename does not have a valid extension set and its type "
               "cannot be determined unambiguously by examining its contents. This option "
               "is ignored if no file is specified as an argument.\n"
               "Available file types are:\n"
               "- \"project\" for a project file (the default)\n"
               "- \"shortcut\" for a shortcut file pointing to a\n"
               "  project.\n"
               "- \"connection\" for database connection data."),
        "name"),
    connectionShortcut(QStringList() << "conn" << "connection",
        xi18nc("'connection' command line option",
               "Specify a database connection shortcut .kexic file containing connection data. "
               "Can be used with --createdb or --create-opendb for convenience instead "
               "of using options such as --user, --host or --port.\n"
               "Note: Options like --user, --host have precedence over settings defined "
               "in the shortcut file."),
        "shortcut_filename"),
    readOnly("readonly",
        xi18nc("'readonly' command line option",
               "Specify that any database connections will be performed without write support. "
               "This option is ignored when \"createdb\" option is present, otherwise the "
               "database could not be created.")),
    userMode("user-mode",
        xi18nc("'user-mode' command line option",
               "Start project in User Mode, regardless of the project settings.")),
    designMode("design-mode",
        xi18nc("'design-mode' command line option",
               "Start project in Design Mode, regardless of the project settings.")),
    showNavigator("show-navigator",
        xi18nc("'show-navigator' command line option",
               "Show the Project Navigator side pane even if Kexi runs in User Mode.")),
    hideMenu("hide-menu",
        xi18nc("'hide-menu' command line option",
               "Hide the main menu (the tabbed toolbar) completely. A number of commands "
               "from the main menu is still visible. This option is useful in User Mode.")),

    // Options related to opening objects within a project:
    open("open",
        xi18nc("'open' command line option",
               "Open object of type 'object_type' and name 'object_name' from specified "
               "project on application start. 'object_type' is optional, if omitted - %1 "
               "type is assumed. Other object types can be %2, %3, %4, %5. "
               "There may by more or less types available depending on Kexi plugins "
               "installed.\n"
               "Use \"\" characters to specify names containing spaces.\n"
               "Examples: --open MyTable, --open %2:\"My very big query\"",
               "table", "query", "form", "report", "script"),
        "[object_type:]object_name"),
    design("design",
        xi18nc("'design' command line option",
               "Like --open, but the object will be opened in Design Mode, if one is available."),
        "[object_type:]object_name"),
    editText("edittext",
        xi18nc("'edittext' command line option",
               "Like --open, but the object will be opened in Text Mode, if one is available."),
        "[object_type:]object_name"),
    execute(QStringList() << "exec" << "execute",
        xi18nc("'execute' command line option",
               "Start execution of object of type 'object_type' and name 'object_name' on "
               "application start. 'object_type' is optional, if omitted - %1 type is "
               "assumed. Object type can be also %2. There may by more or less types "
               "available depending on Kexi plugins installed.\n"
               "Use \"\" characters to specify names containing spaces.",
               "macro", "script"),
        "[object_type:]object_name"),
    newObject("new",
        xi18nc("'new' command line option",
               "Start design of a new object of type 'object_type'.")),
    print("print",
        xi18nc("'print' command line option",
               "Open the Print dialog window for an object of type 'object_type' and "
               "name 'object_name' in the specified project when the application starts "
               "for quick printing of the object's data. 'object_type' is optional; "
               "if omitted, %1 type is assumed. Object type can also be %2.",
               "table", "query"),
        "[object_type:]object_name"),
    printPreview("print-preview",
        xi18nc("'print-preview' command line option",
               "Open the Print Preview window for an object of type 'object_type' and "
               "name 'object_name' in the specified project when the application starts "
               "to see preview of the object's data printout. 'object_type' is optional; "
               "if omitted, %1 type is assumed. Object type can also be %2.",
               "table", "query"),
        "[object_type:]object_name"),

    // Options related to database servers:
    user(QStringList() << "u" << "user",
        xi18nc("'user' command line option",
               "Database server's user name whem connecting to a project. Ignored if the "
               "project is opened using a shortcut file. Default user name is the same "
               "as the current login (\"%1\").",
               KUser().loginName())),
    host(QStringList() << "h" << "host",
        xi18nc("'host' command line option",
               "Network server's (host) name to be used when connecting to a database "
               "project. Ignored if the project is opened using a shortcut file. Default "
               "host is the local computer."),
        "name"),
    port("port",
        xi18nc("'port' command line option",
               "Network server's port number to be used when connecting to a database "
               "project. Ignored if the project is opened using a shortcut file. "
               "Defaults depend on the used server type (e.g. %1, %2).",
               "MySQL", "PostgreSQL"),
        "number"),
    localSocket(QStringList() << "socket" << "local-socket",
        xi18nc("'local-socket' command line option",
               "Local computer's socket filename to be used when connecting to "
               "a database project. Ignored if the project is opened using a shortcut file. "
               "Defaults depend on the used server type (e.g. %1, %2).",
               "MySQL", "PostgreSQL"),
        "filename"),

    // Options related to the GUI:
    skipConnDialog("skip-conn-dialog",
        xi18nc("'skip-conn-dialog' command line option",
               "Skip displaying connection dialog window and connect directly. Available "
               "when opening .kexic or .kexis shortcut files.")),
    fullScreen(QStringList() << "f" << "fullscreen",
        xi18nc("'fullscreen' command line option",
               "Start Kexi in full screen mode to occupy the whole screen area by hiding "
               "window decorations such as title bars.")),

    // Options that display configuration or state of Kexi installation.
    // When used, Kexi immediately exits without showing the GUI even if other options
    // or arguments are present.
    listPlugins("list-plugins",
      xi18nc("'list-plugins' command line option",
             "Displays list of plugins available for Kexi with their name, description, "
             "version and filenames."))
{
}
