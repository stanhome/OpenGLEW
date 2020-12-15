#pragma once

#include <string.h>
#include <stdarg.h>
#include "log.h"


bool Log::init(const char *fn) {
	if (_filename) {
		delete[] _filename;
	}

	_filename = new char[strlen(fn)];
	if (!_filename) return false;

	strcpy(_filename, fn);

	// open file, overwriting an previous contents.
	_logFile = fopen(_filename, "wb");
	if (!_logFile) return false;

	fclose(_logFile);
	_logFile = nullptr;

	return true;
}

void Log::newLine() {
	_logFile = fopen(_filename, "a+");
	if (!_logFile) return;

	putc('\n', _logFile);

	fclose(_logFile);
	_logFile = nullptr;
}

void Log::d(const char *txt, ...) {
	va_list argList;
	va_start(argList, txt);

	p(txt, "D", argList);

	va_end(argList);
}

void Log::i(const char *txt, ...) {
	va_list argList;
	va_start(argList, txt);

	p(txt, "I", argList);

	va_end(argList);
}

void Log::e(const char *txt, ...) {
	va_list argList;
	va_start(argList, txt);

	p(txt, "E", argList);

	va_end(argList);
}

void Log::p(const char *txt, const char *tag, va_list argList) {
	// open file to append
	_logFile = fopen(_filename, "a+");
	if (!_logFile) return;

	// write the log content
	fprintf(_logFile, "[%s]", tag);
	vfprintf(_logFile, txt, argList);
	putc('\n', _logFile);

	fclose(_logFile);
	_logFile = nullptr;
}

