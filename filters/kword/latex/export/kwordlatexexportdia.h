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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KWORDLATEXEXPORTDIA_H__
#define __KWORDLATEXEXPORTDIA_H__

#include <latexexportdia.h>
#include <qstringlist.h>
#include <kurl.h>

#include <dcopobject.h>

class KoStore;
class KConfig;

class KWordLatexExportDia : public LatexExportDia, public DCOPObject
{
    Q_OBJECT

	private:
		//K_DCOP

    QString _fileOut;
    KoStore* _in;
    KConfig* _config;

	public:
    KWordLatexExportDia( KoStore*, QWidget* parent = 0,
				const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    virtual ~KWordLatexExportDia();

    void setOutputFile(QString file) { _fileOut = file; }
	
	k_dcop:
		void useDefaultConfig() { accept(); }

	public slots:
    virtual void reject();
    virtual void accept();
    virtual void addLanguage();
    virtual void removeLanguage();
};

#endif /* __KWORDLATEXEXPORTDIA_H__ */
