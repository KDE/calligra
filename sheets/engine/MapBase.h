/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __MAPBASE_H__
#define __MAPBASE_H__



#include "sheets_engine_export.h"
#include <QList>


namespace Calligra
{
namespace Sheets
{

class SheetBase;

class CALLIGRA_SHEETS_ENGINE_EXPORT MapBase
{
public:
    explicit MapBase();

    virtual ~MapBase();



    /**
     * @return a pointer to the sheet at index @p index in this map
     * @return @c 0 if the index exceeds the list boundaries
     */
    SheetBase* sheet(int index) const;

    /**
     * @return index of @p sheet in this map
     * @return @c 0 if the index exceeds the list boundaries
     */
    int indexOf(SheetBase* sheet) const;

    /**
     * @return the list of sheets in this map
     */
    QList<SheetBase*>& sheetList() const;

    /**
     * @return amount of sheets in this map
     */
    int count() const;

    /**
     * Searches for a sheet named @p name .
     * @return a pointer to the searched sheet
     * @return @c 0 if nothing was found
     */
    SheetBase* findSheet(const QString& name) const;

    /**
     * @return a pointer to the next sheet to @p sheet
     */
    SheetBase* nextSheet(SheetBase *sheet) const;

    /**
     * @return a pointer to the previous sheet to @p sheet
     */
    SheetBase* previousSheet(SheetBase *) const;


    /**
     * \return true if the document is currently loading.
     */
    virtual bool isLoading() const;

    /**
     * \param l whether the document is currently loading
     */
    void setLoading(bool l);




private:
    // disable copying
    MapBase(const MapBase &) = delete;
    MapBase &operator=(const MapBase &) = delete;

    class Private;
    Private * const d;
};


} // namespace Sheets
} // namespace Calligra


#endif
