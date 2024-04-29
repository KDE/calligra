/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Yannick Motta <yannick.motta@gmail.com>
 * SPDX-FileCopyrightText: 2009-2010 Benjamin Port <port.benjamin@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRHTMLEXPORTDIALOG_H
#define KPRHTMLEXPORTDIALOG_H

#include "ui_KPrHtmlExport.h"

// #include <QWebPage>

#include <KoDialog.h>
#include <KoPAPageBase.h>

class QUrl;

class KPrHtmlExportDialog : public KoDialog
{
    Q_OBJECT
public:
    KPrHtmlExportDialog(const QList<KoPAPageBase *> &slides, const QString &title, const QString &author, QWidget *parent = nullptr);

    QList<KoPAPageBase *> checkedSlides();
    QStringList slidesNames();
    QUrl templateUrl();
    QString title();
    QString author();
    bool openBrowser();

private Q_SLOTS:
    void checkAllItems();
    void uncheckAllItems();
    void renderPreview();
    void favoriteAction();
    void updateFavoriteButton();
    void generateNext();
    void generatePrevious();
    void generatePreview(int item = -1);
    void browserAction();

private:
    void generateSlidesNames(const QList<KoPAPageBase *> &slides);
    void loadTemplatesList();
    bool selectedTemplateIsFavorite();
    bool selectedTemplateIsSystemFavorite();
    bool verifyZipFile(const QString &zipLocalPath);
    void addSelectedTemplateToFavorite();
    void delSelectedTemplateFromFavorite();

    QList<KoPAPageBase *> m_allSlides;
    QString m_title;
    Ui::KPrHtmlExport ui;
    //     QWebPage preview;
    int frameToRender;
};

#endif // KPRHTMLEXPORTDIALOG_H
