/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <kcmdlineargs.h>
#include <klocale.h>

static KCmdLineOptions options[] =
{
  { ":", I18N_NOOP("Options related to entire projects:"), 0 },
  { "createdb", I18N_NOOP(
	"Create a new, blank project using specified\n"
	"database driver and database name\n"
	"and exit immediately.\n"
	"You will be asked for confirmation\n"
	"if overwriting is needed."), 0 },
  { "create-opendb", I18N_NOOP(
	"Like --createdb, but also open newly\n"
	"created database.\n"), 0 },
  { "dropdb", I18N_NOOP(
	"Drop (remove) a project using specified\n"
	"database driver and database name.\n"
	"You will be asked for confirmation."), 0 },
  { "drv", 0, 0 },
  { "dbdriver <name>", I18N_NOOP(
	"Database driver to be used\n"
	"for connecting to a database project\n"
	"(SQLite by default).\n"
	"Ignored if a shortcut filename\n"
	"is provided."), 0 },
  { "t", 0, 0 },
  { "type <name>", I18N_NOOP(
	"Specifies a type of a file provided\n"
	"as argument. This option is useful\n"
	"if your filename has not set a valid \n"
	"extension and it's type cannot be detected\n"
	"unambiguously by looking at it's contents.\n"
	"This option is ignored if no file\n"
	"is specified as an argument.\n"
	"Available file types are:\n"
	"- \"project\" for a project file (the default)\n"
	"- \"shortcut\" for a shortcut file pointing\n"
	"  you to a project.\n"
	"- \"connection\" for a database connection data.\n"
	), 0 },

  { ":", I18N_NOOP("Options related to opening objects within a project:"), 0 },
  { "open [<object_type>:]<object_name>", I18N_NOOP(
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
	" --open query:\"My very big query\""), 0 },
  { "design [<object_type>:]<object_name>", I18N_NOOP(
	"\nLike --open, but the object will\n"
	"be opened in Design Mode, if one is available"), 0 },
  { "edittext [<object_type>:]<object_name>", I18N_NOOP(
	"\nLike --open, but the object will\n"
	"be opened in Text Mode, if one is available"), 0 },
  { "new <object_type>", I18N_NOOP(
	"Start new object design of type <object_type>"), 0 },
  { "print [<object_type>:]<object_name>", I18N_NOOP(
	"\nOpen object of type <object_type>\n"
	"and name <object_name> from specified project\n"
	"on application start for quick printing\n"
	"of the object's data.\n"
	"<object_type>: is optional, if omitted - table\n"
	"type is assumed.\n"
	"Other object types can be query, report, form,\n"
	"script (may be more or less, depending on your\n"
	"plugins installed).\n"
	"Use \"\" chars to specify names containing spaces.\n"
	"Examples: --print MyTable,\n"
	" --print query:\"My very big query\""), 0 },
  { "printpreview [<object_type>:]<object_name>", I18N_NOOP(
	"\nOpen Print Preview window for object\n"
	"of type <object_type> and name <object_name>\n"
	"from specified project on application start.\n"
	"See --print for more details"), 0 },
#ifdef KEXI_SHOW_UNIMPLEMENTED
  { "final-mode", I18N_NOOP(
	"Start project in the Final Mode, regardless \n"
	"of the project settings"), 0 },
  { "design-mode", I18N_NOOP(
	"Start project in the Design Mode, regardless \n"
	"of the project settings"), 0 },
#endif

#ifdef KEXI_SERVER_SUPPORT
  { ":", I18N_NOOP("Options related to database servers:"), 0 },
  { "u", 0, 0 },
  { "user <name>", I18N_NOOP(
	"User name to be used\n"
	"for connecting to a database project.\n"
	"Ignored if a shortcut filename\n"
	"is provided."), 0 },
/*  { "password <password>", I18N_NOOP(
	"User password to be used\n"
	"for connecting to a database project.\n"
	"Ignored if a shortcut filename\n"
	"is provided."), 0 },*/
  { "h", 0, 0 },
  { "host <name>", I18N_NOOP(
	"Server (host) name to be used\n"
	"for connecting to a database project.\n"
	"Ignored if a shortcut filename\n"
	"is provided."), 0 },
  { "port <number>", I18N_NOOP(
	"Server's port number to be used\n"
	"for connecting to a database project.\n"
	"Ignored if a shortcut filename\n"
	"is provided."), 0 },
  { "local-socket <filename>", I18N_NOOP(
	"Server's local socket filename\n"
	"to be used for connecting to a database\n"
	"project. Ignored if a shortcut filename\n"
	"is provided."), 0 },
#endif //KEXI_SERVER_SUPPORT

  { "+[project-name]", I18N_NOOP(
	"Kexi database project's filename\n"
	"or Kexi shortcut's filename\n"
	"or Kexi database project's name\n"
	"on a server to open"), 0 },
  // INSERT YOUR COMMANDLINE OPTIONS HERE
  KCmdLineLastOption
};
