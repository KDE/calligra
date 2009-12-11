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

//Added by qt3to4:
#include <QTextStream>

#ifndef _KWORD13LAYOUT
#define _KWORD13LAYOUT

class QTextStream;

#include <QString>
#include <QMap>

#include "kword13formatone.h"

/**
 * Layout or style
 */
class KWord13Layout
{
public:
    explicit KWord13Layout(void);
    ~KWord13Layout(void);

public:
    void xmldump(QTextStream& iostream);

    /**
     * @brief Get a key representating the properties
     *
     * This key helps to categorize the automatic styles
     */
    QString key(void) const;


    QString getProperty(const QString& name) const;

public:
    KWord13FormatOneData m_format; ///< Character format properties
    QMap<QString, QString> m_layoutProperties;
    bool m_outline;
    QString m_name; ///< Name of the style (either the used one or the one currently being defined)
public: // OASIS-specific
    QString m_autoStyleName; ///< Name of the OASIS automatic style
};

#endif // _KWORD13LAYOUT
