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

#ifndef __KWORD_FILEHEADER_H__
#define __KWORD_FILEHEADER_H__

#include <qtextstream.h>

#include "xmlparser.h"

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

enum TUnit
{
	TU_MM,
	TU_CM,
	TU_PT,
	TU_INCH
};

enum TOrient
{
	TO_PORTRAIT,
	TO_LANDSCAPE
};

enum TColonne
{
	TC_1,
	TC_2,
	TC_MORE
};

enum THeadfoot
{
	TH_ALL,
	TH_FIRST,
	TH_EVODD
};

/***********************************************************************/
/* Class: FileHeader                                                   */
/***********************************************************************/

/**
 * This class hold all general information about the doc : does it use a color markup, ... ?
 * paper size, ... and generate the information about the doc (packages and extentions
 * to include, ...).
 */
class FileHeader: public XmlParser
{
	/* PAPER */
	TFormat   _format;
	double    _width[4],
		  _height[4];
	TOrient   _orientation;
	TColonne  _colonne;
	double    _columnSpc[4];
	THeadfoot _headType;
	THeadfoot _footType;
	double    _footBody[4];
	double    _headBody[4];

	/* PAPERBORDERS */
	double    _leftBorder[4],
		  _rightBorder[4],
		  _bottomBorder[4],
		  _topBorder[4];

	/* ATTRIBUTES */
	TUnit    _unite;
	bool     _hasHeader;
	bool     _hasFooter;

	/* FOOTNOTEMGR */

	/* DIVERSE */
	/* for special packages to include */
	bool     _hasColor;
	bool     _hasUnderline;
	bool     _hasEnumerate;
	
	public:
		/**
		 * Constructor
		 */
		FileHeader();

		/**
		 * Destructor
		 */
		virtual ~FileHeader();

		/**
		 * Accessors
		 */
		TFormat   getFormat     () const { return _format;      }
		TOrient   getOrientation() const { return _orientation; }
		TColonne  getColumns    () const { return _colonne;     }
		THeadfoot getHeadType   () const { return _headType;    }
		THeadfoot getFootType   () const { return _footType;    }
		TUnit     getUnit       () const { return _unite;       }
		bool      hasHeader     () const { return _hasHeader;   }
		bool      hasFooter     () const { return _hasFooter;   }
		bool      hasColor      () const { return _hasColor;    }
		bool      hasUnderline  () const { return _hasUnderline;}
		bool      hasEnumerate  () const { return _hasEnumerate;}

		/**
		 * Modifiors
		 */
		void setFormat     (TFormat f)  { _format       = f;              }
		void setFormat     (int f)      { _format       = (TFormat) f;    }
		void setOrientation(TOrient o)  { _orientation  = o;              }
		void setOrientation(int o)      { _orientation  = (TOrient) o;    }
		void setColumns    (TColonne c) { _colonne      = c;              }
		void setColumns    (int c)      { _colonne      = (TColonne) c;   }
		void setUnit       (int u)      { _unite        = (TUnit) u;      }
		void setHeadType   (int ht)     { _headType     = (THeadfoot) ht; }
		void setFootType   (int ft)     { _footType     = (THeadfoot) ft; }
		void useColor      ()           { _hasColor     = true;           }
		void useUnderline  ()           { _hasUnderline = true;           }
		void useEnumerate  ()           { _hasEnumerate = true;           }

		void analysePaper     (const Markup *);
		void analyseAttributs (const Markup *);

		void generate         (QTextStream &);

	private:

		void analysePaperParam(const Markup*);

		void generatePaper    (QTextStream&);
		void generatePreambule(QTextStream&);
		void generatePackage  (QTextStream&);

};

#endif /* __KWORD_FILEHEADER_H__ */
