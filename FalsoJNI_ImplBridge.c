/*
 * FalsoJNI_ImplBridge.c
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

#include "FalsoJNI_Impl.h"
#include "FalsoJNI_Logger.h"

#include "FalsoJNI_ImplBridge.h"

#include <string.h>
#include <malloc.h>
#include <pthread.h>

#include "converter.h"

#define JDA_MAGIC 0xB00B1E5

jfieldID getFieldIdByName(const char* name) {
    for (int i = 0; i < nameToFieldId_size() / sizeof(NameToFieldID); i++) {
        if (strcmp(name, nameToFieldId[i].name) == 0) {
            return (jfieldID) nameToFieldId[i].id;
        }
    }

    fjni_logv_warn("Unknown field name \"%s\"", name);
    return NULL;
}

const char* fieldTypeToStr(FIELD_TYPE t) {
    switch (t) {
        case FIELD_TYPE_INT:
            return "FIELD_TYPE_INT";
        case FIELD_TYPE_OBJECT:
            return "FIELD_TYPE_OBJECT";
        case FIELD_TYPE_BOOLEAN:
            return "FIELD_TYPE_BOOLEAN";
        case FIELD_TYPE_BYTE:
            return "FIELD_TYPE_BYTE";
        case FIELD_TYPE_CHAR:
            return "FIELD_TYPE_CHAR";
        case FIELD_TYPE_SHORT:
            return "FIELD_TYPE_SHORT";
        case FIELD_TYPE_LONG:
            return "FIELD_TYPE_LONG";
        case FIELD_TYPE_FLOAT:
            return "FIELD_TYPE_FLOAT";
        case FIELD_TYPE_DOUBLE:
            return "FIELD_TYPE_DOUBLE";
        default:
            return "FIELD_TYPE_UNKNOWN";
    }
}

jsize getFieldTypeSize(FIELD_TYPE fieldType) {
    switch (fieldType) {
        case FIELD_TYPE_OBJECT:
            return sizeof(jobject);
        case FIELD_TYPE_BOOLEAN:
            return sizeof(jboolean);
        case FIELD_TYPE_BYTE:
            return sizeof(jbyte);
        case FIELD_TYPE_CHAR:
            return sizeof(jchar);
        case FIELD_TYPE_SHORT:
            return sizeof(jshort);
        case FIELD_TYPE_INT:
            return sizeof(jint);
        case FIELD_TYPE_LONG:
            return sizeof(jlong);
        case FIELD_TYPE_FLOAT:
            return sizeof(jfloat);
        case FIELD_TYPE_DOUBLE:
            return sizeof(jdouble);
        default:
            return sizeof(void *);
    }
}

jobject getObjectFieldValueById(jfieldID id) {
    getFieldValueById(jobject, FIELD_TYPE_OBJECT, FieldsObject, fieldsObject, fieldsObject_size, id, (jobject)0x42424242);
}

jint getIntFieldValueById(jfieldID id) {
    getFieldValueById(jint, FIELD_TYPE_INT, FieldsInt, fieldsInt, fieldsInt_size, id, 1);
}

jboolean getBooleanFieldValueById(jfieldID id) {
    getFieldValueById(jboolean, FIELD_TYPE_BOOLEAN, FieldsBoolean, fieldsBoolean, fieldsBoolean_size, id, JNI_FALSE);
}

jbyte getByteFieldValueById(jfieldID id) {
    getFieldValueById(jbyte, FIELD_TYPE_BYTE, FieldsByte, fieldsByte, fieldsByte_size, id, 'a');
}

jchar getCharFieldValueById(jfieldID id) {
    getFieldValueById(jchar, FIELD_TYPE_CHAR, FieldsChar, fieldsChar, fieldsChar_size, id, 'b');
}

jshort getShortFieldValueById(jfieldID id) {
    getFieldValueById(jshort, FIELD_TYPE_SHORT, FieldsShort, fieldsShort, fieldsShort_size, id, 1);
}

jlong getLongFieldValueById(jfieldID id) {
    getFieldValueById(jlong, FIELD_TYPE_LONG, FieldsLong, fieldsLong, fieldsLong_size, id, 1);
}

jfloat getFloatFieldValueById(jfieldID id) {
    getFieldValueById(jfloat, FIELD_TYPE_FLOAT, FieldsFloat, fieldsFloat, fieldsFloat_size, id, 1.0f);
}

jdouble getDoubleFieldValueById(jfieldID id) {
    getFieldValueById(jdouble, FIELD_TYPE_DOUBLE, FieldsDouble, fieldsDouble, fieldsDouble_size, id, 1);
}

void setObjectFieldValueById(jfieldID id, jobject value) {
    setFieldValueById(jobject, FIELD_TYPE_OBJECT, FieldsObject, fieldsObject, fieldsObject_size, id, value);
}

void setIntFieldValueById(jfieldID id, jint value) {
    setFieldValueById(jint, FIELD_TYPE_INT, FieldsInt, fieldsInt, fieldsInt_size, id, value);
}

void setBooleanFieldValueById(jfieldID id, jboolean value) {
    setFieldValueById(jboolean, FIELD_TYPE_BOOLEAN, FieldsBoolean, fieldsBoolean, fieldsBoolean_size, id, value);
}

void setByteFieldValueById(jfieldID id, jbyte value) {
    setFieldValueById(jbyte, FIELD_TYPE_BYTE, FieldsByte, fieldsByte, fieldsByte_size, id, value);
}

void setCharFieldValueById(jfieldID id, jchar value) {
    setFieldValueById(jchar, FIELD_TYPE_CHAR, FieldsChar, fieldsChar, fieldsChar_size, id, value);
}

void setShortFieldValueById(jfieldID id, jshort value) {
    setFieldValueById(jshort, FIELD_TYPE_SHORT, FieldsShort, fieldsShort, fieldsShort_size, id, value);
}

void setLongFieldValueById(jfieldID id, jlong value) {
    setFieldValueById(jlong, FIELD_TYPE_LONG, FieldsLong, fieldsLong, fieldsLong_size, id, value);
}

void setFloatFieldValueById(jfieldID id, jfloat value) {
    setFieldValueById(jfloat, FIELD_TYPE_FLOAT, FieldsFloat, fieldsFloat, fieldsFloat_size, id, value);
}

void setDoubleFieldValueById(jfieldID id, jdouble value) {
    setFieldValueById(jdouble, FIELD_TYPE_DOUBLE, FieldsDouble, fieldsDouble, fieldsDouble_size, id, value);
}

jmethodID getMethodIdByName(const char* name) {
    for (int i = 0; i < nameToMethodId_size() / sizeof(NameToMethodID); i++) {
        if (strcmp(name, nameToMethodId[i].name) == 0) {
            return (jmethodID) nameToMethodId[i].id;
        }
    }
    return NULL;
}

jobject methodObjectCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsObject_size() / sizeof(MethodsObject); i++) {
        if (methodsObject[i].id == (int)id) {
            return methodsObject[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return NULL;
}

void methodVoidCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsVoid_size() / sizeof(MethodsVoid); i++) {
        if (methodsVoid[i].id == (int)id) {
            return methodsVoid[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
}

jboolean methodBooleanCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsBoolean_size() / sizeof(MethodsBoolean); i++) {
        if (methodsBoolean[i].id == (int)id) {
            return methodsBoolean[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return JNI_FALSE;
}

jbyte methodByteCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsByte_size() / sizeof(MethodsByte); i++) {
        if (methodsByte[i].id == (int)id) {
            return methodsByte[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return 0;
}

jshort methodShortCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsShort_size() / sizeof(MethodsShort); i++) {
        if (methodsShort[i].id == (int)id) {
            return methodsShort[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return 0;
}

jdouble methodDoubleCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsDouble_size() / sizeof(MethodsDouble); i++) {
        if (methodsDouble[i].id == (int)id) {
            return methodsDouble[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return 0;
}

jchar methodCharCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsChar_size() / sizeof(MethodsChar); i++) {
        if (methodsChar[i].id == (int)id) {
            return methodsChar[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return 0;
}

jlong methodLongCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsLong_size() / sizeof(MethodsLong); i++) {
        if (methodsLong[i].id == (int)id) {
            return methodsLong[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return -1;
}

jint methodIntCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsInt_size() / sizeof(MethodsInt); i++) {
        if (methodsInt[i].id == (int)id) {
            return methodsInt[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return -1;
}

jfloat methodFloatCall(jmethodID id, va_list args) {
    for (int i = 0; i < methodsFloat_size() / sizeof(MethodsFloat); i++) {
        if (methodsFloat[i].id == (int)id) {
            return methodsFloat[i].Method(id, args);
        }
    }

    fjni_logv_warn("method ID %i not found!", (int)id);
    return -1;
}

JavaDynArray * jda_alloc(jsize len, FIELD_TYPE type) {
    void * array = malloc(len * getFieldTypeSize(type));
    if (!array) {
        return NULL;
    }

    JavaDynArray * ret = malloc(sizeof(JavaDynArray));
    if (!ret) {
        free(array);
        return NULL;
    }

    ret->magic = JDA_MAGIC;
    ret->array = array;
    ret->len = len;
    ret->type = type;

    return ret;
}

jsize jda_sizeof(JavaDynArray * jda) {
    if (!jda)
        return -1;

    return jda->len;
}

jboolean jda_realloc(JavaDynArray * jda, jsize len) {
    if (!jda)
        return JNI_FALSE;

    void * res = realloc(jda->array, len * getFieldTypeSize(jda->type));
    if (res == NULL) {
        return JNI_FALSE;
    }

    jda->array = res;

    return JNI_TRUE;
}

jboolean jda_free(JavaDynArray * jda) {
    if (!jda)
        return JNI_FALSE;

    if (jda->magic != JDA_MAGIC)
        return JNI_FALSE;

    free(jda->array);
    free(jda);

    return JNI_TRUE;
}

jboolean jstr_utf16_to_utf8(JavaString * jstr) {
    if (!jstr) return JNI_FALSE;

    if (jstr->utf8 == NULL) {
        jstr->utf8 = jda_alloc(jstr->utf16->len+1, FIELD_TYPE_BYTE);
        if (jstr->utf8 == NULL) {
            return JNI_FALSE;
        }
    } else if (jstr->utf8->len < jstr->utf16->len+1) {
        if (jda_realloc(jstr->utf8, jstr->utf16->len+1) == JNI_FALSE) {
            return JNI_FALSE;
        }
    }

    utf16_to_utf8(jstr->utf16->array, jstr->utf16->len, jstr->utf8->array, jstr->utf8->len);

    char * arr = jstr->utf8->array;
    arr[jstr->utf8->len - 1] = '\0';
    return JNI_TRUE;
}

jboolean jstr_utf8_to_utf16(JavaString * jstr) {
    if (!jstr) return JNI_FALSE;

    if (jstr->utf8 == NULL) {
        return JNI_FALSE;
    }

    if (jstr->utf16->len + 1 < jstr->utf8->len) {
        if (jda_realloc(jstr->utf16, jstr->utf8->len - 1) == JNI_FALSE) {
            return JNI_FALSE;
        }
    }

    utf8_to_utf16(jstr->utf8->array, jstr->utf8->len - 1, jstr->utf16->array, jstr->utf16->len);

    return JNI_TRUE;
}

va_list _AtoV(int dummy, ...) {
    va_list args1;
    va_start(args1, dummy);
    va_list args2;
    va_copy(args2, args1);
    va_end(args1);
    return args2;
}

/*
 * Method signatures, recorded by GetMethodID / GetStaticMethodID for
 * fjni_jvalues_to_va_list(). A fixed table filled with atomic operations:
 * an entry's id is published after its signature, so a concurrent reader
 * either skips the entry or sees it complete.
 */
