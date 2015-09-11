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

#ifndef TESTCUSTOMSLIDESHOWS_H
#define TESTCUSTOMSLIDESHOWS_H

#include <QObject>
class MockDocument;
class KoPAPageBase;

class TestCustomSlideShows : public QObject
{
    Q_OBJECT
private:
    void populateDoc(MockDocument &doc, QList<KoPAPageBase*> &slideList1, QList<KoPAPageBase*> &slideList2,
                     QString &customShowName1, QString &customShowName2);
private Q_SLOTS:
    void insertCustomSlideShow();
    void removeCustomSlideShow();
    void updateCustomSlideShow();
    void customSlideShowsNames();
    void getCustomSlideShowByName();
    void addSlideToAllCustomSlideShows();
    void addSlidesToAllCustomSlideShows();
    void removeSlideFromAllCustomSlideShows();
    void removeSlidesFromAllCustomSlideShows();
};

#endif // TESTCUSTOMSLIDESHOWS_H
