// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef IMPORTTAGS_H
#define IMPORTTAGS_H

typedef enum
{
    modeDisplayUnknown=0,
    modeDisplayNone, // (forces child elements to be modeDisplayNone too!)
    modeDisplayBlock,
    modeDisplayInline
} ModeDisplay;

class ParsingTag
{
public:
    ParsingTag (void) { }
    ParsingTag (const ModeDisplay mode, const bool empty, const QString& strStyle)
     : m_modeDisplay(mode), m_empty(empty), m_strStyle(strStyle) { }
    ~ParsingTag(void) { }
public:
    inline bool isEmptyElement(void) const {return m_empty;}
    inline ModeDisplay getModeDisplay(void) const {return m_modeDisplay;}
    inline QString getStyle(void) const {return m_strStyle;}
private:
    // TODO: put the following variables in a better order!
    ModeDisplay m_modeDisplay; // Default CSS display propety for this element
    bool m_empty; // Is this element EMPTY in HTML's sence
    QString m_strStyle; // Default CSS style for this element
};

class MapTag : public QMap<QString,ParsingTag>
{
public:
    MapTag  (void) { InitMapTag(); }
    ~MapTag (void) {}
private:
    bool AddTag(const QString& strName, const ParsingTag* tag);
    bool AddTag(const QString& strName, const bool empty); // Depreciated!
    bool AddNoneTag(const QString& strName);
    bool AddBlockTag(const QString& strName,const QString& strStyle);
    bool AddInlineTag(const QString& strName,const QString& strStyle);
    bool InitMapTag(void);
};



#endif
