/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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
#include <KoIcon.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("Visual database applications creator");

KexiAboutData::KexiAboutData()
 : KAboutData(
        "kexi", 0,
        ki18n(KEXI_APP_NAME),
        Kexi::fullVersionString(),
        ki18n(description),
        KAboutData::License_LGPL_V2,
        ki18n("© 2002-%1, The Kexi Team").subs(CALLIGRA_YEAR),
        ki18n("This software is developed by Kexi Team - an international group "
              "of independent developers. They form a part of the Calligra Project."),
        "http://www.calligra.org/kexi",
        "submit@bugs.kde.org"
    )
{
    setProgramIconName(koIconName("calligrakexi"));
    // authors sorted by last nontrivial contribution date * size
    addAuthor(
        ki18n("Jarosław Staniek"), ki18n("Project maintainer & developer, overall design"), "staniek@kde.org");
    addAuthor(
        ki18n("OpenOffice Polska LLC"), ki18n("Sponsoring and support (employer of Jarosław Staniek in 2003-2007)"), "info@openoffice.com.pl");
    addAuthor(
        ki18n("Adam Pigg"), ki18n("PostgreSQL database driver, Migration and Reporting modules, numerous bug fixes"), "adam@piggz.co.uk");
    addAuthor(
        ki18n("Radosław Wicik"), ki18n("Map elements for forms and reports, map flake shape"), "radoslaw@wicik.pl");
    addAuthor(
        ki18n("Wojciech Kosowicz"), ki18n("Features and bug fixes"), "pcellix@gmail.com");
    addAuthor(
        ki18n("Roman Shtemberko"), ki18n("Features and bug fixes"), "shtemberko@gmail.com");
    addAuthor(
        ki18n("Dimitrios T. Tanis"), ki18n("Users Manual for Kexi 2, main window improvements, numerous bug reports"),  "dimitrios.tanis@kdemail.net");
    addAuthor(
        ki18n("Oleg Kukharchuk"), ki18n("Several form widgets, porting to Qt 4, stabilization"), "oleg.kuh@gmail.com");
    addAuthor(
        ki18n("Shreya Pandit"), ki18n("Web elements for forms and reports"), "shreya.pandit25@gmail.com");
    addAuthor(
        ki18n("Sebastian Sauer"), ki18n("Scripting module (KROSS), Python language bindings, design"), "mail@dipe.org");
    addAuthor(
        ki18n("Lorenzo Villani"), ki18n("Web Forms module"), "lvillani@binaryhelix.net");
    addAuthor(
        ki18n("Sharan Rao"), ki18n("Sybase/MS SQL Server/ODBC database drivers, xBase migration plugin, improvements for KexiDB"), "sharanrao@gmail.com");
    addAuthor(
        ki18n("Cédric Pasteur"), ki18n("First version of Property Editor and Form Designer"), "cedric.pasteur@free.fr");
    addAuthor(
        ki18n("Martin Ellis"), ki18n("Contributions for MySQL and KexiDB, fixes, Migration module, MS Access file format support"), "martin.ellis@kdemail.net");
    addAuthor(
        ki18n("Julia Sanchez-Simon"), ki18n("Oracle database driver"), "hithwen@gmail.com");
    addAuthor(
        ki18n("Christian Nitschkowski"), ki18n("Graphics effects, helper dialogs"), "segfault_ii@web.de");
    addAuthor(
        ki18n("Matt Rogers"), ki18n("ODBC database driver"), "mattr@kde.org");
    addAuthor(
        ki18n("Lucijan Busch"), ki18n("Former project maintainer & developer"), "lucijan@kde.org");
    addAuthor(
        ki18n("Peter Simonsson"), ki18n("Former developer"), "psn@linux.se");
    addAuthor(
        ki18n("Joseph Wenninger"), ki18n("Original Form Designer, original user interface & much more"), "jowenn@kde.org");
    addAuthor(
        ki18n("Seth Kurzenberg"), ki18n("CQL++, SQL assistance"),  "seth@cql.com");
    addAuthor(
        ki18n("Laurent Montel"), ki18n("Original code cleanings"), "montel@kde.org");
    addAuthor(
        ki18n("Till Busch"), ki18n("Bugfixes, original Table Widget"), "till@bux.at");

    addCredit(
        ki18n("Ian Balchin"), ki18n("Numerous bug reports and tests, handbook improvements"), "inksi@fables.co.za");
    addCredit(
        ki18n("Robert Leleu"), ki18n("Numerous bug reports and tests"), "robert.jean.leleu@wanadoo.fr");
    addCredit(
        ki18n("Friedrich W. H. Kossebau"), ki18n("Bug fixes, build system improvements, code cleanups"), "kossebau@kde.org");
    addCredit(
        ki18n("Boudewijn Rempt"), ki18n("Code cleanups"), "boud@valdyas.org");
    addCredit(
        ki18n("David Faure"), ki18n("Code cleanups"), "faure@kde.org");
    addCredit(
        ki18n("Daniel Molkentin"), ki18n("Initial design improvements"),  "molkentin@kde.org");
    addCredit(
        ki18n("Kristof Borrey"), ki18n("Icons and user interface research"), "kristof.borrey@skynet.be");
    addCredit(
        ki18n("Tomas Krassnig"), ki18n("Coffee sponsoring"), "tkrass05@hak1.at");
    addCredit(
        ki18n("Paweł Wirecki / OpenOffice Polska"), ki18n("Numerous bug reports, usability tests, technical support"));

    setTranslator(
        ki18nc("NAME OF TRANSLATORS", "Your names"), ki18nc("EMAIL OF TRANSLATORS", "Your emails"));
}
