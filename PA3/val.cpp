#include "parserInterp.h"
#include "val.h"

 // numeric overloaded add this to op
Value Value::operator+(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    return Value(GetInt() + op.GetInt());
                case VREAL:
                    return Value(GetInt() + op.GetReal());
                case VCHAR:
                    return Value(GetInt() + op.GetChar());
                default:
                    return Value(); // Unsupported operation
            }
            break;
        case VREAL:
            switch (b) {
                case VINT:
                    return Value(GetReal() + op.GetInt());
                case VREAL:
                    return Value(GetReal() + op.GetReal());
                case VCHAR:
                    return Value(GetReal() + op.GetChar());
                default:
                    return Value();
            }
            break;
        case VCHAR:
            switch (b) {
                case VINT:
                    return Value(GetChar() + op.GetInt());
                case VCHAR:
                    return Value(GetChar() + op.GetChar());
                case VREAL:
                    return Value(GetChar() + op.GetReal());
                case VSTRING:
                    return Value(GetChar() + op.GetString());
                default:
                    return Value();
            }
            break;
        case VSTRING:
            switch (b) {
                case VSTRING:
                    return Value(GetString() + op.GetString());
                case VCHAR:
                    return Value(GetString() + op.GetChar());
                default:
                    return Value();
            }
            break;
        default:
            return Value(); // Unsupported types
    }
}

 // numeric overloaded subtract op from this
Value Value::operator-(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    return Value(GetInt() - op.GetInt());
                case VREAL:
                    return Value(GetInt() - op.GetReal());
                case VCHAR:
                    return Value(GetInt() - op.GetChar());
                default:
                    return Value();
            }
            break;
        case VREAL:
            switch (b) {
                case VINT:
                    return Value(GetReal() - op.GetInt());
                case VREAL:
                    return Value(GetReal() - op.GetReal());
                case VCHAR:
                    return Value(GetReal() - op.GetChar());
                default:
                    return Value();
            }
            break;
        case VCHAR:
            switch (b) {
                case VINT:
                    return Value(GetChar() - op.GetInt());
                case VCHAR:
                    return Value(GetChar() - op.GetChar());
                default:
                    return Value();
            }
            break;
        default:
            return Value();
    }
}

// numeric overloaded multiply this by op
Value Value::operator*(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    return Value(GetInt() * op.GetInt());
                case VREAL:
                    return Value(GetInt() * op.GetReal());
                default:
                    return Value();
            }
            break;
        case VREAL:
            switch (b) {
                case VINT:
                    return Value(GetReal() * op.GetInt());
                case VREAL:
                    return Value(GetReal() * op.GetReal());
                default:
                    return Value();
            }
            break;
        default:
            return Value(); // Multiplication not supported for other types
    }
}

// numeric overloaded divide this by op
Value Value::operator/(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    if (op.GetInt() == 0) throw std::runtime_error("RUNTIME ERROR: Division by zero");
                    return Value(GetInt() / op.GetInt());
                case VREAL:
                    if (op.GetReal() == 0.0) throw std::runtime_error("RUNTIME ERROR: Division by zero");
                    return Value(GetInt() / op.GetReal());
                default:
                    return Value();
            }
            break;
        case VREAL:
            switch (b) {
                case VINT:
                    if (op.GetInt() == 0) throw std::runtime_error("RUNTIME ERROR: Division by zero");
                    return Value(GetReal() / op.GetInt());
                case VREAL:
                    if (op.GetReal() == 0.0) throw std::runtime_error("RUNTIME ERROR: Division by zero");
                    return Value(GetReal() / op.GetReal());
                default:
                    return Value();
            }
            break;
        default:
            return Value(); // Division not supported for other types
    }
}

