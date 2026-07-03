Tagged unions
A union in C lets multiple types share the same memory:
cunion value {
    int   i;
    float f;
    char *s;
};
The problem is you lose track of which type is actually stored. A tagged union wraps it in a struct with an enum that tracks the active type:
ctypedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STR } value_tag;

typedef struct {
    value_tag tag;
    union {
        int   i;
        float f;
        char *s;
    } val;
} tagged_value;
Now each element is self-describing. You could store an array of tagged_value contiguously — but notice every element is now sizeof(tagged_value), which is the size of the largest member plus the tag. You've regained uniformity by paying a size penalty and capping what types you can store to a known set.