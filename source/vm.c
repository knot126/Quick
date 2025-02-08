#include "common.h"
#include "vm.h"

const char *vm_tolcstring(vm_context vm, object_id object, char aux[8], size_t *size) {
	if (GET_OBJID_CLS(object) == OCLS_SSTR) {
		aux[0] = object;
		aux[1] = object >> 8;
		aux[2] = object >> 16;
		aux[3] = object >> 24;
		aux[4] = object >> 32;
		aux[5] = object >> 40;
		aux[6] = object >> 48;
		aux[7] = '\0';
		
		if (size) {
			*size = SSTR_SIZE(object);
		}
		
		return aux;
	}
	else {
		// Return a weak reference to a "real" string object
	}
}

const char *vm_tocstring(vm_context vm, object_id object, char aux[8]) {
	return vm_tolcstring(vm, object, aux, NULL);
}

object_id vm_tolstring(vm_context vm, const char *string, size_t size) {
	if (size <= 7) {
		object_id content = 0;
		
		for (size_t i = 0; i < size; i++) {
			content |= string[i] << (8 * i);
		}
		
		content |= (size << 56);
		
		return MAKE_OBJID(OCLS_SSTR, content);
	}
	else {
		// Allocate a real string object in string table
	}
}

object_hd *vm_lookup(vm_context vm, object_id object) {
	return NULL;
}

object_id vm_accquire(vm_context vm, object_id object) {
	/**
	 * Increment the refcount of an object
	 */
	
	if (GET_OBJID_CLS(object) == OCLS_ID) {
		object_hd *header = vm_lookup(vm, object);
		header->refs++;
	}
	
	return object;
}

object_id vm_release(vm_context vm, object_id object) {
	/**
	 * Decrement the refcount of an object
	 */
	
	if (GET_OBJID_CLS(object) == OCLS_ID) {
		object_hd *header = vm_lookup(vm, object);
		header->refs--;
		
		if (header->refs < 0) {
			if (GET_OBJID_CLS(header->type) == OCLS_ID) {
				// Free all subobjects
			}
			else {
				// Handle free for non-inline primitives
			}
		}
	}
	
	return object;
}

object_id handle_smallinteger_msg_send(vm_context vm, object_id object, object_id selector, size_t args, object_id *ids) {
// 	switch (selector) {
// 		case MAKE_SSTR1('+'): {
// 			int64_t a = OBJID_SEXT(object), b = OBJID_SEXT(ids[0]);
// 			int64_t c = a + b;
// 			return MAKE_OBJID(OCLS_SINT, c);
// 		}
// 		
// 		case MAKE_SSTR1('-'): {
// 			int64_t a = OBJID_SEXT(object), b = OBJID_SEXT(ids[0]);
// 			int64_t c = a - b;
// 			return MAKE_OBJID(OCLS_SINT, c);
// 		}
// 		
// 		case MAKE_SSTR1('*'): {
// 			int64_t a = OBJID_SEXT(object), b = OBJID_SEXT(ids[0]);
// 			int64_t c = a * b;
// 			return MAKE_OBJID(OCLS_SINT, c);
// 		}
// 		
// 		case MAKE_SSTR1('/'): {
// 			double a = (double)OBJID_SEXT(object), b = (double)OBJID_SEXT(ids[0]);
// 			double c = a / b;
// 			return MAKE_OBJID(OCLS_FLOAT, c);
// 		}
// 		
// 		case MAKE_SSTR2('/', '/'): {
// 			int64_t a = OBJID_SEXT(object), b = OBJID_SEXT(ids[0]);
// 			int64_t c = a / b;
// 			return MAKE_OBJID(OCLS_SINT, c);
// 		}
// 		
// 		case MAKE_SSTR1('%'): {
// 			int64_t a = OBJID_SEXT(object), b = OBJID_SEXT(ids[0]);
// 			int64_t c = a % b;
// 			return MAKE_OBJID(OCLS_SINT, c);
// 		}
// 		
// 		default: {
// 			break;
// 		}
// 	}
}

// nonid_msg_send_handler nonid_msg_send_handlers[] = {
// 	[OCLS_SINT] = handle_smallinteger_msg_send,
// 	[OCLS_SSTR] = 
// 	[OCLS_FLOAT] = 
// 	[OCLS_BOOL] = 
// 	[OCLS_PRIM] = 
// };

object_id vm_msg_send(vm_context vm, object_id object, object_id selector, size_t args, object_id *ids) {
	
	if (GET_OBJID_CLS(object) == OCLS_ID) {
		// class
		return OID_NIL;
	}
	else {
		// return __handle_nonid_msg_send(vm, object, selector, args, ids);
	}
}
