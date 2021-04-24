/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2013 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <ui_StyleManager.h>

#include <QWidget>

class StylesManagerModel;
class StylesSortFilterProxyModel;
class KoStyleManager;
class KoStyleThumbnailer;
class KoParagraphStyle;
class KoCharacterStyle;

class QModelIndex;

class StyleManager : public QWidget
{
    Q_OBJECT
public:
    explicit StyleManager(QWidget *parent = 0);
    ~StyleManager() override;

    void setStyleManager(KoStyleManager *sm);

    void setUnit(const KoUnit &unit);

    //Check that the new name of style is unique or not
    bool checkUniqueStyleName();

public Q_SLOTS:
    void save();
    void setParagraphStyle(KoParagraphStyle *style);
    void setCharacterStyle(KoCharacterStyle *style, bool canDelete = false);
    bool unappliedStyleChanges();

private Q_SLOTS:
    void slotParagraphStyleSelected(const QModelIndex &index);
    void slotCharacterStyleSelected(const QModelIndex &index);
    void addParagraphStyle(KoParagraphStyle*);
    void addCharacterStyle(KoCharacterStyle*);
    void removeParagraphStyle(KoParagraphStyle*);
    void removeCharacterStyle(KoCharacterStyle*);
    void currentParagraphStyleChanged();
    void currentParagraphNameChanged(const QString &name);
    void currentCharacterStyleChanged();
    void currentCharacterNameChanged(const QString &name);
    void buttonNewPressed();
    void tabChanged(int index);

private:
    bool checkUniqueStyleName(int widgetIndex);

    Ui::StyleManager widget;
    KoStyleManager *m_styleManager;

    QMap<KoParagraphStyle *, KoParagraphStyle *> m_modifiedParagraphStyles;
    QMap<KoCharacterStyle *, KoCharacterStyle *> m_modifiedCharacterStyles;

    StylesManagerModel *m_paragraphStylesModel;
    StylesManagerModel *m_characterStylesModel;
    StylesSortFilterProxyModel *m_paragraphProxyModel;
    StylesSortFilterProxyModel *m_characterProxyModel;
    KoStyleThumbnailer *m_thumbnailer;
    bool m_unappliedStyleChanges;
};

#endif
