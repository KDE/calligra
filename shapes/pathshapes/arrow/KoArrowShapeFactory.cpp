/* This file is part of the KDE project
 * Copyright (C) 2006 Laurent Montel <montel@kde.org>
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

#include <KoShape.h>
#include <KoProperties.h>
#include <KoLineBorder.h>
#include "arrow/KoArrowShapeFactory.h"
#include "arrow/KoArrowShape.h"

#include <klocale.h>

KoArrowShapeFactory::KoArrowShapeFactory(QObject *parent)
: KoShapeFactory(parent, KoArrowShapeId , i18n("A arrow shape"))
{
	setToolTip(i18n("A arrow"));
	setIcon("arrow"); //TODO add it.
}

KoShape *KoArrowShapeFactory::createDefaultShape()
{
	KoArrowShape *s = new KoArrowShape();
	s->resize(QSizeF(100,100));
	s->setBackground(QBrush(Qt::red));
	s->setBorder( new KoLineBorder( 1.0 ) );
	return s;
}

KoShape *KoArrowShapeFactory::createShape(const KoProperties* props) const
{
	KoArrowShape *s = new KoArrowShape();
	//define type of Arrow
	s->resize(QSizeF(100,100));
	s->setBackground(QBrush(Qt::red));
	s->setBorder( new KoLineBorder( 1.0 ) );
	return s;
}

#include <KoArrowShapeFactory.moc>
