/*
   This file is part of the KDE project
   Copyright 2001, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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
#include <QRadioButton>

#include <kdebug.h>
#include <klocale.h>
#include <kcharsets.h>
#include <kglobal.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <kmessagebox.h>

#include <ExportDialog.h>

AsciiExportDialog :: AsciiExportDialog(QWidget* parent)
        : KDialog(parent),
        m_dialog(new QWidget(this))
{
    m_ui.setupUi(m_dialog);
    m_radioGroup.addButton(m_ui.radioEndOfLineLF);
    m_radioGroup.addButton(m_ui.radioEndOfLineCRLF);
    m_radioGroup.addButton(m_ui.radioEndOfLineCR);

    setButtons(Ok | Cancel);
    setCaption(i18n("KWord's Plain Text Export Filter"));
    kapp->restoreOverrideCursor();

    QStringList encodings;
    encodings << i18nc("Descriptive encoding name", "Recommended ( %1 )" , QString("UTF-8"));
    encodings << i18nc("Descriptive encoding name", "Locale ( %1 )" , QString(QTextCodec::codecForLocale()->name()));
    encodings += KGlobal::charsets()->descriptiveEncodingNames();
    // Add a few non-standard encodings, which might be useful for text files
    const QString description(i18nc("Descriptive encoding name", "Other ( %1 )"));
    encodings << description.arg("Apple Roman"); // Apple
    encodings << description.arg("IBM 850") << description.arg("IBM 866"); // MS DOS
    encodings << description.arg("CP 1258"); // Windows

    m_ui.comboBoxEncoding->addItems(encodings);

    setMainWidget(m_dialog);

}

AsciiExportDialog :: ~AsciiExportDialog(void)
{
    kapp->setOverrideCursor(Qt::WaitCursor);
}

QTextCodec* AsciiExportDialog::getCodec(void) const
{
    const QString strCodec(KGlobal::charsets()->encodingForName(m_ui.comboBoxEncoding->currentText()));
    kDebug(30502) << "Encoding:" << strCodec;

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
        kWarning(30502) << "Cannot find encoding:" << strCodec;
        // ### TODO: what parent to use?
        KMessageBox::error(0, i18n("Cannot find encoding: %1", strCodec));
        return 0;
    }

    return codec;
}

QString AsciiExportDialog::getEndOfLine(void) const
{
    QString strReturn;
    QAbstractButton* checkedButton = m_radioGroup.checkedButton();
    if (m_ui.radioEndOfLineLF == checkedButton)
        strReturn = "\n";
    else if (m_ui.radioEndOfLineCRLF == checkedButton)
        strReturn = "\r\n";
    else if (m_ui.radioEndOfLineCR == checkedButton)
        strReturn = "\r";
    else
        strReturn = "\n";

    return strReturn;
}

#include <ExportDialog.moc>
