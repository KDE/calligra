#ifndef FAKE_KXMLGUICLIENT_HH
#define FAKE_KXMLGUICLIENT_HH

#include <QByteArray>
#include <QObject>
#include <QDomDocument>
#include <QDebug>
#include <kaction.h>
#include <kactioncollection.h>
#include <kcomponentdata.h>

class KXMLGUIFactory;

class KXMLGUIBuilder
{
};

class KXMLGUIClient
{
public:
    KXMLGUIClient(KXMLGUIClient *parent = 0) : m_factory(0), m_builder(0), m_actionCollection(new KActionCollection()) {}
    virtual ~KXMLGUIClient() { delete m_actionCollection; }

    static KAction* action(const QByteArray &name) { return 0; }

    QAction* action( const char* name ) const { return m_actionCollection->action(name); }
    virtual QAction *action( const QDomElement &element ) const { return 0; }
    virtual KActionCollection* actionCollection() const { return m_actionCollection; }
    virtual KComponentData componentData() const { return KGlobal::mainComponent(); }
    virtual QDomDocument domDocument() const { return QDomDocument(); }
    virtual QString xmlFile() const { return QString(); }
    virtual QString localXMLFile() const { return QString(); }
    void setXMLGUIBuildDocument( const QDomDocument &doc ) {}
    QDomDocument xmlguiBuildDocument() const { return QDomDocument(); }
    void setFactory( KXMLGUIFactory *factory ) { m_factory = factory; }
    KXMLGUIFactory* factory() const { return m_factory; }
    KXMLGUIClient *parentClient() const { return 0; }
    void insertChildClient( KXMLGUIClient *child ) {}
    void removeChildClient( KXMLGUIClient *child ) {}
    QList<KXMLGUIClient*> childClients() { return QList<KXMLGUIClient*>(); }
    void setClientBuilder( KXMLGUIBuilder *builder ) { m_builder = builder; }
    KXMLGUIBuilder *clientBuilder() const { return m_builder; }
    void reloadXML() {}
    void plugActionList( const QString &name, const QList<QAction*> &actionList ) {}
    void unplugActionList( const QString &name ) {}
    static QString findMostRecentXMLFile( const QStringList &files, QString &doc ) { return QString(); }
    void addStateActionEnabled(const QString& state, const QString& action) {}
    void addStateActionDisabled(const QString& state, const QString& action) {}
    enum ReverseStateChange { StateNoReverse, StateReverse };
    struct StateChange { QStringList actionsToEnable; QStringList actionsToDisable; };
    StateChange getActionsToChangeForState(const QString& state) { return StateChange(); }
    void beginXMLPlug( QWidget * ) {}
    void endXMLPlug() {}
    void prepareXMLUnplug( QWidget * ) {}
    void replaceXMLFile( const QString& xmlfile, const QString& localxmlfile, bool merge = false ) {}

protected:
  virtual void setComponentData(const KComponentData &componentData) {}
  virtual void setXMLFile( const QString& file, bool merge = false, bool setXMLDoc = true ) {}
  void loadStandardsXmlFile() {}
  virtual void setLocalXMLFile( const QString &file ) {}
  virtual void setXML( const QString &document, bool merge = false ) {}
  virtual void setDOMDocument( const QDomDocument &document, bool merge = false ) {}
  virtual void stateChanged(const QString &newstate, ReverseStateChange reverse = StateNoReverse) {}

private:
    KXMLGUIFactory *m_factory;
    KXMLGUIBuilder *m_builder;
    KActionCollection *m_actionCollection;
};

#endif
