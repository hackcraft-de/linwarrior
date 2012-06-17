#include "Object.h"


bool Object::equals(const Object* other) {
    return (this == other);
}

int Object::hashCode() {
    return 0;
}

String* Object::toString() {
    return (String*) 0;
}

Object* Object::clone() {
    throw "Cloning is not supported by this object.";
}
