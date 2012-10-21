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
#ifndef STYLESMANAGER_H
#define STYLESMANAGER_H

#include <QDialog>
#include <QMap>

class StylesModel;
class StylesManagerStylesModel;
class StylesManagerStylesListModel;
class StylesManagerStylesListItemDelegate;

class KoCharacterStyle;
class KoParagraphStyle;
class KoStyleManager;
class KoStyleThumbnailer;

class QModelIndex;
class QSortFilterProxyModel;

namespace Ui {
class StylesManager;
}

class StylesManager : public QDialog
{
    Q_OBJECT

public:
    explicit StylesManager(KoStyleManager *sm, QWidget *parent = 0);
    ~StylesManager();

    void setStyleManager(KoStyleManager *sm);

public slots:
    void setCurrentCharacterStyle(KoCharacterStyle *style);
    void setCurrentParagraphStyle(KoParagraphStyle *style);

private slots:
    void slotCharacterStyleSelected(const QModelIndex&);
    void slotParagraphStyleSelected(const QModelIndex&);

    void slotCharacterStyleChanged();
    void slotParagraphStyleChanged();

    void slotSaveCharacterStyle();
    void slotSaveParagraphStyle();
private:
    Ui::StylesManager *ui;

    KoStyleManager *m_styleManager;
    KoStyleThumbnailer *m_styleThumbnailer;
    StylesModel *m_characterStyleBaseModel;
    StylesModel *m_paragraphStyleBaseModel;
//    StylesModelV1 *m_paragraphStyleModel;

    StylesManagerStylesModel *m_characterStylesModel;
    StylesManagerStylesModel *m_paragraphStylesModel;
    StylesManagerStylesListModel *m_characterStylesListModel;
    StylesManagerStylesListModel *m_paragraphStylesListModel;

    StylesManagerStylesListItemDelegate *m_characterStylesListItemDelegate;
    StylesManagerStylesListItemDelegate *m_paragraphStylesListItemDelegate;

    KoCharacterStyle *m_currentCharacterStyle;
    KoParagraphStyle *m_currentParagraphStyle;

//    QMap<int, KoCharacterStyle*> m_modifiedCharacterStyles;
//    QMap<int, KoParagraphStyle*> m_modifiedParagraphStyles;
};

#endif // STYLESMANAGER_H
