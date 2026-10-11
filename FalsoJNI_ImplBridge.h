/*
 * FalsoJNI_ImplBridge.h
 *
 * Fake Java Native Interface, providing JavaVM and JNIEnv objects.
 *
 * Copyright (C) 2021 Andy Nguyen
 * Copyright (C) 2021 Rinnegatamante
 * Copyright (C) 2022 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#ifndef FALSOJNI_IMPL_BRIDGE
#define FALSOJNI_IMPL_BRIDGE

#include <stddef.h>
#include <stdint.h>
#include "jni.h"

/*
 * Type definitions for Fields
 */

typedef enum FIELD_TYPE {
    FIELD_TYPE_UNKNOWN   = 0,
    FIELD_TYPE_OBJECT    = 1,
    FIELD_TYPE_BOOLEAN   = 2,
    FIELD_TYPE_BYTE      = 3,
    FIELD_TYPE_CHAR      = 4,
    FIELD_TYPE_SHORT     = 5,
    FIELD_TYPE_INT       = 6,
    FIELD_TYPE_LONG      = 7,
    FIELD_TYPE_FLOAT     = 8,
    FIELD_TYPE_DOUBLE    = 9
} FIELD_TYPE;

typedef struct {
    int id;
    char *name;
    FIELD_TYPE f;
} NameToFieldID;

typedef struct { int id; jobject value; }   FieldsObject;
typedef struct { int id; jboolean value; }  FieldsBoolean;
typedef struct { int id; jbyte value; }     FieldsByte;
typedef struct { int id; jchar value; }     FieldsChar;
typedef struct { int id; jshort value; }    FieldsShort;
typedef struct { int id; jint value; }      FieldsInt;
typedef struct { int id; jlong value; }     FieldsLong;
typedef struct { int id; jfloat value; }    FieldsFloat;
typedef struct { int id; jdouble value; }   FieldsDouble;

jfieldID    getFieldIdByName(const char* name);
jsize       getFieldTypeSize(FIELD_TYPE fieldType);

jobject     getObjectFieldValueById(jfieldID id);
jboolean    getBooleanFieldValueById(jfieldID id);
jbyte       getByteFieldValueById(jfieldID id);
jchar       getCharFieldValueById(jfieldID id);
jshort      getShortFieldValueById(jfieldID id);
jint        getIntFieldValueById(jfieldID id);
jlong       getLongFieldValueById(jfieldID id);
jfloat      getFloatFieldValueById(jfieldID id);
jdouble     getDoubleFieldValueById(jfieldID id);

void        setObjectFieldValueById(jfieldID id, jobject value);
void        setBooleanFieldValueById(jfieldID id, jboolean value);
void        setByteFieldValueById(jfieldID id, jbyte value);
void        setCharFieldValueById(jfieldID id, jchar value);
void        setShortFieldValueById(jfieldID id, jshort value);
void        setIntFieldValueById(jfieldID id, jint value);
void        setLongFieldValueById(jfieldID id, jlong value);
void        setFloatFieldValueById(jfieldID id, jfloat value);
void        setDoubleFieldValueById(jfieldID id, jdouble value);

/*
 * Type definitions for Methods
 */

typedef enum METHOD_TYPE {
    METHOD_TYPE_UNKNOWN   = 0,
    METHOD_TYPE_VOID      = 1,
    METHOD_TYPE_OBJECT    = 2,
    METHOD_TYPE_BOOLEAN   = 3,
    METHOD_TYPE_BYTE      = 4,
    METHOD_TYPE_CHAR      = 5,
    METHOD_TYPE_SHORT     = 6,
    METHOD_TYPE_INT       = 7,
    METHOD_TYPE_LONG      = 8,
    METHOD_TYPE_FLOAT     = 9,
    METHOD_TYPE_DOUBLE    = 10
} METHOD_TYPE;

typedef struct {
    int id;
    char *name;
    METHOD_TYPE f;
} NameToMethodID;

