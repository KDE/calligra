/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KFORMULAPARTFACTORY_H
#define KFORMULAPARTFACTORY_H

#include <KoFactory.h>

class KInstance;
class KAboutData;

/**
 * @short Factory implementation for the KFormulaPart
 *
 * The class KFormulaPartFactory holds a static instance of the KFormula application
 * and an instance of the @ref KAboutData class. The latter is created in the
 * aboutData() method with the help of newKFormulaAboutData() which is implemented
 * in the KFormulaAboutData.h file.
 * Use createPartObject() to obtain a pointer to a new instance of the KFormulaPart.
 * With global you can access the current @ref KInstance of KFormula
 */
class KFormulaPartFactory : public KoFactory
{
    Q_OBJECT
public:
    KFormulaPartFactory( QObject* parent = 0 );
    ~KFormulaPartFactory();

    virtual KParts::Part *createPartObject( QWidget *parentWidget = 0,
		                            QObject *parent = 0,
					    const char *classname = "KoDocument",
   			                    const QStringList &args = QStringList() );
    static KInstance* global();
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
    static KAboutData* s_aboutData;
};

#endif
