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

#ifndef __LIBMSWRITE_DEFS_H__
#define __LIBMSWRITE_DEFS_H__

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "config.libmswrite.h"
#include "list.h"

namespace MSWrite
{
	// library version
	extern const char *Version;


	/**
	 *
	 * Basic data types
	 * (should be correct for 16-bit, 32-bit and 64-bit systems)
	 *
	 */

	typedef unsigned char Byte;					// 1 byte
	typedef unsigned short Word;					// 2 bytes
	typedef signed short Short;					// 2 bytes

	// undefine for 16-bit (although the code is untested)
	#define ATLEAST32BIT

	#ifdef ATLEAST32BIT	// 32-bit or 64-bit system
		typedef unsigned int DWord;				// 4 bytes
		typedef signed int Long;					// 4 bytes
	#else
		typedef unsigned long DWord;				// 4 bytes
		typedef signed long Long;					//	4 Bytes
	#endif


	//
	// possible errors passed to Devices
	//
	class Error
	{
	public:
		static const int Ok = 0;
		static const int Warn = 1;
		static const int InvalidFormat = 2;
		static const int OutOfMemory = 3;
		static const int InternalError = 4;
		static const int Unsupported = 5;
		static const int FileError = 6;

		static const int LastOne = 255;
		// for your own Errors, only use values >= 256 (values < 256 are reserved)
	};


	/**
	 * Handy conversion functions for the following:
	 *
	 * Twip = 1/20 Points/Pixels
	 * Point = 1/72 inch
	 * Inch = 25.4 mm
	 *
	 * LibMSWrite will return all values (except font sizes) in twips for
	 * maximum accuracy.  It is your responsibility to convert the
	 * measurements to whatever you want.
	 *
	 */
	#define Twip2Point(val)	((val) / 20)
	#define Point2Twip(val)	((val) * 20)

	#define Twip2Inch(val)	((val) / 1440)
	#define Inch2Twip(val)	((val) * 1440)

	#define Point2Inch(val)	((val) / 72)
	#define Inch2Point(val)	((val) * 72)

	#define Twip2Milli(val) ((val) / 56.6929)
	#define Milli2Twip(val) ((val) * 56.6929)

	#define Point2Milli(val)	((val) / 2.83465)
	#define Milli2Point(val)	((val) * 2.83465)

	#define Inch2Mill(val)	((val) * 25.4)
	#define Milli2Inch(val)	((val) / 25.4)

	/**
	 *
	 * Portable functions for reading a basic data type from a Byte array
	 *
	 */
	inline Byte ReadByte (Byte &dest, const Byte *src)
	{
		return dest = *src;
	}

	inline Word ReadWord (Word &dest, const Byte *src)
	{
		return dest = ((Word) src [0]) |
							(((Word) src [1]) << 8);
	}

	inline Short ReadShort (Short &dest, const Byte *src)
	{
		return (Short) ReadWord ((Word &) dest, src);
	}

	inline DWord ReadDWord (DWord &dest, const Byte *src)
	{
		return dest = ((DWord) src [0]) |
							(((DWord) src [1]) << 8) |
							(((DWord) src [2]) << 16) |
							(((DWord) src [3]) << 24);
	}

	inline Long ReadLong (Long &dest, const Byte *src)
	{
		return (Long) ReadDWord ((DWord &) dest, src);
	}

	inline Byte WriteByte (const Byte &src, Byte *dest)
	{
		*dest = src;
		return src;
	}

	inline Word WriteWord (const Word &src, Byte *dest)
	{
		dest [0] = (Byte) (src & 255);
		dest [1] = (Byte) (src >> 8);
		return src;
	}

	inline Short WriteShort (const Short &src, Byte *dest)
	{
		WriteWord ((Word) src, dest);
		return src;
	}

	inline DWord WriteDWord (const DWord &src, Byte *dest)
	{
		dest [0] = (Byte) (src & 255);
		dest [1] = (Byte) ((src >> 8) & 255);
		dest [2] = (Byte) ((src >> 16) & 255);
		dest [3] = (Byte) (src >> 24);
		return src;
	}

	inline Long WriteLong (const Long &src, Byte *dest)
	{
		WriteDWord ((DWord) src, dest);
		return src;
	}

	/**
	 *
	 * Portable functions for reading/writing some bits from/to a Byte
	 *
	 */

	// optimisation freak, I know :)
	#define BitMask1 1
	#define BitMask2 3
	#define BitMask3 7
	#define BitMask4 15
	#define BitMask5 31
	#define BitMask6 63
	#define BitMask7 127
	#define BitMask8 255

