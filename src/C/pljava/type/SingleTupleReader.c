/*
 * This file contains software that has been made available under The BSD
 * license. Use and distribution hereof are subject to the restrictions set
 * forth therein.
 * 
 * Copyright (c) 2003 TADA AB - Taby Sweden
 * All Rights Reserved
 */
#include "pljava/type/Type_priv.h"

#if (PGSQL_MAJOR_VER == 7 && PGSQL_MINOR_VER < 5)

extern Datum SingleTupleReader_initialize(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(SingleTupleReader_initialize);
Datum SingleTupleReader_initialize(PG_FUNCTION_ARGS)
{
	PG_RETURN_VOID();
}

#else

#include "pljava/type/Type_priv.h"
#include "pljava/type/HeapTupleHeader.h"

/*
 * void primitive type.
 */
static jclass s_SingleTupleReader_class;
static jmethodID s_SingleTupleReader_init;
static TypeClass s_SingleTupleReaderClass;
static Type s_SingleTupleReader;

static jvalue _SingleTupleReader_coerceDatum(Type self, JNIEnv* env, Datum arg)
{
	jvalue result;
	jobject ttSlot = HeapTupleHeader_create(env, DatumGetHeapTupleHeader(arg));
	result.l = PgObject_newJavaObject(env, s_SingleTupleReader_class, s_SingleTupleReader_init, ttSlot);
	(*env)->DeleteLocalRef(env, ttSlot);
	return result;
}

static Datum _SingleTupleReader_coerceObject(Type self, JNIEnv* env, jobject nothing)
{
	/* Should never be used here.
	 */
	return 0;
}

static Type SingleTupleReader_obtain(Oid typeId)
{
	return s_SingleTupleReader;
}

/* Make this datatype available to the postgres system.
 */
extern Datum SingleTupleReader_initialize(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(SingleTupleReader_initialize);
Datum SingleTupleReader_initialize(PG_FUNCTION_ARGS)
{
	JNIEnv* env = (JNIEnv*)PG_GETARG_POINTER(0);

	s_SingleTupleReader_class = (*env)->NewGlobalRef(
				env, PgObject_getJavaClass(env, "org/postgresql/pljava/jdbc/SingleTupleReader"));

	s_SingleTupleReader_init = PgObject_getJavaMethod(
				env, s_SingleTupleReader_class, "<init>", "(Lorg/postgresql/pljava/internal/HeapTupleHeader;)V");

	s_SingleTupleReaderClass = TypeClass_alloc("type.SingleTupleReader");
	s_SingleTupleReaderClass->JNISignature = "Ljava/sql/ResultSet;";
	s_SingleTupleReaderClass->javaTypeName = "java.lang.ResultSet";
	s_SingleTupleReaderClass->coerceDatum  = _SingleTupleReader_coerceDatum;
	s_SingleTupleReaderClass->coerceObject = _SingleTupleReader_coerceObject;
	s_SingleTupleReader = TypeClass_allocInstance(s_SingleTupleReaderClass, InvalidOid);

	Type_registerJavaType("org.postgresql.pljava.jdbc.SingleTupleReader", SingleTupleReader_obtain);
	PG_RETURN_VOID();
}
#endif