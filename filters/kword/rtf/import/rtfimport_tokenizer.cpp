/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include <kdebug.h>

#include "rtfimport_tokenizer.h"


RTFTokenizer::RTFTokenizer()
{
    tokenText.resize( 4112 );
    fileBuffer.resize( 4096 );
    infile = 0L;
}

/**
 * Open tokenizer from file.
 * @param in the input file
 */
void RTFTokenizer::open( QFile *in )
{
    fileBufferPtr = 0L;
    fileBufferEnd = 0L;
    infile = in;
}

/**
 * Reads the next token.
 */
void RTFTokenizer::next()
{
    int ch;
    value=0;
    if (!infile)
	return;

    do
    {
	if (fileBufferPtr == fileBufferEnd)
	{
	    int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );

	    if (n <= 0)
	    {
		// Return CloseGroup on EOF
		ch = '}';
		break;
	    }
	    fileBufferPtr = (uchar *)fileBuffer.data();
	    fileBufferEnd = (fileBufferPtr + n);
	}
	ch = *fileBufferPtr++;
    }
    while (ch == '\n' || ch == '\r' && ch != 0);

    // Skip one byte for prepend '@' to destinations
    text = (tokenText.data() + 1);
    hasParam = false;

    uchar *_text = (uchar *)text;

    if (ch == '{')
	type = RTFTokenizer::OpenGroup;
    else if (ch == '}')
	type = RTFTokenizer::CloseGroup;
    else if (ch == '\\')
    {
	type = RTFTokenizer::ControlWord;

	if (fileBufferPtr == fileBufferEnd)
	{
	    int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );

	    if (n <= 0)
	    {
		// Return CloseGroup on EOF
		type = RTFTokenizer::CloseGroup;
		return;
	    }
	    fileBufferPtr = (uchar *)fileBuffer.data();
	    fileBufferEnd = (fileBufferPtr + n);
	}
	ch = *fileBufferPtr++;

	// Type is either control word or control symbol
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
	    int v = 0;

	    // Read alphabetic string (command)
	    while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	    {
		*_text++ = ch;

		if (fileBufferPtr == fileBufferEnd)
		{
		    int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );

		    if (n <= 0)
		    {
			ch = ' ';
			break;
		    }
		    fileBufferPtr = (uchar *)fileBuffer.data();
		    fileBufferEnd = (fileBufferPtr + n);
		}
		ch = *fileBufferPtr++;
	    }

	    // Read numeric parameter (param)
	    bool isneg = (ch == '-');

	    if (isneg)
	    {
		if (fileBufferPtr == fileBufferEnd)
		{
		    int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );

		    if (n <= 0)
		    {
			// Return CloseGroup on EOF
			type = RTFTokenizer::CloseGroup;
			return;
		    }
		    fileBufferPtr = (uchar *)fileBuffer.data();
		    fileBufferEnd = (fileBufferPtr + n);
		}
		ch = *fileBufferPtr++;
	    }
	    while (ch >= '0' && ch <= '9')
	    {
		v	 = (10 * v) + ch - '0';
		hasParam = true;

		if (fileBufferPtr == fileBufferEnd)
		{
		    int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );

		    if (n <= 0)
		    {
			ch = ' ';
			break;
		    }
		    fileBufferPtr = (uchar *)fileBuffer.data();
		    fileBufferEnd = (fileBufferPtr + n);
		}
		ch = *fileBufferPtr++;
	    }
	    value = isneg ? -v : v;

	    // If delimiter is a space, it's part of the control word
	    if (ch != ' ')
	    {
		--fileBufferPtr;
	    }

            *_text = 0; // Just put an end of string for the test, it can then be over-written again
            if ( !qstrncmp( tokenText.data()+1, "bin", 4 ) ) // Test the NULL too to avoid catching keywords starting with "bin"
            {   // We have \bin, so we need to read the bytes
                kdDebug(30515) << "Token:" << tokenText << endl;
                if (value > 0)
                {
                    kdDebug(30515) << "\\bin" << value << endl;
                    type = RTFTokenizer::BinaryData;
                    binaryData.resize(value);
                    for (int i=0; i<value; i++)
                    {
                        if (fileBufferPtr == fileBufferEnd)
                        {
                            const int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );

                            if (n <= 0)
                            {
                                kdError(30515) << "\\bin stream hit end of file." << endl;
                                type = RTFTokenizer::CloseGroup;
                                break;
                            }
                            fileBufferPtr = (uchar *)fileBuffer.data();
                            fileBufferEnd = (fileBufferPtr + n);
                        }
                        binaryData[i]=*fileBufferPtr++;
                    }
                }
                else
                {
                    kdError(30515) << "\\bin with negative value skipping" << endl;
                }
            }

	}
	else if (ch=='\'')
	{
	    type = RTFTokenizer::ControlWord;
	    *_text++ = ch;
	    if (fileBufferPtr == fileBufferEnd)
	    {
		int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );

		if (n <= 0)
		{
		    // Return CloseGroup on EOF
		    type = RTFTokenizer::CloseGroup;
		    return;
		}
		fileBufferPtr = (uchar *)fileBuffer.data();
		fileBufferEnd = (fileBufferPtr + n);
	    }
	    ch = *fileBufferPtr++;
	    for(int i=0;i<2;i++)
	    {
		hasParam = true;
		value<<=4;
		value=value|((ch + ((ch & 16) ? 0 : 9)) & 0xf);

		if (fileBufferPtr == fileBufferEnd)
		{
		    int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );

		    if (n <= 0)
		    {
			ch = ' ';
			break;
		    }
		    fileBufferPtr = (uchar *)fileBuffer.data();
		    fileBufferEnd = (fileBufferPtr + n);
		}
		ch = *fileBufferPtr++;
	    }
		--fileBufferPtr;
	    }
	else
	{
	    type = RTFTokenizer::ControlWord;
	    *_text++ = ch;
	}
    }
    else
    {
	type = RTFTokenizer::PlainText;

	// Everything until next backslash, opener or closer
	while ( ch != '\\' && ch != '{' && ch != '}' && ch != '\n' &&
		ch != '\r' && fileBufferPtr <= fileBufferEnd )
	{
	    *_text++ = ch;
	    ch = *fileBufferPtr++;
	}

	// Give back last char
	--fileBufferPtr;
    }
    *_text++ = 0;
}
