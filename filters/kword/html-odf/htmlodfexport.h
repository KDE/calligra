/* This file is part of the KOffice project
   Copyright (C) 2010 Pramod S G <pramod.xyle@gmail.com>
   Copyright (C) 2010 Srihari Prasad G V <sri-hari@live.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef HTMLODFEXPORT_H
#define HTMLODFEXPORT_H

#include <qdom.h>
#include <KoFilter.h>
#include <KoXmlWriter.h>
//Added by qt3to4:
#include <QByteArray>


namespace KWord
{

}

class ExportDialog;
class HTMLOdfExport : public KoFilter
{
    Q_OBJECT
public:
    HTMLOdfExport(QObject* parent, const QStringList&);
    virtual ~HTMLOdfExport();
    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);


private:
    ExportDialog *m_dialog;

};

#endif // HTMLODFEXPORT_H
