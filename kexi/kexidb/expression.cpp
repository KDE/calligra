/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   Based on nexp.cpp : Parser module of Python-like language
   (C) 2001 Jaroslaw Staniek, MIMUW (www.mimuw.edu.pl)

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

#include "expression.h"
#include <kdebug.h>

KEXI_DB_EXPORT QString KexiDB::exprClassName(int c)
{
	if (c==KexiDBExpr_Unary)
		return "Unary";
	else if (c==KexiDBExpr_Arithm)
		return "Arithm";
	else if (c==KexiDBExpr_Logical)
		return "Logical";
	else if (c==KexiDBExpr_Relational)
		return "Relational";
	else if (c==KexiDBExpr_SpecialBinary)
		return "SpecialBinary";
	else if (c==KexiDBExpr_Const)
		return "Const";
	else if (c==KexiDBExpr_Variable)
		return "Variable";
	else if (c==KexiDBExpr_Function)
		return "Function";
	else if (c==KexiDBExpr_Aggregation)
		return "Aggregation";
	else if (c==KexiDBExpr_TableList)
		return "TableList";
	
	return "Unknown";
}

using namespace KexiDB;

//=========================================

BaseExpr::BaseExpr(int type) 
 : m_cl(KexiDBExpr_Unknown)
 , m_par(0)
 , m_type(type)
{

}

BaseExpr::~BaseExpr()
{
}

QString BaseExpr::debugString()
{
	return QString("BaseExpr(%1)").arg(m_type);
}

void BaseExpr::check()
{
//	UNIMPLEMENTED("check()");
}

//=========================================

NArgExpr::NArgExpr(int aClass, int type)
 : BaseExpr(type)
{
	m_cl = aClass;
	list.setAutoDelete(TRUE);
}

QString NArgExpr::debugString()
{
	QString s = QString("NArgExpr(")
		+ "class=" + exprClassName(m_cl);
	BaseExpr::ListIterator it(list);
	for ( int i=0; it.current(); ++it, i++ ) {
		s+=", ";
		s+=it.current()->debugString();
	}
	s+=")";
	return s;
}

BaseExpr* NArgExpr::arg(int nr)
{
	return list.at(nr); 
}

void NArgExpr::add(BaseExpr *expr)
{
	list.append(expr);
}

int NArgExpr::args()
{
	return list.count();
}

void NArgExpr::check()
{
	BaseExpr::check();
	BaseExpr *e;
	for (BaseExpr::ListIterator it(list); (e=it.current()) ; ++it ) {
		e->check();
	}
}

//=========================================
UnaryExpr::UnaryExpr(int typ, BaseExpr *n)
 : NArgExpr(KexiDBExpr_Unary, typ)
{
	list.append(n);
	//ustaw ojca
	n->setParent(this);
}

UnaryExpr::~UnaryExpr()
{
}

QString UnaryExpr::debugString()
{
	return "UnaryExpr(" 
		+ (arg() ? arg()->debugString() : QString("<NONE>")) 
		+ ")";
}

void UnaryExpr::check()
{
	NArgExpr::check();
#if 0
	BaseExpr *n = l.at(0);

	n->check();
/*typ wyniku:
		const bool dla "NOT <bool>" (negacja)
		int dla "# <str>" (dlugosc stringu)
		int dla "+/- <int>"
		*/
	if (is(NOT) && n->nodeTypeIs(TYP_BOOL)) {
		node_type=new NConstType(TYP_BOOL);
	}
	else if (is('#') && n->nodeTypeIs(TYP_STR)) {
		node_type=new NConstType(TYP_INT);
	}
	else if ((is('+') || is('-')) && n->nodeTypeIs(TYP_INT)) {
		node_type=new NConstType(TYP_INT);
	}
	else {
		ERR("Niepoprawny argument typu '%s' dla operatora '%s'",
			n->nodeTypeName(),is(NOT)?QString("not"):QChar(typ()));
	}
#endif
}
	
//=========================================
BinaryExpr::BinaryExpr(int aClass, BaseExpr *l_n, int typ, BaseExpr *r_n)
 : NArgExpr(aClass, typ)
{
	list.append(l_n);
	list.append(r_n);
	//ustaw ojca
	l_n->setParent(this);
	r_n->setParent(this);
}

