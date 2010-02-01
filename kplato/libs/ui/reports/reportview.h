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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef KPLATOREPORTVIEW_H
#define KPLATOREPORTVIEW_H


#include "kplatoui_export.h"

#include "kptviewbase.h"
#include "kptsplitterview.h"
#include "ui_reportnavigator.h"

class KoDocument;

class KoReportData;
class ORPreRender;
class ORODocument;
class ReportViewPageSelect;
class ScriptAdaptor;
class ReportDesigner;

namespace KoProperty
{
    class EditorView;
    class Set;
}

class QScrollArea;
class QDomElement;
class QDockWidget;
class QTabWidget;
class QDomElement;

namespace KPlato
{

class KPlato_ReportDesigner;
class ReportPage;
class ReportData;
class ReportDesignerView;
class ReportSourceEditor;
class ReportNavigator;

class KPLATOUI_EXPORT Report : public SplitterView 
{
    Q_OBJECT
public:
    Report( KoDocument *part, QWidget *parent);

    QDockWidget *createPropertyDocker();
    
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    void insertDataModel( const QString &tag, QAbstractItemModel *model );
    
public slots:
    /// Activate/deactivate the gui (also of subviews)
    virtual void setGuiActive( bool activate );
    
private:
    QTabWidget *m_tab;
    ReportDesignerView *m_designer;
};

class KPLATOUI_EXPORT ReportView : public ViewBase 
{
    Q_OBJECT
public:
    ReportView( KoDocument *part, QWidget *parent, ReportDesignerView *designer = 0 );

public slots:
    void setGuiActive( bool active );
    
    /// refresh display
    void refresh();
    
    void renderPage( int page );
    
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

protected:
    void setupGui();

private slots:
    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();
    void slotPrintReport();
#if 0
    void slotRenderKSpread();
#endif
    void slotExportHTML();

private:
    QDomDocument tempData() const;
    ReportData *createReportData( const QDomElement &connection ) const;
    QAbstractItemModel *setGroupByModels( const QDomElement &element, QAbstractItemModel *model ) const;
    KoReportData* sourceData( QDomElement e );

private:
    ORPreRender *m_preRenderer;
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

    void setupGui();

    void setPropertyEditor( QDockWidget *docker, KoProperty::EditorView *editor, ReportSourceEditor *se );
    
    ReportDesigner *view() const { return m_designer; }
    
    QDomDocument document() const;
    
    void setSourceModel( QAbstractItemModel *model );
    void insertDataModel( const QString &tag, QAbstractItemModel *model );
    QAbstractItemModel *dataModel( const QString &tag ) const;
    
    void setReportDesigner( ReportDesigner *designer );
    
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

signals:
    void insertItem( const QString &name );
    void modelChanged( const QAbstractItemModel *model );
    void dataChanged();
    
public slots:
    /// Activate/deactivate the gui 
    virtual void setGuiActive( bool activate );

    void slotSourceChanged( const QModelIndex&, const QModelIndex& );
    
protected slots:
    void slotInsertAction();
    void slotPropertySetChanged();
    void slotSectionEditor();
private:
    void setChecked( const QModelIndex &idx );
    QAbstractItemModel *sourceModel();

private:
    QScrollArea *m_scrollarea;
    ReportDesigner *m_designer;
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

} // namespace KPlato

#endif
