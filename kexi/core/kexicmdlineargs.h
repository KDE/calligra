/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXICMDLINEARGS_H
#define KEXICMDLINEARGS_H

#include <kcmdlineargs.h>
#include <klocale.h>

static KCmdLineOptions kexi_options ()
{
    KCmdLineOptions options;
    options
    .add(":", ki18n("Options related to entire projects:"))
    .add("createdb", ki18n(
        "Create a new, blank project using specified\n"
        "database driver and database name\n"
        "and exit immediately.\n"
        "You will be asked for confirmation\n"
        "if overwriting is needed."))
    .add("create-opendb", ki18n(
        "Like --createdb, but also open newly\n"
        "created database.\n"))
    .add("dropdb", ki18n(
        "Drop (remove) a project using specified\n"
        "database driver and database name.\n"
        "You will be asked for confirmation."))
    .add("drv")
    .add("dbdriver <name>", ki18n(
        "Database driver to be used\n"
        "for connecting to a database project\n"
        "(SQLite by default).\n"
        "Ignored if a shortcut filename\n"
        "is provided."))
    .add("t")
    .add("type <name>", ki18n(
        "Specify the type of file provided as an argument.\n"
        "This option is only useful if the filename does\n"
        "not have a valid extension set and its type\n"
        "cannot be determined unambiguously by examining\n"
        "its contents.\n"
        "This option is ignored if no file is specified as\n"
        "an argument.\n"
        "Available file types are:\n"
        "- \"project\" for a project file (the default)\n"
        "- \"shortcut\" for a shortcut file pointing to a\n"
        "  project.\n"
        "- \"connection\" for database connection data.\n"
        ))
    .add("conn")
    .add("connection <shortcut_filename>", ki18n(
        "\nSpecify a database connection shortcut .kexic\n"
        "file containing connection data.\n"
        "Can be used with --createdb or --create-opendb\n"
        "for convenience instead of using options like \n"
        "--user, --host or --port.\n"
        "Note: Options like --user, --host have\n"
        "precedence over settings defined in the shortcut\n"
        "file." ))
    .add("readonly", ki18n(
        "Specify that any database connections will\n"
        "be performed without write support. This option\n"
        "is ignored when \"createdb\" option is present,\n"
        "otherwise the database could not be created."))
    .add("user-mode", ki18n(
        "Start project in User Mode, regardless \n"
        "of the project settings."))
    .add("design-mode", ki18n(
        "Start project in Design Mode, regardless \n"
        "of the project settings."))
    .add("show-navigator", ki18n(
        "Show the Project Navigator side pane even\n"
        "if Kexi runs in User Mode."))
    .add("skip-startup-dialog", ki18n(
        "Skip displaying startup dialog window.\n"
        "If there is no project name specified to open,\n"
        "empty application window will appear."))
    .add(":", ki18n("Options related to opening objects within a project:"))
    .add("open [<object_type>:]<object_name>", ki18n(
        "\nOpen object of type <object_type>\n"
        "and name <object_name> from specified project\n"
        "on application start.\n"
        "<object_type>: is optional, if omitted - table\n"
        "type is assumed.\n"
        "Other object types can be query, report, form,\n"
        "script (may be more or less, depending on your\n"
        "plugins installed).\n"
        "Use \"\" chars to specify names containing spaces.\n"
        "Examples: --open MyTable,\n"
        " --open query:\"My very big query\""))
    .add("design [<object_type>:]<object_name>", ki18n(
        "\nLike --open, but the object will\n"
        "be opened in Design Mode, if one is available."))
    .add("edittext [<object_type>:]<object_name>", ki18n(
        "\nLike --open, but the object will\n"
        "be opened in Text Mode, if one is available."))
    .add("exec")
    .add("execute [<object_type>:]<object_name>", ki18n(
        "\nStart execution of object of type <object_type>\n"
        "and name <object_name> on application start.\n"
        "<object_type>: is optional, if omitted - macro\n"
        "type is assumed.\n"
        "Other object types can be script (may be more\n"
        "or less, depending on your plugins installed).\n"
        "Use \"\" chars to specify names containing spaces."))
    .add("new <object_type>", ki18n(
        "Start new object design of type <object_type>."))
    .add("print [<object_type>:]<object_name>", ki18n(
        "\nOpen the Print dialog window for an object of type\n"
        "<object_type> and name <object_name> in the specified\n"
        "project when the application starts, for quick printing\n"
        "of the object's data.\n"
        "<object_type>: is optional; if omitted, table\n"
        "type is assumed. Object type can also be query."))
    .add("print-preview [<object_type>:]<object_name>", ki18n(
        "\nOpen Print Preview window for object\n"
        "of type <object_type> and name <object_name>\n"
        "from specified project on application start.\n"
        "See --print for more details."))
    .add(":", ki18n("Options related to database servers:"))
    .add("u")
    .add("user <name>", ki18n(
        "User name to be used\n"
        "for connecting to a database project.\n"
        "Ignored if a shortcut filename\n"
        "is provided."))
    /* .add("password <password>", ki18n(
    "User password to be used\n"
    "for connecting to a database project.\n"
    "Ignored if a shortcut filename\n"
    "is provided.")) */
    .add("h")
    .add("host <name>", ki18n(
        "Server (host) name to be used\n"
        "for connecting to a database project.\n"
        "Ignored if a shortcut filename\n"
        "is provided."))
    .add("port <number>", ki18n(
        "Server's port number to be used\n"
        "for connecting to a database project.\n"
        "Ignored if a shortcut filename\n"
        "is provided."))
    .add("local-socket <filename>", ki18n(
        "Server's local socket filename\n"
        "to be used for connecting to a database\n"
        "project. Ignored if a shortcut filename\n"
        "is provided."))
    .add("skip-conn-dialog", ki18n(
        "Skip displaying connection dialog window\n"
        "and connect directly. Available when\n"
        "opening .kexic or .kexis shortcut files."))
    .add("+[project-name]", ki18n(
        "Kexi database project filename,\n"
        "Kexi shortcut filename,\n"
        "or name of a Kexi database\n"
        "project on a server to open."))
    ;
    return options;
}

#endif
