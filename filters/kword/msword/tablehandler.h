/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H

#include <handlers.h>
#include <deque>

class KWordTableHandler : public wvWare::TableHandler
{
public:
    KWordTableHandler();

    //////// TableHandler interface
    virtual void tableRowFound( const wvWare::TableRowFunctor& tableRow );

    virtual void tableRowStart( wvWare::SharedPtr<const wvWare::Word97::TAP> tap );
    virtual void tableRowEnd();
    virtual void tableCellStart();
    virtual void tableCellEnd();

    ///////// Our own interface
    void writeOutTables(); // feed me with parameters :-)

private:
    std::deque<wvWare::TableRowFunctor> tableRows;
};

#endif // TABLEHANDLER_H
