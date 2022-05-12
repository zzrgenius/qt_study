#ifndef TTKGLOBAL_H
#define TTKGLOBAL_H

/***************************************************************************
 * This file is part of the TTK Widget Tools project
 * Copyright (C) 2015 - 2022 Greedysky Studio

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#include <QMap>
#include <QSet>
#include <QVariant>

#ifdef Q_CC_GNU
#  pragma GCC diagnostic ignored "-Wswitch"
#  pragma GCC diagnostic ignored "-Wparentheses"
#  pragma GCC diagnostic ignored "-Wunused-result"
#  pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define TTK_QT_VERSION_CHECK(major, minor, patch) (QT_VERSION >= QT_VERSION_CHECK(major, minor, patch))

#ifndef qPrintable
#  define qPrintable(s) QString(s).toLocal8Bit().constData()
#endif

#ifndef qUtf8Printable
#  define qUtf8Printable(s) QString(s).toUtf8().constData()
#endif

#if !TTK_QT_VERSION_CHECK(5,7,0)
#  define TTK_AS_CONST
// this adds const to non-const objects (like std::as_const)
#  ifndef Q_CC_MSVC
template <typename T>
Q_DECL_CONSTEXPR typename std::add_const<T>::type &qAsConst(T &t) noexcept { return t; }
// prevent rvalue arguments:
template <typename T>
void qAsConst(const T &&) = delete;
#  else
template <typename T>
Q_DECL_CONSTEXPR typename std::add_const<T>::type &qAsConst(T &t) { return t; }
#  endif
#endif

//
#ifdef QT_DEBUG
#  define TTK_DEBUG
#endif

//
#if __cplusplus >= 201103L
#  define TTK_CAST
#endif

// cast
#ifdef TTK_CAST
#  define TTKConst_cast(x,y) (const_cast<x>(y))
#else
#  define TTKConst_cast(x,y) ((x)(y))
#endif

#ifdef TTK_CAST
#  define TTKDynamic_cast(x,y) (dynamic_cast<x>(y))
#else
#  define TTKDynamic_cast(x,y) ((x)(y))
#endif

#ifdef TTK_CAST
#  define TTKReinterpret_cast(x,y) (reinterpret_cast<x>(y))
#else
#  define TTKReinterpret_cast(x,y) ((x)(y))
#endif

#ifdef TTK_CAST
#  define TTKStatic_cast(x,y) (static_cast<x>(y))
#else
#  define TTKStatic_cast(x,y) ((x)(y))
#endif

#ifdef TTK_CAST
#  define TTKObject_cast(x,y) (qobject_cast<x>(y))
#else
#  define TTKObject_cast(x,y) ((x)(y))
#endif

#if defined TTK_CAST && TTK_QT_VERSION_CHECK(5,15,0)
#  define TTKVoid_cast(x) (x)(void*)
#else
#  define TTKVoid_cast(x) (x)
#endif

//
#if !TTK_QT_VERSION_CHECK(5,0,0) && defined(Q_CC_GNU)
#  if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#    define Q_COMPILER_DEFAULT_MEMBERS
#    define Q_COMPILER_DELETE_MEMBERS
#    define Q_COMPILER_NULLPTR
#    define Q_COMPILER_EXPLICIT_OVERRIDES
#  endif
#endif

// C++11 keywords and expressions
#ifdef Q_COMPILER_NULLPTR
#  define TTK_NULLPTR  nullptr
#else
# define TTK_NULLPTR  NULL
#endif

#ifdef Q_COMPILER_DEFAULT_MEMBERS
#  define TTK_DEFAULT = default
#else
#  define TTK_DEFAULT
#endif

#ifdef Q_COMPILER_DELETE_MEMBERS
# define TTK_DELETE = delete
#else
# define TTK_DELETE
#endif

#ifdef Q_COMPILER_EXPLICIT_OVERRIDES
# define TTK_OVERRIDE override
# define TTK_FINAL final
#else
# ifndef TTK_OVERRIDE
#  define TTK_OVERRIDE
# endif
# ifndef TTK_FINAL
#  define TTK_FINAL
# endif
#endif

// disable copy
#define TTK_DISABLE_COPY(Class) \
private: \
    Class(const Class &) TTK_DELETE; \
    Class &operator=(const Class &) TTK_DELETE;

// make class name
#define TTK_DECLARE_MODULE(Class) \
public: \
    inline static QString className() \
    { \
        return #Class; \
    }

// marco str cat
#ifndef _MSC_VER
// gcc version less than 3.4.0
#  if __GNUC__ <= 3 && __GNUC_MINOR__ <= 4
#    define TTK_STRCAT(a, b)    a##b
#  else
#    define TTK_STRCAT(a, b)    a b
#  endif
#else
#  define TTK_STRCAT(a, b)      a b
#endif

#define TTK_DECLARE_LIST(Class)        typedef QList<Class> Class##List
#define TTK_DECLARE_FLAG(Flags, Enum)  typedef QFlags<Enum> Flags

#define TTK_DOT             "."
#define TTK_DOTDOT          ".."
#define TTK_SEPARATOR       "/"
#define TTK_PDIR            TTK_STRCAT(TTK_DOTDOT, TTK_SEPARATOR)

#define TTK_SPLITER         "*|||*"
#define TTK_DEFAULT_STR     "-"
#define TTK_NAN_STR         "NaN"
#define TTK_NULL_STR        "null"

// C style format
typedef signed char                      TTKInt8;          /* 8 bit signed */
typedef unsigned char                    TTKUInt8;         /* 8 bit unsigned */
typedef short                            TTKInt16;         /* 16 bit signed */
typedef unsigned short                   TTKUInt16;        /* 16 bit unsigned */
typedef int                              TTKInt32;         /* 32 bit signed */
typedef unsigned int                     TTKUInt32;        /* 32 bit unsigned */
typedef long long                        TTKInt64;         /* 64 bit signed */
typedef unsigned long long               TTKUInt64;        /* 64 bit unsigned */

typedef double                           TTKDouble;        /* double */
typedef float                            TTKFloat;         /* float */
typedef bool                             TTKBool;          /* bool */

// C++ style format
typedef std::string                      TTKString;        /* string */
typedef std::wstring                     TTKWString;       /* wstring */

// Qt style format
typedef QList<int>                       TTKIntList;       /* intlist */
typedef QList<QVariant>                  TTKVariantList;   /* variantlist */
typedef QSet<int>                        TTKIntSet;        /* intset */
typedef QMap<QString, QVariant>          TTKVariantMap;    /* stringVariantMap */
typedef QMap<QString, QString>           TTKStringMap;     /* stringsMap */
typedef QMap<QString, QStringList>       TTKStringListMap; /* stringStrlistsMap */
typedef QMap<int, TTKIntList>            TTKIntListMap;    /* intIntlistMap */
typedef QMap<qint64, QString>            TTKIntStringMap;  /* intStrMap */

typedef QMapIterator<QString, QString>   TTKStringMapIter;      /* stringsMapIterator */
typedef QMapIterator<qint64, QString>    TTKIntStringMapIter;   /* intStrMapIterator */

#endif // TTKGLOBAL_H