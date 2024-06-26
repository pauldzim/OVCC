/*****************************************************************************/
/**
	xDebug.c
 
	uEng - micro cross platform engine
	Copyright 2010 by Wes Gale All rights reserved.
	Used by permission in VccX
*/
/*****************************************************************************/

/*
	System headers
*/ 
//#include <ucontext.h>
//#include <unistd.h>
//#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "defines.h"

#include "xdebug.h"

/*****************************************************************************/

int	g_bTraceMessageLogging	= TRUE;
//int	g_bTraceMessageLogFile	= FALSE;

/*****************************************************************************/

#ifdef _DEBUG
#if 0
void _xDbgTrace(const void * pFile, const int iLine, const void * pFormat, ...)
{
	va_list		args;
	
	va_start(args,pFormat);
	
	fprintf(stdout,"%s(%d) : ", (char *)pFile, iLine);

	vfprintf(stdout,(char *)pFormat,args);
	
	va_end(args);
	
	fflush(stdout);
}
#else
void _xDbgTrace(const void * pFile, const int iLine, const void * pFormat, ...)
{
	va_list		args;
	char temp[1024];

#ifdef DARWIN
	if (logg == NULL)
		return;
#endif

	va_start(args, pFormat);

	sprintf(temp, "%s(%d) : ", (char *)pFile, iLine);
#ifdef DARWIN
	fprintf(logg, "%s", temp);
#else
	fprintf(stderr, "%s", temp);
#endif

	vsprintf(temp, (char *)pFormat, args);
#ifdef DARWIN
	fprintf(logg, "%s", temp);
#else
	fprintf(stderr, "%s", temp);
#endif

	va_end(args);

//	fflush(stdout);
}
#endif
#endif

/*****************************************************************************/

