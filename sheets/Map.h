/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __MAP_H__
#define __MAP_H__

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include "engine/MapBase.h"

#include "ProtectableObject.h"
#include "Region.h"

#include "sheets_export.h"

#include <KoDataCenterBase.h>
#include <KoXmlReader.h>

class KoStore;
class KoStyleManager;
class KoDocumentResourceManager;

class KCompletion;

class QDomElement;
class QDomDocument;
class KUndo2Command;

class KoXmlWriter;

namespace Calligra
{
namespace Sheets
{
class ApplicationSettings;
class BindingManager;
class CalculationSettings;
class ColumnFormat;
class Damage;
class DatabaseManager;
class DependencyManager;
class DocBase;
class LoadingInfo;
class NamedAreaManager;
class RecalcManager;
class RowFormat;
class Sheet;
class Style;
class StyleManager;
class ValueParser;
class ValueConverter;
class ValueFormatter;
class ValueCalc;

/**
 * The "embedded document".
 * The Map holds all the document data.
 */
class CALLIGRA_SHEETS_ODF_EXPORT Map : public MapBase, public QObject, public KoDataCenterBase, public ProtectableObject
{
    Q_OBJECT
public:
    /**
     * Created an empty map.
     */
    explicit Map(DocBase* doc = 0, int syntaxVersion = 1);

    /**
     * This deletes all sheets contained in this map.
     */
    ~Map() override;

    /**
     * \return the document this map belongs to
     */
    DocBase* doc() const;

    /**
     * \brief Sets whether the document can be edited or is read only.
     */
    void setReadWrite(bool readwrite = true);

    /**
     * \return Returns whether the document can be edited or is read only.
     */
    bool isReadWrite() const;

    // KoDataCenterBase interface
    bool completeLoading(KoStore *store) override;
    bool completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext * context) override;

    /**
     * \return a pointer to the binding manager
     */
    BindingManager* bindingManager() const;

    /**
     * \return a pointer to the database manager
     */
    DatabaseManager* databaseManager() const;

    /**
     * \return a pointer to the dependency manager
     */
    DependencyManager* dependencyManager() const;

    /**
     * \return a pointer to the named area manager
     */
    NamedAreaManager* namedAreaManager() const;

    /**
     * \return a pointer to the recalculation manager
     */
    RecalcManager* recalcManager() const;

    /**
     * @return the StyleManager of this Document
     */
    StyleManager* styleManager() const;

    /**
     * @return the KoStyleManager of this Document
     */
    KoStyleManager* textStyleManager() const;

    /**
     * @return the value parser of this Document
     */
    ValueParser* parser() const;

    /**
     * @return the value formatter of this Document
     */
    ValueFormatter* formatter() const;

    /**
     * @return the value converter of this Document
     */
    ValueConverter* converter() const;

    /**
     * @return the value calculator of this Document
     */
    ValueCalc* calc() const;

    /**
     * \return the application settings
     */
    ApplicationSettings* settings() const;

    /**
     * \return the calculation settings
     */
    CalculationSettings* calculationSettings() const;

    /**
     * \return the default row format
     */
    const ColumnFormat* defaultColumnFormat() const;

    /**
     * \return the default row format
     */
    const RowFormat* defaultRowFormat() const;

    /**
     * Sets the default column width to \p width.
     */
    void setDefaultColumnWidth(double width);

    /**
     * Sets the default row height to \p height.
     */
    void setDefaultRowHeight(double height);


    bool loadChildren(KoStore* _store);

    /**
     * The sheet named @p _from is being moved to the sheet @p _to.
     * If @p  _before is true @p _from is inserted before (after otherwise)
     * @p  _to.
     */
    void moveSheet(const QString & _from, const QString & _to, bool _before = true);

    /**
     * Searches for a sheet named @p name .
     * @return a pointer to the searched sheet
     * @return @c 0 if nothing was found
     */
    Sheet* findSheet(const QString& name) const;

    /**
     * @return a pointer to the next sheet to @p sheet
     */
    Sheet* nextSheet(Sheet* sheet) const;

    /**
     * @return a pointer to the previous sheet to @p sheet
     */
    Sheet* previousSheet(Sheet*) const;

