/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
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

#ifndef STYLESMANAGERSTYLESMODEL_H
#define STYLESMANAGERSTYLESMODEL_H

#include "StylesFilteredModelBase.h"

class AbstractStylesModel;
class KoCharacterStyle;
class KoParagraphStyle;
class KoStyleManager;
class KoStyleThumbnailer;

class StylesManagerStylesModel : public StylesFilteredModelBase
{
    Q_OBJECT
public:
    explicit StylesManagerStylesModel(QObject *parent = 0);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

    virtual int rowCount(const QModelIndex &parent) const;

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void setStyleManager(KoStyleManager *sm);

    KoCharacterStyle* currentSelectedStyle();

signals:

public slots:
    void slotModifiedStyle();

    void slotSetCurrentIndex(QModelIndex index);

protected:
    void createMapping();

private:
    KoStyleManager *m_styleManager;
//    KoStyleThumbnailer *m_thumbnailer;

    QMap<int, KoCharacterStyle*> m_modifiedStyles;
    KoCharacterStyle *m_currentSelectedStyle;
};

#endif // STYLESMANAGERSTYLESMODEL_H
