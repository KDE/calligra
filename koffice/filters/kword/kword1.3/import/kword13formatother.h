/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD13_FORMAT_OTHER
#define KWORD13_FORMAT_OTHER

#include "kword13format.h"

/**
 * \brief format for variable
 *
 * For \<FORMAT id="4"\>
 */
class KWord13FormatFour : public KWord13Format
{
public:
    KWord13FormatFour(void);
    virtual ~KWord13FormatFour(void);
public:
    QString m_text; ///< replacement text
};

/**
 * \brief format for anchor
 *
 * For \<FORMAT id="6"\>
 */
class KWord13FormatSix : public KWord13Format
{
public:
    KWord13FormatSix(void);
    virtual ~KWord13FormatSix(void);
public:
    QString m_anchorName; ///< name of anchor
};


#endif // KWORD13_FORMAT_OTHER
