/* This file is part of the KDE project
   Copyright (C) 2010-2011 Christoph Goerlich <chgoerlich@gmx.de>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef GOOGLEDOCUMENT_H
#define GOOGLEDOCUMENT_H


#include <QString>


class GoogleDocument
{
public:
    QString etag() const { return m_etag; }
    void setEtag(const QString etag) { m_etag = etag; }
    
    QString id() const { return m_id; }
    void setId(const QString id) { m_id = id; }
    
    QString published() const { return m_published; }
    void setPublished(const QString published) { m_published = published; }
    
    QString updated() const { return m_updated; }
    void setUpdated(const QString updated) { m_updated = updated; }
    
    QString edited() const { return m_edited; }
    void setEdited(const QString edited) { m_edited = edited; }
    
    QString docType() const { return m_docType; }
    void setDocType(const QString docType) { m_docType = docType; }
    
    QString title() const { return m_title; }
    void setTitle(const QString title) { m_title = title; }
    
    QString exportUrl() const { return m_exportUrl; }
    void setExportUrl(const QString exportUrl) { m_exportUrl = exportUrl; }
    
    QString editUrl() const { return m_editUrl; }
    void setEditUrl(const QString editUrl) { m_editUrl = editUrl; }
    
    QString editMediaUrl() const { return m_editMediaUrl; }
    void setEditMediaUrl(const QString editMediaUrl) { m_editMediaUrl = editMediaUrl; }
    
    QString worksheetsUrl() const { return m_worksheetsUrl; }
    void setWorksheetsUrl(const QString worksheetsUrl) { m_worksheetsUrl = worksheetsUrl; }
    
    QString tablesUrl() const { return m_tablesUrl; }
    void setTablesUrl(const QString tablesUrl) { m_tablesUrl = tablesUrl; }
    
    QString author() const { return m_author; }
    void setAuthor(const QString author) { m_author = author; }
    
    QString lastModifiedBy() const { return m_lastModifiedBy; }
    void setLastModifiedBy(const QString lastModifiedBy) { m_lastModifiedBy = lastModifiedBy; }
    
private:
    QString m_etag;
    QString m_id;
    QString m_published;
    QString m_updated;
    QString m_edited;
    QString m_docType;
    QString m_title;
    QString m_exportUrl;
    //QString m_alternateUrl;
    //QString m_resumableEditMediaUrl;
    //QString m_selfUrl;
    QString m_editUrl;
    QString m_editMediaUrl;
    //Spreadsheet-specific
    QString m_worksheetsUrl;
    QString m_tablesUrl;
    //for ?Documents?: resumableEditMediaUri;
    QString m_author;
    QString m_lastModifiedBy;
    //QString m_aclUrl;
    //QString m_revisionsUrl;
};

#endif // GOOGLEDOCUMENT_H
