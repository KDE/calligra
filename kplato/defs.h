/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org

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

#ifndef defs_h
#define defs_h

enum TimingType {
  START_ON_DATE=0,
  FINISH_BY_DATE=1,
  WORK_BETWEEN_DATES=2,
  MILESTONE=3,
  PERCENT_OF_PROJECT=4 };

enum TimingRelation {
  FINISH_START=0,   // child node cannot start until this node has finished 
  FINISH_FINISH=1,  // child node cannot finish until this node has finished
  START_START=2 };  // child node cannot start until this node has started

#endif
