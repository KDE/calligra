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

#include <KReportRendererBase>

#include <KoDialog.h>

#include <QDomDocument>

class KoDocument;
class KoShape;
struct KoPageLayout;

class KReportPage;
class KReportPreRenderer;
class ORODocument;
class KReportDesigner;
class KReportRendererBase;

class KPropertyEditorView;
class KPropertySet;

class QGraphicsView;
class QGraphicsScene;
class QDomElement;
class QStackedWidget;
class QDomElement;
class QScrollArea;
class QStandardItemModel;
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

public Q_SLOTS:
    virtual void startPrinting(RemovePolicy removePolicy = DoNotDelete);

protected:
    ORODocument *m_reportDocument;
    KReportRendererContext m_context;
    KReportRendererBase *m_renderer;
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

public Q_SLOTS:
    void setGuiActive( bool active );
    void setScheduleManager( ScheduleManager *sm );

private Q_SLOTS:
    void slotEditReport();
    void slotViewReport();

Q_SIGNALS:
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

public Q_SLOTS:
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

Q_SIGNALS:
    void editReportDesign();

public Q_SLOTS:
    /// refresh display
    void slotRefreshView();

protected:
    void setupGui();

private Q_SLOTS:
    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();

    void slotExport();
    void slotExportFinished( int result );

private:
    ReportData *createReportData( const QDomElement &connection );
    ReportData *createReportData( const QString &type );
    void exportToOdtTable( KReportRendererContext &context );
    void exportToOdtFrames( KReportRendererContext &context );
    void exportToOds( KReportRendererContext &context );
    void exportToHtml( KReportRendererContext &context );
    void exportToXHtml( KReportRendererContext &context );

private:
    KReportPreRenderer *m_preRenderer;
    KReportRendererFactory m_factory;
    ORODocument *m_reportDocument;
    QGraphicsView *m_reportView;
    QGraphicsScene *m_reportScene;
    KReportPage *m_reportPage;
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
    explicit ReportNavigator(QWidget *parent = 0);
    void setCurrentPage( int page );
    
public Q_SLOTS:
    void setMaximum( int );
    
protected Q_SLOTS:
    void slotMaxChanged( int );
    void setButtonsEnabled();
};

class KPLATOUI_EXPORT ReportDesignDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit ReportDesignDialog( QWidget *parent = 0 );
    
    ReportDesignDialog( const QDomElement &element, const QList<ReportData*> &models, QWidget *parent = 0 );

    QDomDocument document() const;

Q_SIGNALS:
    void createReportView( ReportDesignDialog *dlg );
    void modifyReportDefinition( KUndo2Command *cmd );

public Q_SLOTS:
    void slotViewCreated( ViewBase *view );

protected Q_SLOTS:
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
    ModifyReportDefinitionCmd( ReportView *view, const QDomDocument &value, const KUndo2MagicString &name = KUndo2MagicString() );
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
    void setModified( bool on );
    QDomDocument document() const;
    void setData( const QDomDocument &doc );

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

public Q_SLOTS:
    void setReportData( const QString &tag );

Q_SIGNALS:
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
    void setData();

protected Q_SLOTS:
    void slotPropertySetChanged();
    void slotInsertAction();
    void slotItemInserted( const QString & );
    void slotSectionToggled( bool );
    void undoAllChanges();
    void slotModified();

private:
    QScrollArea *m_scrollarea;
    KReportDesigner *m_designer;
    ReportSourceEditor *m_sourceeditor;
    KPropertyEditorView *m_propertyeditor;
    QList<ReportData*> m_reportdatamodels;
    GroupSectionEditor *m_groupsectioneditor;
    QDomDocument m_original;
    QAction *m_undoaction;
};

} // namespace KPlato

#endif
