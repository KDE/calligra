/* This file is part of the KDE project
   Copyright (C) 2011-2012 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiWelcomeStatusBar.h"
#include "KexiWelcomeStatusBar_p.h"

#include <core/KexiMainWindowIface.h>
#include <kexi_version.h>
#include <kexiutils/utils.h>
#include <kexiutils/KexiContextMessage.h>
#include "KexiUserFeedbackAgent.h"

#include <KGlobalSettings>
#include <KIconLoader>
#include <KColorUtils>
#include <KColorScheme>
#include <KStandardDirs>
#include <KLocale>
#include <KDebug>
#include <KAction>
#include <KStandardGuiItem>
#include <KFadeWidgetEffect>
#include <KConfigGroup>
#include <KIO/Job>
#include <KIO/CopyJob>
#include <KCodecs>
#include <KTemporaryFile>
#include <KTempDir>
#include <kde_file.h>

#include <QEvent>
#include <QLayout>
#include <QProgressBar>
#include <QtCore/qmath.h>
#include <QFile>
#include <QDesktopServices>
#include <QUiLoader>
#include <QScrollArea>
#include <QLabel>
#include <QResource>
#include <QTimer>

static const int GUI_UPDATE_INTERVAL = 60; // update interval for GUI, in minutes
static const int UPDATE_FILES_LIST_SIZE_LIMIT = 1024 * 128;
static const int UPDATE_FILES_COUNT_LIMIT = 128;

static QString basePath()
{
    return QString("kexi/status/") + Kexi::stableVersionString();
}

static QString findFilename(const QString &guiFileName)
{
    const QString result = KStandardDirs::locate("data", basePath() + '/' + guiFileName);
    kDebug() << result;
    return result;
}

// ---

class KexiWelcomeStatusBarGuiUpdater::Private
{
public:
    Private()
     : configGroup(KConfigGroup(KGlobal::config()->group("User Feedback")))
    {
    }
    KConfigGroup configGroup;
    QStringList fileNamesToUpdate;
    QString tempDir;
};

KexiWelcomeStatusBarGuiUpdater::KexiWelcomeStatusBarGuiUpdater()
 : QObject()
 , d(new Private)
{
}

KexiWelcomeStatusBarGuiUpdater::~KexiWelcomeStatusBarGuiUpdater()
{
    delete d;
}

QString KexiWelcomeStatusBarGuiUpdater::uiPath(const QString &fname) const
{
    KexiUserFeedbackAgent *f = KexiMainWindowIface::global()->userFeedbackAgent();
    return f->serviceUrl() + QString("/ui/%1/").arg(Kexi::stableVersionString())
        + fname;
}

void KexiWelcomeStatusBarGuiUpdater::update()
{
    QDateTime lastStatusBarUpdate = d->configGroup.readEntry("LastStatusBarUpdate", QDateTime());
    if (lastStatusBarUpdate.isValid()) {
        int minutes = lastStatusBarUpdate.secsTo(QDateTime::currentDateTime()) / 60;
        
        if (minutes < GUI_UPDATE_INTERVAL) {
            kDebug() << "gui updated" << minutes << "min. ago, next auto-update in" 
                << (GUI_UPDATE_INTERVAL - minutes) << "min.";
            return;
        }
    }
    
    d->configGroup.writeEntry("LastStatusBarUpdate", QDateTime::currentDateTime());
    
    KexiUserFeedbackAgent *f = KexiMainWindowIface::global()->userFeedbackAgent();
    f->waitForRedirect(this, SLOT(slotRedirectLoaded()));
}

void KexiWelcomeStatusBarGuiUpdater::slotRedirectLoaded()
{
    QByteArray postData = Kexi::stableVersionString().toLatin1();
    KIO::Job* sendJob = KIO::storedHttpPost(postData,
                                            KUrl(uiPath(".list")),
                                            KIO::HideProgressInfo);
    connect(sendJob, SIGNAL(result(KJob*)), this, SLOT(sendRequestListFilesFinished(KJob*)));
    sendJob->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
}

void KexiWelcomeStatusBarGuiUpdater::sendRequestListFilesFinished(KJob* job)
{
    if (job->error()) {
        kWarning() << "Error while receiving .list file - no files will be updated";
        //! @todo error...
        return;
    }
    KIO::StoredTransferJob* sendJob = qobject_cast<KIO::StoredTransferJob*>(job);
    QString result = sendJob->data();
    if (result.length() > UPDATE_FILES_LIST_SIZE_LIMIT) { // anit-DOS protection
        kWarning() << "Too large .list file (" << result.length()
            << "); the limit is" << UPDATE_FILES_LIST_SIZE_LIMIT
            << "- no files will be updated";
        return;
    }
    kDebug() << result;
    QStringList data = result.split('\n', QString::SkipEmptyParts);
    result.clear();
    d->fileNamesToUpdate.clear();
    if (data.count() > UPDATE_FILES_COUNT_LIMIT) { // anti-DOS protection
        kWarning() << "Too many files to update (" << data.count()
            << "); the limit is" << UPDATE_FILES_COUNT_LIMIT
            << "- no files will be updated";
        return;
    }
    // OK, try to update (stage 1: check, stage 2: checking)
    for (int stage = 1; stage <= 2; stage++) {
        int i = 0;
        for (QStringList::ConstIterator it(data.constBegin()); it!=data.constEnd(); ++it, i++) {
            const QByteArray hash((*it).left(32).toLatin1());
            const QString remoteFname((*it).mid(32 + 2));
            if (stage == 1) {
                if (hash.length() != 32) {
                    kWarning() << "Invalid hash" << hash << "in line" << i+1 << "- no files will be updated";
                    return;
                }
                if ((*it).mid(32, 2) != "  ") {
                    kWarning() << "Two spaces expected but found" << (*it).mid(32, 2)
                        << "in line" << i+1 << "- no files will be updated";
                    return;
                }
                if (remoteFname.contains(QRegExp("\\s"))) {
                    kWarning() << "Filename expected without whitespace but found" << remoteFname
                        << "in line" << i+1 << "- no files will be updated";
                    return;
                }
            }
            else if (stage == 2) {
                checkFile(hash, remoteFname, &d->fileNamesToUpdate);
            }
        }
    }
    // update files
    KUrl::List sourceFiles;
    foreach (const QString &fname, d->fileNamesToUpdate) {
        sourceFiles.append(KUrl(uiPath(fname)));
    }
    KTempDir tempDir(KStandardDirs::locateLocal("tmp", "kexi-status"));
    tempDir.setAutoRemove(false);
    d->tempDir = tempDir.name();
    kDebug() << tempDir.name();
    KIO::CopyJob *copyJob = KIO::copy(sourceFiles,
                                        KUrl("file://" + tempDir.name()),
                                        KIO::HideProgressInfo | KIO::Overwrite);
    connect(copyJob, SIGNAL(result(KJob*)), this, SLOT(filesCopyFinished(KJob*)));
        //kDebug() << "copying from" << KUrl(uiPath(fname)) << "to"
//            << (dir + fname);
}

void KexiWelcomeStatusBarGuiUpdater::checkFile(const QByteArray &hash,
                                               const QString &remoteFname,
                                               QStringList *fileNamesToUpdate)
{
    QString localFname = findFilename(remoteFname);
    if (localFname.isEmpty()) {
        fileNamesToUpdate->append(remoteFname);
        kDebug() << "missing filename" << remoteFname << "- download it";
        return;
    }
    QFile file(localFname);
    if (!file.open(QIODevice::ReadOnly)) {
        kWarning() << "could not open file" << localFname << "- update it";
        fileNamesToUpdate->append(remoteFname);
        return;
    }
    KMD5 md5("");
    if (!md5.update(file)) {
        kWarning() << "could not check MD5 for file" << localFname << "- update it";
        fileNamesToUpdate->append(remoteFname);
        return;
    }
    if (!md5.verify(hash)) {
        kDebug() << "not matching file" << localFname << "- update it";
        fileNamesToUpdate->append(remoteFname);
    }
}

void KexiWelcomeStatusBarGuiUpdater::filesCopyFinished(KJob* job)
{
    if (job->error()) {
        //! @todo error...
        kDebug() << "ERROR:" << job->errorString();
        return;
    }
    KIO::CopyJob* copyJob = qobject_cast<KIO::CopyJob*>(job);
    kDebug() << "DONE" << copyJob->destUrl();

    QString dir(KStandardDirs::locateLocal("data", basePath() + '/', true /*create*/));
    kDebug() << dir;
    foreach (const QString &fname, d->fileNamesToUpdate) {
        if (0 != KDE::rename(d->tempDir + fname, dir + fname)) {
            kWarning() << "cannot move" << (d->tempDir + fname) << "to" << (dir + fname);
        }
    }
    KTempDir::removeDir(d->tempDir);
}

