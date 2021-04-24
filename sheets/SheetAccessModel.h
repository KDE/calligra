/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHEETACCESSMODEL_H
#define SHEETACCESSMODEL_H

#include <QStandardItemModel>

#include "Sheet.h"

namespace Calligra
{
namespace Sheets
{

class Damage;
class Map;
class Sheet;

/**
 * @brief Class that can be used by any shape embedded in Calligra Sheets to access sheet data,
 * without the need to link against Calligra Sheets. It is available through the Doc's data center map,
 * or KoShapeLoadingContext::dataCenterMap() in the process of loading a shape from ODF.
 *
 * Essentially, this model is a list of models to access a sheet's data. It contains a single row,
 * and has exactly one sheet model per column. In short, a model containing models.
 *
 * To allow name-based referencing of a sheet's data (e.g. in an ODF-conform cell region like "Table1.A1:B2")
 * each column's header contains the name of the sheet returned by KSpread::Sheet::sheetName() .
 *
 * To access the QAbstractItemModel instance for a sheet's data, take the following code as example:
 * @code
 * QAbstractItemModel *sheetAccessModel = dynamic_cast<QAbstractItemModel*>( dataCenterMap["SheetAccessModel"] );
 * QModelIndex firstSheetIndex = sheetAccessModel->index( 0, 0 );
 * QPointer<QAbstractItemModel> firstSheet = sheetAccessModel->data( firstSheetIndex ).value< QPointer<QAbstractItemModel> >();
 * view->setModel( firstSheet.data() );
 * @endcode
 */
class SheetAccessModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit SheetAccessModel(Map *map);
    ~SheetAccessModel() override;

public Q_SLOTS:
    void slotSheetAdded(Sheet *sheet);
    void slotSheetRemoved(Sheet *sheet);
        void handleDamages(const QList<Damage*> &damages);

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif
