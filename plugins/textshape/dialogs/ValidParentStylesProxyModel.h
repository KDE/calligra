/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef VALIDPARENTSTYLESPROXYMODEL_H
#define VALIDPARENTSTYLESPROXYMODEL_H

// #include "AbstractStylesModel.h"
#include "StylesFilteredModelBase.h"

#include <QVector>

class KoStyleManager;

class QModelIndex;

/** This class is acting as a proxy between a "real" StylesModel and views. It is used to filter out styles which would not make a valid parent of the given
 * childStyle (see void setCurrentChildStyleId(int styleId) ).
 *
 * In that matter, QSortFilterProxyModel implementation was a great source of inspiration.
 *
 * It is to be noted that this is in no way a full proxyModel. It is built with several assumptions:
 * - it is used to filter a StylesModel which in turn is a flat list of items. There is only one level of items. (this also means that "parent" QModelIndexes
 * are always invalid)
 * - there is no header in the model.
 * - the model has only one column
 * - only the following methods are used when updating the underlying model's data: resetModel, insertRows, moveRows, removeRows (cf QAbstractItemModel)
 */

class ValidParentStylesProxyModel : public StylesFilteredModelBase
{
    Q_OBJECT
public:
    explicit ValidParentStylesProxyModel(QObject *parent = nullptr);

    void setStyleManager(KoStyleManager *manager); // it is needed to travel the style's parenting chain in order to assess if the currentChildStyle is suitable
                                                   // for being a parent
    void setCurrentChildStyleId(int styleId);

protected:
    void createMapping() override;

private:
    KoStyleManager *m_styleManager;
    int m_currentChildStyleId;
};

#endif // VALIDPARENTSTYLESPROXYMODEL_H
