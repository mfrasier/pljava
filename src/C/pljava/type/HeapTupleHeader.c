/*
 * Copyright (c) 2003, 2004 TADA AB - Taby Sweden
 * Distributed under the terms shown in the file COPYRIGHT.
 * 
 * @author Thomas Hallgren
 */
#include "pljava/type/Type_priv.h"
#include "pljava/type/HeapTupleHeader.h"

#if (PGSQL_MAJOR_VER == 7 && PGSQL_MINOR_VER < 5)
jobject HeapTupleHeader_create(JNIEnv* env, HeapTupleHeader ht)
{
	return 0;
}
extern Datum HeapTupleHeader_initialize(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(HeapTupleHeader_initialize);
Datum HeapTupleHeader_initialize(PG_FUNCTION_ARGS)
{
	PG_RETURN_VOID();
}
#else

#include <executor/spi.h>
#include <utils/typcache.h>

#include "pljava/Exception.h"
#include "pljava/type/TupleDesc.h"
#include "pljava/type/HeapTupleHeader_JNI.h"

static Type      s_HeapTupleHeader;
static TypeClass s_HeapTupleHeaderClass;
static jclass    s_HeapTupleHeader_class;
static jmethodID s_HeapTupleHeader_init;

/*
 * org.postgresql.pljava.type.Tuple type.
 */
jobject HeapTupleHeader_create(JNIEnv* env, HeapTupleHeader ht)
{
	jobject jht;
	if(ht == 0)
		return 0;

	jht = NativeStruct_obtain(env, ht);
	if(jht == 0)
	{
		jht = PgObject_newJavaObject(env, s_HeapTupleHeader_class, s_HeapTupleHeader_init);
		NativeStruct_init(env, jht, ht);
	}
	return jht;
}

static jvalue _HeapTupleHeader_coerceDatum(Type self, JNIEnv* env, Datum arg)
{
	jvalue result;
	result.l = HeapTupleHeader_create(env, (HeapTupleHeader)DatumGetPointer(arg));
	return result;
}

static Type HeapTupleHeader_obtain(Oid typeId)
{
	return s_HeapTupleHeader;
}

/* Make this datatype available to the postgres system.
 */
extern Datum HeapTupleHeader_initialize(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(HeapTupleHeader_initialize);
Datum HeapTupleHeader_initialize(PG_FUNCTION_ARGS)
{
	JNINativeMethod methods[] = {
		{
		"_getObject",
	  	"(I)Ljava/lang/Object;",
	  	Java_org_postgresql_pljava_internal_HeapTupleHeader__1getObject
		},
		{
		"_getTupleDesc",
		"()Lorg/postgresql/pljava/internal/TupleDesc;",
		Java_org_postgresql_pljava_internal_HeapTupleHeader__1getTupleDesc
		},
		{ 0, 0, 0 }};

	JNIEnv* env = (JNIEnv*)PG_GETARG_POINTER(0);

	s_HeapTupleHeader_class = (*env)->NewGlobalRef(
				env, PgObject_getJavaClass(env, "org/postgresql/pljava/internal/HeapTupleHeader"));

	PgObject_registerNatives2(env, s_HeapTupleHeader_class, methods);

	s_HeapTupleHeader_init = PgObject_getJavaMethod(
				env, s_HeapTupleHeader_class, "<init>", "()V");

	s_HeapTupleHeaderClass = NativeStructClass_alloc("type.Tuple");
	s_HeapTupleHeaderClass->JNISignature   = "Lorg/postgresql/pljava/internal/HeapTupleHeader;";
	s_HeapTupleHeaderClass->javaTypeName   = "org.postgresql.pljava.internal.HeapTupleHeader";
	s_HeapTupleHeaderClass->coerceDatum    = _HeapTupleHeader_coerceDatum;
	s_HeapTupleHeader = TypeClass_allocInstance(s_HeapTupleHeaderClass, InvalidOid);

	Type_registerJavaType("org.postgresql.pljava.internal.HeapTupleHeader", HeapTupleHeader_obtain);
	PG_RETURN_VOID();
}

/****************************************
 * JNI methods
 ****************************************/
 
/*
 * Class:     org_postgresql_pljava_internal_HeapTupleHeader
 * Method:    _getObject
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_postgresql_pljava_internal_HeapTupleHeader__1getObject(JNIEnv* env, jobject _this, jint attrNo)
{
	HeapTupleHeader self;
	jobject result = 0;

	PLJAVA_ENTRY_FENCE(0)
	self = (HeapTupleHeader)NativeStruct_getStruct(env, _this);
	if(self == 0)
		return 0;

	PG_TRY();
	{
		TupleDesc tupleDesc = lookup_rowtype_tupdesc(
					HeapTupleHeaderGetTypeId(self),
					HeapTupleHeaderGetTypMod(self));

		Oid typeId = SPI_gettypeid(tupleDesc, (int)attrNo);
		if(!OidIsValid(typeId))
		{
			Exception_throw(env,
				ERRCODE_INVALID_DESCRIPTOR_INDEX,
				"Invalid attribute number \"%d\"", (int)attrNo);
		}
		else
		{
			Datum binVal;
			bool wasNull = false;
			Type type = Type_fromOid(typeId);
			if(Type_isPrimitive(type))
				/*
				 * This is a primitive type
				 */
				type = type->m_class->objectType;

			binVal = GetAttributeByNum(self, (AttrNumber)attrNo, &wasNull);
			if(!wasNull)
				result = Type_coerceDatum(type, env, binVal).l;
		}
	}
	PG_CATCH();
	{
		Exception_throw_ERROR(env, "GetAttributeByNum");
	}
	PG_END_TRY();
	return result;
}

/*
 * Class:     org_postgresql_pljava_internal_HeapTupleHeader
 * Method:    _getTupleDesc
 * Signature: ()Lorg/postgresql/pljava/internal/TupleDesc;
 */
JNIEXPORT jobject JNICALL
Java_org_postgresql_pljava_internal_HeapTupleHeader__1getTupleDesc(JNIEnv* env, jobject _this)
{
	HeapTupleHeader self;
	PLJAVA_ENTRY_FENCE(0)
	self = (HeapTupleHeader)NativeStruct_getStruct(env, _this);
	if(self == 0)
		return 0;

	return TupleDesc_create(env, lookup_rowtype_tupdesc(
					HeapTupleHeaderGetTypeId(self),
					HeapTupleHeaderGetTypMod(self)));
}
#endif
