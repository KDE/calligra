/* This file is part of the KDE project
   Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiBugReportDialog.h"
#include "KexiUserFeedbackAgent.h"
#include <core/KexiMainWindowIface.h>
#include <kexiutils/utils.h>
#include <kexi_version.h>

#include <KAboutData>
#include <KTitleWidget>
#include <KToolInvocation>
#include <KLocalizedString>

#include <QGridLayout>
#include <QLabel>
#include <QDesktopServices>

/*! Make a deep copy so we can modify the version.
 We need to override the version since it sometimes can look like
 "2.9 Alpha (git 4e06281 master)" what confuses the bugs.kde.org service.
 KEXI_VERSION_STRING returns just 2.9 Alpha */
static KAboutData copyAboutDataWithFixedVersion()
{
    KAboutData data = KAboutData::applicationData();
    data.setVersion(KEXI_VERSION_STRING);
    return data;
}

KexiBugReportDialog::KexiBugReportDialog(QWidget *parent)
 : KBugReport(copyAboutDataWithFixedVersion(), parent)
{
    setModal(true);
    setWindowTitle(xi18nc("@title:window", "Report a Bug or Wish"));
    collectData();
    QWidget *title = KexiUtils::findFirstChild<QWidget*>(this, "KTitleWidget");
    if (title) {
        title->hide();
        QVBoxLayout* lay = qobject_cast<QVBoxLayout*>(title->layout());
        lay->insertSpacing(0, 6);
        lay->addStretch(1);
    }
    QGridLayout *glay = KexiUtils::findFirstChild<QGridLayout*>(this, "QGridLayout");
    if (glay) {
        if (glay->itemAtPosition(0, 0) && glay->itemAtPosition(0, 0)->widget()) {
            glay->itemAtPosition(0, 0)->widget()->hide(); // app name label
        }
        if (glay->itemAtPosition(0, 1) && glay->itemAtPosition(0, 1)->widget()) {
            glay->itemAtPosition(0, 1)->widget()->hide(); // app name
        }
        QLabel *lbl;
        if (glay->itemAtPosition(1, 0) && ((lbl = qobject_cast<QLabel*>(glay->itemAtPosition(1, 0)->widget())))) {
            lbl->setAlignment(Qt::AlignRight);
        }
        if (glay->itemAtPosition(2, 0) && ((lbl = qobject_cast<QLabel*>(glay->itemAtPosition(2, 0)->widget())))) {
            lbl->setText(xi18n("Operating system & platform:"));
            lbl->setAlignment(Qt::AlignRight);
        }
        if (glay->itemAtPosition(2, 1) && ((lbl = qobject_cast<QLabel*>(glay->itemAtPosition(2, 1)->widget())))) {
            QString op_sys = m_op_sys;
            QString rep_platform = m_rep_platform;
            if (op_sys == "other") {
                op_sys = xi18nc("Other operating system", "Other");
            }
            if (rep_platform == "Other") {
                rep_platform = xi18nc("Other platform", "Other");
            }
            lbl->setText(xi18nc("<operating system>, <platform>", "%1, %2", op_sys, rep_platform));
        }
        if (glay->itemAtPosition(3, 0) && glay->itemAtPosition(3, 0)->widget()) {
            glay->itemAtPosition(3, 0)->widget()->hide(); // compiler label
        }
        if (glay->itemAtPosition(3, 1) && glay->itemAtPosition(3, 1)->widget()) {
            glay->itemAtPosition(3, 1)->widget()->hide(); // compiler
        }
        glay->addItem(new QSpacerItem(1, 10), glay->count(), 0);
    }
    //! @todo KEXI3 test it
    setMinimumHeight(sizeHint().height()); // WORKAROUND: prevent "cropped" kcombobox
    adjustSize();
}

void KexiBugReportDialog::accept()
{
    // override, based on KBugReport::accept()
    QUrl url("https://bugs.kde.org/enter_bug.cgi");
    url.addQueryItem("format", "guided"); // use the guided form
    // the string format is product/component, where component is optional
    url.addQueryItem("product", "kexi");
    url.addQueryItem("version", KEXI_VERSION_STRING);
#if 0   //! @todo add when enter_bug.cgi supports adding comments or when Kexi gets
    //! own Bug Report GUI and communicates using RPC.
    QString desc;
    if (0 != qstrcmp(Kexi::fullVersionString(), Kexi::versionString())) {
        desc += futureI18nc("Full version of Kexi app", "Full version: %1", Kexi::fullVersionString());
    }
    body += futureI18n("(filed directly from Kexi app)");
    url.addQueryItem("comment", m_aboutData->version());
#endif
    url.addQueryItem("op_sys", m_op_sys);
    url.addQueryItem("rep_platform", m_rep_platform);
    QDesktopServices::openUrl(url);
    QDialog::accept();
}

