/*
 *  MICO --- a free CORBA implementation
 *  Copyright (C) 1997-98 Kay Roemer & Arno Puder
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <chart.h>

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------
#ifdef HAVE_NAMESPACE
namespace Chart { CORBA::TypeCodeConst _tc_DoubleSeq; };
#else
CORBA::TypeCodeConst Chart::_tc_DoubleSeq;
#endif

#ifdef HAVE_NAMESPACE
namespace Chart { CORBA::TypeCodeConst _tc_StringSeq; };
#else
CORBA::TypeCodeConst Chart::_tc_StringSeq;
#endif

#ifdef HAVE_NAMESPACE
namespace Chart { CORBA::TypeCodeConst _tc_Matrix; };
#else
CORBA::TypeCodeConst Chart::_tc_Matrix;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
Chart::Matrix::Matrix()
{
}

Chart::Matrix::Matrix( const Matrix& _s )
{
  columns = ((Matrix&)_s).columns;
  rows = ((Matrix&)_s).rows;
  columnDescription = ((Matrix&)_s).columnDescription;
  rowDescription = ((Matrix&)_s).rowDescription;
  matrix = ((Matrix&)_s).matrix;
}

Chart::Matrix::~Matrix()
{
}

Chart::Matrix&
Chart::Matrix::operator=( const Matrix& _s )
{
  columns = ((Matrix&)_s).columns;
  rows = ((Matrix&)_s).rows;
  columnDescription = ((Matrix&)_s).columnDescription;
  rowDescription = ((Matrix&)_s).rowDescription;
  matrix = ((Matrix&)_s).matrix;
  return *this;
}
#endif

void operator<<=( CORBA::Any &_a, const Chart::Matrix &_s )
{
  CORBA::StaticAny _sa (_marshaller_Chart_Matrix, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, Chart::Matrix *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Chart::Matrix &_s )
{
  CORBA::StaticAny _sa (_marshaller_Chart_Matrix, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Chart::Matrix *&_s )
{
  return _a.to_static_any (_marshaller_Chart_Matrix, (void *&)_s);
}

class _Marshaller_Chart_Matrix : public CORBA::StaticTypeInfo {
    typedef Chart::Matrix _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Chart_Matrix::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_Chart_Matrix::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_Chart_Matrix::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Chart_Matrix::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->columns ) &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->rows ) &&
    CORBA::_stcseq_string->demarshal( dc, &((_MICO_T*)v)->columnDescription ) &&
    CORBA::_stcseq_string->demarshal( dc, &((_MICO_T*)v)->rowDescription ) &&
    CORBA::_stcseq_double->demarshal( dc, &((_MICO_T*)v)->matrix ) &&
    dc.struct_end();
}

void _Marshaller_Chart_Matrix::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->columns );
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->rows );
  CORBA::_stcseq_string->marshal( ec, &((_MICO_T*)v)->columnDescription );
  CORBA::_stcseq_string->marshal( ec, &((_MICO_T*)v)->rowDescription );
  CORBA::_stcseq_double->marshal( ec, &((_MICO_T*)v)->matrix );
  ec.struct_end();
}

CORBA::TypeCode_ptr _Marshaller_Chart_Matrix::typecode()
{
  return Chart::_tc_Matrix;
}

CORBA::StaticTypeInfo *_marshaller_Chart_Matrix;

#ifdef HAVE_NAMESPACE
namespace Chart { CORBA::TypeCodeConst _tc_Range; };
#else
CORBA::TypeCodeConst Chart::_tc_Range;
#endif

#ifdef HAVE_EXPLICIT_STRUCT_OPS
Chart::Range::Range()
{
}

Chart::Range::Range( const Range& _s )
{
  top = ((Range&)_s).top;
  left = ((Range&)_s).left;
  bottom = ((Range&)_s).bottom;
  right = ((Range&)_s).right;
  table = ((Range&)_s).table;
}

Chart::Range::~Range()
{
}

Chart::Range&
Chart::Range::operator=( const Range& _s )
{
  top = ((Range&)_s).top;
  left = ((Range&)_s).left;
  bottom = ((Range&)_s).bottom;
  right = ((Range&)_s).right;
  table = ((Range&)_s).table;
  return *this;
}
#endif

void operator<<=( CORBA::Any &_a, const Chart::Range &_s )
{
  CORBA::StaticAny _sa (_marshaller_Chart_Range, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, Chart::Range *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Chart::Range &_s )
{
  CORBA::StaticAny _sa (_marshaller_Chart_Range, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, Chart::Range *&_s )
{
  return _a.to_static_any (_marshaller_Chart_Range, (void *&)_s);
}

class _Marshaller_Chart_Range : public CORBA::StaticTypeInfo {
    typedef Chart::Range _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Chart_Range::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_Chart_Range::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_Chart_Range::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Chart_Range::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->top ) &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->left ) &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->bottom ) &&
    CORBA::_stc_long->demarshal( dc, &((_MICO_T*)v)->right ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->table.inout() ) &&
    dc.struct_end();
}

void _Marshaller_Chart_Range::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->top );
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->left );
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->bottom );
  CORBA::_stc_long->marshal( ec, &((_MICO_T*)v)->right );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->table.inout() );
  ec.struct_end();
}

CORBA::TypeCode_ptr _Marshaller_Chart_Range::typecode()
{
  return Chart::_tc_Range;
}

CORBA::StaticTypeInfo *_marshaller_Chart_Range;


// Stub interface SimpleChart
Chart::SimpleChart::~SimpleChart()
{
}

void *Chart::SimpleChart::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:Chart/SimpleChart:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

bool Chart::SimpleChart::_narrow_helper2( CORBA::Object_ptr _obj )
{
  if( strcmp( _obj->_repoid(), "IDL:Chart/SimpleChart:1.0" ) == 0) {
    return true;
  }
  for( vector<CORBA::Narrow_proto>::size_type _i = 0;
       _narrow_helpers && _i < _narrow_helpers->size(); _i++ ) {
    bool _res = (*(*_narrow_helpers)[ _i ])( _obj );
    if( _res )
      return true;
  }
  return false;
}

Chart::SimpleChart_ptr Chart::SimpleChart::_narrow( CORBA::Object_ptr _obj )
{
  Chart::SimpleChart_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:Chart/SimpleChart:1.0" )))
      return _duplicate( (Chart::SimpleChart_ptr) _p );
    if( _narrow_helper2( _obj ) ||
       ( _obj->_is_a_remote( "IDL:Chart/SimpleChart:1.0" ) ) ) {
      _o = new Chart::SimpleChart_stub;
      _o->MICO_SCOPE(CORBA,Object::operator=)( *_obj );
      return _o;
    }
  }
  return _nil();
}

Chart::SimpleChart_ptr
Chart::SimpleChart::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

Chart::SimpleChart_stub::~SimpleChart_stub()
{
}

void Chart::SimpleChart_stub::fill( const Chart::Range& range, const Chart::Matrix& matrix )
{
  CORBA::StaticAny _range( _marshaller_Chart_Range, &range );
  CORBA::StaticAny _matrix( _marshaller_Chart_Matrix, &matrix );
  CORBA::StaticRequest __req( this, "fill" );
  __req.add_in_arg( &_range );
  __req.add_in_arg( &_matrix );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


void Chart::SimpleChart_stub::showWizard()
{
  CORBA::StaticRequest __req( this, "showWizard" );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


#ifdef HAVE_NAMESPACE
namespace Chart { vector<CORBA::Narrow_proto> * SimpleChart::_narrow_helpers; };
#else
vector<CORBA::Narrow_proto> * Chart::SimpleChart::_narrow_helpers;
#endif
#ifdef HAVE_NAMESPACE
namespace Chart { CORBA::TypeCodeConst _tc_SimpleChart; };
#else
CORBA::TypeCodeConst Chart::_tc_SimpleChart;
#endif

void
operator<<=( CORBA::Any &_a, const Chart::SimpleChart_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_Chart_SimpleChart, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, Chart::SimpleChart_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_Chart_SimpleChart, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, Chart::SimpleChart_ptr &_obj )
{
  Chart::SimpleChart_ptr *p;
  if (_a.to_static_any (_marshaller_Chart_SimpleChart, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}

class _Marshaller_Chart_SimpleChart : public CORBA::StaticTypeInfo {
    typedef Chart::SimpleChart_ptr _MICO_T;
  public:
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    CORBA::Boolean demarshal (CORBA::DataDecoder&, StaticValueType) const;
    void marshal (CORBA::DataEncoder &, StaticValueType) const;
    CORBA::TypeCode_ptr typecode ();
};


CORBA::StaticValueType _Marshaller_Chart_SimpleChart::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_Chart_SimpleChart::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::Chart::SimpleChart::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_Chart_SimpleChart::free( StaticValueType v ) const
{
  CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

CORBA::Boolean _Marshaller_Chart_SimpleChart::demarshal( CORBA::DataDecoder &dc, StaticValueType v ) const
{
  CORBA::Object_ptr obj;
  if (!CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::Chart::SimpleChart::_narrow( obj );
  CORBA::Boolean ret = CORBA::is_nil (obj) || !CORBA::is_nil (*(_MICO_T *)v);
  CORBA::release (obj);
  return ret;
}

void _Marshaller_Chart_SimpleChart::marshal( CORBA::DataEncoder &ec, StaticValueType v ) const
{
  CORBA::Object_ptr obj = *(_MICO_T *) v;
  CORBA::_stc_Object->marshal( ec, &obj );
}

CORBA::TypeCode_ptr _Marshaller_Chart_SimpleChart::typecode()
{
  return Chart::_tc_SimpleChart;
}

CORBA::StaticTypeInfo *_marshaller_Chart_SimpleChart;

struct __tc_init_CHART {
  __tc_init_CHART()
  {
    Chart::_tc_DoubleSeq = 
    "010000001500000044000000010000001800000049444c3a43686172742f"
    "446f75626c655365713a312e30000a000000446f75626c65536571000000"
    "130000000c000000010000000700000000000000";
    Chart::_tc_StringSeq = 
    "010000001500000048000000010000001800000049444c3a43686172742f"
    "537472696e675365713a312e30000a000000537472696e67536571000000"
    "130000001000000001000000120000000000000000000000";
    Chart::_tc_Matrix = 
    "010000000f00000074010000010000001500000049444c3a43686172742f"
    "4d61747269783a312e3000000000070000004d6174726978000005000000"
    "08000000636f6c756d6e73000300000005000000726f7773000000000300"
    "000012000000636f6c756d6e4465736372697074696f6e00000015000000"
    "48000000010000001800000049444c3a43686172742f537472696e675365"
    "713a312e30000a000000537472696e675365710000001300000010000000"
    "010000001200000000000000000000000f000000726f7744657363726970"
    "74696f6e00001500000048000000010000001800000049444c3a43686172"
    "742f537472696e675365713a312e30000a000000537472696e6753657100"
    "000013000000100000000100000012000000000000000000000007000000"
    "6d617472697800001500000044000000010000001800000049444c3a4368"
    "6172742f446f75626c655365713a312e30000a000000446f75626c655365"
    "71000000130000000c000000010000000700000000000000";
    _marshaller_Chart_Matrix = new _Marshaller_Chart_Matrix;
    Chart::_tc_Range = 
    "010000000f0000007c000000010000001400000049444c3a43686172742f"
    "52616e67653a312e30000600000052616e67650000000500000004000000"
    "746f700003000000050000006c656674000000000300000007000000626f"
    "74746f6d0000030000000600000072696768740000000300000006000000"
    "7461626c650000001200000000000000";
    _marshaller_Chart_Range = new _Marshaller_Chart_Range;
    Chart::_tc_SimpleChart = 
    "010000000e00000034000000010000001a00000049444c3a43686172742f"
    "53696d706c6543686172743a312e300000000c00000053696d706c654368"
    "61727400";
    _marshaller_Chart_SimpleChart = new _Marshaller_Chart_SimpleChart;
  }
};

static __tc_init_CHART __init_CHART;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

Chart::SimpleChart_skel::SimpleChart_skel( const CORBA::BOA::ReferenceData &_id )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Chart/SimpleChart:1.0", "SimpleChart" );
  assert( !CORBA::is_nil( _impl ) );
  _create_ref( _id,
    CORBA::InterfaceDef::_nil(),
    _impl,
    "IDL:Chart/SimpleChart:1.0" );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<SimpleChart_skel>( this ) );
}

Chart::SimpleChart_skel::SimpleChart_skel( CORBA::Object_ptr _obj )
{
  CORBA::ImplementationDef_var _impl =
    _find_impl( "IDL:Chart/SimpleChart:1.0", "SimpleChart" );
  assert( !CORBA::is_nil( _impl ) );
  _restore_ref( _obj,
    CORBA::BOA::ReferenceData(),
    CORBA::InterfaceDef::_nil(),
    _impl );
  register_dispatcher( new StaticInterfaceDispatcherWrapper<SimpleChart_skel>( this ) );
}

Chart::SimpleChart_skel::~SimpleChart_skel()
{
}

bool Chart::SimpleChart_skel::dispatch( CORBA::StaticServerRequest_ptr _req, CORBA::Environment & /*_env*/ )
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( _req->op_name(), "fill" ) == 0 ) {
      Range range;
      CORBA::StaticAny _range( _marshaller_Chart_Range, &range );
      Matrix matrix;
      CORBA::StaticAny _matrix( _marshaller_Chart_Matrix, &matrix );

      _req->add_in_arg( &_range );
      _req->add_in_arg( &_matrix );

      if( !_req->read_args() )
        return true;

      fill( range, matrix );
      _req->write_results();
      return true;
    }
    if( strcmp( _req->op_name(), "showWizard" ) == 0 ) {

      if( !_req->read_args() )
        return true;

      showWizard();
      _req->write_results();
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_catch &_ex ) {
    _req->set_exception( _ex->_clone() );
    _req->write_results();
    return true;
  } catch( ... ) {
    assert( 0 );
    return true;
  }
  #endif
  return false;
}

Chart::SimpleChart_ptr Chart::SimpleChart_skel::_this()
{
  return Chart::SimpleChart::_duplicate( this );
}

