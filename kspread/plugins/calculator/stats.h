/*
    $Id$

    KCalc, a scientific calculator for the X window system using the
    Qt widget libraries, available at no cost at http://www.troll.no

    Copyright (C) 1996 Bernd Johannes Wuebben
                       wuebben@math.cornell.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef KSTATS_H
#define KSTATS_H

#include <qptrlist.h>
#include <math.h>
#include "kcalctype.h"

class KStats
{

public:

    KStats();
    ~KStats();

public:

    void clearAll();
    void enterData(CALCAMNT data);
    void clearLast();
    CALCAMNT min();
    CALCAMNT max();
    CALCAMNT sum();
    CALCAMNT mul();
    CALCAMNT sum_of_squares();
    CALCAMNT mean();
    CALCAMNT median();
    CALCAMNT std_kernel();
    CALCAMNT std();
    CALCAMNT sample_std();
    int	     count();
    bool     error();

private:

    QPtrList<CALCAMNT> data;
    bool error_flag;

};

class MyList : public  QPtrList<CALCAMNT>
{

public:

  MyList() : QPtrList<CALCAMNT>() {};

protected:
  int compareItems(Item item1,Item item2);
};


#endif /*KSTATS_H*/

