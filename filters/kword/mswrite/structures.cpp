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

/*
 * structures.cpp - Process Internal Microsoft(r) Write file structures
 * This file has ugly reading/writing code that couldn't be generated.
 * It performs some higher-level sanity checks.
 *
 */

#include "structures.h"
#include "structures_private.h"

namespace MSWrite
{
	PageLayout::PageLayout () : m_numModified (0)
	{
	}

	PageLayout::~PageLayout ()
	{
	}

	PageLayout &PageLayout::operator= (const PageLayout &rhs)
	{
		if (this == &rhs)
			return *this;

		PageLayoutGenerated::operator= (rhs);
		NeedsHeader::operator= (rhs);

		m_numModified = rhs.m_numModified;

		return *this;
	}

	bool PageLayout::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PAGELAYOUT
		m_device->debug ("\n<<<< PageLayout::readFromDevice >>>>\n");
	#endif

		int numPageLayoutPages = m_header->getNumPageSectionProperty ();

	#ifdef DEBUG_PAGELAYOUT
		m_device->debug ("num pageLayoutPages: ", numPageLayoutPages);
	#endif

		// no PageLayout
		if (numPageLayoutPages == 0)
			return true;
		else if (numPageLayoutPages > 1)
			ErrorAndQuit (Error::InvalidFormat, "invalid #pageLayoutPages\n");

		// seek to the PageLayout in the file
		if (!m_device->seekInternal (m_header->getPageSectionProperty () * 128, SEEK_SET))
			ErrorAndQuit (Error::FileError, "could not seek to pageLayout\n");

		if (!PageLayoutGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_PAGELAYOUT
		Dump (magic102);
		Dump (magic512);

		Dump (pageHeight);
		Dump (pageWidth);
		Dump (pageNumberStart);
		Dump (topMargin);
		Dump (textHeight);
		Dump (leftMargin);
		Dump (textWidth);

		Dump (magic256);

		Dump (headerFromTop);
		Dump (footerFromTop);

		Dump (magic720);
		Dump (zero);
		Dump (magic1080);
		Dump (unknown);
		Dump (zero2);
	#endif

		#define UpdateModifiedCount(variable) if (m_##variable != variable##Default) m_numModified++
		UpdateModifiedCount (magic102);
		UpdateModifiedCount (magic512);
		UpdateModifiedCount (pageHeight);
		UpdateModifiedCount (pageWidth);
		UpdateModifiedCount (pageNumberStart);
		UpdateModifiedCount (topMargin);
		UpdateModifiedCount (textHeight);
		UpdateModifiedCount (leftMargin);
		UpdateModifiedCount (textWidth);
		UpdateModifiedCount (magic256);
		UpdateModifiedCount (headerFromTop);
		UpdateModifiedCount (footerFromTop);
		UpdateModifiedCount (magic720);
		UpdateModifiedCount (zero);
		UpdateModifiedCount (magic1080);
		//UpdateModifiedCount (unknown);	// no reliable default for unknown
		UpdateModifiedCount (zero2);
		#undef UpdateModifiedCount

		return true;
	}

	bool PageLayout::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PAGELAYOUT
		m_device->debug ("\n>>>> PageLayout::writeToDevice <<<<\n");
	#endif

		m_header->setPageSectionProperty (m_device->tellInternal () / 128);

		// if we're just using defaults, we don't need a pageLayout
		if (!getIsModified ())
			return true;

		if (!PageLayoutGenerated::writeToDevice ())
			return false;