typedef struct { int id; void (*Method)(jmethodID id, va_list args); }      MethodsVoid;
typedef struct { int id; jobject (*Method)(jmethodID id, va_list args); }   MethodsObject;
typedef struct { int id; jboolean (*Method)(jmethodID id, va_list args); }  MethodsBoolean;
typedef struct { int id; jbyte (*Method)(jmethodID id, va_list args); }     MethodsByte;
typedef struct { int id; jchar (*Method)(jmethodID id, va_list args); }     MethodsChar;
typedef struct { int id; jshort (*Method)(jmethodID id, va_list args); }    MethodsShort;
typedef struct { int id; jint (*Method)(jmethodID id, va_list args); }      MethodsInt;
typedef struct { int id; jlong (*Method)(jmethodID id, va_list args); }     MethodsLong;
typedef struct { int id; jfloat (*Method)(jmethodID id, va_list args); }    MethodsFloat;
typedef struct { int id; jdouble (*Method)(jmethodID id, va_list args); }   MethodsDouble;

jmethodID   getMethodIdByName(const char* name);

void        methodVoidCall(jmethodID id, va_list args);
jobject     methodObjectCall(jmethodID id, va_list args);
jboolean    methodBooleanCall(jmethodID id, va_list args);
jbyte       methodByteCall(jmethodID id, va_list args);
jchar       methodCharCall(jmethodID id, va_list args);
jshort      methodShortCall(jmethodID id, va_list args);
jint        methodIntCall(jmethodID id, va_list args);
jlong       methodLongCall(jmethodID id, va_list args);
jfloat      methodFloatCall(jmethodID id, va_list args);
jdouble     methodDoubleCall(jmethodID id, va_list args);

/*
 * Dynamically allocated arrays
 */

typedef struct {
    uint32_t    magic;
    jarray      array;
    jsize       len;
    FIELD_TYPE  type;
} JavaDynArray;

JavaDynArray * jda_alloc(jsize len, FIELD_TYPE type);
jsize          jda_sizeof(JavaDynArray * jda);
jboolean       jda_free(JavaDynArray * jda);

/*
 * Internal string storage and manipulation
 */

typedef struct {
    JavaDynArray * utf16;
    JavaDynArray * utf8;
} JavaString;

jboolean jstr_utf16_to_utf8(JavaString * jstr);
jboolean jstr_utf8_to_utf16(JavaString * jstr);

/*
 * Helper macros / functions
 */

/*
 * Arguments of the Call*MethodA / NewObjectA variants.
 *
 * The A variants receive their arguments as a jvalue array, but method
 * implementations read them with va_arg. A function cannot build a va_list
 * out of an array, so FJNI_A_ARGS lays out, in a buffer that lives in the
 * caller's frame, the bytes a variadic call would have produced, and points
 * a va_list at them.
 *
 * The layout is the one va_arg walks under the ARM procedure call standard
 * (AAPCS): every argument goes through the default promotions (jboolean,
 * jbyte, jchar and jshort become int, jfloat becomes double) and takes a
 * 4-byte slot; 64-bit values (jlong, jdouble and the promoted jfloat) start
 * on an 8-byte boundary. Argument types come from the signature passed to
 * GetMethodID / GetStaticMethodID, recorded per method ID. Methods are
 * resolved by name, so overloads share an ID and the last signature wins.
 *
 * Implementations read A-variant arguments exactly like those of the ... and
 * V variants: va_arg(args, int) for Z, B, C, S and I, va_arg(args, jlong) for
 * J, va_arg(args, double) for F and D, va_arg(args, jobject) for objects and
 * arrays.
 */
#define FJNI_A_ARGS_MAX_BYTES 512

void fjni_remember_signature(jmethodID id, const char *sig);
void fjni_jvalues_to_va_list(jmethodID id, const jvalue *args, uint64_t *area, va_list *out);

