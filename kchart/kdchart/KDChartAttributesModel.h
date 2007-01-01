/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef __KDCHART_ATTRIBUTES_MODEL_H__
#define __KDCHART_ATTRIBUTES_MODEL_H__

#include "KDChartAbstractProxyModel.h"
#include <QMap>
#include <QVariant>

#include "KDChartGlobal.h"

namespace KDChart {

class KDCHART_EXPORT AttributesModel : public AbstractProxyModel
{
    Q_OBJECT
public:
    enum PaletteType {
        PaletteTypeDefault = 0,
        PaletteTypeRainbow = 1,
        PaletteTypeSubdued = 2
    };

    explicit AttributesModel( QAbstractItemModel* model, QObject * parent = 0 );
    ~AttributesModel();

    /* Copies the internal data (maps and palette) of another
       AttributesModel* into this one.
    */
    void initFrom( const AttributesModel* other );

    /* Attributes Model specific API */
    bool setModelData( const QVariant value, int role );
    QVariant modelData( int role ) const;

    /** Returns whether the given role corresponds to one of the known
     * internally used ones. */
    bool isKnownAttributesRole( int role ) const;

    /** Sets the palettetype used by this attributesmodel */
    void setPaletteType( PaletteType type );
    PaletteType paletteType() const;

    /** Returns the data that were specified at global level,
      * or the default data, or QVariant().
      */
    QVariant data(int role) const;

    /** Returns the data that were specified at per column level,
      * or the globally set data, or the default data, or QVariant().
      */
    QVariant data(int column, int role) const;

    /** \reimpl */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    /** \reimpl */
    int rowCount(const QModelIndex& ) const;
    /** \reimpl */
    int columnCount(const QModelIndex& ) const;
    /** \reimpl */
    QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
    /** \reimpl */
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::DisplayRole);
    /** \reimpl */
    bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value,
                         int role = Qt::DisplayRole);
    /** \reimpl */
    void setSourceModel ( QAbstractItemModel* sourceModel );

Q_SIGNALS:
    void attributesChanged( const QModelIndex&, const QModelIndex& );

private:
    // helper
    QVariant defaultsForRole( int role ) const;

    QMap<int, QMap<int, QMap<int, QVariant> > > mDataMap;
    QMap<int, QMap<int, QVariant> > mHorizontalHeaderDataMap;
    QMap<int, QMap<int, QVariant> > mVerticalHeaderDataMap;
    QMap<int, QVariant> mModelDataMap;
    PaletteType mPaletteType;
};

}

#endif
