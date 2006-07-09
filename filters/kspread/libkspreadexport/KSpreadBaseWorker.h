/*
This file is part of the KDE project
Copyright (C) 2002 Fred Malabre <fmalabre@yahoo.com>

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

#ifndef KSPREAD_BASE_WORKER_H
#define KSPREAD_BASE_WORKER_H

#include <KoFilter.h>
#include <qmap.h>


typedef QMap<QString, QString> KSpreadFilterProperty;

class KSpreadBaseWorker
{
public:
	KSpreadBaseWorker();
	virtual ~KSpreadBaseWorker();

	virtual KoFilter::ConversionStatus startDocument(KSpreadFilterProperty property);
	virtual KoFilter::ConversionStatus startInfoLog(KSpreadFilterProperty property);
	virtual KoFilter::ConversionStatus startInfoAuthor(KSpreadFilterProperty property);
	virtual KoFilter::ConversionStatus startInfoAbout(KSpreadFilterProperty property);
	virtual KoFilter::ConversionStatus startSpreadBook(KSpreadFilterProperty property);
	virtual KoFilter::ConversionStatus startSpreadSheet(KSpreadFilterProperty property);
	virtual KoFilter::ConversionStatus startSpreadCell(KSpreadFilterProperty property);
};

#endif /* KSPREAD_BASE_WORKER_H */