BaseExpr *BinaryExpr::left()
{
	return arg(0);
}

BaseExpr *BinaryExpr::right()
{
	return arg(1);
}

void BinaryExpr::check() {
	BaseExpr::check();
}

QString BinaryExpr::debugString()
{
	return QString("BinaryExpr(")
		+ "class=" + exprClassName(m_cl)
		+ "," + (left() ? left()->debugString() : QString("<NONE>")) 
		+ "," + QString::number(m_type) + ","
		+ (right() ? right()->debugString() : QString("<NONE>")) 
		+ ")";
}

//=========================================
ConstExpr::ConstExpr( int type, const QVariant& val)
: BaseExpr( type )
, value(val)
{
	m_cl = KexiDBExpr_Const;
}

QString ConstExpr::debugString()
{
	return QString("ConstExpr(") + value.typeName() + " " + value.toString() + ")";
}

//=========================================
VariableExpr::VariableExpr( const QString& _name)
: BaseExpr( 0/*undefined*/ )
, name(_name)
{
	m_cl = KexiDBExpr_Variable;
}

QString VariableExpr::debugString()
{
	return QString("VariableExpr(") + name + ")";
}

//=========================================
FunctionExpr::FunctionExpr( const QString& _name, NArgExpr* args_ )
 : BaseExpr( 0/*undefined*/ )
 , name(_name)
 , args(args_)
{
	m_cl = KexiDBExpr_Function;
	args->setParent( this );
}

FunctionExpr::~FunctionExpr()
{
	delete args;
}

QString FunctionExpr::debugString()
{
	return QString("FunctionExpr(") + name + "," + args->debugString() + ")";
}

#if 0
//=========================================
ArithmeticExpr::ArithmeticExpr(BaseExpr *l_n, int typ, BaseExpr *r_n)
	 : BinaryExpr(l_n,typ,r_n)
{
}

void ArithmeticExpr::check() {
	BinaryExpr::check();

	BaseExpr *l_n = l.at(0);
	BaseExpr *r_n = l.at(1);
	ASSERT(l_n!=NULL);
	ASSERT(r_n!=NULL);

	l_n->check();
	r_n->check();

/*typ wyniku:
		oba argumenty musza miec rwartosc
		typu string lub int (stala albo zmienna) */
	if (l_n->nodeTypeIs(TYP_INT) && r_n->nodeTypeIs(TYP_INT)) {
		node_type=new NConstType(TYP_INT);
	}
	else if (l_n->nodeTypeIs(TYP_STR) && r_n->nodeTypeIs(TYP_STR) && is('+')) {
		node_type=new NConstType(TYP_STR);
	}
	else {
		ERR("Niepoprawne argumenty typu '%s' i '%s' dla operatora '%s'",
			l_n->nodeTypeName(),
			r_n->nodeTypeName(),
			QChar(typ()));
	}
debug("ArithmeticExpr::check() OK");
}

//=========================================
RelationalExpr::RelationalExpr(BaseExpr *l_n, int typ, BaseExpr *r_n)
 : BinaryExpr(l_n,typ,r_n)
{
}

