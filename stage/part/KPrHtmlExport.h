/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009-2010 Benjamin Port <port.benjamin@gmail.com>
   SPDX-FileCopyrightText: 2009 Yannick Motta <yannick.motta@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRHTMLEXPORT_H
#define KPRHTMLEXPORT_H

#include <QObject>
#include <QStringList>
#include <QUrl>

class KPrView;
class KoPAPageBase;
class KJob;

class KPrHtmlExport : public QObject
{
    Q_OBJECT
public:
    struct Parameter {
        Parameter() = default;

        Parameter(const QUrl &styleUrl,
                  KPrView *kprView,
                  const QList<KoPAPageBase *> &slides,
                  const QUrl &destination,
                  const QString &author,
                  const QString &title,
                  const QStringList &slidesNames,
                  bool openBrowser)
            : styleUrl(styleUrl)
            , kprView(kprView)
            , slides(slides)
            , destination(destination)
            , author(author)
            , title(title)
            , slidesNames(slidesNames)
            , openBrowser(openBrowser)
        {
        }

        QUrl styleUrl;
        KPrView *kprView;
        QList<KoPAPageBase *> slides;
        QUrl destination;
        QString author;
        QString title;
        QStringList slidesNames;
        bool openBrowser;
    };

    KPrHtmlExport();
    ~KPrHtmlExport() override;
    void exportHtml(const Parameter &parameters);

    /**
     * Generates a preview of 1 frame into a tempoary directory
     * @param parameters Presentation data (only 1 slide should be provided in "slides" filed)
     */
    QUrl exportPreview(const Parameter &parameters);

protected:
    void extractStyle();
    void generateHtml();
    void generateToc();
    void exportImageToTmpDir();
    void writeHtmlFileToTmpDir(const QString &fileName, const QString &htmlBody);
    void copyFromTmpToDest();

private Q_SLOTS:
    void moveResult(KJob *job);

private:
    QString m_tmpDirPath;
    Parameter m_parameters;
};

#endif /* KPRHTMLEXPORT_H */
