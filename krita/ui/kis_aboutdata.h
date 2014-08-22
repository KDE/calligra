/*
 *  kis_aboutdata.h - part of Krayon
 *
 *  Copyright (c) 1999-2000 Matthias Elter  <me@kde.org>
 *  Copyright (c) 2003-2007 Boudewijn Rempt <boud@valdyas.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_ABOUT_DATA_H_
#define KIS_ABOUT_DATA_H_

#include <kaboutdata.h>
#include <klocale.h>
#include <calligraversion.h>
#include <calligragitversion.h>
#include <KoIcon.h>

KAboutData *newKritaAboutData()
{
    QString calligraVersion(CALLIGRA_VERSION_STRING);
    QString version;


#ifdef CALLIGRA_GIT_SHA1_STRING
    QString gitVersion(CALLIGRA_GIT_SHA1_STRING);
    version = QString("%1 (git %2)").arg(calligraVersion).arg(gitVersion).toLatin1();
#else
    version = calligraVersion;
#endif

    KAboutData * aboutData = new KAboutData("krita", 0,
                                            ki18n("Krita"),
                                            version.toLatin1(),
                                            ki18n("Digital Painting for Artists"),
                                            KAboutData::License_GPL,
                                            ki18n("© 1999-%1, The Krita Team").subs(CALLIGRA_YEAR),
                                            KLocalizedString(),
                                            "http://www.krita.org",
                                            "submit@bugs.kde.org");

    aboutData->setProgramIconName(koIconName("calligrakrita"));

    aboutData->addAuthor(ki18n("Boudewijn Rempt"), ki18n("Project Maintainer"));

    aboutData->addAuthor(ki18n("Aaron J. Seigo"));
    aboutData->addAuthor(ki18n("Adam Celarek"));
    aboutData->addAuthor(ki18n("Adam Pigg"));
    aboutData->addAuthor(ki18n("Adriaan de Groot"));
    aboutData->addAuthor(ki18n("Adrian Page"));
    aboutData->addAuthor(ki18n("Adrian Schroeter"));
    aboutData->addAuthor(ki18n("Albert Astals Cid"));
    aboutData->addAuthor(ki18n("Alberto Villa"));
    aboutData->addAuthor(ki18n("Alexander Neundorf"));
    aboutData->addAuthor(ki18n("Alexander Potashev"));
    aboutData->addAuthor(ki18n("Alexis Ménard"));
    aboutData->addAuthor(ki18n("Alfredo Beaumont Sainz"));
    aboutData->addAuthor(ki18n("Allen Winter"));
    aboutData->addAuthor(ki18n("Ana Beatriz Guerrero López"));
    aboutData->addAuthor(ki18n("Andras Mantia"));
    aboutData->addAuthor(ki18n("Andreas Hartmetz"));
    aboutData->addAuthor(ki18n("Andreas Lundin"));
    aboutData->addAuthor(ki18n("André Marcelo Alvarenga"));
    aboutData->addAuthor(ki18n("Andrew Coles"));
    aboutData->addAuthor(ki18n("Andre Woebbeking"));
    aboutData->addAuthor(ki18n("Andrius da Costa Ribas"));
    aboutData->addAuthor(ki18n("Andy Fawcett"));
    aboutData->addAuthor(ki18n("Anne-Marie Mahfouf"));
    aboutData->addAuthor(ki18n("Ariya Hidayat"));
    aboutData->addAuthor(ki18n("Arjen Hiemstra"));
    aboutData->addAuthor(ki18n("Bart Coppens"));
    aboutData->addAuthor(ki18n("Ben Cooksley"));
    aboutData->addAuthor(ki18n("Benjamin K. Stuhl"));
    aboutData->addAuthor(ki18n("Benjamin Meyer"));
    aboutData->addAuthor(ki18n("Benjamin Reed"));
    aboutData->addAuthor(ki18n("Benoît Jacob"));
    aboutData->addAuthor(ki18n("Ben Schleimer"));
    aboutData->addAuthor(ki18n("Bernhard Rosenkraenzer"));
    aboutData->addAuthor(ki18n("Bo Thorsen"));
    aboutData->addAuthor(ki18n("Brad Hards"));
    aboutData->addAuthor(ki18n("Bram Schoenmakers"));
    aboutData->addAuthor(ki18n("Burkhard Lück"));
    aboutData->addAuthor(ki18n("Carlo Segato"));
    aboutData->addAuthor(ki18n("C. Boemann"));
    aboutData->addAuthor(ki18n("Christer Stenbrenden"));
    aboutData->addAuthor(ki18n("Christian Ehrlicher"));
    aboutData->addAuthor(ki18n("Christian Mueller"));
    aboutData->addAuthor(ki18n("Christoph Feck"));
    aboutData->addAuthor(ki18n("Chusslove Illich"));
    aboutData->addAuthor(ki18n("Clarence Dang"));
    aboutData->addAuthor(ki18n("Cyrille Berger"));
    aboutData->addAuthor(ki18n("Daniel M. Duley"));
    aboutData->addAuthor(ki18n("Daniel Molkentin"));
    aboutData->addAuthor(ki18n("Dan Leinir Turthra Jensen"));
    aboutData->addAuthor(ki18n("Dan Meltzer"));
    aboutData->addAuthor(ki18n("Danny Allen"));
    aboutData->addAuthor(ki18n("David Faure"));
    aboutData->addAuthor(ki18n("David Gowers"));
    aboutData->addAuthor(ki18n("Dirk Mueller"));
    aboutData->addAuthor(ki18n("Dirk Schönberger"));
    aboutData->addAuthor(ki18n("Dmitry Kazakov"));
    aboutData->addAuthor(ki18n("Edward Apap"));
    aboutData->addAuthor(ki18n("Elvis Stansvik"));
    aboutData->addAuthor(ki18n("Emanuele Tamponi"));
    aboutData->addAuthor(ki18n("Enrique Matías Sánchez"));
    aboutData->addAuthor(ki18n("Fabian Kosmale"));
    aboutData->addAuthor(ki18n("Frank Osterfeld"));
    aboutData->addAuthor(ki18n("Frederik Schwarzer"));
    aboutData->addAuthor(ki18n("Fredrik Edemar"));
    aboutData->addAuthor(ki18n("Fredy Yanardi"));
    aboutData->addAuthor(ki18n("Friedrich W. H. Kossebau"));
    aboutData->addAuthor(ki18n("Gábor Lehel"));
    aboutData->addAuthor(ki18n("Gary Cramblitt"));
    aboutData->addAuthor(ki18n("Geoffry Song"));
    aboutData->addAuthor(ki18n("Gioele Barabucci"));
    aboutData->addAuthor(ki18n("Giovanni Venturi"));
    aboutData->addAuthor(ki18n("Gopalakrishna Bhat A"));
    aboutData->addAuthor(ki18n("Hanna Scott"));
    aboutData->addAuthor(ki18n("Harald Sitter"));
    aboutData->addAuthor(ki18n("Hasso Tepper"));
    aboutData->addAuthor(ki18n("Helge Deller"));
    aboutData->addAuthor(ki18n("Helio Castro"));
    aboutData->addAuthor(ki18n("Hoàng Đức Hiếu"));
    aboutData->addAuthor(ki18n("Hugo Pereira Da Costa"));
    aboutData->addAuthor(ki18n("Inge Wallin"));
    aboutData->addAuthor(ki18n("Ingo Klöcker"));
    aboutData->addAuthor(ki18n("İsmail Dönmez"));
    aboutData->addAuthor(ki18n("Ivan Yossi"));
    aboutData->addAuthor(ki18n("Jaime"));
    aboutData->addAuthor(ki18n("Jaime Torres"));
    aboutData->addAuthor(ki18n("Jaison Lee"));
    aboutData->addAuthor(ki18n("Jakob Petsovits"));
    aboutData->addAuthor(ki18n("Jakub Stachowski"));
    aboutData->addAuthor(ki18n("Jan Hambrecht"));
    aboutData->addAuthor(ki18n("Jarosław Staniek"));
    aboutData->addAuthor(ki18n("Jens Herden"));
    aboutData->addAuthor(ki18n("Jessica Hall"));
    aboutData->addAuthor(ki18n("Johannes Simon"));
    aboutData->addAuthor(ki18n("John Layt"));
    aboutData->addAuthor(ki18n("Jonathan Riddell"));
    aboutData->addAuthor(ki18n("Jonathan Singer"));
    aboutData->addAuthor(ki18n("José Luis Vergara"));
    aboutData->addAuthor(ki18n("Juan Luis Boya García"));
    aboutData->addAuthor(ki18n("Juan Palacios"));
    aboutData->addAuthor(ki18n("Jure Repinc"));
    aboutData->addAuthor(ki18n("Kai-Uwe Behrmann"));
    aboutData->addAuthor(ki18n("Kevin Krammer"));
    aboutData->addAuthor(ki18n("Kevin Ottens"));
    aboutData->addAuthor(ki18n("Kurt Pfeifle"));
    aboutData->addAuthor(ki18n("Laurent Montel"));
    aboutData->addAuthor(ki18n("Lauri Watts"));
    aboutData->addAuthor(ki18n("Leo Savernik"));
    aboutData->addAuthor(ki18n("Lukáš Tinkl"));
    aboutData->addAuthor(ki18n("Lukáš Tvrdý"));
    aboutData->addAuthor(ki18n("Maciej Mrozowski"));
    aboutData->addAuthor(ki18n("Malcolm Hunter"));
    aboutData->addAuthor(ki18n("Manuel Riecke"));
    aboutData->addAuthor(ki18n("manu tortosa"));
    aboutData->addAuthor(ki18n("Marc Pegon"));
    aboutData->addAuthor(ki18n("Marijn Kruisselbrink"));
    aboutData->addAuthor(ki18n("Martin Ellis"));
    aboutData->addAuthor(ki18n("Martin Gräßlin"));
    aboutData->addAuthor(ki18n("Matthew Woehlke"));
    aboutData->addAuthor(ki18n("Matthias Klumpp"));
    aboutData->addAuthor(ki18n("Matthias Kretz"));
    aboutData->addAuthor(ki18n("Matus Talcik"));
    aboutData->addAuthor(ki18n("Maximiliano Curia"));
    aboutData->addAuthor(ki18n("Melchior Franz"));
    aboutData->addAuthor(ki18n("Michael David Howell"));
    aboutData->addAuthor(ki18n("Michael Drueing"));
    aboutData->addAuthor(ki18n("Michael Thaler"));
    aboutData->addAuthor(ki18n("Michel Hermier"));
    aboutData->addAuthor(ki18n("Mohit Goyal"));
    aboutData->addAuthor(ki18n("Mojtaba Shahi Senobari"));
    aboutData->addAuthor(ki18n("Montel Laurent"));
    aboutData->addAuthor(ki18n("Nick Shaforostoff"));
    aboutData->addAuthor(ki18n("Nicolas Goutte"));
    aboutData->addAuthor(ki18n("Olivier Goffart"));
    aboutData->addAuthor(ki18n("Patrick Julien"));
    aboutData->addAuthor(ki18n("Patrick Spendrin"));
    aboutData->addAuthor(ki18n("Pavel Heimlich"));
    aboutData->addAuthor(ki18n("Peter Simonsson"));
    aboutData->addAuthor(ki18n("Pierre Ducroquet"));
    aboutData->addAuthor(ki18n("Pierre Stirnweiss"));
    aboutData->addAuthor(ki18n("Pino Toscano"));
    aboutData->addAuthor(ki18n("Rafael Fernández López"));
    aboutData->addAuthor(ki18n("Raphael Langerhorst"));
    aboutData->addAuthor(ki18n("Rex Dieter"));
    aboutData->addAuthor(ki18n("Rob Buis"));
    aboutData->addAuthor(ki18n("Roopesh Chander"));
    aboutData->addAuthor(ki18n("Sahil Nagpal"));
    aboutData->addAuthor(ki18n("Salil Kapur"));
    aboutData->addAuthor(ki18n("Samuel Buttigieg"));
    aboutData->addAuthor(ki18n("Sander Koning"));
    aboutData->addAuthor(ki18n("Sascha Suelzer"));
    aboutData->addAuthor(ki18n("Scott Petrovic"));
    aboutData->addAuthor(ki18n("Scott Wheeler"));
    aboutData->addAuthor(ki18n("Sebastian Sauer"));
    aboutData->addAuthor(ki18n("Shivaraman Aiyer"));
    aboutData->addAuthor(ki18n("Siddharth Sharma"));
    aboutData->addAuthor(ki18n("Silvio Heinrich"));
    aboutData->addAuthor(ki18n("Somsubhra Bairi"));
    aboutData->addAuthor(ki18n("Spencer Brown"));
    aboutData->addAuthor(ki18n("Srikanth Tiyyagura"));
    aboutData->addAuthor(ki18n("Stefan Nikolaus"));
    aboutData->addAuthor(ki18n("Stephan Binner"));
    aboutData->addAuthor(ki18n("Stephan Kulow"));
    aboutData->addAuthor(ki18n("Stuart Dickson"));
    aboutData->addAuthor(ki18n("Sune Vuorela"));
    aboutData->addAuthor(ki18n("Sven Langkamp"));
    aboutData->addAuthor(ki18n("Thiago Macieira"));
    aboutData->addAuthor(ki18n("Thomas Capricelli"));
    aboutData->addAuthor(ki18n("Thomas Friedrichsmeier"));
    aboutData->addAuthor(ki18n("Thomas Klausner"));
    aboutData->addAuthor(ki18n("Thomas Nagy"));
    aboutData->addAuthor(ki18n("Thomas Zander"));
    aboutData->addAuthor(ki18n("Thorsten Staerk"));
    aboutData->addAuthor(ki18n("Thorsten Zachmann"));
    aboutData->addAuthor(ki18n("Tim Beaulen"));
    aboutData->addAuthor(ki18n("Timothée Giet"));
    aboutData->addAuthor(ki18n("Tobias Koenig"));
    aboutData->addAuthor(ki18n("Tom Burdick"));
    aboutData->addAuthor(ki18n("Torio Mlshi"));
    aboutData->addAuthor(ki18n("Torsten Rahn"));
    aboutData->addAuthor(ki18n("Unai Garro"));
    aboutData->addAuthor(ki18n("Urs Wolfer"));
    aboutData->addAuthor(ki18n("Vadim Zhukov"));
    aboutData->addAuthor(ki18n("Vera Lukman"));
    aboutData->addAuthor(ki18n("Victor Lafon"));
    aboutData->addAuthor(ki18n("Volker Krause"));
    aboutData->addAuthor(ki18n("Waldo Bastian"));
    aboutData->addAuthor(ki18n("Werner Trobin"));
    aboutData->addAuthor(ki18n("Wilco Greven"));
    aboutData->addAuthor(ki18n("Will Entriken"));
    aboutData->addAuthor(ki18n("William Steidtmann"));
    aboutData->addAuthor(ki18n("Wolthera van Hovell"));
    aboutData->addAuthor(ki18n("Yann Bodson"));
    aboutData->addAuthor(ki18n("Yue Liu"));
    aboutData->addAuthor(ki18n("Yuri Chornoivan"));

    aboutData->addCredit(ki18n("Peter Sikking"), k1i8n("Project Vision"));
    aboutData->addCredit(ki18n("Ramon Miranda"), ki18n("Artist, Muses author, brush and preset creator"));
    aboutData->addCredit(ki18n("David Revoy"), ki18n("Brushes and Palettes"));
    aboutData->addCredit(ki18n("Ilya Portnov"), ki18n("MyPaint shade selector"));
    aboutData->addCredit(ki18n("Martin Renold"), ki18n("MyPaint shade selector"));
    aboutData->addCredit(ki18n("Sander Koning"), k1i8n("Icon Management"));


    return aboutData;
}

#endif // KIS_ABOUT_DATA_H_
