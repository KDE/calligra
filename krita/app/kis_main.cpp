/*
 *  kis_main.cpp - part of Krita
 *
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
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
 */
#include "kis_application.h"

extern "C" int main(int argc, char **argv)
{
    // first create the application so we can create a  pixmap
    KisApplication app(argc, argv);

    if (!app.start()) {
        return 1;
    }

    int state = app.exec();
    return state;
}

