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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __STACK_H__
#define __STACK_H__

/* Type of commands */
typedef enum EType
{
	NONE,
	ENV,
	MATH
};

#define GROUP_STACK_SIZE 10

typedef struct _Stack
{
	int name;
} Stack;

static Stack *stack = 0;
static int stack_size = GROUP_STACK_SIZE;
static int stackp = 0;

#endif /* __STACK_H__ */
