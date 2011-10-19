/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qabstractproxymodel.h"

#ifndef QT_NO_PROXYMODEL

#include "qitemselectionmodel.h"
#include <private/qabstractproxymodel_p.h>
#include <QtCore/QSize>
#include <QtCore/QStringList>


//namespace KexiUtils {

/*!
    \since 4.1
    \class QAbstractProxyModel
    \brief The QAbstractProxyModel class provides a base class for proxy item
    models that can do sorting, filtering or other data processing tasks.
    \ingroup model-view

    This class defines the standard interface that proxy models must use to be
    able to interoperate correctly with other model/view components. It is not
    supposed to be instantiated directly.

    All standard proxy models are derived from the QAbstractProxyModel class.
    If you need to create a new proxy model class, it is usually better to
    subclass an existing class that provides the closest behavior to the one
    you want to provide.

    Proxy models that filter or sort items of data from a source model should
    be created by using or subclassing QSortFilterProxyModel.

    To subclass QAbstractProxyModel, you need to implement mapFromSource() and
    mapToSource(). The mapSelectionFromSource() and mapSelectionToSource()
    functions only need to be reimplemented if you need a behavior different
    from the default behavior.

    \note If the source model is deleted or no source model is specified, the
    proxy model operates on a empty placeholder model.

    \sa QSortFilterProxyModel, QAbstractItemModel, {Model/View Programming}
*/

//detects the deletion of the source model
void QAbstractProxyModelPrivate::_q_sourceModelDestroyed()
{
    model = QAbstractItemModelPrivate::staticEmptyModel();
}

//} //namespace KexiUtils

#endif // QT_NO_PROXYMODEL
