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
#include <kexiutils/KexiFadeWidgetEffect.h>
#include "KexiUserFeedbackAgent.h"

#include <KColorScheme>
#include <KStandardGuiItem>
#include <KConfigGroup>
#include <KIO/Job>
#include <KIO/CopyJob>
#include <kcodecs.h>
#include <KSharedConfig>
#include <KLocalizedString>
#include <KMessageBox>

#include <QDebug>
#include <QEvent>
#include <QLayout>
#include <qmath.h>
#include <QFile>
#include <QDesktopServices>
#include <QUiLoader>
#include <QScrollArea>
#include <QLabel>
#include <QResource>
#include <QTimer>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QDir>
#include <QFontDatabase>
#include <QAction>
#include <QLocale>

#include <stdio.h>

static const int GUI_UPDATE_INTERVAL = 60; // update interval for GUI, in minutes
static const int DONATION_INTERVAL = 10; // donation interval, in days
static const int UPDATE_FILES_LIST_SIZE_LIMIT = 1024 * 128;
static const int UPDATE_FILES_COUNT_LIMIT = 128;

//! @return x.y.0
static QString stableVersionStringDot0()
{
    return QString::number(Kexi::stableVersionMajor()) + '.'
           + QString::number(Kexi::stableVersionMinor()) + ".0";
}

static QString basePath()
{
    return QString("kexi/status/") + stableVersionStringDot0();
}

static QString findFilename(const QString &guiFileName)
{
    const QString result = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  basePath() + '/' + guiFileName);
    //qDebug() << result;
    return result;
}

// ---

class KexiWelcomeStatusBarGuiUpdater::Private
{
public:
    Private()
     : configGroup(KConfigGroup(KSharedConfig::openConfig()->group("User Feedback")))
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
    return f->serviceUrl() + QString("/ui/%1/").arg(stableVersionStringDot0())
        + fname;
}

