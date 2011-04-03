/**  This file is part of the KDE project
 * 
 *  Copyright (C) 2011 Adam Pigg <adam@piggz.co.uk>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KexiMobile_H
#define KexiMobile_H

#include <QtGui/QMainWindow>
#include <core/KexiMainWindowIface.h>

class KexiMobileToolbar;
class QHBoxLayout;
class KexiMobileWidget;

class KexiMobileMainWindow : public QMainWindow, public KexiMainWindowIface
{
Q_OBJECT
public:
    KexiMobileMainWindow();
    virtual ~KexiMobileMainWindow();

public slots:
    
    KexiWindow* openObject(KexiPart::Item* item);
    
    //KexiMainWindowIface Overrides    
    virtual void acceptProjectClosingRequested(bool& cancel);
    virtual void acceptPropertySetEditing();
    virtual KActionCollection* actionCollection() const;
    virtual void addToolBarAction(const QString& toolBarName, QAction* action);
    virtual QList< QAction* > allActions() const;
    virtual void appendWidgetToToolbar(const QString& name, QWidget* widget);
    virtual void beforeProjectClosing();
    virtual tristate closeObject(KexiPart::Item* item);
    virtual tristate closeWindow(KexiWindow* window);
    virtual KexiWindow* currentWindow() const;
    virtual tristate executeCustomActionForObject(KexiPart::Item* item, const QString& actionName);
    virtual QWidget* focusWidget() const;
    virtual tristate getNewObjectInfo(KexiPart::Item* partItem, KexiPart::Part* part, bool& allowOverwriting, const QString& messageWhenAskingForName = QString());
    virtual KXMLGUIClient* guiClient() const;
    virtual KXMLGUIFactory* guiFactory();
    virtual void highlightObject(const QString& mime, const QString& name);
    virtual bool newObject(KexiPart::Info* info, bool& openingCancelled);
    virtual KexiWindow* openObject(KexiPart::Item* item, Kexi::ViewMode viewMode, bool& openingCancelled, QMap< QString, QVariant >* staticObjectArgs = 0, QString* errorMessage = 0);
    virtual KexiWindow* openObject(const QString& mime, const QString& name, Kexi::ViewMode viewMode, bool& openingCancelled, QMap< QString, QVariant >* staticObjectArgs = 0);
    virtual void plugActionList(const QString& name, const QList< KAction* >& actionList);
    virtual tristate printItem(KexiPart::Item* item);
    virtual tristate printPreviewForItem(KexiPart::Item* item);
    virtual KexiProject* project();
    virtual void projectClosed();
    virtual void propertySetSwitched(KexiWindow* window, bool force = false, bool preservePrevSelection = true, bool sortedProperties = false, const QByteArray& propertyToSelect = QByteArray());
    virtual void registerChild(KexiWindow* window);
    virtual tristate saveObject(KexiWindow* window, const QString& messageWhenAskingForName = QString(), bool dontAsk = false);
    virtual void setWidgetVisibleInToolbar(QWidget* widget, bool visible);
    virtual tristate showPageSetupForItem(KexiPart::Item* item);
    virtual void slotObjectRenamed(const KexiPart::Item& item, const QString& oldName);
    virtual tristate switchToViewMode(KexiWindow& window, Kexi::ViewMode viewMode);
    virtual KToolBar* toolBar(const QString& name) const;
    virtual void unplugActionList(const QString& name);
    virtual void updatePropertyEditorInfoLabel(const QString& textToDisplayForNullSet = QString());
    virtual bool userMode() const;
    
    
public slots:
    void slotOpenDatabase();
    
private:
    KexiMobileWidget *m_mobile;
    KexiMobileToolbar *m_toolbar;
    QAction *m_openFileAction;
    KexiProject *m_project;
    QHBoxLayout *m_layout;
    
    void setupToolbar();
    
    KexiProject* openProject(const KUrl &url);
    bool openingAllowed(KexiPart::Item* item, Kexi::ViewMode viewMode, QString* errorMessage);
};

#endif // KexiMobile_H
