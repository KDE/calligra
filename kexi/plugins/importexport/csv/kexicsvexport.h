/* This file is part of the KDE project
   Copyright (C) 2005,2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXI_CSVEXPORT_H
#define KEXI_CSVEXPORT_H

#include <kexidb/utils.h>

class QTextStream;

namespace KexiCSVExport
{

//! Exporting mode: a file or clipboard
enum Mode { Clipboard, File };

//! Options used in KexiCSVExportWizard contructor.
class Options
{
public:
    Options();

    //! Assigns \a args. \return false on failure.
    bool assign(QMap<QString, QString>& args);

    Mode mode;
    int itemId; //!< Table or query ID
    QString fileName;
    QString delimiter;
    QString forceDelimiter; //!< Used for "clipboard" mode
    QString textQuote;
    bool addColumnNames : 1;
};

/*! Exports data. \return false on failure.
 @param options options for the export
 @param rowCount row count of the input data or -1 if the row cound has not yet been computed
 @param predefinedTextStream text stream that should be used instead of writing to a file
*/
bool exportData(KexiDB::TableOrQuerySchema& tableOrQuery, const Options& options,
                int rowCount = -1,  QTextStream *predefinedTextStream = 0);

}

#endif
