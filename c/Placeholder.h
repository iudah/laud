/**
 * @file Placeholder.h
 *
 * This file contains declarations for the LaudPlaceholder data type and
 * associated functions.
 */
#ifndef PLACEHOLDER_H
#define PLACEHOLDER_H

#ifdef _WIN32

#ifdef LAUDEXPORTS
#define LAUDAPI __declspec(dllexport)
#else
#define LAUDAPI __declspec(dllimport)
#endif
#define LAUDCALL __cdecl

#else

#define LAUDAPI
#define LAUDCALL

#endif

LAUDAPI extern const void *LAUD_PLACEHOLDER;
#endif