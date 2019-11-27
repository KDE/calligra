/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef TEMPLATEVARIANTSMODEL_H
#define TEMPLATEVARIANTSMODEL_H

#include <QModelIndex>

class TemplateVariantsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum TemplateVariantRoles
    {
        NameRole = Qt::UserRole + 1,
        ColorRole,
        ThumbnailRole,
        SwatchRole,
        UrlRole
    };
    explicit TemplateVariantsModel(QObject* parent = 0);
    ~TemplateVariantsModel() override;

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    void addVariant(QString name, QString color, QString swatch, QString picture, QString file);
    QModelIndex firstIndex();

private:
    class Private;
    Private* d;
};

Q_DECLARE_METATYPE(TemplateVariantsModel*);

#endif // TEMPLATEVARIANTSMODEL_H
