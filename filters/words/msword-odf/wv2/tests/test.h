/* This file is part of the wvWare 2 project
   Copyright (C) 2002-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#include <cstdlib>
#include <iostream>
#include <string>

#include <stdio.h>

namespace wvWare
{

    void test( bool result, const std::string& failureMessage, const std::string& successMessage = "" )
    {
        if ( result ) {
            if ( !successMessage.empty() )
                std::cerr << successMessage << std::endl;
        }
        else {
            std::cerr << failureMessage << std::endl;
            std::cerr << "Test NOT finished successfully." << std::endl;
            ::exit( 1 );
        }
    }

    void test( bool result )
    {
        test( result, "Failed", "Passed" );
    }

} // namespace wvWare
