#include <common.h>

typedef void *vm_context;
typedef uint64_t object_id;

// The first level of "un-indirection": common small immutable objects have
// their data stored in the object_id directly. This is called an "inline"
// object for want of a better term. They are technically also primitives.
// Their class IDs must fit in three bits and generally any class ID that can
// fit in 3 or less bits is reserved for them.
#define OCLS_ID     0b000
#define OCLS_SINT   0b001
#define OCLS_SSTR   0b010
#define OCLS_FLOAT  0b011
#define OCLS_BOOL   0b100
#define OCLS_PRIM   0b101

// Primitive object types are actually allocated, but don't follow the typical
// object format. They are used for things like strings, integers and classes
// which need some backing code in the implementation.
// They are usually used in the type feild of an object e.g. the type feild is
// set to something like MAKE_OBJID(OCLS_PRIM, OCLS_CLASS)
#define OCLS_STRING 0b1000 // Object is a LongString
#define OCLS_CLASS  0b1001 // Object is a Class

#define GET_OBJID_CLS(x) (x >> 61)
#define GET_OBJID_VAL(x) (x & 0x1fffffffffffffff)
#define MAKE_OBJID(t, v) ((t << 61) | (v & 0x1fffffffffffffff))
#define OBJID_SEXT(x) ((int64_t)(((x >> 60) & 1) ? (0xe000000000000000 | x) : x))

#define SSTR_SIZE(x) ((x >> 56) & 0b11111)
#define MAKE_SSTR1(c0) MAKE_OBJID(OCLS_SSTR, (1 << 56) | c0)
#define MAKE_SSTR2(c0, c1) MAKE_OBJID(OCLS_SSTR, (2 << 56) | (c1 << 8) | c0)

#define RAW_CAST(t, v) (*(t *)(&(v)))
#define OBJ_DOUBLE2ID(x) MAKE_OBJID(OCLS_FLOAT, RAW_CAST(uint64_t, x) >> 3)
#define OBJ_ID2DOUBLE(x) RAW_CAST(double, x << 3)

#define OID_NIL 0
#define OID_FALSE MAKE_OBJID(OCLS_BOOL, 0)
#define OID_TRUE MAKE_OBJID(OCLS_BOOL, 1)
#define OID_LONG_STRING MAKE_OBJID(OCLS_PRIM, 1) // Long string type

#define IS_OBJ_FALSEY(x) (x == OID_NIL || x == OID_FALSE || x == MAKE_OBJID(OCLS_SINT, 0))

#define VMSTK_RESERVED 256

typedef struct {
	uint16_t top;
	object_id data[VMSTK_RESERVED];
} vm_stack;

#define stk_pop(s) ((s)->top == 0 ? OID_NIL : (s)->data[--(s)->top])

typedef struct {
	object_id type;
	size_t refs;
} object_hd;

// The object table efficently maps object IDs to object structure pointers
typedef struct {
	object_hd **objects;
	size_t capacity;
	size_t count;
} object_table;

// Long strings are just strings. Just like shorts strings, they are immutable
// and may contain embedded zeros.
typedef struct {
	object_hd header;
	size_t length;
	char data[0];
} objt_string;

// Dynamic arrays which efficently store object IDs
typedef struct {
	object_hd header;
	size_t capacity;
	size_t length;
	object_id data[0];
} objt_array;

typedef struct {
	object_hd header;
	object_id feilds;
} objt_class;

typedef struct {
	object_hd header;
	object_id *pairs;
} objt_dict;
