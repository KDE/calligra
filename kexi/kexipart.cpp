/***************************************************************************
                          kexipart.cpp  -  description
                             -------------------
    begin                : Sun Nov  17 23:30:00 CET 2002
    copyright            : (C) 2002 Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <core/kexi_factory.h>

extern "C"
{
    void* init_libkexipart()
    {
        return new KexiFactory;
    }
};

