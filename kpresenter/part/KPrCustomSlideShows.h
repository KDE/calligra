/* This file is part of the KDE project
   Copyright (C) 2007-2008 Carlos Licea <carlos.licea@kdemail.org>
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

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
#ifndef KPRCUSTOMSLIDESHOWS_H
#define KPRCUSTOMSLIDESHOWS_H

//Qt includes
#include<QMap>
#include<QList>
#include<QString>

//Forward declarations
class KoXmlWriter;
class KoPAPageBase;
#include "KoXmlReaderForward.h"
class KoPASavingContext;
class KoPALoadingContext;
class KPrDocument;

#include "kpresenter_export.h"

class KPRESENTER_TEST_EXPORT KPrCustomSlideShows
{
public:
    KPrCustomSlideShows();
    ~KPrCustomSlideShows();

    /**
     * @brief Insert a new Custom SlideShow
     *
     * @param name The name of the new SlideShow
     * @param slideShow a list of the pages in the desired order 
     */
    void insert( const QString &name, const QList<KoPAPageBase*> &slideShow );

    /**
     * @brief Delete a Custom SlideShow by its name
     *
     * @param name The name of the SlideShow to be removed
     */
    void remove( const QString &name );

    /**
     * @brief Replaces a custom SlideShow with the given SlideShow
     *
     * @param name The name of the SlideShow to be updated
     * @param slideShow slideShow to be inserted
     */
    void update( const QString &name, const QList<KoPAPageBase*> &slideShow );

    /**
     * @brief Replaces a custom SlideShow with the given SlideShow
     *
     * @param oldName The name of the SlideShow to be renamed
     * @param newName The new name of the SlideShow
     */
    void rename( const QString &oldName, const QString &newName );

    /**
     * @brief Returns a list of the names of the known SlideShows
     *
     */
    const QList<QString> names() const;

    /**
     * @brief Ask if a name is in use and return the object
     * 
     * @param name the name of the custom SlideShow to be retrived
     * @return a List of the Pages in the SlideShow if it was found
     *         a blank list if the SlideShow name was not found
     */
    QList<KoPAPageBase*> getByName( const QString &name ) const;

    /**
     * @brief Insert a slide to the right of the given position of all the known custom SlideShows.
     * If the position is higher than the size of the list of the SlideShow, the slide is inserted at the end.
     * 
     * @param slide slide to be inserted
     * @param position the position at which the slide will be inserted.
     * If it's 0 it will be prepended to the list, if it's the size it will be appended to the list.
     * Negative numbers can be used to count backwards.
     */
    void addSlideToAll( KoPAPageBase* page, unsigned int position );

    /**
     * @brief Insert a  list of slides to the right of the given position of all the known custom SlideShows.
     * If the position is higher than the size of the list of the SlideShow, the list of slides are inserted at the end.
     *
     * @param slideShow list of slides to be inserted
     * @param position the position at which the slide will be inserted
     */
    void addSlidesToAll( const QList<KoPAPageBase*> &slideShow, unsigned int position );

    /**
     * @brief Deletes all the ocurrencies of a given slide from all the known custom SlideShows
     *
     * @param slide slide to be removed
     */
    void removeSlideFromAll( KoPAPageBase* page );

    /**
     * @brief Deletes all the ocurrencies of a given list of slides from all the known custom SlideShows
     *
     * @param slideShow list of slides to be removed
     */
    void removeSlidesFromAll( const QList<KoPAPageBase*> &slideShow );

    /**
     * @brief Save the slide shows
     *
     * @param context The saving context
     */
    void saveOdf( KoPASavingContext & context );

    /**
     * @brief Load the slide shows
     *
     * @param element The element containing the presentation:settings
     * @param context The loading context
     */
    void loadOdf( const KoXmlElement & presentationSettings, KoPALoadingContext & context );

private:
    QMap< QString, QList<KoPAPageBase*> > m_customSlideShows;
};
#endif /* KPRCUSTOMSLIDESHOWS_H */
