/****************************************************************************
** $Id$
**
** Implementation of QXML classes
**
** Created : 990128
**
** Copyright (C) 1992-1999 Troll Tech AS.  All rights reserved.
**
** This file is part of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Troll Tech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** Licensees holding valid Qt Professional Edition licenses may use this
** file in accordance with the Qt Professional Edition License Agreement
** provided with the Qt Professional Edition.
**
** See http://www.troll.no/pricing.html or email sales@troll.no for
** information about the Professional Edition licensing, or see
** http://www.troll.no/qpl/ for QPL licensing information.
**
*****************************************************************************/

#include "qxml.h"

#include <qmap.h>

#include <ctype.h>

/*********************************************
 *
 * QXMLConsumer
 *
 *********************************************/

QXMLConsumer::QXMLConsumer()
{
}

QXMLConsumer::~QXMLConsumer()
{
}

/*********************************************
 *
 * QXMLSimpleParserPrivate
 *
 *********************************************/

class QXMLSimpleParserPrivate
{
public:
  struct ExternParamEntity
  {
    QString publicId;
    QString systemId;
  };
  struct ExternEntity
  {
    QString publicId;
    QString systemId;
    QString notation;
  };

  QMap<QString,ExternParamEntity> externParamEntities;
  QMap<QString,QString> paramEntities;
  QMap<QString,ExternEntity> externEntities;
  QMap<QString,QString> entities;
  QXMLConsumer* consumer;
};

/*********************************************
 *
 * QXMLSimpleParser
 *
 *********************************************/

QXMLSimpleParser::QXMLSimpleParser()
{
  d = new QXMLSimpleParserPrivate();
}

QXMLSimpleParser::~QXMLSimpleParser()
{
  delete d;
}

bool QXMLSimpleParser::parameterEntity( const QString& name, const QString& publicId, const QString& systemId )
{
  if ( d->consumer )
    if ( !d->consumer->parameterEntity( name, publicId, systemId ) )
      return FALSE;

  struct QXMLSimpleParserPrivate::ExternParamEntity e;
  e.publicId = publicId;
  e.systemId = systemId;

  d->externParamEntities.insert( name, e );

  return TRUE;
}

bool QXMLSimpleParser::parameterEntity( const QString& name, const QString& value )
{
  QString repl;
  if ( !replaceCharRefs( value, &repl ) )
  {
    qWarning("Could not replace char refs in '%s'\n",value.ascii());
    return FALSE;
  }

  if ( d->consumer )
    if ( !d->consumer->parameterEntity( name, repl ) )
      return FALSE;

  d->paramEntities.insert( name, repl );

  return TRUE;
}

bool QXMLSimpleParser::paramEntityValue( const QString& name, QString* value )
{
  QMap<QString,QXMLSimpleParserPrivate::ExternParamEntity>::Iterator it = d->externParamEntities.find( name );
  if ( it != d->externParamEntities.end() )
  {
    qWarning("Can not expand extern parameter entity %s\n",name.ascii());
    return FALSE;
  }

  QMap<QString,QString>::Iterator it2 = d->paramEntities.find( name );
  if ( it2 == d->paramEntities.end() )
  {
    qWarning("Can not find parameter entity %s\n",name.ascii());
    return FALSE;
  }

  *value = it2.data();

  return TRUE;
}

bool QXMLSimpleParser::entity( const QString& name, const QString& publicId, const QString& systemId, const QString& ndata )
{
  if ( d->consumer )
    if ( !d->consumer->entity( name, publicId, systemId, ndata ) )
      return FALSE;

  struct QXMLSimpleParserPrivate::ExternEntity e;
  e.publicId = publicId;
  e.systemId = systemId;
  e.notation = ndata;

  d->externEntities.insert( name, e );

  return TRUE;
}

bool QXMLSimpleParser::entity( const QString& name, const QString& value )
{
  QString repl;
  if ( !replaceCharRefs( value, &repl ) )
  {
    qWarning("Could not replace char refs in '%s'\n",value.ascii());
    return FALSE;
  }

  if ( d->consumer )
    if ( !d->consumer->entity( name, repl ) )
      return FALSE;

  d->entities.insert( name, repl );

  return TRUE;
}

