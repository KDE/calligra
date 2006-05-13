/* This file is part of the KDE project
   Copyright (C) 2003 Robert JACOLIN <rjacolin@ifrance.com>

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

#ifndef __KWORDLATEXEXPORTDIA_H__
#define __KWORDLATEXEXPORTDIA_H__

#include <latexexportdia.h>
#include <QStringList>
#include <kurl.h>

class KoStore;
class KConfig;
class LatexExportIface;

class KWordLatexExportDia : public LatexExportDia
{
    Q_OBJECT

	private:
    QString _fileOut;
    KoStore* _in;
    KConfig* _config;
		LatexExportIface* _iface;

	public:
    KWordLatexExportDia( KoStore*, QWidget* parent = 0,
				const char* name = 0, bool modal = true, Qt::WFlags fl = 0 );
    virtual ~KWordLatexExportDia();

    void setOutputFile(QString file) { _fileOut = file; }
	
	public slots:
    virtual void reject();
    virtual void accept();
    virtual void addLanguage();
    virtual void removeLanguage();
};

#endif /* __KWORDLATEXEXPORTDIA_H__ */
