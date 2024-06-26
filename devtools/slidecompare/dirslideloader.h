/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef DIRSLIDELOADER_H
#define DIRSLIDELOADER_H

#include "slideloader.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QTimer>

class DirSlideLoader : public SlideLoader
{
private:
    Q_OBJECT

    class Slide
    {
    private:
        QImage image;

    public:
        int version;
        QDateTime lastModified;
        QString path;
        QImage scaledImage;

        Slide()
            : version(0)
        {
        }
        Slide(const Slide &s)
        {
            version = 0;
            *this = s;
        }
        explicit Slide(const QString &p)
        {
            path = p;
            lastModified = QFileInfo(path).lastModified();
            version = 1;
        }
        bool operator==(const Slide &slide) const
        {
            return slide.lastModified == lastModified && slide.path == path;
        }
        bool operator!=(const Slide &slide) const
        {
            return !(slide == *this);
        }
        void operator=(const Slide &slide)
        {
            if (slide != *this) {
                version++;
                path = slide.path;
                lastModified = slide.lastModified;
                image = QImage();
                scaledImage = QImage();
            }
        }
        const QImage &getImage()
        {
            if (image.isNull()) {
                image.load(path);
            }
            return image;
        }
        QPixmap getPixmap(const QSize &maxsize)
        {
            QPixmap pixmap;
            const QImage &image = getImage();
            if (image.isNull())
                return pixmap;
            if (image.width() > maxsize.width()) {
                // scale to a smaller pixmap
                if (scaledImage.width() != maxsize.width()) {
                    scaledImage = image.scaled(maxsize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }
                pixmap = QPixmap::fromImage(scaledImage);
            } else {
                pixmap = QPixmap::fromImage(image);
            }
            return pixmap;
        }
    };

    QString slidedir;
    QString slideNamePattern;
    QFileSystemWatcher watcher;
    QTimer fileSystemDelay;
    QVector<Slide> slides;
    int numberofslides;
    QSize slidesize;

    QSize calculateSlideSize()
    {
        QSize size;
        for (int i = 0; size.isEmpty() && i < slides.size(); ++i) {
            size = slides[i].getImage().size();
        }
        return size;
    }
private Q_SLOTS:
    void slotDirectoryChanged(const QString &)
    {
        if (!fileSystemDelay.isActive()) {
            fileSystemDelay.setSingleShot(true);
            fileSystemDelay.start(100);
            connect(&fileSystemDelay, SIGNAL(timeout()), this, SLOT(slotUpdateSlides()));
        }
    }
    void slotUpdateSlides()
    {
        QDir dir(slidedir);
        QVector<Slide> newslides;
        if (numberofslides >= 0) {
            newslides.resize(numberofslides);
            // loop over all files
            for (int slideNumber = 0; slideNumber < numberofslides; ++slideNumber) {
                QString name = slideNamePattern.arg(slideNumber);
                if (dir.exists(name)) {
                    newslides[slideNumber] = dir.absoluteFilePath(name);
                }
            }
        } else {
            QRegExp pattern(QString(slideNamePattern).replace("%1", "(\\d+)"));
            foreach (const QString &name, dir.entryList()) {
                if (pattern.indexIn(name) != -1) {
                    bool ok;
                    int slideNumber = pattern.cap(1).toInt(&ok);
                    if (ok) {
                        if (newslides.size() <= slideNumber) {
                            newslides.resize(slideNumber + 1);
                        }
                        newslides[slideNumber] = dir.absoluteFilePath(name);
                    }
                }
            }
        }
        if (slides != newslides) {
            slides = newslides;
            Q_EMIT slidesChanged();
        }
    }

public:
    DirSlideLoader(QObject *parent = nullptr)
        : SlideLoader(parent)
        , watcher(this)
    {
        numberofslides = -1;
        connect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(slotDirectoryChanged(QString)));
    }
    void setSlideDir(const QString &path)
    {
        if (slidedir != path) {
            if (!slidedir.isEmpty()) {
                watcher.removePath(slidedir);
            }
            watcher.addPath(path);
            slidedir = path;
            slotDirectoryChanged(path);
        }
    }
    void setSlideNamePattern(const QString &pattern)
    {
        slideNamePattern = pattern;
        slotDirectoryChanged(slidedir);
    }
    void setNumberOfSlides(int n)
    {
        numberofslides = n;
        slotUpdateSlides();
    }
    int numberOfSlides()
    {
        return (numberofslides >= 0) ? numberofslides : slides.size();
    }
    void setSlideSize(const QSize &size)
    {
        slidesize = size;
    }
    QSize slideSize()
    {
        if (slidesize.isEmpty()) {
            return calculateSlideSize();
        }
        return slidesize;
    }
    int slideVersion(int position)
    {
        return slides.value(position).version;
    }
    QPixmap loadSlide(int position, const QSize &maxsize)
    {
        if (position < 0 || position >= slides.size())
            return QPixmap();
        return slides[position].getPixmap(maxsize);
    }
};

#endif
