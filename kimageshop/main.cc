/*
 *  kis_color.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
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

#include <koApplication.h>
#include <koDocument.h>
#include <koMainWindow.h>
#include <koQueryTypes.h>

#include <dcopclient.h>

int main( int argc, char **argv )
{
    KoApplication app( argc, argv, "kimageshop", "application/x-kimageshop" );

    app.dcopClient()->attach();
    app.dcopClient()->registerAs( "kimageshop" );

    app.start();
    app.exec();

    return 0;
}
