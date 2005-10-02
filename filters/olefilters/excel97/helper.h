/* This file is part of the KDE project

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef HELPER_H
#define HELPER_H

#include <qdatastream.h>
#include <qptrlist.h>
#include <qintdict.h>
#include <qdom.h>
#include <qstringlist.h>

#include <klocale.h>

#include "definitions.h"

class FormulaTodo
{
public:
	FormulaTodo(Q_UINT16 col, Q_UINT16 row, Q_UINT16 biff) { m_col = col; m_row = row; m_biff = biff; }

	Q_UINT16 col() { return m_col; }
	Q_UINT16 row() { return m_row; }
	Q_UINT16 biff() { return m_biff; }

private:
	Q_UINT16 m_col, m_row, m_biff;
};

class SharedFormula
{
public:
	SharedFormula(int fr, int lr, int fc, int lc, QDataStream *stream, QByteArray *a, char *store, int length)
	{
		m_fr = fr; m_lr = lr; m_fc = fc; m_lc = lc;
		m_stream = stream; m_byteArray = a; m_store = store; m_dataLength = length;
	}

	~SharedFormula()
	{
		m_byteArray->resetRawData(m_store, m_dataLength);
		delete m_byteArray;
		delete []m_store;
		delete m_stream;
	}

	bool checkrow(int row) { return (row >= m_fr && row <= m_lr); }
	bool checkcol(int col) { return (col >= m_fc && col <= m_lc); }
	QDataStream *stream() { return m_stream; }

private:
	int m_fr, m_lr, m_fc, m_lc;
	int m_dataLength;
	
	char *m_store;
	QByteArray *m_byteArray;
	QDataStream *m_stream;
};

class MergeInfo
{
public:
	MergeInfo(int fr, int lr, int fc, int lc)
	{ m_fr = fr; m_lr = lr; m_fc = fc; m_lc = lc; }
	
	int row() { return m_fr + 1; }
	int col() { return m_fc + 1; }

	int rowspan() { return m_lr - m_fr; }
	int colspan() { return m_lc - m_fc; }

private:
	int m_fr, m_lr, m_fc, m_lc;
};

class PenFormat
{
public:
	PenFormat() { }

	void setWidth(int width) { m_width = width; }
	void setStyle(int style) { m_style = style; }

	int width() { return m_width; }
	int style() { return m_style; }

private:
	int m_width, m_style;
};

class Helper
{
public:
	Helper(QDomDocument *root, QPtrList<QDomElement> *tables);
	~Helper();

	void done();
	
	void addDict(Dictionary dict, int index, void *obj);
	void *queryDict(Dictionary dict, int index);

	void addSharedFormula(SharedFormula *formula);
	
	void getTime(double time, int &hour,int  &min, int &second);
	QDate getDate(double _numdays);
	
	void getFont(Q_UINT16, QDomElement &f, Q_UINT16 fontid);
	void getPen(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid);
	
	double GetDoubleFromRK( Q_UINT32 nRKValue );
	QString formatValue(double value, Q_UINT16 xf);

	const QString getFormula(Q_UINT16 row, Q_UINT16 column, QDataStream &rgce, Q_UINT16 biff, bool shared = false);
	const QDomElement getFormat(Q_UINT16 xf);

	PenFormat borderStyleToQtStyle(int penStyle);

	KLocale locale() { return m_locale; }

	void setDate1904(int v) { m_date1904 = v; };

	void addName( const QString& s );
	
private:
	QDomDocument *m_root;
	QPtrList<QDomElement> *m_tables;
	
	QIntDict<xfrec> m_xfrec;
	QIntDict<QString> m_sstrec;
	QIntDict<fontrec> m_fontrec;
	QIntDict<formatrec> m_formatrec;	

	QPtrList<SharedFormula> m_formulaList;
	QPtrList<FormulaTodo> m_todoFormula;

	QStringList m_names;

	KLocale m_locale;

	int m_date1904;
};

#endif
