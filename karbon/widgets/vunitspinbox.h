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
#include <qvalidator.h>
#include <koUnit.h>

class VUnitDoubleSpinBox : public KDoubleSpinBox
{
public:
	VUnitDoubleSpinBox( double lower, double upper, double step, double value = 0.0,
					 int precision = 2, QWidget *parent = 0, const char *name = 0 );

	virtual void setValidator( const QValidator * );
	void setUnit( KoUnit::Unit = KoUnit::U_PT );

private:
	class VUnitDoubleValidator : public QValidator
	{
	public:
		VUnitDoubleValidator( VUnitDoubleSpinBox *spin, QObject *parent, const char *name = 0 );

		virtual	QValidator::State validate( QString &, int & ) const;

		KoUnit::Unit		m_unit;
		VUnitDoubleSpinBox	*m_spin;
	};

	VUnitDoubleValidator *m_validator;
};

#endif

