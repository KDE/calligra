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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPLOADINGINFO_H
#define KSPLOADINGINFO_H

namespace KSpread
{
class Sheet;

/// Temporary information used only during loading
class KSPLoadingInfo
{
public:
    KSPLoadingInfo() { m_loadTemplate = false;}
    ~KSPLoadingInfo() {}
    void addWordInAreaList( const QString & word) { m_areaNamed.append( word ); }
    bool findWordInAreaList(const QString & word) const { return (m_areaNamed.indexOf( word ) != -1);}
    void appendValidation( const QString &name, const QDomElement &element){ m_validationList.insert( name, element);}
    QDomElement validation( const QString &name) { return m_validationList[name];}

    //Use when we load ods file.
    //Necessary to initialise kspread_view selection
    void addMarkerSelection( Sheet *sheet, const QPoint & _point ) { m_markerSelection.insert( sheet, _point );}

    void setLoadTemplate( bool _b ) {
        m_loadTemplate = _b;
    }
    bool loadTemplate() const {
        return m_loadTemplate;
    }

private:
    QStringList m_areaNamed;
    QMap<QString,QDomElement> m_validationList;
    QMap<Sheet*, QPoint> m_markerSelection;
    bool m_loadTemplate;
};

} // namespace KSpread

#endif /* KPRLOADINGINFO_H */

