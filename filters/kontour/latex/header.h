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

#ifndef __KILLU_FILEHEADER_H__
#define __KILLU_FILEHEADER_H__

#include <qtextstream.h>
#include <qcolor.h>
#include <qptrlist.h>

#include "xmlparser.h"

/*enum TFormat
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

enum TUnit
{
	TU_MM,
	TU_CM,
	TU_PT,
	TU_INCH
};*/

enum TOrient
{
	TO_PORTRAIT,
	TO_LANDSCAPE
};

/***********************************************************************/
/* Class: Header                                                       */
/***********************************************************************/

/**
 * This class hold all general information about the doc : does it use a color markup, ... ?
 * paper size, ... and generate the information about the doc (packages and extentions
 * to include, ...).
 */
class Header: public XmlParser
{
	/* LAYOUT */
	/*TFormat   _format;
	double    _width,
		  _height;
	TOrient   _orientation;
	double    _leftMargin,
		  _rightMargin,
		  _bottomMargin,
		  _topMargin;
	TUnit     _unit;*/
	QPtrList<QColor> _colors;
	QPtrList<QString> _colorname;

	/* GRID */
	int       _dx, _dy;
	QString   _gridColorName;
	QString   _gridColor;
	bool      _gridAlign;
	bool      _gridShow;
	bool      _helpAlign;

	/* DIVERSE */
	/* for special packages to include */
	bool     _hasColor;
	bool     _hasUnderline;
	bool     _hasEnumerate;
	bool     _hasGradient;
	bool     _useRotate;

	/* USEFULL */
	TOrient   _currentOrient;
	
	public:
		/**
		 * Constructor
		 */
		Header();

		/**
		 * Destructor
		 */
		virtual ~Header();

		/**
		 * Accessors
		 */
		/*TFormat   getFormat       () const { return _format;       }
		TOrient   getOrientation  () const { return _orientation;  }
		double    getHeight       () const { return _height;       }
		double    getWidth        () const { return _width;        }
		double    getHeightCm     () const { return (_height / 10);}
		double    getWidthCm      () const { return (_width / 10); }
		double    getRightMargin  () const { return _rightMargin;  }
		double    getLeftMargin   () const { return _leftMargin;   }
		double    getTopMargin    () const { return _topMargin;    }
		double    getBottomMargin () const { return _bottomMargin; }*/
		QString   getGridColor    () const { return _gridColor;    }
		QString   getGridColorName() const { return _gridColorName;}
		bool      isHelpAlign     () const { return _helpAlign;    }
		bool      isGridAlign     () const { return _gridAlign;    }
		bool      isGridShow      () const { return _gridShow;     }
		bool      hasColor        () const { return _hasColor;     }
		bool      hasUnderline    () const { return _hasUnderline; }
		bool      hasEnumerate    () const { return _hasEnumerate; }
		bool      hasGradient     () const { return _hasGradient;  }
		bool      mustRotate      () const { return _useRotate;    }
		int       getDx           () const { return _dx;           }
		int       getDy           () const { return _dy;           }
		//QString   getColor        (QString);

		/**
		 * Modifiors
		 */
		/*void setHeight      (double h)  { _height        = h;              }
		void setWidth       (double w)  { _width         = w;              }
		void setRightMargin (double rm) { _rightMargin   = rm;             }
		void setLeftMargin  (double lm) { _leftMargin    = lm;             }
		void setTopMargin   (double tm) { _topMargin     = tm;             }
		void setBottomMargin(double bm) { _bottomMargin  = bm;             }*/
		//void setUnit        (int unit)  { _unit          = (TUnit) unit;   }
		//void setFormat     (TFormat f)  { _format       = f;              }
		//void setFormat      (int f)     { _format        = (TFormat) f;    }
		//void setOrientation(TOrient o)  { _orientation  = o;              }
		//void setOrientation (int o)      { _orientation  = (TOrient) o;    }
		void setGridColor    (QString c)  { _gridColor     = c;        }
		void setGridColorName(QString cn) { _gridColorName = cn;       }
		void setHelpAlign    (int ha)     { _helpAlign     = ha;       }
		void setGridAlign    (int ga)     { _gridAlign     = ga;       }
		void setGridShow     (int gs)     { _gridShow      = gs;       }
		void setDx           (int dx)     { _dx            = dx;       }
		void setDy           (int dy)     { _dy            = dy;       }
		void useColor        ()           { _hasColor      = true;     }
		void useUnderline    ()           { _hasUnderline  = true;     }
		void useEnumerate    ()           { _hasEnumerate  = true;     }
		void useGradient     ()           { _hasGradient   = true;     }
		void useRotate       ()           { _useRotate     = true;     }
		void setCurrentOrient(TOrient o)  { _currentOrient = o;        }
		QString addNewColor(QString);

		double convert(double);

		void analyse            (const QDomNode);
		void generate           (QTextStream &);

	private:
		//void analyseLayoutParam (const QDomNode);
		void analyseGrid        (const QDomNode);
		void analyseGridParam   (const QDomNode);

		void generatePaper    (QTextStream&);
		void generatePreambule(QTextStream&);
		void generatePackage  (QTextStream&);

};

#endif /* __KILLU_FILEHEADER_H__ */