// ---

//! @internal
class ScrollArea : public QScrollArea
{
public:
    ScrollArea(QWidget *parent = 0) : QScrollArea(parent)
    {
        setFrameShape(QFrame::NoFrame);
        setBackgroundRole(QPalette::Base);
        setWidgetResizable(true);
    }

    void setEnabled(bool set) {
        if (set != isEnabled()) {
            QScrollArea::setEnabled(set);
            updateColors();
        }
    }

protected:
    virtual void changeEvent(QEvent* event)
    {
        switch (event->type()) {
        case QEvent::EnabledChange:
        case QEvent::PaletteChange:
            updateColors();
            break;
        default:;
        }
        QScrollArea::changeEvent(event);
    }

    void updateColors() {
        if (!widget())
            return;
        KColorScheme scheme(palette().currentColorGroup());
        QColor linkColor = scheme.foreground(KColorScheme::LinkText).color();
        //kDebug() << "_____________" << isEnabled();

        foreach(QLabel* lbl, widget()->findChildren<QLabel*>()) {
            QString t = lbl->text();
            QRegExp re("<a.*>");
            re.setMinimal(true);
            int pos = 0;
            int oldPos = 0;
            QString newText;
            //kDebug() << "t:" << t;
            while ((pos = re.indexIn(t, pos)) != -1) {
                //kDebug() << "pos:" << pos;
                //kDebug() << "newText += t.mid(oldPos, pos - oldPos)"
                //    << t.mid(oldPos, pos - oldPos);
                newText += t.mid(oldPos, pos - oldPos);
                //kDebug() << "newText1:" << newText;
                //kDebug() << lbl->objectName() << "~~~~" << t.mid(pos, re.matchedLength());
                QString a = t.mid(pos, re.matchedLength());
                //kDebug() << "a:" << a;
                int colPos = a.indexOf("color:");
                if (colPos == -1) { // add color
                    a.insert(a.length() - 1, " style=\"color:" + linkColor.name() + ";\"");
                }
                else { // replace color
                    colPos += qstrlen("color:");
                    for (;colPos < a.length() && a[colPos] == ' '; colPos++)
                        ;
                    if (colPos < a.length() && a[colPos] == '#') {
                        colPos++;
                        int i = colPos;
                        for (;i < a.length(); i++) {
                            if (a[i] == ';' || a[i] == ' ' || a[i] == '"' || a[i] == '\'')
                                break;
                        }
                        //kDebug() << "******" << a.mid(colPos, i - colPos);
                        a.replace(colPos, i - colPos, linkColor.name().mid(1));
                    }
                }
                //kDebug() << "a2:" << a;
                newText += a;
                //kDebug() << "newText2:" << newText;
                pos += re.matchedLength();
                oldPos = pos;
                //kDebug() << "pos2:" << pos;
            }
            //kDebug() << "oldPos:" << oldPos;
            newText += t.mid(oldPos);
            //kDebug() << "newText3:" << newText;
            lbl->setText(newText);
        }

#if 0
        QString text;
        text = QString("<a href=\"%1\" style=\"color:%2;\">%3</a>")
            .arg(link).arg(linkColor.name()).arg(linkText);
        if (!format.isEmpty()) {
            text = QString(format).replace("%L", text);
        }
        q->setText(text);
#endif
    }
};

