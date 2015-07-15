#ifndef GLOBALS_H
#define GLOBALS_H
#include <QDebug>
#include <QString>
#include <QObject>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define MAXPATHLEN 256
#define DEFAULT_BUFFER_SIZE 1048576

template<typename T>
T implicit_cast(T t){
	return t;
}

/*
 * Concatenate preprocessor tokens A and B without expanding macro definitions
 * (however, if invoked from a macro, macro arguments are expanded).
 */
#define PPCAT_NX(A, B) A ## B

/*
 * Concatenate preprocessor tokens A and B after macro-expanding them.
 */
#define PPCAT(A, B) PPCAT_NX(A, B)

/*
 * Turn A into a string literal without expanding macro definitions
 * (however, if invoked from a macro, macro arguments are expanded).
 */
#define STRINGIZE_NX(A) #A

/*
 * Turn A into a string literal after macro-expanding it.
 */
#define STRINGIZE(A) STRINGIZE_NX(A)

QString loadModule(QString path);

#include "exceptions.h"
#endif // GLOBALS_H
