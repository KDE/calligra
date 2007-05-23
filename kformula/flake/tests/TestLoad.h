/* This file is part of the KDE project
   Copyright 2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _TESTLOAD_H_
#define _TESTLOAD_H_

#include <QtCore/QObject>

class TestLoad : public QObject
{
    Q_OBJECT
private slots:
    // Token Elements
    void identifierElement_data();
    void identifierElement();
    void numberElement_data();
    void numberElement();
    void operatorElement_data();
    void operatorElement();
    void textElement_data();
    void textElement();
    void spaceElement_data();
    void spaceElement();
    void stringElement_data();
    void stringElement();
    void glyphElement_data();
    void glyphElement();
    

    // General layout elements
    void rowElement_data();
    void rowElement();
    void fracElement_data();
    void fracElement();
    void rootElement_data();
    void rootElement();
    void fencedElement_data();
    void fencedElement();
    
};

#endif // _TESTLOAD_H_