// ---

class KexiWelcomeStatusBar::Private
{
public:
    Private(KexiWelcomeStatusBar* _q)
     : statusWidget(0), helpAction(0), shareAction(0), cancelAction(0),
       q(_q)
    {
        rccFname = findFilename("status.rcc");
        if (!rccFname.isEmpty())  {
            QResource::registerResource(rccFname);
        }

        scores.insert(KexiUserFeedbackAgent::BasicArea, 4);
        scores.insert(KexiUserFeedbackAgent::SystemInfoArea, 4);
        scores.insert(KexiUserFeedbackAgent::ScreenInfoArea, 2);
        scores.insert(KexiUserFeedbackAgent::RegionalSettingsArea, 2);
        totalFeedbackScore = 0;
        foreach (int s, scores.values()) {
            totalFeedbackScore += s;
        }
        kDebug() << "totalFeedbackScore:" << totalFeedbackScore;
    }
    
    ~Private() {
        delete msgWidget;
        if (!rccFname.isEmpty())  {
            QResource::unregisterResource(rccFname);
        }
    }

    int currentFeedbackScore() const
    {
        int score = 0;
        KexiUserFeedbackAgent *f = KexiMainWindowIface::global()->userFeedbackAgent();
        KexiUserFeedbackAgent::Areas areas = f->enabledAreas();
        for (QMap<KexiUserFeedbackAgent::Area, int>::ConstIterator it(scores.constBegin());
             it!=scores.constEnd(); ++it)
        {
            if (areas & it.key()) {
                score += it.value();
            }
        }
        kDebug() << score;
        return score;
    }
    
    template<typename T>
    T widgetOfClass(T parent, const char *widgetName) const
    {
        T w = qFindChild<T>(parent, widgetName);
        if (!w) {
            kWarning() << "NO SUCH widget" << widgetName << "in" << parent;
        }
        return w;
    }
    
    QWidget* widget(QWidget *parent, const char *widgetName) const
    {
        return widgetOfClass<QWidget*>(parent, widgetName);
    }
    
    QObject* object(QObject *parent, const char *objectName) const
    {
        QObject *o = qFindChild<QObject*>(parent, objectName);
        if (!o) {
            kWarning() << "NO SUCH object" << objectName << "in" << parent;
        }
        return o;
    }

    void setProperty(QWidget *parent, const char *widgetName,
                     const char *propertyName, const QVariant &value)
    {
        QWidget *w = widget(parent, widgetName);
        if (w) {
            w->setProperty(propertyName, value);
        }
    }

    QVariant property(QWidget *parent, const char *widgetName, const char *propertyName) const
    {
        QWidget *w = widget(parent, widgetName);
        return w ? w->property(propertyName) : QVariant();
    }

    void connect(QWidget *parent, const char *widgetName, const char *signalName,
                 QObject *receiver, const char *slotName)
    {
        QWidget *w = widget(parent, widgetName);
        if (w) {
            QObject::connect(w, signalName, receiver, slotName);
        }
    }

    void animatedHide(QWidget *parent, const char *widgetName)
    {
        QWidget *w = widget(parent, widgetName);
        if (!w)
            return;
        KFadeWidgetEffect *animation = new KFadeWidgetEffect(w);
        QObject::connect(animation, SIGNAL(destroyed()), w, SLOT(hide()));
        animation->start();
    }
    
    QWidget* loadGui(const QString &guiFileName, QWidget *parentWidget = 0)
    {
        QString fname = findFilename(guiFileName);
        if (fname.isEmpty()) {
            kWarning() << "filename" << fname << "not found";
            return 0;
        }
        QFile file(fname);
        if (!file.open(QIODevice::ReadOnly)) {
            kWarning() << "could not open file" << fname;
            return 0;
        }
        QUiLoader loader;
        QWidget* widget = loader.load(&file, parentWidget);
        if (!widget) {
            kWarning() << "could load ui from file" << fname;
        }
        file.close();
        return widget;
    }

