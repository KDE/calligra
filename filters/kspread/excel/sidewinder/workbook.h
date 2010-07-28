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

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QRect>
#include <string>
#include <map>

class KoStore;

namespace Swinder
{

class Sheet;
class Format;

class Workbook : public QObject
{
    Q_OBJECT
public:

    /**
     * Constructs a new workbook.
     *
     * @a store An optional implementation of the Store class
     * that is used to write content like images to.
     */
    explicit Workbook(KoStore* store = 0);

    /**
     * Destroys the workbook.
     */
    virtual ~Workbook();

    /**
     * Returns the used KoStore or NULL if not KoStore was set.
    /*/
    KoStore* store() const;

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

    std::map<std::pair<unsigned, UString>, UString>& namedAreas();
    void setNamedArea(unsigned sheet, UString name, UString formula);

    QList<QRect> filterRanges(unsigned sheet) const;
    void addFilterRange(unsigned sheet, const QRect& range);

    int activeTab() const;
    void setActiveTab(int tab);

    bool isPasswordProtected() const;
    void setPasswordProtected(bool p);

    unsigned long password() const;
    void setPassword(unsigned long hash);

    int addFormat(const Format& format);
    Format* format(int index);

    void emitProgress(int value);

#ifdef SWINDER_XLS2RAW
    void dumpStats();
#endif
signals:
    void sigProgress(int value);

private:
    // no copy or assign
    Workbook(const Workbook&);
    Workbook& operator=(const Workbook&);

    class Private;
    Private* d;
};

}


#endif // SWINDER_WORKBOOK_H
