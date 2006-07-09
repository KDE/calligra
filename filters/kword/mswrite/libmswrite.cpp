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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "libmswrite.h"

namespace MSWrite
{
	FormatInfo::FormatInfo () : m_nextChar (0),
											m_leftMargin (0xFFFF), m_rightMargin (0xFFFF),
											m_fontTable (NULL)
	{
		// m_type =
	}

	FormatInfo::~FormatInfo ()
	{
	}

	bool FormatInfo::readFromDevice (void)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FORMATINFO
		if (m_type == ParaType)
			m_device->debug ("\n<<<< libmswrite.cpp::FormatInfo::readFromDevice (ParaType) >>>>\n");
		else	// if (m_type == CharType)
			m_device->debug ("\n<<<< libmswrite.cpp::FormatInfo::readFromDevice (CharType) >>>>\n");
	#endif

		int formatInfoPageNum;
		if (m_type == ParaType)
			formatInfoPageNum = m_header->getNumPageParaInfo ();
		else	// if (m_type == CharType)
			formatInfoPageNum = m_header->getNumPageCharInfo ();


	#ifdef DEBUG_FORMATINFO
		m_device->debug ("numpages format info=", formatInfoPageNum);
	#endif

		// you have to have information pages that cover the _entire_ document (if it's not empty that is)
		if (m_header->getNumCharBytes () && formatInfoPageNum == 0)
		{
			if (m_type == ParaType)
			{
				ErrorAndQuit (Error::InvalidFormat, "no paragraph formatting information page\n");
			}
			else	// if (m_type == CharType)
			{
				ErrorAndQuit (Error::InvalidFormat, "no character formatting information page\n");
			}
		}

		// seek to start of info pages
		if (!m_device->seek (((m_type == ParaType) ?
										m_header->getPageParaInfo () : m_header->getPageCharInfo ()) * 128, SEEK_SET))
			return false;

		//
		// read in every info page
		// (this eats up all your memory...)
		//
		// The reason this is done is because infoPages (formatting information) are required
		// in between reads of text and on most devices, continually seeking back and forth
		// between the text pages and information pages is inconvenient and inefficient to say
		// the least.
		//
		// Generally speaking there is more text than formatting information some it is probably
		// cheaper (memory-wise) to cache the formatting in memory, rather than the text.
		//
		// A few more good reasons for caching formatting information:
		//
		// * we don't actually expand the formatting pages into the full FormatPointers
		//   and FormatProperty's (until begin()/next() are called) so we save quite a bit of
		//   memory
		// * some filters need to know information like the number of objects in advance
		//   so an extra parse of the formatting information (already cached :)) is required
		//
		for (int i = 0; i < formatInfoPageNum; i++)
		{
			if (!m_formatInfoPageList.addToBack ())
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for an information page\n");

			FormatInfoPage &fip = *m_formatInfoPageList.begin (false);
			if (m_device->bad ()) return false;

			fip.setHeader (m_header);
			fip.setDevice (m_device);
			fip.setType (m_type);
			
			if (m_type == ParaType)
				fip.setMargins (m_leftMargin, m_rightMargin);
			else	// if (m_type == CharType)
				fip.setFontTable (m_fontTable);

			if (!fip.readFromDevice ())
				return false;
		}