#define FJNI_MAX_SIGNATURES 1024

static struct {
    volatile int id;
    const char * volatile sig;
} method_signatures[FJNI_MAX_SIGNATURES];
static volatile int method_signatures_count = 0;

static int find_signature_slot(jmethodID id) {
    int n = method_signatures_count;
    if (n > FJNI_MAX_SIGNATURES) n = FJNI_MAX_SIGNATURES;
    for (int i = 0; i < n; i++) {
        if (method_signatures[i].id == (int)id)
            return i;
    }
    return -1;
}

void fjni_remember_signature(jmethodID id, const char *sig) {
    if (!id || !sig)
        return;

    int i = find_signature_slot(id);
    if (i >= 0) {
        if (strcmp(method_signatures[i].sig, sig) != 0)
            method_signatures[i].sig = strdup(sig); // overload: the last signature wins
        return;
    }

    i = __sync_fetch_and_add(&method_signatures_count, 1);
    if (i >= FJNI_MAX_SIGNATURES) {
        fjni_logv_err("signature table full (%i), Call*MethodA cannot pass the arguments of method ID %i",
                      FJNI_MAX_SIGNATURES, (int)id);
        return;
    }
    method_signatures[i].sig = strdup(sig);
    __sync_synchronize();
    method_signatures[i].id = (int)id;
}

