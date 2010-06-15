/* This file is part of the KDE project
 * Copyright (C) 2009 Yannick Motta <yannick.motta@gmail.com>
 * Copyright (C) 2009-2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#ifndef KPRHTMLEXPORTDIALOG_H
#define KPRHTMLEXPORTDIALOG_H

#include "ui_KPrHtmlExport.h"

#include <QWebPage>

#include <KoPAPageBase.h>

class KPrHtmlExportDialog  : public KDialog
{
    Q_OBJECT
public:
    KPrHtmlExportDialog(const QList<KoPAPageBase*> &slides, const QString &title, const QString &author, QWidget *parent=0);

    QList<KoPAPageBase*> checkedSlides();
    QStringList slidesNames();
    KUrl templateUrl();
    QString title();
    QString author();
    bool openBrowser();

private slots:
    void checkAllItems();
    void uncheckAllItems();
    void renderPreview();
    void favoriteAction();
    void updateFavoriteButton();
    void generateNext();
    void generatePrevious();
    void generatePreview(int item=-1);
    void browserAction();


private:
    void generateSlidesNames(const QList<KoPAPageBase*> &slides);
    void loadTemplatesList();
    bool selectedTemplateIsFavorite();
    bool selectedTemplateIsSystemFavorite();
    bool verifyZipFile(const QString &zipLocalPath);
    void addSelectedTemplateToFavorite();
    void delSelectedTemplateFromFavorite();

    QList<KoPAPageBase*> m_allSlides;
    QString m_title;
    Ui::KPrHtmlExport ui;
    QWebPage preview;
    int frameToRender;


};



#endif // KPRHTMLEXPORTDIALOG_H
