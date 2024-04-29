/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef DOCKERSTYLESCOMBOMODEL_H
#define DOCKERSTYLESCOMBOMODEL_H

#include "StylesFilteredModelBase.h"

#include <QVector>

class KoCharacterStyle;
class KoStyleManager;

class DockerStylesComboModel : public StylesFilteredModelBase
{
    Q_OBJECT
public:
    enum CategoriesInternalIds { UsedStyleId = -32000, UnusedStyleId = -32001 };

    explicit DockerStylesComboModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setStyleManager(KoStyleManager *sm);

    void setInitialUsedStyles(QVector<int> usedStyles);

Q_SIGNALS:

public Q_SLOTS:
    void styleApplied(const KoCharacterStyle *style);

protected:
    void createMapping() override;

private:
    KoCharacterStyle *findStyle(int styleId) const;
    KoStyleManager *m_styleManager;
    QVector<int> m_usedStylesId;
    QVector<int> m_usedStyles;
    QVector<int> m_unusedStyles;
};

#endif // DOCKERSTYLESCOMBOMODEL_H
