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

#include <functor.h>
#include <handlers.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qobject.h>
#include <deque>

namespace KWord
{
    typedef const wvWare::TableRowFunctor* TableRowFunctorPtr;

    struct Row
    {
        Row() : functorPtr( 0L )  {} // QValueList wants that one
        Row( TableRowFunctorPtr ptr ) : functorPtr(ptr) {}
        ~Row() {}
        TableRowFunctorPtr functorPtr;
    };

    struct Table
    {
        QString name; // kword's grpMgr attribute
        QValueList<Row> rows; // need to use QValueList to benefit from implicit sharing
    };
};

class KWordTableHandler : public QObject, public wvWare::TableHandler
{
    Q_OBJECT
public:
    KWordTableHandler();

    //////// TableHandler interface
    virtual void tableRowStart( wvWare::SharedPtr<const wvWare::Word97::TAP> tap );
    virtual void tableRowEnd();
    virtual void tableCellStart();
    virtual void tableCellEnd();

    ///////// Our own interface
    void tableStart( const QString& name );
    void tableEnd();

signals:
    // Tells Document to create frameset for cell
    void sigTableCellStart( int row, int column, int rowSize, int columnSize, const QString& tableName );
    void sigTableCellEnd();

private:
    QString m_currentTableName;
    int m_row;
    int m_column;
    wvWare::SharedPtr<const wvWare::Word97::TAP> m_tap;
};

#endif // TABLEHANDLER_H
