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
#include <QObject>

namespace Calligra
{
namespace Sheets
{

class CalculationSettings;
class Damage;
class DependencyManager;
class NamedAreaManager;
class RecalcManager;
class Region;
class SheetBase;
class ValueCalc;
class ValueConverter;
class ValueParser;

class CALLIGRA_SHEETS_ENGINE_EXPORT MapBase : public QObject
{
    Q_OBJECT
public:
    explicit MapBase();

    virtual ~MapBase();

    /**
     * @return a pointer to the sheet at index @p index in this map
     * @return @c 0 if the index exceeds the list boundaries
     */
    SheetBase *sheet(int index) const;

    /**
     * @return index of @p sheet in this map
     * @return @c 0 if the index exceeds the list boundaries
     */
    int indexOf(SheetBase *sheet) const;

    /**
     * @return the list of sheets in this map
     */
    QList<SheetBase *> &sheetList() const;

    /**
     * @return amount of sheets in this map
     */
    int count() const;

    /**
     * Searches for a sheet named @p name .
     * @return a pointer to the searched sheet
     * @return @c 0 if nothing was found
     */
    SheetBase *findSheet(const QString &name) const;

    /**
     * The sheet named @p _from is being moved to the sheet @p _to.
     * If @p  _before is true @p _from is inserted before (after otherwise)
     * @p  _to.
     */
    void moveSheet(const QString &_from, const QString &_to, bool _before = true);

    /**
     * @return a pointer to the next sheet to @p sheet
     */
    SheetBase *nextSheet(SheetBase *sheet) const;

    /**
     * @return a pointer to the previous sheet to @p sheet
     */
    SheetBase *previousSheet(SheetBase *) const;

    /**
     * Adds @p sheet to this map.
     */
    void addSheet(SheetBase *sheet);

    /**
     * Creates a new sheet.
     * The sheet is not added to the map nor added to the GUI.
     * @return a pointer to a new Sheet
     */
    virtual SheetBase *createSheet(const QString &name = QString());

    /**
     * Creates a new sheet.
     * Adds a new sheet to this map.
     * @return a pointer to the new sheet
     */
    SheetBase *addNewSheet(const QString &name = QString());

    /**
     * Actions when a sheet is hidden, or shown.
     */
    void onSheetHidden(SheetBase *sheet, bool hidden);

    /**
     * \return true if the document is currently loading.
     */
    virtual bool isLoading() const;

    /**
     * \param l whether the document is currently loading
     */
    void setLoading(bool l);

    /**
     * @return the value parser of this Document
     */
    ValueParser *parser() const;

    /**
     * @return the value converter of this Document
     */
    ValueConverter *converter() const;

    /**
     * @return the value calculator of this Document
     */
    ValueCalc *calc() const;

    /**
     * \return a pointer to the dependency manager
     */
    DependencyManager *dependencyManager() const;

    /**
     * \return a pointer to the named area manager
     */
    NamedAreaManager *namedAreaManager() const;

    /**
     * \return a pointer to the recalculation manager
     */
    RecalcManager *recalcManager() const;

    /**
     * \return the calculation settings
     */
    CalculationSettings *calculationSettings() const;

    /**
     * Convenience wrapper for namedAreaManager()->contains
     * */
    bool isNamedArea(const QString &name);

    /**
     * \ingroup Damages
     */
    void addDamage(Damage *damage);

    QStringList visibleSheets() const;
    QStringList hiddenSheets() const;

    /**
     * Creates a region consisting of the region defined in @p expression .
     * @param expression a string representing the region (e.g. "A1:B3")
     * @param sheet the fallback sheet, if \p expression does not contain one
     */
    Region regionFromName(const QString &expression, SheetBase *sheet = nullptr) const;
    /**
     * @param sRegion will be modified, if a valid sheet was found. The sheetname
     * will be removed
     * @return sheet named in the @p sRegion or null
     */
    SheetBase *filterSheetName(QString &sRegion) const;

    void removeSheet(SheetBase *sheet);
    void reviveSheet(SheetBase *sheet);

Q_SIGNALS:
    /**
     * \ingroup Damages
     */
    void damagesFlushed(const QList<Damage *> &damages);

    /**
     * Emitted, if a newly created sheet was added to the document.
     */
    void sheetAdded(SheetBase *sheet);

    /**
     * Emitted, if a sheet was deleted from the document.
     */
    void sheetRemoved(SheetBase *sheet);

    /**
     * Emitted, if a sheet was revived, i.e. a deleted sheet was reinserted.
     */
    void sheetRevived(SheetBase *sheet);

    /**
     * Emitted if a sheet has been hidden.
     */
    void sheetHidden(SheetBase *sheet);

    /**
     * Emitted if a sheet has been shown.
     */
    void sheetShown(SheetBase *sheet);

public Q_SLOTS:
    /**
     * \ingroup Damages
     */
    void flushDamages();

    /**
     * \ingroup Damages
     */
    virtual void handleDamages(const QList<Damage *> &damages);

private:
    // disable copying
    MapBase(const MapBase &) = delete;
    MapBase &operator=(const MapBase &) = delete;

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif
