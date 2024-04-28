/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_MAP_VIEW_MODEL
#define CALLIGRA_SHEETS_MAP_VIEW_MODEL

#include "MapModel.h"

class KoCanvasBase;
class KoShape;

class KXMLGUIClient;

class QAction;

namespace Calligra
{
namespace Sheets
{

/**
 * Extends the map model by active sheet tracking.
 *
 * KPART_TODO: fix the actions in a document with multiple sheets, we used to call
 *             d->xmlGuiClient->plugActionList("go_goto_sheet_actionlist", actions);
 *             in the eventFilter.
 */
class CALLIGRA_SHEETS_UI_EXPORT MapViewModel : public MapModel
{
    Q_OBJECT
public:
    MapViewModel(Map *map, KoCanvasBase *canvas, KXMLGUIClient *xmlGuiClient);
    ~MapViewModel() override;

    // QAbstractItemModel interface
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Sheet *activeSheet() const;

public Q_SLOTS:
    /**
     * Set the active \p sheet and emits activeSheetChanged(Sheet*) afterwards.
     */
    void setActiveSheet(Sheet *sheet);

    /**
     * Adds the \p shape, if \p sheet is active.
     */
    void addShape(Sheet *sheet, KoShape *shape);

    /**
     * Removes the \p shape, if \p sheet is active.
     */
    void removeShape(Sheet *sheet, KoShape *shape);

private Q_SLOTS:
    /**
     * Adds \p sheet to the goto sheet actions.
     */
    void addSheet(SheetBase *sheet) override;

    /**
     * Removes \p sheet from the goto sheet actions.
     */
    void removeSheet(SheetBase *sheet) override;

    /**
     * Activates the associated sheet of the \p action.
     */
    void gotoSheetActionTriggered(QAction *action);

Q_SIGNALS:
    void activeSheetChanged(Sheet *sheet);

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_MAP_VIEW_MODEL
