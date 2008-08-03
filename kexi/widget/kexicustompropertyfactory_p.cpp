/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexicustompropertyfactory_p.h"

#include <qlineedit.h>
#include <kdebug.h>
#include <koproperty/property.h>
#include <kexiutils/identifier.h>

using namespace KoProperty;

KexiImagePropertyEdit::KexiImagePropertyEdit(
    Property *property, QWidget *parent)
        : PixmapEdit(property, parent)
        , m_id(0)
{
}

KexiImagePropertyEdit::~KexiImagePropertyEdit()
{
}

void KexiImagePropertyEdit::selectPixmap()
{
    QString fileName(PixmapEdit::selectPixmapFileName());
    if (fileName.isEmpty())
        return;
    KexiBLOBBuffer::Handle h(KexiBLOBBuffer::self()->insertPixmap(KUrl(fileName)));
    setValue((uint)/*! @todo unsafe*/h.id());
#if 0 //will be reenabled for new image collection
    if (!m_manager->activeForm() || !property())
        return;

    ObjectTreeItem *item = m_manager->activeForm()->objectTree()->lookup(m_manager->activeForm()->selectedWidget()->name());
    QString name = item ? item->pixmapName(property()->name()) : "";
    PixmapCollectionChooser dialog(m_manager->activeForm()->pixmapCollection(), name, topLevelWidget());
    if (dialog.exec() == QDialog::Accepted) {
        setValue(dialog.pixmap(), true);
        item->setPixmapName(property()->name(), dialog.pixmapName());
    }
#endif
}

QVariant KexiImagePropertyEdit::value() const
{
    return (uint)/*! @todo unsafe*/m_id;
}

void KexiImagePropertyEdit::setValue(const QVariant &value, bool emitChange)
{
    m_id = value.toInt();
    PixmapEdit::setValue(KexiBLOBBuffer::self()->objectForId(m_id).pixmap(), emitChange);
}

void KexiImagePropertyEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r,
                                       const QVariant &value)
{
    KexiBLOBBuffer::Handle h(KexiBLOBBuffer::self()->objectForId(value.toInt()));
    PixmapEdit::drawViewer(p, cg, r, h.pixmap());
}

//----------------------------------------------------------------

KexiIdentifierPropertyEdit::KexiIdentifierPropertyEdit(
    Property *property, QWidget *parent)
        : StringEdit(property, parent)
{
    KexiUtils::IdentifierValidator *val = new KexiUtils::IdentifierValidator(m_edit);
    m_edit->setValidator(val);
    val->setObjectName("KexiIdentifierPropertyEdit Validator");
}

KexiIdentifierPropertyEdit::~KexiIdentifierPropertyEdit()
{
}

void KexiIdentifierPropertyEdit::setValue(const QVariant &value, bool emitChange)
{
    QString string(value.toString());
    if (string.isEmpty()) {
        kWarning() << "KexiIdentifierPropertyEdit::setValue(): "
        "Value cannot be empty. This call has no effect." << endl;
        return;
    }
    QString identifier(KexiUtils::string2Identifier(string));
    if (identifier != string)
        kDebug() << QString("KexiIdentifierPropertyEdit::setValue(): "
                            "String \"%1\" converted to identifier \"%2\".").arg(string).arg(identifier) << endl;
    StringEdit::setValue(identifier, emitChange);
}

#include "kexicustompropertyfactory_p.moc"
