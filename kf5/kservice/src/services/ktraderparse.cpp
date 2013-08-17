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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// TODO: Torben: On error free memory!   (r932881 might serve as inspiration)

extern "C"
{
#include "ktraderparse_p.h"

void KTraderParse_mainParse( const char *_code );
}

#include "ktraderparsetree_p.h"

#include <assert.h>
#include <stdlib.h>

#include <QThreadStorage>
#include <QDebug>

namespace KTraderParse
{

struct ParsingData
{
    ParseTreeBase::Ptr ptr;
    QByteArray buffer;
};

}

using namespace KTraderParse;

Q_GLOBAL_STATIC(QThreadStorage<ParsingData *>, s_parsingData)

ParseTreeBase::Ptr KTraderParse::parseConstraints( const QString& _constr )
{
    ParsingData *data = new ParsingData();
    s_parsingData()->setLocalData(data);
    data->buffer = _constr.toUtf8();
    KTraderParse_mainParse(data->buffer.constData());
    ParseTreeBase::Ptr ret = data->ptr;
    s_parsingData()->setLocalData(0);
    return ret;
}

void KTraderParse_setParseTree( void *_ptr1 )
{
    ParsingData *data = s_parsingData()->localData();
    data->ptr = static_cast<ParseTreeBase*>(_ptr1);
}


void KTraderParse_error( const char* err )
{
    qWarning() << "Parsing" << s_parsingData()->localData()->buffer << "gave:" << err;
}

void* KTraderParse_newOR( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeOR( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* KTraderParse_newAND( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeAND( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* KTraderParse_newCMP( void *_ptr1, void *_ptr2, int _i )
{
  return new ParseTreeCMP( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2, _i );
}

void* KTraderParse_newIN( void *_ptr1, void *_ptr2, int _cs )
{
  return new ParseTreeIN( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2, _cs == 1 ? Qt::CaseSensitive : Qt::CaseInsensitive );
}

void* KTraderParse_newSubstringIN( void *_ptr1, void *_ptr2, int _cs )
{
  return new ParseTreeIN((ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2,
                          _cs == 1 ? Qt::CaseSensitive : Qt::CaseInsensitive, true);
}

void* KTraderParse_newMATCH( void *_ptr1, void *_ptr2, int _cs )
{
  return new ParseTreeMATCH( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2, _cs == 1 ? Qt::CaseSensitive : Qt::CaseInsensitive );
}

void* KTraderParse_newCALC( void *_ptr1, void *_ptr2, int _i )
{
  return new ParseTreeCALC( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2, _i );
}

void* KTraderParse_newBRACKETS( void *_ptr1 )
{
  return new ParseTreeBRACKETS( (ParseTreeBase*)_ptr1 );
}

void* KTraderParse_newNOT( void *_ptr1 )
{
  return new ParseTreeNOT( (ParseTreeBase*)_ptr1 );
}

void* KTraderParse_newEXIST( char *_ptr1 )
{
  ParseTreeEXIST *t = new ParseTreeEXIST( _ptr1 );
  free(_ptr1);
  return t;
}

void* KTraderParse_newID( char *_ptr1 )
{
  ParseTreeID *t = new ParseTreeID( _ptr1 );
  free(_ptr1);
  return t;
}

void* KTraderParse_newSTRING( char *_ptr1 )
{
  ParseTreeSTRING *t = new ParseTreeSTRING( _ptr1 );
  free(_ptr1);
  return t;
}

void* KTraderParse_newNUM( int _i )
{
  return new ParseTreeNUM( _i );
}

void* KTraderParse_newFLOAT( float _f )
{
  return new ParseTreeDOUBLE( _f );
}

void* KTraderParse_newBOOL( char _b )
{
  return new ParseTreeBOOL( (bool)_b );
}

void* KTraderParse_newMAX2( char *_id )
{
  ParseTreeMAX2 *t = new ParseTreeMAX2( _id );
  free(_id);
  return t;
}

void* KTraderParse_newMIN2( char *_id )
{
  ParseTreeMIN2 *t = new ParseTreeMIN2( _id );
  free(_id);
  return t;
}

void KTraderParse_destroy(void *node)
{
    ParsingData *data = s_parsingData()->localData();
    ParseTreeBase *p = reinterpret_cast<ParseTreeBase *>(node);
    if (p != data->ptr) {
        delete p;
    }
}

