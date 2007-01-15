/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PICTURE_SHAPE_FACTORY
#define PICTURE_SHAPE_FACTORY

#include <QStringList>

#include <KoShapeFactory.h>


class KoShape;

class PictureShapePlugin : public QObject
{
    Q_OBJECT

public:

    PictureShapePlugin( QObject * parent,  const QStringList & );
    ~PictureShapePlugin() {}

};

class PictureShapeFactory : public KoShapeFactory
{
    Q_OBJECT
public:
    PictureShapeFactory( QObject* parent, const QStringList& );
    ~PictureShapeFactory() {}

    KoShape* createDefaultShape() const;
    KoShape* createShape( const KoProperties* params ) const;
};

#endif 
