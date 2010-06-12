/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_MAP_VIEW_MODEL
#define KSPREAD_MAP_VIEW_MODEL

#include "MapModel.h"

class KoCanvasBase;
class KoShape;

class KXMLGUIClient;

class QAction;

namespace KSpread
{

/**
 * Extends the map model by active sheet tracking.
 */
class MapViewModel : public MapModel
{
    Q_OBJECT
public:
    MapViewModel(Map *map, KoCanvasBase *canvas, KXMLGUIClient *xmlGuiClient);
    virtual ~MapViewModel();

    // QAbstractItemModel interface
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Sheet* activeSheet() const;

public Q_SLOTS:
    /**
     * Set the active \p sheet and emits activeSheetChanged(Sheet*) afterwards.
     */
    void setActiveSheet(Sheet* sheet);

protected:
    /**
     * Plugs the action lists in, if a KParts::GUIActivateEvent is received.
     * \return always \c false
     */
    bool eventFilter(QObject *object, QEvent *event);

private Q_SLOTS:
    /**
     * Adds \p sheet to the goto sheet actions.
     */
    virtual void addSheet(Sheet *sheet);

    /**
     * Removes \p sheet from the goto sheet actions.
     */
    virtual void removeSheet(Sheet *sheet);

    /**
     * Adds the \p shape, if \p sheet is active.
     */
    void addShape(Sheet *sheet, KoShape *shape);

    /**
     * Removes the \p shape, if \p sheet is active.
     */
    void removeShape(Sheet *sheet, KoShape *shape);

    /**
     * Activates the associated sheet of the \p action.
     */
    void gotoSheetActionTriggered(QAction *action);

Q_SIGNALS:
    void activeSheetChanged(Sheet* sheet);

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_MAP_VIEW_MODEL
