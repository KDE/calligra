/* -*- Mode: C++ -*-
   $Id$
   KDGantt - a multi-platform charting engine
*/
/****************************************************************************
 ** Copyright (C)  2002-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDGantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDGantt licenses may use this file in
 ** accordance with the KDGantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
 **   information about KDGantt Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 ** As a special exception, permission is given to link this program
 ** with any edition of Qt, and distribute the resulting executable,
 ** without including the source code for Qt in the source distribution.
 **
 **********************************************************************/


#include "KDGanttViewTaskLinkGroup.h"
#include "KDGanttXMLTools.h"
#include "KDGanttView.h"

QDict<KDGanttViewTaskLinkGroup> KDGanttViewTaskLinkGroup::sGroupDict;

/*! \class KDGanttViewTaskLinkGroup KDGanttViewTaskLinkGroup.h
  A group of task links.

  This class groups a number of task links together in order to
  manipulate them uniformly.
*/

/*!
  Constructs an empty task link group
*/
KDGanttViewTaskLinkGroup::KDGanttViewTaskLinkGroup()
{
    generateAndInsertName(QString());
}

/*!
  Destructor
  Removes this task link group from the list of task link groups in the
  KDGanttView class.
*/
KDGanttViewTaskLinkGroup::~KDGanttViewTaskLinkGroup()
{
    if (!myTaskLinkList.isEmpty()) {
        myTaskLinkList.first()->from().first()->myGanttView->removeTaskLinkGroup(this);
    }
}



/*!
  Constructs an empty task link group and records it under the name \a
  name so that it can later be found again with
  KDGanttViewTaskLinkGroup::find().

  \param name the search name of this task link group
*/
KDGanttViewTaskLinkGroup::KDGanttViewTaskLinkGroup( const QString& name )
{
    generateAndInsertName( name );
}

/*!
  Adds a task link LINK to this group. If the task link is already a member of
  another group, it will be removed from it.
  This function is equivalent to  LINK->setGroup(this), where this is
  a pointer to this TaskLinkGroup.
  \param link a pointer to the task link to add to this task link group
  visible, and false to hide them
  \sa remove()
*/
void KDGanttViewTaskLinkGroup::insert (KDGanttViewTaskLink* link)
{
    link->setGroup(this);
}


/*!
  Removes a task link LINK from this group.
  You may remove a tasklink  LINK from its group with  LINK->setGroup(0).

  \param link a pointer to the task link to remove from this task link group
  \return true if the task link was a member of this group
  \sa insert()
*/
bool KDGanttViewTaskLinkGroup::remove (KDGanttViewTaskLink* link)
{
    KDGanttViewTaskLinkGroup* g = link->group();
    if ((g == this))
        link->setGroup(0);
    return (g == this);
}


/*!
  Specifies whether the task links of this group should be visible or not.

  \param show visible pass true to make the task links of this group
  visible, and false to hide them
  \sa isVisible()
*/
void KDGanttViewTaskLinkGroup::setVisible( bool show )
{
    isvisible = show;
    QPtrListIterator<KDGanttViewTaskLink> it(myTaskLinkList);
    for ( ; it.current(); ++it ) {
        it.current()->setVisible(show);
    }
}


/*!
  Returns whether the task links of this group should be visible or not.

  \return true if the task links of this group are visible
  \sa setVisible()
*/
bool KDGanttViewTaskLinkGroup::visible() const
{
    return isvisible;
}


/*!
  Specifies whether the task links of this group should be shown
  highlighted. The user can also highlight a task link with the mouse.

  \param highlight pass true in order to highlight the task links in
  this group
  \sa highlight()
*/
void KDGanttViewTaskLinkGroup::setHighlight( bool highlight )
{
    ishighlighted=  highlight;
    QPtrListIterator<KDGanttViewTaskLink> it(myTaskLinkList);
    for ( ; it.current(); ++it )
        it.current()->setHighlight(highlight );

}


/*!
  Returns whether all task links in this group are highlighted, either
  programmatically by setHighlight() or by the user with the
  mouse. This method is not particularly useful and is mainly provided
  for API uniformity reasons.

  \return true if all the task links in this group are highlighted
  \sa setHighlight()
*/
bool KDGanttViewTaskLinkGroup::highlight() const
{
    return ishighlighted;
}


/*!
  Specifies the color to draw the task links in this group in.

  \param color the color to draw the task links in this group in
  \sa color()
*/
void KDGanttViewTaskLinkGroup::setColor( const QColor& color )
{
    myColor = color;
    QPtrListIterator<KDGanttViewTaskLink> it(myTaskLinkList);
    for ( ; it.current(); ++it )
        it.current()->setColor(color);
}


/*!
  Returns the color in which the task links in this group are
  drawn. If task links have been assigned individual colors, the
  return value of this method is undefined. This method is not
  particularly useful and is mainly provided for API uniformity
  reasons.

  \return the color in which the task links in this group are drawn
  \sa setColor()
*/
QColor KDGanttViewTaskLinkGroup::color() const
{
    return myColor;
}


