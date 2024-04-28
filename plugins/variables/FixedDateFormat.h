/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef FIXEDDATEFORMAT_H
#define FIXEDDATEFORMAT_H

#include <QWidget>

#include <ui_FixedDateFormat.h>

class DateVariable;
class QListWidgetItem;
class QMenu;

class FixedDateFormat : public QWidget
{
    Q_OBJECT
public:
    explicit FixedDateFormat(DateVariable *variable);

private Q_SLOTS:
    void customClicked(int state);
    void listClicked(QListWidgetItem *item);
    void offsetChanged(int offset);
    void insertCustomButtonPressed();
    void customTextChanged(const QString &text);

private:
    Ui::FixedDateFormat widget;
    DateVariable *m_variable;
    QMenu *m_popup;
};

#endif