void KexiBugReportDialog::collectData()
{
#ifdef Q_OS_LINUX
    m_op_sys = "Linux";
    const QString linux_id = KexiMainWindowIface::global()->userFeedbackAgent()->value("linux_id").toString().toLower();
    const QString linux_desc = KexiMainWindowIface::global()->userFeedbackAgent()->value("linux_desc").toString().toLower();
    if (linux_id.contains("arch")) {
        m_rep_platform = "Archlinux Packages";
    }
    else if (linux_id.contains("balsam")) {
        m_rep_platform = "Balsam Professional";
    }
    else if (linux_id.contains("chakra")) {
        m_rep_platform = "Chakra";
    }
    else if (linux_id.contains("debian")) {
        if (linux_id.contains("unstable")) {
            m_rep_platform = "Debian unstable";
        }
        else if (linux_id.contains("testing")) {
            m_rep_platform = "Debian testing";
        }
        else {
            m_rep_platform = "Debian stable";
        }
    }
    else if (linux_id.contains("exherbo")) {
        m_rep_platform = "Exherbo Packages";
    }
    else if (linux_id.contains("fedora")) {
        m_rep_platform = "Fedora RPMs";
    }
    else if (linux_id.contains("gentoo")) {
        m_rep_platform = "Gentoo Packages";
    }
    else if (linux_id.contains("ubuntu")) {
        m_rep_platform = "Ubuntu Packages";
    }
    else if (linux_id.contains("kubuntu")) {
        m_rep_platform = "Kubuntu Packages";
    }
    else if (linux_id.contains("mageia")) {
        m_rep_platform = "Mageia RPMs";
    }
    else if (linux_id.contains("mint")) {
        if (linux_desc.contains("debian")) {
            m_rep_platform = "Mint (Debian based)";
        }
        else {
            m_rep_platform = "Mint (Ubuntu based)";
        }
    }
    else if (linux_id.contains("opensuse")) {
        m_rep_platform = "openSUSE RPMs";
    }
    else if (linux_id.contains("pclinuxos")) {
        m_rep_platform = "PCLinuxOS";
    }
    else if (linux_id.contains("redhat")) {
        m_rep_platform = "RedHat RPMs";
    }
    else if (linux_id.contains("slackware")) {
        m_rep_platform = "Slackware Packages";
    }
    else {
        m_rep_platform = "Other";
    }
#elif defined(Q_OS_FREEBSD)
    m_op_sys = "FreeBSD";
    m_rep_platform = "FreeBSD Ports";
#elif defined(Q_OS_NETBSD)
    m_op_sys = "NetBSD";
    m_rep_platform = "NetBSD pkgsrc";
#elif defined(Q_OS_OPENBSD)
    m_op_sys = "OpenBSD";
    m_rep_platform = "OpenBSD Packages";
#elif defined(Q_OS_AIX)
    m_op_sys = "AIX";
    m_rep_platform = "AIX Packages";
#elif defined(Q_OS_HPUX)
    m_op_sys = "HP-UX";
    m_rep_platform = "Other";
#elif defined(Q_OS_IRIX)
    m_op_sys = "IRIX";
    m_rep_platform = "Other";
#elif defined(Q_OS_OSF)
    m_op_sys = "Tru64";
    m_rep_platform = "Tru64 Unix Packages";
#elif defined(Q_OS_SOLARIS)
    m_op_sys = "Solaris";
    m_rep_platform = "Solaris Packages";
#elif defined(Q_OS_MAC)
    m_op_sys = "OS X";
    m_rep_platform = "Mac OS X Disk Images";
#elif defined(Q_OS_WIN)
    m_op_sys = "MS Windows";
    m_rep_platform = "MS Windows";
#elif defined(Q_OS_WINCE)
    m_op_sys = "Windows CE";
    m_rep_platform = "Windows CE";
#elif defined(Q_OS_ANDROID)
    m_op_sys = "Android 4.x";
    m_rep_platform = "Android";
#else
    m_op_sys = "other";
    m_rep_platform = "Other";
#endif
}
