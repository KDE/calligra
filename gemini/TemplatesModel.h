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

#ifndef TEMPLATESMODEL_H
#define TEMPLATESMODEL_H

#include <QAbstractListModel>

class TemplatesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString templateType READ templateType WRITE setTemplateType NOTIFY templateTypeChanged)
    Q_PROPERTY(bool showWide READ showWide WRITE setShowWide NOTIFY showWideChanged)
public:
    enum TemplateRoles {
        TitleRole = Qt::UserRole + 1,
        DescriptionRole,
        ColorRole,
        ThumbnailRole,
        UrlRole,
        VariantCountRole,
        VariantsRole
    };
    explicit TemplatesModel(QObject* parent = 0);
    ~TemplatesModel() override;

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;

    QString templateType() const;
    void setTemplateType(const QString& newType);

    bool showWide() const;
    void setShowWide(const bool& newValue);

Q_SIGNALS:
    void templateTypeChanged();
    void showWideChanged();

private:
    class Private;
    Private* d;
};

#endif // TEMPLATESMODEL_H
