/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TestCustomSlideShows.h"

#include <KPrCustomSlideShows.h>

void TestCustomSlideShows::insertCustomSlideShow()
{
    // create some slide shows and insert then then test if you can access them again
}

void TestCustomSlideShows::removeCustomSlideShow()
{
    // create some slide shows and insert then then test if you can access them again
    // then remove a´the slideshows again and test of they are no longer there
}

void TestCustomSlideShows::updateCustomSlideShow()
{
    // create some slide shows and insert then then test if you can access them again
    // modify a slide show and update it
    // test if you get the updated slide show
}

void TestCustomSlideShows::customSlideShowsNames()
{
    // insert different slide shows
    // test if you get the correct name of the slide shows
}

void TestCustomSlideShows::getCustomSlideShowByName()
{
    // insert some slide shows
    // test if you can get each slide show correctly
}

void TestCustomSlideShows::addSlideToAllCustomSlideShows()
{
}

void TestCustomSlideShows::addSlidesToAllCustomSlideShows()
{
}

void TestCustomSlideShows::removeSlideFromAllCustomSlideShows()
{
}

void TestCustomSlideShows::removeSlidesFromAllCustomSlideShows()
{
}

QTEST_MAIN(TestCustomSlideShows)
#include "TestCustomSlideShows.moc"
