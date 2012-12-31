/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

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

#include <iostream>
#include <string>

#include <kexi_version.h>
#include <sqlite3.h>

using namespace std;

extern "C" int shell_main(const char *inFilename);

#define JOIN(a, b) a#b
#define STRINGIFY(s) JOIN("", s) // indirection needed because only function-like macro parameters can be pasted

void usage()
{
    cout << STRINGIFY(KEXI_SQLITE_DUMP_TOOL) " version " KEXI_VERSION_STRING << endl
         << "A tool for executing DUMP command on SQLite 3 databases." << endl
         << "Usage: " STRINGIFY(KEXI_SQLITE_DUMP_TOOL) " <database>" << endl;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        usage();
        return 1;
    }
    string arg(argv[1]);
    if (arg == "-h" || arg == "--help") {
        usage();
        return 0;
    }

    return shell_main(arg.c_str());
}
