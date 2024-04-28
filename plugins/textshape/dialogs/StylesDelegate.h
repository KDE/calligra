/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011-2012 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef STYLESDELEGATE_H
#define STYLESDELEGATE_H

#include <QStyledItemDelegate>

/** This is an internal class, used for the preview of styles in the dropdown of the @class StylesCombo.
 * This class is also responsible for drawing and handling the buttons to call the style manager or to delete a style.
 * NB. Deleting a style is currently not supported, therefore the button has been disabled. */

class StylesDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    StylesDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    void setEditButtonEnable(bool enable);

Q_SIGNALS:
    void styleManagerButtonClicked(const QModelIndex &index);
    void deleteStyleButtonClicked(const QModelIndex &index);
    void needsUpdate(const QModelIndex &index);
    void clickedInItem(const QModelIndex &index);

private:
    bool m_editButtonPressed;
    bool m_deleteButtonPressed;
    bool m_enableEditButton;

    int m_buttonSize;
    int m_buttonDistance;
};

#endif