    void updateStatusWidget()
    {
        QWidget *widget = loadGui("status.ui", statusScrollArea);
        if (!widget) {
            return;
        }
        int smallFontSize = qFloor((KGlobalSettings::smallestReadableFont().pointSizeF()
                                   + q->font().pointSizeF())
                                   / 2.0);
        smallFont = q->font();
        smallFont.setPointSizeF(smallFontSize);
        widget->setFont(smallFont);
        delete statusWidget;
        statusWidget = widget;
        statusScrollArea->setWidget(statusWidget);
        setProperty(statusWidget, "contribution_progress", "minimumHeight",
                    q->fontMetrics().height());
        setProperty(statusWidget, "contribution_progress", "maximumHeight",
                    q->fontMetrics().height());
        label_involved_text_mask = property(statusWidget, "label_involved", "text").toString();
        setProperty(statusWidget, "link_share_usage_info", "text",
                    property(statusWidget, "link_share_usage_info", "text").toString().arg(totalFeedbackScore));
        link_share_more_usage_info_mask = property(statusWidget, "link_share_more_usage_info", "text").toString();
        updateUserProgress();
        updateContributionLinksVisibility();
        // do not alter background palette
        QPalette pal(widget->palette());
        pal.setColor(QPalette::Disabled, QPalette::Base,
                     pal.color(QPalette::Normal, QPalette::Base));
        widget->setPalette(pal);
        connect(statusWidget, "link_contribute_show_help", SIGNAL(linkActivated(QString)),
                q, SLOT(showContributionHelp()));
        connect(statusWidget, "link_share_usage_info", SIGNAL(linkActivated(QString)),
                q, SLOT(showShareUsageInfo()));
        connect(statusWidget, "link_share_more_usage_info", SIGNAL(linkActivated(QString)),
                q, SLOT(showShareUsageInfo()));
        connect(statusWidget, "link_show_contribution_details", SIGNAL(linkActivated(QString)),
                q, SLOT(showContributionDetails()));
#if 0
        baseFname = QString("status/%1/status.html").arg(Kexi::stableVersionString());
        fname = KStandardDirs::locate("data", "kexi/" + baseFname);

        QString html;
        if (!fname.isEmpty()) {
            QFile file(fname);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                html = file.readAll();
                file.close();
            }
        }
        if (html.isEmpty()) {
            //! @todo hardcode simplest version
        }

        QString newHtml = transform(html);
        
        kDebug() << html;
        kDebug() << "-----------";
        kDebug() << newHtml;
        html = newHtml;
        
        title = translate("You in Kexi Project"); // Your Status in Kexi Project
    //TODO: replace every ${..}

        QString t = "What's New?";
        // <img src='ext'>
        html += QString(
            "<br>"
            "<p><b class=\"head\">%1</b></p>"
            "<p><a href=\"news://0\"> Fruits of CSS2: Office Forms</a></p>"
            "<p align=\"right\"><a href=\"news://show_all\">See all <b>News</b> &raquo;</a></p>"
            ).arg(t);

        t = "Recent Releases";
        html += QString(
            "<br>"
            "<p><b class=\"head\">%1</b></p>"
            "<p><table>"
            "<tr><td><p>Stable release: </p></td><td><a href=\"http://www.koffice.org/news/announcements/koffice-2-3-3-update/\">2.3.3</a></td></tr>"
            "<tr><td><p>Preview release: </p></td><td><a href=\"http://www.calligra.org/news/announcements/calligra-2-4-beta-6/\">2.4 Beta 6</a></td></tr>"
            "</table></p>"
            "<p align=\"right\"><a href=\"releases://show_all\">See more <b>Releases</b> &raquo;</a></p>"
            ).arg(t);
        
        QString htmlHead(QLatin1String(
            "<html><head>"
            "<link rel='stylesheet' type='text/css' href='format.css'>"
            "</head><body>"));
        doc->setHtml(htmlHead + html + "</body></html>");
        statusBrowser->setDocument(doc);

