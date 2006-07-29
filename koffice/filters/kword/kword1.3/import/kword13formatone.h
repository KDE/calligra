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

#ifndef KWORD_1_3_FORMAT_ONE
#define KWORD_1_3_FORMAT_ONE

class QTextStream;

#include <qstring.h>
#include <qmap.h>

#include "kword13format.h"

/**
 * Contains the data of the \<FORMAT id="1"\> children
 * also the grand-children of \<STYLE\> and \<LAYOUT\>
 */
class KWord13FormatOneData
{
public:
    KWord13FormatOneData( void );
    ~KWord13FormatOneData( void );
    
public:
    void xmldump( QTextStream& iostream );
    
    /**
     * @brief Get a key representating the properties
     *
     * This key helps to categorize the automatic styles
     */
    QString key( void ) const;

    QString getProperty( const QString& name ) const;

public:
    QMap<QString,QString> m_properties;
public: // OASIS-specific
    QString m_autoStyleName; ///< Name of the OASIS automatic style
};

/**
 * \brief class for \<FORMAT id="1"\>
 */
class KWord13FormatOne : public KWord13Format
{
public:
    KWord13FormatOne(void);
    virtual ~KWord13FormatOne(void);
public:
    /// Get lenght of format
    virtual int length( void );
    /**
     * Get a pointer to the data correponding to \<FORMAT id="1"\>
     *
     * \note The returned pointer MUST NOT be deleted!
     */
    virtual KWord13FormatOneData* getFormatOneData(void);
public:
    int m_length; ///< Length of format
    KWord13FormatOneData m_formatOne; ///< Data of children of \<FORMAT id="1"\>
};

#endif // KWORD_1_3_FORMAT_ONE