/*!
  Specifies the highlight color to draw the task links in this group in.

  \param color the highlight color to draw the task links in this group in
  \sa color()
*/
void KDGanttViewTaskLinkGroup::setHighlightColor( const QColor& color )
{

    myColorHL = color;
    QPtrListIterator<KDGanttViewTaskLink> it(myTaskLinkList);
    for ( ; it.current(); ++it )
        it.current()->setHighlightColor(color);
}


/*!
  Returns the highlight color in which the task links in this group are
  drawn. If task links have been assigned individual highlight colors,
  the return value of this method is undefined. This method is not
  particularly useful and is mainly provided for API uniformity reasons.

  \return the highlight color in which the task links in this group
  are drawn
  \sa setColor()
*/
QColor KDGanttViewTaskLinkGroup::highlightColor() const
{
    return myColorHL;
}


/*!
  Adds a task link LINK to this group. If the task link is already a member of
  another group, it will  not be removed from it.
  \param a pointer to the task link to add to this task link group
  visible, and false to hide them
  \sa removeItem()
*/
void KDGanttViewTaskLinkGroup::insertItem (KDGanttViewTaskLink* link)
{
    myTaskLinkList.append (link);
}


/*!
  Removes a task link LINK from this group.

  \param a pointer to the task link to remove from this task link group
  \sa insertItem()
*/
void KDGanttViewTaskLinkGroup::removeItem (KDGanttViewTaskLink* link)
{
    myTaskLinkList.remove(link);
}


/*!
  Returns the task link group with the specified name.

  \param name the name to search for
  \return the task link group with the specified name; 0 if no group
  with that name exists
*/
KDGanttViewTaskLinkGroup* KDGanttViewTaskLinkGroup::find( const QString& name )
{
    if (name.isEmpty()) // avoid error msg from QDict
        return 0;
    return sGroupDict.find( name );
}


/*!
  Creates a DOM node that describes this task link group.

  \param doc the DOM document to which the node belongs
  \param parentElement the element into which to insert this node
*/
void KDGanttViewTaskLinkGroup::createNode( QDomDocument& doc,
                                           QDomElement& parentElement )
{
    QDomElement taskLinkGroupElement = doc.createElement( "TaskLink" );
    parentElement.appendChild( taskLinkGroupElement );

    KDGanttXML::createBoolNode( doc, taskLinkGroupElement, "Highlight",
                           highlight() );
    KDGanttXML::createColorNode( doc, taskLinkGroupElement, "Color", color() );
    KDGanttXML::createColorNode( doc, taskLinkGroupElement, "HighlightColor",
                            highlightColor() );
    KDGanttXML::createBoolNode( doc, taskLinkGroupElement, "Visible",
                           visible() );
    KDGanttXML::createStringNode( doc, taskLinkGroupElement, "Name", _name );
}


/*!
  Creates a KDGanttViewTaskLinkGroup according to the specification in a DOM
  element.

  \param element the DOM element from which to read the specification
  \return the newly created task link group
*/
KDGanttViewTaskLinkGroup* KDGanttViewTaskLinkGroup::createFromDomElement( QDomElement& element )
{
    QDomNode node = element.firstChild();
    bool highlight = false, visible = false;
    QColor color, highlightColor;
    QString name;
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Highlight" ) {
                bool value;
                if( KDGanttXML::readBoolNode( element, value ) )
                    highlight = value;
            } else if( tagName == "Visible" ) {
                bool value;
                if( KDGanttXML::readBoolNode( element, value ) )
                    visible = value;
            } else if( tagName == "Color" ) {
                QColor value;
                if( KDGanttXML::readColorNode( element, value ) )
                    color = value;
            } else if( tagName == "HighlightColor" ) {
                QColor value;
                if( KDGanttXML::readColorNode( element, value ) )
                    highlightColor = value;
            } else if( tagName == "Name" ) {
                QString value;
                if( KDGanttXML::readStringNode( element, value ) )
                    name = value;
            } else {
                qDebug( "Unrecognized tag name: %s", tagName.latin1() );
                Q_ASSERT( false );
            }
        }
        node = node.nextSibling();
    }

    KDGanttViewTaskLinkGroup* tlg;
    if( !name.isEmpty() )
        tlg = new KDGanttViewTaskLinkGroup( name );
    else
        tlg = new KDGanttViewTaskLinkGroup();

    tlg->setHighlight( highlight );
    tlg->setVisible( visible );
    tlg->setHighlightColor( highlightColor );
    tlg->setColor( color );

    return tlg;
}

/*!
  Generates a unique name if necessary and inserts it into the group
  dictionary.
*/
void KDGanttViewTaskLinkGroup::generateAndInsertName( const QString& name )
{
    // First check if we already had a name. This can be the case if
    // the item was reconstructed from an XML file.
    if( !_name.isEmpty() )
        // We had a name, remove it
        sGroupDict.remove( _name );
    
    QString newName;
    if ( name.isEmpty() || sGroupDict.find( name ) ) {
        // create unique name
        newName.sprintf( "%p", (void* )this );
        while( sGroupDict.find( newName ) ) {
            newName += "_0";
        }
    } else {
        newName = name;
    }
    sGroupDict.insert( newName, this );
    _name = newName;
    //qDebug("KDGanttViewTaskLinkGroup::generateAndInsertName: inserted '%s'",newName.latin1());
}

