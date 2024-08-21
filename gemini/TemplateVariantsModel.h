/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef TEMPLATEVARIANTSMODEL_H
#define TEMPLATEVARIANTSMODEL_H

#include <QModelIndex>

class TemplateVariantsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum TemplateVariantRoles { NameRole = Qt::UserRole + 1, ColorRole, ThumbnailRole, SwatchRole, UrlRole };
    explicit TemplateVariantsModel(QObject *parent = nullptr);
    ~TemplateVariantsModel() override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void addVariant(QString name, QString color, QString swatch, QString picture, QString file);
    QModelIndex firstIndex();

private:
    class Private;
    Private *d;
};

Q_DECLARE_METATYPE(TemplateVariantsModel *);

#endif // TEMPLATEVARIANTSMODEL_H
