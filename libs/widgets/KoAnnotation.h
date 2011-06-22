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

class KoAnnotation : public QObject
{
    Q_OBJECT
public:
    explicit KoAnnotation(QString author, QDateTime date, QString content, QObject *parent = 0);
    QString author(){return m_author;}
    QDateTime date(){return m_date;}
    QString content(){return m_content;}

private:
    QString m_author;
    QDateTime m_date;
    QString m_content;

};

#endif // KOANNOTATION_H
