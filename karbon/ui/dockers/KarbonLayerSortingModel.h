/* This file is part of the KDE project
 * Copyright (C) 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KARBON_LAYER_SORTING_MODEL_H
#define KARBON_LAYER_SORTING_MODEL_H

#include <QtGui/QSortFilterProxyModel>

class KarbonDocument;

class KarbonLayerSortingModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    KarbonLayerSortingModel(QObject * parent);
    /// Sets a new document to use for sorting
    void setDocument(KarbonDocument * newDocument);
protected:
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
private:
    KarbonDocument *m_document; ///< the underlying data structure
};

#endif // KARBON_LAYER_SORTING_MODEL_H
