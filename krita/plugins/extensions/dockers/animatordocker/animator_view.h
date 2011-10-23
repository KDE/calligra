/*
 *
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef ANIMATOR_VIEW_H
#define ANIMATOR_VIEW_H

#include <qtreeview.h>
#include "animator_model.h"

class AnimatorView : public QTreeView {
    Q_OBJECT
    
public:
    AnimatorView();
    virtual ~AnimatorView();
    
public:
    
    
protected slots:
    virtual void activate(QModelIndex index);
    
protected:
    AnimatorModel* amodel();
};

#endif