void KexiWelcomeStatusBarGuiUpdater::update()
{
    QDateTime lastStatusBarUpdate = d->configGroup.readEntry("LastStatusBarUpdate", QDateTime());
    if (lastStatusBarUpdate.isValid()) {
        int minutes = lastStatusBarUpdate.secsTo(QDateTime::currentDateTime()) / 60;

        if (minutes < GUI_UPDATE_INTERVAL) {
            qDebug() << "gui updated" << minutes << "min. ago, next auto-update in"
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
    QByteArray postData = stableVersionStringDot0().toLatin1();
    KIO::Job* sendJob = KIO::storedHttpPost(postData,
                                            QUrl(uiPath(".list")),
                                            KIO::HideProgressInfo);
    connect(sendJob, SIGNAL(result(KJob*)), this, SLOT(sendRequestListFilesFinished(KJob*)));
    sendJob->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
}

void KexiWelcomeStatusBarGuiUpdater::sendRequestListFilesFinished(KJob* job)
{
    if (job->error()) {
        qWarning() << "Error while receiving .list file - no files will be updated";
        //! @todo error...
        return;
    }
    KIO::StoredTransferJob* sendJob = qobject_cast<KIO::StoredTransferJob*>(job);
    QString result = sendJob->data();
    if (result.length() > UPDATE_FILES_LIST_SIZE_LIMIT) { // anit-DOS protection
        qWarning() << "Too large .list file (" << result.length()
            << "); the limit is" << UPDATE_FILES_LIST_SIZE_LIMIT
            << "- no files will be updated";
        return;
    }
    qDebug() << result;
    QStringList data = result.split('\n', QString::SkipEmptyParts);
    result.clear();
    d->fileNamesToUpdate.clear();
    if (data.count() > UPDATE_FILES_COUNT_LIMIT) { // anti-DOS protection
        qWarning() << "Too many files to update (" << data.count()
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
                    qWarning() << "Invalid hash" << hash << "in line" << i+1 << "- no files will be updated";
                    return;
                }
                if ((*it).mid(32, 2) != "  ") {
                    qWarning() << "Two spaces expected but found" << (*it).mid(32, 2)
                        << "in line" << i+1 << "- no files will be updated";
                    return;
                }
                if (remoteFname.contains(QRegExp("\\s"))) {
                    qWarning() << "Filename expected without whitespace but found" << remoteFname
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
    QList<QUrl> sourceFiles;
    foreach (const QString &fname, d->fileNamesToUpdate) {
        sourceFiles.append(QUrl(uiPath(fname)));
    }
    QTemporaryDir tempDir(QDir::tempPath() + "/kexi-status");
    tempDir.setAutoRemove(false);
    d->tempDir = tempDir.path();
    qDebug() << tempDir.path();
    KIO::CopyJob *copyJob = KIO::copy(sourceFiles,
                                      QUrl("file://" + tempDir.path()),
                                      KIO::HideProgressInfo | KIO::Overwrite);
    connect(copyJob, SIGNAL(result(KJob*)), this, SLOT(filesCopyFinished(KJob*)));
    //qDebug() << "copying from" << QUrl(uiPath(fname)) << "to"
    //         << (dir + fname);
}

void KexiWelcomeStatusBarGuiUpdater::checkFile(const QByteArray &hash,
                                               const QString &remoteFname,
                                               QStringList *fileNamesToUpdate)
{
    QString localFname = findFilename(remoteFname);
    if (localFname.isEmpty()) {
        fileNamesToUpdate->append(remoteFname);
        qDebug() << "missing filename" << remoteFname << "- download it";
        return;
    }
    QFile file(localFname);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "could not open file" << localFname << "- update it";
        fileNamesToUpdate->append(remoteFname);
        return;
    }
    QCryptographicHash md5(QCryptographicHash::Md5);
    if (!md5.addData(&file)) {
        qWarning() << "could not check MD5 for file" << localFname << "- update it";
        fileNamesToUpdate->append(remoteFname);
        return;
    }
    if (md5.result() != hash) {
        qDebug() << "not matching file" << localFname << "- update it";
        fileNamesToUpdate->append(remoteFname);
    }
}

void KexiWelcomeStatusBarGuiUpdater::filesCopyFinished(KJob* job)
{
    if (job->error()) {
        //! @todo error...
        qDebug() << "ERROR:" << job->errorString();
        return;
    }
    KIO::CopyJob* copyJob = qobject_cast<KIO::CopyJob*>(job);
    qDebug() << "DONE" << copyJob->destUrl();

    QString dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                + QLatin1Char('/') + basePath() + '/');
    bool ok = true;
    if (!QDir(dir).exists()) {
        if (!QDir().mkpath(dir)) {
            ok = false;
            qWarning() << "Could not create" << dir;
        }
    }
    if (ok) {
        foreach (const QString &fname, d->fileNamesToUpdate) {
            if (!QFile::rename(d->tempDir + fname, dir + fname)) {
                qWarning() << "cannot move" << (d->tempDir + fname) << "to" << (dir + fname);
            }
        }
    }
    QDir(d->tempDir).removeRecursively();
}

// ---

//! @internal
class ScrollArea : public QScrollArea
{
public:
    explicit ScrollArea(QWidget *parent = 0) : QScrollArea(parent)
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
        //qDebug() << "_____________" << isEnabled();

        foreach(QLabel* lbl, widget()->findChildren<QLabel*>()) {
            QString t = lbl->text();
            QRegExp re("<a.*>");
            re.setMinimal(true);
            int pos = 0;
            int oldPos = 0;
            QString newText;
            //qDebug() << "t:" << t;
            while ((pos = re.indexIn(t, pos)) != -1) {
                //qDebug() << "pos:" << pos;
                //qDebug() << "newText += t.mid(oldPos, pos - oldPos)"
                //    << t.mid(oldPos, pos - oldPos);
                newText += t.mid(oldPos, pos - oldPos);
                //qDebug() << "newText1:" << newText;
                //qDebug() << lbl->objectName() << "~~~~" << t.mid(pos, re.matchedLength());
                QString a = t.mid(pos, re.matchedLength());
                //qDebug() << "a:" << a;
                int colPos = a.indexOf("color:");
                if (colPos == -1) { // add color
                    a.insert(a.length() - 1, " style=\"color:" + linkColor.name() + ";\"");
                }
                else { // replace color
                    colPos += qstrlen("color:");
                    for (;colPos < a.length() && a[colPos] == ' '; colPos++) {
                    }
                    if (colPos < a.length() && a[colPos] == '#') {
                        colPos++;
                        int i = colPos;
                        for (;i < a.length(); i++) {
                            if (a[i] == ';' || a[i] == ' ' || a[i] == '"' || a[i] == '\'')
                                break;
                        }
                        //qDebug() << "******" << a.mid(colPos, i - colPos);
                        a.replace(colPos, i - colPos, linkColor.name().mid(1));
                    }
                }
                //qDebug() << "a2:" << a;
                newText += a;
                //qDebug() << "newText2:" << newText;
                pos += re.matchedLength();
                oldPos = pos;
                //qDebug() << "pos2:" << pos;
            }
            //qDebug() << "oldPos:" << oldPos;
            newText += t.mid(oldPos);
            //qDebug() << "newText3:" << newText;
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
    explicit Private(KexiWelcomeStatusBar* _q)
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
        donationScore = 20;
        donated = false;
        //qDebug() << "totalFeedbackScore:" << totalFeedbackScore;
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
        //qDebug() << score;
        return score;
    }

    template<typename T>
    T widgetOfClass(T parent, const char *widgetName) const
    {
        T w = parent->findChild<T>(widgetName);
        if (!w) {
            qWarning() << "NO SUCH widget" << widgetName << "in" << parent;
        }
        return w;
    }

    QWidget* widget(QWidget *parent, const char *widgetName) const
    {
        return widgetOfClass<QWidget*>(parent, widgetName);
    }

    QObject* object(QObject *parent, const char *objectName) const
    {
        QObject *o = parent->findChild<QObject*>(objectName);
        if (!o) {
            qWarning() << "NO SUCH object" << objectName << "in" << parent;
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
        KexiFadeWidgetEffect *animation = new KexiFadeWidgetEffect(w);
        QObject::connect(animation, SIGNAL(destroyed()), w, SLOT(hide()));
        animation->start();
    }

    QWidget* loadGui(const QString &guiFileName, QWidget *parentWidget = 0)
    {
        QString fname = findFilename(guiFileName);
        if (fname.isEmpty()) {
            qWarning() << "filename" << fname << "not found";
            return 0;
        }
        QFile file(fname);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "could not open file" << fname;
            return 0;
        }
        QUiLoader loader;
        QWidget* widget = loader.load(&file, parentWidget);
        if (!widget) {
            qWarning() << "could load ui from file" << fname;
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
        int smallFontSize = qFloor((QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont).pointSizeF()
                                   + q->font().pointSizeF())
                                   / 2.0);
        smallFont = q->font();
        smallFont.setPointSizeF(smallFontSize);
        widget->setFont(smallFont);
        //delete statusWidget;
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

        setProperty(statusWidget, "link_donate", "text",
                    property(statusWidget, "link_donate", "text").toString().arg(donationScore));

        updateDonationInfo();
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

        setProperty(statusWidget, "donation_url", "visible", false);
        connect(statusWidget, "link_donate", SIGNAL(linkActivated(QString)),
                q, SLOT(showDonation()));
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
        if (donated) {
            progress += donationScore;
        }
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

    void updateDonationInfo()
    {
        KConfigGroup configGroup(KSharedConfig::openConfig()->group("User Feedback"));
        QDateTime lastDonation = configGroup.readEntry("LastDonation", QDateTime());
        if (lastDonation.isValid()) {
            int days = lastDonation.secsTo(QDateTime::currentDateTime()) / 60 / 60 / 24;
            if (days >= DONATION_INTERVAL) {
                donated = false;
                qDebug() << "last donation declared" << days << "days ago, next in"
                    << (DONATION_INTERVAL - days) << "days.";
            }
            else if (days >= 0) {
                donated = true;
            }
        }
        //show always: setProperty(statusWidget, "donate", "visible", !donated);
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
            //qDebug() << q->pos() << q->mapToGlobal(QPoint(0, 100));
            QPoint p(q->mapToGlobal(QPoint(0, 100)));
            QWidget *alignToWidget = this->widget(statusWidget, alignToWidgetName.toLatin1());
            if (alignToWidget) {
                p.setY(
                    alignToWidget->mapToGlobal(
                        QPoint(-5, alignToWidget->height() / 2)).y());
                if (calloutAlignment == AlignToWidget) {
                    p.setX(alignToWidget->mapToGlobal(QPoint(-5, 0)).x());
                    //qDebug() << p;
                }
            }
            else {
                qWarning() << alignToWidgetName << "not found!";
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
        //qDebug() << (q->parentWidget()->width() - q->width()) << "***";
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
            qWarning() << alignToWidgetName << "not found!";
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
    QAction *helpAction;
    QAction *shareAction;
    QAction *cancelAction;
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
    int donationScore;
    bool donated;

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
    d->updateDonationInfo();
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
    QLabel *lbl = widget->findChild<QLabel*>("question");
    if (!lbl) {
        return;
    }
    KexiContextMessage msg(lbl->text());
    delete widget;
    if (!d->helpAction) {
        d->helpAction = new QAction(KStandardGuiItem::help().icon(),
                                    KStandardGuiItem::help().text(), this);
        connect(d->helpAction, SIGNAL(triggered()), this, SLOT(showContributionHelp()));
    }
    if (!d->shareAction) {
        d->shareAction = new QAction(KStandardGuiItem::yes().icon(), xi18n("Share"), this);
        connect(d->shareAction, SIGNAL(triggered()), this, SLOT(slotShareFeedback()));
    }
    if (!d->cancelAction) {
        d->cancelAction = new QAction(KStandardGuiItem::cancel().icon(),
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

    d->msgWidget->animatedShow();
}

void KexiWelcomeStatusBar::showDonation()
{
    if (!sender()) {
        return;
    }
    if (KMessageBox::Yes != KMessageBox::questionYesNo(this,
       xi18nc("@info donate to the project", "<para><title>Kexi may be totally free, but its development is costly.</title><nl/>"
            "<para>Power, hardware, office space, internet access, traveling for meetings - everything costs.</para>"
            "<para>Direct donation is the easiest and fastest way to efficiently support the Kexi Project. "
            "Everyone, regardless of any degree of involvement can do so.</para>"
            "<para>What do you receive for your donation? Kexi will become more feature-full and stable as "
            "contributors will be able to devote more time to Kexi. Not only you can "
            "expect new features, but you can also have an influence on what features are added!</para>"
            "<para>Currently we are accepting donations through <emphasis>BountySource</emphasis> (a funding platform "
            "for open-source software) using secure PayPal, Bitcoin and Google Wallet transfers.</para>"
            "<para>Contact us at http://community.kde.org/Kexi/Contact for more information.</para>"
            "<para>Thanks for your support!</para>"),
       xi18n("Donate to the Project"),
       KGuiItem(xi18n("Proceed to the Donation Web Page"), QIcon(":/icons/heart.png")),
       KGuiItem(xi18nc("Do not donate now", "Not Now"))))
    {
        return;
    }
    QUrl donationUrl(d->property(this, "donation_url", "text").toString());
    if (donationUrl.isValid()) {
        QDesktopServices::openUrl(donationUrl);
        d->donated = true;
        d->updateStatusWidget();
        KConfigGroup configGroup(KSharedConfig::openConfig()->group("User Feedback"));
        int donationsCount = configGroup.readEntry("DonationsCount", 0);
        configGroup.writeEntry("LastDonation", QDateTime::currentDateTime());
        configGroup.writeEntry("DonationsCount", donationsCount + 1);
    }
    else {
        qWarning() << "Invalid donation URL" << donationUrl;
    }
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
    //qDebug() << sender();
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
            w->setVisible(on);
        }
    }
    if (d->detailsDataVisible) {
        slotToggleContributionDetailsDataVisibility();
    }
    // fill shared values
    QLocale locale;
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
                    .arg(f->value(name).toString() /*!< @todo KEXI3 port KLocale::global()->countryCodeToName(f->value(name).toString()) */)
                    .arg(f->value(name).toString());
            }
            else if (name == QLatin1String("language")) {
                if (d->languageMask.isEmpty()) {
                    d->languageMask = lbl->text();
                }
                value = d->languageMask
                    .arg(f->value(name).toString() /*!< @todo KEXI3 port KLocale::global()->languageCodeToName(f->value(name).toString()) */)
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
            lbl->setFont(d->smallFont);
        }
    }
    QLabel* lbl;
    KConfigGroup configGroup(KSharedConfig::openConfig()->group("User Feedback"));
    if ((lbl = d->contributionDetailsWidget->findChild<QLabel*>("value_recent_donation"))) {
        QDateTime lastDonation = configGroup.readEntry("LastDonation", QDateTime());
        QString recentDonation = "-";
        if (lastDonation.isValid()) {
            int days = lastDonation.secsTo(QDateTime::currentDateTime()) / 60 / 60 / 24;
            if (days == 0) {
                recentDonation = xi18nc("Donation today", "today");
            }
            else if (days > 0) {
                recentDonation = xi18ncp("Recent donation date (xx days)", "%1 (1 day)", "%1 (%2 days)",
                                         locale.toString(lastDonation), days);
            }
        }
        lbl->setText(recentDonation);
    }
    if ((lbl = d->contributionDetailsWidget->findChild<QLabel*>("value_donations_count"))) {
        int donationsCount = configGroup.readEntry("DonationsCount", 0);
        if (donationsCount == 0) {
            lbl->setText(QString::number(donationsCount));
        }
        else {
            lbl->setText(xi18nc("donations count", "%1 (thanks!)", donationsCount));
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
    //qDebug() << areas;
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
    //qDebug() << f->enabledAreas();
}

void KexiWelcomeStatusBar::slotToggleContributionDetailsDataVisibility()
{
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
            //qDebug() << "+++" << w;
            w->setVisible(d->detailsDataVisible);
        }
    }
    if (show) {
        d->contributionDetailsWidget->show();
    }
}

// Contribution Details END
