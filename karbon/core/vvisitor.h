/* This file is part of the KDE project
   Copyright (C) 2002-2005, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VVISITOR_H__
#define __VVISITOR_H__

#include <koffice_export.h>

class VPath;
class VDocument;
class VGroup;
class VLayer;
class VObject;
class VSubpath;
class VSelection;
class VText;
class VImage;

/**
  \brief The abstract visitor class
  
  (From Wikipedia)
  
  In object-oriented programming and software engineering, the visitor design pattern is a way of  separating an algorithm from an object structure. A practical result of this separation is the ability to add new operations to existing object structures without modifying those structures.
 
  The idea is to use a structure of element classes, each of which has an accept method that takes a visitor object as an argument. The visitor is an interface that has a different visit() method for each element class. The accept() method of an element class calls back the visit() method for its class. Separate concrete visitor classes can then be written that perform some particular operations.
 
  One of these visit() methods of a concrete visitor can be thought of as methods not of a single class, but rather methods of a pair of classes: the concrete visitor and the particular element class. Thus the visitor pattern simulates double dispatch in a conventional single-dispatch object-oriented language such as Java, Smalltalk, and C++.
 
  The visitor pattern also specifies how iteration occurs over the object structure. In the simplest version, where each algorithm needs to iterate in the same way, the accept() method of a container element, in addition to calling back the visit() method of the visitor, also passes the visitor object to the accept() method of all its constituent child elements.
 
  Because the Visitor object has one principal function (manifested in a plurality of specialized methods) and that function is called visit(), the Visitor can be readily identified as a potential function object or functor. Likewise, the accept() function can be identified as a function applicator, a mapper, which knows how to traverse a particular type of object and apply a function to its elements.
*/
class KARBONBASE_EXPORT VVisitor
{
public:
	/**
		Constructs a new visitor class
	*/
	VVisitor()
	{
		m_success = false;
	}

	/**
		General visit method. Pass an \a object to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
		Return the success value.
	*/
	virtual bool visit( VObject& object );

	/**
		Visit method for a VObject. Pass an \a object to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVObject( VObject& object );

	/**
		Visit method for a VPath. Pass a \a composite path to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVPath( VPath& composite );

	/**
		Visit method for a VDocument. Pass a \a document to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVDocument( VDocument& document );

	/**
		Visit method for a VGroup. Pass a \a group of objects to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVGroup( VGroup& group );

	/**
		Visit method for a VLayer. Pass a \a layer to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVLayer( VLayer& layer );

	/**
		Visit method for a VSubpath. Pass a \a path to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVSubpath( VSubpath& path );

	/**
		Visit method for a VSelection. Pass a \a selection to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVSelection( VSelection& selection );

	/**
		Visit method for a VText. Pass some \a text to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVText( VText& text );

	/**
		Visit method for a VImage. Pass an \a image to this function.
		This is a virtual function so you need to implement it in the subclass if you want to use it.
	*/
	virtual void visitVImage( VImage& img );

	/**
		Return if the operation was a success or not
	*/
	bool success() const
	{
		return m_success;
	}

protected:
	/*
	 * Make this class "abstract".
	 */
	/**
		Destructs a visitor class
	*/
	virtual ~VVisitor() {}

	/**
		Set the success property.
	*/
	void setSuccess( bool success = true )
	{
		m_success = success;
	}

private:
	bool m_success;
};

#endif

