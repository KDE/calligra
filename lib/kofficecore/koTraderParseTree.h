/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __parse_tree_h__
#define __parse_tree_h__

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qshared.h>

#include <kservices.h>
#include <kuserprofile.h>

#include "koTrader.h"

class ParseTreeBase;

struct PreferencesReturn
{
  enum Type { PRT_DOUBLE, PRT_ERROR };

  PreferencesReturn() { type = PRT_ERROR; }

  PreferencesReturn( const PreferencesReturn& _r )
  {
    type = _r.type;
    f = _r.f;
  }

  Type type;
  double f;
};


/**
 * @return 0  => Does not match
 *         1  => Does match
 *         <0 => Error
 */
int matchConstraint( const ParseTreeBase *_tree, const KService*,
		     const KServiceTypeProfile::OfferList& );

/**
 * @return 1 on success or <0 on Error
 */
PreferencesReturn matchPreferences( const ParseTreeBase *_tree, const KService*,
				    const KServiceTypeProfile::OfferList& );

struct PreferencesMaxima
{
  enum Type { PM_ERROR, PM_INVALID_INT, PM_INVALID_DOUBLE, PM_DOUBLE, PM_INT };

  Type type;
  int iMax;
  int iMin;
  double fMax;
  double fMin;
};

class ParseContext
{
public:
  /**
   * This is NOT a copy constructor.
   */
  ParseContext( const ParseContext* _ctx ) : service( _ctx->service ), maxima( _ctx->maxima ),
    offers( _ctx->offers ) {}
  ParseContext( const KService* _service, const KServiceTypeProfile::OfferList& _offers,
		QMap<QString,PreferencesMaxima>& _m )
    : service( _service ), maxima( _m ), offers( _offers ) {}

  bool initMaxima( const QString& _prop);

  enum Type { T_STRING = 1, T_DOUBLE = 2, T_NUM = 3, T_BOOL = 4, T_NUM_SEQ = 5,
	      T_STR_SEQ = 6, T_DOUBLE_SEQ = 7 };

  QString str;
  int i;
  double f;
  bool b;
  QValueList<int> numSeq;
  QValueList<double> doubleSeq;
  QStringList strSeq;
  Type type;

  const KService* service;

  QMap<QString,PreferencesMaxima>& maxima;
  const KServiceTypeProfile::OfferList& offers;
};

class ParseTreeBase : public KShared
{
  typedef KSharedPtr<ParseTreeBase> Ptr;
public:
  ParseTreeBase() { }

  virtual bool eval( ParseContext *_context ) const = 0;
protected:
  virtual ~ParseTreeBase() { };
};

ParseTreeBase::Ptr parseConstraints( const QString& _constr );
ParseTreeBase::Ptr parsePreferences( const QString& _prefs );

class ParseTreeOR : public ParseTreeBase
{
public:
  ParseTreeOR( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2 ) { m_pLeft = _ptr1; m_pRight = _ptr2; }

  bool eval( ParseContext *_context ) const;

protected:
  ParseTreeBase::Ptr m_pLeft;
  ParseTreeBase::Ptr m_pRight;
};

class ParseTreeAND : public ParseTreeBase
{
public:
  ParseTreeAND( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2 ) { m_pLeft = _ptr1; m_pRight = _ptr2; }

  bool eval( ParseContext *_context ) const;

protected:
  ParseTreeBase::Ptr m_pLeft;
  ParseTreeBase::Ptr m_pRight;
};

class ParseTreeCMP : public ParseTreeBase
{
public:
  ParseTreeCMP( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2, int _i ) { m_pLeft = _ptr1; m_pRight = _ptr2; m_cmd = _i; }

  bool eval( ParseContext *_context ) const;

protected:
  ParseTreeBase::Ptr m_pLeft;
  ParseTreeBase::Ptr m_pRight;
  int m_cmd;
};

class ParseTreeIN : public ParseTreeBase
{
public:
  ParseTreeIN( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2 ) { m_pLeft = _ptr1; m_pRight = _ptr2; }

  bool eval( ParseContext *_context ) const;

protected:
  ParseTreeBase::Ptr m_pLeft;
  ParseTreeBase::Ptr m_pRight;
};

class ParseTreeMATCH : public ParseTreeBase
{
public:
  ParseTreeMATCH( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2 ) { m_pLeft = _ptr1; m_pRight = _ptr2; }

  bool eval( ParseContext *_context ) const;

protected:
  ParseTreeBase::Ptr m_pLeft;
  ParseTreeBase::Ptr m_pRight;
};

class ParseTreeCALC : public ParseTreeBase
{
public:
  ParseTreeCALC( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2, int _i ) { m_pLeft = _ptr1; m_pRight = _ptr2; m_cmd = _i; }

  bool eval( ParseContext *_context ) const;

protected:
  ParseTreeBase::Ptr m_pLeft;
  ParseTreeBase::Ptr m_pRight;
  int m_cmd;
};

class ParseTreeBRACKETS : public ParseTreeBase
{
public:
  ParseTreeBRACKETS( ParseTreeBase *_ptr ) { m_pLeft = _ptr; }

  bool eval( ParseContext *_context ) const { return m_pLeft->eval( _context ); }

protected:
  ParseTreeBase::Ptr m_pLeft;
};

class ParseTreeNOT : public ParseTreeBase
{
public:
  ParseTreeNOT( ParseTreeBase *_ptr ) { m_pLeft = _ptr; }

  bool eval( ParseContext *_context ) const;

protected:
  ParseTreeBase::Ptr m_pLeft;
};

class ParseTreeEXIST : public ParseTreeBase
{
public:
  ParseTreeEXIST( const char *_id ) { m_id = _id; }

  bool eval( ParseContext *_context ) const;

protected:
  QString m_id;
};

class ParseTreeID : public ParseTreeBase
{
public:
  ParseTreeID( const char *arg ) { m_str = arg; }

  bool eval( ParseContext *_context ) const;

protected:
  QString m_str;
};

class ParseTreeSTRING : public ParseTreeBase
{
public:
  ParseTreeSTRING( const char *arg ) { m_str = arg; }

  bool eval( ParseContext *_context ) const { _context->type = ParseContext::T_STRING; _context->str = m_str; return true; }

protected:
  QString m_str;
};

class ParseTreeNUM : public ParseTreeBase
{
public:
  ParseTreeNUM( int arg ) { m_int = arg; }

  bool eval( ParseContext *_context ) const { _context->type = ParseContext::T_NUM; _context->i = m_int; return true; }

protected:
  int m_int;
};

class ParseTreeDOUBLE : public ParseTreeBase
{
public:
  ParseTreeDOUBLE( double arg ) { m_double = arg; }

  bool eval( ParseContext *_context ) const { _context->type = ParseContext::T_DOUBLE; _context->f = m_double; return true; }

protected:
  double m_double;
};

class ParseTreeBOOL : public ParseTreeBase
{
public:
  ParseTreeBOOL( bool arg ) { m_bool = arg; }

  bool eval( ParseContext *_context ) const { _context->type = ParseContext::T_BOOL; _context->b = m_bool; return true; }

protected:
  bool m_bool;
};

class ParseTreeMAX2 : public ParseTreeBase
{
public:
  ParseTreeMAX2( const char *_id ) { m_strId = _id; }

  bool eval( ParseContext *_context ) const;

protected:
  QString m_strId;
};

class ParseTreeMIN2 : public ParseTreeBase
{
public:
  ParseTreeMIN2( const char *_id ) { m_strId = _id; }

  bool eval( ParseContext *_context ) const;

protected:
  QString m_strId;
};

#endif


