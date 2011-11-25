/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiaboutdata.h"
#include <kexi_version.h>

#include <calligraversion.h> //only for CALLIGRA_VERSION_STRING
#include <klocale.h>

static const char *description =
#ifndef CUSTOM_VERSION
    I18N_NOOP("Database creation for everyone\n\nThis application is a part of the Calligra Suite.")
#else
    I18N_NOOP("Database creation for everyone")
#endif
    ;

using namespace Kexi;

KAboutData* Kexi::createAboutData()
{
    KAboutData *aboutData = new KAboutData(
        "kexi", 0,
        ki18n(KEXI_APP_NAME),
        KEXI_VERSION_STRING
#ifndef CUSTOM_VERSION
        " (Calligra " CALLIGRA_VERSION_STRING ")"
#endif
        , ki18n(description),
        KAboutData::License_LGPL_V2,
        ki18n("(c) 2002-2011, Kexi Team"),
        ki18n("This software is developed by Kexi Team - an international group\n"
              "of independent developers."),
        "http://www.calligra.org/kexi",
        "submit@bugs.kde.org"
    );
    // authors sorted by last nontrivial contribution date
    aboutData->addAuthor(
        ki18n("Jarosław Staniek"), ki18n("Project maintainer & developer, design, KexiDB, commercially supported version, MS Windows version"), "staniek@kde.org");
    aboutData->addAuthor(
        ki18n("OpenOffice Polska LLC"), ki18n("Sponsoring and support (employer of Jarosław Staniek in 2003-2007)"), "it@openoffice.com.pl");
    aboutData->addAuthor(
        ki18n("Adam Pigg"), ki18n("PostgreSQL database driver, Migration and Reporting modules"), "adam@piggz.co.uk");
    aboutData->addAuthor(
        ki18n("Radosław Wicik"), ki18n("Map elements for forms and reports, map flake shape"), "radoslaw@wicik.pl");
    aboutData->addAuthor(
        ki18n("Shreya Pandit"), ki18n("Web elements for forms and reports"), "shreya.pandit25@gmail.com");
    aboutData->addAuthor(
        ki18n("Sebastian Sauer"), ki18n("Scripting module (KROSS), Python language bindings, design"), "mail@dipe.org");
    aboutData->addAuthor(
        ki18n("Sharan Rao"), ki18n("Sybase/MS SQL Server database drivers, xBase migration plugin, improvements for KexiDB"), "sharanrao@gmail.com");
    aboutData->addAuthor(
        ki18n("Cédric Pasteur"), ki18n("First version of Property Editor and Form Designer"), "cedric.pasteur@free.fr");
    aboutData->addAuthor(
        ki18n("Martin Ellis"), ki18n("Contributions for MySQL and KexiDB, fixes, Migration module, MS Access file format support"), "martin.ellis@kdemail.net");
    aboutData->addAuthor(
        ki18n("Christian Nitschkowski"), ki18n("Graphics effects, helper dialogs"), "segfault_ii@web.de");
    aboutData->addAuthor(
        ki18n("Lucijan Busch"), ki18n("Former project maintainer & developer"), "lucijan@kde.org");
    aboutData->addAuthor(
        ki18n("Peter Simonsson"), ki18n("Former developer"), "psn@linux.se");
    aboutData->addAuthor(
        ki18n("Joseph Wenninger"), ki18n("Original Form Designer, original user interface & much more"), "jowenn@kde.org");
    aboutData->addAuthor(
        ki18n("Seth Kurzenberg"), ki18n("CQL++, SQL assistance"),  "seth@cql.com");
    aboutData->addAuthor(
        ki18n("Laurent Montel"), ki18n("Original code cleanings"), "montel@kde.org");
    aboutData->addAuthor(
        ki18n("Till Busch"), ki18n("Bugfixes, original Table Widget"), "till@bux.at");

     aboutData->addCredit(
        ki18n("Dimitrios T. Tanis"), ki18n("Documentation for Kexi 2"),  "jtanis@tanisfood.gr");
    aboutData->addCredit(
        ki18n("Daniel Molkentin"), ki18n("Initial design improvements"),  "molkentin@kde.org");
    aboutData->addCredit(
        ki18n("Kristof Borrey"), ki18n("Icons and user interface research"), "kristof.borrey@skynet.be");
    aboutData->addCredit(
        ki18n("Tomas Krassnig"), ki18n("Coffee sponsoring"), "tkrass05@hak1.at");
    aboutData->addCredit(
        ki18n("Paweł Wirecki / OpenOffice Polska"), ki18n("Numerous bug reports, usability tests, technical support"));

    aboutData->setTranslator(
        ki18nc("NAME OF TRANSLATORS", "Your names"), ki18nc("EMAIL OF TRANSLATORS", "Your emails"));
#if defined(CUSTOM_VERSION) && defined(Q_WS_WIN)
    aboutData->setProgramLogo(KEXI_APP_LOGO);
#endif
    return aboutData;
}
