/* A TEXT ZONE IS SOME WORDS WITH A SPECIAL STYLE (ITALIC, ...).
 * IT'S NOT USE FOR TITLE (BUT IN THE FUTURE IT WILL BE USED FOR)
 * OR FOR SPECIAL PARAG.
 */
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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __KWORD_LATEX_EXPORT_PIXMAPFRAME_H__
#define __KWORD_LATEX_EXPORT_PIXMAPFRAME_H__

#include "element.h"
//Added by qt3to4:
#include <QTextStream>

/***********************************************************************/
/* Class: PixmapFrame                                                       */
/***********************************************************************/

/**
 * This class hold a pixmap (frame). The pixmap can be converted in (e)ps. by using
 * the convert command of ImageMagick.
 */
class PixmapFrame : public Element
{
	/* DATA MARKUP */
	int _left;
	int _top;
	int _right;
	int _bottom;
	TAround  _runaround;
	double   _runaroundGap;
	TCreate  _autoCreate;
	TNFrame  _newFrameBehaviour;
	TSide   _sheetSide;
	bool    _keepAspectRatio;

	/* TEXT MARKUP */
	QString _key;
	QString _filenamePS;

	public:
		PixmapFrame();
		PixmapFrame(QString, QString);
		virtual ~PixmapFrame();

		/**
		 * Accessors
		 */
		TAround getRunAround  () const { return _runaround;         }
		double  getAroundGap  () const { return _runaroundGap;      }
		TCreate getAutoCreate () const { return _autoCreate;        }
		TNFrame getNewFrame   () const { return _newFrameBehaviour; }
		TSide   getSheetSide  () const { return _sheetSide;         }

		void getPixmap(QDomNode);
		QString getKey       () const { return _key;        }
		QString getFilenamePS() const { return _filenamePS; }

		/**
		 * Modifiers
		 */
		void setRunAround (const int a)    { _runaround = (TAround) a;  }
		void setAroundGap (const double r) { _runaroundGap = r;         }
		void setAutoCreate(const int a)    { _autoCreate = (TCreate) a; }
		void setNewFrame  (const int n)    { _newFrameBehaviour = (TNFrame) n; }
		void setSheetSide (const int s)    { _sheetSide = (TSide) s;    }
		void setKeepAspectRatio(const QString);

		void setKey        (QString k) { _key        = k; }
		void setFilenamePS (QString f) { _filenamePS = f; }

		/**
		 * Helpful functions
		 */

		void analyze(const QDomNode);
		void convert();
		void generate(QTextStream&);
		/*void generate_format_begin(QTextStream &);
		void generate_format_end(QTextStream &);*/

	private:
		void analyzeParamFrame(const QDomNode);
};

#endif /* __KWORD_LATEX_EXPORT_PIXMAPFRAME_H__ */
