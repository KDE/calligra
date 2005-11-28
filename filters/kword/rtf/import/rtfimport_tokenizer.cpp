/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>
   Copyright (C) 2005 Tommi Rantala <tommi.rantala@cs.helsinki.fi>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include <kdebug.h>

#include "rtfimport_tokenizer.h"


RTFTokenizer::RTFTokenizer()
{
    tokenText.resize( 4113 );
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
    type = RTFTokenizer::PlainText;
}

int RTFTokenizer::nextChar()
{
    if ( fileBufferPtr == fileBufferEnd ) {
        int n = infile->readBlock( fileBuffer.data(), fileBuffer.size() );
        fileBufferPtr = ( uchar* ) fileBuffer.data();
        fileBufferEnd = fileBufferPtr;

        if ( n <= 0 )
            return -1;

        fileBufferEnd = fileBufferPtr + n;
    }
    return *fileBufferPtr++;
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

    do {
        int n = nextChar();

        if ( n <= 0 ) {
            ch = '}';
            break;
        }

        ch = n;
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

        int n = nextChar();

        if ( n <= 0 ) {
            // Return CloseGroup on EOF
            type = RTFTokenizer::CloseGroup;
            return;
        }
	ch = n;

	// Type is either control word or control symbol
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
	    int v = 0;

	    // Read alphabetic string (command)
	    while (_text < ( uchar* )tokenText.data()+tokenText.size()-3 && 
                  ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) )
	    {
		*_text++ = ch;

                int n = nextChar();
                if ( n <= 0 ) {
                    ch = ' ';
                    break;
                }
                ch = n;
	    }

	    // Read numeric parameter (param)
	    bool isneg = (ch == '-');

	    if (isneg) {
                int n = nextChar();
                if ( n <= 0 ) {
                    type = RTFTokenizer::CloseGroup;
                    return;
                }
		ch = n;
	    }

	    while (ch >= '0' && ch <= '9') {
		v	 = (10 * v) + ch - '0';
		hasParam = true;

                int n = nextChar();

                if ( n <= 0 )
                    n = ' ';
                ch = n;
            }
	    value = isneg ? -v : v;

	    // If delimiter is a space, it's part of the control word
	    if (ch != ' ')
	    {
		--fileBufferPtr;
	    }

            *_text = 0; // Just put an end of string for the test, it can then be over-written again
            if ( !memcmp( tokenText.data()+1, "bin", 4 ) )
            {   // We have \bin, so we need to read the bytes
                kdDebug(30515) << "Token:" << tokenText << endl;
                if (value > 0)
                {
                    kdDebug(30515) << "\\bin" << value << endl;
                    type = RTFTokenizer::BinaryData;
                    binaryData.resize(value);
                    for (int i=0; i<value; i++)
                    {
                        int n = nextChar();
                        if ( n <= 0 ) {
                            type = RTFTokenizer::CloseGroup;
                            break;
                        }

                        binaryData[i] = n;
                    }
                }
            }

	}
	else if (ch=='\'')
	{
	    // Got hex value, for example \'2d

	    type = RTFTokenizer::ControlWord;
	    *_text++ = ch;

	    for(int i=0;i<2;i++)
	    {
		int n = nextChar();

		if ( n <= 0 ) {
		    if ( i == 0 ) {
		        type = RTFTokenizer::CloseGroup;
		        return;
		    } else {
                        ch = ' ';
			break;
		    }
		}

		ch = n;

		hasParam = true;
		value<<=4;
		value=value|((ch + ((ch & 16) ? 0 : 9)) & 0xf);
	    }
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
		ch != '\r')
	{
	    *_text++ = ch;
            if(fileBufferPtr >= fileBufferEnd)
                break;
	    ch = *fileBufferPtr++;
	}
        if(fileBufferPtr < fileBufferEnd)
          --fileBufferPtr; // give back the last char
    }
    *_text++ = 0;

}
