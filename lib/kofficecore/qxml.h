/****************************************************************************
** $Id$
**
** Definition of QXML classes
*
** Created : 980128
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
#ifndef QXML_H
#define QXML_H

#include "qstring.h"

class QXMLConsumer
{
public:
  QXMLConsumer();
  virtual ~QXMLConsumer();

  virtual bool tagStart( const QString& name ) = 0;
  virtual bool tagEnd( const QString& name ) = 0;
  virtual bool attrib( const QString& name, const QString& value ) = 0;
  virtual bool text( const QString& text ) = 0;
  virtual bool entityRef( const QString& name ) = 0;
  virtual bool processingInstruction( const QString& name, const QString& value ) = 0;
  virtual bool doctype( const QString& name ) = 0;
  virtual bool doctypeExtern( const QString& publicId, const QString& systemId ) = 0;
  // virtual bool parameterEntityRef( const QString& name ) = 0;
  virtual bool element( const QString& data ) = 0;
  virtual bool attlist( const QString& data ) = 0;
  virtual bool parameterEntity( const QString& name, const QString& publicId, const QString& systemId ) = 0;
  virtual bool parameterEntity( const QString& name, const QString& value ) = 0;
  virtual bool entity( const QString& name, const QString& publicId, const QString& systemId, const QString& ndata ) = 0;
  virtual bool entity( const QString& name, const QString& value ) = 0;
  virtual bool notation( const QString& name, const QString& publicId, const QString& systemId ) = 0;
  virtual void parseError( int pos ) = 0;
  virtual bool finished() = 0;
};

class QXMLSimpleParserPrivate;

class QXMLSimpleParser
{
public:
  enum Type { Element, Attlist, Entity, Notation };

  QXMLSimpleParser();
  virtual ~QXMLSimpleParser();

  int parse( QString _text, QXMLConsumer* = 0 );

private:
  bool parameterEntity( const QString& name, const QString& publicId, const QString& systemId );
  bool parameterEntity( const QString& name, const QString& value );
  bool paramEntityValue( const QString& name, QString* value );
  bool entity( const QString& name, const QString& publicId, const QString& systemId, const QString& ndata );
  bool entity( const QString& name, const QString& value );
  bool entityValue( const QString& name, QString* value );
  bool replaceCharRefs( const QString& x, QString* text );

  QXMLSimpleParserPrivate* d;
};

class QXMLValidatingParserPrivate;

class QXMLValidatingParser
{
public:
  QXMLValidatingParser() { d = 0; };
  virtual ~QXMLValidatingParser() { };

  int parse( QString, QXMLConsumer* ) { ASSERT( 0 ); return 0; }

private:
  QXMLValidatingParserPrivate* d;
};

#endif
