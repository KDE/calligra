/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   Based on nexp.h : Parser module of Python-like language
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

#ifndef KEXIDB_EXPRESSION_H
#define KEXIDB_EXPRESSION_H

#include <kexidb/field.h>

namespace KexiDB {

//! classes
#define KexiDBExpr_Unknown 0
#define KexiDBExpr_Unary 1
#define KexiDBExpr_Arithm 2
#define KexiDBExpr_Logical 3
#define KexiDBExpr_Relational 4
#define KexiDBExpr_Const 5
#define KexiDBExpr_Variable 6
#define KexiDBExpr_Function 7
#define KexiDBExpr_Aggregation 8

//! A base class for all expressions
class KEXI_DB_EXPORT BaseExpr
{
public:
	typedef QPtrList<BaseExpr> List;
	typedef QPtrListIterator<BaseExpr> ListIterator;

	BaseExpr(int type);
	virtual ~BaseExpr();
	int type() const { return m_type; }
	BaseExpr *BaseExpr::parent() const { return m_par; }
	virtual void setParent(BaseExpr *p) { m_par = p; }
	virtual void check();
	virtual QString debugString();

	int cl; //class
protected:
	BaseExpr *m_par; //!< parent
	int m_type;
};

//! A base class N-argument operation
class KEXI_DB_EXPORT NArgExpr : public BaseExpr
{
public:
	NArgExpr(int typ);
	void add(BaseExpr *expr);
	BaseExpr *arg(int n);
	int args();
	virtual void check();
	virtual QString debugString();
	BaseExpr::List list;
};

//! An unary argument operation: + - NOT (or !) ~ "IS NULL" "IS NOT NULL" 
class KEXI_DB_EXPORT UnaryExpr : public NArgExpr
{
public:
	UnaryExpr(int type, BaseExpr *n);
	virtual ~UnaryExpr();
	virtual QString debugString();
	BaseExpr *arg() { return NArgExpr::arg(0); }
	virtual void check();
};

//! A base class for binary operation
//! -  arithmetic operations: + - / * % << >> & | ||
//! - relational operations: = (or ==) < > <= >= <> (or !=) LIKE IN
//! - logical operations: OR (or ||) AND (or &&) XOR
class KEXI_DB_EXPORT BinaryExpr : public NArgExpr
{
public:
	BinaryExpr(int aClass, BaseExpr *left_expr, int typ, BaseExpr *right_expr);
	virtual QString debugString();
	BaseExpr *left();
	BaseExpr *right();
	virtual void check();
};

//! string, integer, float constants
//! also includes NULL value
class KEXI_DB_EXPORT ConstExpr : public BaseExpr
{
public:
	ConstExpr(int type, const QVariant& val);
	virtual QString debugString();
	QVariant value;
};

//! variables like <fieldname> or <tablename>.<fieldname>
class KEXI_DB_EXPORT VariableExpr : public BaseExpr
{
public:
	VariableExpr(const QString& _name);
	virtual QString debugString();
	QString name;
};

//! aggregation functions like SUM, COUNT, MAX, ...
//! builtin functions like CURRENT_TIME()
//! user defined functions
class KEXI_DB_EXPORT FunctionExpr : public BaseExpr
{
public:
	FunctionExpr(const QString& _name, NArgExpr* args_);
	virtual ~FunctionExpr();
	virtual QString debugString();
	QString name;
	NArgExpr* args;
};

/*
//! Integer constant
class NConstInt : public BaseExpr
{
public:
	NConstInt(int val);
	virtual const QString dump();
	int value();
	virtual void check();
protected:
	int val;
};

//! boolean
class NConstBool : public BaseExpr
{
public:
	NConstBool(const char v);
	virtual const QString dump();
	const char value();
	virtual void check();
	virtual void genCode();
protected:
	char val;
};

//! char const
class NConstStr : public BaseExpr
{
public:
	NConstStr(const char *v);
	virtual const QString dump();
	const QString value();
	virtual void check();
	virtual void genCode();
protected:
	QString val;
};
*/

//-----------------------------------------
#if 0
/*! This class cantains information about any expression that can be used
 in sql queries.
*/
class KEXI_DB_EXPORT Expression
{
	public:
		Expression();
		virtual ~Expression();

		/*! \return type of this expression (selected from Field::Type).
		 Type information is just reused from field's type.
		 If it is not valid expression (no field is assigned), Field::InvalidType is returned. */
		int type();

		//! owner of this expression: the filed that uses expression
		KexiDB::Field* field();
	protected:
		ExpressionPrivate *d; //unused

		Field *m_field; //owner

		friend class Field;
};
#endif

} //namespace KexiDB

#endif