bool QXMLSimpleParser::entityValue( const QString& name, QString* value )
{
  QMap<QString,QXMLSimpleParserPrivate::ExternEntity>::Iterator it = d->externEntities.find( name );
  if ( it != d->externEntities.end() )
  {
    if ( it.data().notation.isEmpty() )
    {
      qWarning("Can not expand extern entity %s\n",name.ascii());
      return FALSE;
    }
    // Unparsed external entities (binary data like images) is just represented by an entity reference
    if ( d->consumer )
      if ( !d->consumer->entityRef( name ) )
	return FALSE;
    *value = QString::null;
    return TRUE;
  }

  QMap<QString,QString>::Iterator it2 = d->entities.find( name );
  if ( it2 == d->entities.end() )
  {
    qWarning("Can not find entity %s\n",name.ascii());
    return FALSE;
  }

  *value = it2.data();

  return TRUE;
}

bool QXMLSimpleParser::replaceCharRefs( const QString& x, QString* text )
{
  *text = x;
  int pos = 0;
  int len = text->length();

  while( pos < len )
  {
    if ( (*text)[pos] == '&' )
    {
      int begin = pos++;
      int start = pos;

      while( pos < len && (*text)[pos] != ';' )
	++pos;
      if ( pos == len )
	return FALSE;
      QString entity = text->mid( start, pos - start );
      if ( entity.isEmpty() )
	return FALSE;
      ++pos;

      QString value;
      if ( entity.length() >= 2 && entity[0] == '#' && entity[1] == 'x' )
      {
	if ( entity.length() < 3 )
	  return FALSE;
	QString tmp = entity.mid( 2 );
	bool ok;
	uint i = tmp.toUInt( &ok, 16 );
	if ( !ok )
	  return FALSE;
	value = QString( QChar( i ) );
      }
      else if ( entity[0] == '#' )
      {
	if ( entity.length() < 2 )
	  return FALSE;
	QString tmp = entity.mid( 1 );
	bool ok;
	uint i = tmp.toUInt( &ok );
	if ( !ok )
	  return FALSE;
	value = QString( QChar( i ) );
      }
      else
	continue;
      text->replace( begin, pos - begin, value );
      pos = begin + value.length();
    }
    else
      ++pos;
  }

  return TRUE;
}

/*!
  Returns -1 if everything went fine or the position in the text
  where the error was detected. If no consumer is passed to this
  function then the parser checks just wether it can scan
  the text.
*/

