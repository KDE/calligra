/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009-2010 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TRACKEDCHANGEMANAGER_H
#define TRACKEDCHANGEMANAGER_H

#include <ui_TrackedChangeManager.h>

// #include <KoDialog.h>
#include <QWidget>

class TrackedChangeModel;

class QModelIndex;

class TrackedChangeManager : public QWidget
{
    Q_OBJECT
public:
    explicit TrackedChangeManager(QWidget *parent = nullptr);
    ~TrackedChangeManager() override;

    void setModel(TrackedChangeModel *model);

    void selectItem(const QModelIndex &newIndex);

Q_SIGNALS:
    void currentChanged(const QModelIndex &newIndex);

private Q_SLOTS:
    void slotCurrentChanged(const QModelIndex &newIndex, const QModelIndex &previousIndex);

private:
    Ui::trackedChange widget;
    TrackedChangeModel *m_model;
};

#endif // TRACKEDCHANGEMANAGER_H
