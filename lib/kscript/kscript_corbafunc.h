/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#if 0

#ifndef __KSCRIPT_CORBA_FUNC_H__
#define __KSCRIPT_CORBA_FUNC_H__

#include <qvaluelist.h>
#include <qstring.h>

#include "kscript_typecode.h"
#include "kscript_func.h"
#include "kscript_context.h"

class KSCorbaFunc : public KSFunction
{
public:
  enum ParameterMode { T_IN, T_OUT, T_INOUT };

  typedef KSSharedPtr<KSCorbaFunction> Ptr;

  KSCorbaFunc( KSModule* m, KSParseNode* );
  virtual ~KSCorbaFunc() { }

  virtual bool call( KSContext& context );
  virtual bool isSignal() const { return false; }
  virtual QString name() const { return m_name; }

  void addParameter( ParameterMode, const QString&, const KSTypeCode::Ptr & );
  void addException( const KSValue::Ptr& );
  void setReturnTypeCode( const KSTypeCode::Ptr& );

private:
  bool init( KSContext& context );

  struct Parameter
  {
    ParameterMode mode;
    QString name;
    KSTypeCode::Ptr typecode;
  };

  QValueList<KSValue::Ptr> m_exceptions;
  QValueList<Parameter> m_parameters;
  KSTypeCode::Ptr m_returnTypeCode;
  QString m_name;
  KSParseNode* m_node;
};

#endif

#endif
