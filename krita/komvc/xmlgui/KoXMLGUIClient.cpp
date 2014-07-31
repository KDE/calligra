/* This file is part of the KDE libraries
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000 Kurt Granroth <granroth@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoXMLGUIClient.h"
#include "KoXMLGUIVersionHandler_p.h"
#include "KoXMLGUIFactory.h"
#include "KoXMLGUIBuilder.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtCore/QTextIStream>
#include <QtCore/QRegExp>
#include <QtCore/QPointer>

#include <kcomponentdata.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kauthorized.h>

#include "kaction.h"
#include "kactioncollection.h"

#include <assert.h>

class KoXMLGUIClientPrivate
{
public:
  KoXMLGUIClientPrivate() 
    : m_componentData(KGlobal::mainComponent()),
      m_actionCollection(0),
      m_parent(0L),
      m_builder(0L)
  {
  }
  ~KoXMLGUIClientPrivate()
  {
  }

  bool mergeXML( QDomElement &base, QDomElement &additive,
                 KActionCollection *actionCollection );
  bool isEmptyContainer(const QDomElement& base,
                        KActionCollection *actionCollection) const;

  QDomElement findMatchingElement( const QDomElement &base,
                                   const QDomElement &additive );

  KComponentData m_componentData;

  QDomDocument m_doc;
  KActionCollection *m_actionCollection;
  QDomDocument m_buildDocument;
  QPointer<KoXMLGUIFactory> m_factory;
  KoXMLGUIClient *m_parent;
  //QPtrList<KoXMLGUIClient> m_supers;
  QList<KoXMLGUIClient*> m_children;
  KoXMLGUIBuilder *m_builder;
  QString m_xmlFile;
  QString m_localXMLFile;

  // Actions to enable/disable on a state change
  QMap<QString,KoXMLGUIClient::StateChange> m_actionsStateMap;
};


KoXMLGUIClient::KoXMLGUIClient()
    : d( new KoXMLGUIClientPrivate )
{
}

KoXMLGUIClient::KoXMLGUIClient( KoXMLGUIClient *parent )
    : d( new KoXMLGUIClientPrivate )
{
  parent->insertChildClient( this );
}

KoXMLGUIClient::~KoXMLGUIClient()
{
  if ( d->m_parent ) {
    d->m_parent->removeChildClient( this );
  }

  if ( d->m_factory ) {
      kWarning(240) << this << "deleted without having been removed from the factory first. This will leak standalone popupmenus and could lead to crashes.";
    d->m_factory->forgetClient(this);
  }

  foreach (KoXMLGUIClient* client, d->m_children) {
      if (d->m_factory)
          d->m_factory->forgetClient(client);
      assert( client->d->m_parent == this );
      client->d->m_parent = 0;
  }

  delete d->m_actionCollection;
  delete d;
}

QAction *KoXMLGUIClient::action( const char *name ) const
{
  QAction* act = actionCollection()->action( name );
  if ( !act ) {
    foreach (KoXMLGUIClient* client, d->m_children) {
      act = client->actionCollection()->action( name );
      if ( act )
        break;
    }
  }
  return act;
}

KActionCollection *KoXMLGUIClient::actionCollection() const
{
  if ( !d->m_actionCollection )
  {
      d->m_actionCollection = new KActionCollection(0, d->m_componentData);
      d->m_actionCollection->setObjectName( "KoXMLGUIClient-KActionCollection" );
  }
  return d->m_actionCollection;
}

QAction *KoXMLGUIClient::action( const QDomElement &element ) const
{
  static const QString &attrName = KGlobal::staticQString( "name" );
  return actionCollection()->action( qPrintable(element.attribute( attrName )) );
}

KComponentData KoXMLGUIClient::componentData() const
{
  return d->m_componentData;
}

QDomDocument KoXMLGUIClient::domDocument() const
{
  return d->m_doc;
}

QString KoXMLGUIClient::xmlFile() const
{
  return d->m_xmlFile;
}

QString KoXMLGUIClient::localXMLFile() const
{
  if ( !d->m_localXMLFile.isEmpty() )
    return d->m_localXMLFile;

  if ( !QDir::isRelativePath(d->m_xmlFile) )
      return QString(); // can't save anything here

    if (d->m_xmlFile.isEmpty()) // setXMLFile not called at all, can't save. Use case: ToolBarHandler
        return QString();

  return KStandardDirs::locateLocal( "data", componentData().componentName() + '/' + d->m_xmlFile );
}


void KoXMLGUIClient::reloadXML()
{
    // TODO: this method can't be used for the KoXmlGuiWindow, since it doesn't merge in ui_standards.rc!
    //   -> KDE5: load ui_standards_rc in setXMLFile using a flag, and remember that flag?
    //            and then KEditToolBar can use reloadXML.
    QString file( xmlFile() );
    if ( !file.isEmpty() )
        setXMLFile( file );
}

void KoXMLGUIClient::setComponentData(const KComponentData &componentData)
{
  d->m_componentData = componentData;
  actionCollection()->setComponentData( componentData );
  if ( d->m_builder )
    d->m_builder->setBuilderClient( this );
}

void KoXMLGUIClient::loadStandardsXmlFile()
{
    const QString file = KStandardDirs::locate("config", "ui/ui_standards.rc", componentData());
    if (file.isEmpty()) {
        kWarning() << "ui/ui_standards.rc not found in" << componentData().dirs()->resourceDirs("config");
    } else {
        const QString doc = KoXMLGUIFactory::readConfigFile( file );
        setXML( doc );
    }
}

void KoXMLGUIClient::setXMLFile( const QString& _file, bool merge, bool setXMLDoc )
{
  // store our xml file name
  if ( !_file.isNull() )
    d->m_xmlFile = _file;

  if ( !setXMLDoc )
    return;

  QString file = _file;
  QStringList allFiles;
  if ( !QDir::isRelativePath( file ) ) {
    allFiles.append( file );
  } else {
    const QString filter = componentData().componentName() + '/' + _file;
    allFiles = componentData().dirs()->findAllResources("data", filter) +
                 componentData().dirs()->findAllResources("data", _file);
  }
  if ( allFiles.isEmpty() && !_file.isEmpty() ) {
    // if a non-empty file gets passed and we can't find it,
    // inform the developer using some debug output
    kWarning() << "cannot find .rc file" << _file << "for component" << componentData().componentName();
  }

  // make sure to merge the settings from any file specified by setLocalXMLFile()
  if ( !d->m_localXMLFile.isEmpty() && !file.endsWith("ui_standards.rc") ) {
    const bool exists = QDir::isRelativePath(d->m_localXMLFile) || QFile::exists(d->m_localXMLFile);
    if (exists && !allFiles.contains(d->m_localXMLFile))
      allFiles.prepend( d->m_localXMLFile );
  }

  QString doc;
  if ( !allFiles.isEmpty() )
    file = findMostRecentXMLFile(allFiles, doc);

  // Always call setXML, even on error, so that we don't keep all ui_standards.rc menus.
  setXML( doc, merge );
}

void KoXMLGUIClient::setLocalXMLFile( const QString &file )
{
    d->m_localXMLFile = file;
}

void KoXMLGUIClient::replaceXMLFile( const QString& xmlfile, const QString& localxmlfile, bool merge )
{
  if ( !QDir::isAbsolutePath ( xmlfile ) ) {
    kWarning() << "xml file" << xmlfile << "is not an absolute path";
  }

  setLocalXMLFile ( localxmlfile );
  setXMLFile ( xmlfile, merge );
}

void KoXMLGUIClient::setXML( const QString &document, bool merge )
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    // QDomDocument raises a parse error on empty document, but we accept no app-specific document,
    // in which case you only get ui_standards.rc layout.
    bool result = document.isEmpty() || doc.setContent(document, &errorMsg, &errorLine, &errorColumn);
    if ( result ) {
        setDOMDocument( doc, merge );
    } else {
#ifdef NDEBUG
        kError(240) << "Error parsing XML document:" << errorMsg << "at line" << errorLine << "column" << errorColumn;
        setDOMDocument(QDomDocument(), merge); // otherwise empty menus from ui_standards.rc stay around
#else
        kFatal() << "Error parsing XML document:" << errorMsg << "at line" << errorLine << "column" << errorColumn;
#endif
    }
}

void KoXMLGUIClient::setDOMDocument( const QDomDocument &document, bool merge )
{
  if ( merge && !d->m_doc.isNull() )
  {
    QDomElement base = d->m_doc.documentElement();

    QDomElement e = document.documentElement();

    // merge our original (global) xml with our new one
    d->mergeXML(base, e, actionCollection());

    // reassign our pointer as mergeXML might have done something
    // strange to it
    base = d->m_doc.documentElement();

    //kDebug(260) << "Result of xmlgui merging:" << d->m_doc.toString();

    // we want some sort of failsafe.. just in case
    if ( base.isNull() )
      d->m_doc = document;
  }
  else
  {
    d->m_doc = document;
  }

  setXMLGUIBuildDocument( QDomDocument() );
}

// if (equals(a,b)) is more readable than if (a.compare(b, Qt::CaseInsensitive)==0)
static bool equalstr(const QString& a, const QString& b) {
    return a.compare(b, Qt::CaseInsensitive) == 0;
}

bool KoXMLGUIClientPrivate::mergeXML( QDomElement &base, QDomElement &additive, KActionCollection *actionCollection )
{
    static const QString &tagAction = KGlobal::staticQString( "Action" );
    static const QString &tagMerge = KGlobal::staticQString( "Merge" );
    static const QString &tagSeparator = KGlobal::staticQString( "Separator" );
    static const QString &attrName = KGlobal::staticQString( "name" );
    static const QString &attrAppend = KGlobal::staticQString( "append" );
    static const QString &attrWeakSeparator = KGlobal::staticQString( "weakSeparator" );
    static const QString &tagMergeLocal = KGlobal::staticQString( "MergeLocal" );
    static const QString &tagText = KGlobal::staticQString( "text" );
    static const QString &attrAlreadyVisited = KGlobal::staticQString( "alreadyVisited" );
    static const QString &attrNoMerge = KGlobal::staticQString( "noMerge" );
    static const QString &attrOne = KGlobal::staticQString( "1" );

    // there is a possibility that we don't want to merge in the
    // additive.. rather, we might want to *replace* the base with the
    // additive.  this can be for any container.. either at a file wide
    // level or a simple container level.  we look for the 'noMerge'
    // tag, in any event and just replace the old with the new
    if ( additive.attribute(attrNoMerge) == attrOne ) // ### use toInt() instead? (Simon)
    {
        base.parentNode().replaceChild(additive, base);
        return true;
    } else {
        // Merge attributes
        {
            const QDomNamedNodeMap attribs = additive.attributes();
            const uint attribcount = attribs.count();

            for(uint i = 0; i < attribcount; ++i) {
                const QDomNode node = attribs.item(i);
                base.setAttribute(node.nodeName(), node.nodeValue());
            }
        }

        // iterate over all elements in the container (of the global DOM tree)
        QDomNode n = base.firstChild();
        while ( !n.isNull() )
        {
            QDomElement e = n.toElement();
            n = n.nextSibling(); // Advance now so that we can safely delete e
            if (e.isNull())
                continue;

            const QString tag = e.tagName();

            // if there's an action tag in the global tree and the action is
            // not implemented, then we remove the element
            if (equalstr(tag, tagAction)) {
                const QString name =  e.attribute(attrName);
                if (!actionCollection->action(name) ||
                    !KAuthorized::authorizeKAction(name))
                {
                    // remove this child as we aren't using it
                    base.removeChild( e );
                    continue;
                }
            }

            // if there's a separator defined in the global tree, then add an
            // attribute, specifying that this is a "weak" separator
            else if (equalstr(tag, tagSeparator)) {
                e.setAttribute( attrWeakSeparator, (uint)1 );

                // okay, hack time. if the last item was a weak separator OR
                // this is the first item in a container, then we nuke the
                // current one
                QDomElement prev = e.previousSibling().toElement();
                if (prev.isNull() ||
                    (equalstr(prev.tagName(), tagSeparator) && !prev.attribute(attrWeakSeparator).isNull() ) ||
                    (equalstr(prev.tagName(), tagText))) {
                    // the previous element was a weak separator or didn't exist
                    base.removeChild( e );
                    continue;
                }
            }

            // the MergeLocal tag lets us specify where non-standard elements
            // of the local tree shall be merged in.  After inserting the
            // elements we delete this element
            else if (equalstr(tag, tagMergeLocal)) {
                QDomNode it = additive.firstChild();
                while ( !it.isNull() )
                {
                    QDomElement newChild = it.toElement();
                    it = it.nextSibling();
                    if (newChild.isNull() )
                        continue;

                    if (equalstr(newChild.tagName(), tagText))
                        continue;

                    if ( newChild.attribute( attrAlreadyVisited ) == attrOne )
                        continue;

                    QString itAppend( newChild.attribute( attrAppend ) );
                    QString elemName( e.attribute( attrName ) );

                    if ( ( itAppend.isNull() && elemName.isEmpty() ) ||
                         ( itAppend == elemName ) )
                    {
                        // first, see if this new element matches a standard one in
                        // the global file.  if it does, then we skip it as it will
                        // be merged in, later
                        QDomElement matchingElement = findMatchingElement( newChild, base );
                        if (matchingElement.isNull() || equalstr(newChild.tagName(), tagSeparator))
                            base.insertBefore( newChild, e );
                    }
                }

                base.removeChild( e );
                continue;
            }

            else if (equalstr(tag, tagText)) {
                continue;
            }
            else if (equalstr(tag, tagMerge)) {
                continue;
            }

            // in this last case we check for a separator tag and, if not, we
            // can be sure that it is a container --> proceed with child nodes
            // recursively and delete the just proceeded container item in
            // case it is empty (if the recursive call returns true)
            else {
                QDomElement matchingElement = findMatchingElement( e, additive );
                if ( !matchingElement.isNull() )
                {
                    matchingElement.setAttribute( attrAlreadyVisited, (uint)1 );

                    if ( mergeXML( e, matchingElement, actionCollection ) )
                    {
                        base.removeChild( e );
                        additive.removeChild(matchingElement); // make sure we don't append it below
                        continue;
                    }

                    continue;
                }
                else
                {
                    // this is an important case here! We reach this point if the
                    // "local" tree does not contain a container definition for
                    // this container. However we have to call mergeXML recursively
                    // and make it check if there are actions implemented for this
                    // container. *If* none, then we can remove this container now
                    QDomElement dummy;
                    if ( mergeXML( e, dummy, actionCollection ) )
                        base.removeChild( e );
                    continue;
                }
            }
        }

        //here we append all child elements which were not inserted
        //previously via the LocalMerge tag
        n = additive.firstChild();
        while ( !n.isNull() )
        {
            QDomElement e = n.toElement();
            n = n.nextSibling(); // Advance now so that we can safely delete e
            if (e.isNull())
                continue;

            QDomElement matchingElement = findMatchingElement( e, base );

            if ( matchingElement.isNull() )
            {
                base.appendChild( e );
            }
        }

        // do one quick check to make sure that the last element was not
        // a weak separator
        QDomElement last = base.lastChild().toElement();
        if (equalstr(last.tagName(), tagSeparator) &&
            (!last.attribute(attrWeakSeparator).isNull())) {
            base.removeChild( last );
        }
    }

    return isEmptyContainer(base, actionCollection);
}

bool KoXMLGUIClientPrivate::isEmptyContainer(const QDomElement& base, KActionCollection *actionCollection) const
{
    // now we check if we are empty (in which case we return "true", to
    // indicate the caller that it can delete "us" (the base element
    // argument of "this" call)
    QDomNode n = base.firstChild();
    while (!n.isNull()) {
        const QDomElement e = n.toElement();
        n = n.nextSibling(); // Advance now so that we can safely delete e
        if (e.isNull())
            continue;

        const QString tag = e.tagName();

        static const QString &tagAction = KGlobal::staticQString("Action");
        static const QString &tagSeparator = KGlobal::staticQString("Separator");
        static const QString &tagText = KGlobal::staticQString("text");
        static const QString &tagMerge = KGlobal::staticQString("Merge");
        if (equalstr(tag, tagAction)) {
            // if base contains an implemented action, then we must not get
            // deleted (note that the actionCollection contains both,
            // "global" and "local" actions)
            static const QString &attrName = KGlobal::staticQString("name");
            if (actionCollection->action(e.attribute(attrName))) {
                return false;
            }
        }
        else if (equalstr(tag, tagSeparator)) {
            // if we have a separator which has *not* the weak attribute
            // set, then it must be owned by the "local" tree in which case
            // we must not get deleted either
            static const QString &attrWeakSeparator = KGlobal::staticQString("weakSeparator");
            const QString weakAttr = e.attribute(attrWeakSeparator);
            if (weakAttr.isEmpty() || weakAttr.toInt() != 1) {
                return false;
            }
        }

        else if (equalstr(tag, tagMerge)) {
            continue;
        }

        // a text tag is NOT enough to spare this container
        else if (equalstr(tag, tagText)) {
            continue;
        }

        // what's left are non-empty containers! *don't* delete us in this
        // case (at this position we can be *sure* that the container is
        // *not* empty, as the recursive call for it was in the first loop
        // which deleted the element in case the call returned "true"
        else {
            return false;
        }
    }

    return true; // I'm empty, please delete me.
}

QDomElement KoXMLGUIClientPrivate::findMatchingElement( const QDomElement &base, const QDomElement &additive )
{
  static const QString &tagAction = KGlobal::staticQString( "Action" );
  static const QString &tagMergeLocal = KGlobal::staticQString( "MergeLocal" );
  static const QString &attrName = KGlobal::staticQString( "name" );

  QDomNode n = additive.firstChild();
  while ( !n.isNull() )
  {
    QDomElement e = n.toElement();
    n = n.nextSibling(); // Advance now so that we can safely delete e -- TODO we don't, so simplify this
    if (e.isNull())
       continue;

    const QString tag = e.tagName();
    // skip all action and merge tags as we will never use them
    if (equalstr(tag, tagAction)
        || equalstr(tag, tagMergeLocal)) {
      continue;
    }

    // now see if our tags are equivalent
    if (equalstr(tag, base.tagName()) &&
        e.attribute(attrName) == base.attribute(attrName)) {
        return e;
    }
  }

  // nope, return a (now) null element
  return QDomElement();
}

void KoXMLGUIClient::setXMLGUIBuildDocument( const QDomDocument &doc )
{
  d->m_buildDocument = doc;
}

QDomDocument KoXMLGUIClient::xmlguiBuildDocument() const
{
  return d->m_buildDocument;
}

void KoXMLGUIClient::setFactory( KoXMLGUIFactory *factory )
{
  d->m_factory = factory;
}

KoXMLGUIFactory *KoXMLGUIClient::factory() const
{
  return d->m_factory;
}

KoXMLGUIClient *KoXMLGUIClient::parentClient() const
{
  return d->m_parent;
}

void KoXMLGUIClient::insertChildClient( KoXMLGUIClient *child )
{
  if (  child->d->m_parent )
    child->d->m_parent->removeChildClient( child );
   d->m_children.append( child );
   child->d->m_parent = this;
}

void KoXMLGUIClient::removeChildClient( KoXMLGUIClient *child )
{
  assert( d->m_children.contains( child ) );
  d->m_children.removeAll( child );
  child->d->m_parent = 0;
}

/*bool KoXMLGUIClient::addSuperClient( KoXMLGUIClient *super )
{
  if ( d->m_supers.contains( super ) )
    return false;
  d->m_supers.append( super );
  return true;
}*/

