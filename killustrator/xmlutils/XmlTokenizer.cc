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

#include "XmlTokenizer.h"
#include <ctype.h>

XmlTokenizer::XmlTokenizer (istream& is) : 
  strm (is.rdbuf ()), use_last (false), is_open (false) {
}

XmlTokenizer::~XmlTokenizer () {
}

/*
bool XmlTokenizer::hasMoreTokens () {
  return ! strm.eof ();
}
*/

void XmlTokenizer::skipWhitespace () {
  char c;
  do {
    strm.get (c);
    if (! isspace (c)) {
      strm.unget ();
      return;
    }
    else if (strm.eof ())
      return;
  } while (1);
}

void XmlTokenizer::unget () {
  use_last = true;
}

XmlTokenizer::Token XmlTokenizer::nextToken () {
  char c;

  if (use_last) {
    use_last = false;
    return last_tok;
  }

  skipWhitespace ();
  if (strm.eof ())
    return last_tok = Tok_EOF;

  strm.get (c);
  switch (c) {
  case '<':
    is_open = true;
    return last_tok = Tok_Lt;
    break;
  case '>':
    is_open = false;
    return last_tok = Tok_Gt;
    break;
  case '?':
    return last_tok = Tok_QSign;
    break;
  case '/':
    return last_tok = Tok_Slash;
    break;
  case '=':
    return last_tok = Tok_Eq;
    break;
  case '(':
    return last_tok = Tok_LParen;
    break;
  case ')':
    return last_tok = Tok_RParen;
    break;
  case '[':
    return last_tok = Tok_LBracket;
    break;
  case ']':
    return last_tok = Tok_RBracket;
    break;
  case '|':
    return last_tok = Tok_Bar;
    break;
  case '*':
    return last_tok = Tok_Asterisk;
    break;
  case '+':
    return last_tok = Tok_Plus;
    break;
  case ',':
    return last_tok = Tok_Comma;
    break;
  case ';':
    return last_tok = Tok_Semicolon;
    break;
  case '%':
    return last_tok = Tok_Percent;
    break;
  case '#':
    return last_tok = Tok_NSign;
    break;
  case '\'':
    return last_tok = Tok_Apostr;
    break;
  case '"':
    // String einlesen
    return last_tok = readString ();
    break;
  default:
    if (is_open) {
      if (isalpha (c) || isdigit (c)) {
	// Symbol (Element oder Attributbezeichner)
	strm.unget ();
	return last_tok = readSymbol ();
      }
      else if (c == '!') {
	strm.get (c);
	strm.unget ();
	if (c == '-')
	  return last_tok = readComment ();
	else
	  return last_tok = Tok_Exclam;
      }
      else {
	cout << "HIER1:" << c << endl;
	return last_tok = Tok_Invalid;
      }
    }
    else {
      strm.unget ();
      return last_tok = readText ();
    }
    break;
  }
}

const string& XmlTokenizer::element () {
  return elem;
}

XmlTokenizer::Token XmlTokenizer::readSymbol () {
  char c;
  elem = "";

  while (1) {
    strm.get (c);
    if (strm.eof () || isspace (c))
      // Symbol ist abgeschlossen
      break;
    else if (c == '=' || c == '/' || c == '>' || c == '?' || c == '|' ||
	     c == ')' || c == '\'' || c == ',' || c == ';') {
      // Symbol ist abgeschlossen, das gelesene Zeichen wird 
      // aber noch benoetigt
      strm.unget ();
      break;
    }
    else if (isalnum (c) || c == '-' || (c == '_' && elem.size () > 0))
      // korrektes Zeichen -> anhaengen
      elem += tolower (c);
    else {
      // Zeichen nicht erlaubt ?
      cout << "HIER2: " << c << endl;
      return Tok_Invalid;
    }
  }
  // alle Grossbuchstaben in Kleinbuchstaben aendern !!!!
  return Tok_Symbol;
}

XmlTokenizer::Token XmlTokenizer::readString () {
  char c;
  elem = "";

  while (1) {
    strm.get (c);
    if (strm.eof ())
      // String ist noch nicht abgeschlossen
      return Tok_Invalid;
    else if (c == '\\') {
      // naechstes Zeichen quoten
    }
    else if (c == '"') {
      // String ist abgeschlossen
      return Tok_String;
    }
    else
      elem += c;
  }
}

XmlTokenizer::Token XmlTokenizer::readComment () {
  char c1, c2;
  elem = "";

  strm.get (c1);
  strm.get (c2);
  if (c1 != '-' || c2 != '-' || strm.eof ())
    return Tok_Invalid;

  while (1) {
    strm.get (c1);
    if (strm.eof ())
      return Tok_Invalid;
    else if (c1 == '>')
      return Tok_Comment;
    else
      elem += c1;
  }
}

XmlTokenizer::Token XmlTokenizer::readText () {
  char c;
  elem = "";

  while (1) {
    strm.get (c);
    if (strm.eof ())
      return Tok_EOF;
    else if (c == '<') {
      strm.unget ();
      return Tok_Text;
    }
    else if (c == '&') {
      string s;
      while (c != ';') {
	s += c;
	strm.get (c);
	if (strm.eof ())
	  return Tok_EOF;
      }
      if (s == "&lt;")
	elem += "<";
      else if (s == "&gt;")
	elem += ">";
      else if (s == "&amp;")
	elem += "&";
    }
    else
      elem += c;
  }
}

#ifdef TEST
int main (int argc, char** argv) {
  XmlTokenizer::Token tok;

  XmlTokenizer tokenizer (cin);

  while ((tok = tokenizer.nextToken ()) != XmlTokenizer::Tok_EOF) {
    switch (tok) {
    case XmlTokenizer::Tok_Exclam:
      cout << "! ";
      break;
    case XmlTokenizer::Tok_Bar:
      cout << "| ";
      break;
    case XmlTokenizer::Tok_LParen:
      cout << "( ";
      break;
    case XmlTokenizer::Tok_RParen:
      cout << ") ";
      break;
    case XmlTokenizer::Tok_LBracket:
      cout << "[ ";
      break;
    case XmlTokenizer::Tok_RBracket:
      cout << "] ";
      break;
    case XmlTokenizer::Tok_Plus:
      cout << "+ ";
      break;
    case XmlTokenizer::Tok_Asterisk:
      cout << "* ";
      break;
    case XmlTokenizer::Tok_Comma:
      cout << ", ";
      break;
    case XmlTokenizer::Tok_Semicolon:
      cout << "; ";
      break;
    case XmlTokenizer::Tok_NSign:
      cout << "# ";
      break;
    case XmlTokenizer::Tok_Apostr:
      cout << "' ";
      break;
    case XmlTokenizer::Tok_Percent:
      cout << "% ";
      break;
    case XmlTokenizer::Tok_Lt:
      cout << "< ";
      break;
    case XmlTokenizer::Tok_Gt:
      cout << "> ";
      break;
    case XmlTokenizer::Tok_QSign:
      cout << "? ";
      break;
    case XmlTokenizer::Tok_Slash:
      cout << "/ ";
      break;
    case XmlTokenizer::Tok_Eq:
      cout << "= ";
      break;
    case XmlTokenizer::Tok_Symbol:
      cout << "SYMBOL(" << tokenizer.element () << ") ";
      break;
    case XmlTokenizer::Tok_String:
      cout << "STRING(" << tokenizer.element () << ") ";
      break;
    case XmlTokenizer::Tok_Comment:
      cout << "COMMENT > ";
      break;
    default:
      cout << "INVALID(" << tok << ")" << endl;
      return 1;
      break;
    }
  }
}
#endif
