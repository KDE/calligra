/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef XmlTokenizer_h_
#define XmlTokenizer_h_

#include <iostream.h>
#include <string>

/**
 * The XMLTokenizer class allows an application to break a XML stream 
 * into tokens.
 *
 * @short     A class for tokenizing an XML stream.
 * @author    Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
 * @version   $Id$
 */
class XmlTokenizer {
public:
  /**
   * The expected tokens for a XML stream.
   */
  enum Token { Tok_Invalid,
	       Tok_EOF, 
	       Tok_Symbol, 
	       Tok_String, 
	       Tok_Text, 
	       Tok_Comment, 
	       Tok_Lt,       /* < */
	       Tok_Gt,       /* > */
	       Tok_QSign,    /* ? */
	       Tok_Eq,       /* = */
	       Tok_Slash,    /* / */
	       Tok_Exclam,   /* ! */
	       Tok_Bar,      /* | */
	       Tok_LParen,   /* ( */
	       Tok_RParen,   /* ) */
	       Tok_LBracket, /* [ */
	       Tok_RBracket, /* ] */
	       Tok_Plus,     /* + */
	       Tok_Asterisk, /* * */
	       Tok_Comma,    /* , */
	       Tok_Semicolon,/* ; */
	       Tok_NSign,    /* # */
	       Tok_Apostr,   /* ' */
	       Tok_Percent   /* % */
	       };

  /**
   * Create a XmlTokenizer instance for the given input stream.
   *
   * @param is   The open input stream for reading.
   */
  XmlTokenizer (istream& is);

  /**
   * Destructor.
   */
  ~XmlTokenizer ();

  /**
    * Return the next token from the stream.
   * @return       The next token from the stream.
   */
  Token nextToken ();

  /**
   * Return the string representation of the current token.
   *
   * @return  The string representation.
   */
  const std::string& element ();

  /**
   * Cause the next call to method <tt>nextToken</tt> of this tokenizer 
   * to return the current token.
   */
  void unget ();

protected:
  void skipWhitespace ();
  Token readString ();
  Token readSymbol ();
  Token readText ();
  Token readComment ();

private:
  std::istream strm;
  std::string elem;
  Token last_tok;
  bool use_last;
  bool is_open;
};

#endif

