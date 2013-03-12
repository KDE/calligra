#ifndef FAKE_KMACROS_H
#define FAKE_KMACROS_H

#include <QMetaType>
#include <QByteArray>
#include <QPair>
#include <klocale.h>

#define KDE_EXPORT Q_DECL_EXPORT
#define KDE_IMPORT Q_DECL_IMPORT
// #define KDE_EXPORT
// #define KDE_IMPORT

#define KDE_IS_VERSION(x,y,z) 1

#define KDE_DEPRECATED
#define KDE_CONSTRUCTOR_DEPRECATED

#endif
