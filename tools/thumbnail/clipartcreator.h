/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef CLIPARTCREATOR_H
#define CLIPARTCREATOR_H

#include <kio/thumbcreator.h>

class ClipartCreator : public ThumbCreator
{
public:
    ClipartCreator() {};
    virtual bool create(const QString &path, int, int, QImage &img);
    virtual Flags flags() const;
};

#endif // CLIPARTCREATOR_H
