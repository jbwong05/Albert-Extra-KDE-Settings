// Copyright (C) 2014-2018 Manuel Schneider

#pragma once

#if defined(_MSC_VER)
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
#else
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef XDG_DEF
 #define EXPORT_XDG EXPORT
#else
 #define EXPORT_XDG IMPORT
#endif
