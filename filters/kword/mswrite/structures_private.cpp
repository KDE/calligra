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
 * structures_private.cpp - Process Internal Microsoft(r) Write file structures
 *                        - that are hidden/abstracted away from the user
 * This file has ugly reading/writing code that couldn't be generated.
 * It performs some higher-level sanity checks.
 *
 * Don't use any of these classes as it would defeat the purpose of LibMSWrite.
 */

#include "structures_private.h"
#include "structures.h"

namespace MSWrite
{
	Header::Header ()
	{
	}

	Header::~Header ()
	{
	}

	Header &Header::operator= (const Header &rhs)
	{
		if (this == &rhs)
			return *this;

		HeaderGenerated::operator= (rhs);

		m_numCharBytes = rhs.m_numCharBytes;
		m_pageCharInfo = rhs.m_pageCharInfo;

		return *this;
	}

	bool Header::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_HEADER
		m_device->debug ("\n<<<< Header::readFromDevice >>>>\n");
	#endif

		if (!m_device->seekInternal (0, SEEK_SET)) return false;
		if (!HeaderGenerated::readFromDevice ()) return false;

	#ifdef DEBUG_HEADER
		switch (m_magic)
		{
		case 0xBE31:	m_device->debug ("normal write file\n");	break;
		case 0xBE32:	m_device->debug ("write file with OLE\n");	break;
		default:	ErrorAndQuit (Error::InternalError, "magic test passed but failed later?");
		}
	#endif

		m_numCharBytes = m_numCharBytesPlus128 - 128;

	#ifdef DEBUG_HEADER
		m_device->debug ("num bytes of data (text+images+OLE): ", m_numCharBytes);
		m_device->debug ("page # start of parainfo: ", m_pageParaInfo);
		m_device->debug ("page # footnote table: ", m_pageFootnoteTable);
		m_device->debug ("page # pageLayout: ", m_pageSectionProperty);
		m_device->debug ("page # sectionTable: ", m_pageSectionTable);
		m_device->debug ("page # pageTable: ", m_pagePageTable);
		m_device->debug ("page # fontTable: ", m_pageFontTable);
		m_device->debug ("num pages in file: ", m_numPages);
	#endif

		/*
		 * more checks
		 */

		// footnoteTables simply do not exist...
		if (m_pageFootnoteTable != m_pageSectionProperty)
			ErrorAndQuit (Error::InvalidFormat, "document should not have a footnoteTable\n");


		// cannot have a sectionProperty without a sectionTable or vice-versa

		// no sectionProperty?
		if (m_pageSectionProperty == m_pagePageTable)
		{
			// must not have a sectionTable then
			if (m_pageSectionTable != m_pagePageTable)
				ErrorAndQuit (Error::InvalidFormat, "sectionTable without sectionProperty\n");
		}
		// have sectionProperty...
		else
		{
			if (m_pageSectionTable != m_pageSectionProperty + 1)
				ErrorAndQuit (Error::InvalidFormat, "sectionTable not immediately after sectionProperty\n");

			if (m_pageSectionTable == m_pagePageTable)
				ErrorAndQuit (Error::InvalidFormat, "sectionProperty without sectionTable\n");
		}

		// charInfo page comes immediately after the header and text
		m_pageCharInfo = (m_numCharBytesPlus128 + 127) / 128;

	#ifdef DEBUG_HEADER
		m_device->debug ("page # start of charinfo: ", m_pageCharInfo);
	#endif

		// catches a lot of corrupted files
		if (m_pageCharInfo > m_pageParaInfo)
			ErrorAndQuit (Error::InvalidFormat, "charInfo page after paraInfo page\n");

