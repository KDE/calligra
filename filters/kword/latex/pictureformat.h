
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KWORD_PICTUREFORMAT_H__
#define __KWORD_PICTUREFORMAT_H__

#include "format.h"


class PictureFormat: public Format
{
	int      _pos;
	int      _length;	/* Length of the string !! */
	QString  _filename,
		 _filenameEPS,
		 _path,
		 _pathEPS;

	public:
		PictureFormat(): _pos(0)
		{
		}
		virtual ~PictureFormat() {}

		int     getPos        () const { return _pos;         }
		int     getLength     () const { return _length;      }
		QString getFilename   () const { return _filename;    }
		QString getFilenameEPS() const { return _filenameEPS; }
		QString getPath       () const { return _path;        }
		QString getPathEPS    () const { return _pathEPS;     }

		void setPos        (const int p)   { _pos         = p; }
		void setLength     (const int t)   { _length      = t; }
		void setFilename   (QString f)     { _filename    = f; }
		void setFilenameEPS(QString f)     { _filenameEPS = f; }
		void setPathEPS    (QString p)     { _pathEPS     = p; }

		void analysePictureFormat(const QDomNode);
		void analyseParam        (const QDomNode);
		void analyseImage        (const QDomNode);

};

#endif /* __KWORD_PICTUREFORMAT_H__ */
