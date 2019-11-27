/*  This file is part of the KDE project
    Copyright (C) 2008 James Hogan <james@albanarts.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA
*/

#ifndef KPRVIEWADAPTOR_H
#define KPRVIEWADAPTOR_H

#ifndef QT_NO_DBUS

#include <KoViewAdaptor.h>
#include <QStringList>

class KPrView;

class KPrViewAdaptor : public KoViewAdaptor
{
    Q_OBJECT
    Q_CLASSINFO( "D-Bus Interface", "org.kde.calligra.presentation.view" )

public:
    explicit KPrViewAdaptor( KPrView* view );
    ~KPrViewAdaptor() override;

public Q_SLOTS:

    // custom slideshows

    /**
     * Get a list of custom slide show names.
     *
     * @return List of custom slide show names not including the empty string
     *         (for all slides)
     */
    QStringList customSlideShows() const;

    /**
     * Get the current active slide show name.
     *
     * @return Name of current slide show or empty string for all slides
     */
    QString activeCustomSlideShow() const;

    /**
     * Set the active custom slide show.
     *
     * @param name name of the custom slide show to activate or empty string
     *             for all slides
     * @return true if the custom slideshow was changed
     */
    bool setActiveCustomSlideShow( const QString &name );

    // slides in the custom slideshow

    /**
     * Get the number of slides in the current slide show.
     */
    int numCustomSlideShowSlides() const;

    /**
     * Get the name of a page.
     *
     * If the page does not have a name an empty string is returned.
     * It is common in this case to use the string i18n( "Page %1", @p page + 1 ).
     *
     * @param page The page index within the current slideshow.
     * @returns The name of page with index @p page in the current slideshow.
     */
    QString pageName( int page ) const;

    /**
     * Get the notes associated with a page of the slideshow.
     *
     * The notes can be obtained in plain text or HTML.
     *
     * @param page The page index within the current slideshow.
     * @param format The format of the return value.  Possible values are "plain" and "html".
     * @returns The notes associated with page @p page in the format specified by @p format.
     *          An empty string is returned when @p format is not recognised or @p page is invalid.
     */
    QString pageNotes( int page, const QString &format ) const;

    /**
     * Save page to an image file.
     *
     * Export a page of the current presentation to disk
     * using a bitmap file like e.g. PNG
     * This method uses a QPixmap::save() call.
     *
     * @param page the page index within the current slideshow
     * @param width the desired image width in px
     * @param height the desired image height in px
     * @param filename the name of the image file to be created
     * @param format the format of the image file (see QPixmap::save())
     * @param quality the quality of the image in [0,100] or -1 to use default (see QPixmap::save())
     *
     * @returns whether the image was successfully saved
     */
    bool exportPageThumbnail( int page, int width, int height,
                              const QString &filename, const QString &format, int quality );

    // Presentation control
    void presentationStart();
    void presentationStartFromFirst();
    void presentationStop();
    void presentationPrev();
    void presentationNext();
    void presentationPrevSlide();
    void presentationNextSlide();
    void presentationFirst();
    void presentationLast();
    void gotoPresentationPage( int pg );

    // Presentation accessors
    bool isPresentationRunning() const;
    int currentPresentationPage() const;
    int currentPresentationStep() const;
    int numStepsInPresentationPage() const;
    int numPresentationPages() const;

Q_SIGNALS:
    /**
     * Emitted when the active custom slide show changes.
     *
     * @param customSlideShow the new active custom slide show
     */
    void activeCustomSlideShowChanged( const QString &customSlideShow );

    /**
     * Emitted when the custom slide shows have been modified.
     */
    void customSlideShowsModified();

    /**
     * Emitted when the slideshow is started.
     *
     * @param numSlides Number of slides in the slideshow
     */
    void presentationStarted( int numSlides );

    /**
     * Emitted when the slideshow is finished.
     */
    void presentationStopped();

    /**
     * Emitted when the presentation page is changed.
     *
     * @param page new page index within the current slideshow
     * @param stepsInPage the number of steps in the new page
     */
    void presentationPageChanged( int page, int stepsInPage );

    /**
     * Emitted when the presentation step is changed.
     *
     * @param step new step index within the page
     */
    void presentationStepChanged( int step );

private Q_SLOTS:
    /**
     * Fired when the presentation is activated.
     */
    void presentationActivated();

private:
    KPrView* m_view;
};

#endif // QT_NO_DBUS

#endif
