/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2002 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __KWORD_LATEX_EXPORT_KEY_H__
#define __KWORD_LATEX_EXPORT_KEY_H__

#include <QString>
//Added by qt3to4:
#include <QTextStream>

#include "xmlparser.h"

/***********************************************************************/
/* Class: Key                                                         */
/***********************************************************************/

/**
 * This class hold a real paragraph. It tells about the text in this
 * paragraph, its format, etc. The complete text is a list of Key instances.
 * A footnote is a list of paragraph instances (now but not in the "futur").
 */
class Key: public XmlParser
{
	public:
		enum eKeyType { PIXMAP, PICTURE };
	private:
		/* MARKUP DATA */
		QString _filename;
		QString _name;
		int _hour;
		int _minute;
		int _second;
		int _msec;
		int _day;
		int _month;
		int _year;

		eKeyType _type;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of Key.
		 */
		Key(eKeyType);

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of little zones.
		 */
		virtual ~Key();

		/**
		 * Accessors
		 */

		/**
		 *  @return the paragraph's name.
		 */
		QString getName() const { return _name; }
		QString getFilename() const { return _filename; }
		int getHour() const { return _hour; }
		int getMSec() const { return _msec; }
		int getDay() const { return _day; }
		int getMinute() const { return _minute; }
		int getSecond() const { return _second; }
		int getMonth() const { return _month; }
		int getYear() const { return _year; }

		//bool notEmpty() const { return (_lines == 0) ? false : (_lines->count() != 0); }
		/**
		 * Modifiers
		 */
		void setName(QString name) { _name = name; }
		void setFilename(QString filename) { _filename = filename; }
		void setHour(int hour) { _hour = hour; }
		void setMSec(int msec) { _msec = msec; }
		void setDay(int day) { _day = day; }
		void setMinute(int minute) { _minute = minute; }
		void setSecond(int second) { _second = second; }
		void setMonth(int month) { _month = month; }
		void setYear(int year) { _year = year; }

		/**
		 * Helpful functions
		 */

		/**
		 * Get information from a markup tree.
		 */
		void analyze         (const QDomNode);

		/**
		 * Write the paragraph in a file.
		 */
		void generate        (QTextStream&);

	private:

};

#endif /* __KWORD_LATEX_EXPORT_KEY_H__ */
