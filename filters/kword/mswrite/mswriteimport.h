/* $Id$ */
/* This file is part of the KDE project
   Copyright (C) 2001-2003 Clarence Dang <dang@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License Version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License Version 2 for more details.

   You should have received a copy of the GNU Library General Public License
   Version 2 along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef MSWRITEIMPORT_H
#define MSWRITEIMPORT_H

#include <KoFilter.h>

class WRIDevice;
class MSWrite::InternalParser;
class KWordGenerator;

class MSWriteImport : public KoFilter
{
	Q_OBJECT

private:
	WRIDevice *m_device;
	MSWrite::InternalParser *m_parser;
	KWordGenerator *m_generator;

public:
	MSWriteImport (KoFilter *parent, const char *name, const QStringList &);
	virtual ~MSWriteImport ();

	KoFilter::ConversionStatus convert (const QCString &from, const QCString &to);
	
	void sigProgress (const int value)
	{
		emit KoFilter::sigProgress (value);
	}
};

#endif // MSWRITEIMPORT_H
