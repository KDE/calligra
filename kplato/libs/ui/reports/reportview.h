/*
 * KPlato Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010 by Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef KPLATOREPORTVIEW_H
#define KPLATOREPORTVIEW_H


#include "kplatoui_export.h"

#include "kptviewbase.h"
#include "kptsplitterview.h"
#include "ui_reportnavigator.h"

class KoDocument;

class KoReportData;
class KoReportPreRenderer;
class ORODocument;
class ReportViewPageSelect;
class ScriptAdaptor;
class KoReportDesigner;

namespace KoProperty
{
    class EditorView;
    class Set;
}

class KTabWidget;

class QScrollArea;
class QDomElement;
class QDockWidget;
class QDomElement;

namespace KPlato
{

class Project;
class ReportView;
class ReportPage;
class ReportData;
class ReportSourceEditor;
class ReportNavigator;
class ReportDesignPanel;
class ReportSourceModel;


class KPLATOUI_EXPORT ReportView : public ViewBase 
{
    Q_OBJECT
public:
    ReportView( KoDocument *part, QWidget *parent );

public slots:
    void setGuiActive( bool active );
    
    void renderPage( int page );
    
    /// Return the design document
    QDomDocument document() const;
    /// Load the design document @p doc
    bool loadXML( const QDomDocument &doc );
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    /// refresh display
    void refresh();

    QMap<QString, QAbstractItemModel*> reportModels() const;
    void setReportModels( const QMap<QString, QAbstractItemModel*> &map );

    QMap<QString, QAbstractItemModel*> createReportModels( Project *project, ScheduleManager *manager, QObject *parent = 0 ) const;

signals:
    void editReportDesign( ReportView *view );

protected:
    void setupGui();

private slots:
    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();
    void slotEditReport();
    void slotPrintReport();
#if 0
    void slotRenderKSpread();
#endif
    void slotExportHTML();

private:
    ReportData *createReportData( const QDomElement &connection );

private:
    KoReportPreRenderer *m_preRenderer;
    ORODocument *m_reportDocument;
    QScrollArea *m_scrollArea;
    ReportPage *m_reportWidget;
    ReportNavigator *m_pageSelector;
    int m_currentPage;
    int m_pageCount;
    QMap<QString, QAbstractItemModel*> m_modelmap;
    QDomDocument m_design;
};

//-----------------
class KPLATOUI_EXPORT ReportNavigator : public QWidget, public Ui::ReportNavigator
{
    Q_OBJECT
public:
    ReportNavigator( QWidget *parent = 0 );
    void setCurrentPage( int page );
    
public slots:
    void setMaximum( int );
    
protected slots:
    void slotMaxChanged( int );
    void setButtonsEnabled();
};

class KPLATOUI_EXPORT ReportDesignDialog : public KDialog
{
    Q_OBJECT
public:
    explicit ReportDesignDialog( QWidget *parent = 0 );
    
    ReportDesignDialog( Project *project, ScheduleManager *manager, const QDomElement &element, const QMap<QString, QAbstractItemModel*> &models, QWidget *parent = 0 );
    
    /// Edit the report definition in @p view
    ReportDesignDialog( Project *project, ScheduleManager *manager, ReportView *view, QWidget *parent = 0 );

    QDomDocument document() const;

signals:
    void createReportView( ReportDesignDialog *dlg );

public slots:
    void slotViewCreated( ViewBase *view );

protected slots:
    void slotSaveToFile();
    void slotSaveToView();
    virtual void slotButtonClicked(int button);

private:
    ReportDesignPanel *m_panel;
    ReportView *m_view;
};

} // namespace KPlato

#endif
