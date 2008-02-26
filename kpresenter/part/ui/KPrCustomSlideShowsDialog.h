/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

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

#ifndef KPRCUSTOMSLIDESHOWSDIALOG_H
#define KPRCUSTOMSLIDESHOWSDIALOG_H

#include <QtGui/QDialog>

#include "ui_KPrCustomSlideShowsDialog.h"

class KPrCustomSlideShows;
class KoPAPageBase;
class QListWidgetItem;
class KPrCustomSlideShows;
class KPrDocument;

class KPrCustomSlideShowsDialog : public QDialog
{
    Q_OBJECT

public:
    KPrCustomSlideShowsDialog( QWidget *parent, KPrCustomSlideShows *slideShows, KPrDocument *doc, KPrCustomSlideShows *&newSlideShows );
    ~KPrCustomSlideShowsDialog();
private Q_SLOTS:
    void addCustomSlideShow();
    void addSlidesToCurrentSlideShow();
    void deleteCustomSlideShow();
    void renameCustomSlideShow( QListWidgetItem *item );
    void loadCustomSlideShowsData();
    void changedSelectedSlideshow( QListWidgetItem *current, QListWidgetItem *previous );
    void removeSlidesFromCurrentSlideShow();

//     Q_SIGNALS:
private:
    enum {
        SlideShowNameData = 33,
        SlideData = 34,
        SlidePositionData = 35
    };

    bool m_firstTime;
    QString m_selectedSlideShowName;

    Ui::CustomSlideShowsWidget m_uiWidget;
    KPrCustomSlideShows *m_slideShows;
    KPrCustomSlideShows *m_oldSlideShows;
    KPrDocument *m_doc;
};
#endif
