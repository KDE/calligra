#ifndef STYLESTACK_H
#define STYLESTACK_H

#include <qvaluelist.h>
#include <qdom.h>

/**
 *  This class implements a stack for the different styles of an object.
 *
 *  There can be several styles that are valid for one object. For example
 *  a textobject on a page has styles 'pr3' and 'P7' and a paragraph in
 *  that textobject has styles 'P1' and 'T3'. And some styles even have
 *  parent-styles...
 *
 *  If you want to know if there is, for example,  the attribute 'fo:font-family'
 *  for this paragraph, you have to look into style 'T3', 'P1', 'P7' and 'pr3'.
 *  When you find this attribute in one style you have to stop processing the list
 *  and take the found attribute for this object.
 *
 *  This is what this class does. You can push styles on the stack while walking
 *  through the xml-tree to your object and then ask the stack if any of the styles
 *  provides a certain attribute. The stack will search from top to bottom, i.e.
 *  in our example from 'T3' to 'pr3' and return the first occurrence of the wanted
 *  attribute.
 *
 *  So this is some sort of inheritance where the styles on top of the stack overwrite
 *  the same attribute of a lower style on the stack.
 */
class StyleStack
{
public:
    StyleStack();
    virtual ~StyleStack();

    enum Marks { PageMark = 0, ObjectMark, ParagraphMark, SpanMark };

    /**
     * Clears the complete stack.
     */
    void clear();

    /**
     * Keep only the styles up to the given mark.
     * For instance popToMark( PageMark ) keeps only the page styles on the stack,
     * popToMark( ObjectMark ) keeps page and object styles on the stack, etc.
     */
    void popToMark( int mark );

    /**
     * Set the mark (stores the index of the object on top of the stack).
     */
    void setMark( int mark );

    /**
     * Removes the style on top of the stack.
     */
    void pop();

    /**
     * Pushs the new style onto the stack.
     */
    void push( const QDomElement& style );

    /**
     * Check if any of the styles on the stack has an attribute called 'name'.
     */
    bool hasAttribute( const QString& name ) const;

    /**
     * Search for the attribute called 'name', starting on top of the stack,
     * and return it.
     */
    QString attribute( const QString& name ) const;

    /**
     * Check if any of the styles on the stack has a child node called 'name'.
     */
    bool hasChildNode(const QString & name) const;

    /**
     * Search for a child node called 'name', starting on top of the stack,
     * and return it.
     */
    QDomNode childNode(const QString & name) const;

    /**
     * Special case for the current font size, due to special handling of fo:font-size="115%".
     */
    double fontSize() const;

    /**
     * Return the name of the style specified by the user,
     * i.e. not an auto style
     */
    QString userStyleName() const;

private:
    // We use QMemArray<uint> instead of uint[3] so that we can support
    // application-defined marks.
    QMemArray<int> m_marks;

    // We use QValueList instead of QValueStack because we need access to all styles
    // not only the top one.
    QValueList<QDomElement> m_stack;
};


#endif /* STYLESTACK_H */

