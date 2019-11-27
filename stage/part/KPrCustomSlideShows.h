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
#include<QObject>

//Forward declarations
class KoPAPageBase;
#include "KoXmlReaderForward.h"
class KoPASavingContext;
class KoPALoadingContext;

#include "stage_export.h"

class STAGE_TEST_EXPORT KPrCustomSlideShows : public QObject
{
    Q_OBJECT
public:
    KPrCustomSlideShows();
    ~KPrCustomSlideShows() override;

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
     * @brief Return the object that match the give index in the list with the given name
     *
     * @param name the name of the custom SlideShow
     / @param index the index of the slide to be retrived from the custom SlideShow
     * @return a Page in the SlideShow if it was found or a null pointer if not.
     */
    KoPAPageBase* pageByIndex(const QString &name, int index) const;

    /**
     * @brief return the index of a page within a custom show
     *
     * @param name the name of the custom SlideShow
     * @param page the KoPAPageBase within the custom SlideShow
     * @return a int index of the page within the SlideShow if it was found
     *         -1 if the page was not found
     */
    int indexByPage(const QString &name, KoPAPageBase *page) const;

    /**
     * @brief Insert a slide to the right of the given position of all the known custom SlideShows.
     * If the position is higher than the size of the list of the SlideShow, the slide is inserted at the end.
     * 
     * @param page slide page to be inserted
     * @param position the position at which the slide will be inserted.
     * If it's 0 it will be prepended to the list, if it's the size it will be appended to the list.
     * Negative numbers can be used to count backwards.
     */
    void addSlideToAll(KoPAPageBase *page, unsigned int position);

    /**
     * @brief Insert a  list of slides to the right of the given position of all the known custom SlideShows.
     * If the position is higher than the size of the list of the SlideShow, the list of slides are inserted at the end.
     *
     * @param slideShow list of slides to be inserted
     * @param position the position at which the slide will be inserted
     */
    void addSlidesToAll( const QList<KoPAPageBase*> &slideShow, unsigned int position );

    /**
     * @brief Deletes all the occurrences of a given slide from all the known custom SlideShows
     *
     * @param page slide page to be removed
     */
    void removeSlideFromAll(KoPAPageBase *page);

    /**
     * @brief Deletes all the occurrences of a given list of slides from all the known custom SlideShows
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
     * @param presentationSettings The element containing the presentation:settings
     * @param context The loading context
     */
    void loadOdf( const KoXmlElement & presentationSettings, KoPALoadingContext & context );

    /**
     * @brief Return the names of custom shows that contains the given page
     *
     * @param page the page to be searched in the custom shows
     * @return a List of the Slide Show names that contains the page
     *         a blank list if the page was not found
     */
    QStringList namesByPage(KoPAPageBase *page) const;

Q_SIGNALS:
    void updated();

private:
    QMap< QString, QList<KoPAPageBase*> > m_customSlideShows;

};
#endif /* KPRCUSTOMSLIDESHOWS_H */
