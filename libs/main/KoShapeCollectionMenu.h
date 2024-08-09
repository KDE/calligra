// SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
// SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QMenu>

class QToolButton;
class CollectionItemModel;

class KoShapeCollectionMenu : public QMenu
{
    Q_OBJECT

public:
    explicit KoShapeCollectionMenu(QWidget *parent = nullptr);
    virtual ~KoShapeCollectionMenu();

    QSize sizeHint() const override;

private:
    void loadDefaultShapes();
    void activateShapeCollection(const QString &id);
    void activateShapeCreationTool(const QModelIndex &index);
    QToolButton *addCollection(const QString &id, const QString &title, CollectionItemModel *model);

    class Private;
    std::unique_ptr<Private> d;
};
