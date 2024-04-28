/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SWINDER_SUBSTREAMHANDLER_H
#define SWINDER_SUBSTREAMHANDLER_H

namespace Swinder
{

class Record;

class SubStreamHandler
{
public:
    virtual ~SubStreamHandler();

    virtual void handleRecord(Record *record) = 0;
};

} // namespace Swinder

#endif // SWINDER_SUBSTREAMHANDLER_H