    /**
     * Creates a new sheet.
     * The sheet is not added to the map nor added to the GUI.
     * @return a pointer to a new Sheet
     */
    Sheet* createSheet(const QString& name = QString());

    /**
     * Adds @p sheet to this map.
     * The sheet becomes the active sheet.
    */
    void addSheet(Sheet* sheet);

    /**
     * Creates a new sheet.
     * Adds a new sheet to this map.
     * @return a pointer to the new sheet
     */
    Sheet* addNewSheet(const QString& name = QString());

    /**
     * @return a pointer to the sheet at index @p index in this map
     * @return @c 0 if the index exceeds the list boundaries
     */
    Sheet* sheet(int index) const;

    /**
     * @return index of @p sheet in this map
     * @return @c 0 if the index exceeds the list boundaries
     */
    int indexOf(Sheet* sheet) const;

    /**
     * @return the list of sheets in this map
     */
    QList<Sheet*>& sheetList() const;

    /**
     * @return amount of sheets in this map
     */
    int count() const;

    void removeSheet(Sheet* sheet);
    void reviveSheet(Sheet* sheet);

    QStringList visibleSheets() const;
    QStringList hiddenSheets() const;

    void setOverallRowsCounter(int number);
    int increaseLoadedRowsCounter(int i = 1);

    /**
     * \return true if the document is currently loading.
     */
    bool isLoading() const;

    /**
     * \param l whether the document is currently loading
     */
    void setLoading(bool l);

    /**
     * \return the document's syntax version
     * \ingroup NativeFormat
     */
    int syntaxVersion() const;

    /**
     * Sets the document's syntax \p version.
     * \ingroup NativeFormat
     */
    void setSyntaxVersion(int version);

    /**
     * Creates the loading info, if it does not exist yet.
     * \return the loading info
     */
    LoadingInfo* loadingInfo() const;

    /**
     * Deletes the loading info. Called after loading is complete.
     */
    void deleteLoadingInfo();

    /**
     * \return the KCompletion object, that allows user input completions.
     */
    KCompletion &stringCompletion();

    /**
     * Adds \p string to the list of string values in order to be able to
     * complete user inputs.
     */
    void addStringCompletion(const QString &string);

    /**
     * Creates a region consisting of the region defined in @p expression .
     * @param expression a string representing the region (e.g. "A1:B3")
     * @param sheet the fallback sheet, if \p expression does not contain one
     */
    Region & regionFromName(const QString& expression, Sheet* sheet = 0);

    /**
     * @param sRegion will be modified, if a valid sheet was found. The sheetname
     * will be removed
     * @return sheet named in the @p sRegion or null
     */
    Sheet* filterSheetName(QString& sRegion);

    /**
     * \ingroup Damages
     */
    void addDamage(Damage* damage);

    /**
     * Return a pointer to the resource manager associated with the
     * document. The resource manager contains
     * document wide resources * such as variable managers, the image
     * collection and others.
     * @see KoCanvasBase::resourceManager()
     */
    KoDocumentResourceManager *resourceManager() const;
public Q_SLOTS:
    /**
     * \ingroup Damages
     */
    void flushDamages();

    /**
     * \ingroup Damages
     */
    void handleDamages(const QList<Damage*>& damages);

    /**
     * Emits the signal commandAdded(KUndo2Command *).
     * You have to connect the signal to the object holding the undo stack or
     * any relay object, that propagates \p command to the undo stack.
     */
    void addCommand(KUndo2Command *command);

Q_SIGNALS:
    /**
     * \ingroup Damages
     */
    void damagesFlushed(const QList<Damage*>& damages);

    /**
     * Emitted, if a command was added by addCommand(KUndo2Command *).
     */
    void commandAdded(KUndo2Command *command);

    /**
     * Emitted, if a newly created sheet was added to the document.
     */
    void sheetAdded(Sheet* sheet);

    /**
     * Emitted, if a sheet was deleted from the document.
     */
    void sheetRemoved(Sheet* sheet);

    /**
     * Emitted, if a sheet was revived, i.e. a deleted sheet was reinserted.
     */
    void sheetRevived(Sheet* sheet);

    /**
     * Emitted, if a status \p message should be shown in the status bar
     * for \p timeout msecs.
     */
    void statusMessage(const QString &message, int timeout);

private:
    Q_DISABLE_COPY(Map)

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif
