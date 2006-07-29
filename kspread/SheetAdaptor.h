/* This file is part of the KDE project

   Copyright 2006 Fredrik Edemar <f_edemar@linux.se>
   Copyright 2000, 2003 Laurent Montel <montel@kde.org>
   Copyright 2002-2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999 Torben Weis <weis@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_TABLE_IFACE_H
#define KSPREAD_TABLE_IFACE_H

#include <QtDBus/QtDBus>

#include <QRect>
#include <QString>

namespace KSpread
{
class Sheet;
class CellProxy;

class SheetAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.spreadsheet.sheet")
public:
    SheetAdaptor( Sheet* );
    ~SheetAdaptor();

//     bool processDynamic( const DCOPCString& fun, const QByteArray& data,
// 			 DCOPCString& replyType, QByteArray &replyData );

//     void sheetNameHasChanged();
public Q_SLOTS: // METHODS
    virtual QString cell( int x, int y );
    virtual QString cell( const QString& name );

    virtual QString column( int _col );
    virtual QString row( int _row );

    virtual QString name() const;
    virtual int maxColumn() const;
    virtual int maxRow() const;
    virtual bool setSheetName( const QString & name);


    virtual bool insertColumn( int col,int nbCol );
    virtual bool insertRow( int row,int nbRow);
    virtual void removeColumn( int col,int nbCol );
    virtual void removeRow( int row,int nbRow );

    virtual bool isHidden()const;
    virtual bool areaHasNoContent(QRect area) const ;
    virtual bool areaHasNoComments(QRect area) const ;

    virtual bool showGrid() const;
    virtual bool showFormula() const;
    virtual bool lcMode() const;
    virtual bool autoCalc() const;
    virtual bool showColumnNumber() const;
    virtual bool hideZero() const;
    virtual bool firstLetterUpper() const;
    virtual void setShowPageBorders( bool b );

    virtual float paperHeight() const;
    virtual float paperWidth() const ;
    virtual float leftBorder() const;
    virtual float rightBorder() const;
    virtual float topBorder() const;
    virtual float bottomBorder() const;
    QString paperFormatString() const;

    QString headLeft()const;
    QString headMid()const;
    QString headRight()const;
    QString footLeft()const;
    QString footMid()const;
    QString footRight()const;

    void setHeaderLeft(const QString & text);
    void setHeaderMiddle(const QString & text);
    void setHeaderRight(const QString & text);
    void setFooterLeft(const QString & text);
    void setFooterMiddle(const QString & text);
    void setFooterRight(const QString & text);
    bool isProtected() const;
private:
    Sheet* m_sheet;
//     CellProxy* m_proxy;
    QByteArray ident;
};

} // namespace KSpread

#endif