#define FJNI_A_ARGS(va, id, args) \
    uint64_t va##_area[FJNI_A_ARGS_MAX_BYTES / sizeof(uint64_t)]; \
    va_list va; \
    fjni_jvalues_to_va_list((id), (args), va##_area, &va)

/*
 * Deprecated: returns a va_list that points into _AtoV's own stack frame,
 * already released when the caller uses it, and that holds the jvalue pointer
 * rather than the values. Kept so that existing code still compiles; use
 * FJNI_A_ARGS instead.
 */
va_list _AtoV(int dummy, ...);

#define getFieldValueById(jtype, fieldtype, containertype, container, containersize, id, defaultval) ({ \
  for (int i = 0; i < nameToFieldId_size() / sizeof(NameToFieldID); i++) { \
    if (nameToFieldId[i].id == (int)(id)) { \
        if (nameToFieldId[i].f != (fieldtype)) { \
            fjni_logv_err("Field type mismatch for field #%i: expected %s, found %s", (int)id, fieldTypeToStr(fieldtype), fieldTypeToStr(nameToFieldId[i].f)); \
            return defaultval; \
        } \
         \
        const jtype * x = NULL; \
        for (int u = 0; u < containersize() / sizeof(containertype); u++) { \
            if (container[u].id == (int)id) { \
                x = &container[u].value; \
            } \
        } \
        \
        if (!x) { \
            fjni_logv_err("Field #%i is defined in NameToFieldID table but has no value set", (int)id); \
            return defaultval; \
        } \
         \
        return *x; \
    } \
  } \
  fjni_logv_err("Undefined fieldID #%i", (int)id); \
  return defaultval; \
})

#define setFieldValueById(jtype, fieldtype, containertype, container, containersize, id, value) ({ \
  for (int i = 0; i < nameToFieldId_size() / sizeof(NameToFieldID); i++) { \
    if (nameToFieldId[i].id == (int)(id)) { \
        if (nameToFieldId[i].f != (fieldtype)) { \
            fjni_logv_err("Field type mismatch for field #%i: expected %s, found %s", (int)(id), fieldTypeToStr(fieldtype), fieldTypeToStr(nameToFieldId[i].f)); \
            return; \
        } \
         \
        jtype * x = NULL; \
        for (int u = 0; u < containersize() / sizeof(containertype); u++) { \
            if ((container)[u].id == (int)(id)) { \
                x = &(container)[u].value; \
            } \
        } \
        \
        if (!x) { \
            fjni_logv_err("Field #%i is defined in NameToFieldID table but has no value set", (int)(id)); \
            return; \
        } \
        \
        *x = value; \
    } \
  } \
  fjni_logv_err("Undefined fieldID #%i", (int)(id)); \
  return; \
})

#define GetPrimitiveArrayRegion(fun_name, fieldType, jType, array, start, length, buffer) ({ \
    JavaDynArray * jda = (JavaDynArray *) array; \
    if (!jda) { \
        fjni_logv_err("[JNI] %s(env, 0x%x, %i, %i, 0x%x): Array not found.", fun_name, (int)array, start, length, buffer); \
        return; \
    } \
     \
    if (start < 0 || length > jda->len) { \
        fjni_logv_err("[JNI] %s(env, 0x%x, %i, %i, 0x%x): Index out of bounds! (real length: %i)", fun_name, (int)array, start, length, buffer, jda->len); \
        return; \
    } \
     \
    fjni_logv_dbg("[JNI] %s(env, 0x%x, %i, %i, 0x%x)", fun_name, (int)array, start, length, buffer); \
     \
    if (!buffer) \
        buffer = (jType*) malloc(length); \
     \
    jType* arr = jda->array; \
    memcpy(buffer, &arr[start], length * getFieldTypeSize(fieldType));\
})

#define SetPrimitiveArrayRegion(fun_name, fieldType, jType, array, start, length, buffer) ({ \
    JavaDynArray * jda = (JavaDynArray *) array; \
    if (!jda) { \
        fjni_logv_err("[JNI] %s(env, 0x%x, %i, %i, 0x%x): Array not found!", fun_name, (int)array, start, length, buffer); \
        return; \
    } \
     \
    if (start < 0 || length > jda->len) { \
        fjni_logv_err("[JNI] %s(env, 0x%x, %i, %i, 0x%x): Index out of bounds! (real length: %i)", fun_name, (int)array, start, length, buffer, jda->len); \
        return; \
    } \
     \
    fjni_logv_dbg("[JNI] %s(env, 0x%x, %i, %i, 0x%x)", fun_name, (int)array, start, length, buffer); \
     \
    if (!buffer) \
        fjni_logv_warn("[JNI] %s(env, 0x%x, %i, %i, 0x%x): buffer is NULL", fun_name, (int)array, start, length, buffer); \
     \
    jType* arr = jda->array; \
    memcpy(&arr[start], buffer, length * getFieldTypeSize(fieldType));\
})

#endif // FALSOJNI_IMPL_BRIDGE