        kDebug() << statusBrowser->toHtml();
#endif
    }

    void setUserProgress(int progress)
    {
        setProperty(statusWidget, "contribution_progress", "value", progress);
        setProperty(statusWidget, "label_involved", "text",
                    label_involved_text_mask.arg(progress));
    }

    void updateUserProgress()
    {
        int progress = 0;
        progress += currentFeedbackScore();
        setUserProgress(progress);
    }
    
    void updateContributionLinksVisibility()
    {
        KexiUserFeedbackAgent *f = KexiMainWindowIface::global()->userFeedbackAgent();
        int availableLinks = 0;
        bool noneEnabled = f->enabledAreas() == KexiUserFeedbackAgent::NoAreas;
        bool allEnabled = f->enabledAreas() == KexiUserFeedbackAgent::AllAreas;
        setProperty(statusWidget, "share_usage_info", "visible", noneEnabled);
        if (noneEnabled) {
            availableLinks++;
        }
        setProperty(statusWidget, "share_more_usage_info", "visible", 
                    !noneEnabled && !allEnabled);
        if (!noneEnabled && !allEnabled) {
            availableLinks++;
        }
        setProperty(statusWidget, "link_share_more_usage_info", "text",
                    link_share_more_usage_info_mask.arg(totalFeedbackScore - currentFeedbackScore()));
        
        setProperty(statusWidget, "lbl_contribute", "visible", availableLinks > 0);
    }

    enum CalloutAlignment {
        AlignToBar,
        AlignToWidget
    };
    
    //! Aligns callout pointer position of msgWidget to widget named @a alignToWidgetName
    void setMessageWidgetCalloutPointerPosition(
        const QString& alignToWidgetName,
        CalloutAlignment calloutAlignment = AlignToBar)
    {
            kDebug() << q->pos() << q->mapToGlobal(QPoint(0, 100));
            QPoint p(q->mapToGlobal(QPoint(0, 100)));
            QWidget *alignToWidget = this->widget(statusWidget, alignToWidgetName.toLatin1());
            if (alignToWidget) {
                p.setY(
                    alignToWidget->mapToGlobal(
                        QPoint(-5, alignToWidget->height() / 2)).y());
                if (calloutAlignment == AlignToWidget) {
                    p.setX(alignToWidget->mapToGlobal(QPoint(-5, 0)).x());
                    kDebug() << p << "++++++++++";
                }
            }
            else {
                kWarning() << alignToWidgetName << "not found!";
            }
            msgWidget->setCalloutPointerPosition(p, alignToWidget);
    }

    //! Shows message widget taking maximum space within the welcome page
    //! Returns created layout for further use into @a layout.
    //! Created widge is assigned to msgWidget.
    //! Calls slot @a slotToCallAfterShow after animated showing, if provided.
    //! Call msgWidget->animatedShow() afterwards.
    void showMaximizedMessageWidget(const QString &alignToWidgetName,
                                    QPointer<QGridLayout> *layout,
                                    const char* slotToCallAfterShow,
                                    CalloutAlignment calloutAlignment = AlignToBar)
    {
        QWidget *alignToWidget = this->widget(statusWidget, alignToWidgetName.toLatin1());
        int msgWidth;
        if (alignToWidget && calloutAlignment == AlignToWidget) {
            msgWidth = q->parentWidget()->width() - alignToWidget->width() - 10;
        }
        else {
            msgWidth = q->parentWidget()->width() - q->width();
        }
        QWidget *widget = new QWidget;
        *layout = new QGridLayout(widget);
        if (msgWidth > 100) { // nice text margin
            (*layout)->setColumnMinimumWidth(0, 50);
        }
        kDebug() << (q->parentWidget()->width() - q->width()) << "***";
        KexiContextMessage msg(widget);
        if (msgWidget) {
            delete static_cast<KexiContextMessageWidget*>(msgWidget);
        }
        msgWidget
            = new KexiContextMessageWidget(q->parentWidget()->parentWidget(), 0, 0, msg);
        msgWidget->setCalloutPointerDirection(KMessageWidget::Right);
        msgWidget->setMessageType(KMessageWidget::Information);
        msgWidget->setCloseButtonVisible(true);
        int offset_y = 0;
        if (alignToWidget) {
            offset_y = alignToWidget->mapToGlobal(QPoint(0, 0)).y()
                       - q->parentWidget()->mapToGlobal(QPoint(0, 0)).y();
        }
        else {
            kWarning() << alignToWidgetName << "not found!";
        }
        msgWidget->resize(msgWidth, q->parentWidget()->height() - offset_y);
        setMessageWidgetCalloutPointerPosition(alignToWidgetName, calloutAlignment);
        msgWidget->setResizeTrackingPolicy(Qt::Horizontal | Qt::Vertical);
        statusScrollArea->setEnabled(false);
        // async show to for speed up
        if (slotToCallAfterShow) {
            QObject::connect(msgWidget, SIGNAL(animatedShowFinished()),
                            q, slotToCallAfterShow);
        }
        QObject::connect(msgWidget, SIGNAL(animatedHideFinished()),
                         q, SLOT(slotMessageWidgetClosed()));
    }

    ScrollArea *statusScrollArea;
    QWidget *statusWidget;
    QVBoxLayout *lyr;
    QPointer<KexiContextMessageWidget> msgWidget;
    QFont smallFont;
    KAction *helpAction;
    KAction *shareAction;
    KAction *cancelAction;
    QString label_involved_text_mask;
    QString link_share_more_usage_info_mask;
    QPointer<QGridLayout> contributionHelpLayout;
    QPointer<QGridLayout> contributionDetailsLayout;
    QPointer<QWidget> contributionDetailsWidget;
    QMap<KexiUserFeedbackAgent::Area, int> scores;
    QString countryMask;
    QString languageMask;
    bool detailsDataVisible;
    int totalFeedbackScore;

    KexiWelcomeStatusBarGuiUpdater guiUpdater;
private:
    QString rccFname;
    KexiWelcomeStatusBar *q;
    QMap<QString, QString> dict;
};
    
KexiWelcomeStatusBar::KexiWelcomeStatusBar(QWidget* parent)
 : QWidget(parent), d(new Private(this))
{
    d->lyr = new QVBoxLayout(this);

    init();
}

KexiWelcomeStatusBar::~KexiWelcomeStatusBar()
{
    delete d;
}

void KexiWelcomeStatusBar::init()
{
#if 0
    <p><span class="head">${YOU_IN_KEXI}</span> <a class='gray' href='feedback://help_your_status'>(?)</a></p>
<p><img src='feedbackdata://progress'> ${PERCENT_INVOLVED}</p>
<p><b>${CONTRIBUTE}</b> <a class='gray' href='feedback://help_why_contribute'>(${WHY}?)</a></p>
<p><img src="kicon://list-add"> <a href='feedback://show_share_usage_info'>${SHARE_USAGE_INFO}</a> <span class='gray'>(+5%)</span></p>

    QString title("You in Kexi Project"); // Your Status in Kexi Project
    QString html = QString(
        "<p><span class=\"head\">%1</span> <a class='gray' href='feedback://help_your_status'>(?)</a></p>"
        "<p><img src='feedbackdata://progress'> <b>%2%</b> involved</p>"
        "<p><b>Contribute</b> <a class='gray' href='feedback://help_why_contribute'>(Why?)</a></p>"
        "<p><img src=\"kicon://list-add\"> <a href='feedback://show_share_usage_info'>Share Usage Info</a> <span class='gray'>(+5%)</span></p>"
        ).arg(title).arg(d->userProgressBar.value());
#endif

    d->statusScrollArea = new ScrollArea(this);
    d->lyr->addWidget(d->statusScrollArea);

    d->updateStatusWidget();
    QTimer::singleShot(10, &d->guiUpdater, SLOT(update()));
}

