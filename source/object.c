#include "object.h"
/*
TrObject *TrObjectNew(TrObjectType *type) {
	TrObject *this = DgMemoryAllocate(type ? type->size : sizeof *this);
	
	this->refs = 1;
	this->type = type;
	
	if (type) {
		TrObjectSendMessage((TrObjectType *) type, "retain", NULL);
	}
	
	TrObjectSendMessage(this, "construct", NULL);
	
	return this;
}

TrObject *TrObjectRetain(TrObject *this) {
	this->refs++;
	return this;
}

TrObject *TrObjectRelease(TrObject *this) {
	this->refs--;
	if (this->refs == 0) {
		// todo: recursive
		TrObjectSendMessage(this, "destruct", NULL);
		DgMemoryFree(this);
		return NULL;
	}
	return this;
}

TrObject *TrObjectSendMessage(TrObject *this, TrSelector sel, DgArray *args) {
	TrObjectType *type = this->type;
	TrMethodImp imp = NULL;
	
	while (!imp) {
		imp = DgTableGetPointer(&type->methods, sel);
		
		if (!imp) {
			if (type->parent.type != type) {
				type = type->parent.type;
			}
			else {
				break;
			}
		}
	}
	
	if (!imp) {
		return NULL;
	}
	
	return imp(this, args);
}

static TrObject *TrObjectRetain_Imp(TrObject *this, DgArray *args) {
	return TrObjectRetain(this);
}

static TrObject *TrObjectRelease_Imp(TrObject *this, DgArray *args) {
	return TrObjectRelease(this);
}

static TrObject *TrObjectGetType_Imp(TrObject *this, DgArray *args) {
	return (TrObject *) this->type;
}

TrObjectType *gTypeType;

static void TrObject_InitGodType(void) {
	TrObjectType tt;
	tt->size = sizeof(TrObjectType);
	gTypeType = TrObjectNew(&tt);
	gTypeType->parent.type = gTypeType;
	gTypeType->name = "Type";
	gTypeType->size = sizeof(TrObjectType);
	DgTableInit(&gTypeType->methods);
	DgTableSetPointer(&gTypeType->methods, "retain", TrObjectRetain_Imp);
	DgTableSetPointer(&gTypeType->methods, "release", TrObjectRelease_Imp);
	DgTableSetPointer(&gTypeType->methods, "getType", TrObjectGetType_Imp);
}

TrObjectType *TrObjectTypeNew(const char *name, size_t size) {
	if (!gTypeType) {
		TrObject_InitGodType();
	}
	
	TrObjectType *type = (TrObjectType *) TrObjectNew(gTypeType);
	type->name = name;
	type->size = size;
	DgTableInit(&gTypeType->methods);
	return type;
}

void TrObjectTypeBindMethod(TrObjectType *this, TrSelector sel, TrMethodImp imp) {
	DgTableSetPointer(&this->methods, sel, imp);
}
*/
