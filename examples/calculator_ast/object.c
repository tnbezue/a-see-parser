#include "object.h"

object_vtable_t object_vtable = { NULL,dtor_noop};
void ctor_noop(void* ptr) { }
