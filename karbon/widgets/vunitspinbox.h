/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

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

#ifndef __VNUMINPUT_H__
#define __VNUMINPUT_H__

#include <knuminput.h>
#include <knumvalidator.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <koUnit.h>

class VUnitDoubleBase;

class KoUnitDoubleValidator : public KDoubleValidator
{
public:
	KoUnitDoubleValidator( VUnitDoubleBase *base, QObject *parent, const char *name = 0 );

	virtual	QValidator::State validate( QString &, int & ) const;

private:
	VUnitDoubleBase	*m_base;
};

class VUnitDoubleBase
{
public:
	VUnitDoubleBase( unsigned int precision ) : m_precision( precision ) {}
	virtual ~VUnitDoubleBase() {}

	virtual void changeValue( double ) = 0;
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT ) = 0;

protected:
	friend class KoUnitDoubleValidator;
	unsigned int m_precision;
	KoUnit::Unit m_unit;
};

class VUnitDoubleSpinBox : public KDoubleSpinBox, public VUnitDoubleBase
{
public:
	VUnitDoubleSpinBox( QWidget *parent, double lower, double upper, double step, double value = 0.0,
					 unsigned int precision = 2, const char *name = 0 );

	virtual void setValidator( const QValidator * );

	virtual void changeValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );

private:
	KoUnitDoubleValidator *m_validator;
};

class VUnitDoubleLineEdit : public QLineEdit, public VUnitDoubleBase
{
public:
	VUnitDoubleLineEdit( QWidget *parent, double lower, double upper, double value = 0.0, unsigned int precision = 2, const char *name = 0 );

	virtual void setValidator( const QValidator * );

	virtual void changeValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );

	void setValue( double value ) { m_value = value; }
	double value() { return m_value; }

private:
	KoUnitDoubleValidator *m_validator;
	double	m_value;
};

class VUnitDoubleComboBox : public QComboBox, public VUnitDoubleBase
{
public:
	VUnitDoubleComboBox( QWidget *parent, double lower, double upper, double value = 0.0, unsigned int precision = 2, const char *name = 0 );

	virtual void changeValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );

	void setValue( double value ) { m_value = value; }
	double value() { return m_value; }

private:
	KoUnitDoubleValidator *m_validator;
	double	m_value;
};


#endif

