/***************************************************************************
                          property.h  -  description
                             -------------------
    begin                : 08.12.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef PROPERTY_H
#define PROPERTY_H

class QWidget;
class QString;
class PropertyEditor;

/**
  *@author Alexander Dymo
  */
/*
enum PropertyType {
    Report_PageSize, Report_PageOrientation, Report_TopMargin,
    Report_BottomMargin, Report_LeftMargin, Report_RightMargin,
    Section_Height, Section_PrintFrequency,
    Detail_Level,
    Line_X1, Line_Y1, Line_X2, Line_Y2, Line_Width, Line_Color, Line_Style,
    Item_Text, Item_X, Item_Y, Item_Width, Item_BackgroundColor, Item_ForegroundColor,
    Item_BorderColor, Item_BorderWidth, Item_BorderStyle, Item_FontFamily,
    Item_FontSize, Item_FontWeight, Item_FontItalic, Item_HAlignment,
    Item_VAlignment, Item_WordWrap,
    Field_Field, Field_DataType, Field_DateFormat, Field_Precision, Field_Currency,
    Field_NegValueColor, Field_CommaSeparator,
    CalcField_CalculationType}
*/

/** Integers that represent the type of the property */
enum PropertyType {
    StringValue,   /*string*/
    IntegerValue,  /*integer number*/
    ValueFromList, /*only numbers or strings from a list*/
    Color,         /*color*/
    Symbol,        /*unicode symbol's code*/
    FontName,      /*font name - "times new roman"*/
    LineStyle      /*combobox with lines at the place of combo items descriptions */
};

/**
  Property.
  Contains name, type (PropertyType) and value.
  Value is of QString type, not QVariant - may be I'm wrong here.
  Property has operator < to be able to be included in std::map<>
  (all objects with properties must have
   map<QString, Property* > propertyList
   or similar).
  Those property lists can be intersected as sets of Property by
  set_intersection() - STL algo function.
  The purpose of Property intersection is to find a common properties
  for various elements and then display them in a property editor. 
*/
class Property {
public:
    Property() {}
    Property(int type, QString name, QString description="", QString value=QString::null);
    virtual ~Property();

    bool operator<(const Property &prop) const;

    QString name() const;
    void setName(QString name);
    int type() const;
    void setType(int type);
    QString value() const;
    void setValue(QString value);
    QString description() const;
    void setDescription(QString description);

    virtual QWidget *editorOfType(const PropertyEditor *editor);

protected:
    int m_type;
    QString m_name;
    QString m_description;
    QString m_value;
};

/** Property with the values that have descriptions. In the combobox
    descriptions must be displayed, not the values itself.
 */
class DescriptionProperty: public Property {
public:
    DescriptionProperty(): Property() {}
    /** map<description, value> <-> map<QString, QString> */
    DescriptionProperty(QString name, std::map<QString, QString> v_correspList,
        QString description="", QString value=QString::null);

    void setCorrespList(std::map<QString, QString> list);
    std::map<QString, QString> correspList;

    virtual QWidget *editorOfType(const PropertyEditor *editor);
};

/** Smart pointer to Property */
class PropPtr{
public:
    PropPtr()
    {
        m_prop = 0;
    }
    PropPtr(Property *prop): m_prop(prop) {}

    ~PropPtr()
    {
        if (m_prop != 0)
            delete m_prop;
    }
    
    Property *operator->()
    {
        if (m_prop != 0)
            return m_prop;
        else
            return new Property();
    }

    Property *operator->() const
    {
        if (m_prop != 0)
            return m_prop;
        else
            return new Property();
    }

    bool operator<(const PropPtr& p) const
    {
        if ((prop()->type() < p.prop()->type()) && (prop()->name() < p.prop()->name()))
            return true;
        else
            return false;        
    }

    Property *prop() const
    {
        return m_prop;
    }
private:
    Property *m_prop;
};

#endif
