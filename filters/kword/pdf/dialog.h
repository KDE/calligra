/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <q3valuevector.h>
#include <QPair>

#include <kdialogbase.h>

class Q3ButtonGroup;
class QRadioButton;
class QCheckBox;
class KLineEdit;


//-----------------------------------------------------------------------------
class SelectionRange
{
public:
    SelectionRange() {}
    SelectionRange(const QString &);

    uint nbPages() const;

private:
    Q3ValueVector<QPair<uint, uint> > _ranges;

    friend class SelectionRangeIterator;
};

class SelectionRangeIterator
{
public:
    SelectionRangeIterator(const SelectionRange &);

    int toFirst();
    int current() const { return _current; }
    int next();
    static int end() { return -1; }

private:
    uint _index;
    int  _current;
    const Q3ValueVector<QPair<uint, uint> > &_ranges;
};

//-----------------------------------------------------------------------------
namespace PDFImport
{

class Options
{
public:
    Options() {}

public:
    SelectionRange range;
    QString        ownerPassword, userPassword;
    bool           importImages, smart;
};

class Dialog : public KDialogBase
{
Q_OBJECT
public:
    Dialog(uint nbPages, bool isEncrypted, QWidget *parent);
    ~Dialog();

    Options options() const;

private slots:
    void rangeChanged(const QString &);

private:
    uint          _nbPages;
    QRadioButton *_allButton, *_rangeButton;
    Q3ButtonGroup *_group;
    KLineEdit    *_range, *_owner, *_user;
    QCheckBox    *_images, *_smart;
};

} // namespace

#endif
