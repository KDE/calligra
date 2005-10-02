/* This file is part of the KDE project
   Copyright (C) 2000 Robert JACOLIN <rjacolin@ifrance.com>

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

#ifndef __STACK_H__
#define __STACK_H__



#include <qptrlist.h>
#include <qptrstack.h>

#define GROUP_STACK_SIZE 10

typedef struct _Stack
{
	int name;
} Stack;

/*static Stack *stack = 0;
static int stack_size = GROUP_STACK_SIZE;
static int stackp = 0;
static Element * _currentElement = 0;
static Group* _tree = 0;
static QPtrStack<Element> _stackElement;*/

#endif /* __STACK_H__ */
