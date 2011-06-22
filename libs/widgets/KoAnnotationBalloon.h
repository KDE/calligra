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

#ifndef KOANNOTATIONBALLOON_H
#define KOANNOTATIONBALLOON_H

#include <KoBalloon.h>
#include <KoAnnotation.h>

class KoAnnotationBalloon : public KoBalloon
{
public:
    KoAnnotationBalloon(KoAnnotation content, int position = 0, QWidget *parent = 0);

public slots:
    // override setFocus, gives focus to m_textContent
    void setFocus();

private:
    KoAnnotation *m_content;
    QTextEdit *m_textContent;
    QLabel *m_author;
    QLabel *m_date;

    QPushButton *optionButton;
    QMenu *options;
};

#endif // KOANNOTATIONBALLOON_H
