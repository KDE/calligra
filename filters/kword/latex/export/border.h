//Added by qt3to4:
#include <QTextStream>
/* A TEXT IS A SET OF TITLES, A (SET OF) PARAGRAPHS OR LISTS */
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

#ifndef __KWORD_LATEXBORDER_H__
#define __KWORD_LATEXBORDER_H__

/***********************************************************************/
/* Class: Border                                                       */
/***********************************************************************/

/**
 * This class hold 2 lists of paragraphs for the text and for the footnotes.
 * NOTE: The frame analysis can be (must be) in an Element class.
 */
class Border
{
	/* DATA MARKUP */
	int  _lWidth,  _rWidth,   _tWidth, _bWidth;
	int  _lRed,    _lGreen,   _lBlue;
	int  _rRed,    _rGreen,   _rBlue;
	int  _tRed,    _tGreen,   _tBlue;
	int  _bRed,    _bGreen,   _bBlue;
	int  _lStyle , _rStyle,   _tStyle, _bStyle;
	int  _bkRed,   _bkGreen,  _bkBlue;
	int  _bleftpt, _brightpt, _btoppt, _bbottompt;

	/* USEFULL DATA */
	bool   _hasLeftBorder;
	bool   _hasRightBorder;
	bool   _hasTopBorder;
	bool   _hasBottomBorder;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instances of a Border of a text frame.
		 *
		 */
		Border()
		{
			_hasLeftBorder   = false;
			_hasRightBorder  = false;
			_hasTopBorder    = false;
			_hasBottomBorder = false;
			_lWidth    = 0,
			_rWidth    = 0;
			_tWidth    = 0;
			_bWidth    = 0;
			_lRed      = 0;
			_lGreen    = 0;
			_lBlue     = 0;
			_rRed      = 0;
			_rGreen    = 0;
			_rBlue     = 0;
			_tRed      = 0;
			_tGreen    = 0;
			_tBlue     = 0;
			_bRed      = 0;
			_bGreen    = 0;
			_bBlue     = 0;
			_lStyle    = 0;
			_rStyle    = 0;
			_tStyle    = 0;
			_bStyle    = 0;
			_bkRed     = 0;
			_bkGreen   = 0;
			_bkBlue    = 0;
			_bleftpt   = 0;
			_brightpt  = 0;
			_btoppt    = 0;
			_bbottompt = 0;
		}

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of parag and footnotes.
		 */
		virtual ~Border() {}

		/**
		 * Accessors
		 */

		bool    hasLeftBorder  () const { return _hasLeftBorder;   }
		bool    hasRightBorder () const { return _hasRightBorder;  }
		bool    hasTopBorder   () const { return _hasTopBorder;    }
		bool    hasBottomBorder() const { return _hasBottomBorder; }
		int     getLeftWidth   () const { return _lWidth;          }
		int     getRightWidth  () const { return _rWidth;          }
		int     getTopWidth    () const { return _tWidth;          }
		int     getBottomWidth () const { return _bWidth;          }
		int     getLeftRed     () const { return _lRed;            }
		int     getLeftGreen   () const { return _lGreen;          }
		int     getLeftBlue    () const { return _lBlue;           }
		int     getRightRed    () const { return _rRed;            }
		int     getRightGreen  () const { return _rGreen;          }
		int     getRightBlue   () const { return _rBlue;           }
		int     getTopRed      () const { return _tRed;            }
		int     getTopGreen    () const { return _tGreen;          }
		int     getTopBlue     () const { return _tBlue;           }
		int     getBottomRed   () const { return _bRed;            }
		int     getBottomGreen () const { return _bGreen;          }
		int     getBottomBlue  () const { return _bBlue;           }
		int     getLeftStyle   () const { return _lStyle;          }
		int     getRightStyle  () const { return _rStyle;          }
		int     getTopStyle    () const { return _tStyle;          }
		int     getBottomStyle () const { return _bStyle;          }
		int     getBkRed       () const { return _bkRed;           }
		int     getBkGreen     () const { return _bkGreen;         }
		int     betBkBlue      () const { return _bkBlue;          }
/*		int     bleftpt        () const { return _bleftpt;         }
		int     brightpt       () const { return _brightpt;        }
		int     btoppt         () const { return _btoppt;          }
		int     bbottompt      () const { return _bbottompt;       }
*/

		/**
		 * Modifiors
		 */
		void useLeftBorder  ()              { _hasLeftBorder   = true; }
		void useRightBorder ()              { _hasRightBorder  = true; }
		void useTopBorder   ()              { _hasTopBorder    = true; }
		void useBottomBorder()              { _hasBottomBorder = true; }

		void setLeftWidth  (const int w)    { _lWidth          = w; }
		void setRightWidth (const int w)    { _rWidth          = w; }
		void setTopWidth   (const int w)    { _tWidth          = w; }
		void setBottomWidth(const int w)    { _bWidth          = w; }

		void setLeftRed    (const int r)    { _lRed            = r; }
		void setLeftGreen  (const int g)    { _lGreen          = g; }
		void setLeftBlue   (const int b)    { _lBlue           = b; }
		void setRightRed   (const int r)    { _rRed            = r; }
		void setRightGreen (const int g)    { _rGreen          = g; }
		void setRightBlue  (const int b)    { _rBlue           = b; }
		void setTopRed     (const int r)    { _tRed            = r; }
		void setTopGreen   (const int g)    { _tGreen          = g; }
		void setTopBlue    (const int b)    { _tBlue           = b; }
		void setBottomRed  (const int r)    { _bRed            = r; }
		void setBottomGreen(const int g)    { _bGreen          = g; }
		void setBottomBlue (const int b)    { _bBlue           = b; }

		void setLeftStyle  (const int l)    { _lStyle          = l; }
		void setRightStyle (const int r)    { _rStyle          = r; }
		void setTopStyle   (const int t)    { _tStyle          = t; }
		void setBottomStyle(const int b)    { _bStyle          = b; }

		void setBkRed      (const int r)    { _bkRed           = r; }
		void setBkGreen    (const int g)    { _bkGreen         = g; }
		void setBkBlue     (const int b)    { _bkBlue          = b; }
/*		void setBLeftPt    (const int l)    { _bkLeftPt        = l; }
		void setBRightPt   (const int r)    { _bkRightPt       = r; }
		void setBTopPt     (const int t)    { _bkTopPt         = t; }
		void setBBottomPt  (const int b)    { _bkBottomPt      = b; }
*/

		/**
		 * Write a border frame
		 */
		void generate(QTextStream&);

	private:
};

#endif /* __KWORD_LATEXBORDER_H__ */