void KexiWelcomeStatusBar::showContributionHelp()
{
    d->showMaximizedMessageWidget("link_contribute_show_help",
                                  &d->contributionHelpLayout,
                                  SLOT(slotShowContributionHelpContents()));
    d->msgWidget->animatedShow();
/*    int msgWidth = parentWidget()->width() - width();
    QWidget *widget = new QWidget;
    d->contributionHelpLayout = new QGridLayout(widget);
    if (msgWidth > 100) { // nice text margin
        d->contributionHelpLayout->setColumnMinimumWidth(0, 50);
    }
    kDebug() << (parentWidget()->width() - width()) << "***";
    KexiContextMessage msg(widget);
    if (d->msgWidget) {
        delete static_cast<KexiContextMessageWidget*>(d->msgWidget);
    }
    d->msgWidget
        = new KexiContextMessageWidget(parentWidget(), 0, 0, msg);
    d->msgWidget->setCalloutPointerDirection(KMessageWidget::Right);
    kDebug() << pos() << mapToGlobal(QPoint(0, 100));
    QPoint p(mapToGlobal(QPoint(0, 100)));
    QWidget *link_contribute_show_help = d->widget("link_contribute_show_help");
    if (link_contribute_show_help) {
        p.setY(
            link_contribute_show_help->mapToGlobal(
                QPoint(-5, link_contribute_show_help->height() / 2)).y());
    }
    d->msgWidget->setMessageType(KMessageWidget::Information);
    d->msgWidget->setCalloutPointerPosition(p);
    d->msgWidget->setCloseButtonVisible(true);
    d->msgWidget->resize(msgWidth, parentWidget()->height() - 12);
    d->statusScrollArea->setEnabled(false);
    // async show to for speed up
    connect(d->msgWidget, SIGNAL(animatedShowFinished()),
            this, SLOT(slotShowContributionHelpContents()));
    connect(d->msgWidget, SIGNAL(animatedHideFinished()),
            this, SLOT(slotContributionHelpClosed()));
    d->msgWidget->animatedShow();*/
}

void KexiWelcomeStatusBar::slotShowContributionHelpContents()
{
    QWidget *helpWidget = d->loadGui("contribution_help.ui");
    d->contributionHelpLayout->addWidget(helpWidget, 1, 1);
    d->msgWidget->setPaletteInherited();
}

void KexiWelcomeStatusBar::slotMessageWidgetClosed()
{
    d->statusScrollArea->setEnabled(true);
    d->updateUserProgress();
    d->updateContributionLinksVisibility();
}

void KexiWelcomeStatusBar::showShareUsageInfo()
{
    if (!sender()) {
        return;
    }
    QWidget *widget = d->loadGui("status_strings.ui");
    if (!widget) {
        return;
    }
    QLabel *lbl = qFindChild<QLabel*>(widget, "question");
    if (!lbl) {
        return;
    }
    KexiContextMessage msg(lbl->text());
    delete widget;
    if (!d->helpAction) {
        d->helpAction = new KAction(KStandardGuiItem::help().icon(),
                                    KStandardGuiItem::help().text(), this);
        connect(d->helpAction, SIGNAL(triggered()), this, SLOT(showContributionHelp()));
    }
    if (!d->shareAction) {
        d->shareAction = new KAction(KStandardGuiItem::yes().icon(), i18n("Share"), this);
        connect(d->shareAction, SIGNAL(triggered()), this, SLOT(slotShareFeedback()));
    }
    if (!d->cancelAction) {
        d->cancelAction = new KAction(KStandardGuiItem::cancel().icon(),
                                      KStandardGuiItem::cancel().text(), this);
        QObject::connect(d->cancelAction, SIGNAL(triggered()), this, SLOT(slotCancelled()));
    }
    msg.addAction(d->helpAction, KexiContextMessage::AlignLeft);
    msg.addAction(d->shareAction);
    msg.addAction(d->cancelAction);
    if (d->msgWidget) {
        delete static_cast<KexiContextMessageWidget*>(d->msgWidget);
    }
    d->msgWidget
        = new KexiContextMessageWidget(parentWidget(), 0, 0, msg);
    d->msgWidget->setMessageType(KMessageWidget::Information);
    d->msgWidget->setCalloutPointerDirection(KMessageWidget::Right);
    d->setMessageWidgetCalloutPointerPosition(sender()->objectName());
    d->statusScrollArea->setEnabled(false);
    d->msgWidget->setMaximumWidth(parentWidget()->width() - width());
    d->msgWidget->setResizeTrackingPolicy(Qt::Horizontal);

    /*foreach (QLabel *lbl, d->statusScrollArea->findChildren<QLabel*>()) {
        if (lbl->isEnabled()) {
            //d->enabledLinks.append(w);
            lbl->setEnabled(false);
        }
    }*/
    d->msgWidget->animatedShow();
}