int QXMLSimpleParser::parse( QString text, QXMLConsumer* consumer )
{
  // Remember the consumer
  d->consumer = consumer;

  int len = text.length();
  int pos = 0;
  int start = 0;
  QString attrib;
  bool preclosed;
  QString tag;
  QString publicId;
  QString systemId;
  QString repl;
  QChar quote;
  bool parameter_entity;
  int include_counter = 0;
  int ignore_counter = 0;
  Type type;

 Node1: // accepts
  if ( pos == len )
    goto Ok;
  else if ( text[pos] == '<' )
  {
    pos++;
    goto Node2;
  }
  start = pos++;
  goto Node20;
 Node2: // Tag
  if ( pos + 3 <= len && text[pos] == '!' && text[pos+1] == '-' && text[pos+2] == '-' )
  {
    pos += 3;
    goto Node30;
  }
  if ( pos == len )
    goto Failed;

  preclosed = false;
  tag = QString::null;

  if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node2;
  }
  if ( text[pos] == '?' )
  {
    pos++;
    goto Node40;
  }
  else if ( text[pos] == '!' )
  {
    pos++;
    goto Node50;
  }
  else if ( text[pos] == '/' )
  {
    preclosed = true;
    pos++;
    goto Node3;
  }
  else if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' )
  {
    start = pos++;
    goto Node4;
  }
  else
    goto Failed;
 Node3:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node3;
  }
  else if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' )
  {
    start = pos++;
    goto Node4;
  }
  else
    goto Failed;
 Node4:
  if ( pos == len )
    goto Failed;
  else if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' || text[pos] == '.' || text[pos] == '-' )
  {
    ++pos;
    goto Node4;
  }
  else if ( text[pos] == '>' )
  {
    if ( consumer )
    {
      if ( preclosed )
      {
	if ( !consumer->tagEnd( text.mid( start, pos - start ) ) )
	  return pos;
      }
      else if ( !consumer->tagStart( text.mid( start, pos - start ) ) )
	return pos;
    }
    pos++;
    goto Node15;
  }
  else if ( text[pos] == '/' )
  {
    if ( preclosed )
      goto Failed;

    if ( consumer )
    {
      if ( !consumer->tagStart( text.mid( start, pos - start ) ) )
	return pos;
      if ( !consumer->tagEnd( text.mid( start, pos - start ) ) )
	return pos;
    }

    pos++;
    goto Node14;
  }
  else if ( text[pos].isSpace() )
  {
    tag = text.mid( start, pos - start );

    if ( consumer )
    {
      if ( preclosed )
      {
	if ( !consumer->tagEnd( text.mid( start, pos - start ) ) )
	  return pos;
      }
      else
      {
	if ( !consumer->tagStart( tag ) )
	  return pos;
      }
    }
    pos++;
    goto Node5;
  }
  else
    goto Failed;
 Node5:
  if ( pos == len )
    goto Failed;
  else if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node5;
  }
  else if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' )
  {
    start = pos++;
    goto Node6;
  }
  else if ( text[pos] == '>' )
  {
    /* if ( consumer )
    {
      if ( preclosed )
      {
	if ( !consumer->tagEnd( tag ) )
	  return pos;
      }
      else if ( !consumer->tagStart( tag ) )
	return pos;
    } */

    pos++;
    goto Node15;
  }
  else if ( text[pos] == '/' )
  {
    if ( preclosed )
      goto Failed;

    if ( consumer )
    {
      // if ( !consumer->tagStart( tag ) )
      // return pos;
      if ( !consumer->tagEnd( tag ) )
	return pos;
    }
    pos++;
    goto Node14;
  }
  else
    goto Failed;
 Node6:
  if ( pos == len )
    goto Failed;

  /* if ( consumer )
    if ( !consumer->tagStart( tag ) )
    return pos; */

  else if ( text[pos].isSpace() )
  {
    attrib = text.mid( start, pos - start );
    ++pos;
    goto Node7;
  }
  else if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' || text[pos] == '.' || text[pos] == '-' )
  {
    ++pos;
    goto Node6;
  }
  else if ( text[pos] == '=' )
  {
    attrib = text.mid( start, pos - start );
    ++pos;
    goto Node8;
  }
  else
    goto Failed;
 Node7:
  if ( pos == len )
    goto Failed;
  else if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node7;
  }
  else if ( text[pos] == '=' )
  {
    ++pos;
    goto Node8;
  }
  else
    goto Failed;
 Node8:
  if ( pos == len )
    goto Failed;
  else if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node10;
  }
  else if ( text[pos] == '"' )
  {
    ++pos;
    goto Node9;
  }
  else
    goto Failed;
 Node9:
  if ( pos == len )
    goto Failed;
  start = pos;
  if ( text[pos] == '"' )
  {
    if ( consumer )
      if ( !consumer->attrib( attrib, text.mid( start, pos - start ) ) )
	return pos;

    pos++;
    goto Node12;
  }
  else
  {
    ++pos;
    goto Node11;
  }
 Node10:
  if ( pos == len )
    goto Failed;
  else if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node10;
  }
  else if ( text[pos] == '"' )
  {
    ++pos;
    goto Node9;
  }
  else
    goto Failed;
 Node11:
  if ( pos == len )
    goto Failed;
  else if ( text[pos] == '"' )
  {
    if ( consumer )
      if ( !consumer->attrib( attrib, text.mid( start, pos - start ) ) )
	return pos;

    pos++;
    goto Node12;
  }
  else
  {
    ++pos;
    goto Node11;
  }
 Node12:
  if ( pos == len )
    goto Failed;
  else if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node13;
  }
  else if ( text[pos] == '/' )
  {
    if ( preclosed )
      goto Failed;

    if ( consumer )
      if ( !consumer->tagEnd( tag ) )
	return pos;

    ++pos;
    goto Node14;
  }
  else if ( text[pos] == '>' )
  {
    ++pos;
    goto Node15;
  }
  else
    goto Failed;
 Node13:
  if ( pos == len )
    goto Failed;
  else if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node13;
  }
  else if ( text[pos] == '/' )
  {
    if ( preclosed )
      goto Failed;

    if ( consumer )
      if ( !consumer->tagEnd( tag ) )
	return pos;

    ++pos;
    goto Node14;
  }
  else if ( text[pos] == '>' )
  {
    ++pos;
    goto Node15;
  }
  else if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' )
  {
    start = pos++;
    goto Node6;
  }
  else
    goto Failed;
 Node14:
  if ( pos == len )
    goto Failed;
  else if ( text[pos] == '>' )
  {
    ++pos;
    goto Node15;
  }
  else
    goto Failed;
 Node15: // accepts
  if ( pos == len )
    goto Ok;
  goto Node1;
 Node20: // Text
  if ( pos == len )
  {
    if ( pos != start && consumer )
    {
      QString tmp = text.mid( start, pos - start );
      tmp = tmp.simplifyWhiteSpace();
      if ( !tmp.isEmpty() )
	if ( !consumer->text( tmp ) )
	  return pos;
    }
    goto Ok;
  }
  else if ( pos + 7 < len && text[pos] == '[' && text[pos+1] == 'C' && text[pos+2] == 'D' && text[pos+3] == 'A' &&
	    text[pos+4] == 'T' && text[pos+5] == 'A' && text[pos+6] == '[' )
  {
    if ( pos != start && consumer )
    {
      QString tmp = text.mid( start, pos - start );
      tmp = tmp.simplifyWhiteSpace();
      if ( !tmp.isEmpty() )
	if ( !consumer->text( tmp ) )
	  return pos;
    }

    pos += 7;
    start = pos;
    while( pos + 2 <= len && ( text[pos] != ']' || text[pos+1] != ']' ) )
      ++pos;
    if ( pos + 2 > len )
      goto Failed;

    if ( pos != start && consumer )
    {
      QString tmp = text.mid( start, pos - start );
      if ( !tmp.isEmpty() )
	if ( !consumer->text( tmp ) )
	  return pos;
    }

    pos += 2;
    start = pos;
    goto Node20;
  }
  else if ( text[pos] == '&' )
  {
    int begin = pos;
    ++pos;

    while( pos < len && text[pos] != ';' )
      ++pos;
    if ( pos == len )
      goto Failed;

    bool simple_entity = TRUE;

    QString entity = text.mid( begin + 1, pos - begin - 1 );
    if ( entity.isEmpty() )
      goto Failed;
    QString value;

    // Test for simple entities
    if ( entity == "amp" )
      value = "&";
    else if ( entity == "lt" )
      value = "<";
    else if ( entity == "gt" )
      value = ">";
    else if ( entity == "quot" )
      value = "\"";
    else if ( entity == "apos" )
      value = "'";
    else if ( entity.length() >= 2 && entity[0] == '#' && entity[1] == 'x' )
    {
      if ( entity.length() < 3 )
	goto Failed;
      QString tmp = entity.mid( 2 );
      bool ok;
      uint i = tmp.toUInt( &ok, 16 );
      if ( !ok )
	goto Failed;
      value = QString( QChar( i ) );
    }
    else if ( entity[0] == '#' )
    {
      if ( entity.length() < 2 )
	goto Failed;
      QString tmp = entity.mid( 1 );
      bool ok;
      uint i = tmp.toUInt( &ok );
      if ( !ok )
	goto Failed;
      value = QString( QChar( i ) );
    }
    else
    {
      // We will replace the &xxxxx; construct with
      // the entities value and parse the replaces text, too.
      // That is not the case for simple entities like &lt;
      simple_entity = FALSE;
      // This is a custom entity. The application has to know about ...
      if ( !entityValue( entity, &value ) )
	return pos;
    }

    // Replace the &xxxxx; construct with the entities value
    ++pos;
    text.replace( begin, pos - begin, value );
    len += value.length();
    len -= ( pos - begin );
    pos = begin;

    // Skip replacement of simple entities like &lt;
    if ( simple_entity )
      pos += value.length();

    goto Node20;
  }
  else if ( text[pos] == '<' )
  {
    if ( pos != start && consumer )
    {
      QString tmp = text.mid( start, pos - start );
      tmp = tmp.simplifyWhiteSpace();
      if ( !tmp.isEmpty() )
	if ( !consumer->text( tmp ) )
	  return pos;
    }
    ++pos;
    goto Node2;
  }
  ++pos;
  goto Node20;
 Node30: // Comments
  if ( pos == len )
    goto Failed;
  else if ( text[pos] == '-' )
  {
    ++pos;
    goto Node31;
  }
  else 
  {
    ++pos;
    goto Node30;
  }
 Node31:
  if ( pos == len )
    goto Failed;
  else if ( text[pos] == '-' )
  {
    ++pos;
    goto Node32;
  }
  else 
  {
    ++pos;
    goto Node30;
  }
 Node32:
  if ( pos == len )
    goto Failed;
  else if ( text[pos] == '>' )
  {
    ++pos;
    goto Node1;
  }
  else if ( text[pos] == '-' )
  {
    ++pos;
    goto Node32;
  }
  ++pos;
  goto Node30;
 Node40: // Processing Instructions
  if ( pos == len )
    goto Failed;

  if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' )
  {
    start = pos++;
    goto Node41;
  }
  goto Failed;
 Node41:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' || text[pos] == '.' || text[pos] == '-' )
  {
    pos++;
    goto Node41;
  }
  else if ( text[pos].isSpace() )
  {
    tag = text.mid( start, pos - start );
    pos++;
    start = pos;
    goto Node42;
  }
  else if ( text[pos] == '?' )
  {
    if ( !consumer->processingInstruction( text.mid( start, pos - start ), QString::null ) )
      return pos;
    pos++;
    goto Node44;
  }
  goto Failed;
 Node42:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '?' )
  {
    pos++;
    goto Node43;
  }
  pos++;
  goto Node42;
 Node43:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '>' )
  {
    if ( !consumer->processingInstruction( tag, text.mid( start, pos - start - 1 ) ) )
      return pos;
    pos++;
    goto Node1;
  }
  pos++;
  goto Node42;
 Node44:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '>' )
  {
    pos++;
    goto Node1;
  }
  goto Failed;
 Node50: // Doctype
  if ( pos == len )
    goto Failed;
  if ( pos + 7 < len && text[pos] == 'D' && text[pos+1] == 'O' && text[pos+2] == 'C' && text[pos+3] == 'T' &&
	    text[pos+4] == 'Y' && text[pos+5] == 'P' && text[pos+6] == 'E' )
  {
    pos += 7;
    goto Node51;
  }
  goto Failed;
 Node51:
  if ( pos == len )
    goto Failed;
  if ( !text[pos].isSpace() )
    goto Failed;
  ++pos;
  goto Node52;
 Node52:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node52;
  }
  if ( text[pos].isLetter() || text[pos] == '_' || text[pos] == ':' )
  {
    start = pos++;
    goto Node53;
  }
  goto Failed;
 Node53:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '>' )
  {
    tag = text.mid( start, pos - start );
    if ( consumer )
      if ( !consumer->doctype( tag ) )
	return pos;
    pos++;
    goto Node55;
  }
  if ( text[pos].isSpace() )
  { 
    tag = text.mid( start, pos - start );
    if ( consumer )
      if ( !consumer->doctype( tag ) )
	return pos;
    ++pos;
    goto Node54;
  }
  if ( text[pos].isLetter() || text[pos] == ':' || text[pos] == '_' || text[pos] == '-' || text[pos] == '.' )
  {
    ++pos;
    goto Node53;
  }
  goto Failed;
 Node54:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node54;
  }
  if ( text[pos] == '>' )
  {
    pos++;
    goto Node55;
  }
  if ( text[pos] == '[' )
  {
    pos++;
    goto Node65;
  }
  if ( pos + 6 < len && text[pos] == 'P' && text[pos+1] == 'U' && text[pos+2] == 'B' && text[pos+3] == 'L' &&
	    text[pos+4] == 'I' && text[pos+5] == 'C')
  {
    pos += 6;
    goto Node56;
  };
  if ( pos + 6 < len && text[pos] == 'S' && text[pos+1] == 'Y' && text[pos+2] == 'S' && text[pos+3] == 'T' &&
	    text[pos+4] == 'E' && text[pos+5] == 'M')
  {
    pos += 6;
    goto Node64;
  };
  goto Failed;
 Node55:
  if ( pos == len )
    goto Failed;
  // TODO
  goto Node1;
 Node56:
  if ( pos == len )
    goto Failed;
  if ( !text[pos].isSpace() )
    goto Failed;
  ++pos;
  goto Node57;
 Node57:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node57;
  }
  if ( text[pos] == '"' || text[pos] == '\'' )
  {
    quote = text[pos];
    start = ++pos;
    goto Node58;
  }
  goto Failed;
 Node58:
  if ( pos == len )
    goto Failed;
  if ( text[pos] != quote )
  {
    ++pos;
    goto Node58;
  }
  publicId = text.mid( start, pos - start );
  ++pos;
  goto Node59;
 Node59:
  if ( pos == len )
    goto Failed;
  if ( !text[pos].isSpace() )
    goto Failed;
  ++pos;
  goto Node60;
 Node60:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node60;
  }
  if ( text[pos] == '"' || text[pos] == '\'' )
  {
    quote = text[pos];
    start = ++pos;
    goto Node61;
  }
  goto Failed;
 Node61:
  if ( pos == len )
    goto Failed;
  if ( text[pos] != quote )
  {
    ++pos;
    goto Node61;
  }
  systemId = text.mid( start, pos - start );
  if ( consumer )
    if ( !consumer->doctypeExtern( publicId, systemId ) )
      return pos;
  ++pos;
  goto Node62;
 Node62:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '>' )
  {
    pos++;
    goto Node55;
  }
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node63;
  }
  goto Failed;
 Node63:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node63;
  }
  if ( text[pos] == '>' )
  {
    pos++;
    goto Node55;
  }
  if ( text[pos] == '[' )
  {
    pos++;
    goto Node65;
  }
  goto Failed;
 Node64: // System
  if ( pos == len )
    goto Failed;
  publicId = QString::null;
  if ( text[pos].isSpace() )
  {
    pos++;
    goto Node60;
  }
 Node65: // Internal DTD
  if ( pos == len )
    goto Failed;
  if ( pos + 3 < len && text[pos] == ']' && text[pos+1] == ']' && text[pos+2] == '>' )
  {
    pos += 3;
    if ( include_counter == 0 )
      goto Failed;
    --include_counter;
    goto Node65;
  }
  if ( text[pos] == '%' )
  {
    start = ++pos;
    goto Node70;
  }
  if ( text[pos] == '<' )
  {
    ++pos;
    goto Node72;
  }
  if ( text[pos] == ']' )
  {
    ++pos;
    goto Node66;
  }
  if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node65;
  }
  goto Failed;
 Node66:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '>' )
  {
    ++pos;
    goto Node55;
  }
  if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node67;
  }
  goto Failed;
 Node67:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '>' )
  {
    ++pos;
    goto Node55;
  }
  if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node67;
  }
  goto Failed;
 Node70: // Parameter Entity
  if ( pos == len )
    goto Failed;
  if ( text[pos] != ';' )
  {
    ++pos;
    goto Node70;
  }
  // Insert the value of the parameter entity here so that we
  // can parse it.
  if ( !paramEntityValue( text.mid( start, pos - start ), &repl ) )
    return pos;
  ++pos;
  text.insert( pos, repl );
  len += repl.length();
  goto Node65;
 Node72:
  if ( pos == len )
    goto Failed;
  if ( pos + 2 <= len && text[pos] == '!' && text[pos+1] == '[' )
  {
    pos += 2;
    goto Node110;
  }
  if ( pos + 7 < len && text[pos] == '!' && text[pos+1] == 'E' && text[pos+2] == 'N' && text[pos+3] == 'T' &&
	    text[pos+4] == 'I' && text[pos+5] == 'T' && text[pos+6] == 'Y' )
  {
    type = Entity;
    pos += 7;
    goto Node76;
  }
  if ( pos + 7 < len && text[pos] == '!' && text[pos+1] == 'E' && text[pos+2] == 'L' && text[pos+3] == 'E' &&
	    text[pos+4] == 'M' && text[pos+5] == 'E' && text[pos+6] == 'N'  && text[pos+7] == 'T' )
  {
    type = Element;
    pos += 8;
    goto Node73;
  }
  if ( pos + 7 < len && text[pos] == '!' && text[pos+1] == 'A' && text[pos+2] == 'T' && text[pos+3] == 'T' &&
	    text[pos+4] == 'L' && text[pos+5] == 'I' && text[pos+6] == 'S'  && text[pos+7] == 'T' )
  {
    type = Attlist;
    pos += 8;
    goto Node73;
  }
  if ( pos + 9 < len && text[pos] == '!' && text[pos+1] == 'N' && text[pos+2] == 'O' && text[pos+3] == 'T' &&
	    text[pos+4] == 'A' && text[pos+5] == 'T' && text[pos+6] == 'I' && text[pos+7] == 'O' && text[pos+8] == 'N' )
  {
    type = Notation;
    pos += 9;
    goto Node76;
  }
  goto Failed;
 Node73:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  {
    start = ++pos;
    goto Node74;
  }
  goto Failed;
 Node74:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '"' || text[pos] == '\'' )
  {
    quote = text[pos];
    ++pos;
    goto Node75;
  }
  if ( text[pos] == '>' )
  {
    if ( consumer )
    {
      if ( type == Element )
	if ( !consumer->element( text.mid( start, pos - start ) ) )
	  return pos;
      if ( type == Attlist )
	if ( !consumer->attlist( text.mid( start, pos - start ) ) )
	  return pos;
    }
    ++pos;
    goto Node65;
  }
  ++pos;
  goto Node74;
 Node75:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == quote )
  {
    ++pos;
    goto Node74;
  }
  goto Node75;
 Node76: // Entity
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node77;
  }
  goto Failed;
 Node77:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node77;
  }
  if ( text[pos] == '%' )
  {
    if ( type != Entity )
      goto Failed;
    parameter_entity = TRUE;
    start = ++pos;
    goto Node78;
  }
  if ( text[pos].isLetter() || text[pos] == ':' || text[pos] == '_' )
  {
    parameter_entity = FALSE;
    start = pos++;
    goto Node100;
  }
  goto Failed;
 Node78: // Parameter entity decl
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    start = ++pos;
    goto Node79;
  }
  goto Failed;
 Node79:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isLetter() || text[pos] == ':' || text[pos] == '_' )
  {
    start = pos++;
    goto Node100;
  }
  if ( text[pos].isSpace() )
  {
    ++pos;
    goto Node79;
  }
  goto Failed;
 Node84:
  if ( pos == len )
    goto Failed;
  if ( !text[pos].isSpace() )
    goto Failed;
  ++pos;
  goto Node85;
 Node85:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node85;
  }
  if ( text[pos] == '"' || text[pos] == '\'' )
  {
    quote = text[pos];
    start = ++pos;
    goto Node86;
  }
  goto Failed;
 Node86:
  if ( pos == len )
    goto Failed;
  if ( text[pos] != quote )
  {
    ++pos;
    goto Node86;
  }
  publicId = text.mid( start, pos - start );
  ++pos;
  goto Node87;
 Node87:
  if ( pos == len )
    goto Failed;
  if ( type == Notation && text[pos] == '>' )
  {
    if ( consumer )
      if ( !consumer->notation( tag, publicId, QString::null ) )
	return pos;
    ++pos;
    goto Node65;
  }
  if ( !text[pos].isSpace() )
    goto Failed;
  ++pos;
  goto Node88;
 Node88:
  if ( pos == len )
    goto Failed;
  if ( type == Notation && text[pos] == '>' )
  {
    if ( consumer )
      if ( !consumer->notation( tag, publicId, QString::null ) )
	return pos;
    ++pos;
    goto Node65;
  }
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node88;
  }
  if ( text[pos] == '"' || text[pos] == '\'' )
  {
    quote = text[pos];
    start = ++pos;
    goto Node89;
  }
  goto Failed;
 Node89:
  if ( pos == len )
    goto Failed;
  if ( text[pos] != quote )
  {
    ++pos;
    goto Node89;
  }
  systemId = text.mid( start, pos - start );
  ++pos;
  goto Node90;
 Node90:
  if ( pos == len )
    goto Failed;
  if ( text[pos] == '>' )
  {
    if ( type == Entity && parameter_entity )
    {
      if ( !parameterEntity( tag, publicId, systemId ) )
	return pos;
    }
    else if ( type == Entity )
    {
      if ( !entity( tag, publicId, systemId, QString::null ) )
	return pos;
    }
    else if ( type == Notation )
    {
      if ( consumer )
	if ( !consumer->notation( tag, publicId, systemId ) )
	  return pos;
    }
    else
      qFatal("QXMLSimpleParser: Internal error in Node90");
    ++pos;
    goto Node65;
  }
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node91;
  }
  goto Failed;
 Node91:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node91;
  }
  if ( text[pos] == '>' )
  {
    ++pos;
    if ( type == Entity && parameter_entity )
    {
      if ( !consumer->parameterEntity( tag, publicId, systemId ) )
	return pos;
    }
    else if ( type == Entity )
    {
      if ( !entity( tag, publicId, systemId, QString::null ) )
	return pos;
    }
    else if ( type == Notation )
    {
      if ( consumer )
	if ( !consumer->notation( tag, publicId, systemId ) )
	  return pos;
    }
    else
      qFatal("QXMLSimpleParser: Internal error in Node91");
    goto Node65;
  }
  if ( pos + 5 < len && text[pos] == 'N' && text[pos+1] == 'D' && text[pos+2] == 'A' && text[pos+3] == 'T' &&
	    text[pos+4] == 'A' )
  {
    // Only <!ENTITY name .... > may have an NDATA section
    if ( parameter_entity || type != Entity )
      goto Failed;
    pos += 5;
    goto Node92;
  };
  goto Failed;
 Node92:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node93;
  }
  goto Failed;
 Node93:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isLetter() || text[pos] == ':' || text[pos] == '_' )
  {
    start = pos++;
    goto Node94;
  }
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node91;
  }
  goto Failed;
 Node94:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isLetter() || text[pos] == ':' || text[pos] == '_' || text[pos] == '-' || text[pos] == '.' )
  {
    ++pos;
    goto Node94;
  }
  if ( text[pos].isSpace() )
  { 
    if ( !entity( tag, publicId, systemId, text.mid( start, pos - start ) ) )
      return pos;
    ++pos;
    goto Node95;
  }
  if ( text[pos] == '>' )
  { 
    if ( !entity( tag, publicId, systemId, text.mid( start, pos - start ) ) )
	return pos;
    ++pos;
    goto Node65;
  }
  goto Failed;
 Node95:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node95;
  }
  if ( text[pos] == '>' )
  { 
    ++pos;
    goto Node65;
  }
  goto Failed;
 Node96:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    publicId = QString::null;
    ++pos;
    goto Node88;
  }
  goto Failed;
 Node100: // Value entity
  if ( pos == len )
    goto Failed;
  if ( text[pos].isLetter() || text[pos] == ':' || text[pos] == '_' || text[pos] == '-' || text[pos] == '.' )
  {
    ++pos;
    goto Node100;
  }
  if ( text[pos].isSpace() )
  { 
    tag = text.mid( start, pos - start );
    ++pos;
    goto Node101;
  }
  goto Failed;
 Node101:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node101;
  }
  if ( text[pos] == '"' || text[pos] == '\'' )
  { 
    // No string value allowed in <!NOTATION>
    if ( type == Notation )
      goto Failed;
    quote = text[pos];
    start = ++pos;
    goto Node102;
  }
  if ( pos + 6 < len && text[pos] == 'P' && text[pos+1] == 'U' && text[pos+2] == 'B' && text[pos+3] == 'L' &&
	    text[pos+4] == 'I' && text[pos+5] == 'C')
  {
    pos += 6;
    goto Node84;
  };
  if ( pos + 6 < len && text[pos] == 'S' && text[pos+1] == 'Y' && text[pos+2] == 'S' && text[pos+3] == 'T' &&
	    text[pos+4] == 'E' && text[pos+5] == 'M')
  {
    pos += 6;
    goto Node96;
  };
  goto Failed;
 Node102:
  if ( pos == len )
    goto Failed;
  if ( text[pos] != quote )
  {
    ++pos;
    goto Node102;
  }
  if ( parameter_entity )
  {
    if ( !parameterEntity( tag, text.mid( start, pos - start ) ) )
      return pos;
  }
  else
    {
      if ( !entity( tag, text.mid( start, pos - start ) ) )
	return pos;
  }
  ++pos;
  goto Node103;
 Node103:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node103;
  }
  if ( text[pos] == '>' )
  { 
    ++pos;
    goto Node65;
  }
  goto Failed;
 Node110:
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node110;
  }
  if ( pos + 7 < len && text[pos] == 'I' && text[pos+1] == 'N' && text[pos+2] == 'C' && text[pos+3] == 'L' &&
	    text[pos+4] == 'U' && text[pos+5] == 'D' && text[pos+6] == 'E' )
  {
    pos += 7;
    goto Node111;
  };
  if ( pos + 6 < len && text[pos] == 'I' && text[pos+1] == 'G' && text[pos+2] == 'N' && text[pos+3] == 'O' &&
	    text[pos+4] == 'R' && text[pos+5] == 'E' )
  {
    pos += 6;
    goto Node112;
  };
  goto Failed;
 Node111:
  include_counter++;
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node111;
  }
  if ( text[pos] == '[' )
  { 
    ++pos;
    goto Node65;
  }
  goto Failed;
 Node112:
  ignore_counter++;
  if ( pos == len )
    goto Failed;
  if ( text[pos].isSpace() )
  { 
    ++pos;
    goto Node112;
  }
  if ( text[pos] == '[' )
  { 
    ++pos;
    goto Node113;
  }
  goto Failed;
 Node113:
  if ( pos == len )
    goto Failed;
  if ( pos + 3 < len && text[pos] == ']' && text[pos+1] == ']' && text[pos+2] == '>' )
  {
    pos += 3;
    ignore_counter--;
    if ( ignore_counter == 0 )
      goto Node65;
    goto Node113;
  }
  if ( pos + 3 < len && text[pos] == '<' && text[pos+1] == '!' && text[pos+2] == '[' )
  {
    pos += 3;
    ignore_counter++;
    goto Node113;
  }
  ++pos;
  goto Node113;

 Ok:
  if ( consumer )
    if ( !consumer->finished() )
      return pos;
  return -1;
 Failed:
  if ( consumer )
    consumer->parseError( pos );
  return pos;
}
