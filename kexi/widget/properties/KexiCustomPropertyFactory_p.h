/* This file is part of the KDE project
   Copyright (C) 2005-2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXICUSTOMPROPFACTORY_P_H
#define KEXICUSTOMPROPFACTORY_P_H

//! @todo #include <kexiblobbuffer.h>
#include <KPropertyStringEditor>

#if 0 // todo
//! Kexi-specific image editor for property editor's item
class KexiImagePropertyEdit : public KPropertyPixmapEditor
{
    Q_OBJECT

public:
    explicit KexiImagePropertyEdit(KProperty *property,
                          QWidget *parent = 0);
    virtual ~KexiImagePropertyEdit();

    virtual QVariant value() const;
    virtual void setValue(const QVariant &value, bool emitChange = true);
    virtual void drawViewer(QPainter *p, cg, const QRect &r,
                            const QVariant &value);

public Q_SLOTS:
    virtual void selectPixmap();

protected:
    KexiBLOBBuffer::Id_t m_id;
};
#endif

/*! Identifier editor based on ordinary string editor but always keeps a valid identifier
 or empty value. It's line edit has IdentifierValidator::IdentifierValidator set, so user
 is unable to enter invalid characters. Any chages to a null value or empty string,
 have no effect.

 @todo move this to the KProperty library (when KexiUtils moves to Calligra Libs)
 */
class KexiIdentifierPropertyEditor : public KPropertyStringEditor
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue USER true)

public:
    explicit KexiIdentifierPropertyEditor(QWidget *parent = 0);
    virtual ~KexiIdentifierPropertyEditor();

public Q_SLOTS:
    /*! Reimplemented: sets \a value but it is converted to identifier
     using KDb::stringToIdentifier().
     If \a value is empty string, this method has no effect. */
    virtual void setValue(const QString &value);
};

#endif