		return true;
	}


	Font::Font (const Byte *name, const Byte family) : m_name (NULL)
	{
		if (name) setName (name);
		FontGenerated::m_family = family;
	}

	Font::~Font ()
	{
		delete [] m_name;
	}

	Font &Font::operator= (const Font &rhs)
	{
		if (this == &rhs)
			return *this;

		FontGenerated::operator= (rhs);

		setName (rhs.getName ());

		return *this;
	}

	bool Font::readFromDevice (void)
	{
	CHECK_DEVICE;

		if (!FontGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_FONT
		Dump (numDataBytes);
		Dump (family);
	#endif

		// is this font on the next page?
		if (m_numDataBytes == 0xFFFF)
			return false;	// Device::m_error will be clear so this is ok

		// is this the last font _ever_?
		if (m_numDataBytes == 0)
			return false;	// Device::m_error will be clear so this is ok

		// Font structure size can't exceed a page anyway
		if (m_numDataBytes > 128 - sizeof (m_numDataBytes))
			ErrorAndQuit (MSWrite::Error::InvalidFormat, "Font nameLen is too big\n");

		// calculate length of name
		int nameLen = m_numDataBytes - sizeof (m_family) - 1	/* NUL */;

		// we should really call this->setName()...
		m_name = new Byte [nameLen + 1];
		if (!m_name)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for fontName\n");

		// read fontName
		if (!m_device->readInternal (m_name, nameLen + 1))
			ErrorAndQuit (Error::FileError, "could not read fontName\n");

		// check name is valid
		if (m_name [nameLen] != '\0')
			ErrorAndQuit (Error::InvalidFormat, "fontName not NUL-terminated\n");

	#ifdef DEBUG_FONT
		m_device->debug ("\tfontName=", (const char *) m_name);
	#endif
		return true;
	}

	bool Font::writeToDevice (void)
	{
	CHECK_DEVICE;

		// should I be on the next page? (TODO: this check is stupid if it's the last font)
		Word offset = m_device->tellInternal () % 128;	// from start of current page
		if (offset + sizeof (m_numDataBytes) + m_numDataBytes + sizeof (m_numDataBytes)/*for next Font's 0xFFFF*/ > 128)
		{
			// signal that font is on next page
			Byte scratch [2] = {0xFF, 0xFF};
			if (!m_device->writeInternal (scratch, 2))
				ErrorAndQuit (Error::FileError, "could not write Font 0xFFFF\n");
			return false;	// Device::m_error will be clear so this is ok
		}

		// --- we don't have to write 0 to indicate last font (since we write the right number of elements) ---

		// write numDataBytes, family
		if (!FontGenerated::writeToDevice ())
			return false;

		if (!m_device->writeInternal (m_name, m_numDataBytes - sizeof (m_family)))
			ErrorAndQuit (Error::FileError, "could not write fontName\n");

		return true;
	}


	FormatCharProperty::FormatCharProperty () : m_fontTable (NULL)
	{
		// m_afterEndCharByte =
		// m_font =
	}

	FormatCharProperty::~FormatCharProperty ()
	{
	}

	FormatCharProperty &FormatCharProperty::operator= (const FormatCharProperty &rhs)
	{
		if (this == &rhs)
			return *this;

		FormatCharPropertyGenerated::operator= (rhs);

		m_afterEndCharByte = rhs.m_afterEndCharByte;
		m_fontTable = rhs.m_fontTable;
		m_font = rhs.m_font;

		return *this;
	}

	bool FormatCharProperty::operator== (FormatCharProperty &rhs)
	{
		DWord numDataBytes;

		if ((numDataBytes = getNumDataBytes ()) != rhs.getNumDataBytes ())
			return false;

		writeToArray ();
		rhs.writeToArray ();

		return memcmp (m_data + sizeof (m_numDataBytes), rhs.m_data + sizeof (m_numDataBytes), numDataBytes) == 0;
	}

	bool FormatCharProperty::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_CHAR
		m_device->debug ("\nFormatCharProperty::readFromDevice\n");
	#endif

		if (!FormatCharPropertyGenerated::readFromDevice ())
			return false;

		if (!m_fontTable)
			ErrorAndQuit (Error::InternalError, "m_fontTable not setup for FormatCharProperty::readFromDevice\n");

		// note: the converse is done in FormatInfoPage::add(), not FormatCharProperty::writeToDevice
		if (!updateFont ()) return false;

		return true;
	}

	bool FormatCharProperty::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_CHAR
		m_device->debug ("\nFormatCharProperty::writeToDevice, #bytes=", getNumDataBytes ());
	#endif

		return FormatCharPropertyGenerated::writeToDevice ();
	}

	bool FormatCharProperty::updateFont (void)
	{
		Font *fontPtr = m_fontTable->getFont (getFontCode ());
		if (!fontPtr)
			ErrorAndQuit (Error::InvalidFormat, "fontCode out of range\n");

		m_font = *fontPtr;
		return true;
	}

	bool FormatCharProperty::updateFontCode (void)
	{
		DWord code = m_fontTable->addFont (&m_font);
		if (code == 0xFFFFFFFF)
			return false;
		else
		{
			setFontCode (code);
			return true;
		}
	}


	FormatParaPropertyTabulator::FormatParaPropertyTabulator ()
	{
	}

	FormatParaPropertyTabulator::~FormatParaPropertyTabulator ()
	{
	}

	FormatParaPropertyTabulator &FormatParaPropertyTabulator::operator= (const FormatParaPropertyTabulator &rhs)
	{
		if (this == &rhs)
			return *this;

		FormatParaPropertyTabulatorGenerated::operator= (rhs);

		return *this;
	}

	bool FormatParaPropertyTabulator::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PARA_TAB
		m_device->debug ("\nFormatParaPropertyTabulator::readFromDevice\n");
	#endif

		if (!FormatParaPropertyTabulatorGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_PARA_TAB
		Dump (indent);
		Dump (type);
		Dump (zero);
	#endif

		return true;
	}

	bool FormatParaPropertyTabulator::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PARA_TAB
		m_device->debug ("\nFormatParaPropertyTabulator::writeToDevice\n");
	#endif

		return FormatParaPropertyTabulatorGenerated::writeToDevice ();
	}


	FormatParaProperty::FormatParaProperty () : m_leftMargin (0xFFFF),
																m_rightMargin (0xFFFF),
																m_numTabulators (0),
																m_addedTooManyTabs (false)
	{
		// m_afterEndCharByte =
	}

	FormatParaProperty::~FormatParaProperty ()
	{
	}

	FormatParaProperty &FormatParaProperty::operator= (const FormatParaProperty &rhs)
	{
		if (this == &rhs)
			return *this;

		FormatParaPropertyGenerated::operator= (rhs);

		m_afterEndCharByte = rhs.m_afterEndCharByte;
		m_leftMargin = rhs.m_leftMargin, m_rightMargin = rhs.m_rightMargin;
		m_numTabulators = rhs.m_numTabulators;
		m_addedTooManyTabs = rhs.m_addedTooManyTabs;

		return *this;
	}

	bool FormatParaProperty::operator== (FormatParaProperty &rhs)
	{
		DWord numDataBytes;
		if ((numDataBytes = getNumDataBytes ()) != rhs.getNumDataBytes ())
			return false;

		writeToArray ();
		rhs.writeToArray ();

		return memcmp (m_data + sizeof (m_numDataBytes), rhs.m_data + sizeof (m_numDataBytes), numDataBytes) == 0;
	}

	bool FormatParaProperty::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PARA
		m_device->debug ("\nFormatParaProperty::readFromDevice\n");
	#endif

	#ifdef CHECK_INTERNAL
		if (m_leftMargin == 0xFFFF && m_rightMargin == 0xFFFF)
			ErrorAndQuit (Error::InternalError, "FormatParaProperty::readFromDevice call not setup\n");
	#endif

		if (!FormatParaPropertyGenerated::readFromDevice ())
			return false;

		// Correct indentation in Header and Footer:
		//
		// For unknown reasons, Write adds the leftMargin to the leftIndent
		// and rightMargin to rightIndent in the Header and Footer only but
		// does not touch the leftIndentFirstLine
		//
		// TODO: what if it's an image in the header/footer?
		if (getIsNotNormalParagraph ())
		{
			// adjust leftIndent
			if (m_leftMargin < m_leftIndent)
				m_leftIndent -= m_leftMargin;
			else
				m_leftIndent = 0;

			// adjust rightIndent
			if (m_rightMargin < m_rightIndent)
				m_rightIndent -= m_rightMargin;
			else
				m_rightIndent = 0;
		}

		if (m_numDataBytes > 22)
			m_numTabulators = (m_numDataBytes - 22) / FormatParaPropertyTabulator::s_size;
		else
			m_numTabulators = 0;

		// for impexp
		if (getNumDataBytes () != m_numDataBytes && !m_numTabulators)
			m_device->error (Error::Warn, "m_numDataBytes != getNumDataBytes ()\n");
		UseThisMuch::signalHaveSetData (false, m_numDataBytes * 8);

		return true;
	}

	bool FormatParaProperty::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef CHECK_INTERNAL
		if (m_leftMargin == 0xFFFF && m_rightMargin == 0xFFFF)
			ErrorAndQuit (Error::InternalError, "FormatParaProperty::writeToDevice call not setup\n");
	#endif

	#ifdef DEBUG_PARA
		m_device->debug ("\nFormatParaProperty::writeToDevice, #bytes=", getNumDataBytes ());
	#endif

		if (m_addedTooManyTabs)
			ErrorAndQuit (Error::InternalError, "cannot have more than 14 tabulators; shouldn't even have more than 12\n");

		if (m_numTabulators > 12)
			m_device->error (Error::Warn, "should not have more than 12 tabulators since you can only access 12 tabs via the GUI\n");

		// --- indents adjusted in updateIndents() called by FormatInfoPage::add() ---

		if (!FormatParaPropertyGenerated::writeToDevice ())
			return false;

		return true;
	}


	Image::Image () : m_externalImage (NULL),
							m_externalImageSize (0),
							m_externalImageUpto (0),
							m_originalWidth (0), m_originalHeight (0),
							m_displayedWidth (0), m_displayedHeight (0)
	{
	}

	Image::~Image ()
	{
		delete [] m_externalImage;
	}

	Image &Image::operator= (const Image &rhs)
	{
		if (this == &rhs)
			return *this;

		ImageGenerated::operator= (rhs);

		m_externalImageSize = rhs.m_externalImageSize;
		m_externalImageUpto = rhs.m_externalImageUpto;

		delete [] m_externalImage;
		m_externalImage = new Byte [m_externalImageSize];
		if (!m_externalImage)
			return *this;  // TODO: error check

		if (rhs.m_externalImage)
			memcpy (m_externalImage, rhs.m_externalImage, m_externalImageUpto);

		m_originalWidth = rhs.m_originalWidth;
		m_originalHeight = rhs.m_originalHeight;

		m_displayedWidth = rhs.m_displayedWidth;
		m_displayedHeight = rhs.m_displayedHeight;

		return *this;
	}

	// returns how many bytes are needed for each scanline (with byte padding)
	int Image::getBytesPerScanLine (const int width, const int bitsPerPixel, const int padBytes)
	{
		int bitWidth;
		int byteWidth;

		// figure out how many bytes it would take to represent "width" pixels, each taking "bitsPerPixel" bits
		bitWidth = width * bitsPerPixel;
		byteWidth = bitWidth / 8;
		if (bitWidth % 8)
			byteWidth++;

		// pad the number of bytes taken to the next multiple of "padBytes"
		// (if not divisible by "padBytes")
		byteWidth = (byteWidth + padBytes - 1) / padBytes * padBytes;

		return byteWidth;
	}

	bool Image::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n<<<< Image::readFromDevice >>>>\n");
	#endif

		if (!ImageGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_IMAGE
		Dump (mappingMode);
		Dump (MFP_width);
		Dump (MFP_height);
		Dump (MFP_unknown);

		Dump (indent);
		Dump (width);
		Dump (height);
		Dump (zero);

		Dump (numHeaderBytes);
		Dump (numDataBytes);
		Dump (horizontalScalingRel1000);
		Dump (verticalScalingRel1000);
	#endif

		if (getIsWMF ())
		{
			//
			// get image dimensions
			//

			if (m_bmh->getWidth () || m_bmh->getHeight ())
				m_device->error (Error::Warn, "m_bmh structure should be 0 for WMFs\n");

			m_originalWidth = Milli2Twip (double (m_MFP_width) / 100.0) * 4.0/3.0;
			m_originalHeight = Milli2Twip (double (m_MFP_height) / 100.0) * 4.0/3.0;

			m_displayedWidth = double (m_width);
			m_displayedHeight = double (m_height);

			if (m_horizontalScalingRel1000 != 1000)
				m_device->error (Error::Warn, "horizontal scaling should not be set for WMFs\n");
			if (m_verticalScalingRel1000 != 1000)
				m_device->error (Error::Warn, "vertical scaling should not be set for WMFs\n");


			//
			// read image
			//

			m_externalImage = new Byte [m_externalImageSize = getNumDataBytes ()];
			if (!m_externalImage)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for external WMF image\n");

			if (!m_device->readInternal (m_externalImage, m_externalImageSize))
				ErrorAndQuit (Error::FileError, "could not read internal WMF\n");

			// Header check
			WMFHeader wmfHeader;
			m_device->setCache (m_externalImage);
				wmfHeader.setDevice (m_device);
				if (!wmfHeader.readFromDevice ())
					return false;

				// TODO: fix this incorrect check (see wmf.cpp)
				//if (wmfHeader.getFileSize () * sizeof (Word) != m_numDataBytes)
				//	m_device->error (Error::Warn, "wmfHeader.fileSize != numDataBytes\n");
			m_device->setCache (NULL);
		}
		else	//	if (getIsBMP ())
		{
			//
			// get image dimensions
			//

			m_originalWidth = Point2Twip (DWord (m_bmh->getWidth ()));
			m_originalHeight = Point2Twip (DWord (m_bmh->getHeight ()));

			m_displayedWidth = m_originalWidth / 1.38889 * m_horizontalScalingRel1000 / 1000;
			m_displayedHeight = m_originalHeight / 1.38889 * m_verticalScalingRel1000 / 1000;

#define MSWrite_fabs(val) (((val)>=0)?(val):(-(val)))

			if (MSWrite_fabs (m_MFP_width / double (m_bmh->getWidth ()) - 2.64) > .3)
				m_device->error (Error::Warn, "m_MFP_width != m_bmh->getWidth() * 2.64\n");
			if (MSWrite_fabs (m_MFP_height / double (m_bmh->getHeight ()) - 2.64) > .3)
				m_device->error (Error::Warn, "m_MFP_height != m_bmh->getHeight() * 2.64\n");

#undef MSWrite_fabs

			if (m_width)
				m_device->error (Error::Warn, "m_width should not be set for BMPs\n");

			if (m_height)
				m_device->error (Error::Warn, "m_height should not be set for BMPs\n");


			//
			// read image
			//

			Byte *internalData = new Byte [getNumDataBytes ()];
			if (!internalData)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for internal BMP image\n");
			if (!m_device->readInternal (internalData, getNumDataBytes ()))
				ErrorAndQuit (Error::FileError, "could not read internal BMP\n");

			// infoHeader
			BMP_BitmapInfoHeader infoHeader;
			infoHeader.setWidth (m_bmh->getWidth ());
			infoHeader.setHeight (m_bmh->getHeight ());
			if (m_bmh->getNumPlanes () != 1)
				ErrorAndQuit (Error::InvalidFormat, "bmh.m_numPlanes != 1\n");
			infoHeader.setNumPlanes (m_bmh->getNumPlanes ());
			infoHeader.setBitsPerPixel (m_bmh->getBitsPerPixel ());
			infoHeader.setCompression (0);	// BI_RGB (uncompressed)
			infoHeader.setSizeImage (0);		// lazy
			infoHeader.setXPixelsPerMeter (0), infoHeader.setYPixelsPerMeter (0);
			infoHeader.setColorsUsed (1 << infoHeader.getBitsPerPixel ());
			infoHeader.setColorsImportant (infoHeader.getColorsUsed ());

			if (infoHeader.getColorsUsed () != 2)
				ErrorAndQuit (Error::InternalError, "color bitmap???  Please email clarencedang@users.sourceforge.net this file\n");

			Word colorTableSize = infoHeader.getColorsUsed () * BMP_BitmapColorIndex::s_size;

			// fileHeader
			BMP_BitmapFileHeader fileHeader;
			DWord fileSize = BMP_BitmapFileHeader::s_size + BMP_BitmapInfoHeader::s_size
										+ colorTableSize
										+ (m_bmh->getHeight ()
											* getBytesPerScanLine (m_bmh->getWidth (), m_bmh->getBitsPerPixel (), 4));

			fileHeader.setTotalBytes (fileSize);
			fileHeader.setActualImageOffset (BMP_BitmapFileHeader::s_size + BMP_BitmapInfoHeader::s_size
														+ colorTableSize);

			// colorTable
			BMP_BitmapColorIndex *colorIndex = new BMP_BitmapColorIndex [infoHeader.getColorsUsed ()];
			if (!colorIndex)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for colorIndex[]\n");


			// black and white...
			colorIndex [0].setRed (0), colorIndex [0].setGreen (0), colorIndex [0].setBlue (0);
			colorIndex [1].setRed (0xFF), colorIndex [1].setGreen (0xFF), colorIndex [1].setBlue (0xFF);

			m_externalImage = new Byte [m_externalImageSize = fileSize];
			if (!m_externalImage)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for external BMP image\n");

			MemoryDevice device;
			device.setCache (m_externalImage);
			fileHeader.setDevice (&device);
			fileHeader.writeToDevice ();
			infoHeader.setDevice (&device);
			infoHeader.writeToDevice ();
			for (int i = 0; i < 2; i++)
			{
				colorIndex [i].setDevice (&device);
				colorIndex [i].writeToDevice ();
			}

			// write out each scanline
			// (BMP padded to 4 bytes vs WRI input bitmap which is actually padded to 2)
			Word scanLineWRILength = getBytesPerScanLine (infoHeader.getWidth (), infoHeader.getBitsPerPixel (), 2);
			if (scanLineWRILength != m_bmh->getWidthBytes ())
				ErrorAndQuit (Error::InvalidFormat, "scanLineWRILength != m_bmh->getWidthBytes()\n");
			Word scanLineBMPLength = getBytesPerScanLine (infoHeader.getWidth (), infoHeader.getBitsPerPixel (), 4);

		#ifdef DEBUG_IMAGE
			m_device->debug ("in: scanLineWRILength: ", scanLineWRILength);
			m_device->debug ("out: scanLineBMPLength: ", scanLineBMPLength);
		#endif

			Byte *padding = new Byte [scanLineBMPLength - scanLineWRILength];
			if (!padding)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for scanline\n");
			memset (padding, 0, scanLineBMPLength - scanLineWRILength);

			// the DIB is upside-down...
			for (int i = (int) infoHeader.getHeight () - 1; i >= 0; i--)
			{
				// write bitmap scanline
				device.writeInternal (internalData + i * scanLineWRILength, scanLineWRILength * sizeof (Byte));

				// write padding for scanline
				device.writeInternal (padding, (scanLineBMPLength - scanLineWRILength) * sizeof (Byte));
			}

			delete [] padding;

			device.setCache (NULL);

			delete [] colorIndex;
			delete [] internalData;
		}

		return true;
	}

	bool Image::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n>>>> Image::writeToDevice <<<<\n");
	#endif

	#ifdef DEBUG_IMAGE
		Dump (mappingMode);
		//Dump (MFP_width);	// will change below
		//Dump (MFP_height);
		Dump (MFP_unknown);

		Dump (indent);
		//Dump (width);
		//Dump (height);
		Dump (zero);

		Dump (numHeaderBytes);
		//Dump (numDataBytes);
		//Dump (horizontalScalingRel1000);
		//Dump (verticalScalingRel1000);
	#endif

		//
		// write data
		//
		//
		if (getIsWMF ())
		{
			// Header check
			WMFHeader wmfHeader;
			m_device->setCache (m_externalImage);
				wmfHeader.setDevice (m_device);
				if (!wmfHeader.readFromDevice ()) return false;

				// TODO: fix this incorrect check (see wmf.cpp)
				//if (wmfHeader.getFileSize () * sizeof (Word) != m_externalImageSize)
				//	m_device->error (Error::Warn, "wmfHeader.fileSize != externalImageSize\n");
			m_device->setCache (NULL);


			//
			// set image dimensions
			//

			// entire BitmapHeader is unused with WMFs
			m_bmh->setWidth (0);
			m_bmh->setHeight (0);
			m_bmh->setWidthBytes (0);
			m_bmh->setNumPlanes (0);
			m_bmh->setBitsPerPixel (0);

			m_MFP_width = Word (Twip2Milli (m_originalWidth * 0.75) * 100.0);
			m_MFP_height = Word (Twip2Milli (m_originalHeight * 0.75) * 100.0);

			m_width = Word (m_displayedWidth);
			m_height = Word (m_displayedHeight);

			// not used by WMFs
			m_horizontalScalingRel1000 = m_verticalScalingRel1000 = 1000;


			// write header
			setNumDataBytes (m_externalImageSize);
			if (!ImageGenerated::writeToDevice ())
				return false;

			// external=internal with WMF (i.e. we really do write a WMF)
			if (!m_device->writeInternal (m_externalImage, m_externalImageSize)) return false;
		}
		else	//	if (getIsBMP ())
		{
			m_device->setCache (m_externalImage);

			BMP_BitmapFileHeader fileHeader;
			fileHeader.setDevice (m_device);
			if (!fileHeader.readFromDevice ()) return false;


			/*Word colorTableSize = (1 << m_bmh->getNumPlanes ()) * BMP_BitmapColorIndex::s_size;

			// fileHeader
			DWord fileSize = BMP_BitmapFileHeader::s_size + BMP_BitmapInfoHeader::s_size
												+ colorTableSize
												+ (m_bmh->getHeight ()
													* getBytesPerScanLine (m_bmh->getWidth (), m_bmh->getBitsPerPixel (), 4));

			fileHeader.setTotalBytes (fileSize);
			fileHeader.setActualImageOffset (BMP_BitmapFileHeader::s_size + BMP_BitmapInfoHeader::s_size
														+ colorTableSize);*/

			// infoHeader
			BMP_BitmapInfoHeader infoHeader;
			infoHeader.setDevice (m_device);
			if (!infoHeader.readFromDevice ()) return false;

			// write out each scanline
			// to .WRI (padded to 2) vs input BMP (padded to 4
			Word scanLineWRILength = getBytesPerScanLine (infoHeader.getWidth (), infoHeader.getBitsPerPixel (), 2);
			Word scanLineBMPLength = getBytesPerScanLine (infoHeader.getWidth (), infoHeader.getBitsPerPixel (), 4);

			if (infoHeader.getWidth () != Word (Twip2Point (m_originalWidth)))
				ErrorAndQuit (Error::Warn, "infoHeader width != m_originalWidth\n");
			if (infoHeader.getHeight () != Word (Twip2Point (m_originalHeight)))
				ErrorAndQuit (Error::Warn, "infoHeader.height != m_originalHeight\n");

			m_bmh->setWidth (infoHeader.getWidth ());
			m_bmh->setHeight (infoHeader.getHeight ());
			m_bmh->setWidthBytes (scanLineWRILength);
			if (infoHeader.getNumPlanes () != 1)
				ErrorAndQuit (Error::InvalidFormat, "infoHeader.getNumPlanes() != 1\n");
			m_bmh->setNumPlanes (infoHeader.getNumPlanes ());
			m_bmh->setBitsPerPixel (infoHeader.getBitsPerPixel ());
			if (infoHeader.getCompression () != 0)	// BI_RGB (uncompressed)
				ErrorAndQuit (Error::Unsupported, "compressed bitmaps unsupported\n");
			//infoHeader.setSizeImage (0);		// lazy
			//infoHeader.setXPixelsPerMeter (0), infoHeader.setYPixelsPerMeter (0);
			infoHeader.setColorsUsed (1 << infoHeader.getBitsPerPixel ());	// make life easier
			//infoHeader.setColorsImportant (infoHeader.getColorsUsed ());

			if (infoHeader.getColorsUsed () != 2)
				ErrorAndQuit (Error::Unsupported, "can't save color BMPs, use WMFs for that purpose\n");

			// colorTable
			BMP_BitmapColorIndex *colorIndex = new BMP_BitmapColorIndex [infoHeader.getColorsUsed ()];
			if (!colorIndex)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for colorIndex[]\n");
			colorIndex [0].setDevice (m_device);
			if (!colorIndex [0].readFromDevice ()) return false;
			if (colorIndex [0].getRed () != 0 || colorIndex [0].getGreen () != 0 || colorIndex [0].getBlue () != 0)
				m_device->error (Error::Warn, "black not black\n");
			colorIndex [1].setDevice (m_device);
			if (!colorIndex [1].readFromDevice ()) return false;
			if (colorIndex [1].getRed () != 0xFF || colorIndex [1].getGreen () != 0xFF || colorIndex [1].getBlue () != 0xFF)
				m_device->error (Error::Warn, "white not white\n");

			// finish reading from m_externalImage
			m_device->setCache (NULL);


			//
			// set image dimensions
			//

			m_MFP_width = Word (Twip2Point (m_originalWidth) * 2.64);
			m_MFP_height = Word (Twip2Point (m_originalHeight) * 2.64);

			// BMPs don't use
			m_width = 0, m_height = 0;

			m_horizontalScalingRel1000 = Word (m_displayedWidth * 1.38889 * 1000.0 / m_originalWidth);
			m_verticalScalingRel1000 = Word (m_displayedHeight * 1.38889 * 1000.0 / m_originalHeight);


			// write header
			setNumDataBytes (infoHeader.getHeight () * scanLineBMPLength);
			if (!ImageGenerated::writeToDevice ())
				return false;

			// the DIB is upside-down...
			Byte *bmpData = m_externalImage + fileHeader.getActualImageOffset () + (infoHeader.getHeight () - 1) * scanLineBMPLength;
			for (int i = (int) infoHeader.getHeight () - 1; i >= 0; i--)
			{
				// write bitmap scanline (padded to 2)
				//if (!m_device->writeInternal (m_externalImage + fileHeader.getActualImageOffset () + i * scanLineBMPLength, scanLineWRILength))
				if (!m_device->writeInternal (bmpData, scanLineWRILength))
					return false;

				bmpData -= scanLineBMPLength;
			}

			delete [] colorIndex;
		}

		return true;
	}


	OLE::OLE () : m_externalObject (NULL),
						m_externalObjectSize (0), m_externalObjectUpto (0)
	{
	}

	OLE::~OLE ()
	{
		delete [] m_externalObject;
	}

	OLE &OLE::operator= (const OLE &rhs)
	{
		if (this == &rhs)
			return *this;

		OLEGenerated::operator= (rhs);

		m_externalObjectSize = rhs.m_externalObjectSize;
		m_externalObjectUpto = rhs.m_externalObjectUpto;

		delete [] m_externalObject;
		m_externalObject = new Byte [m_externalObjectSize];
		if (!m_externalObject)
			return *this;  // TODO: error check

		if (rhs.m_externalObject)
			memcpy (m_externalObject, rhs.m_externalObject, m_externalObjectUpto);

		return *this;
	}

	bool OLE::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_OBJECT
		m_device->debug ("\n<<<< OLE::readFromDevice >>>>\n");
	#endif

		if (!OLEGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_OBJECT
		Dump (zero);

		switch (m_objectType)
		{
		case OLEType::Static:
			m_device->debug ("\tobjectType: 1 - static\n");
			break;
		case OLEType::Embedded:
			m_device->debug ("\tobjectType: 2 - embedded\n");
			break;
		case OLEType::Link:
			m_device->debug ("\tobjectType: 3 - link\n");
			break;
		}

		Dump (indent);
		Dump (width);
		Dump (height);
		Dump (zero2);
		Dump (numDataBytes);
		Dump (zero3);
		Dump (objectName);
		Dump (zero4);
		Dump (numHeaderBytes);
		Dump (zero5);
		Dump (widthScaledRel1000);
		Dump (heightScaledRel1000);
	#endif

		// OPT: TODO: this is dumb, we read it only to give it back to the parser who tells the generator to write it
		m_externalObject = new Byte [m_externalObjectSize = getNumDataBytes ()];
		if (!m_externalObject)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for external OLE object\n");

		if (!m_device->readInternal (m_externalObject, m_externalObjectSize))
			return false;

		return true;
	}

	bool OLE::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_OBJECT
		m_device->debug ("\n>>>> OLE::writeToDevice <<<<\n");
	#endif

	#ifdef DEBUG_OBJECT
		Dump (zero);

		switch (m_objectType)
		{
		case OLEType::Static:
			m_device->debug ("\tobjectType: 1 - static\n");
			break;
		case OLEType::Embedded:
			m_device->debug ("\tobjectType: 2 - embedded\n");
			break;
		case OLEType::Link:
			m_device->debug ("\tobjectType: 3 - link\n");
			break;
		}

		Dump (indent);
		Dump (width);
		Dump (height);
		Dump (zero2);
		Dump (numDataBytes);
		Dump (zero3);
		Dump (objectName);
		Dump (zero4);
		Dump (numHeaderBytes);
		Dump (zero5);
		Dump (widthScaledRel1000);
		Dump (heightScaledRel1000);
	#endif

		// write header
		if (!OLEGenerated::writeToDevice ())
			return false;

		// write data
		if (!m_device->writeInternal (m_externalObject, m_externalObjectSize))
			return false;

		return true;
	}

}	// namespace MSWrite	{

// end of structures.cpp
