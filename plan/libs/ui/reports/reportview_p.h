/* This file is part of the KDE project
  Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATOREPORTVIEW_P_H
#define KPLATOREPORTVIEW_P_H


#include "kplatoui_export.h"

#include "KoReportDesigner.h"

#include <QSplitter>
#include <QStandardItemModel>
#include <QMap>
#include <qaction.h>

class KoReportData;
class ORPreRender;
class ORODocument;
class ReportViewPageSelect;
class RecordNavigator;
class ScriptAdaptor;
class ReportDesigner;

class KToolBar;

class QScrollArea;
class QDomElement;
class QActionGroup;

namespace KPlato
{


class ReportDesignPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ReportDesignPanel( QWidget *parent = 0 );

    ReportDesignPanel( Project *project, ScheduleManager *manager, const QDomElement &element, const QMap<QString, QAbstractItemModel*> &models, QWidget *parent );
    
    QDomDocument document() const;
    
    KoReportDesigner *m_designer;
    KoProperty::EditorView *m_propertyeditor;
    QMap<QString, QAbstractItemModel*> m_modelmap;
    ReportSourceEditor *m_sourceeditor;
    bool m_modified;
    QActionGroup *m_actionGroup;

    QStandardItemModel *createSourceModel( QObject *parent = 0 ) const;

signals:
    void insertItem( const QString &name );

public slots:
    void slotPropertySetChanged();
    void slotInsertAction();
    
    void setReportData( const QString &tag );
    void createReportData( const QString &type, ReportData *rd );
    
    void setModified() { m_modified = true; }
    void slotItemInserted(const QString &item);
    
protected:
    ReportData *createReportData( const QString &type );
    void populateToolbar( KToolBar *tb );

private:
    
};


} // namespace KPlato

#endif
