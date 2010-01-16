/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003-2005 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>

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
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_WORKBOOK_H
#define SWINDER_WORKBOOK_H

#include "ustring.h"

#include <QtCore/QVariant>
#include <string>
#include <map>

namespace Swinder
{

class Store
{
public:
    explicit Store() {}
    virtual ~Store() {}
    
    virtual bool open(const std::string& filename) = 0;
    virtual bool write(const char *data, int size) = 0;
    virtual bool close() = 0;
};

class Sheet;

class Workbook
{
public:

    /**
     * Constructs a new workbook.
     * 
     * @a store An optional implementation of the Store class
     * that is used to write content like images to.
     */
    explicit Workbook(Store* store = 0);

    /**
     * Destroys the workbook.
     */
    virtual ~Workbook();

    /**
     * Returns the used KoStore or NULL if not KoStore was set.
    /*/
    Store* store() const;
    
    /**
     * Clears the workbook, i.e. makes it as if it is just constructed.
     */
    void clear();

    /**
     * Loads the workbook from file. Returns false if error occurred.
     */
    bool load(const char* filename);

    /**
     * Appends a new sheet.
     */
    void appendSheet(Sheet* sheet);

    /**
     * Returns the number of worksheet in this workbook. A newly created
     * workbook has no sheet, i.e. sheetCount() returns 0.
     */
    unsigned sheetCount() const;

    /**
     * Returns a worksheet at given index. If index is invalid (e.g. larger
     * than total number of worksheet), this function returns NULL.
     */
    Sheet* sheet(unsigned index);

    enum PropertyType {
        PIDSI_TITLE = 0x02,
        PIDSI_SUBJECT = 0x03,
        PIDSI_AUTHOR = 0x04,
        PIDSI_KEYWORDS = 0x05,
        PIDSI_COMMENTS = 0x06,
        PIDSI_TEMPLATE = 0x07,
        PIDSI_LASTAUTHOR = 0x08,
        PIDSI_REVNUMBER = 0x09,
        PIDSI_EDITTIME = 0x0a,
        PIDSI_LASTPRINTED_DTM = 0x0b,
        PIDSI_CREATE_DTM = 0x0c,
        PIDSI_LASTSAVED_DTM = 0x0d,
        PIDSI_APPNAME = 0x12,
    };

    bool hasProperty(PropertyType type) const;
    QVariant property(PropertyType type, const QVariant &defaultValue = QVariant()) const;
    void setProperty(PropertyType type, const QVariant &value);

    std::map<UString, UString>& namedAreas();
    void setNamedArea(UString name, UString formula);
    
    int activeTab() const;
    void setActiveTab(int tab);

    bool isPasswordProtected() const;
    void setPasswordProtected(bool p);

    unsigned long password() const;
    void setPassword(unsigned long hash);

private:
    // no copy or assign
    Workbook(const Workbook&);
    Workbook& operator=(const Workbook&);

    class Private;
    Private* d;
};

}


#endif // SWINDER_WORKBOOK_H
