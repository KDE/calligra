/*  This file is part of the KDE libraries
    Copyright (C) 2008 Chusslove Illich <caslav.ilic@gmx.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef COMMON_HELPERS_P_H
#define COMMON_HELPERS_P_H

#include <QString>

// Standalone (pure Qt) functionality needed internally in more than
// one source file on localization.

/**
  * @internal
  *
  * Removes accelerator marker from a UI text label.
  *
  * Accelerator marker is not always a plain ampersand (&),
  * so it is not enough to just remove it by @c QString::remove().
  * The label may contain escaped markers ("&&") which must be resolved
  * and skipped, as well as CJK-style markers ("Foo (&F)") where
  * the whole parenthesis construct should be removed.
  * Therefore always use this function to remove accelerator marker
  * from UI labels.
  *
  * @param label UI label which may contain an accelerator marker
  * @return label without the accelerator marker
  */
QString removeAcceleratorMarker (const QString &label);

#endif
