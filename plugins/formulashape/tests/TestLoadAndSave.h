// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef _TESTLOADANDSAVE_H_
#define _TESTLOADANDSAVE_H_

#include <QObject>

class TestLoadAndSave : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // Token Elements
    void identifierElement_data();
    void identifierElement();
    void numberElement_data();
    void numberElement();
    void operatorElement_data();
    void operatorElement();
    void textElement_data();
    void textElement();
    void spaceElement_data();
    void spaceElement();
    void stringElement_data();
    void stringElement();
    void glyphElement_data();
    void glyphElement();

    // Attributes of Token Elements
    void mathVariant_data();
    void mathVariant();
    void mathSize_data();
    void mathSize();
    void mathColor_data();
    void mathColor();
    void mathBackground_data();
    void mathBackground();

    // Deprecated Attributes of Token Elements
    // fontfamily is not included since it's quite freeform
    void fontSize_data();
    void fontSize();
    void fontWeight_data();
    void fontWeight();
    void fontStyle_data();
    void fontStyle();
    void color_data();
    void color();

    // General layout elements
    void rowElement_data();
    void rowElement();
    void fractionElement_data();
    void fractionElement();
    void rootElement_data();
    void rootElement();
    void styleElement_data();
    void styleElement();
    void errorElement_data();
    void errorElement();
    void paddedElement_data();
    void paddedElement();
    void phantomElement_data();
    void phantomElement();
    void fencedElement_data();
    void fencedElement();
    void encloseElement_data();
    void encloseElement();

    // Script and limit elements
    void subElement_data();
    void subElement();
    void supElement_data();
    void supElement();
    void subsupElement_data();
    void subsupElement();
    void underElement_data();
    void underElement();
    void overElement_data();
    void overElement();
    void underoverElement_data();
    void underoverElement();
    void multiscriptsElement_data();
    void multiscriptsElement();

    // Tables and matrices
    void tableElement_data();
    void tableElement();
    void trElement_data();
    void trElement();
    // void labeledtrElement_data();
    // void labeledtrElement();
    void tdElement_data();
    void tdElement();

    // Enlivening elements
    void actionElement_data();
    void actionElement();
};

#endif // _TESTLOADANDSAVE_H_