void KexiWelcomeStatusBar::slotShareFeedback()
{
    d->statusScrollArea->setEnabled(true);
    d->msgWidget->animatedHide();
    KexiMainWindowIface::global()->userFeedbackAgent()
        ->setEnabledAreas(KexiUserFeedbackAgent::AllAreas);
    
    d->animatedHide(d->statusWidget, "share_usage_info");
    d->animatedHide(d->statusWidget, "share_more_usage_info");
    d->animatedHide(d->statusWidget, "lbl_contribute");
    d->updateUserProgress();
}

void KexiWelcomeStatusBar::slotCancelled()
{
    d->statusScrollArea->setEnabled(true);
}

// Contribution Details BEGIN

void KexiWelcomeStatusBar::showContributionDetails()
{
    d->showMaximizedMessageWidget("link_show_contribution_details",
                                  &d->contributionDetailsLayout,
                                  0,
                                  KexiWelcomeStatusBar::Private::AlignToWidget);
    d->contributionDetailsLayout->setColumnMinimumWidth(0, 6); // smaller
    d->contributionDetailsWidget = d->loadGui("contribution_details.ui");
    
    KexiUserFeedbackAgent *f = KexiMainWindowIface::global()->userFeedbackAgent();
    d->setProperty(d->contributionDetailsWidget, "group_share", "checked",
                   f->enabledAreas() != KexiUserFeedbackAgent::NoAreas);
    d->setProperty(d->contributionDetailsWidget, "group_basic", "title",
                   d->property(d->contributionDetailsWidget, "group_basic", "title")
                       .toString().arg(d->scores.value(KexiUserFeedbackAgent::BasicArea)));

    updateContributionGroupCheckboxes();
    
    d->setProperty(d->contributionDetailsWidget, "group_system", "title",
                   d->property(d->contributionDetailsWidget, "group_system", "title")
                       .toString().arg(d->scores.value(KexiUserFeedbackAgent::SystemInfoArea)));
    d->connect(d->contributionDetailsWidget, "group_system", SIGNAL(toggled(bool)),
               this, SLOT(slotShareContributionDetailsGroupToggled(bool)));

    d->setProperty(d->contributionDetailsWidget, "group_screen", "title",
                   d->property(d->contributionDetailsWidget, "group_screen", "title")
                       .toString().arg(d->scores.value(KexiUserFeedbackAgent::ScreenInfoArea)));
    d->connect(d->contributionDetailsWidget, "group_screen", SIGNAL(toggled(bool)),
               this, SLOT(slotShareContributionDetailsGroupToggled(bool)));

    d->setProperty(d->contributionDetailsWidget, "group_regional_settings", "title",
                   d->property(d->contributionDetailsWidget, "group_regional_settings", "title")
                       .toString().arg(d->scores.value(KexiUserFeedbackAgent::RegionalSettingsArea)));
    d->connect(d->contributionDetailsWidget, "group_regional_settings", SIGNAL(toggled(bool)),
               this, SLOT(slotShareContributionDetailsGroupToggled(bool)));
                   
    d->detailsDataVisible = false;
    slotShareContributionDetailsToggled(
        d->property(d->contributionDetailsWidget, "group_share", "checked").toBool());
    d->detailsDataVisible = true; // to switch off
    slotToggleContributionDetailsDataVisibility();
    d->connect(d->contributionDetailsWidget, "group_share", SIGNAL(toggled(bool)),
               this, SLOT(slotShareContributionDetailsToggled(bool)));
    d->connect(d->contributionDetailsWidget, "link_show_shared_info",
               SIGNAL(linkActivated(QString)),
               this, SLOT(slotToggleContributionDetailsDataVisibility()));

    d->setProperty(d->contributionDetailsWidget, "label_where_is_info_sent", "visible", false);
    
    ScrollArea *contributionDetailsArea = new ScrollArea(d->msgWidget);
    d->contributionDetailsLayout->addWidget(contributionDetailsArea, 1, 1);
    contributionDetailsArea->setWidget(d->contributionDetailsWidget);
    d->msgWidget->animatedShow();
    d->msgWidget->setPaletteInherited();
}

void KexiWelcomeStatusBar::updateContributionGroupCheckboxes()
{
    KexiUserFeedbackAgent *f = KexiMainWindowIface::global()->userFeedbackAgent();
    d->setProperty(d->contributionDetailsWidget, "group_system", "checked",
                   bool(f->enabledAreas() & KexiUserFeedbackAgent::SystemInfoArea));
    d->setProperty(d->contributionDetailsWidget, "group_screen", "checked",
                   bool(f->enabledAreas() & KexiUserFeedbackAgent::ScreenInfoArea));
    d->setProperty(d->contributionDetailsWidget, "group_regional_settings", "checked",
                   bool(f->enabledAreas() & KexiUserFeedbackAgent::RegionalSettingsArea));
}

