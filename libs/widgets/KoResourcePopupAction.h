/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KORESOURCEPOPUPACTION_H
#define KORESOURCEPOPUPACTION_H

#include "kowidgets_export.h"

#include <QAction>

#include <QSharedPointer>

class KoShapeBackground;
class KoAbstractResourceServerAdapter;
class QModelIndex;

class KOWIDGETS_EXPORT KoResourcePopupAction : public QAction
{
    Q_OBJECT

public:
    /**
     * Constructs a KoResourcePopupAction (gradient or pattern) with the specified parent.
     *
     * @param gradientResourceAdapter A pointer to gradient resource adapter.
     * @param parent The parent for this action.
     */
    explicit KoResourcePopupAction(QSharedPointer<KoAbstractResourceServerAdapter> gradientResourceAdapter, QObject *parent = nullptr);

    /**
     * Destructor
     */
    ~KoResourcePopupAction() override;

    QSharedPointer<KoShapeBackground> currentBackground() const;
    void setCurrentBackground(QSharedPointer<KoShapeBackground> background);

Q_SIGNALS:
    /// Emitted when a resource was selected
    void resourceSelected(QSharedPointer<KoShapeBackground> background);

public Q_SLOTS:
    void updateIcon();

private Q_SLOTS:
    void indexChanged(const QModelIndex &modelIndex);

private:
    class Private;
    Private *const d;
};

#endif /* KORESOURCEPOPUPACTION_H */
