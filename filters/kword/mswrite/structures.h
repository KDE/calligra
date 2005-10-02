/* This file is part of the LibMSWrite Library
   Copyright (C) 2001-2003 Clarence Dang <clarencedang@users.sourceforge.net>

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

   LibMSWrite Project Website:
   http://sourceforge.net/projects/libmswrite/
*/

#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#include "structures_generated.h"
#include "structures_private.h"

namespace MSWrite
{
	class Format
	{
	public:
		static const Word Write_3_0 = 0xBE31;
		static const Word Write_3_1 = 0xBE32;
	};


	class PageLayout : public PageLayoutGenerated,
								public NeedsHeader
	{
	private:
		int m_numModified;

		bool readFromDevice (void);	friend class InternalGenerator;
		bool writeToDevice (void);	friend class InternalParser;

		// the defaults of course
		static const Byte magic102Default = Byte (102);
		static const Word magic512Default = Word (512);
		static const Word pageHeightDefault = Word (Inch2Twip (11));
		static const Word pageWidthDefault = Word (Inch2Twip (8.5));
		static const Word pageNumberStartDefault = Word (1);
		static const Word topMarginDefault = Word (Inch2Twip (1));
		static const Word textHeightDefault = Word (Inch2Twip (9));
		static const Word leftMarginDefault = Word (Inch2Twip (1.25));
		static const Word textWidthDefault = Word (Inch2Twip (6));
		static const Word magic256Default = Word (256);
		static const Word headerFromTopDefault = Word (Inch2Twip (0.75));
		static const Word footerFromTopDefault = Word (Inch2Twip (10.25 /*11.0 (m_pageHeight) - 0.75*/));
		static const Word magic720Default = Word (720);
		static const Word zeroDefault = Word (0);
		static const Word magic1080Default = Word (1080);
		// no reliable default for m_unknown
		static const Word zero2Default = Word (0);

	public:
		PageLayout ();
		virtual ~PageLayout ();

		PageLayout &operator= (const PageLayout &rhs);

		bool getIsModified (void) const	{	return m_numModified > 0;	}

		#define PageLayoutModify(variable,value)										\
		{																							\
			if (m_##variable != value)	/* don't skew m_numModified count */	\
			{																						\
				if (value == variable##Default)											\
					--m_numModified;															\
				else																				\
					m_numModified++;															\
				m_##variable = value;														\
			}																						\
		}

		Word getPageNumberStart (const bool purist = false) const
		{
			if (purist)
				return m_pageNumberStart;

			if (m_pageNumberStart == Word (0xFFFF))
				return 1;
			else
				return m_pageNumberStart;
		}
		void setPageNumberStart (const Word val)	{	PageLayoutModify (pageNumberStart, val);	}

		//
		// since these are calculated values,
		// there can't be corresponding set functions
		// (how do I know what value you want to change?)
		//
		Word getBottomMargin (void) const
		{
			return getPageHeight () - getTopMargin () - getTextHeight ();
		}

		Word getRightMargin (void) const
		{
			return getPageWidth () - getLeftMargin () - getTextWidth ();
		}


		//
		// get and set functions (could not generated because we want to keep a
		// "modified" counter)
		//

		Word getPageHeight (void) const	{	return m_pageHeight;	}
		void setPageHeight (const Word val)	{	PageLayoutModify (pageHeight, val);	}

		Word getPageWidth (void) const	{	return m_pageWidth;	}
		void setPageWidth (const Word val)	{	PageLayoutModify (pageWidth, val);	}

		Word getTopMargin (void) const	{	return m_topMargin;	}
		void setTopMargin (const Word val)	{	PageLayoutModify (topMargin, val);	}

		Word getTextHeight (void) const	{	return m_textHeight;	}
		void setTextHeight (const Word val)	{	PageLayoutModify (textHeight, val);	}

		Word getLeftMargin (void) const	{	return m_leftMargin;	}
		void setLeftMargin (const Word val)	{	PageLayoutModify (leftMargin, val);	}

		Word getTextWidth (void) const	{	return m_textWidth;	}
		void setTextWidth (const Word val)	{	PageLayoutModify (textWidth, val);	}

		Word getHeaderFromTop (void) const	{	return m_headerFromTop;	}
		void setHeaderFromTop (const Word val)	{	PageLayoutModify (headerFromTop, val);	}

		Word getFooterFromTop (void) const	{	return m_footerFromTop;	}
		void setFooterFromTop (const Word val)	{	PageLayoutModify (footerFromTop, val);	}
	};


	class Font : public FontGenerated
	{
	private:
		Byte *m_name;

		friend class FontTable;
			bool readFromDevice (void);
			bool writeToDevice (void);

	public:
		enum Family
		{
			DontCare		= 0x00,
			Roman			= 0x10,	Times			= 0x10,	Serif			= 0x10,
			Swiss			= 0x20,	Helvetica	= 0x20,	SansSerif	= 0x20,
			Modern		= 0x30,	Courier		= 0x30,	TypeWriter	= 0x30,
			Script		= 0x40,
			Decorative	= 0x50,	OldEnglish	= 0x50
		};

		Font (const Byte *name = NULL, const Byte family = DontCare);
		virtual ~Font ();

		Font &operator= (const Font &rhs);

		Byte *getName (void) const	{	return m_name;	}
		Byte *setName (const Byte *s)
		{
#if 0 // TODO fix
		#ifdef CHECK_INTERNAL
			if (!m_device)
				CHECK_DEVICE_ERROR;
		#endif

			if (!s)
			{
				m_device->error (Error::InternalError, "NULL fontName passed to Font::setName\n");
				return m_name = NULL;
			}
#endif
			int length = strlen ((const char *) s) + 1 /* NUL */;

			delete [] m_name;
			m_name = new Byte [length];
			if (!m_name)
				m_device->error (Error::OutOfMemory, "could not allocate memory for fontName\n");
			else
				strcpy ((char *) m_name, (const char *) s);

			FontGenerated::m_numDataBytes = sizeof (m_family) + length;
			
			return m_name;
		}

		bool operator!= (const Font &rhs)
		{
			// if their m_family's are different, it's not my problem...
			return strcmp ((const char *) this->m_name, (const char *) rhs.m_name) != 0;
		}

		bool operator== (const Font &rhs)
		{
			// if their m_family's are different, it's not my problem...
			return strcmp ((const char *) this->m_name, (const char *) rhs.m_name) == 0;
		}

	private:
		Font (const Font &rhs);
	};


	class FontTable;	// from structures_private.h
	class FormatCharProperty : public FormatCharPropertyGenerated
	{
	private:
		DWord m_afterEndCharByte;	// from the FormatPointer that points to us
		FontTable *m_fontTable;	// pointer to fontTable (valid only during & after FormatInfoPage::(readFromDevice|add))
		Font m_font;	// temporarily store font until we get the fontTable

		Word getFontCode (void) const
		{
			return ((Word) getFontCodeLow ()) | (((Word) getFontCodeHigh ()) << 6);
		}

		void setFontCode (const Word code)
		{
			setFontCodeHigh ((code >> 6) & BitMask3);	// OPT: I don't think we really need & BitMask3
			setFontCodeLow (code & BitMask6);
		}

		friend class FormatInfoPage;
		friend class FormatInfo;
		friend class InternalParser;
		friend class InternalGenerator;
			bool readFromDevice (void);
			bool writeToDevice (void);
		
			void setFontTable (FontTable *fontTable)	{	m_fontTable = fontTable;	}
		
			DWord getAfterEndCharByte (void) const	{	return m_afterEndCharByte;	}
			void setAfterEndCharByte (const DWord b)	{	m_afterEndCharByte = b;	}
		
			// convenience functions
			DWord getEndCharByte (void) const	{	return m_afterEndCharByte - 1;	}
			void setEndCharByte (const DWord b)	{	m_afterEndCharByte = b + 1;	}

	public:
		FormatCharProperty ();
		virtual ~FormatCharProperty ();

		FormatCharProperty &operator= (const FormatCharProperty &rhs);

		const Font *getFont (void) const
		{
			return &m_font;
		}

		void setFont (const Font *font)
		{
			m_font = *font;
		}

		// [PRIVATE] called by FormatCharProperty::readFromDevice and other places too
		bool updateFont (void);
		// [PRIVATE] called by FormatInfoPage::add
		bool updateFontCode (void);

		Byte getFontSizeHalfPoints (void) const	{	return m_fontSize;	}
		void setFontSizeHalfPoints (const Byte val)
		{
			m_fontSize = val;
			signalHaveSetData (m_fontSize == fontSizeDefault, (2 * 8)/*offset*/ + (1 * 8)/*size*/);
		}

		// loss of accuracy but look, would you prefer it in half-points or twips instead? :)
		Byte getFontSize (void) const	{	return getFontSizeHalfPoints () / 2;	}
		void setFontSize (const Byte val)	{	setFontSizeHalfPoints (val * 2);	}

		bool getIsNormalPosition (void) const	{	return getPosition () == 0;	}
		void setIsNormalPosition (void)	{	setPosition (0);	}

		bool getIsSuperscript (void) const	{	return getPosition () >= 1 && getPosition () <= 127;	}
		void setIsSuperscript (void)	{	setPosition (4);	/* common value (sometimes 10, but) */	}

		bool getIsSubscript (void) const	{	return getPosition () >= 128;	}
		void setIsSubscript (void)	{	setPosition (252);	/* common value */	}

		// compares if the contents are the same (not if it points to the same place etc. etc.)
		bool operator== (FormatCharProperty &rhs);
	};

	
	class FormatParaPropertyTabulator : public FormatParaPropertyTabulatorGenerated
	{
	private:
		friend class FormatParaPropertyGenerated;
		friend class FormatParaProperty;
			bool readFromDevice (void);
			bool writeToDevice (void);

	public:
		FormatParaPropertyTabulator ();
		virtual ~FormatParaPropertyTabulator ();

		FormatParaPropertyTabulator &operator= (const FormatParaPropertyTabulator &rhs);
		
		// you can use getType()/setType() if you really want to...
		bool getIsNormal (void) const	{	return m_type == 0;	}
		void setIsNormal (const bool yes = true)	{	m_type = (yes ? 0 : 3);	}

		bool getIsDecimal (void) const	{	return m_type == 3;	}
		void setIsDecimal (const bool yes = true)	{	m_type = (yes ? 3 : 0);	}

		// is this a sentinel tabulator?
		bool getIsDummy (void) const	{	return m_indent == 0;	}
	};


	class Alignment
	{
	public:
		static const int Left			=	0;
		static const int Centre			=	1;
		static const int Center			=	1;
		static const int Right			=	2;
		static const int Justify		=	3;
	};

	// lazy synonym
	class Align : public Alignment {};

	class LineSpacing
	{
	public:
		static const int Normal			=	240;
		static const int Single			=	240;
		static const int OneAndAHalf	=	360;
		static const int Double			=	480;
	};

	class FormatParaProperty : public FormatParaPropertyGenerated
	{
	private:
		DWord m_afterEndCharByte;	// from the FormatPointer that points to us
		Word m_leftMargin, m_rightMargin;	// needed to adjust leftIndent & rightIndent in header/footer
		int m_numTabulators;
		bool m_addedTooManyTabs;

		friend class FormatInfoPage;
		friend class FormatInfo;
		friend class InternalParser;
		friend class InternalGenerator;
			bool readFromDevice (void);
			bool writeToDevice (void);
			
			void setMargins (const Word leftMargin, const Word rightMargin)
			{
				m_leftMargin = leftMargin;
				m_rightMargin = rightMargin;
			}

			DWord getAfterEndCharByte (void) const	{	return m_afterEndCharByte;	}
			void setAfterEndCharByte (const DWord b)	{	m_afterEndCharByte = b;	}

			// convenience functions
			DWord getEndCharByte (void) const	{	return m_afterEndCharByte - 1;	}
			void setEndCharByte (const DWord b)	{	m_afterEndCharByte = b + 1;	}

	public:
		FormatParaProperty ();
		virtual ~FormatParaProperty ();

		FormatParaProperty &operator= (const FormatParaProperty &rhs);
		
		// convenience functions
		Byte getAlign (void) const	{	return getAlignment ();	}
		void setAlign (const Byte val)	{	setAlignment (val);	}
		
		Short getLeftIndentFirstLine (const bool purist = false) const
		{
			// Write _always_ ignores "First Line Indent" if it's an object
			if (getIsObject () && !purist)
			{
			#ifdef DEBUG_PARA
				if (m_leftIndentFirstLine)
				#ifdef CHECK_INTERNAL
					if (m_device)
				#endif
						m_device->debug ("user specified leftIndentFirstLine for an object=", m_leftIndentFirstLine);
				#ifdef CHECK_INTERNAL
					else
						CHECK_DEVICE_ERROR;
				#endif
			#endif

				return 0;
			}
			else
				return m_leftIndentFirstLine;
		}
		void setLeftIndentFirstLine (const Word val)
		{
			m_leftIndentFirstLine = val;
			signalHaveSetData (m_leftIndentFirstLine == leftIndentFirstLineDefault,
										8 * 8/*offset*/ + 2 * 8/*size*/);
		}

		// [PRIVATE] called by FormatInfoPage::add
		bool updateIndents (void)
		{
			if (getIsNotNormalParagraph ())
			{
				m_leftIndent += m_leftMargin;
				m_rightIndent += m_rightMargin;
			}

			return true;
		}
		
		bool getIsText (void) const	{	return !getIsObject ();	}
		void setIsText (const bool val)	{	setIsObject (!val);	}

		bool getIsNormalParagraph (void) const	{	return !getIsNotNormalParagraph ();	}
		void setIsNormalParagraph (const bool val = true)	{	setIsNotNormalParagraph (!val);	}
		
		bool getIsHeader (void) const	{	return getIsNotNormalParagraph () && getHeaderOrFooter () == 0;	}
		void setIsHeader (const bool val)
		{
			setIsNormalParagraph (false);
			setHeaderOrFooter (val ? 0 : 1);
		}

		bool getIsFooter (void) const	{	return getIsNotNormalParagraph () && getHeaderOrFooter () == 1;	}
		void setIsFooter (const bool val)
		{
			setIsNormalParagraph (false);
			setHeaderOrFooter (val ? 1 : 0);
		}

		Word getNumTabulator (void) const	{	return m_numTabulators;	}
		const FormatParaPropertyTabulator *getTabulator (const int which) const
		{
			if (which > m_numTabulators) return NULL;
			return m_tab [which];
		}

		bool addTabulator (FormatParaPropertyTabulator *fpp)
		{
			if (m_numTabulators >= 14)
			{
				// we can't return false yet because there may be no m_device
				// so we will return false in writeToDevice() instead
				m_addedTooManyTabs = true;
				return true;
			}
	
			*m_tab [m_numTabulators++] = *fpp;

			// OPT: could use fewer bytes if Tabulator can do so
			signalHaveSetData (false/*probably not default*/, (22 + m_numTabulators * FormatParaPropertyTabulator::s_size) * 8);
			return true;
		}

		// compares if the contents are the same (not if it points to the same place etc. etc.)
		bool operator== (FormatParaProperty &rhs);
	};
	

	class ObjectType
	{
	public:
		static const int NotObject = 0;
		static const int Bitmap = 1, BMP = 1;
		static const int WindowsMetaFile = 2, WMF = 2;
		static const int OLE = 3;
	};

	class Image : public ImageGenerated
	{
	private:
		Byte *m_externalImage;
		DWord m_externalImageSize;
		DWord m_externalImageUpto;

		// image dimensions in twips
		double m_originalWidth, m_originalHeight;
		double m_displayedWidth, m_displayedHeight;
		
		static int getBytesPerScanLine (const int width, const int bitsPerPixel, const int padBytes);

		// these read and write an image stored in .WRI format
		bool readFromDevice (void);	friend class InternalGenerator;
		bool writeToDevice (void);	friend class InternalParser;
	public:
		Image ();
		virtual ~Image ();

		Image &operator= (const Image &rhs);
		
		bool getIsWMF (void) const	{	return m_mappingMode != 0xE3;	}
		bool getIsBMP (void) const	{	return m_mappingMode == 0xE3;	}

		// 0x88 is a common value for WMFs
		void setIsWMF (const bool yes = true)	{	yes ? m_mappingMode = 0x88 : m_mappingMode = 0xE3;	}
		void setIsBMP (const bool yes = true)	{	yes ? m_mappingMode = 0xE3 : m_mappingMode = 0x88;	}

		
		double getOriginalWidth (void) const	{	return m_originalWidth;	}
		void setOriginalWidth (const double val)	{	m_originalWidth = val;	}
		
		double getOriginalHeight (void) const	{	return m_originalHeight;	}
		void setOriginalHeight (const double val)	{	m_originalHeight = val;	}
		
		double getDisplayedWidth (void) const	{	return m_displayedWidth;	}
		void setDisplayedWidth (const double val)	{	m_displayedWidth = val;	}
		
		double getDisplayedHeight (void) const	{	return m_displayedHeight;	}
		void setDisplayedHeight (const double val)	{	m_displayedHeight = val;	}

		
		// these read and write an image stored in an external format
		Byte *getExternalImage (void) const	{	return m_externalImage;	}
		DWord getExternalImageSize (void) const	{	return m_externalImageSize;	}

		bool setExternalImage (const Byte *data, const DWord size)
		{
		CHECK_DEVICE;

			if (!m_externalImage)
			{
				m_externalImage = new Byte [m_externalImageSize];
				if (!m_externalImage)
					ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for external image\n");
			}
			
			if (m_externalImageUpto + size > m_externalImageSize)
			{
				Dump (externalImageUpto);
				m_device->debug ("\tsize: ", size);
				Dump (externalImageSize);
				
				ErrorAndQuit (Error::InternalError, "user overflowed setExternalImage(); attempt to write too much binary data\n");
			}

			memcpy (m_externalImage + m_externalImageUpto, data, size);
			m_externalImageUpto += size;
			return true;
		}

		bool setExternalImageSize (const DWord externalImageSize)
		{
			m_externalImageSize = externalImageSize;
			m_externalImageUpto = 0;

			return true;
		}
	};


	class OLEType
	{
	public:
		static const int Static = 1;
		static const int Embedded = 2;
		static const int Link = 3;
	};

	class OLE : public OLEGenerated
	{
	private:
		Byte *m_externalObject;
		DWord m_externalObjectSize;
		DWord m_externalObjectUpto;

		// these read and write OLE stored in .WRI format
		bool readFromDevice (void);	friend class InternalGenerator;
		bool writeToDevice (void);	friend class InternalParser;

	public:
		OLE ();
		virtual ~OLE ();

		OLE &operator= (const OLE &rhs);

		Byte *getExternalObject (void) const	{	return m_externalObject;	}
		DWord getExternalObjectSize (void) const	{	return m_externalObjectSize;	}

		bool setExternalObject (const Byte *data, const DWord size)
		{
		CHECK_DEVICE;

			if (!m_externalObject)
			{
				m_externalObject = new Byte [m_externalObjectSize];
				if (!m_externalObject)
					ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for external object\n");
			}
			
			if (m_externalObjectUpto + size > m_externalObjectSize)
			{
				Dump (externalObjectUpto);
				m_device->debug ("\tsize: ", size);
				Dump (externalObjectSize);

				ErrorAndQuit (Error::InternalError, "user overflowed setExternalObject (); attempt to write too much binary data\n");
			}

			memcpy (m_externalObject + m_externalObjectUpto, data, size);
			m_externalObjectUpto += size;
			return true;
		}

		bool setExternalObjectSize (const DWord externalObjectSize)
		{
			m_externalObjectSize = externalObjectSize;
			m_externalObjectUpto = 0;

			return true;
		}
	};

}	// namespace MSWrite	{

#endif	// __STRUCTURES_H__

// end of structures.h
