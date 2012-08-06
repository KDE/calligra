/*
 * KPlato Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010, 2011 by Dag Andersen <danders@get2net.dk>
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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPLATOREPORTVIEW_H
#define KPLATOREPORTVIEW_H


#include "kplatoui_export.h"

#include "kptviewbase.h"
#include "kptsplitterview.h"
#include "kptcommand.h"

#include "ui_reportnavigator.h"

#include <KoReportRendererBase.h>

class KoDocument;
class KoShape;
class KoPageLayout;

class KoReportPage;
class KoReportData;
class KoReportPreRenderer;
class ORODocument;
class ReportViewPageSelect;
class ScriptAdaptor;
class KoReportDesigner;
class KoReportRendererBase;

namespace KoProperty
{
    class EditorView;
    class Set;
}

class KTabWidget;

class QGraphicsView;
class QGraphicsScene;
class QDomElement;
class QDockWidget;
class QStackedWidget;
class QDomElement;
class QScrollArea;
class QStandardItemModel;
class QActionGroup;
class KUndo2Command;

namespace KPlato
{

class Project;
class ReportView;
class ReportWidget;
class ReportDesigner;
class ReportData;
class ReportSourceEditor;
class ReportNavigator;
class ReportDesignPanel;
class ReportSourceModel;
class GroupSectionEditor;

class ReportPrintingDialog : public KoPrintingDialog
{
    Q_OBJECT
public:
    ReportPrintingDialog( ViewBase *view, ORODocument *reportDocument );
    ~ReportPrintingDialog();
    
    void printPage( int page, QPainter &painter );

    int documentLastPage() const;

    virtual QList<QWidget*> createOptionWidgets() const { return QList<QWidget*>(); }
    virtual QList<KoShape*> shapesOnPage(int) { return QList<KoShape*>(); }

    virtual QAbstractPrintDialog::PrintDialogOptions printDialogOptions() const;

public slots:
    virtual void startPrinting(RemovePolicy removePolicy = DoNotDelete);

protected:
    ORODocument *m_reportDocument;
    KoReportRendererContext m_context;
    KoReportRendererBase *m_renderer;
};

//-------------------
class KPLATOUI_EXPORT ReportView : public ViewBase
{
    Q_OBJECT
public:
    ReportView(KoPart *part, KoDocument *doc, QWidget *parent);

    void setProject( Project *project );

    KoPrintJob *createPrintJob();

    /// Load the design document @p doc
    bool loadXML( const QDomDocument &doc );
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    ReportWidget *reportWidget() const;
    ReportDesigner *reportDesigner() const;

    QDomDocument document() const;
    QList<ReportData*> reportDataModels() const;

public slots:
    void setGuiActive( bool active );
    void setScheduleManager( ScheduleManager *sm );

private slots:
    void slotEditReport();
    void slotViewReport();

signals:
    void editReportDesign( ReportWidget* );

private:
    QStackedWidget *m_stack;
};
//-------------------
class KPLATOUI_EXPORT ReportWidget : public ViewBase
{
    Q_OBJECT
public:
    ReportWidget(KoPart *part, KoDocument *doc, QWidget *parent);

public slots:
    void setGuiActive( bool active );
    
    void renderPage( int page );
    
    /// Return true if document is null
    bool documentIsNull() const;
    /// Load the design document @p doc
    bool loadXML( const QDomDocument &doc );
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    KoPrintJob *createPrintJob();

    /// Return the page layout used for printing this view
    KoPageLayout pageLayout() const;

    void setReportDataModels( const QList<ReportData*> &models );
    QList<ReportData*> reportDataModels() const { return m_reportdatamodels; }

signals:
    void editReportDesign();

public slots:
    /// refresh display
    void slotRefreshView();

protected:
    void setupGui();

private slots:
    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();

    void slotExport();
    void slotExportFinished( int result );

private:
    ReportData *createReportData( const QDomElement &connection );
    ReportData *createReportData( const QString &type );
    void exportToOdtTable( KoReportRendererContext &context );
    void exportToOdtFrames( KoReportRendererContext &context );
    void exportToOds( KoReportRendererContext &context );
    void exportToHtml( KoReportRendererContext &context );
    void exportToXHtml( KoReportRendererContext &context );

private:
    KoReportPreRenderer *m_preRenderer;
    KoReportRendererFactory m_factory;
    ORODocument *m_reportDocument;
    QGraphicsView *m_reportView;
    QGraphicsScene *m_reportScene;
    KoReportPage *m_reportPage;
    ReportNavigator *m_pageSelector;
    int m_currentPage;
    int m_pageCount;
    QList<ReportData*> m_reportdatamodels;
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
    
    ReportDesignDialog( const QDomElement &element, const QList<ReportData*> &models, QWidget *parent = 0 );

    QDomDocument document() const;

signals:
    void createReportView( ReportDesignDialog *dlg );
    void modifyReportDefinition( KUndo2Command *cmd );

public slots:
    void slotViewCreated( ViewBase *view );

protected slots:
    void slotSaveToFile();
    void slotSaveToView();
    virtual void slotButtonClicked(int button);
    void closeEvent ( QCloseEvent * e );

protected:
    void saveToView();

private:
    ReportDesignPanel *m_panel;
    ReportView *m_view;
};

//-------------------
class  KPLATOUI_EXPORT ModifyReportDefinitionCmd : public NamedCommand
{
public:
    ModifyReportDefinitionCmd( ReportView *view, const QDomDocument &value, const QString &name = QString() );
    void execute();
    void unexecute();

private:
    ReportView *m_view;
    QDomDocument m_newvalue;
    QDomDocument m_oldvalue;
};

//-------------------------
class  KPLATOUI_EXPORT ReportDesigner : public ViewBase
{
    Q_OBJECT
public:
    ReportDesigner(KoPart *part, KoDocument *doc, QWidget *parent = 0);

    bool isModified() const;
    QDomDocument document() const;
    void setData( const QDomDocument doc );

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

public slots:
    void setReportData( const QString &tag );

signals:
    void viewReport();
    void resetButtonState( bool );
    void raiseClicked();
    void lowerClicked();
    void cutActivated();
    void copyActivated();
    void pasteActivated();
    void deleteActivated();

protected:
    void setupGui();
    void createDockers();
    QStandardItemModel *createSourceModel( QObject *parent ) const;

protected slots:
    void slotPropertySetChanged();
    void slotInsertAction();
    void slotItemInserted( const QString & );
    void slotSectionToggled( bool );
private:
    QScrollArea *m_scrollarea;
    KoReportDesigner *m_designer;
    ReportSourceEditor *m_sourceeditor;
    KoProperty::EditorView *m_propertyeditor;
    QList<ReportData*> m_reportdatamodels;
    GroupSectionEditor *m_groupsectioneditor;
};

} // namespace KPlato

#endif
