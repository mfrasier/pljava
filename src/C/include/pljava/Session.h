/*
 * Copyright (c) 2004, 2005 TADA AB - Taby Sweden
 * Distributed under the terms shown in the file COPYRIGHT
 * found in the root folder of this project or at
 * http://eng.tada.se/osprojects/COPYRIGHT.html
 *
 * @author Thomas Hallgren
 */
#ifndef __pljava_Session_h
#define __pljava_Session_h

#include "pljava/PgObject.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************
 * Session related stuff.
 * 
 * @author Thomas Hallgren
 *
 ***********************************************************************/

extern Datum Session_initialize(PG_FUNCTION_ARGS);

#ifdef __cplusplus
} /* end of extern "C" declaration */
#endif
#endif