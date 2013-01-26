/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef CAPRESENTATIONHANDLER_H
#define CAPRESENTATIONHANDLER_H

#include "CAAbstractDocumentHandler.h"

class CAPADocumentModel;
class QSize;
class QSizeF;
class KoFindMatch;

class CAPresentationHandler : public CAAbstractDocumentHandler
{
    Q_OBJECT
    Q_PROPERTY(int slideshowDelay READ slideshowDelay WRITE setSlideshowDelay NOTIFY slideshowDelayChanged)
    Q_PROPERTY(int currentSlideNumber READ currentSlideNumber WRITE setCurrentSlideNumber NOTIFY currentSlideNumberChanged)
    Q_PROPERTY(int totalNumberOfSlides READ totalNumberOfSlides NOTIFY totalNumberOfSlidesChanged)
    Q_PROPERTY (QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged)

public:
    explicit CAPresentationHandler (CADocumentController* documentController);
    virtual ~CAPresentationHandler();
    virtual bool openDocument (const QString& uri);
    virtual QStringList supportedMimetypes();
    virtual QString documentTypeName();
    virtual KoZoomMode::Mode preferredZoomMode() const;

    virtual QString topToolbarSource() const;
    virtual QString rightToolbarSource() const;
    virtual QString leftToolbarSource() const;
    virtual QString centerOverlaySource() const;
    virtual QString bottomToolbarSource() const;
    virtual QString previousPageImage() const;
    virtual QString nextPageImage() const;
    virtual FlickModes flickMode() const;

    virtual void gotoNextPage();
    virtual void gotoPreviousPage();

    int slideshowDelay() const;
    void setSlideshowDelay(int delay);

    int currentSlideNumber() const;
    int totalNumberOfSlides() const;
    QString searchString() const;
    void setSearchString (const QString& searchString);
    void setTextData(int slideNumber);
    enum SearchDirection {
         SearchForward,
         SearchBackwards
    };
    void searchOtherSlides(SearchDirection direction);

    Q_INVOKABLE CAPADocumentModel *paDocumentModel() const;

public slots:
    void tellZoomControllerToSetDocumentSize(const QSize &size);

    void nextSlide();
    void previousSlide();
    void zoomToFit();
    void updateCanvas();
    void resizeCanvas(const QSizeF &canvasSize);
    void startSlideshow();
    void stopSlideshow();
    void setCurrentSlideNumber(int number);
    void findNext();
    void findPrevious();

signals:
    void slideshowDelayChanged();
    void slideshowStarted();
    void slideshowStopped();
    void currentSlideNumberChanged();
    void totalNumberOfSlidesChanged();
    void searchStringChanged();

protected:
    virtual KoDocument* document();

private slots:
    void advanceSlideshow();
    void gotoCurrentSlide();
    void findMatchFound(const KoFindMatch& match);
    void findNoMatchFound();

private:
    class Private;
    Private * const d;
};

#endif // CAPRESENTATIONHANDLER_H