void RelationalExpr::check() 
{
	BinaryExpr::check();
	BaseExpr *l_n = l.at(0);
	BaseExpr *r_n = l.at(1);
	ASSERT(l_n!=NULL);
	ASSERT(r_n!=NULL);

	l_n->check();
	r_n->check();

	char errop=0; //==1 gdy bledna oper.
/*typ wyniku:
	const bool dla:
	"<int> =,<,>,<>,<=,>= <int>"
	"<string> =,<,>,<>,<=,>= <string>"
	"<bool> =,<> <bool>"
	"<class> =,<> <class>"
	"<dict> =,<> <dict>"
		*/
	if ((l_n->nodeTypeIs(TYP_INT)	&& r_n->nodeTypeIs(TYP_INT))
		||(l_n->nodeTypeIs(TYP_STR) && r_n->nodeTypeIs(TYP_STR))) {
			switch (typ()) {
			case '=':
			case '<':
			case '>':
			case REL_ROZNE:
			case REL_MN_ROWNE:
			case REL_WIEK_ROWNE:
				break;//ok
			default:
				errop=1;//blad
			}
	}
//	else if ((l_n->nodeTypeIs(TYP_BOOL) && r_n->nodeTypeIs(TYP_BOOL))
//		||(l_n->nodeTypeIs(TYP_CLASS) && r_n->nodeTypeIs(TYP_CLASS))
//		||(l_n->nodeTypeIs(TYP_DICT) && r_n->nodeTypeIs(TYP_DICT))) {
	else if ((l_n->nodeTypeIs(TYP_BOOL)
	 || l_n->nodeTypeIs(TYP_DICT)
	 || l_n->nodeTypeIs(TYP_CLASS)
	 || l_n->nodeTypeIs(TYP_NIL))
	 && r_n->nodeType()->like(l_n->nodeType())) {
			switch (typ()) {
			case '=':
			case REL_ROZNE:
				break;//ok
			default:
				errop=1;//blad
			}
	}
	else
		errop=1;
	
	if (errop) {
		ERR("Niepoprawne argumenty typu '%s' i '%s' dla operatora relacyjnego '%s'",
			l_n->nodeTypeName(),
			r_n->nodeTypeName(),
			tname());
	}
	else {//ok:
		node_type=new NConstType(TYP_BOOL);
	}
}

//=========================================
LogicalExpr::LogicalExpr(BaseExpr *l_n, int typ, BaseExpr *r_n)
 : BinaryExpr(l_n,typ,r_n)
{
}

void LogicalExpr::check()
{
	BinaryExpr::check();
	BaseExpr *l_n = l.at(0);
	BaseExpr *r_n = l.at(1);
	ASSERT(l_n!=NULL);
	ASSERT(r_n!=NULL);

	l_n->check();
	r_n->check();

/*typ wyniku: const bool dla "<bool> OR/AND <bool>"
		*/
	if (l_n->nodeTypeIs(TYP_BOOL)
		&& r_n->nodeTypeIs(TYP_BOOL)) {
		node_type = l_n->nodeType();
	}
	else {
		ERR("Niepoprawne argumenty typu '%s' i '%s' dla operacji logicznej '%s'",
			l_n->nodeTypeName(),
			r_n->nodeTypeName(),
			QString(is(AND)?"and":"or"));
	}
}
#endif

#if 0
NConstInt::NConstInt(int v)
	: BaseExpr(CONST_INT), val(v)
{
	node_type = new NConstType(TYP_INT);
}
//-----------------------------------------
const QString NConstInt::dump() {
	return QString(name()) + "(" + QString::number(val)+ ")";
}
//-----------------------------------------
const QString NConstInt::name() { return "ConstInt"; }
//-----------------------------------------
int NConstInt::value() { return val; }
//-----------------------------------------
void NConstInt::check() {
	BaseExpr::check();
}

//=========================================
//stala booleowska
NConstBool::NConstBool(const char v)
	: BaseExpr(CONST_BOOL), val(v)
{
	node_type = new NConstType(TYP_BOOL);
}
//-----------------------------------------
const QString NConstBool::dump()	{
	return QString(name()) + "(" + QString::number(val) + ")";
}
//-----------------------------------------
const QString NConstBool::name() { return "ConstBool"; }
//-----------------------------------------
const char NConstBool::value() { return val; }
//-----------------------------------------
void NConstBool::check() {
	BaseExpr::check();
}

//=========================================
//stala znakowa
NConstStr::NConstStr(const char *v)
	: BaseExpr(CONST_STR), val(v)
{
	node_type = new NConstType(TYP_STR);
}
//-----------------------------------------
const QString NConstStr::dump()	{
	return QString(name()) + "(" + val + ")";
}
//-----------------------------------------
const QString NConstStr::name() {
	return "ConstStr"; }
//-----------------------------------------
const QString NConstStr::value() {
	return val; }
//-----------------------------------------
void NConstStr::check() {
	BaseExpr::check();
//dodaj etykiete i zapisz string
	lab=generateUniqueLabel("const");
	gen.mlabel(lab);
	gen.mcode(MN_TEXT,val);
}

#endif

/* OLD
Expression::Expression()
	:d(0)//unused
	,m_field(0)
{
}

Expression::~Expression()
{
}

int Expression::type()
{
	if (!m_field)
		return Field::InvalidType;
	return m_field->type();
}
*/

