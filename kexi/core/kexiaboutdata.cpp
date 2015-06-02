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
#include <KexiIcon.h>
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
        kxi18n("© 2002-%1, The Kexi Team").subs(CALLIGRA_YEAR),
        kxi18n("This software is developed by Kexi Team - an international group "
              "of independent developers. They form a part of the Calligra Project."),
        "http://www.calligra.org/kexi",
        "submit@bugs.kde.org"
    )
{
    setProgramIconName(koIconName("calligrakexi"));
    // authors sorted by last nontrivial contribution date * size
    addAuthor(
        kxi18n("Jarosław Staniek"), kxi18n("Project maintainer & developer, overall design"), "staniek@kde.org");
    addAuthor(
        kxi18n("OpenOffice Polska LLC"), kxi18n("Sponsoring and support (employer of Jarosław Staniek in 2003-2007)"), "info@openoffice.com.pl");
    addAuthor(
        kxi18n("Adam Pigg"), kxi18n("PostgreSQL database driver, Migration and Reporting modules, numerous bug fixes"), "adam@piggz.co.uk");
    addAuthor(
        kxi18n("Radosław Wicik"), kxi18n("Map elements for forms and reports, map flake shape"), "radoslaw@wicik.pl");
    addAuthor(
        kxi18n("Wojciech Kosowicz"), kxi18n("Features and bug fixes"), "pcellix@gmail.com");
    addAuthor(
        kxi18n("Roman Shtemberko"), kxi18n("Features and bug fixes"), "shtemberko@gmail.com");
    addAuthor(
        kxi18n("Dimitrios T. Tanis"), kxi18n("Users Manual for Kexi 2, main window improvements, numerous bug reports"),  "dimitrios.tanis@kdemail.net");
    addAuthor(
        kxi18n("Oleg Kukharchuk"), kxi18n("Several form widgets, porting to Qt 4, stabilization"), "oleg.kuh@gmail.com");
    addAuthor(
        kxi18n("Shreya Pandit"), kxi18n("Web elements for forms and reports"), "shreya.pandit25@gmail.com");
    addAuthor(
        kxi18n("Sebastian Sauer"), kxi18n("Scripting module (KROSS), Python language bindings, design"), "mail@dipe.org");
    addAuthor(
        kxi18n("Lorenzo Villani"), kxi18n("Web Forms module"), "lvillani@binaryhelix.net");
    addAuthor(
        kxi18n("Sharan Rao"), kxi18n("Sybase/MS SQL Server/ODBC database drivers, xBase migration plugin, improvements for KexiDB"), "sharanrao@gmail.com");
    addAuthor(
        kxi18n("Cédric Pasteur"), kxi18n("First version of Property Editor and Form Designer"), "cedric.pasteur@free.fr");
    addAuthor(
        kxi18n("Martin Ellis"), kxi18n("Contributions for MySQL and KexiDB, fixes, Migration module, MS Access file format support"), "martin.ellis@kdemail.net");
    addAuthor(
        kxi18n("Julia Sanchez-Simon"), kxi18n("Oracle database driver"), "hithwen@gmail.com");
    addAuthor(
        kxi18n("Christian Nitschkowski"), kxi18n("Graphics effects, helper dialogs"), "segfault_ii@web.de");
    addAuthor(
        kxi18n("Matt Rogers"), kxi18n("ODBC database driver"), "mattr@kde.org");
    addAuthor(
        kxi18n("Lucijan Busch"), kxi18n("Former project maintainer & developer"), "lucijan@kde.org");
    addAuthor(
        kxi18n("Peter Simonsson"), kxi18n("Former developer"), "psn@linux.se");
    addAuthor(
        kxi18n("Joseph Wenninger"), kxi18n("Original Form Designer, original user interface & much more"), "jowenn@kde.org");
    addAuthor(
        kxi18n("Seth Kurzenberg"), kxi18n("CQL++, SQL assistance"),  "seth@cql.com");
    addAuthor(
        kxi18n("Laurent Montel"), kxi18n("Original code cleanings"), "montel@kde.org");
    addAuthor(
        kxi18n("Till Busch"), kxi18n("Bugfixes, original Table Widget"), "till@bux.at");

    addCredit(
        kxi18n("Ian Balchin"), kxi18n("Numerous bug reports and tests, handbook improvements"), "inksi@fables.co.za");
    addCredit(
        kxi18n("Robert Leleu"), kxi18n("Numerous bug reports and tests"), "robert.jean.leleu@wanadoo.fr");
    addCredit(
        kxi18n("Friedrich W. H. Kossebau"), kxi18n("Bug fixes, build system improvements, code cleanups"), "kossebau@kde.org");
    addCredit(
        kxi18n("Boudewijn Rempt"), kxi18n("Code cleanups"), "boud@valdyas.org");
    addCredit(
        kxi18n("David Faure"), kxi18n("Code cleanups"), "faure@kde.org");
    addCredit(
        kxi18n("Daniel Molkentin"), kxi18n("Initial design improvements"),  "molkentin@kde.org");
    addCredit(
        kxi18n("Kristof Borrey"), kxi18n("Icons and user interface research"), "kristof.borrey@skynet.be");
    addCredit(
        kxi18n("Tomas Krassnig"), kxi18n("Coffee sponsoring"), "tkrass05@hak1.at");
    addCredit(
        kxi18n("Paweł Wirecki / OpenOffice Polska"), kxi18n("Numerous bug reports, usability tests, technical support"));

    setTranslator(
        kxi18nc("NAME OF TRANSLATORS", "Your names"), kxi18nc("EMAIL OF TRANSLATORS", "Your emails"));
}