//numeric overloaded Remainder this by op
Value Value::operator%(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    if (op.GetInt() == 0) throw std::runtime_error("RUNTIME ERROR: Modulus by zero");
                    return Value(GetInt() % op.GetInt());
                default:
                    return Value(); // Modulus only supported for integers
            }
            break;
        case VCHAR:
            switch (b) {
                case VINT:
                    if (op.GetInt() == 0) throw std::runtime_error("RUNTIME ERROR: Modulus by zero");
                    return Value(GetChar() % op.GetInt());
                case VCHAR:
                    if (op.GetChar() == 0) throw std::runtime_error("RUNTIME ERROR: Modulus by zero");
                    return Value(GetChar() % op.GetChar());
                default:
                    return Value(); // Modulus only supported for characters
            }
            break;
        default:
            return Value(); // Modulus not supported for other types
    }
}

//overloaded equality operator of this with op
Value Value::operator==(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    return Value(GetInt() == op.GetInt());
                case VREAL:
                    return Value(GetInt() == op.GetReal());
                default:
                    return Value();
            }
            break;
        case VREAL:
            switch (b) {
                case VINT:
                    return Value(GetReal() == op.GetInt());
                case VREAL:
                    return Value(GetReal() == op.GetReal());
                default:
                    return Value();
            }
            break;
        case VCHAR:
            switch (b) {
                case VCHAR:
                    return Value(GetChar() == op.GetChar());
                default:
                    return Value();
            }
            break;
        default:
            return Value();
    }
}

//overloaded inequality operator of this with op
Value Value::operator!=(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    return Value(GetInt() != op.GetInt());
                case VREAL:
                    return Value(GetInt() != op.GetReal());
                default:
                    return Value();
            }
            break;
        case VREAL:
            switch (b) {
                case VINT:
                    return Value(GetReal() != op.GetInt());
                case VREAL:
                    return Value(GetReal() != op.GetReal());
                default:
                    return Value();
            }
            break;
        case VCHAR:
            switch (b) {
                case VCHAR:
                    return Value(GetChar() != op.GetChar());
                default:
                    return Value();
            }
            break;
        case VSTRING:
            switch (b) {
                case VSTRING:
                    return Value(GetString() != op.GetString());
                default:
                    return Value();
            }
            break;
        case VBOOL:
            switch (b) {
                case VBOOL:
                    return Value(GetBool() != op.GetBool());
                default:
                    return Value();
            }
            break;
        default:
            return Value();
    }
}

//overloaded greater than operator of this with op
Value Value::operator>(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    return Value(GetInt() > op.GetInt());
                case VREAL:
                    return Value(GetInt() > op.GetReal());
                default:
                    return Value();
            }
            break;
        case VREAL:
            switch (b) {
                case VINT:
                    return Value(GetReal() > op.GetInt());
                case VREAL:
                    return Value(GetReal() > op.GetReal());
                default:
                    return Value();
            }
            break;
        default:
            return Value();
    }
}

//overloaded less than operator of this with op
Value Value::operator<(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    switch (a) {
        case VINT:
            switch (b) {
                case VINT:
                    return Value(GetInt() < op.GetInt());
                case VREAL:
                    return Value(GetInt() < op.GetReal());
                default:
                    return Value();
            }
            break;
        case VREAL:
            switch (b) {
                case VINT:
                    return Value(GetReal() < op.GetInt());
                case VREAL:
                    return Value(GetReal() < op.GetReal());
                default:
                    return Value();
            }
            break;
        case VCHAR:
            switch (b) {
                case VCHAR:
                    return Value(GetChar() < op.GetChar());
                default:
                    return Value();
            }
            break;
        case VSTRING:
            switch (b) {
                case VSTRING:
                    return Value(GetString() < op.GetString());
                default:
                    return Value();
            }
            break;
        default:
            return Value();
    }
}

//overloaded logical Anding operator of this with op
Value Value::operator&&(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    if (a == VBOOL && b == VBOOL) {
        return Value(GetBool() && op.GetBool());
    }
    return Value();
}

//overloaded logical Oring operator of this with op
Value Value::operator||(const Value& op) const {
    ValType a = GetType();
    ValType b = op.GetType();

    if (a == VBOOL && b == VBOOL) {
        return Value(GetBool() || op.GetBool());
    }
    return Value();
}

//overloaded logical Complement operator of this object
Value Value::operator!() const {
    ValType a = GetType();

    if (a == VBOOL) {
        return Value(!GetBool());
    }
    return Value();
}
