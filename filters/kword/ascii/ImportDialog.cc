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

#include <QTextCodec>
#include <QRadioButton>
#include <q3buttongroup.h>

#include <klocale.h>
#include <kcharsets.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <kmessagebox.h>

#include <ImportDialogUI.h>
#include <ImportDialog.h>

AsciiImportDialog :: AsciiImportDialog(QWidget* parent)
    : KDialog(parent),
      m_dialog(new ImportDialogUI(this))
{
    setButtons( Ok|Cancel);
    setCaption( i18n("KWord's Plain Text Import Filter") );
	setDefaultButton(KDialog::No);
    kapp->restoreOverrideCursor();

    QStringList encodings;
    encodings << i18nc( "Descriptive encoding name", "Recommended ( %1 )", "UTF-8" );
    encodings << i18nc( "Descriptive encoding name", "Locale ( %1 )" ,QString(QTextCodec::codecForLocale()->name() ));
    encodings += KGlobal::charsets()->descriptiveEncodingNames();
    // Add a few non-standard encodings, which might be useful for text files
    const QString description(i18nc("Descriptive encoding name","Other ( %1 )"));
    encodings << description.arg("Apple Roman"); // Apple 
    encodings << description.arg("IBM 850") << description.arg("IBM 866"); // MS DOS
    encodings << description.arg("CP 1258"); // Windows

    m_dialog->comboBoxEncoding->insertStringList(encodings);

    setMainWidget(m_dialog);
}

AsciiImportDialog :: ~AsciiImportDialog(void)
{
    kapp->setOverrideCursor(Qt::WaitCursor);
}

QTextCodec* AsciiImportDialog::getCodec(void) const
{
    const QString strCodec( KGlobal::charsets()->encodingForName( m_dialog->comboBoxEncoding->currentText() ) );
    kDebug(30502) << "Encoding: " << strCodec << endl;

    bool ok = false;
    QTextCodec* codec = QTextCodec::codecForName( strCodec.utf8() );

    // If QTextCodec has not found a valid encoding, so try with KCharsets.
    if ( codec )
    {
        ok = true;
    }
    else
    {
        codec = KGlobal::charsets()->codecForName( strCodec, ok );
    }

    // Still nothing?
    if ( !codec || !ok )
    {
        // Default: UTF-8
        kWarning(30502) << "Cannot find encoding:" << strCodec << endl;
        // ### TODO: what parent to use?
        KMessageBox::error( 0, i18n("Cannot find encoding: %1", strCodec ) );
        return 0;
    }

    return codec;
}

int AsciiImportDialog::getParagraphStrategy(void) const
{
    if (m_dialog->radioParagraphAsIs==m_dialog->buttonGroupParagraph->selected())
    {
        return 0;
    }
    if (m_dialog->radioParagraphSentence==m_dialog->buttonGroupParagraph->selected())
    {
        return 1;
    }
    else if (m_dialog->radioParagraphOldWay==m_dialog->buttonGroupParagraph->selected())
    {
        return 999;
    }
    return 0;
}

#include <ImportDialog.moc>
