/* 
 * EasySoap++ - A C++ library for SOAP (Simple Object Access Protocol)
 * Copyright (C) 2001 David Crowley; SciTegic, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include "SOAP.h"
#include "SOAPBase64.h"

static char base64encode[64];
static int  base64decode[256];
static char base64pad = '=';

static int
initializeBase64Tables()
{
	int i = 0;

	// initialize the encoding table
	for (i = 0; i < 26; ++i)
	{
		base64encode[i] = 'A' + i;
		base64encode[26 + i] = 'a' + i;
	}

	for (i = 0; i < 10; ++i)
		base64encode[52 + i] = '0' + i;

	base64encode[62] = '+';
	base64encode[63] = '/';


	// initialize the decoding table
	for (i = 0; i < 256; ++i)
		base64decode[i] = (char)0x80;
	for (i = 'A'; i <= 'Z'; ++i)
		base64decode[i] = i - 'A';
	for (i = 'a'; i <= 'z'; ++i)
		base64decode[i] = 26 + i - 'a';
	for (i = '0'; i <= '9'; ++i)
		base64decode[i] = 52 + i - '0';

	base64decode['+'] = 62;
	base64decode['/'] = 63;
	base64decode[base64pad] = 0;

	return 1;
}

static int initialized = initializeBase64Tables();


SOAPBase64::SOAPBase64()
{

}

inline int
nextChar(const char*& str)
{
	int c = 0;

	// skip over white space
	while (sp_isspace(*str))
		++str;

	// only increment pointer if
	// we're not at the end of string
	if ((c = *str))
		++str;

	return c;
}

void
SOAPBase64::Decode(const SOAPString& strx, SOAPArray<char>& array)
{
	array.Resize(0);
	bool done = false;
	const char *str = strx;

	while (!done)
	{
		int		in[4];
		char	out[3];
		int		valid = 3;

		in[0] = nextChar(str);
		in[1] = nextChar(str);
		in[2] = nextChar(str);
		in[3] = nextChar(str);

		if (in[0] == 0)
			break;

		if (in[2] == base64pad)
			valid = 1;
		else if (in[3] == base64pad)
			valid = 2;

		in[0] = base64decode[in[0]];
		in[1] = base64decode[in[1]];
		in[2] = base64decode[in[2]];
		in[3] = base64decode[in[3]];

		if (in[0] == 0x80 ||
			in[1] == 0x80 ||
			in[2] == 0x80 ||
			in[3] == 0x80)
		{
			throw SOAPException("Invalid character in base64 string.");
		}

		out[0] = (in[0] << 2) | (in[1] >> 4);
		out[1] = (in[1] << 4) | (in[2] >> 2);
		out[2] = (in[2] << 6) |  in[3];

		if (valid == 1)
		{
			array.Add(out[0]);
			done = true;
		}
		else if (valid == 2)
		{
			array.Add(out[0]);
			array.Add(out[1]);
			done = true;
		}
		else // valid == 3
		{
			array.Add(out[0]);
			array.Add(out[1]);
			array.Add(out[2]);
		}
	}
}

void
SOAPBase64::Encode(const SOAPArray<char>& array, SOAPString& str)
{
	size_t size = array.Size();
	size_t num64chars = (size / 3) * 4 + 4;
	str.Resize(num64chars + 4);

	char *out = str.Str();
	const unsigned char *in = (const unsigned char *)array.Ptr();
	size_t numout = 0;

	while (size >= 3)
	{
		*out++ = base64encode[in[0] >> 2];
		*out++ = base64encode[((in[0] & 3) << 4) | (in[1] >> 4)];
		*out++ = base64encode[((in[1] & 0xF) << 2) | (in[2] >> 6)];
		*out++ = base64encode[in[2] & 0x3F];
		size -= 3;
		in += 3;
		numout += 4;
	}

	if (size == 2)
	{
		*out++ = base64encode[in[0] >> 2];
		*out++ = base64encode[((in[0] & 3) << 4) | (in[1] >> 4)];
		*out++ = base64encode[(in[1] & 0xF) << 2];
		*out++ = base64pad;
	}
	else if (size == 1)
	{
		*out++ = base64encode[in[0] >> 2];
		*out++ = base64encode[(in[0] & 3) << 4];
		*out++ = base64pad;
		*out++ = base64pad;
	}

	*out = 0;
}