		return true;
	}

	bool Header::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_HEADER
		m_device->debug ("\n<<<< Header::writeToDevice >>>>\n");
	#endif

		m_numCharBytesPlus128 = m_numCharBytes + 128;

		if (!m_device->seekInternal (0, SEEK_SET)) return false;
		if (!HeaderGenerated::writeToDevice ()) return false;

		return true;
	}


	SectionDescriptor::SectionDescriptor ()
	{
	}

	SectionDescriptor::~SectionDescriptor ()
	{
	}

	SectionDescriptor &SectionDescriptor::operator= (const SectionDescriptor &rhs)
	{
		if (this == &rhs)
			return *this;

		SectionDescriptorGenerated::operator= (rhs);

		return *this;
	}


	SectionTable::SectionTable ()
	{
	}

	SectionTable::~SectionTable ()
	{
	}

	SectionTable &SectionTable::operator= (const SectionTable &rhs)
	{
		if (this == &rhs)
			return *this;

		SectionTableGenerated::operator= (rhs);
		NeedsHeader::operator= (rhs);

		return *this;
	}

	// this function really only does some sanity checking
	// in fact, a SectionTable is quite useless in Write...
	// ...so you actually don't have to call it :)
	bool SectionTable::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PAGELAYOUT
		m_device->debug ("\n<<<< SectionTable::readFromDevice >>>>\n");
	#endif

		int numSectionTablePages = m_header->getNumPageSectionTable ();

	#ifdef DEBUG_PAGELAYOUT
		m_device->debug ("num sectionTablePages=", numSectionTablePages);
	#endif

		// no SectionTable
		if (numSectionTablePages == 0)
			return true;
		else if (numSectionTablePages > 1)
			ErrorAndQuit (Error::InvalidFormat, "invalid #sectionTablePages\n");

		// seek to the SectionTable in the file
		if (!m_device->seekInternal (m_header->getPageSectionTable () * 128, SEEK_SET))
			return false;

		if (!SectionTableGenerated::readFromDevice ()) return false;

	#ifdef DEBUG_PAGELAYOUT
		m_device->debug ("num sectionDescriptors=", m_numSectionDescriptors);
		for (int i = 0; i < 2; i++)
		{
			m_device->debug ("Dumping SED #", i);
				m_device->debug ("\tbyte after section=", m_sed [i]->getAfterEndCharByte ());
				m_device->debug ("\tsectionProperty location=", m_sed [i]->getSectionPropertyLocation ());
		}
	#endif

		if (m_numSectionDescriptors != 2)
			m_device->error (Error::Warn, "#sectionDescriptors != 2, ignoring");

		// check that 1st SectionDescriptor covers all characters
		if (m_sed [0]->getAfterEndCharByte () != m_header->getNumCharBytes ())
			m_device->error (Error::Warn, "sectionDescriptor #1 does not cover entire document\n");

		// check that 1st SectionDescriptor points to the SectionProperty
		if (m_sed [0]->getSectionPropertyLocation () != m_header->getPageSectionProperty () * DWord (128))
			m_device->error (Error::Warn, "sectionDescriptor #1 does not refer to correct sectionProperty, ignoring\n");

		// check that 2nd SectionDescriptor covers after the end of the document
		if (m_sed [1]->getAfterEndCharByte () != m_header->getNumCharBytes () + 1)
			m_device->error (Error::Warn, "sectionDescriptor #2 does not cover post-document\n");

		// check that 2nd SectionDescriptor is indeed a dummy
		if (m_sed [1]->getSectionPropertyLocation () != (DWord) 0xFFFFFFFF)
			m_device->error (Error::Warn, "sectionDescriptor #2 is not a dummy\n");

		return true;
	}

	bool SectionTable::writeToDevice (const bool needed)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PAGELAYOUT
		m_device->debug ("\n>>>> SectionTable::writeToDevice <<<<\n");
	#endif

		m_header->setPageSectionTable (m_device->tellInternal () / 128);

		// if no sectionProperty, no sectionTable
		if (!needed)
			return true;


		// set that 1st SectionDescriptor covers all characters
		m_sed [0]->setAfterEndCharByte (m_header->getNumCharBytes ());

		// set that 1st SectionDescriptor points to the SectionProperty
		m_sed [0]->setSectionPropertyLocation (m_header->getPageSectionProperty () * 128);

		// set that 2nd SectionDescriptor covers after the end of the document
		m_sed [1]->setAfterEndCharByte (m_header->getNumCharBytes () + 1);

		// set that 2nd SectionDescriptor is indeed a dummy
		m_sed [1]->setSectionPropertyLocation ((DWord) 0xFFFFFFFF);

		if (!SectionTableGenerated::writeToDevice ())
			return false;

		return true;
	}


	FontTable::FontTable ()
	{
	}

	FontTable::~FontTable ()
	{
	}

	FontTable &FontTable::operator= (const FontTable &rhs)
	{
		if (this == &rhs)
			return *this;

		FontTableGenerated::operator= (rhs);
		NeedsHeader::operator= (rhs);

		m_fontList = rhs.m_fontList;

		return *this;
	}

	bool FontTable::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FONT
		m_device->debug ("\n<<<< FontTable::readFromDevice >>>>\n");
	#endif

		int numFontTablePages = m_header->getNumPageFontTable ();

		// no FontTable
		if (numFontTablePages == 0)
			return true;

		// seek to the fontTable in the file
		if (!m_device->seekInternal (m_header->getPageFontTable () * 128, SEEK_SET))
			return false;

		if (!FontTableGenerated::readFromDevice ()) return false;

	#ifdef DEBUG_FONT
		m_device->debug ("num Fonts: ", m_numFonts);
	#endif

		bool dontAddToBack = false;
		for (int i = 0; i < FontTableGenerated::m_numFonts; i++)
		{
		#ifdef DEBUG_FONT
			m_device->debug ("Font: ", i);
		#endif

			if (!dontAddToBack)
			{
				if (!m_fontList.addToBack ())
					ErrorAndQuit (Error::OutOfMemory, "could not add Font to fontList\n");
			}
			else
				dontAddToBack = false;

			List <Font>::Iterator it = m_fontList.begin (false);
			Font &font = *it;

			// read font
			font.setDevice (m_device);
			if (!font.readFromDevice ())
			{
				// a real error...
				if (m_device->bad ())
					return false;

				//
				// ... or Font::readFromDevice() signalled something
				//

				// font is on next page?
				if (font.getNumDataBytes () == 0xFFFF)
				{
				#ifdef DEBUG_FONT
					m_device->debug ("\tcurrent offset=", m_device->tellInternal ());
					m_device->debug ("\tnext page offset=", (m_device->tellInternal () + 127) / 128 * 128);
				#endif

					// seek to next page
					if (!m_device->seekInternal ((m_device->tellInternal () + 127) / 128 * 128, SEEK_SET))
						return false;
					--i;	// still reading the same font
					dontAddToBack = true;	// don't need to add another font, just re-use this sentinel
					continue;
				}

				// no more entries (should _not_ get in here)?
				if (font.getNumDataBytes () == 0)
				{
					if (i != m_numFonts - 1)	// actually, this check isn't required
						m_device->error (Error::Warn, "font marked as last but is not\n");

					// let's not cause problems later with uninitialised Font::m_name's
					m_fontList.erase (it);

					// better quit just in case (it is better to have fewer fonts than risk errors)
					break;
				}
			}
		}	// for (int i = 0; i < m_numFonts; i++) {

		return true;
	}

	bool FontTable::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FONT
		m_device->debug ("\n>>>> FontTable::writeToDevice <<<<\n");
	#endif

		// indicate fontTable page in header
		m_header->setPageFontTable (m_device->tellInternal () / 128);

		FontTableGenerated::m_numFonts = m_fontList.getNumElements ();

		// no fontTable
		if (FontTableGenerated::m_numFonts == 0)
		{
			m_device->error (Error::Warn, "not writing fontTable\n");
			return true;
		}

		// write numFonts
		if (!FontTableGenerated::writeToDevice ()) return false;

		int i = 0;
		for (List <Font>::Iterator it = m_fontList.begin ();
				it != m_fontList.end ();
				/* don't iterate here since we might "continue" with the same font */)
		{
		#ifdef DEBUG_FONT
			m_device->debug ("\twriting font #", i);
		#endif

			Font &font = *it;

			// write font
			font.setDevice (m_device);
			if (!font.writeToDevice ())
			{
				// a real error...
				if (m_device->bad ())
					return false;

				//
				// ... or Font::readFromDevice() signalled something
				//

				// font is on next page then
			#ifdef DEBUG_FONT
				m_device->debug ("\tcurrent offset=", m_device->tellInternal ());
				m_device->debug ("\tnext page offset=", (m_device->tellInternal () + 127) / 128 * 128);
			#endif

				// seek to next page
				if (!m_device->seekInternal ((m_device->tellInternal () + 127) / 128 * 128, SEEK_SET))
					return false;

				// still writing the same font
				continue;
			}

			// get next font to write
			it++;
			i++;
		}

		return true;
	}

	Font *FontTable::getFont (const DWord fontCode) const
	{
		// OPT: inefficient, maybe an array of pointers because we can't delete fonts anyway
		List <Font>::Iterator it = m_fontList.begin ();
		for (int i = 0; i < int (fontCode) && it != m_fontList.end (); i++, ++it)
			;
		if (it == m_fontList.end ()) return NULL;
		return &(*it);
	}

	DWord FontTable::findFont (const Font *want) const
	{
		DWord code = 0;

		List <Font>::Iterator it;
		for (it = m_fontList.begin (); it != m_fontList.end (); ++it)
		{
			if ((*it) == *want)
				return code;

			code++;
		}

		// couldn't find
		return 0xFFFFFFFF;
	}

	DWord FontTable::addFont (const Font *input)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FONT
		m_device->debug ("\t\t\tTrying to add Font ", input->getName ());
	#endif

		DWord e = findFont (input);

		// already in list
		if (e != 0xFFFFFFFF)
		{
		#ifdef DEBUG_FONT
			m_device->debug ("\t\t\t\tAlready in list, fontCode=", e);
		#endif

			// return fontCode
			return e;
		}
		// new font
		else
		{
		#ifdef DEBUG_FONT
			m_device->debug ("\t\t\t\tNew font, adding to list\n");
		#endif

			if (!m_fontList.addToBack (*input))
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for next font element\n");
			return m_fontList.getNumElements () - 1;
		}
	}


	PagePointer::PagePointer ()
	{
	}

	PagePointer::~PagePointer ()
	{
	}

	PagePointer &PagePointer::operator= (const PagePointer &rhs)
	{
		if (this == &rhs)
			return *this;

		PagePointerGenerated::operator= (rhs);

		return *this;
	}

	bool PagePointer::readFromDevice (void)
	{
	CHECK_DEVICE;

		if (!PagePointerGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_PAGETABLE
		m_device->debug ("\t\tpage number: ", m_pageNumber);
		m_device->debug ("\t\tfirst char: ", m_firstCharByte);
	#endif

		return true;
	}

	bool PagePointer::writeToDevice (void)
	{
	CHECK_DEVICE;

		if (!PagePointerGenerated::writeToDevice ())
			return false;

		return true;
	}


	PageTable::PageTable () : m_pageNumberStart (0xFFFF)
	{
	}

	PageTable::~PageTable ()
	{
	}

	PageTable &PageTable::operator= (const PageTable &rhs)
	{
		if (this == &rhs)
			return *this;

		PageTableGenerated::operator= (rhs);
		NeedsHeader::operator= (rhs);

		m_pagePointerList = rhs.m_pagePointerList;
		m_pageNumberStart = rhs.m_pageNumberStart;

		m_pageTableIterator = rhs.m_pageTableIterator;

		return *this;
	}

	bool PageTable::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PAGETABLE
		m_device->debug ("\n<<<< PageTable::readFromDevice >>>>\n");
	#endif

	#ifdef CHECK_INTERNAL
		if (m_pageNumberStart == (Word) 0xFFFF || NeedsHeader::m_header == NULL)
			ErrorAndQuit (Error::InternalError, "PageTable call not setup\n");
	#endif

		// no pageTable
		if (m_header->getNumPagePageTable () == 0)
			return true;

		// seek to the pageTable in the file
		if (!m_device->seekInternal (m_header->getPagePageTable () * 128, SEEK_SET))
			return false;

		if (!PageTableGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_PAGETABLE
		Dump (numPagePointers);
		Dump (zero);
	#endif

		DWord lastCharByte = (DWord) 0xFFFFFFFF;
		Word lastPage = (Word) 0xFFFF;	// do not have to init
		for (int i = 0; i < m_numPagePointers; i++)
		{
			if (!m_pagePointerList.addToBack ())
				ErrorAndQuit (Error::OutOfMemory, "could not add pagePointer to list\n");

			PagePointer &pp = *m_pagePointerList.begin (false);

		#ifdef DEBUG_PAGETABLE
			m_device->debug ("\tPagePointer: ", i);
		#endif

			// read pointer
			pp.setDevice (m_device);
			if (!pp.readFromDevice ()) return false;

			// first pointer?
			if (i == 0)
			{
				if (pp.getPageNumber () != m_pageNumberStart)
					ErrorAndQuit (Error::InvalidFormat, "pageTable & sectionProperty disagree on pageNumberStart\n");
			}
			else
			{
				// according to KOffice 1.2:
				//
				// "the pageTable can get really out of sync with reality
				//  if the user doesn't repaginate after deleting a few pages
				//  -- so this is just a warning, not an error"
				//
				if (pp.getPageNumber () != lastPage + 1)
					m_device->error (Error::Warn, "pages don't follow each other\n");

				if (pp.getFirstCharByte () <= lastCharByte)
					ErrorAndQuit (Error::InvalidFormat, "pageTable is not going forward?\n");
			}

			// for checking, on next iteration
			lastPage = pp.getPageNumber ();
			lastCharByte = pp.getFirstCharByte ();
		}

		return true;
	}

	bool PageTable::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_PAGETABLE
		m_device->debug ("\n>>>> PageTable::writeToDevice <<<<\n");
	#endif

		m_header->setPagePageTable (m_device->tellInternal () / 128);

		// LibMSWrite directly (!) adds to the list so m_numPagePointers may be out of date
		PageTableGenerated::m_numPagePointers = m_pagePointerList.getNumElements ();

		// no pageTable
		if (m_numPagePointers == 0)
			return true;

		// write numPagePointers, zero
		if (!PageTableGenerated::writeToDevice ())
			return false;

		int i = 0;
		List <PagePointer>::Iterator it;
		for (it = m_pagePointerList.begin (); it != m_pagePointerList.end (); ++it, i++)
		{
			PagePointer &pp = (*it);

		#ifdef DEBUG_PAGETABLE
			m_device->debug ("\tPagePointer: ", i);
		#endif

			// read page pointer
			pp.setDevice (m_device);
			if (!pp.writeToDevice ())
				return false;
		}

		return true;
	}


	FormatPointer::FormatPointer () : m_afterEndCharByte (0)
	{
	}

	FormatPointer::~FormatPointer ()
	{
	}

	FormatPointer &FormatPointer::operator= (const FormatPointer &rhs)
	{
		if (this == &rhs)
			return *this;

		FormatPointerGenerated::operator= (rhs);

		m_afterEndCharByte = rhs.m_afterEndCharByte;
		m_formatProperty = rhs.m_formatProperty;

		return *this;
	}

	bool FormatPointer::readFromDevice (void)
	{
	CHECK_DEVICE;

		if (!FormatPointerGenerated::readFromDevice ())
			return false;

		m_afterEndCharByte = m_afterEndCharBytePlus128 - 128;

	#ifdef DEBUG_FORMATINFO
		m_device->debug ("\t\tafterEndCharByte: ", m_afterEndCharByte);
		m_device->debug ("\t\tformatPropertyOffset: ", m_formatPropertyOffset);
	#endif

		return true;
	}

	bool FormatPointer::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FORMATINFO
		m_device->debug ("\t\t\t\tafterEndCharByte: ", m_afterEndCharByte);
		m_device->debug ("\t\t\t\tformatPropertyOffset: ", m_formatPropertyOffset);
	#endif

		m_afterEndCharBytePlus128 = m_afterEndCharByte + 128;

		if (!FormatPointerGenerated::writeToDevice ())
			return false;

		return true;
	}


	FormatInfoPage::FormatInfoPage () : m_firstCharByte (0),
													m_formatPointer (NULL),
													m_formatCharProperty (NULL),
														m_fontTable (NULL),
													m_formatParaProperty (NULL),
														m_leftMargin (0xFFFF), m_rightMargin (0xFFFF),
													m_formatPointerUpto (0),
													m_nextCharByte (0),
													m_formatPointerPos (0), m_formatPropertyPos (123)
	{
		// m_type=
		// m_lastPropertyOffset=
		// m_numProperty=
	}

	FormatInfoPage::~FormatInfoPage ()
	{
		delete [] m_formatParaProperty;
		delete [] m_formatCharProperty;
		delete [] m_formatPointer;
	}


	bool FormatInfoPage::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FORMATINFO
		if (m_type == ParaType)
			m_device->debug ("FormatInfoPage::readFromDevice (ParaType)\n");
		else	// if (m_type == CharType)
			m_device->debug ("FormatInfoPage::readFromDevice (CharType)\n");
	#endif

		if (!FormatInfoPageGenerated::readFromDevice ())
			return false;

		m_firstCharByte = FormatInfoPageGenerated::m_firstCharBytePlus128 - 128;

	#ifdef DEBUG_FORMATINFO
		m_device->debug ("number of FormatPointers on this page: ", FormatInfoPageGenerated::m_numFormatPointers);
		m_device->debug ("byte number of first character covered by this page: ", m_firstCharByte);
	#endif

		return true;
	}

	bool FormatInfoPage::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FORMATINFO
		if (m_type == ParaType)
			m_device->debug ("FormatInfoPage::writeToDevice (ParaType)\n");
		else	// if (m_type == CharType)
			m_device->debug ("FormatInfoPage::writeToDevice (CharType)\n");
	#endif

		FormatInfoPageGenerated::m_firstCharBytePlus128 = m_firstCharByte + 128;

	#ifdef DEBUG_FORMATINFO
		m_device->debug ("\tnumber of FormatPointers on this page: ", FormatInfoPageGenerated::m_numFormatPointers);
		m_device->debug ("\tbyte number of first character covered by this page: ", m_firstCharByte);
	#endif

		// will call FormatInfoPageGenerated::writeToArray()...
		// ...which will resolve to FormatInfoPage::writeToArray()...
		// which will then call the base FormatInfoPageGenerated::writeToArray()
		return FormatInfoPageGenerated::writeToDevice ();
	}

	bool FormatInfoPage::writeToArray (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FORMATINFO
		if (m_type == ParaType)
			m_device->debug ("\tFormatInfoPage::writeToArray (ParaType)\n");
		else	// if (m_type == CharType)
			m_device->debug ("\tFormatInfoPage::writeToArray (CharType)\n");
	#endif

		MemoryDevice memdev;

		m_formatPointerPos = 0;
		m_formatPropertyPos = 123;

	#ifdef DEBUG_FORMATINFO
		m_device->debug ("\t\tWriting FormatPointer[], num=", m_numFormatPointers);
	#endif
		for (int i = 0; i < m_numFormatPointers; i++)
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\t\t\tOffset=", m_formatPointerPos);
		#endif
			// write FormatPointer
			memdev.setCache (m_packedStructs + m_formatPointerPos);
				m_formatPointer [i].setDevice (&memdev);
				if (!m_formatPointer [i].writeToDevice ()) return false;
			memdev.setCache (NULL);
			m_formatPointerPos += FormatPointer::s_size;
		}

	#ifdef DEBUG_FORMATINFO
		m_device->debug ("\t\tWriting FormatProperty[], num=", m_numProperty);
	#endif
		for (int i = 0; i < m_numProperty; i++)
		{
			// write FormatProperty
			if (m_type == ParaType)
			{
				FormatParaProperty *prop = &m_formatParaProperty [i];

				m_formatPropertyPos -= (sizeof (Byte) + prop->getNumDataBytes ());
				memdev.setCache (m_packedStructs + m_formatPropertyPos);
					prop->setDevice (&memdev);	// device was set to m_device to keep operator== happy in FormatInfoPage::add
					if (!prop->writeToDevice ()) return false;
				memdev.setCache (NULL);
			}
			else	// if (m_type == CharType)
			{
				FormatCharProperty *prop = &m_formatCharProperty [i];

				m_formatPropertyPos -= (sizeof (Byte) + prop->getNumDataBytes ());
				memdev.setCache (m_packedStructs + m_formatPropertyPos);
					prop->setDevice (&memdev);	// device was set to m_device to keep operator== happy in FormatInfoPage::add
					if (!prop->writeToDevice ()) return false;
				memdev.setCache (NULL);
			}
			#ifdef DEBUG_FORMATINFO
				m_device->debug ("\t\t\tWrote at Offset=", m_formatPropertyPos);
			#endif
		}

		if (!FormatInfoPageGenerated::writeToArray ())
			return false;

		return true;
	}

	void *FormatInfoPage::begin (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FORMATINFO
		if (m_type == ParaType)
			m_device->debug ("\nFormatInfoPage::begin (ParaType)\n");
		else	// if (m_type == CharType)
			m_device->debug ("\nFormatInfoPage::begin (CharType)\n");
	#endif

		m_formatPointerUpto = 0;
		m_nextCharByte = 0;	// for consistency checking

		m_lastPropertyOffset = -1;

		if (!m_formatPointer)
		{
			m_formatPointer = new FormatPointer [1];

			if (!m_formatPointer)
			{
				m_device->error (Error::OutOfMemory, "could not allocate memory for FormatPointer\n");
				return NULL;
			}

			m_formatPointer->setDevice (m_device);
		}

		return this->next ();
	}

	void *FormatInfoPage::next (void)
	{
		void *ret;

	#ifdef DEBUG_FORMATINFO
		if (m_type == ParaType)
			m_device->debug ("\tPara FormatPointer #", m_formatPointerUpto);
		else	// if (m_type == CharType)
			m_device->debug ("\tChar FormatPointer #", m_formatPointerUpto);
	#endif


		//
		// read Format Pointer
		//

		// check Format Pointer allocated
		if (!m_formatPointer)
		{
			m_device->error (Error::InternalError, "formatPointer not initialised - call FormatInfoPage::begin() before next()\n");
			return NULL;
		}

		// read next Format Pointer (fixed length)
		m_device->setCache (m_packedStructs + m_formatPointerUpto * FormatPointer::s_size);
		if (!m_formatPointer->readFromDevice ()) return NULL;
		m_device->setCache (NULL);
		DWord formatPointerAfterEndCharByte = m_formatPointer->getAfterEndCharByte ();

		// check that it continues on from the last
		if (formatPointerAfterEndCharByte <= m_nextCharByte)
			m_device->error (Error::Warn, "FormatPointer afterEndCharByte does not go forward\n");
		m_nextCharByte = formatPointerAfterEndCharByte;

		// check whether it is the last one (EOF-wise)
		if (formatPointerAfterEndCharByte >= m_header->getNumCharBytes ())
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\t\tThis is the last FormatPointer!\n");
		#endif

			// past EOF?
			if (formatPointerAfterEndCharByte > m_header->getNumCharBytes ())
			{
				m_device->error (Error::Warn, "FormatPointer ends after EOF, forcing it to end at EOF\n");
				m_formatPointer->setAfterEndCharByte (m_header->getNumCharBytes ());
				m_nextCharByte = m_header->getNumCharBytes ();
			}

			// but there are still more FormatPointers left?
			if (m_formatPointerUpto != m_numFormatPointers - 1)
			{
				m_device->error (Error::Warn, "FormatPointer ends at EOF but is not the last, forcing it to be the last\n");
				m_formatPointerUpto = m_numFormatPointers - 1;	// m_formatPointerUpto will get incremented at the end of this function
			}
		}


		//
		// read in corresponding Format Property
		//
		int currentPropertyOffset = m_formatPointer->getFormatPropertyOffset ();

		bool up2date = (currentPropertyOffset == m_lastPropertyOffset);
		if (!up2date) m_device->setCache (m_packedStructs + currentPropertyOffset);

		if (m_type == CharType)
		{
			if (!up2date)
			{
				// reset to defaults
				delete [] m_formatCharProperty;
				m_formatCharProperty = new FormatCharProperty [1];

				if (!m_formatCharProperty)
				{
					m_device->error (Error::OutOfMemory, "could not allocate memory for FormatCharProperty\n");
					m_device->setCache (NULL);
					return NULL;
				}

				m_formatCharProperty->setDevice (m_device);
				m_formatCharProperty->setFontTable (m_fontTable);
				if (!m_formatCharProperty->updateFont ())
				{
					m_device->setCache (NULL);
					return NULL;
				}

				// corresponding charProperty structure exists?
				if (currentPropertyOffset != 0xFFFF)
				{
					if (!m_formatCharProperty->readFromDevice ())
					{
						m_device->setCache (NULL);
						return NULL;
					}
				}
			}

			assert (m_formatCharProperty);
			m_formatCharProperty->setAfterEndCharByte (m_formatPointer->getAfterEndCharByte ());
			ret = m_formatCharProperty;
		}
		else	// if (m_type == ParaType)
		{
			if (!up2date)
			{
				// reset to defaults
				delete [] m_formatParaProperty;
				m_formatParaProperty = new FormatParaProperty [1];

				if (!m_formatParaProperty)
				{
					m_device->error (Error::OutOfMemory, "could not allocate memory for FormatParaProperty\n");
					m_device->setCache (NULL);
					return NULL;
				}

				m_formatParaProperty->setDevice (m_device);
				m_formatParaProperty->setMargins (m_leftMargin, m_rightMargin);

				// corresponding paraProperty structure exists?
				if (currentPropertyOffset != 0xFFFF)
				{
					if (!m_formatParaProperty->readFromDevice ())
					{
						m_device->setCache (NULL);
						return NULL;
					}
				}
			}

			assert (m_formatParaProperty);
			m_formatParaProperty->setAfterEndCharByte (m_formatPointer->getAfterEndCharByte ());
			ret = m_formatParaProperty;
		}

		if (!up2date) m_device->setCache (NULL);
		m_lastPropertyOffset = currentPropertyOffset;

		m_formatPointerUpto++;
		assert (m_formatPointer);
		return ret;
	}

	bool FormatInfoPage::end (void) const
	{
		assert (m_formatPointerUpto <= FormatInfoPageGenerated::m_numFormatPointers);
		return m_formatPointerUpto >= FormatInfoPageGenerated::m_numFormatPointers;
	}

	bool FormatInfoPage::add (const void *property)
	{
	CHECK_DEVICE;

		// MaxElements is true for FormatPointers since you can't have partial FormatPointers
		// 	This is also true for FormatProperties because m_numFormatPointer >= m_numProperty
		// 	so MaxElements is also the upper limit on the number of properties
		const int MaxElements = 123 / FormatPointer::s_size;

	#ifdef DEBUG_FORMATINFO
		if (m_type == ParaType)
			m_device->debug (">>>> FormatInfoPage::add (ParaType) <<<<\n");
		else	// if (m_type == CharType)
			m_device->debug (">>>> FormatInfoPage::add (CharType) <<<<\n");

		Dump (formatPointerPos);
		Dump (formatPropertyPos);
	#endif


		//
		// Allocated memory for FormatPointers yet?
		//

		if (!m_formatPointer)
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\tFIRST CALL!  Allocating memory for FormatPointer[] and FormatProperty[]\n");
		#endif

			// allocate memory for FormatPointers
			m_formatPointer = new FormatPointer [MaxElements + 1];	// lazy memory allocation (+1 in case we can merge the last one)
			if (!m_formatPointer)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for formatPointer[]\n");
			// don't need to use m_formatPointerUpto here
			FormatInfoPageGenerated::m_numFormatPointers = 0;

			// ok, better allocate memory for relevant FormatProperties
			if (m_type == ParaType)
			{
				assert (!m_formatParaProperty);
				m_formatParaProperty = new FormatParaProperty [MaxElements + 1];	// lazy memory allocation (+1 for default prop)
				m_formatParaProperty [MaxElements].setDevice (m_device);	// we will be == this default which has to writeToArray which NeedsDevice
				if (!m_formatParaProperty)
					ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for formatParaProperty[]\n");
			}
			else	// if (m_type == CharType)
			{
				assert (!m_formatCharProperty);
				m_formatCharProperty = new FormatCharProperty [MaxElements + 1];	// lazy memory allocation (+1 for default prop)
				m_formatCharProperty [MaxElements].setDevice (m_device);	// we will be == this default which has to writeToArray which NeedsDevice
				if (!m_formatCharProperty)
						ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for formatCharProperty[]\n");
			}

			m_numProperty = 0;
		}


		//
		// Setup pointers
		//

		FormatParaProperty inpp;
		FormatCharProperty incp;

		if (m_type == ParaType)
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\tTrying to add ParaProperty, #numDataBytes=", ((FormatParaProperty *) property)->getNumDataBytes ());
		#endif

			inpp = *((FormatParaProperty *) property);
			inpp.setDevice (m_device);
			inpp.setAfterEndCharByte (m_device->tellInternal () - 128);
			inpp.setMargins (m_leftMargin, m_rightMargin);
			if (!inpp.updateIndents ()) return false;
		}
		else	// if (m_type == CharType)
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\tTrying to add CharProperty, #numDataBytes=", ((FormatCharProperty *) property)->getNumDataBytes ());
		#endif

			incp = *((FormatCharProperty *) property);
			incp.setDevice (m_device);
			incp.setAfterEndCharByte (m_device->tellInternal () - 128);
			incp.setFontTable (m_fontTable);
			if (!incp.updateFontCode ()) return false;
		}


		//
		// Attempt to add Property to current page:
		//
		// Since MS programmers tried so hard to save space, we will do the same :)
		// This results in having to store variable-length structures and the use of other nasty
		// tricks such as having multiple FormatPointers pointing to the same FormatProperty.
		// Details follow in code :)
		//

		// In Write, character formatting blocks can extend across paragraphs
		// In LibMSWrite they don't (for convenience?) so here we merge FormatPointers that have been split up
		// This applies _only_ to _character formatting_ and is not MS' fault but rather the design of LibMSWrite.
		// It should _not_ be done to paragraph formatting because I don't think the user would appreciate
		// several paragraphs being merged into one multi-line paragraph (changing paragraph properties would
		// become rather difficult...)
		bool checkedLastFormatPointer = false;
		if (m_type == CharType && m_numProperty > 0 /* && m_numFormatPointers > 0 */)
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\tIs it the same property as last? ");
		#endif

			// simply extend last FormatPointer
			FormatCharProperty *lastProp = (FormatCharProperty *) m_formatPointer [m_numFormatPointers - 1].getFormatProperty ();
			if (incp == *lastProp)
			{
			#ifdef DEBUG_FORMATINFO
				m_device->debug ("Yes, extending FormatPointer #", m_numFormatPointers - 1);
				m_device->debug ("\t\tFrom ", m_formatPointer [m_numFormatPointers - 1].getAfterEndCharByte ());
			#endif

				m_formatPointer [m_numFormatPointers - 1].setAfterEndCharByte (incp.getAfterEndCharByte ());

			#ifdef DEBUG_FORMATINFO
				m_device->debug ("\t\tTo: ", incp.getAfterEndCharByte ());
			#endif
				return true;
			}

			checkedLastFormatPointer = true;
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("No\n");
		#endif
		}
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\tSame as earlier FormatPointer? ");
		#endif

		// points to same property as an earlier formatPointer?
		for (int i = 0; i < m_numFormatPointers - (checkedLastFormatPointer ? 1 : 0); i++)
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug (i);
			m_device->debug ("? ");
		#endif
			if (m_type == ParaType)
			{
				FormatParaProperty *prop = (FormatParaProperty *) m_formatPointer [i].getFormatProperty ();
				if (inpp == *prop)
				{
					// try to add new formatPointer
					if (m_formatPointerPos + FormatPointer::s_size > m_formatPropertyPos)
					{
					#ifdef DEBUG_FORMATINFO
						m_device->debug ("Yes, but out of room on this page\n");
					#endif
						return false;	// not enough room
					}

				#ifdef DEBUG_FORMATINFO
					m_device->debug ("Yes, same property as ", i);
				#endif
					m_formatPointer [m_numFormatPointers].setAfterEndCharByte (inpp.getAfterEndCharByte ());
					m_formatPointer [m_numFormatPointers].setFormatPropertyOffset (m_formatPointer [i].getFormatPropertyOffset ());
					m_formatPointer [m_numFormatPointers].setFormatProperty (prop);

					m_formatPointerPos += FormatPointer::s_size;
					m_numFormatPointers++;
					return true;
				}
			}
			else	// if (m_type == CharType)
			{
				FormatCharProperty *prop = (FormatCharProperty *) m_formatPointer [i].getFormatProperty ();
				if (incp == *prop)
				{
					// try to add new formatPointer
					if (m_formatPointerPos + FormatPointer::s_size > m_formatPropertyPos)
					{
					#ifdef DEBUG_FORMATINFO
						m_device->debug ("Yes, but out of room on this page\n");
					#endif
						return false;	// not enough room
					}

				#ifdef DEBUG_FORMATINFO
					m_device->debug ("Yes, same property as ", i);
				#endif
					m_formatPointer [m_numFormatPointers].setAfterEndCharByte (incp.getAfterEndCharByte ());
					m_formatPointer [m_numFormatPointers].setFormatPropertyOffset (m_formatPointer [i].getFormatPropertyOffset ());
					m_formatPointer [m_numFormatPointers].setFormatProperty (prop);

					m_formatPointerPos += FormatPointer::s_size;
					m_numFormatPointers++;
					return true;
				}
			}
		}
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("No, outputting full FormatPointer & Property\n");
		#endif

		// ok we can't compress, must output a full FormatPointer & FormatProperty
		if (m_type == ParaType)
		{
			int nextPropertyPos = m_formatPropertyPos;
			if (inpp.getNumDataBytes ())
				nextPropertyPos -= (sizeof (Byte) + inpp.getNumDataBytes ());

			if (m_formatPointerPos + FormatPointer::s_size > nextPropertyPos)
			{
			#ifdef DEBUG_FORMATINFO
				m_device->debug ("\tOut of room on this page!\n");
			#endif
				return false;	// not enough room
			}

			m_formatPropertyPos = nextPropertyPos;
			if (inpp.getNumDataBytes ())
				m_formatParaProperty [m_numProperty] = inpp;

			m_formatPointer [m_numFormatPointers].setAfterEndCharByte (inpp.getAfterEndCharByte ());
			if (inpp.getNumDataBytes ())
			{
				m_formatPointer [m_numFormatPointers].setFormatPropertyOffset (m_formatPropertyPos);
				m_formatPointer [m_numFormatPointers].setFormatProperty (&m_formatParaProperty [m_numProperty]);
			}
			else
			{
				m_formatPointer [m_numFormatPointers].setFormatPropertyOffset (0xFFFF/*nowhere*/);
				m_formatPointer [m_numFormatPointers].setFormatProperty (&m_formatParaProperty [MaxElements]/*default*/);
			}
			m_formatPointerPos += FormatPointer::s_size;

			if (inpp.getNumDataBytes ()) m_numProperty++;
			m_numFormatPointers++;
			return true;
		}
		else	// if (m_type == CharType)
		{
			int nextPropertyPos = m_formatPropertyPos;
			if (incp.getNumDataBytes ())
				nextPropertyPos -= (sizeof (Byte) + incp.getNumDataBytes ());

			if (m_formatPointerPos + FormatPointer::s_size > nextPropertyPos)
			{
			#ifdef DEBUG_FORMATINFO
				m_device->debug ("\tOut of room on this page!\n");
			#endif
				return false;	// not enough room
			}

			m_formatPropertyPos = nextPropertyPos;
			if (incp.getNumDataBytes ())
				m_formatCharProperty [m_numProperty] = incp;

			m_formatPointer [m_numFormatPointers].setAfterEndCharByte (incp.getAfterEndCharByte ());
			if (incp.getNumDataBytes ())
			{
				m_formatPointer [m_numFormatPointers].setFormatPropertyOffset (m_formatPropertyPos);
				m_formatPointer [m_numFormatPointers].setFormatProperty (&m_formatCharProperty [m_numProperty]);
			}
			else
			{
				m_formatPointer [m_numFormatPointers].setFormatPropertyOffset (0xFFFF/*nowhere*/);
				m_formatPointer [m_numFormatPointers].setFormatProperty (&m_formatCharProperty [MaxElements]/*default*/);
			}
			m_formatPointerPos += FormatPointer::s_size;

			if (incp.getNumDataBytes ()) m_numProperty++;
			m_numFormatPointers++;
			return true;
		}
	}


	BMP_BitmapFileHeader::BMP_BitmapFileHeader ()
	{
	}

	BMP_BitmapFileHeader::~BMP_BitmapFileHeader ()
	{
	}

	BMP_BitmapFileHeader &BMP_BitmapFileHeader::operator= (const BMP_BitmapFileHeader &rhs)
	{
		if (this == &rhs)
			return *this;

		BMP_BitmapFileHeaderGenerated::operator= (rhs);

		return *this;
	}

	bool BMP_BitmapFileHeader::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n<<<< BMP_BitmapFileHeader::readFromDevice >>>>\n");
	#endif

		if (!BMP_BitmapFileHeaderGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_IMAGE
		Dump (magic);
		Dump (totalBytes);
		Dump (zero [0]);
		Dump (zero [1]);
		Dump (actualImageOffset);
	#endif

		return true;
	}

	bool BMP_BitmapFileHeader::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n<<<< BMP_BitmapFileHeader::writeToDevice >>>>\n");
	#endif

		if (!BMP_BitmapFileHeaderGenerated::writeToDevice ())
			return false;

		return true;

	}


	BMP_BitmapInfoHeader::BMP_BitmapInfoHeader ()
	{
	}

	BMP_BitmapInfoHeader::~BMP_BitmapInfoHeader ()
	{
	}

	BMP_BitmapInfoHeader &BMP_BitmapInfoHeader::operator= (const BMP_BitmapInfoHeader &rhs)
	{
		if (this == &rhs)
			return *this;

		BMP_BitmapInfoHeaderGenerated::operator= (rhs);

		return *this;
	}

	bool BMP_BitmapInfoHeader::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n<<<< BMP_BitmapInfoHeader::readFromDevice >>>>\n");
	#endif

		if (!BMP_BitmapInfoHeaderGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_IMAGE
		Dump (numHeaderBytes);
		Dump (width);
		Dump (height);
		Dump (numPlanes);
		Dump (bitsPerPixel);
		Dump (compression);
		Dump (sizeImage);
		Dump (xPixelsPerMeter);
		Dump (yPixelsPerMeter);
		Dump (colorsUsed);
		Dump (colorsImportant);
	#endif

		return true;
	}

	bool BMP_BitmapInfoHeader::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n<<<< BMP_BitmapInfoHeader::writeToDevice >>>>\n");
	#endif

		if (!BMP_BitmapInfoHeaderGenerated::writeToDevice ())
			return false;

		return true;
	}


	BMP_BitmapColorIndex::BMP_BitmapColorIndex ()
	{
	}

	BMP_BitmapColorIndex::~BMP_BitmapColorIndex ()
	{
	}

	BMP_BitmapColorIndex &BMP_BitmapColorIndex::operator= (const BMP_BitmapColorIndex &rhs)
	{
		if (this == &rhs)
			return *this;

		BMP_BitmapColorIndexGenerated::operator= (rhs);

		return *this;
	}


	BitmapHeader::BitmapHeader ()
	{
	}

	BitmapHeader::~BitmapHeader ()
	{
	}

	BitmapHeader &BitmapHeader::operator= (const BitmapHeader &rhs)
	{
		if (this == &rhs)
			return *this;

		BitmapHeaderGenerated::operator= (rhs);

		return *this;
	}

	bool BitmapHeader::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n<<<< BitmapHeader::readFromDevice >>>>\n");
	#endif

		if (!BitmapHeaderGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_IMAGE
		Dump (zero);
		Dump (width);
		Dump (height);
		Dump (widthBytes);
		Dump (numPlanes);
		Dump (bitsPerPixel);
		Dump (zero2);
	#endif

		return true;
	}

	bool BitmapHeader::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n>>>> BitmapHeader::writeToDevice <<<<\n");
	#endif

		if (!BitmapHeaderGenerated::writeToDevice ())
			return false;

		return true;
	}


	WMFHeader::WMFHeader ()
	{
	}

	WMFHeader::~WMFHeader ()
	{
	}

	WMFHeader &WMFHeader::operator= (const WMFHeader &rhs)
	{
		if (this == &rhs)
			return *this;

		WMFHeaderGenerated::operator= (rhs);

		return *this;
	}

	bool WMFHeader::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n<<<< WMFHeader::readFromDevice >>>>\n");
	#endif

		if (!WMFHeaderGenerated::readFromDevice ())
			return false;

	#ifdef DEBUG_IMAGE
		Dump (fieldType);
		Dump (headerSize);
		Dump (winVersion);
		Dump (fileSize);
		Dump (numObjects);
		Dump (maxRecordSize);
		Dump (zero);
	#endif

		return true;
	}

	bool WMFHeader::writeToDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_IMAGE
		m_device->debug ("\n>>>> WMFHeader::writeToDevice <<<<\n");
	#endif

	#ifdef DEBUG_IMAGE
		Dump (fieldType);
		Dump (headerSize);
		Dump (winVersion);
		Dump (fileSize);
		Dump (numObjects);
		Dump (maxRecordSize);
		Dump (zero);
	#endif

		if (!WMFHeaderGenerated::writeToDevice ())
			return false;

		return true;
	}
}	// namespace MSWrite	{

// end of structures_private.cpp
