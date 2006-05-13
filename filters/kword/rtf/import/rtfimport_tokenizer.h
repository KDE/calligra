/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef __RTFIMPORT_TOKENIZER_H__
#define __RTFIMPORT_TOKENIZER_H__

#include <q3cstring.h>
#include <QFile>


/// This class represents the tokenizer and the token
class RTFTokenizer
{
public:
    enum TokenType { OpenGroup, CloseGroup, ControlWord, PlainText, BinaryData };

    RTFTokenizer();

    /**
     * Open tokenizer from file.
     * @param in the input file
     */
    void open( QFile *in );
    /**
     * Reads the next token.
     */
    void next();

    // token data

    /// plain text or control word/symbol
    char *text;
    TokenType type;
    /// numeric parameter
    int value;
    /// token has a (numeric) parameter
    bool hasParam;

public:
    /// Binary data (of \\bin keyword)
    QByteArray binaryData;

    // tokenizer (private) data
private:
    int nextChar();

    QFile *infile;
    QByteArray fileBuffer;
    QByteArray tokenText;
    uchar *fileBufferPtr;
    uchar *fileBufferEnd;
};

#endif
