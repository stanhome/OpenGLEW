#pragma once

#include <stdio.h>


#define XLOG(fmt, ...) Log::instance()->i(fmt, ##__VA_ARGS__)
#define XLOGD(fmt, ...) Log::instance()->d(fmt, ##__VA_ARGS__)
#define XLOGE(fmt, ...) Log::instance()->e(fmt, ##__VA_ARGS__)

class Log {
protected:
	Log() {
		init("out.log");
	}

	Log(const Log &copy){}
    Log &operator=(const Log &rhs){ return *this;}

public:
	static Log *instance() {
		static Log sInstance;
		return &sInstance;
	}

	bool init(const char *filename);

	void newLine();
	void i(const char *txt, ...);
	void e(const char *txt, ...);
	void d(const char *txt, ...);

protected:
	void p(const char *txt, const char *tag, va_list argList);

protected:
	char *_filename;
	FILE *_logFile; // only open when writing.
};
