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
class KPlato_ReportDesigner;
class ReportPage;
class ReportData;
class ReportDesignerView;
class ReportSourceEditor;
class ReportNavigator;
class ReportDesignPanel;
class ReportSourceModel;

class KPLATOUI_EXPORT Report : public SplitterView 
{
    Q_OBJECT
public:
    Report( KoDocument *part, QWidget *parent);

    QDockWidget *createPropertyDocker();
    
    /// Loads the report document
    virtual bool loadXML( const QDomDocument &doc );
    /// Return the report definition document
    QDomDocument document() const;
    
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    void insertDataModel( const QString &tag, QAbstractItemModel *model );
    void setReportModels( const QMap<QString, QAbstractItemModel*> &map );
    
    static QMap<QString, QAbstractItemModel*> createDefaultReportModels( Project *project, ScheduleManager *manager, QObject *parent = 0 );
    
signals:
    void editReportDesign( Report *view );

public slots:
    /// Activate/deactivate the gui (also of subviews)
    virtual void setGuiActive( bool activate );

protected slots:
    void slotEditReportDesign();

private:
    KTabWidget *m_tab;
    ReportDesignerView *m_designer;
};

class KPLATOUI_EXPORT ReportView : public ViewBase 
{
    Q_OBJECT
public:
    ReportView( KoDocument *part, QWidget *parent, ReportDesignerView *designer = 0 );

public slots:
    void setGuiActive( bool active );
    
    void renderPage( int page );
    
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    /// refresh display
    void refresh();

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
    QDomDocument tempData() const;
    ReportData *createReportData( const QDomElement &connection );
    KoReportData* sourceData( QDomElement e );

private:
    KoReportPreRenderer *m_preRenderer;
    ORODocument *m_reportDocument;
    QScrollArea *m_scrollArea;
    ReportPage *m_reportWidget;
    ReportNavigator *m_pageSelector;
    int m_currentPage;
    int m_pageCount;
    ReportDesignerView *m_designer;
};


class KPLATOUI_EXPORT ReportDesignerView : public ViewBase 
{
    Q_OBJECT
public:
    ReportDesignerView( KoDocument *part, QWidget *parent);
    ~ReportDesignerView();

    void setupGui();

    void setPropertyEditor( QDockWidget *docker, KoProperty::EditorView *editor, ReportSourceEditor *se );
    
    KoReportDesigner *view() const { return m_designer; }
    
    QDomDocument document() const;
    
    /// Set model for the data source editor
    void setSourceModel( QAbstractItemModel *model );
    
    void setDefaultReportModels( const QMap<QString, QAbstractItemModel*> &map );
    /// Insert a new data model @p model with id @p tag
    void insertDataModel( const QString &tag, QAbstractItemModel *model );
    /// Return the data model for @p tag
    QAbstractItemModel *dataModel( const QString &tag ) const;
    
    void setReportDesigner( KoReportDesigner *designer );
    
    /// Loads the report document
    virtual bool loadXML( const QDomDocument &doc );
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    ReportData *createReportData( const QString &type );

    static ReportSourceModel *createSourceModel( QObject *parent = 0 );

signals:
    void insertItem( const QString &name );
    void modelChanged( const QAbstractItemModel *model );
    void dataChanged();
    
    void scheduleManagerChanged( ScheduleManager* );
    
public slots:
    /// Activate/deactivate the gui 
    virtual void setGuiActive( bool activate );
    void setScheduleManager( ScheduleManager *sm );
    
    void slotSourceChanged( const QModelIndex&, const QModelIndex& );
    
    void createReportData( const QString &type, KoReportData *rd );

protected slots:
    void slotInsertAction();
    void slotPropertySetChanged();
    void slotSectionEditor();
private:
    void setChecked( const QModelIndex &idx );
    QAbstractItemModel *sourceModel();

private:
    QScrollArea *m_scrollarea;
    KoReportDesigner *m_designer;
    QDockWidget *m_propertydocker;
    KoProperty::EditorView *m_propertyeditor;
    ReportSourceEditor *m_sourceeditor;
    
    QMap<QString, QAbstractItemModel*> m_modelmap;
    QDomElement m_context;
};

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
    
    ReportDesignDialog( Project *project, ScheduleManager *manager, const QDomElement &element, QWidget *parent = 0 );
    
    /// Edit the report definition in @p view
    ReportDesignDialog( Project *project, ScheduleManager *manager, Report *view, QWidget *parent = 0 );

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
    Report *m_view;
};

} // namespace KPlato

#endif
