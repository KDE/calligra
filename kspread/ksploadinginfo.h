/* This file is part of the KDE project
   Copyright (C) 2004 Laurent Montel <montel@kde.org>

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

#ifndef KSPLOADINGINFO_H
#define KSPLOADINGINFO_H

class KSpreadSheet;

/// Temporary information used only during loading
class KSPLoadingInfo
{
public:
    KSPLoadingInfo() {}
    ~KSPLoadingInfo() {}
    void addWordInAreaList( const QString & word) { m_areaNamed.append( word ); }
    bool findWordInAreaList(const QString & word) const { return (m_areaNamed.find( word ) != m_areaNamed.end());}
    void appendValidation( const QString &name, const QDomElement &element){ m_validationList.insert( name, element);}
    QDomElement validation( const QString &name) { return m_validationList[name];}

    void addMarkerSelection( KSpreadSheet *sheet, const QPoint & _point ) { m_markerSelection.insert( sheet, _point );}

private:
    QStringList m_areaNamed;
    QMap<QString,QDomElement> m_validationList;
    QMap<KSpreadSheet*, QPoint> m_markerSelection;
};

#endif /* KPRLOADINGINFO_H */

