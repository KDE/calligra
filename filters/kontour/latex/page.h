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

#ifndef __KILLU_PAGE_H__
#define __KILLU_PAGE_H__

#include <qtextstream.h>
#include "xmlparser.h"
#include "layer.h"

enum TFormat
{
	TF_A3,
	TF_A4,
	TF_A5,
	TF_USLETTER,
	TF_USLEGAL,
	TF_SCREEN,
	TF_CUSTOM,
	TF_B3,
	TF_USEXECUTIVE
};

enum TOrient
{
	TO_PORTRAIT,
	TO_LANDSCAPE
};


/***********************************************************************/
/* Class: Page                                                         */
/***********************************************************************/

/**
 * This class hold a whole document with its rectangles, layers, ...
 * It can generate a latex file (with pstricks extension).
 */
class Page: public XmlParser
{
	/* LAYOUT */
	TFormat   _format;
	double    _width,
		  _height;
	TOrient   _orientation;
	double    _leftMargin,
		  _rightMargin,
		  _bottomMargin,
		  _topMargin;

	Layer _layer;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Document.
		 */
		Page();

		/**
		 * Destructor
		 *
		 * Remove the list of headers, footers and the body.
		 */
		virtual ~Page();

		/**
		 * Accessors
		 */
		TFormat   getFormat       () const { return _format;       }
		TOrient   getOrientation  () const { return _orientation;  }
		double    getHeight       () const { return _height;       }
		double    getWidth        () const { return _width;        }
		double    getRightMargin  () const { return _rightMargin;  }
		double    getLeftMargin   () const { return _leftMargin;   }
		double    getTopMargin    () const { return _topMargin;    }
		double    getBottomMargin () const { return _bottomMargin; }

		/**
		 * Modifiors
		 */

		void setHeight      (double h)  { _height        = h;              }
		void setWidth       (double w)  { _width         = w;              }
		void setRightMargin (double rm) { _rightMargin   = rm;             }
		void setLeftMargin  (double lm) { _leftMargin    = lm;             }
		void setTopMargin   (double tm) { _topMargin     = tm;             }
		void setBottomMargin(double bm) { _bottomMargin  = bm;             }
		void setFormat      (int f)     { _format        = (TFormat) f;    }
		void setOrientation (int o)      { _orientation  = (TOrient) o;    }
		
		void analyse(const QDomNode);
		void analyseLayout(const QDomNode);

		void generatePSTRICKS(QTextStream&);

	private:
};

#endif /* __KILLU_PAGE_H__ */
