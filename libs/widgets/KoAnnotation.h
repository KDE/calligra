/* This file is part of the KDE project
 * Copyright (C) 2011 Steven Kakoczky <steven.kakoczky@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOANNOTATION_H
#define KOANNOTATION_H

#include <QObject>
#include <QDateTime>

class KoAnnotation : public QObject
{
public:
	// make a new annotation with no text and time set to current
	explicit KoAnnotation(QString author);
	// used for loading annotations
	explicit KoAnnotation(QString &author, QDateTime &date, QString &text);

	QString& author()
	{
		return m_author;
	}
	QString& text()
	{
		return m_text;
	}
	QDateTime& date()
	{
		return m_date;
	}

private:
	QString m_text;
	QString m_author;
	QDateTime m_date;
};

#endif // KOANNOTATION_H