void KexiWelcomeStatusBar::slotShareContributionDetailsToggled(bool on)
{
    //kDebug() << sender();
    QWidget* group_share = d->widget(d->contributionDetailsWidget,
                                     "group_share");
    KexiUserFeedbackAgent *f = KexiMainWindowIface::global()->userFeedbackAgent();
    if (sender() == group_share) {
        f->setEnabledAreas(on ? KexiUserFeedbackAgent::AllAreas : KexiUserFeedbackAgent::NoAreas);
        updateContributionGroupCheckboxes();
    }
    if (!group_share) {
        return;
    }
    for (int i=0; i < group_share->layout()->count(); i++) {
        QWidget *w = group_share->layout()->itemAt(i)->widget();
        if (w) {
            /*if (w->objectName() == "group_basic" && on) {
                // do nothing
            }
            else*/
            {
                w->setVisible(on);
            }
        }
    }
    if (d->detailsDataVisible) {
        slotToggleContributionDetailsDataVisibility();
    }
    // fill shared values
    foreach(QLabel* lbl, d->contributionDetailsWidget->findChildren<QLabel*>()) {
        if (lbl->objectName().startsWith(QLatin1String("value_"))) {
            QString name = lbl->objectName().mid(6); // cut "value_"
            QVariant value;
            if (name == QLatin1String("screen_size")) {
                value = QString("%1 x %2").arg(f->value("screen_width").toString())
                                          .arg(f->value("screen_height").toString());
            }
            else if (name == QLatin1String("country")) {
                if (d->countryMask.isEmpty()) {
                    d->countryMask = lbl->text();
                }
                value = d->countryMask
                    .arg(KGlobal::locale()->countryCodeToName(f->value(name).toString()))
                    .arg(f->value(name).toString());
            }
            else if (name == QLatin1String("language")) {
                if (d->languageMask.isEmpty()) {
                    d->languageMask = lbl->text();
                }
                value = d->languageMask
                    .arg(KGlobal::locale()->languageCodeToName(f->value(name).toString()))
                    .arg(f->value(name).toString());
            }
            else {
                value = f->value(name);
            }

            if (value.type() == QVariant::Bool) {
                value = value.toBool() ? KStandardGuiItem::yes().plainText()
                                       : KStandardGuiItem::no().plainText();
            }

            if (!value.isNull()) {
                lbl->setText(value.toString());
            }
        }
        else if (lbl->objectName().startsWith(QLatin1String("desc_"))) {
//             QFont f(lbl->font());
//             f.setPointSizeF(KGlobalSettings::smallestReadableFont().pointSizeF());
            lbl->setFont(d->smallFont);
/*            QPalette pal(lbl->palette());
            QColor c(pal.color(QPalette::WindowText));
            c.setAlpha(100);
            pal.setColor(QPalette::WindowText, Qt::red);
            pal.setColor(QPalette::Text, Qt::green);
            lbl->setPalette(pal);*/
        }
    }
}

static void setArea(KexiUserFeedbackAgent::Areas *areas,
                    KexiUserFeedbackAgent::Area area, bool on)
{
    *areas |= area;
    if (!on) {
        *areas ^= area;
    }
}

void KexiWelcomeStatusBar::slotShareContributionDetailsGroupToggled(bool on)
{
    if (!sender()) {
        return;
    }
    const QString name = sender()->objectName();
    KexiUserFeedbackAgent *f = KexiMainWindowIface::global()->userFeedbackAgent();
    KexiUserFeedbackAgent::Areas areas = f->enabledAreas();
    //kDebug() << areas;
    if (name == "group_system") {
        setArea(&areas, KexiUserFeedbackAgent::SystemInfoArea, on);
    }
    else if (name == "group_screen") {
        setArea(&areas, KexiUserFeedbackAgent::ScreenInfoArea, on);
    }
    else if (name == "group_regional_settings") {
        setArea(&areas, KexiUserFeedbackAgent::RegionalSettingsArea, on);
    }
    if (areas) {
        areas |= KexiUserFeedbackAgent::AnonymousIdentificationArea;
    }
    f->setEnabledAreas(areas);
    kDebug() << f->enabledAreas();
}

void KexiWelcomeStatusBar::slotToggleContributionDetailsDataVisibility()
{
/*    QWidget* group_share = d->widget(d->contributionDetailsWidget, "group_share");
    if (!group_share) {
        return;
    }
    bool enabled = group_share->property("checked");*/
    QWidget* value_app_ver = d->widget(d->contributionDetailsWidget, "value_app_ver");
    if (!value_app_ver) {
        return;
    }
    d->detailsDataVisible = !d->detailsDataVisible;
    if (d->detailsDataVisible) {
        d->setProperty(d->contributionDetailsWidget, "link_show_shared_info", "visible", false);
        d->setProperty(d->contributionDetailsWidget, "label_where_is_info_sent", "visible", true);
    }
    bool show = d->contributionDetailsWidget->isVisible();
    QList<QWidget*> list;
    d->contributionDetailsWidget->hide();
    QWidget* group_basic = d->widget(d->contributionDetailsWidget, "group_basic");
    if (group_basic) {
        list += group_basic->findChildren<QWidget*>();
    }
//     if (group_basic) {
//         group_basic->setVisible(d->detailsDataVisible);
//     }
    QWidget* group_system = d->widget(d->contributionDetailsWidget, "group_system");
    if (group_system) {
        list += group_system->findChildren<QWidget*>();
    }
    QWidget* group_screen = d->widget(d->contributionDetailsWidget, "group_screen");
    if (group_screen) {
        list += group_screen->findChildren<QWidget*>();
    }
    QWidget* group_regional_settings = d->widget(d->contributionDetailsWidget, "group_regional_settings");
    if (group_regional_settings) {
        list += group_regional_settings->findChildren<QWidget*>();
    }

    foreach (QWidget* w, list) {
        if (qobject_cast<QLabel*>(w) && !w->objectName().startsWith(QLatin1String("desc_"))) {
            //kDebug() << "+++" << w;
            w->setVisible(d->detailsDataVisible);
        }
    }
    if (show) {
        d->contributionDetailsWidget->show();
    }
}

// Contribution Details END

#include "KexiWelcomeStatusBar.moc"
#include "KexiWelcomeStatusBar_p.moc"
