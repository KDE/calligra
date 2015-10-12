/*
   This file is part of the KDE project
   Copyright 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include "ImportDialog.h"

#include <QTextCodec>
#include <QRadioButton>
#include <QApplication>

#include <klocalizedstring.h>
#include <kcharsets.h>
#include <kcombobox.h>
#include <kmessagebox.h>

#include "AsciiImportDebug.h"
#include <ui_ImportDialogUI.h>

AsciiImportDialog::AsciiImportDialog(const QString &encoding, QWidget* parent)
: KoDialog(parent)
{
    QWidget *widget = new QWidget();
    m_ui.setupUi(widget);
    m_radioGroup.addButton(m_ui.radioParagraphAsIs);
    m_radioGroup.addButton(m_ui.radioParagraphSentence);
    m_radioGroup.addButton(m_ui.radioParagraphEmptyLine);

    setButtons(Ok | Cancel);
    setCaption(i18n("Words's Plain Text Import Filter"));
    QApplication::restoreOverrideCursor();

    QStringList encodings;
    encodings << i18nc("Descriptive encoding name", "Recommended ( %1 )", "UTF-8");
    encodings << i18nc("Descriptive encoding name", "Locale ( %1 )" , QString(QTextCodec::codecForLocale()->name()));
    encodings += KCharsets::charsets()->descriptiveEncodingNames();
    // Add a few non-standard encodings, which might be useful for text files
    const QString description(i18nc("Descriptive encoding name", "Other ( %1 )"));
    encodings << description.arg("Apple Roman"); // Apple
    encodings << description.arg("IBM 850") << description.arg("IBM 866"); // MS DOS
    encodings << description.arg("CP 1258"); // Windows

    m_ui.comboBoxEncoding->addItems(encodings);

    // The Recommended  and Local  is the fallback don't use when selecting the correct encoding
    for (int i = 2; i < m_ui.comboBoxEncoding->count(); ++i) {
        // the string in strCodec and the one given back by codec->name() are not always the same so 
        // we check both
        const QString strCodec(KCharsets::charsets()->encodingForName(m_ui.comboBoxEncoding->itemText(i)));
        QTextCodec* codec = QTextCodec::codecForName(strCodec.toUtf8());
        if (!codec) {
            bool ok = false;
            codec = KCharsets::charsets()->codecForName(strCodec, ok);
            if (!ok) {
                codec = 0;
            }
        }
        //debugAsciiImport << "Data:" << strCodec << (codec ? codec->name(): "");
        if ((codec && encoding == codec->name()) || strCodec == encoding) {
            m_ui.comboBoxEncoding->setCurrentIndex(i);
            break;
        }
    }
    setMainWidget(widget);
}

AsciiImportDialog::~AsciiImportDialog()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

QTextCodec* AsciiImportDialog::getCodec() const
{
    const QString strCodec(KCharsets::charsets()->encodingForName(m_ui.comboBoxEncoding->currentText()));
    debugAsciiImport << "Encoding:" << strCodec << m_ui.comboBoxEncoding->currentText();

    bool ok = false;
    QTextCodec* codec = QTextCodec::codecForName(strCodec.toUtf8());

    // If QTextCodec has not found a valid encoding, so try with KCharsets.
    if (codec) {
        ok = true;
    } else {
        codec = KCharsets::charsets()->codecForName(strCodec, ok);
    }

    // Still nothing?
    if (!codec || !ok) {
        // Default: UTF-8
        warnAsciiImport << "Cannot find encoding:" << strCodec;
        // ### TODO: what parent to use?
        KMessageBox::error(0, i18n("Cannot find encoding: %1", strCodec));
        return 0;
    }

    return codec;
}

int AsciiImportDialog::getParagraphStrategy() const
{
    QAbstractButton* checkedButton = m_radioGroup.checkedButton();
    if (m_ui.radioParagraphAsIs == checkedButton) {
        return 0;
    }
    else if (m_ui.radioParagraphSentence == checkedButton) {
        return 1;
    }
    else if (m_ui.radioParagraphEmptyLine == checkedButton) {
        return 2;
    }
    return 0;
}
