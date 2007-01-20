/*
    =========================================================

      This file is part of the library KDABLibFakes,
      used internally for compiling and building the
      projects of Klaraelvdalens Datakonsult AB.
      (visit http://www.kdab.net for information)

      KDABLibFakes is matter of change without notification!

      Do *not* include this library into your own projects!

    =========================================================
*/


/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDAB auxiliary library of internal defines.
 **
 **      It is  N O T  to be included into user projects,
 **      but internally used by KD AB projects only.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial licenses from Klarälvdalens
 ** Datakonsult AB may use this file in accordance with the respective
 ** License Agreement provided with the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** As an example see http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#ifndef KDAB_LIB_FAKES_H
#define KDAB_LIB_FAKES_H

#include <cmath>

#include <qglobal.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#define DEGTORAD(d) (d)*M_PI/180


// We use our own ISNAN / ISINF in the code to detect
// that we defined them.
// reason: Windows / MacOS do not have isnan() / isinf()
#if defined Q_OS_WIN
#include <float.h>
#define ISNAN(x ) _isnan(x )
#define ISINF(x ) (!(_finite(x ) + _isnan(x ) ) )
#elif defined Q_OS_DARWIN
#define ISNAN(x) std::isnan(x)
#define ISINF(x) std::isinf(x)
#else
#define ISNAN(x) isnan(x)
#define ISINF(x) isinf(x)
#endif


// We wrap every for() by extra { } to work around
// the scope bug for loop counters in MS Visual C++ v6
#if defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET)
/* This is done in Qt41 qglobal.h but not Qt42*/
#if QT_VERSION < 0x040200
#define for if (0) {} else for
#endif
#define KDAB_FOREACH( v, c ) if (0) {} else Q_FOREACH( v, c )
#else
#define KDAB_FOREACH( v, c ) Q_FOREACH( v, c )
#endif

#endif
