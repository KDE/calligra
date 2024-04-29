/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LISTLEVELCHOOSER_H
#define LISTLEVELCHOOSER_H

#include <QPushButton>
#include <QWidget>

class ListLevelChooser : public QPushButton
{
    Q_OBJECT
public:
    explicit ListLevelChooser(const int offset, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_offset;
};

#endif // LISTLEVELCHOOSER_H
