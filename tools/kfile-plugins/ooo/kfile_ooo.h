/* This file is part of the KDE project
 * Copyright (C) 2003 Pierre Souchay <pierre@souchay.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef __KFILE_OOO_H__
#define __KFILE_OOO_H__

#include <kfilemetainfo.h>
#include <qiodevice.h>
#include <qdom.h>
#include <karchive.h>
class QStringList;
class QDomNode;
class QDomElement;

class KOfficePlugin: public KFilePlugin
{
    Q_OBJECT

public:
    /**
     * Constructor */
    KOfficePlugin( QObject *parent, const QStringList& args );
    /**
     * Read informations from files and store info in KFileMetaInfo.
     * We currently only parse meta.xml in OOo files.
     * @param info informations to modify about current file
     * @param what What kind of parsing is needed ?
     */
    virtual bool readInfo( KFileMetaInfo& info, uint what);
    /**
     * Write modified informations in meta.xml
     * @param info Informations modified
     */
    virtual bool writeInfo( const KFileMetaInfo& info) const;
    /**
     * We need a validator, for the langage
     */
    virtual QValidator* createValidator( const QString& mimetype,
                                         const QString &group,
					 const QString &key,
					 QObject* parent,
					 const char* name) const;
private:
    bool writeTextNode(QDomDocument & doc,
		       QDomNode & parentNode,
		       const QString  &nodeName,
		       const QString  &value) const;
    KFileMimeTypeInfo::GroupInfo* userdefined;
    void addAttributeInfo(const QDomElement & elem, KFileMetaInfoGroup & group,
		          const QString &attributeName);
    QIODevice* getData(KArchive &m_zip) const;
    bool writeMetaData(const QString & path, const QDomDocument &doc) const;
    QDomDocument getMetaDocument(const QString &path) const;
    QDomNode getBaseNode(const QDomDocument &doc) const;
    void makeMimeTypeInfo(const QString& mimeType);
    QString stringFromNode(const QDomNode &node, const QString &name);
    void getEditingTime(KFileMetaInfoGroup group1, const char *, QString & txt);
    void getDateTime(KFileMetaInfoGroup group1, const char *, QString & txt);
};

#endif
