//
//
// C++ Interface: pg_type
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.fsnet.co.uk>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
//boolean, 'true'/'false'
#define BOOLOID   16

//Unknown Type
#define UNKNOWNOID  705

//Numberic Types
//==============

//~18 digit integer, 8-byte storage
#define INT8OID   20

//-32 thousand to 32 thousand, 2-byte storage
#define INT2OID   21

//array of INDEX_MAX_KEYS int2 integers, used in system tables
#define INT2VECTOROID   22

//-2 billion to 2 billion integer, 4-byte storage
#define INT4OID   23

//single-precision floating point number, 4-byte storage
#define FLOAT4OID   700

//double-precision floating point number, 8-byte storage
#define FLOAT8OID   701

//monetary amounts, $d,ddd.cc
#define CASHOID   790

//numeric(precision, decimal), arbitrary precision number
#define NUMERICOID  1700

//==================================

//Text Types
//==========
//variable-length string, binary values escaped
#define BYTEAOID   17

//single character
#define CHAROID   18

//variable-length string, no limit specified
#define TEXTOID   25

//char(length), blank-padded string, fixed storage length
#define BPCHAROID  1042

//varchar(length), non-blank-padded string, variable storage length
#define VARCHAROID  1043

//fixed-length bit string
#define BITOID    1560

//variable-length bit string
#define VARBITOID    1562

//==================================

//Date Time Types
//===============
//absolute, limited-range date and time (Unix system time)
#define ABSTIMEOID  702

//relative, limited-range time interval (Unix delta time)
#define RELTIMEOID  703

//(abstime,abstime), time interval
#define TINTERVALOID  704

//ANSI SQL date
#define DATEOID   1082

//hh:mm:ss, ANSI SQL time
#define TIMEOID   1083

//date and time
#define TIMESTAMPOID  1114

//date and time with time zone
#define TIMESTAMPTZOID  1184

//@ <number> <units>, time interval
#define INTERVALOID  1186

//hh:mm:ss, ANSI SQL time
#define TIMETZOID  1266


//==================================

//Internal OID Types
//==================
//object identifier(oid), maximum 4 billion
#define OIDOID   26

//(Block, offset), physical location of tuple
#define TIDOID   27

//transaction id
#define XIDOID    28

//command identifier type, sequence in transaction id
#define CIDOID    29

//array of INDEX_MAX_KEYS oids, used in system tables
#define OIDVECTOROID  30


//==================================

//Geometric Types
//===============
//geometric point '(x, y)'
#define POINTOID  600

//geometric line segment '(pt1,pt2)'
#define LSEGOID   601

//geometric path '(pt1,...)'
#define PATHOID   602

//geometric box '(lower left,upper right)'
#define BOXOID   603

//geometric polygon '(pt1,...)'
#define POLYGONOID  604

//geometric line (not implemented)'
#define LINEOID   628

//geometric circle '(center,radius)'
#define CIRCLEOID  718

//==================================

//Network Types
//=============
//XX:XX:XX:XX:XX:XX, MAC address
#define MACADDROID   829

//IP address/netmask, host address, netmask optional
#define INETOID   869

//network IP address/netmask, network address
#define CIDROID   650

//access control list
#define ACLITEMOID  1033


//==================================

//Miscellaneous Types
//===================
//63-character type for storing system identifiers
#define NAMEOID   19

//registered procedure
#define REGPROCOID  24

//reference cursor (portal name)
#define REFCURSOROID  1790

//registered procedure (with args)
#define REGPROCEDUREOID  2202

//registered operator
#define REGOPEROID  2203

//registered operator (with args)
#define REGOPERATOROID  2204

//registered class
#define REGCLASSOID  2205

//registered type
#define REGTYPEOID  2206

