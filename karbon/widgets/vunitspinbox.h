/* This file is part of the KDE project
   Copyright (C) 2002, Rob Buis(buis@kde.org)

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

class KoUnitDoubleBase;

class KoUnitDoubleValidator : public KDoubleValidator
{
public:
	KoUnitDoubleValidator( KoUnitDoubleBase *base, QObject *parent, const char *name = 0 );

	virtual	QValidator::State validate( QString &, int & ) const;

private:
	KoUnitDoubleBase	*m_base;
};

class KoUnitDoubleBase
{
public:
	KoUnitDoubleBase( unsigned int precision ) : m_precision( precision ) {}
	virtual ~KoUnitDoubleBase() {}

	virtual void changeValue( double ) = 0;
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT ) = 0;

	void setValueInUnit( double value, KoUnit::Unit unit )
	{
		changeValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( value, unit ), m_unit ) );
	}
protected:
	friend class KoUnitDoubleValidator;
	unsigned int m_precision;
	KoUnit::Unit m_unit;
};

class KoUnitDoubleSpinBox : public KDoubleSpinBox, public KoUnitDoubleBase
{
public:
	KoUnitDoubleSpinBox( QWidget *parent, double lower, double upper, double step, double value = 0.0,
					 unsigned int precision = 2, const char *name = 0 );

	virtual void setValidator( const QValidator * );

	virtual void changeValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );

private:
	KoUnitDoubleValidator *m_validator;
};

class KoUnitDoubleLineEdit : public QLineEdit, public KoUnitDoubleBase
{
public:
	KoUnitDoubleLineEdit( QWidget *parent, double lower, double upper, double value = 0.0, unsigned int precision = 2, const char *name = 0 );

	virtual void changeValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );

	void setValue( double value ) { m_value = value; }
	double value() { return m_value; }

protected:
	 bool eventFilter( QObject* obj, QEvent* ev );

private:
	KoUnitDoubleValidator *m_validator;
	double m_value;
	double m_lower;
	double m_upper;
};

class KoUnitDoubleComboBox : public QComboBox, public KoUnitDoubleBase
{
public:
	KoUnitDoubleComboBox( QWidget *parent, double lower, double upper, double value = 0.0, unsigned int precision = 2, const char *name = 0 );

	virtual void changeValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );

	void setValue( double value ) { m_value = value; }
	double value() { return m_value; }

protected:
	 bool eventFilter( QObject* obj, QEvent* ev );

private:
	KoUnitDoubleValidator *m_validator;
	double m_value;
	double m_lower;
	double m_upper;
};


#endif