	#define ReadBitsFromByte(destbits,srcbyte,start,len)	\
		((destbits)=((srcbyte)>>(start))&(BitMask##len))
	// NOTE: it is your fault if destbyte is not 0 to start with!
	#define WriteBitsToByte(srcbits,destbyte,start,len)	\
		((destbyte)|=((Byte(srcbits)&(BitMask##len))<<(start)))


	/**
	 *
	 * Device that either reads or writes from/to .WRI files
	 *
	 */
	class Device
	{
	private:
		static const int MaxCacheDepth = 32;
		long m_posInternal;
		Byte *m_cache [MaxCacheDepth];
		int m_cacheCount;

		static const int MaxDebugLen = 1024;
		char m_debugTemp [MaxDebugLen];

	protected:
		int m_error;

	public:
		Device () : m_posInternal (0), m_cacheCount (0), m_error (0)
		{
		}

		virtual ~Device ()
		{
		}

		
		//
		// Do _not_ try to override these functions!
		// They are used internally by LibMSWrite so that reads/writes can
		// be redirected from/to memory blocks.
		//


		/**
		 *
		 * setCache specifies whether or not Device should start reading from
		 * memory block specified by @param cache, instead of from user defined
		 * functions (which probably read from a file).
		 *
		 * setting @p cache to NULL stops the Device from reading from the
		 * last memory block specified.
		 *
		 */
		bool setCache (Byte *const cache)
		{
			if (cache)
			{
				m_cache [m_cacheCount] = cache;
				m_cacheCount++;
				if (m_cacheCount > MaxCacheDepth)
				{
					error (Error::InternalError, "too many caches\n");
					return false;
				}
			}
			else
			{
				--m_cacheCount;
				if (m_cacheCount < 0)
				{
					error (Error::InternalError, "too few caches\n");
					return false;
				}
			}

			return true;
		}

		bool readInternal (Byte *buf, const long numBytes)
		{
			bool ret;

			if (m_cacheCount)
			{
				memcpy (buf, m_cache [m_cacheCount - 1], numBytes);
				m_cache [m_cacheCount - 1] += numBytes;
				ret = true;
			}
			else
			{
				ret = read (buf, numBytes);
				if (ret) m_posInternal += numBytes;
			}

			return ret;
		}

		bool writeInternal (const Byte *buf, const long numBytes)
		{
			bool ret;
			
			if (m_cacheCount)
			{
				memcpy (m_cache [m_cacheCount - 1], buf, numBytes);
				m_cache [m_cacheCount - 1] += numBytes;
				ret = true;
			}
			else
			{
				ret = write (buf, numBytes);
				if (ret) m_posInternal += numBytes;
			}

			return ret;
		}

		bool seekInternal (const long offset, const int whence)
		{
			bool ret = seek (offset, whence);

			if (ret)
			{
				switch (whence)
				{
				case SEEK_SET:
					m_posInternal = offset;
					break;
				case SEEK_CUR:
					m_posInternal += offset;
					break;
				case SEEK_END:
					m_posInternal = tell ();
					break;
				}
			}

			return ret;
		}

		long tellInternal (void) const
		{
			// does not reflect the cache!
			return m_posInternal;
		}


		//
		// convenience functions
		//

		void debug (const char *s, const int i)
		{
			snprintf (m_debugTemp, MaxDebugLen - 1, "%s%i\n", s, i);
			m_debugTemp [MaxDebugLen - 1] = 0;
			debug (m_debugTemp);
		}
		void debug (const char *s1, const char *s2)
		{
			snprintf (m_debugTemp, MaxDebugLen - 1, "%s%s\n", s1, s2);
			m_debugTemp [MaxDebugLen - 1] = 0;
			debug (m_debugTemp);
		}

		void debug (const char *s1, const Byte *s2)
		{
			debug (s1, (const char *) s2);
		}

		#define Dump(name) m_device->debug("\t" #name ": ", m_##name)

		bool good (void) const
		{
			return m_error == 0;
		}

		int bad (void) const
		{
			return m_error;
		}


		/**
		 *
		 * Functions that the user must implement
		 *
		 * If the seek function is called to move past EOF (when writing),
		 * pad from EOF to the new location with 0's and then seek.
		 *
		 * It is your responsibility to call error() if a file op fails.
		 *
		 */
		virtual bool read (Byte *buf, const DWord numBytes) = 0;
		virtual bool write (const Byte *buf, const DWord numBytes) = 0;
		virtual bool seek (const long offset, const int whence) = 0;
		virtual long tell (void) = 0;
		virtual void debug (const char *s)
		{
			fprintf (stderr, "%s", s);
		}
		virtual void debug (const int i)
		{
			fprintf (stderr, "%i", i);
		}
		static const DWord NoToken = DWord (0xABCD1234);	// hopefully won't clash with any values
		virtual void error (const int errorCode, const char *message,
									const char *file = "", const int lineno = 0,
									DWord token = NoToken)
		{
			// errors do not really include warnings after all...
			if (errorCode != Error::Warn)
				m_error = errorCode;

			if (lineno)
				fprintf (stderr, "%s:%i:", file, lineno);
			
			// TODO: why the \n?
			if (token == NoToken)
				fprintf (stderr, "%s\n", message);
			else
				fprintf (stderr, "%s (val=%li)\n", message, long (token));
		}

		#define ErrorAndQuit(errorCode,message)		\
		{															\
			m_device->error(errorCode,message);			\
			return false;										\
		}

		#define Verify(errorCode,expr,token)	\
		((expr)? true : (m_device->error(errorCode, "check \'" #expr "\' failed",__FILE__,__LINE__,token),m_device->good ()))
	};


	/**
	 *
	 * Device capable only of reading/writing to memory blocks
	 * (used internally by LibMSWrite)
	 *
	 */

	class MemoryDevice : public Device
	{
	public:
		MemoryDevice ()
		{
		}

		virtual ~MemoryDevice ()
		{
		}

		bool read (Byte * /*buf*/, const DWord /*numBytes*/)
		{
			error (Error::InternalError, "memory device not reading from memory?\n");
			return false;
		}
		bool write (const Byte * /*buf*/, const DWord /*numBytes*/)
		{
			error (Error::InternalError, "memory device not writing to memory?\n");
			return false;
		}
		bool seek (const long /*offset*/, const int /*whence*/)
		{
			error (Error::InternalError, "memory device cannot seek full stop!\n");
			return false;
		}
		long tell (void)
		{
			error (Error::InternalError, "memory device not accessing memory?\n");
			return -1;
		}
	};

	class NeedsDevice
	{
	protected:
		Device *m_device;

	public:
		NeedsDevice (Device *device = NULL)
		{
			setDevice (device);
		}

		virtual ~NeedsDevice ()
		{
		}

		void setDevice (Device *const device)
		{
			m_device = device;
		}
	};

	#ifdef CHECK_INTERNAL
		#define CHECK_DEVICE_ERROR	fprintf (stderr, "%s:%i: INTERNAL ERROR - device not set\n", __FILE__, __LINE__)
		#define CHECK_DEVICE		\
		if (!m_device)				\
		{								\
			CHECK_DEVICE_ERROR;	\
			return false;			\
		}
	#else
		#define CHECK_DEVICE_ERROR
		#define CHECK_DEVICE
	#endif

	// [PRIVATE]
	class UseThisMuchPrefixSize
	{
	private:
		int m_val;

	public:
		UseThisMuchPrefixSize (const int val = 0)
		{
			setVal (val);
		}

		~UseThisMuchPrefixSize ()
		{
		}

		bool operator== (const UseThisMuchPrefixSize &rhs)
		{
			return m_val == rhs.m_val;
		}

		UseThisMuchPrefixSize &operator= (const UseThisMuchPrefixSize &rhs)
		{
			if (this == &rhs)
				return *this;

			m_val = rhs.m_val;
			return *this;
		}

		int getVal (void) const	{	return m_val;	}
		void setVal (const int val)	{	m_val = val;	}
	};
	
	// [PRIVATE]
	class UseThisMuch
	{
	private:
		List <UseThisMuchPrefixSize> m_notDefaultBits;
		
	protected:
		UseThisMuch &operator= (const UseThisMuch &rhs)
		{
			if (this == &rhs)
				return *this;

			this->m_notDefaultBits = rhs.m_notDefaultBits;
			return *this;
		}
		
		void signalHaveSetData (const bool isDefault, const int needNumBits)
		{
			if (isDefault)
			{
				// it's a delete operation then
				List <UseThisMuchPrefixSize>::Iterator it = m_notDefaultBits.search (needNumBits);
				if (it != m_notDefaultBits.end ()) m_notDefaultBits.erase (it);
			}
			// possibly a new value
			else
			{
				List <UseThisMuchPrefixSize>::Iterator it = m_notDefaultBits.search (needNumBits);

				// new value
				if (it == m_notDefaultBits.end ())
				{
					UseThisMuchPrefixSize utmps (needNumBits);
					m_notDefaultBits.addToBack (utmps);
				}
			}
		}

		// not bits
		int getNeedNumDataBytes (void) const
		{
			int biggest = 0;
			List <UseThisMuchPrefixSize>::Iterator it;
			for (it = m_notDefaultBits.begin (); it != m_notDefaultBits.end (); it++)
			{
				if ((*it).getVal () > biggest)
					biggest = (*it).getVal ();
			}

			if (biggest % 8)
				return biggest / 8 + 1;	// account for fractional byte
			else
				return biggest / 8;
		}

	public:
		UseThisMuch ()
		{
		}

		virtual ~UseThisMuch ()
		{
		}
	};
	
}	// namespace MSWrite	{

#endif	// __LIBMSWRITE_DEFS_H__

// end of libmswrite_defs.h
