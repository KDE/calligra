/*
 *  SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

// Fake header to offer Krita API, without the fixup logic

namespace KisDomUtils
{

template<typename T>
inline QString toString(T value)
{
    return QString::number(value);
}

inline double toDouble(const QString &string)
{
    return string.toDouble();
}

}
