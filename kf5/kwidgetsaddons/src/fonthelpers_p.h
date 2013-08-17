/*
    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

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
#ifndef FONTHELPERS_P_H
#define FONTHELPERS_P_H

// i18n-related helpers for fonts, common to KFont* widgets.

#include <QString>
#include <QStringList>
#include <QHash>

/**
  * @internal
  *
  * Split the compound raw font name into family and foundry.
  *
  * @param name the raw font name reported by Qt
  * @param family the storage for family name
  * @param foundry the storage for foundry name
  */
void splitFontString (const QString &name,
                      QString *family, QString *foundry = NULL);

/**
  * @internal
  *
  * Translate the font name for the user.
  * Primarily for generic fonts like Serif, Sans-Serif, etc.
  *
  * @param name the raw font name reported by Qt
  * @return translated font name
  */
QString translateFontName (const QString &name);

/**
  * @internal
  *
  * Compose locale-aware sorted list of translated font names,
  * with generic fonts handled in a special way.
  * The mapping of translated to raw names can be reported too if required.
  *
  * @param names raw font names as reported by Qt
  * @param trToRawNames storage for mapping of translated to raw names
  * @return sorted list of translated font names
  */
QStringList translateFontNameList (const QStringList &names,
                                   QHash<QString, QString> *trToRawNames = NULL);

# endif
