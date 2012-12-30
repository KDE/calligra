/* This file is part of the KDE project
   Copyright (C) 2005-2012 Jarosław Staniek <staniek@kde.org>

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

#include "kexicharencodingcombobox.h"

#include <QTextCodec>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kcharsets.h>

class KexiCharacterEncodingComboBox::Private {
public:
    Private();

    QHash<QString, QString> encodingDescriptionForName;
    bool defaultEncodingAdded;
};

KexiCharacterEncodingComboBox::KexiCharacterEncodingComboBox(
    QWidget* parent, const QString& selectedEncoding)
  : KComboBox(parent), d(new Private)
{
    QString defaultEncoding(QString::fromLatin1(KGlobal::locale()->encoding()));
    QString defaultEncodingDescriptiveName;

    QString _selectedEncoding = selectedEncoding;
    if (_selectedEncoding.isEmpty())
        _selectedEncoding = QString::fromLatin1(KGlobal::locale()->encoding());

    QStringList descEncodings(KGlobal::charsets()->descriptiveEncodingNames());

    uint id = 0;
    foreach(const QString &descEncoding, descEncodings) {
        bool found = false;
        QString name(KGlobal::charsets()->encodingForName(descEncoding));
        QTextCodec *codecForEnc = KGlobal::charsets()->codecForName(name, found);
        if (found) {
            addItem(descEncoding);
            if (codecForEnc->name() == defaultEncoding || name == defaultEncoding) {
                defaultEncodingDescriptiveName = descEncoding;
                //remember, do not add, will be prepended later
            } else {
                d->encodingDescriptionForName.insert(name, descEncoding);
            }
            if (codecForEnc->name() == _selectedEncoding || name == _selectedEncoding) {
                setCurrentIndex(id);
            }
            id++;
        }
    }

    //prepend default encoding, if present
    if (!defaultEncodingDescriptiveName.isEmpty()) {
        d->defaultEncodingAdded = true;
        QString desc = i18nc("Text encoding: Default", "Default: %1",
                             defaultEncodingDescriptiveName);
        insertItem(0, desc);
        if (_selectedEncoding == defaultEncoding) {
            setCurrentIndex(0);
        }
        d->encodingDescriptionForName.insert(defaultEncoding, desc);
    }
}

KexiCharacterEncodingComboBox::~KexiCharacterEncodingComboBox()
{
    delete d;
}

QString KexiCharacterEncodingComboBox::selectedEncoding() const
{
    if (defaultEncodingSelected()) {
        return QString::fromLatin1(KGlobal::locale()->encoding());
    } else {
        return KGlobal::charsets()->encodingForName(currentText());
    }
}

void KexiCharacterEncodingComboBox::setSelectedEncoding(const QString& encodingName)
{
    QString desc = d->encodingDescriptionForName[encodingName];
    if (desc.isEmpty()) {
        kWarning() << "KexiCharacterEncodingComboBox::setSelectedEncoding(): "
        "no such encoding \"" << encodingName << "\"";
        return;
    }
    setEditText(desc);
}

bool KexiCharacterEncodingComboBox::defaultEncodingSelected() const
{
    return d->defaultEncodingAdded && 0 == currentIndex();
}

void KexiCharacterEncodingComboBox::selectDefaultEncoding()
{
    if (d->defaultEncodingAdded)
        setCurrentIndex(0);
}

KexiCharacterEncodingComboBox::Private::Private()
  : defaultEncodingAdded(false)
{
}
