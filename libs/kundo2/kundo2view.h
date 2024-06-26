/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Matus Talcik <matus.talcik@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
/****************************************************************************
**
** SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef KUNDO2VIEW_H
#define KUNDO2VIEW_H

#include <QListView>
#include <QString>

#include "kundo2_export.h"

#ifndef QT_NO_UNDOVIEW

class KUndo2ViewPrivate;
class KUndo2QStack;
class KUndo2Group;
class QIcon;

class KUNDO2_EXPORT KUndo2View : public QListView
{
    Q_OBJECT
    Q_PROPERTY(QString emptyLabel READ emptyLabel WRITE setEmptyLabel)
    Q_PROPERTY(QIcon cleanIcon READ cleanIcon WRITE setCleanIcon)

public:
    explicit KUndo2View(QWidget *parent = nullptr);
    explicit KUndo2View(KUndo2QStack *stack, QWidget *parent = nullptr);
#ifndef QT_NO_UNDOGROUP
    explicit KUndo2View(KUndo2Group *group, QWidget *parent = nullptr);
#endif
    ~KUndo2View() override;

    KUndo2QStack *stack() const;
#ifndef QT_NO_UNDOGROUP
    KUndo2Group *group() const;
#endif

    void setEmptyLabel(const QString &label);
    QString emptyLabel() const;

    void setCleanIcon(const QIcon &icon);
    QIcon cleanIcon() const;

public Q_SLOTS:
    void setStack(KUndo2QStack *stack);
#ifndef QT_NO_UNDOGROUP
    void setGroup(KUndo2Group *group);
#endif

private:
    KUndo2ViewPrivate *const d;
    Q_DISABLE_COPY(KUndo2View)
};

#endif // QT_NO_UNDOVIEW
#endif // KUNDO2VIEW_H