		return true;
	}

	bool FormatInfo::writeToDevice (const void *defaultProperty)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FORMATINFO
		if (m_type == ParaType)
			m_device->debug ("\n<<<< libmswrite.cpp::FormatInfo::writeToDevice (ParaType) >>>>\n");
		else	// if (m_type == CharType)
			m_device->debug ("\n<<<< libmswrite.cpp::FormatInfo::writeToDevice (CharType) >>>>\n");
	#endif

		// set page number of formatting info in header
		if (m_type == ParaType)
			m_header->setPageParaInfo (m_device->tellInternal () / 128);
		else	// if (m_type == CharType)
		{
			// ms programmers were so space conservative that they didn't
			// store charInfo page
			//m_header->setPageCharInfo (m_device->tellInternal () / 128);
		}

		//
		// No formatting table?
		//
		// Mimick what Write does and make one.
		// It seems that only a fontTable is mandatory but this is conveniently
		// updated when adding to a character formatting page.
		//
		if (!m_formatInfoPageList.getNumElements ())
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\tno formatting information pages, creating one\n");
		#endif
		
			// --- but we should only get in here if numCharBytes = 0 ---
			
			if (m_header->getNumCharBytes ())
			{
				if (m_type == ParaType)
					m_device->error (Error::Warn, "data but no paragraph formatting info\n");
				else	// if (m_type == CharType)
					m_device->error (Error::Warn, "data but no character formatting info\n");
			}
	
			long currentOffset = m_device->tellInternal ();
				if (!m_device->seekInternal (128 + m_header->getNumCharBytes (), SEEK_SET)) return false;
				if (!add (defaultProperty, true/* force */)) return false;
			if (!m_device->seekInternal (currentOffset, SEEK_SET)) return false;
		}

		List <FormatInfoPage>::Iterator it;
		for (it = m_formatInfoPageList.begin (); it != m_formatInfoPageList.end (); ++it)
		{
			FormatInfoPage &fip = *it;

			// OPT: should already have done in add but we have to handle the impexp case
			fip.setHeader (m_header);
			fip.setDevice (m_device);
			fip.setType (m_type);
			
			if (m_type == ParaType)
				fip.setMargins (m_leftMargin, m_rightMargin);
			else	// if (m_type == CharType)
				fip.setFontTable (m_fontTable);

			if (!fip.writeToDevice ())
				return false;
		}

		return true;
	}

	void *FormatInfo::begin (void)
	{
		m_nextChar = 0;

		m_formatInfoPageIterator = m_formatInfoPageList.begin ();

		if (m_formatInfoPageIterator == m_formatInfoPageList.end ())
			return NULL;

		void *ret = (*m_formatInfoPageIterator).begin ();

		if (ret)
		{
			if (m_type == ParaType)
				m_nextChar = ((FormatParaProperty *) ret)->getAfterEndCharByte ();
			else	// if (m_type == CharType)
				m_nextChar = ((FormatCharProperty *) ret)->getAfterEndCharByte ();
		}

		return ret;
	}

	void *FormatInfo::next (void)
	{
	#ifdef CHECK_INTERNAL
		if (!m_device)
		{
			CHECK_DEVICE_ERROR;
			return NULL;
		}
	#endif

		void *ret = NULL;
		if ((*m_formatInfoPageIterator).end ())
		{
			m_formatInfoPageIterator++;

			// out of pages?
			if (m_formatInfoPageIterator == m_formatInfoPageList.end ())
				return NULL;

			if ((*m_formatInfoPageIterator).getFirstCharByte () != m_nextChar)
				m_device->error (Error::Warn, "FormatInfoPage::firstCharByte does not flow on from nextChar\n");

			ret = (*m_formatInfoPageIterator).begin ();
		}

		if (!ret)
			ret = (*m_formatInfoPageIterator).next ();

		if (ret)
		{
			if (m_type == ParaType)
				m_nextChar = ((FormatParaProperty *) ret)->getAfterEndCharByte ();
			else	// if (m_type == CharType)
				m_nextChar = ((FormatCharProperty *) ret)->getAfterEndCharByte ();
		}

		return ret;
	}

	bool FormatInfo::end (void) /*const*/
	{
		return m_formatInfoPageIterator == m_formatInfoPageList.end ();
	}

	bool FormatInfo::add (const void *property, const bool force)
	{
	CHECK_DEVICE;

	#ifdef DEBUG_FORMATINFO
		m_device->debug (">>>> FormatInfo::add <<<<\n");
	#endif
	
		DWord currentChar = m_device->tellInternal () - 128;
		
		// so that export filter writers can be lazy...
		if (m_nextChar == currentChar && !force)
		{
		#ifdef DEBUG_FORMATINFO
			m_device->debug ("\tEmpty FormatProperty, ignoring\n");
		#endif
			return true;
		}

		bool needToAllocate = false;

		if (m_formatInfoPageList.getNumElements ())
		{
			FormatInfoPage &fip = *m_formatInfoPageList.begin (false);
			if (!fip.add (property))
			{
				// a real error
				if (m_device->bad ())
					return false;

				needToAllocate = true;
			}
		}
		else
			needToAllocate = true;

		if (needToAllocate)
		{
			#ifdef DEBUG_FORMATINFO
				m_device->debug ("\tneedToAllocate=yes, FormatInfoPage::firstCharByte=", m_nextChar);
			#endif
				
			if (!m_formatInfoPageList.addToBack ())
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for new formatInfoPage\n");
			
			FormatInfoPage &fip = *m_formatInfoPageList.begin (false);
			fip.setDevice (m_device);
			fip.setFirstCharByte (m_nextChar);
			fip.setType (m_type);
			
			if (m_type == ParaType)
				fip.setMargins (m_leftMargin, m_rightMargin);
			else	// if (m_type == CharType)
			{
				assert (m_fontTable);
				fip.setFontTable (m_fontTable);
			}
			
			if (!fip.add (property))
				return false;
		}

		m_nextChar = currentChar;
		return true;
	}


	Generator::Generator ()
	{
	}

	Generator::~Generator ()
	{
	}

	bool Generator::processText (const Byte *string, bool willReachEndOfParagraph)
	{
		//
		// Look out for characters in the string and emit signals as appropriate:
		//
		//	1	pageNumber
		//	10	newLine
		//	13	carriageReturn
		//	12	pageBreak
		//	31	optionalHyphen
		//	?	text
		//

		const int outBufferMaxLen = 1024;
		Byte outBuffer [outBufferMaxLen];
		DWord outBufferLen = 0;

		for (; *string; string++)
		{
			// buffer full?
			if (outBufferLen >= outBufferMaxLen - 1)
			{
				// flush
				outBuffer [outBufferMaxLen - 1] = '\0';
				if (!writeText (outBuffer)) return false;
				outBufferLen = 0;
			}
		
			switch (*string)
			{
			// write text, generate signals for special characters, write more text...
			case 1:		// pageNumber anchor
			case 12:		// pageBreak (some silly document might have this in the middle of a paragraph!)
			case 13:
			case 10:		// newLine (some silly document _does_ have a newline in the middle of a paragraph!)
			case 31:    // optionalHyphen (aka "soft hyphen" an invisible hyphen, unless at end of line)
				// output text before this character
				if (outBufferLen)
				{
					outBuffer [outBufferLen] = 0;	// null terminate
					if (!writeText (outBuffer)) return false;
					outBufferLen = 0;
				}

				// generate signal
				switch (*string)
				{
				case 1:	if (!writePageNumber ()) return false;	break;
				case 12:	if (!writePageBreak ()) return false;	break;
				case 10:	if (!writeNewLine (willReachEndOfParagraph && string [1] == 0)) return false;	break;
				case 13:	if (!writeCarriageReturn ()) return false;	break;
				case 31: if (!writeOptionalHyphen ()) return false; break;
				}

				break;

			// normal text character
			default:
				outBuffer [outBufferLen++] = *string;
				break;
			}	// switch (*string)	{
		}	// for (; *string; string++)	{

		// flush
		if (outBufferLen)
		{
			outBuffer [outBufferLen] = 0;
			if (!writeText (outBuffer)) return false;
		}

		return true;
	}


	InternalParser::InternalParser () : m_header (NULL),
													m_sectionTable (NULL),
													m_pageLayout (NULL),
													m_pageTable (NULL),
													m_fontTable (NULL),
													m_paragraphInfo (NULL),
													m_characterInfo (NULL),
													m_image (NULL),
													m_ole (NULL)
	{
	}

	InternalParser::~InternalParser ()
	{
		delete m_ole;
		delete m_image;

		delete m_characterInfo;
		delete m_paragraphInfo;
		delete m_fontTable;
		delete m_pageTable;
		delete m_pageLayout;
		delete m_sectionTable;
		delete m_header;
	}

	// use this parser for "import" filters
	bool InternalParser::parse (void)
	{
		if (!m_device)
		{
			fprintf (stderr, "INTERNAL ERROR: InternalParser::parse() called without a device\n");
			return false;	// cannot use ErrorAndQuit() because that calls m_device->error()
		}

		if (!m_generator)
			ErrorAndQuit (Error::InternalError, "generator not passed to parser\n");

		//
		// allocate memory
		//

		m_header = new Header;
		if (!m_header)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for header\n");

		m_sectionTable = new SectionTable;
		if (!m_sectionTable)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for sectionTable\n");

		m_pageLayout = new PageLayout;
		if (!m_pageLayout)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for pageLayout\n");

		m_pageTable = new PageTable;
		if (!m_pageTable)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for pageTable\n");

		m_fontTable = new FontTable;
		if (!m_fontTable)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for fontTable\n");

		m_paragraphInfo = new FormatInfo;
		if (!m_paragraphInfo)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for paragraphInfo\n");

		m_characterInfo = new FormatInfo;
		if (!m_characterInfo)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for charInfo\n");

		//
		// Read everything from .WRI file
		//

		m_header->setDevice (m_device);
		if (!m_header->readFromDevice ()) return false;
		DWord numCharBytes = m_header->getNumCharBytes ();

		m_sectionTable->setHeader (m_header);
		m_sectionTable->setDevice (m_device);
		if (!m_sectionTable->readFromDevice ())
			return false;

		m_pageLayout->setHeader (m_header);
		m_pageLayout->setDevice (m_device);
		if (!m_pageLayout->readFromDevice ())
			return false;

		m_pageTable->setPageNumberStart (m_pageLayout->getPageNumberStart ());
		m_pageTable->setHeader (m_header);
		m_pageTable->setDevice (m_device);
		if (!m_pageTable->readFromDevice ())
			return false;

		m_fontTable->setHeader (m_header);
		m_fontTable->setDevice (m_device);
		if (!m_fontTable->readFromDevice ())
			return false;

		m_paragraphInfo->setHeader (m_header);
		m_paragraphInfo->setDevice (m_device);
		m_paragraphInfo->setType (ParaType);
		m_paragraphInfo->setMargins (m_pageLayout->getLeftMargin (), m_pageLayout->getRightMargin ());
		if (!m_paragraphInfo->readFromDevice ())
			return false;

		m_characterInfo->setHeader (m_header);
		m_characterInfo->setDevice (m_device);
		m_characterInfo->setType (CharType);
		m_characterInfo->setFontTable (m_fontTable);
		if (!m_characterInfo->readFromDevice ())
			return false;

#if 0
		DocumentInfo documentInfo;
		FormatParaProperty *paraProperty = (FormatParaProperty *) m_paragraphInfo->begin ();
		int numObjects = 0;
		while (paraProperty)
		{
			if (paraProperty->getIsObject ())
				numObjects++;
			if (paraProperty->getIsHeader ())
			{
				documentInfo.setHasHeader (true);
				if (paraProperty->getIsOnFirstPage ())
					documentInfo.setHasHeaderOnFirstPage (true);
			}
			if (paraProperty->getIsFooter ())
			{
				documentInfo.setHasFooter (true);
				if (paraProperty->getIsOnFirstPage ())
					documentInfo.setHasFooterOnFirstPage (true);
			}
			paraProperty = (FormatParaProperty *) m_paragraphInfo->next ();
		}
		documentInfo.setNumObjects (numObjects);
#endif

		// Get Ready!
		//

		enum Section { InNothing, InBody, InHeader, InFooter } inWhat = InNothing;

		// must call writeBody*() even if document doesn't have one
		bool wroteBody = false;

		PagePointer *pp = m_pageTable->begin ();
		if (m_device->bad ()) return false;

		//
		// Signal callbacks
		//
		#ifdef DEBUG_INTERNALPARSER
			m_device->debug ("@@@ InternalParser: start of document write\n");
		#endif

		if (!m_generator->writeDocumentBegin (m_header->getMagic (), m_pageLayout)) return false;
		m_generator->sigProgress (0);

		// start of text
		if (!m_device->seekInternal (1 * 128, SEEK_SET))
			return false;

		FormatParaProperty *paraProp = (FormatParaProperty *) m_paragraphInfo->begin ();
		if (m_device->bad ()) return false;
		FormatCharProperty *charProp = (FormatCharProperty *) m_characterInfo->begin ();
		if (m_device->bad ()) return false;
		DWord paraStartByte = 0;
		
		if (numCharBytes) while (paraProp)	// loop if not empty document
		{
		#ifdef DEBUG_INTERNALPARSER
			m_device->debug ("@@@ InternalParser: Start of loop - section write\n");
		#endif
			//
			// Section work
			//

			enum Section inWhatNext = InNothing;
			if (paraProp->getIsFooter ())
				inWhatNext = InFooter;
			else if (paraProp->getIsHeader ())
				inWhatNext = InHeader;
			else
				inWhatNext = InBody;

			// beginning of a new section?
			if (inWhatNext != inWhat)
			{
				// end last thing we were in
				switch (inWhat)
				{
				case InFooter:	if (!m_generator->writeFooterEnd ()) return false; else break;
				case InHeader:	if (!m_generator->writeHeaderEnd ()) return false; else break;
				case InBody:	if (!m_generator->writeBodyEnd ()) return false; else break;
				default:	break;	// keep compiler happy
				}

				// start next section
				switch (inWhat = inWhatNext)
				{
				case InFooter:	if (!m_generator->writeFooterBegin ()) return false; else break;
				case InHeader:	if (!m_generator->writeHeaderBegin ()) return false; else break;
				case InBody:	if (!m_generator->writeBodyBegin ()) return false;
					// if there's not pageTable, manually signal start of page for the first and last time
					if (!pp)
						if (!m_generator->writePageNew ())
							return false;

					wroteBody = true;
				default:	break;	// keep compiler happy
				}
			}


			//
			// start paragraph
			//

			bool paraIsText = paraProp->getIsText ();
			int objectType = ObjectType::NotObject;

			DWord paraEndByte, paraAfterEndByte;
			paraAfterEndByte = paraProp->getAfterEndCharByte ();
			paraEndByte = paraAfterEndByte - 1;

			if (paraIsText)
			{
			#ifdef DEBUG_INTERNALPARSER
				m_device->debug ("@@@ InternalParser: Start of paragraph write\n");
			#endif

				// signal paragraph
				if (!m_generator->writeParaInfoBegin (paraProp, NULL, NULL)) return false;
			}
			else
			{
				//
				// Determine whether the object is OLE or not
				// Yes, I know this isn't entirely clean but
				// name a cleaner and more efficient way of doing this...
				//

				Byte data [2];
				Word mappingMode;

				if (!m_device->readInternal (data, 2)) return false;
				ReadWord (mappingMode, data);
				if (!m_device->seekInternal (-2, SEEK_CUR)) return false;	// ungetc()x2

				switch (mappingMode)
				{
				case 0xE4:
					objectType = ObjectType::OLE;

					m_ole = new OLE;
					if (!m_ole)
						ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for OLE object\n");

					m_ole->setDevice (m_device);
					if (!m_ole->readFromDevice ())
						return false;

					if (!m_generator->writeParaInfoBegin (paraProp, m_ole, NULL)) return false;
					break;
				case 0xE3:	// monochrome bitmap?
					objectType = ObjectType::BMP;

					m_image = new Image;
					if (!m_image)
						ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for BMP image object\n");

					m_image->setDevice (m_device);
					if (!m_image->readFromDevice ())
						return false;

					if (!m_generator->writeParaInfoBegin (paraProp, NULL, m_image)) return false;
					break;
				default:
					objectType = ObjectType::WMF;

					m_image = new Image;
					if (!m_image)
						ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for WMF image object\n");

					m_image->setDevice (m_device);
					if (!m_image->readFromDevice ())
						return false;

					if (!m_generator->writeParaInfoBegin (paraProp, NULL, m_image)) return false;
					break;
				}

				// image: BMP/WMF
				if (objectType != ObjectType::OLE)
				{
					if (m_image->getNumHeaderBytes () + m_image->getNumDataBytes ()
							!= paraAfterEndByte - paraStartByte)
					{
						m_device->error (Error::Warn, "imageHeader: numHeaderBytes + numDataBytes != paragraph length\n");

						// we had better seek to where the paragraphs expect us to be to avoid trouble
						if (!m_device->seekInternal (paraAfterEndByte + 128, SEEK_SET)) return false;
					}
				}
			}

		#ifdef DEBUG_INTERNALPARSER
			m_device->debug ("@@@ InternalParser: Start of text write\n");
		#endif

			DWord uptoByte = paraStartByte;

			// loop until we hit end of paragraph (getting one CharProperty on every iteration)
			while (uptoByte < paraAfterEndByte)
			{
				if (charProp)
				{
					if (uptoByte >= charProp->getAfterEndCharByte ())
					{
						charProp = (FormatCharProperty *) m_characterInfo->next ();
						if (m_device->bad ()) return false;
					}

				#ifdef DEBUG_INTERNALPARSER
					m_device->debug ("@@@ InternalParser: character write\n");
				#endif

					if (!m_generator->writeCharInfoBegin (charProp)) return false;
				}

				// ultimately aim for end of CharProperty block; if that's not possible, aim for end of paragraph
				DWord aimUltimateByte = charProp->getEndCharByte () > paraEndByte ? paraEndByte : charProp->getEndCharByte ();

				while (uptoByte <= aimUltimateByte)
				{
					// flag to tell us to generate pageNewWrite/pageTable signal, after writing some normal text
					bool pageTableAck = false;

					// short-term goal (before end of CharProperty, ParaProperty or page)
					DWord aimByte = aimUltimateByte;

					// we want to efficiently send pageNewWrite signals which might be sandwhiched inside
					// a CharProperty block (note: we do NOT end and then restart a CharProperty block because of a writePageNew())
					if (pp)
					{
						if (pp->getFirstCharByte () <= aimByte)
						{
							pageTableAck = true;
							if (pp->getFirstCharByte ())
								// - 1 is because we want the PageNew signal called before the character
								aimByte = pp->getFirstCharByte () - 1;
							else
								aimByte = 0xFFFFFFFF;	// 0 - 1 :)

						#ifdef DEBUG_INTERNALPARSER
							m_device->debug ("@@@ InternalParser: writePageNew pending\n");
						#endif
						}
					}

					// write text (using buffering)
					while (uptoByte <= aimByte && aimByte != 0xFFFFFFFF)
					{
						const DWord amountToRead = aimByte - uptoByte + 1 > 1023
															? 1023 : aimByte - uptoByte + 1;

						if (paraIsText)
						{
							Byte buffer [1024];

							if (!m_device->readInternal (buffer, amountToRead)) return false;

							buffer [amountToRead] = '\0';
							if (!m_generator->processText (buffer, uptoByte + amountToRead - 1 == paraEndByte)) return false;
						}

						uptoByte += amountToRead;
					}		// while (uptoByte <= aimByte && aimByte != 0xFFFFFFFF) {

					// generate pageNewWrite/pageTable signal, if requested
					if (pageTableAck)
					{
					#ifdef DEBUG_INTERNALPARSER
						m_device->debug ("@@@ InternalParser: writePageNew\n");
					#endif

						if (!m_generator->writePageNew (pp->getPageNumber ())) return false;

						pp = m_pageTable->next ();
						if (m_device->bad ()) return false;
					}
				}		// while (uptoByte <= aimUltimateByte) {


				#ifdef DEBUG_INTERNALPARSER
					m_device->debug ("@@@ InternalParser: character end write\n");
				#endif

				// end char info
				if (charProp)
					if (!m_generator->writeCharInfoEnd (charProp, uptoByte == paraAfterEndByte))
						return false;

			}		// while (uptoByte < paraAfterEndByte) {


			//
			// ouptut object
			//

			if (!paraIsText)
			{
				if (objectType == ObjectType::OLE)
				{
					if (!m_generator->writeBinary (m_ole->getExternalObject (), m_ole->getExternalObjectSize ()))
						return false;
				}
				else	// if (objectType == ObjectType::BMP || objectType == ObjectType::WMF)
				{
					if (!m_generator->writeBinary (m_image->getExternalImage (), m_image->getExternalImageSize ()))
						return false;
				}
			}

			//
			// end paragraph
			//

			if (paraIsText)
			{
				// end paragraph
				if (!m_generator->writeParaInfoEnd (paraProp, NULL)) return false;
			}
			else
			{
				switch (objectType)
				{
				case ObjectType::OLE:
					if (!m_generator->writeParaInfoEnd (paraProp, m_ole, NULL)) return false;
					delete m_ole;
					m_ole = NULL;
					break;
				case ObjectType::BMP:
					if (!m_generator->writeParaInfoEnd (paraProp, NULL, m_image)) return false;
					delete m_image;
					m_image = NULL;
					break;
				case ObjectType::WMF:
					if (!m_generator->writeParaInfoEnd (paraProp, NULL, m_image)) return false;
					delete m_image;
					m_image = NULL;
					break;
				}
			}

			paraStartByte = paraAfterEndByte;

			// numCharBytes != 0 because we checked it before we entered the loop
			m_generator->sigProgress (paraStartByte * 100 / numCharBytes);

			// get next paragraph properties
			paraProp = (FormatParaProperty *) m_paragraphInfo->next ();
			if (m_device->bad ()) return false;
		}

		// end last thing we were in
		switch (inWhat)
		{
		case InFooter:	if (!m_generator->writeFooterEnd ()) return false; else break;
		case InHeader:	if (!m_generator->writeHeaderEnd ()) return false; else break;
		case InBody:	if (!m_generator->writeBodyEnd ()) return false; else break;
		default:	break;	// keep compiler happy
		}

		// didn't output a body (usually due to a blank document)
		if (!wroteBody)
		{
		#ifdef DEBUG_INTERNALPARSER
			m_device->debug ("@@@ InternalParser: did not write body, writing one now\n");
		#endif
		
			if (!m_generator->writeBodyBegin ()) return false;
			if (!m_generator->writeBodyEnd ()) return false;
		}

		m_generator->sigProgress (100);
		if (!m_generator->writeDocumentEnd (m_header->getMagic (), m_pageLayout)) return false;

		return true;
	}


	InternalGenerator::InternalGenerator () : m_header (NULL),
															m_sectionTable (NULL),
															m_pageLayout (NULL),
															m_pageTable (NULL),
															m_fontTable (NULL),
															m_paragraphInfo (NULL),
															m_characterInfo (NULL),
															m_image (NULL),
															m_ole (NULL)
	{
	}

	InternalGenerator::~InternalGenerator ()
	{
		delete m_ole;
		delete m_image;

		delete m_characterInfo;
		delete m_paragraphInfo;
		delete m_fontTable;
		delete m_pageTable;
		delete m_pageLayout;
		delete m_sectionTable;
		delete m_header;
	}

	bool InternalGenerator::seekNextPage (void)
	{
		return m_device->seekInternal ((m_device->tellInternal () + 127) / 128 * 128, SEEK_SET);
	}

	bool InternalGenerator::writeDocumentBegin (const Word, const PageLayout *pageLayout)
	{
		if (!m_device)
		{
			fprintf (stderr, "INTERNAL ERROR: InternalGenerator::writeDocumentBegin() called without a device\n");
			return false;	// cannot use ErrorAndQuit() because that calls m_device->error()
		}

		m_header = new Header;
		if (!m_header)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for header\n");
		m_header->setDevice (m_device);

		m_pageLayout = new PageLayout;
		if (!m_pageLayout)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for pageLayout\n");
		m_pageLayout->setDevice (m_device);

		m_sectionTable = new SectionTable;
		if (!m_sectionTable)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for sectionTable\n");
		m_sectionTable->setDevice (m_device);

		m_pageTable = new PageTable;
		if (!m_pageTable)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for pageTable\n");
		m_pageTable->setDevice (m_device);

		m_fontTable = new FontTable;
		if (!m_fontTable)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for fontTable\n");
		m_fontTable->setDevice (m_device);

		m_paragraphInfo = new FormatInfo;
		if (!m_paragraphInfo)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for paragraphInfo\n");
		m_paragraphInfo->setType (ParaType);
		m_paragraphInfo->setDevice (m_device);

		m_characterInfo = new FormatInfo;
		if (!m_characterInfo)
			ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for characterInfo\n");
		m_characterInfo->setType (CharType);
		m_characterInfo->setDevice (m_device);
		
		*m_pageLayout = *pageLayout;
		
		// get ready to output text
		if (!m_device->seekInternal (1 * 128, SEEK_SET)) return false;

		return true;
	}

	bool InternalGenerator::writeDocumentEnd (const Word format, const PageLayout *)
	{
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: writeDocumentEnd\n");
	#endif

		m_header->setNumCharBytes (m_device->tellInternal () - 128);

	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("before charinfo seek next page: ", m_device->tellInternal ());
	#endif
		if (!seekNextPage ()) return false;	// from text pages
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: Write characterInfo; page=", m_device->tellInternal () / 128);
	#endif
		FormatCharProperty defaultCharProperty;
		defaultCharProperty.setFontTable (m_fontTable);	
			Font defaultFont ((const Byte *) "Arial");	// better than "unknown", I guess
		defaultCharProperty.setFont (&defaultFont);
			
		// write characterInfo pages
		m_characterInfo->setDevice (m_device);
		m_characterInfo->setHeader (m_header);
		m_characterInfo->setFontTable (m_fontTable);
		if (!m_characterInfo->writeToDevice (&defaultCharProperty)) return false;

	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("before parainfo seek next page: ", m_device->tellInternal ());
	#endif
		if (!seekNextPage ()) return false;
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: Write paragraphInfo; page=", m_device->tellInternal () / 128);
	#endif
		FormatParaProperty defaultParaProperty;	
		defaultParaProperty.setMargins (m_pageLayout->getLeftMargin (), m_pageLayout->getRightMargin ());
	
		// write paragraphInfo pages
		m_paragraphInfo->setDevice (m_device);
		m_paragraphInfo->setHeader (m_header);
		m_paragraphInfo->setMargins (m_pageLayout->getLeftMargin (), m_pageLayout->getRightMargin ());
		if (!m_paragraphInfo->writeToDevice (&defaultParaProperty)) return false;

	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: Write footnoteTable\n");
	#endif

		// there's "no such thing (tm)" as a FootnoteTable
		m_header->setPageFootnoteTable ((m_device->tellInternal () + 127) / 128);

		if (!seekNextPage ()) return false;
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: Write pageLayout; page=", m_device->tellInternal () / 128);
	#endif
		m_pageLayout->setDevice (m_device);
		m_pageLayout->setHeader (m_header);
		if (!m_pageLayout->writeToDevice ()) return false;

	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: Write sectionTable\n");
	#endif
		if (!seekNextPage ()) return false;
		m_sectionTable->setDevice (m_device);
		m_sectionTable->setHeader (m_header);
		if (!m_sectionTable->writeToDevice (m_pageLayout->getIsModified ())) return false;

	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: Write pageTable\n");
	#endif
		if (!seekNextPage ()) return false;
		m_pageTable->setDevice (m_device);
		m_pageTable->setHeader (m_header);
		m_pageTable->setPageNumberStart (m_pageLayout->getPageNumberStart ());
		if (!m_pageTable->writeToDevice ()) return false;

	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: Write fontTable\n");
	#endif
		if (!seekNextPage ()) return false;
		m_fontTable->setDevice (m_device);
		m_fontTable->setHeader (m_header);
		if (!m_fontTable->writeToDevice ()) return false;

	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: Write header\n");
	#endif
		// write header
		m_header->setFormat (format);
		m_header->setNumPages ((m_device->tellInternal () + 127) / 128);
		m_header->setDevice (m_device);
		if (!m_device->seekInternal (0 * 128, SEEK_SET)) return false;
		if (!m_header->writeToDevice ()) return false;

		// pad up to 128 (it seems that the ms programmers really did expect 128-byte pages)
		// BTW, if you look in a Write file, you can normally see some garbage after the
		// fontTable that appears to be part of an earlier part of the document
		// (or even some _other_ document!) suggesting that ms really did reuse their memory
		// and that we shouldn't be all that concerned with "unknown", "reserved" and "zero"
		// fields
		if (!m_device->seekInternal (m_header->getNumPages () * 128, SEEK_SET)) return false;

		return true;
	}

	bool InternalGenerator::writeParaInfoBegin (const FormatParaProperty * /*paraProperty*/,
																const OLE *ole,
																const Image *image)
	{
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: writeParaInfoBegin\n");
	#endif

		if (ole)
		{
			m_ole = new OLE;
			if (!m_ole)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for OLE\n");
			*m_ole = *ole;

			m_ole->setDevice (m_device);
			m_ole->setExternalObjectSize (ole->getExternalObjectSize ());
		}

		if (image)
		{
			m_image = new Image;
			if (!m_image)
				ErrorAndQuit (Error::OutOfMemory, "could not allocate memory for image\n");
			*m_image = *image;

			m_image->setDevice (m_device);
			m_image->setExternalImageSize (image->getExternalImageSize ());
		}

		return true;
	}

	bool InternalGenerator::writeParaInfoEnd (const FormatParaProperty *paraProperty,
															const OLE */*ole*/,
															const Image */*image*/)
	{
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: writeParaInfoEnd\n");
	#endif

		if (m_ole)
		{
			if (!m_ole->writeToDevice ())
				return false;

			delete m_ole;
			m_ole = NULL;
		}
		else if (m_image)
		{
			if (!m_image->writeToDevice ())
				return false;

			delete m_image;
			m_image = NULL;
		}

		m_paragraphInfo->setMargins (m_pageLayout->getLeftMargin (), m_pageLayout->getRightMargin ());
		return m_paragraphInfo->add (paraProperty, false/* don't force */);
	}

	bool InternalGenerator::writeCharInfoBegin (const FormatCharProperty * /*charProperty*/)
	{
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: writeCharInfoBegin\n");
	#endif
		
		return true;
	}

	bool InternalGenerator::writeCharInfoEnd (const FormatCharProperty *charProperty,
															const bool)
	{
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: writeCharInfoEnd\n");
	#endif

		m_characterInfo->setFontTable (m_fontTable);
		return m_characterInfo->add (charProperty, false/* don't force */);
	}

	bool InternalGenerator::writeBinary (const Byte *buffer, const DWord length)
	{
		if (m_ole)
		{
			if (!m_ole->setExternalObject (buffer, length))
				return false;
		}
		else if (m_image)
		{
			if (!m_image->setExternalImage (buffer, length))
				return false;
		}
		else
			ErrorAndQuit (Error::InternalError, "attempt to write unknown type of binary data\n");

		return true;
	}

	bool InternalGenerator::writeText (const Byte *string)
	{
	#ifdef DEBUG_INTERNALGENERATOR
//		m_device->debug ("!!!! InternalGenerator: writeText> ", (const char *) string);
	#endif

		DWord length = DWord (strlen ((const char *) string));

		if (!m_device->writeInternal (string, length)) return false;

		return true;
	}

	bool InternalGenerator::writePageNew (const int pageNumberClaimed)
	{
	#ifdef DEBUG_INTERNALGENERATOR
		m_device->debug ("!!!! InternalGenerator: writePageNew() with pageNo ",
								pageNumberClaimed);
	#endif

		// is this a forced new page (as in, the signal was only generated because I put it in the spec?)
		if (pageNumberClaimed == 0)
			return true;	// no real pageTable

		PagePointer pp;
		pp.setPageNumber (pageNumberClaimed);
		pp.setFirstCharByte (m_device->tellInternal () - 128);

		return m_pageTable->add (&pp);
	}

}	// namespace MSWrite	{

// end of libmswrite.cpp
