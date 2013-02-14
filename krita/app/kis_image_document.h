/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_IMAGE_DOCUMENT_H
#define KIS_IMAGE_DOCUMENT_H

#include <abstractdocument.h>
#include <userlistable.h>

#include <kis_types.h>

#define APP_MIMETYPE "application/x-krita"

/**
 * @brief The KisImageDocument class a thin wrapper around KisImage.
 */
class KisImageDocument : public Kasten2::AbstractDocument
{
    Q_OBJECT

public:
    explicit KisImageDocument(QObject *parent = 0);
    explicit KisImageDocument(const QMap<QString, QVariant> &parameters, QObject *parent = 0);
    explicit KisImageDocument(KisImageSP image, QObject *parent = 0);
    
    virtual ~KisImageDocument();

    virtual QString typeName() const;
    virtual QString mimeType() const;
    virtual ContentFlags contentFlags() const;

    KisImageSP image() const;

private:

    bool isModified() const;

private Q_SLOTS:

    void setModified();

private:
    class Private;
    Private * const d;

};

#endif // KIS_IMAGE_DOCUMENT_H
