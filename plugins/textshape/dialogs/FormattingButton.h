/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FORMATTINGBUTTON_H
#define FORMATTINGBUTTON_H

#include <KoListStyle.h>

#include <QMap>
#include <QPixmap>
#include <QToolButton>

class QMenu;
class QAction;
class ItemChooserAction;

class FormattingButton : public QToolButton
{
    Q_OBJECT
public:
    explicit FormattingButton(QWidget *parent = nullptr);

    ItemChooserAction *addItemChooser(int columns, const QString &title = QString());
    void setItemsBackground(ItemChooserAction *chooser, const QColor &color);
    void addItem(ItemChooserAction *chooser, const QPixmap &pm, int id, const QString &toolTip = QString());
    QAction *addItemMenuItem(ItemChooserAction *chooser, int id, const QString &text);
    void addBlanks(ItemChooserAction *chooser, int n);
    void removeLastItem(ItemChooserAction *chooser);

    void addAction(QAction *action);
    void addSeparator();
    bool hasItemId(int id);
    bool isFirstTimeMenuShown();

Q_SIGNALS:
    void itemTriggered(int id);
    void doneWithFocus();
    void aboutToShowMenu();

private Q_SLOTS:
    void itemSelected();
    void menuShown();
    void recalcMenuSize();

private:
    int m_lastId;
    QMenu *m_menu;
    QMap<int, QObject *> m_styleMap;
    bool m_menuShownFirstTime;
};

#endif // FORMATTINGBUTTON_H
