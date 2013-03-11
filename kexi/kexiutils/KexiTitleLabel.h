/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXITITLELABEL_H
#define KEXITITLELABEL_H

#include <QLabel>

#include "kexiutils_export.h"

class KEXIUTILS_EXPORT KexiTitleLabel : public QLabel
{
    Q_OBJECT
public:
    explicit KexiTitleLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
    explicit KexiTitleLabel(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~KexiTitleLabel();
protected:
    void changeEvent(QEvent* event);
private:
    void updateFont();
    void init();
    
    class Private;
    Private * const d;
};

#endif
