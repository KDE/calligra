/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef kw_pageadaptor_h
#define kw_pageadaptor_h

#include <QtDBus/QtDBus>
#include <koffice_export.h>

class KWPage;

//FIXME hmmm... why I keep to get a "error: invalid function declaration" here ?
//class KWORD_TEST_EXPORT KWPageAdaptor : public QDBusAbstractAdaptor {

class KWPageAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.word.page")

public:
    explicit KWPageAdaptor( QObject* parent, KWPage* page );
    ~KWPageAdaptor();

public Q_SLOTS:

    /// return the width of this page in pt.
    double width() const;
    /// set the width of the page in pt.
    void setWidth(double x);

    /// return the height of this page in pt.
    double height() const;
    /// set the height of the page in pt.
    void setHeight(double y);

    /// return the height of the margin at top in pt.
    double topMargin() const;
    /// set height of the top margin in pt
    void setTopMargin(double x);

    /// return the height of the margin at bottom in pt.
    double bottomMargin() const;
    /// set height of the bottom margin in pt
    void setBottomMargin(double y);

    /// return the width of the margin at left in pt.
    double leftMargin() const;
    /// set width of the left margin in pt.
    void setLeftMargin(double l);

    /// return the width of the margin at right in pt;
    double rightMargin() const;
    /// set width of the right margin in pt.
    void setRightMargin(double r);

private:
    KWPage* m_page;
};

#endif
