/* Koffice/Kexi report engine
 * Copyright (C) 2007-2010 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#ifndef __KOREPORTDATA_H__
#define __KOREPORTDATA_H__

#include <kexidb/utils.h>

/**

*/
class KoReportData
{

public:
    virtual ~KoReportData() {};

    class Sort
    {
        public:
        enum Order {
            Ascending = 1,
            Descending
        };

        QString field;
        Order order;
        //bool group; //probably not required?
    };
    typedef QList<Sort> SortList;

    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool moveNext() = 0;
    virtual bool movePrevious() = 0;
    virtual bool moveFirst() = 0;
    virtual bool moveLast() = 0;
    virtual long at() const = 0;
    virtual long recordCount() const = 0;
    virtual unsigned int fieldNumber(const QString &field) = 0;
    virtual QStringList fieldNames() = 0;
    virtual QVariant value(unsigned int) = 0;
    virtual QVariant value(const QString &field) = 0;

    //Should be called before open() so that the data source can be edited accordingly
    //Default impl does nothing
    virtual void setSorting(SortList) {}

    //!Special functions only needed by kexidb driver
    //!Needs to return a KexiDB::Connection pointer
    virtual void* connection() const {
        return 0;
    } 
    
    virtual QString source() const {
        return QString();
    }
    
    //!Needs to return a KexiDB::TableOrQuerySchema pointer, only needs implemented if access via scripting is desirbale
    virtual void* schema() const {
        return 0;
    } 
};




#endif

