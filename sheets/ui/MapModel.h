/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_MAP_MODEL
#define CALLIGRA_SHEETS_MAP_MODEL

#include "sheets_ui_export.h"
#include <QAbstractListModel>

class KUndo2Command;

namespace Calligra
{
namespace Sheets
{
class Map;
class Sheet;
class SheetBase;

/**
 * A model for the 'embedded document'.
 * Actually, a model for a sheet list.
 * \ingroup Model
 */
class CALLIGRA_SHEETS_UI_EXPORT MapModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MapModel(Map *map);
    ~MapModel() override;

    // QAbstractItemModel interface
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

public Q_SLOTS:
    /**
     * Hides \p sheet, if \p hidden is \c true and \p sheet is visible.
     * Shows \p sheet, if \p hidden is \c false and \p sheet is hidden.
     * \return \c true on success; \c false on failure
     */
    bool setHidden(Sheet *sheet, bool hidden = true);

Q_SIGNALS:
    void addCommandRequested(KUndo2Command *command);

protected:
    Map *map() const;

protected Q_SLOTS:
    virtual void addSheet(SheetBase *sheet);
    virtual void removeSheet(SheetBase *sheet);

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_MAP_MODEL
