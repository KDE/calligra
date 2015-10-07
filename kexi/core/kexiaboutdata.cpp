/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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
#include <KLocalizedString>

static const char description[] =
    I18N_NOOP("Visual database applications creator");

KexiAboutData::KexiAboutData()
 : KAboutData(
        "kexi",
        KEXI_APP_NAME,
        Kexi::fullVersionString(),
        xi18n(description),
        KAboutLicense::LGPL_V2,
        xi18n("© 2002-%1, The Kexi Team", QLatin1String(CALLIGRA_YEAR)),
        xi18n("This software is developed by Kexi Team - an international group "
              "of independent developers. They form a part of the Calligra Project."),
        "https://www.calligra.org/kexi",
        "submit@bugs.kde.org"
    )
{
    setOrganizationDomain("kde.org"); // right dbus prefix == org.kde.

    // authors sorted by last nontrivial contribution date * size
    addAuthor(
        xi18n("Jarosław Staniek"), xi18n("Project maintainer & developer, overall design"), "staniek@kde.org");
    addAuthor(
        xi18n("OpenOffice Polska LLC"), xi18n("Sponsoring and support (employer of Jarosław Staniek in 2003-2007)"), "info@openoffice.com.pl");
    addAuthor(
        xi18n("Adam Pigg"), xi18n("PostgreSQL database driver, Migration and Reporting modules, numerous bug fixes"), "adam@piggz.co.uk");
    addAuthor(
        xi18n("Radosław Wicik"), xi18n("Map elements for forms and reports, map flake shape"), "radoslaw@wicik.pl");
    addAuthor(
        xi18n("Wojciech Kosowicz"), xi18n("Features and bug fixes"), "pcellix@gmail.com");
    addAuthor(
        xi18n("Roman Shtemberko"), xi18n("Features and bug fixes"), "shtemberko@gmail.com");
    addAuthor(
        xi18n("Dimitrios T. Tanis"), xi18n("Users Manual for Kexi 2, main window improvements, numerous bug reports"),  "dimitrios.tanis@kdemail.net");
    addAuthor(
        xi18n("Oleg Kukharchuk"), xi18n("Several form widgets, porting to Qt 4, stabilization"), "oleg.kuh@gmail.com");
    addAuthor(
        xi18n("Shreya Pandit"), xi18n("Web elements for forms and reports"), "shreya.pandit25@gmail.com");
    addAuthor(
        xi18n("Sebastian Sauer"), xi18n("Scripting module (KROSS), Python language bindings, design"), "mail@dipe.org");
    addAuthor(
        xi18n("Lorenzo Villani"), xi18n("Web Forms module"), "lvillani@binaryhelix.net");
    addAuthor(
        xi18n("Sharan Rao"), xi18n("Sybase/MS SQL Server/ODBC database drivers, xBase migration plugin, improvements for KexiDB"), "sharanrao@gmail.com");
    addAuthor(
        xi18n("Cédric Pasteur"), xi18n("First version of Property Editor and Form Designer"), "cedric.pasteur@free.fr");
    addAuthor(
        xi18n("Martin Ellis"), xi18n("Contributions for MySQL and KexiDB, fixes, Migration module, MS Access file format support"), "martin.ellis@kdemail.net");
    addAuthor(
        xi18n("Julia Sanchez-Simon"), xi18n("Oracle database driver"), "hithwen@gmail.com");
    addAuthor(
        xi18n("Christian Nitschkowski"), xi18n("Graphics effects, helper dialogs"), "segfault_ii@web.de");
    addAuthor(
        xi18n("Matt Rogers"), xi18n("ODBC database driver"), "mattr@kde.org");
    addAuthor(
        xi18n("Lucijan Busch"), xi18n("Former project maintainer & developer"), "lucijan@kde.org");
    addAuthor(
        xi18n("Peter Simonsson"), xi18n("Former developer"), "psn@linux.se");
    addAuthor(
        xi18n("Joseph Wenninger"), xi18n("Original Form Designer, original user interface & much more"), "jowenn@kde.org");
    addAuthor(
        xi18n("Seth Kurzenberg"), xi18n("CQL++, SQL assistance"),  "seth@cql.com");
    addAuthor(
        xi18n("Laurent Montel"), xi18n("Original code cleanings"), "montel@kde.org");
    addAuthor(
        xi18n("Till Busch"), xi18n("Bugfixes, original Table Widget"), "till@bux.at");

    addCredit(
        xi18n("Ian Balchin"), xi18n("Numerous bug reports and tests, handbook improvements"), "inksi@fables.co.za");
    addCredit(
        xi18n("Robert Leleu"), xi18n("Numerous bug reports and tests"), "robert.jean.leleu@wanadoo.fr");
    addCredit(
        xi18n("Friedrich W. H. Kossebau"), xi18n("Bug fixes, build system improvements, code cleanups"), "kossebau@kde.org");
    addCredit(
        xi18n("Boudewijn Rempt"), xi18n("Code cleanups"), "boud@valdyas.org");
    addCredit(
        xi18n("David Faure"), xi18n("Code cleanups"), "faure@kde.org");
    addCredit(
        xi18n("Daniel Molkentin"), xi18n("Initial design improvements"),  "molkentin@kde.org");
    addCredit(
        xi18n("Kristof Borrey"), xi18n("Icons and user interface research"), "kristof.borrey@skynet.be");
    addCredit(
        xi18n("Tomas Krassnig"), xi18n("Coffee sponsoring"), "tkrass05@hak1.at");
    addCredit(
        xi18n("Paweł Wirecki / OpenOffice Polska"), xi18n("Numerous bug reports, usability tests, technical support"));

    setTranslator(
        xi18nc("NAME OF TRANSLATORS", "Your names"), xi18nc("EMAIL OF TRANSLATORS", "Your emails"));
}
