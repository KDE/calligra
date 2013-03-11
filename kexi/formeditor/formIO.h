/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef FORMIO_H
#define FORMIO_H

#include <QHash>
#include <QPixmap>
#include <QLabel>
#include <QPaintEvent>

#include <kexi_export.h>

class QString;
class QDomElement;
class QDomNode;
class QDomDocument;
class QVariant;
class QLabel;

//! A blank widget displayed when class is not supported
class KFORMEDITOR_EXPORT CustomWidget : public QWidget
{
    Q_OBJECT

public:
    CustomWidget(const QByteArray &className, QWidget *parent);
    virtual ~CustomWidget();

    virtual void paintEvent(QPaintEvent *ev);

private:
    QByteArray m_className;
};

namespace KFormDesigner
{

class Form;
class ObjectTreeItem;
class Container;
class WidgetLibrary;

//! KFormDesigner API version number. Increased on every breaking of backward compatibility.
//! Use KFormDesigner::version() to get real version number of the library.
#define KFORMDESIGNER_VERSION 2

//! \return KFormDesigner API version number for this library. This information is stored
KFORMEDITOR_EXPORT uint version();

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
    FormIO();
    ~FormIO();

    /*! Save the Form in the \a domDoc QDomDocument. Called by saveForm().
        \return true if saving succeeded.
        \sa saveForm() */
    static bool saveFormToDom(Form *form, QDomDocument &domDoc);

    /*! Save the Form \a form to the file \a filename. If \a filename is null or not given,
        a Save File dialog will be shown to choose dest file.
        \return true if saving succeeded.
        \todo Add errors code and error dialog
    */
    static bool saveFormToFile(Form *form, const QString &filename = QString());

    /*! Saves the Form to the \a dest string. \a indent can be specified to apply indentation.
        \return true if saving succeeded.
        \sa saveForm()
     */
    static bool saveFormToString(Form *form, QString &dest, int indent = 0);

    /*! Saves the \a form inside the \a dest QByteArray.
        \return true if saving succeeded.
        \sa saveFormToDom(), saveForm()
     */
    static bool saveFormToByteArray(Form *form, QByteArray &dest);

    /*! Loads a form from the \a domDoc QDomDocument. Called by loadForm() and loadFormData().
        \return true if loading succeeded. */
    static bool loadFormFromDom(Form *form, QWidget *container, QDomDocument &domDoc);

    /*! Loads a form from the \a src QByteArray.
        \sa loadFormFromDom(), loadForm().
        \return true if loading succeeded.
     */
    static bool loadFormFromByteArray(Form *form, QWidget *container, QByteArray &src,
                                      bool preview = false);

    static bool loadFormFromString(Form *form, QWidget *container, QString &src,
                                   bool preview = false);

    /*! Loads the .ui file \a filename in the Form \a form. If \a filename is null or not given,
        a Open File dialog will be shown to select the file to open.
        createToplevelWidget() is used to load the Form's toplevel widget.
        \return true if loading succeeded.
        \todo Add errors code and error dialog
    */
    static bool loadFormFromFile(Form *form, QWidget *container,
                                 const QString &filename = QString());

    /*! Saves the widget associated to the ObjectTreeItem \a item into DOM document \a domDoc,
        with \a parent as parent node.
        It calls readPropertyValue() for each object property, readAttribute() for each
        attribute and itself to save child widgets.
        \return true if saving succeeded.
        This is used to copy/paste widgets.
    */
    static void saveWidget(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc,
                           bool insideGridLayout = false);

    /*! Cleans the "UI" QDomElement after saving widget. It deletes the "includes" element
     not needed when pasting, and make sure all the "widget" elements are at the beginning.
     Call this after copying a widget, before pasting.*/
    static void cleanClipboard(QDomElement &uiElement);

    /*! Loads the widget associated to the QDomElement \a el into the Container \a container,
        with \a parent as parent widget.
        If parent = 0, the Container::widget() is used as parent widget.
        This is used to copy/paste widgets.
    */
    static void loadWidget(Container *container,
                           const QDomElement &el, QWidget *parent = 0);

    /*! Save an element in the \a domDoc as child of \a parentNode.
      The element will be saved like this :
      \code  <$(tagName) name = "$(property)">< value_as_XML ><$(tagName)/>
      \endcode
    */
    static void savePropertyElement(QDomElement &parentNode, QDomDocument &domDoc, const QString &tagName,
                                    const QString &property, const QVariant &value);

    /*! Read an object property in the DOM doc.
       \param node   the QDomNode of the property
       \param obj    the widget whose property is being read
       \param name   the name of the property being read
    */
    static QVariant readPropertyValue(QDomNode node, QObject *obj, const QString &name);

    /*! Write an object property in the DOM doc.
       \param parentNode the DOM document to write to
       \param name   the name of the property being saved
       \param value  the value of this property
       \param w      the widget whose property is being saved
       \param lib    the widget library for which the property is being saved

       Properties of subwidget are saved with subwidget="true" arribute added
       to 'property' XML element.
    */
    static void savePropertyValue(QDomElement &parentNode, QDomDocument &parent, const char *name,
                                  const QVariant &value, QWidget *w, WidgetLibrary *lib = 0);

protected:
    /*! Saves the QVariant \a value as text to be included in an xml file, with \a parentNode.*/
    static void writeVariant(QDomDocument &parent, QDomElement &parentNode, const QVariant& value);

    /*! Creates a toplevel widget from the QDomElement \a element in the Form \a form,
     with \a parent as parent widget.
     It calls readPropertyValue() and loadWidget() to load child widgets.
    */
    static void createToplevelWidget(Form *form, QWidget *container, QDomElement &element);

    /*! \return the name of the pixmap saved, to use to access it
        This function save the QPixmap \a pixmap into the DOM document \a domDoc.
        The pixmap is converted to XPM and compressed for compatibility with Qt Designer.
        Encoding code is taken from Designer.
    */
    static QString saveImage(QDomDocument &domDoc, const QPixmap &pixmap);

    /*! \return the loaded pixmap
        This function loads the pixmap named \a name in the DOM document \a domDoc.
        Decoding code is taken from QT Designer.
    */
    static QPixmap loadImage(QDomDocument domDoc, const QString& name);

    /*! Reads the child nodes of a "widget" element. */
    static void readChildNodes(ObjectTreeItem *tree, Container *container,
                               const QDomElement &el, QWidget *w);

    /*! Adds an include file name to be saved in the "includehints" part of .ui file,
     which is needed by uic. */
    static void addIncludeFileName(const QString &include, QDomDocument &domDoc);

private:
    // This hash stores buddies associations until the Form is completely loaded.
    static QHash<QString, QLabel*> *m_buddies;

    /// Instead of having to pass these for every functions, we just store them in the class
    //static QWidgdet  *m_currentWidget;
//! @todo remove
#ifdef __GNUC__
#warning "remove m_currentItem and m_currentForm.."
#else
#pragma WARNING( remove m_currentItem and m_currentForm.. )
#endif
    static ObjectTreeItem   *m_currentItem;
    static Form *m_currentForm;
    static bool m_savePixmapsInline;
};

}

#endif
