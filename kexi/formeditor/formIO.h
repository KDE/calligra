/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef FORMIO_H
#define FORMIO_H

#include <qobject.h>
#include <qstring.h>

class QString;
class QDomElement;
class QDomNode;
class QDomDocument;
class QDomText;
class QVariant;

namespace KFormDesigner {

class ObjectPropertyBuffer;
class Form;
class ObjectTreeItem;
class Container;
class WidgetLibrary;

/** This class act as a namespace for all .ui files related functions, ie saving/loading .ui files.
    You don't need to create a FormIO object, as all methods are static.\n
    This class is able to read and write Forms to .ui files, and to save each type of properties, including set and enum
    properties, and pixmaps(pixmap-related code was taken from Qt Designer).
 **/
 //! A class to save/load forms from .ui files
class KFORMEDITOR_EXPORT FormIO : public QObject
{
	Q_OBJECT

	public:
		FormIO(QObject *parent, const char *name);
		~FormIO(){;}

		static int saveFormToDom(Form *form, QDomDocument &domDoc);
		/*! \return 0 if saving failed, 1 otherwise\n
		    Save the Form \a form to the file \a filename. If \a filename is null or not given,
		    a Save File dialog will be shown to choose dest file.
		    \todo Add errors code and error dialog
		*/
		static int saveForm(Form *form, const QString &filename=QString::null);
		/*! \return 0 if faild otherwiese 1
		 * uses a QByteArray as dest
		 */
		static int saveForm(Form *form, QByteArray &dest);

		static int loadFormFromDom(Form *form, QWidget *container, QDomDocument &inBuf);

		/*! \return 0 if faild otherwise 1
		 * loads widgets on ground and sets properties to ground
		 */
		static int loadFormData(Form *form, QWidget *container, QByteArray &src, bool preview=false);


		/*! \return 0 if loading failed, 1 otherwise\n
		   Load the .ui file \a filename in the Form \a form. If \a filename is null or not given,
		   a Open File dialog will be shown to select the file to open.
		   createToplevelWidget() is used to load the Form's toplevel widget.
		   \todo Add errors code and error dialog
		*/
		static int loadForm(Form *form, QWidget *container, const QString &filename=QString::null);

		/*! Save the widget associated to the ObjectTreeItem \a item into DOM document \a domDoc,
		    with \a parent as parent node.
		    It calls readProp() for each object property, readAttribute() for each attribute and
		    itself to save child widgets.\n
		    This is used to copy/paste widgets.
		*/
		static void         saveWidget(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc, bool insideGridLayout=false);
		/*! Loads the widget associated to the QDomElement \a el into the Container \a container,
		    with \a parent as parent widget. \a lib is the WidgetLibrary to use to create the widget.
		    If parent = 0, the Container::widget() is used as parent widget.
		    This is used to copy/paste widgets.
		*/
		static void         loadWidget(Container *container, WidgetLibrary *lib, const QDomElement &el, QWidget *parent=0);
		static void         saveProperty(QDomElement &parentNode, QDomDocument &domDoc, const QString &tagName, const QString &property, const QVariant &value);
		/*! Read an object property in the DOM doc.
		   \param node   the QDomNode of the property
		   \param obj    the widget whose property is being read
		   \param name   the name of the property being saved
		*/
		static QVariant     readProp(QDomNode node, QObject *obj, const QString &name);
		/*! Write an object property in the DOM doc.
		   \param parent the DOM document to write to
		   \param name   the name of the property being saved
		   \param value  the value of this property
		   \param w       the widget whose property is being saved
		*/
		static void   prop(QDomElement &parentNode, QDomDocument &parent, const char *name, const QVariant &value, QWidget *w, WidgetLibrary *lib=0);

	protected:
		static void   writeVariant(QDomDocument &parent, QDomElement &type, QDomText &valueE, QVariant value);

		/*! Creates a toplevel widget from the QDomElement \a element in the Form \a form, with \a parent as parent widget.
		  It calls readProp() and loadWidget() to load child widgets.
		*/
		static void         createToplevelWidget(Form *form, QWidget *container, QDomElement &element);

		/*! \return the name of the pixmap saved, to use to access it
		    This function save the QPixmap \a pixmap into the DOM document \a domDoc.
		    The pixmap is converted to XPM and compressed for compatibility with Qt Designer. Encoding code is taken from Designer.
		*/
		static QString      saveImage(QDomDocument &domDoc, const QPixmap &pixmap);
		/*! \return the loaded pixmap
		    This function loads the pixmap named \a name in the DOM document \a domDoc.
		    Decoding code is taken from QT Designer.
		*/
		static QPixmap      loadImage(QDomDocument domDoc, QString name);
		/*! Loads the layout (ie calls Container::setLayout() ) which type is \a name,
		   and belonging to the widget represented by the ObjectTreeItem \a tree. */
		static void         loadLayout(const QDomElement &el, ObjectTreeItem *tree);
		static void         readChildNodes(ObjectTreeItem *tree, Container *container, WidgetLibrary *lib, const QDomElement &el, QWidget *w);
		static void         addIncludeFile(const QString &include, QDomDocument &domDoc);
};

}

#endif


