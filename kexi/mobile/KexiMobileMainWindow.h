#ifndef KexiMobile_H
#define KexiMobile_H

#include <QtGui/QMainWindow>
#include <core/KexiMainWindowIface.h>

class KexiMobileToolbar;
class QHBoxLayout;
class KexiMobileWidget;

class KexiMobileMainWindow : public QWidget, public KexiMainWindowIface
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
    KexiProject *m_project;
    QHBoxLayout *m_layout;
    
    KexiProject* openProject(const KUrl &url);
    bool openingAllowed(KexiPart::Item* item, Kexi::ViewMode viewMode, QString* errorMessage);
};

#endif // KexiMobile_H
