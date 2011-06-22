/* This file is part of the KDE project
 * Copyright (C) 2011 Steven Kakoczky <steven.kakoczky@gmail.com>
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

#include "KoAnnotationBalloon.h"

KoAnnotationBalloon::KoAnnotationBalloon(KoAnnotation content, int position = 0, QWidget *parent = 0):
        QWidget(parent), m_content(content), m_y(position)
{
    QGridLayout layout(this);
    setLayout(layout);
    m_textContent = new QTextEdit(m_content->content(), this);
    m_author = new QLabel(m_content->author, this);
    m_date = new QLabel((m_content->date()).toString("MM/dd/yyyy hh:mm"));

    layout.addWidget(m_textContent, 0, 0, 1, 2);
    layout.addItem(m_author, 1, 0);
    layout.addItem(m_date, 2, 0);

    m_optionButton = new QPushButton(this);
    m_options = new QMenu(this);
    // TODO: add menu items
    m_optionButton->setMenu(m_options);

    layout.addWidget(m_optionButton, 1, 1, 2, 1, Qt::AlignCenter);
}

void KoAnnotationBalloon::setFocus()
{
    m_textContent->setFocus();
}
