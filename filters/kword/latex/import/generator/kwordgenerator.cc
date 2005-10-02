/* This file is part of the KDE project
 * Copyright (C) 2002, 2003 Robert JACOLIN <rjacolin@ifrance.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <kdebug.h>
#include <ktempfile.h>
#include <qfile.h>

#include <qdom.h>

#include "kwordgenerator.h"
#include "document.h"

void KwordGenerator::convert(KoStore* out)
{
	//KTempFile temp("lateximport", ".kwd");
	//temp.setAutoDelete(true);
	//QFile* tempFile = temp.file();
	/*if(out->open("root"))
	{*/
		Document document;
		document.analyse(_root);
		//QTextStream output(tempFile);
		document.generate(out);
		//tempFile->close();
		//out->write(tempFile->readAll());
		/*out->close();
	}*/
	
	//temp.close();
}
