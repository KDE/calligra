/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#if !defined(__CHART_H__) || defined(MICO_NO_TOPLEVEL_MODULES)
#define __CHART_H__

#ifndef MICO_NO_TOPLEVEL_MODULES
#include <CORBA.h>
#include <mico/throw.h>
#endif

#ifdef MICO_IN_GENERATED_CODE
#include <kom.h>
#else
#define MICO_IN_GENERATED_CODE
#include <kom.h>
#undef MICO_IN_GENERATED_CODE
#endif

#ifndef MICO_NO_TOPLEVEL_MODULES
MICO_NAMESPACE_DECL Chart {
#endif

#if !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE_Chart)


typedef SequenceTmpl<CORBA::Double> DoubleSeq;
#ifdef _WINDOWS
static DoubleSeq _dummy_DoubleSeq;
#endif
typedef TSeqVar<SequenceTmpl<CORBA::Double> > DoubleSeq_var;
typedef DoubleSeq_var DoubleSeq_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_DoubleSeq;

typedef SequenceTmpl<CORBA::String_var> StringSeq;
#ifdef _WINDOWS
static StringSeq _dummy_StringSeq;
#endif
typedef TSeqVar<SequenceTmpl<CORBA::String_var> > StringSeq_var;
typedef StringSeq_var StringSeq_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_StringSeq;

struct Matrix {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  Matrix();
  ~Matrix();
  Matrix( const Matrix& s );
  Matrix& operator=( const Matrix& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  CORBA::Long columns;
  CORBA::Long rows;
  StringSeq columnDescription;
  StringSeq rowDescription;
  DoubleSeq matrix;
};

typedef TVarVar<Matrix> Matrix_var;
typedef Matrix_var Matrix_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Matrix;

struct Range {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  Range();
  ~Range();
  Range( const Range& s );
  Range& operator=( const Range& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  CORBA::Long top;
  CORBA::Long left;
  CORBA::Long bottom;
  CORBA::Long right;
  CORBA::String_var table;
};

typedef TVarVar<Range> Range_var;
typedef Range_var Range_out;

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_Range;

class SimpleChart;
typedef SimpleChart *SimpleChart_ptr;
typedef SimpleChart_ptr SimpleChartRef;
typedef ObjVar<SimpleChart> SimpleChart_var;
typedef SimpleChart_var SimpleChart_out;


// Common definitions for interface SimpleChart
class SimpleChart : virtual public CORBA::Object
{
  public:
    virtual ~SimpleChart();
    static SimpleChart_ptr _duplicate( SimpleChart_ptr obj );
    static SimpleChart_ptr _narrow( CORBA::Object_ptr obj );
    static SimpleChart_ptr _nil();

    virtual void *_narrow_helper( const char *repoid );
    static vector<CORBA::Narrow_proto> *_narrow_helpers;
    static bool _narrow_helper2( CORBA::Object_ptr obj );

    virtual void fill( const Range& range, const Matrix& matrix ) = 0;
    virtual void showWizard() = 0;
  protected:
    SimpleChart() {};
  private:
    SimpleChart( const SimpleChart& );
    void operator=( const SimpleChart& );
};

MICO_EXPORT_VAR_DECL CORBA::TypeCodeConst _tc_SimpleChart;

// Stub for interface SimpleChart
class SimpleChart_stub : virtual public SimpleChart
{
  public:
    virtual ~SimpleChart_stub();
    void fill( const Range& range, const Matrix& matrix );
    void showWizard();
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

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::Chart::Matrix &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Chart::Matrix &_s );

CORBA::Boolean operator<<=( CORBA::Any &_a, const ::Chart::Range &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::Chart::Range &_s );

CORBA::Boolean operator<<=( CORBA::Any &a, const Chart::SimpleChart_ptr obj );
CORBA::Boolean operator>>=( const CORBA::Any &a, Chart::SimpleChart_ptr &obj );

CORBA::Boolean operator<<=( CORBA::Any &_a, const SequenceTmpl<CORBA::Double> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl<CORBA::Double> &_s );

#endif // !defined(MICO_NO_TOPLEVEL_MODULES) || defined(MICO_MODULE__GLOBAL)


#if !defined(MICO_NO_TOPLEVEL_MODULES) && !defined(MICO_IN_GENERATED_CODE)
#include <mico/template_impl.h>
#endif

#endif
