/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
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
    enum TemplateRoles { TitleRole = Qt::UserRole + 1, DescriptionRole, ColorRole, ThumbnailRole, UrlRole, VariantCountRole, VariantsRole };
    explicit TemplatesModel(QObject *parent = nullptr);
    ~TemplatesModel() override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

    QString templateType() const;
    void setTemplateType(const QString &newType);

    bool showWide() const;
    void setShowWide(const bool &newValue);

Q_SIGNALS:
    void templateTypeChanged();
    void showWideChanged();

private:
    class Private;
    Private *d;
};

#endif // TEMPLATESMODEL_H
