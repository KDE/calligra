/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <QTextCodec>

#include <klocale.h>
#include <kcharsets.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <QLabel>
#include <QLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <kurlrequester.h>

#include <ExportDialogUI.h>
#include <ExportDialog.h>

HtmlExportDialog :: HtmlExportDialog(QWidget* parent)
        : KDialog(parent),
        m_dialog(new ExportDialogUI(this))
{
    setButtons(Ok | Cancel);
    setCaption(i18n("KWord's HTML Export Filter"));
    kapp->restoreOverrideCursor();

    QStringList encodingList;

    encodingList += i18nc("Descriptive encoding name", "Recommended ( %1 )" , "UTF-8");
    encodingList += i18nc("Descriptive encoding name", "Locale ( %1 )" , QString(QTextCodec::codecForLocale()->name()));
    encodingList += KGlobal::charsets()->descriptiveEncodingNames();

    m_dialog->comboBoxEncoding->addItems(encodingList);

    m_dialog->KURL_ExternalCSS->setMode(KFile::ExistingOnly);

    connect(m_dialog->radioModeEnhanced, SIGNAL(toggled(bool)),
            SLOT(setCSSEnabled(bool)));
    connect(m_dialog->checkExternalCSS, SIGNAL(toggled(bool)),
            m_dialog->KURL_ExternalCSS, SLOT(setEnabled(bool)));

    setMainWidget(m_dialog);
}

HtmlExportDialog :: ~HtmlExportDialog(void)
{
    kapp->setOverrideCursor(Qt::WaitCursor);
}

void HtmlExportDialog::setCSSEnabled(bool b)
{
    m_dialog->checkExternalCSS->setEnabled(b);
    m_dialog->KURL_ExternalCSS->setEnabled(b && m_dialog->checkExternalCSS->isChecked());
}

bool HtmlExportDialog::isXHtml(void) const
{
    if (m_dialog->radioDocType1 == m_dialog->buttonGroup1->selected())
        return false;
    else if (m_dialog->radioDocType2 == m_dialog->buttonGroup1->selected())
        return true;
    return true;
}

QTextCodec* HtmlExportDialog::getCodec(void) const
{
    const QString strCodec(KGlobal::charsets()->encodingForName(m_dialog->comboBoxEncoding->currentText()));
    kDebug(30503) << "Encoding:" << strCodec;

    bool ok = false;
    QTextCodec* codec = QTextCodec::codecForName(strCodec.toUtf8());

    // If QTextCodec has not found a valid encoding, so try with KCharsets.
    if (codec) {
        ok = true;
    } else {
        codec = KGlobal::charsets()->codecForName(strCodec, ok);
    }

    // Still nothing?
    if (!codec || !ok) {
        // Default: UTF-8
        kWarning(30503) << "Cannot find encoding:" << strCodec;
        // ### TODO: what parent to use?
        KMessageBox::error(0, i18n("Cannot find encoding: %1", strCodec));
        return 0;
    }

    return codec;
}

HtmlExportDialog::Mode HtmlExportDialog::getMode(void) const
{
    if (m_dialog->radioModeEnhanced->isChecked()) {
        if (m_dialog->checkExternalCSS->isChecked()) {
            return CustomCSS;
        } else {
            return DefaultCSS;
        }
    } else if (m_dialog->radioModeBasic->isChecked()) {
        return Basic;
    } else if (m_dialog->radioModeLight->isChecked()) {
        return Light;
    }
    return DefaultCSS;//Our default
}

QString HtmlExportDialog::cssURL() const
{
    return m_dialog->KURL_ExternalCSS->url().url();
}

#include <ExportDialog.moc>