QList<KoXMLGUIClient*> KoXMLGUIClient::childClients()
{
  return d->m_children;
}

void KoXMLGUIClient::setClientBuilder( KoXMLGUIBuilder *builder )
{
  d->m_builder = builder;
  if ( builder )
    builder->setBuilderComponentData( componentData() );
}

KoXMLGUIBuilder *KoXMLGUIClient::clientBuilder() const
{
  return d->m_builder;
}

void KoXMLGUIClient::plugActionList( const QString &name, const QList<QAction*> &actionList )
{
  if ( !d->m_factory )
    return;

  d->m_factory->plugActionList( this, name, actionList );
}

void KoXMLGUIClient::unplugActionList( const QString &name )
{
  if ( !d->m_factory )
    return;

  d->m_factory->unplugActionList( this, name );
}

QString KoXMLGUIClient::findMostRecentXMLFile( const QStringList &files, QString &doc )
{
    KoXmlGuiVersionHandler versionHandler(files);
    doc = versionHandler.finalDocument();
    return versionHandler.finalFile();
}

void KoXMLGUIClient::addStateActionEnabled(const QString& state,
                                          const QString& action)
{
  StateChange stateChange = getActionsToChangeForState(state);

  stateChange.actionsToEnable.append( action );
  //kDebug(260) << "KoXMLGUIClient::addStateActionEnabled( " << state << ", " << action << ")";

  d->m_actionsStateMap.insert( state, stateChange );
}


