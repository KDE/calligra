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
    virtual unsigned int fieldNumber(const QString &field) const = 0;
    virtual QStringList fieldNames() const = 0;
    virtual QVariant value(unsigned int) const = 0;
    virtual QVariant value(const QString &field) const = 0;

    //Should be called before open() so that the data source can be edited accordingly
    //Default impl does nothing
    virtual void setSorting(SortList) {}


    //!Utility Functions
    //!TODO These are probably eligable to be moved into a new class
    
    //!Allow the reportdata implementation to return a list of possible scripts for a given language
    virtual QStringList scriptList(const QString& language) const {return QStringList();}
    
    //!Allow the reportdata implementation to return some script code based on a specific script name
    //!and a language, as set in the report
    virtual QString scriptCode(const QString& script, const QString& language) const {return QString();}

    //!Return the name of this source
    virtual QString sourceName() const {return QString();}

    //!Return a list of data sources possible for advanced controls
    virtual QStringList dataSources() const {return QStringList();}
    
    //!Allow a driver to create a new instance with a new data source
    //!source is a driver specific identifier
    virtual KoReportData* data(const QString &source){return 0;}

};




#endif

