/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TABLECREATEBUTTON_H
#define TABLECREATEBUTTON_H

#include <QToolButton>

class QMenu;

class QuickTableButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QuickTableButton(QWidget *parent = nullptr);
    void emitCreate(int rows, int columns);
    void addAction(QAction *action);

Q_SIGNALS:
    void create(int rows, int columns);

private:
    QMenu *m_menu;
};

#endif
