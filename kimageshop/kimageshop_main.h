/*
 *  kimageshop_view.h - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kimageshop_main_h__
#define __kimageshop_main_h__

#include <koApplication.h>

class KImageShopShell;

class KImageShopApp : public KoApplication
{
  Q_OBJECT

public:
  KImageShopApp( int& argc, char** argv );
  ~KImageShopApp() { };

  virtual KoMainWindow* createNewShell();
};

#endif
