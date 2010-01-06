/* Swinder - Portable library for spreadsheet
   Copyright (C) 2009-2010 Sebastian Sauer <sebsauer@kdab.com>

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
   Boston, MA 02110-1301, USA
 */

#ifndef SWINDER_CHARTSUBSTREAMHANDLER_H
#define SWINDER_CHARTSUBSTREAMHANDLER_H

#include "substreamhandler.h"
#include "globalssubstreamhandler.h"
#include "excel.h"
//#include "ustring.h"
//#include <vector>
//#include "cell.h"
//#include "sheet.h"
//#include <map>

namespace Swinder
{

// class FormulaToken;
// typedef std::vector<FormulaToken> FormulaTokens;

class GlobalsSubStreamHandler;

class ChartSubStreamHandler : public SubStreamHandler
{
public:
    explicit ChartSubStreamHandler(const GlobalsSubStreamHandler* globals) {}
    virtual ~ChartSubStreamHandler() {}

    virtual void handleRecord(Record* record) {
        if (!record) return;
        const unsigned type = record->rtti();

        //if (type == BottomMarginRecord::id)
        //    handleBottomMargin(static_cast<BottomMarginRecord*>(record));
        //else {
            std::cout << "Unhandled chart record with type=" << type << std::endl;
        //}
    }

};

} // namespace Swinder

#endif // SWINDER_CHARTSUBSTREAMHANDLER_H
