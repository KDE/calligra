/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STYLESMODELNEW_H
#define STYLESMODELNEW_H

#include <QAbstractListModel>
#include <QList>

class KoCharacterStyle;
class KoStyleThumbnailer;

class StylesManagerModel : public QAbstractListModel
{
public:
    enum Roles {
        StylePointer = Qt::UserRole + 1,
    };

    explicit StylesManagerModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void setStyleThumbnailer(KoStyleThumbnailer *thumbnailer);
    void setStyles(const QList<KoCharacterStyle *> &styles);
    void addStyle(KoCharacterStyle *style);
    void removeStyle(KoCharacterStyle *style);
    void replaceStyle(KoCharacterStyle *oldStyle, KoCharacterStyle *newStyle);
    void updateStyle(KoCharacterStyle *style);

    QModelIndex styleIndex(KoCharacterStyle *style);

private:
    QList<KoCharacterStyle *> m_styles;
    KoStyleThumbnailer *m_styleThumbnailer;
};

#endif /* STYLESMODELNEW_H */
