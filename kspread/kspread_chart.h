/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__KXCL_CHART_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __KXCL_CHART_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

// Generate forward declarations for this scope

// Module CHART
#ifndef MICO_NO_TOPLEVEL_MODULES
struct CHART {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_CHART)

// Generate forward declarations for this scope
class Callback;
typedef Callback *Callback_ptr;
typedef Callback_ptr CallbackRef;
typedef ObjVar<Callback> Callback_var;

class Chart;
typedef Chart *Chart_ptr;
typedef Chart_ptr ChartRef;
typedef ObjVar<Chart> Chart_var;


typedef SequenceTmpl<CORBA::Double> DoubleSeq;
typedef TSeqVar<SequenceTmpl<CORBA::Double> > DoubleSeq_var;

static CORBA::TypeCode_ptr _tc_DoubleSeq;

typedef SequenceTmpl<CORBA::String_var> StringSeq;
typedef TSeqVar<SequenceTmpl<CORBA::String_var> > StringSeq_var;

static CORBA::TypeCode_ptr _tc_StringSeq;

struct Matrix {
  Matrix();
  ~Matrix();
  Matrix( const Matrix& s );
  Matrix& operator=( const Matrix& s );
  CORBA::Long columns;
  CORBA::Long rows;
  CHART::StringSeq columnDescription;
  CHART::StringSeq rowDescription;
  CHART::DoubleSeq matrix;
};

typedef TVar<Matrix> Matrix_var;

static CORBA::TypeCode_ptr _tc_Matrix;

struct Range {
  Range();
  ~Range();
  Range( const Range& s );
  Range& operator=( const Range& s );
  CORBA::Long top;
  CORBA::Long left;
  CORBA::Long bottom;
  CORBA::Long right;
  CORBA::String_var table;
};

typedef TVar<Range> Range_var;

static CORBA::TypeCode_ptr _tc_Range;


// Common definitions for interface Callback
class Callback : virtual public CORBA::Object
{
  public:
    virtual ~Callback();
    static Callback_ptr _duplicate( Callback_ptr obj );
    static Callback_ptr _narrow( CORBA::Object_ptr obj );
    static Callback_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual CHART::Matrix* request( const CHART::Range& range ) = 0;
  protected:
    Callback() {};
  private:
    Callback( const Callback& );
    void operator=( const Callback& );
};

static CORBA::TypeCode_ptr _tc_Callback;

// Stub for interface Callback
class Callback_stub : virtual public Callback
{
  public:
    virtual ~Callback_stub();
    CHART::Matrix* request( const CHART::Range& range );
  private:
    void operator=( const Callback_stub& );
};

class Callback_skel :
  virtual public MethodDispatcher,
  virtual public Callback
{
  public:
    Callback_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Callback_skel();
    Callback_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Callback_ptr _this();

};


// Common definitions for interface Chart
class Chart : virtual public CORBA::Object
{
  public:
    virtual ~Chart();
    static Chart_ptr _duplicate( Chart_ptr obj );
    static Chart_ptr _narrow( CORBA::Object_ptr obj );
    static Chart_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void fill( const CHART::Range& range, const CHART::Matrix& matrix, CHART::Callback_ptr cb ) = 0;
    virtual void fill_dummy( const CHART::Range& range, const CHART::Matrix& matrix ) = 0;
  protected:
    Chart() {};
  private:
    Chart( const Chart& );
    void operator=( const Chart& );
};

static CORBA::TypeCode_ptr _tc_Chart;

// Stub for interface Chart
class Chart_stub : virtual public Chart
{
  public:
    virtual ~Chart_stub();
    void fill( const CHART::Range& range, const CHART::Matrix& matrix, CHART::Callback_ptr cb );
    void fill_dummy( const CHART::Range& range, const CHART::Matrix& matrix );
  private:
    void operator=( const Chart_stub& );
};

class Chart_skel :
  virtual public MethodDispatcher,
  virtual public Chart
{
  public:
    Chart_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~Chart_skel();
    Chart_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    Chart_ptr _this();

};

#endif !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_CHART)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CORBA::Double> &_s );

CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CORBA::Double> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CORBA::String_var> &_s );

CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CORBA::String_var> &_s );

#endif !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CHART::Matrix &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CHART::Matrix &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::CHART::Range &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::CHART::Range &_s );

CORBA::Boolean operator<<=( CORBA::Any &a, const CHART::Callback_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CHART::Callback_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &a, const CHART::Chart_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, CHART::Chart_ptr &obj );

#endif !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#ifndef MICO_NO_TOPLEVEL_MODULES
#include <mico/template_impl.h>
#endif

#endif
