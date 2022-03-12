/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __MAP_H__
#define __MAP_H__

#include <QString>
#include <QStringList>

#include "engine/MapBase.h"

#include "sheets_core_export.h"
#include "ProtectableObject.h"

#include <KoDataCenterBase.h>

class KoStore;
class KoStyleManager;
class KoDocumentResourceManager;

class KUndo2Command;

namespace Calligra
{
namespace Sheets
{
class ApplicationSettings;
class BindingManager;
class ColFormat;
class DocBase;
class LoadingInfo;
class RowFormat;
class Sheet;
class SheetAccessModel;
class Style;
class StyleManager;
class ValueFormatter;

/**
 * The "embedded document".
 * The Map holds all the document data.
 */
class CALLIGRA_SHEETS_CORE_EXPORT Map : public MapBase, public KoDataCenterBase, public ProtectableObject
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
     * \return a pointer to the application settings
     */
    ApplicationSettings* applicationSettings() const;

    /**
     * \return a pointer to the binding manager
     */
    BindingManager* bindingManager() const;

    /**
     * @return the StyleManager of this Document
     */
    StyleManager* styleManager() const;

    /**
     * @return the KoStyleManager of this Document
     */
    KoStyleManager* textStyleManager() const;

    /**
     * @return the value formatter of this Document
     */
    ValueFormatter* formatter() const;

    /**
     * \return the default row format
     */
    const ColFormat defaultColumnFormat() const;

    /**
     * \return the default row format
     */
    const RowFormat defaultRowFormat() const;

    /**
     * Sets the default column width to \p width.
     */
    void setDefaultColumnWidth(double width);

    /**
     * Sets the default row height to \p height.
     */
    void setDefaultRowHeight(double height);


    /**
     * Creates a new sheet.
     * The sheet is not added to the map nor added to the GUI.
     * @return a pointer to a new Sheet
     */
    Sheet* createSheet(const QString& name = QString());

    /**
     * Creates a new sheet.
     * Adds a new sheet to this map.
     * @return a pointer to the new sheet
     */
    Sheet* addNewSheet(const QString& name = QString());

    QStringList visibleSheets() const;
    QStringList hiddenSheets() const;

    void setOverallRowsCounter(int number);
    int increaseLoadedRowsCounter(int i = 1);

    /**
     * \return true if the document is currently loading.
     */
    virtual bool isLoading() const override;
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

    SheetAccessModel *sheetAccessModel() const;

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
    virtual void handleDamages(const QList<Damage*>& damages) override;

    /**
     * Emits the signal commandAdded(KUndo2Command *).
     * You have to connect the signal to the object holding the undo stack or
     * any relay object, that propagates \p command to the undo stack.
     */
    void addCommand(KUndo2Command *command);

Q_SIGNALS:

    /**
     * Emitted, if a command was added by addCommand(KUndo2Command *).
     */
    void commandAdded(KUndo2Command *command);

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
