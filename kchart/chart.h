/*
 *  MICO --- a CORBA 2.0 implementation
 *  Copyright (C) 1997, 1998 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__CHART_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __CHART_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

// Generate forward declarations for this scope

#include "/usr/local/KDE/include/idl/kom.h"

// Module Chart
#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL Chart {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_Chart)

// Generate forward declarations for this scope
class SimpleChart;
typedef SimpleChart *SimpleChart_ptr;
typedef SimpleChart_ptr SimpleChartRef;
typedef ObjVar<SimpleChart> SimpleChart_var;


typedef SequenceTmpl<CORBA::Double> DoubleSeq;
typedef TSeqVar<SequenceTmpl<CORBA::Double> > DoubleSeq_var;

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_DoubleSeq;

typedef SequenceTmpl<CORBA::String_var> StringSeq;
typedef TSeqVar<SequenceTmpl<CORBA::String_var> > StringSeq_var;

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_StringSeq;

struct Matrix {
  Matrix();
  ~Matrix();
  Matrix( const Matrix& s );
  Matrix& operator=( const Matrix& s );
  CORBA::Long columns;
  CORBA::Long rows;
  Chart::StringSeq columnDescription;
  Chart::StringSeq rowDescription;
  Chart::DoubleSeq matrix;
};

typedef TVar<Matrix> Matrix_var;

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_Matrix;

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

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_Range;


// Common definitions for interface SimpleChart
class SimpleChart : virtual public CORBA::Object
{
  public:
    virtual ~SimpleChart();
    static SimpleChart_ptr _duplicate( SimpleChart_ptr obj );
    static SimpleChart_ptr _narrow( CORBA::Object_ptr obj );
    static SimpleChart_ptr _nil();

    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    virtual void *_narrow_helper( const char *repoid );
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void fill( const Chart::Range& range, const Chart::Matrix& matrix ) = 0;
  protected:
    SimpleChart() {};
  private:
    SimpleChart( const SimpleChart& );
    void operator=( const SimpleChart& );
};

MICO_EXPORT_DECL CORBA::TypeCode_ptr _tc_SimpleChart;

// Stub for interface SimpleChart
class SimpleChart_stub : virtual public SimpleChart
{
  public:
    virtual ~SimpleChart_stub();
    void fill( const Chart::Range& range, const Chart::Matrix& matrix );
  private:
    void operator=( const SimpleChart_stub& );
};

class SimpleChart_skel :
  virtual public MethodDispatcher,
  virtual public SimpleChart
{
  public:
    SimpleChart_skel( const CORBA::BOA::ReferenceData & = CORBA::BOA::ReferenceData() );
    virtual ~SimpleChart_skel();
    SimpleChart_skel( CORBA::Object_ptr obj );
    virtual bool dispatch( CORBA::ServerRequest_ptr _req, CORBA::Environment &_env );
    SimpleChart_ptr _this();

};

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_Chart)

#ifndef MICO_NO_TOPLEVEL_MODULES

};
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CORBA::Double> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CORBA::Double> &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CORBA::String_var> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CORBA::String_var> &_s );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::Chart::Matrix &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Chart::Matrix &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::Chart::Range &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Chart::Range &_s );

CORBA::Boolean operator<<=( CORBA::Any &a, const Chart::SimpleChart_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, Chart::SimpleChart_ptr &obj );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#ifndef MICO_NO_TOPLEVEL_MODULES
#include <mico/template_impl.h>
#endif

#endif
