/***************************************************************************
                          kugar_about.h  -  description
                             -------------------
    begin                : Mon Nov 25 17:45:00 CET 2002
    copyright            : (C) 2002 by Joseph Wenninger
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


#ifndef KUGAR_ABOUT_H
#define KUGAR_ABOUT_H

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <kaboutdata.h>
#include <klocale.h>

KAboutData *newKugarAboutData()
{
        return new KAboutData("kugar",
                                                   I18N_NOOP("Kugar"),
                                                       VERSION);
}
#endif