#if !defined(__arm__) || defined(__aarch64__)
#error "fjni_jvalues_to_va_list() builds an ARM AAPCS va_list; port it before using another ABI"
#endif

void fjni_jvalues_to_va_list(jmethodID id, const jvalue *args, uint64_t *area, va_list *out) {
    _Static_assert(sizeof(va_list) == sizeof(void *), "the AAPCS va_list is a single pointer");

    uint8_t *base = (uint8_t *)area;
    size_t off = 0;
    int slot = find_signature_slot(id);
    const char *sig = slot >= 0 ? method_signatures[slot].sig : NULL;

    memset(area, 0, FJNI_A_ARGS_MAX_BYTES);

    if (!sig || sig[0] != '(') {
        // The method reads zeros instead of the arguments.
        fjni_logv_err("no signature recorded for method ID %i, its arguments are not passed", (int)id);
        sig = "()";
    }

    const char *p = sig + 1;
    for (int n = 0; *p && *p != ')'; n++) {
        char type;
        int array = 0;

        while (*p == '[') {
            array = 1;
            p++;
        }
        type = *p;
        if (type == 'L') {
            p = strchr(p, ';');
            if (!p) {
                fjni_logv_err("malformed signature \"%s\" (method ID %i)", sig, (int)id);
                break;
            }
        }
        if (*p) p++;
        if (array) type = 'L'; // arrays travel as jobject references

        size_t size = (type == 'J' || type == 'D' || type == 'F') ? 8 : 4;
        off = (off + size - 1) & ~(size - 1);
        if (off + size > FJNI_A_ARGS_MAX_BYTES) {
            fjni_logv_err("arguments of method ID %i exceed %i bytes, truncated", (int)id, FJNI_A_ARGS_MAX_BYTES);
            break;
        }

        void *dst = base + off;
        switch (type) {
            case 'Z': *(int32_t *)dst = args[n].z; break;
            case 'B': *(int32_t *)dst = args[n].b; break;
            case 'C': *(int32_t *)dst = args[n].c; break;
            case 'S': *(int32_t *)dst = args[n].s; break;
            case 'I': *(int32_t *)dst = args[n].i; break;
            case 'J': *(int64_t *)dst = args[n].j; break;
            case 'F': *(double *)dst = (double)args[n].f; break;
            case 'D': *(double *)dst = args[n].d; break;
            case 'L': *(jobject *)dst = args[n].l; break;
            default:
                fjni_logv_err("unknown type '%c' in signature \"%s\" (method ID %i)", type, sig, (int)id);
                *(int32_t *)dst = args[n].i;
                break;
        }
        off += size;
    }

    void *ap = base;
    memcpy(out, &ap, sizeof(ap));
}
