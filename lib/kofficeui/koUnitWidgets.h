/* This file is part of the KDE project
   Copyright (C) 2002, Rob Buis(buis@kde.org)
   Copyright (C) 2004, Nicolas GOUTTE <goutte@kde.org>

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

#ifndef __KOUNITWIDGETS_H__
#define __KOUNITWIDGETS_H__

#include <knuminput.h>
#include <knumvalidator.h>
#include <klineedit.h>
#include <kcombobox.h>
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
	KoUnitDoubleBase( KoUnit::Unit unit, unsigned int precision ) : m_unit( unit ), m_precision( precision ) {}
	virtual ~KoUnitDoubleBase() {}

	virtual void changeValue( double ) = 0;
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT ) = 0;

	void setValueInUnit( double value, KoUnit::Unit unit )
	{
		changeValue( KoUnit::ptToUnit( KoUnit::ptFromUnit( value, unit ), m_unit ) );
	}

	void setPrecision( unsigned int precision ) { m_precision = precision; };
protected:
    friend class KoUnitDoubleValidator;
    QString getVisibleText( double value ) const;
    double toDouble( const QString& str, bool* ok ) const;

protected:
	KoUnitDoubleValidator	*m_validator;
	KoUnit::Unit			m_unit;
	unsigned int			m_precision;
};

class KoUnitDoubleSpinBox : public KDoubleSpinBox, public KoUnitDoubleBase
{
public:
	KoUnitDoubleSpinBox( QWidget *parent, double lower, double upper, double step, double value = 0.0,
					KoUnit::Unit unit = KoUnit::U_PT, unsigned int precision = 2, const char *name = 0 );

	virtual void changeValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );
    /// @deprecated 
    KDE_DEPRECATED double value() const { return KDoubleSpinBox::value(); }
    /// Get the value, converted in points
    double valueInPoints( void ) const;

protected:
    double m_lower; //< lowest value in points
    double m_upper; //< highest value in points
    double m_step;  //< step in points
};

class KoUnitDoubleLineEdit : public KLineEdit, public KoUnitDoubleBase
{
public:
	KoUnitDoubleLineEdit( QWidget *parent, double lower, double upper, double value = 0.0, KoUnit::Unit unit = KoUnit::U_PT, unsigned int precision = 2, const char *name = 0 );

	virtual void changeValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );

    /// @deprecated 
    KDE_DEPRECATED double value() const { return m_value; }
    /// Get the value, converted in points
    double valueInPoints( void ) const;

protected:
	 bool eventFilter( QObject* obj, QEvent* ev );

private:
	double m_value;
	double m_lower;
	double m_upper;
};

class KoUnitDoubleComboBox : public KComboBox, public KoUnitDoubleBase
{
Q_OBJECT
public:
	KoUnitDoubleComboBox( QWidget *parent, double lower, double upper, double value = 0.0, KoUnit::Unit unit = KoUnit::U_PT, unsigned int precision = 2, const char *name = 0 );

	virtual void changeValue( double );
	void updateValue( double );
	virtual void setUnit( KoUnit::Unit = KoUnit::U_PT );

    /// @deprecated 
     KDE_DEPRECATED double value() const { return m_value; }
    /// Get the value, converted in points
    double valueInPoints( void ) const;
	void insertItem( double, int index = -1 );

protected:
	 bool eventFilter( QObject* obj, QEvent* ev );

signals:
	 void valueChanged(double);

private slots:
	void slotActivated( int );

protected:
	double m_value;
	double m_lower;
	double m_upper;
};

class KoUnitDoubleSpinComboBox : public QWidget
{
Q_OBJECT
public:
	KoUnitDoubleSpinComboBox( QWidget *parent, double lower, double upper, double step, double value = 0.0, KoUnit::Unit unit = KoUnit::U_PT, unsigned int precision = 2, const char *name = 0 );

	void insertItem( double, int index = -1 );
	void updateValue( double );
    /// @deprecated 
    double value() const KDE_DEPRECATED ;
    /// Get the value, converted in points
    double valueInPoints( void ) const;

signals:
	 void valueChanged(double);

private slots:
	void slotUpClicked();
	void slotDownClicked();

private:
	KoUnitDoubleComboBox *m_combo;
	double m_step;
};

#endif