void KoXMLGUIClient::addStateActionDisabled(const QString& state,
                                           const QString& action)
{
  StateChange stateChange = getActionsToChangeForState(state);

  stateChange.actionsToDisable.append( action );
  //kDebug(260) << "KoXMLGUIClient::addStateActionDisabled( " << state << ", " << action << ")";

  d->m_actionsStateMap.insert( state, stateChange );
}


KoXMLGUIClient::StateChange KoXMLGUIClient::getActionsToChangeForState(const QString& state)
{
  return d->m_actionsStateMap[state];
}


void KoXMLGUIClient::stateChanged(const QString &newstate, KoXMLGUIClient::ReverseStateChange reverse)
{
  StateChange stateChange = getActionsToChangeForState(newstate);

  bool setTrue = (reverse == StateNoReverse);
  bool setFalse = !setTrue;

  // Enable actions which need to be enabled...
  //
  for ( QStringList::const_iterator it = stateChange.actionsToEnable.constBegin();
        it != stateChange.actionsToEnable.constEnd(); ++it ) {

    QAction *action = actionCollection()->action(qPrintable((*it)));
    if (action) action->setEnabled(setTrue);
  }

  // and disable actions which need to be disabled...
  //
  for ( QStringList::const_iterator it = stateChange.actionsToDisable.constBegin();
        it != stateChange.actionsToDisable.constEnd(); ++it ) {

    QAction *action = actionCollection()->action(qPrintable((*it)));
    if (action) action->setEnabled(setFalse);
  }

}

void KoXMLGUIClient::beginXMLPlug( QWidget* w )
{
  actionCollection()->addAssociatedWidget( w );
  foreach (KoXMLGUIClient* client, d->m_children)
    client->beginXMLPlug( w );
}

void KoXMLGUIClient::endXMLPlug()
{
}

void KoXMLGUIClient::prepareXMLUnplug( QWidget * w )
{
  actionCollection()->removeAssociatedWidget( w );
  foreach (KoXMLGUIClient* client, d->m_children)
    client->prepareXMLUnplug( w );
}

void KoXMLGUIClient::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }
