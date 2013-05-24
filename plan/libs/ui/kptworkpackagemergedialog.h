/* This file is part of the KDE project
   Copyright (C) 2011 Dag Andersen <danders@get2net.dk>

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

#ifndef WORKPACKAGEMERGEDIALOG_H
#define WORKPACKAGEMERGEDIALOG_H

#include "kplatoui_export.h"

#include "ui_kptworkpackagemergepanel.h"

#include <kdialog.h>
#include <kdatetime.h>

#include <QMap>
#include <QFrame>

class KExtendableItemDelegate;

class QStandardItemModel;

namespace KPlato
{

class Package;

class PackageInfoWidget : public QFrame
{
    Q_OBJECT
public:
    explicit PackageInfoWidget( Package *package, QWidget *parent = 0 );

protected slots:
    void slotUsedEffortChanged( int state );
    void slotProgressChanged( int state );
    void slotDocumentsChanged( int state );

protected:
    Package *m_package;
};

class WorkPackageMergePanel : public QWidget, public Ui::WorkPackageMergePanel
{
    Q_OBJECT
public:
    explicit WorkPackageMergePanel(QWidget *parent = 0);
};

class KPLATOUI_EXPORT WorkPackageMergeDialog : public KDialog
{
    Q_OBJECT
public:
    enum Columns { CheckColumn = 0, TaskNameColumn, OwnerNameColumn, DateTimeColumn };

    WorkPackageMergeDialog( const QString &text, const QMap<KDateTime, Package*> &list, QWidget *parent = 0 );
    ~WorkPackageMergeDialog();

    QList<int> checkedList() const;

protected slots:
    void slotActivated( const QModelIndex &idx );
    void slotChanged();

private:
    WorkPackageMergePanel panel;
    KExtendableItemDelegate *m_delegate;
    QList<Package*> m_packages;
    QStandardItemModel *m_model;
};

} // namespace KPlato

#endif
