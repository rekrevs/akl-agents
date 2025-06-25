
typedef u32  code;

/* [BD] On the DEC Alpha, some other gremlin already defined these: */
#undef TRUE
#undef FALSE

typedef enum {
    REDO = -2,
    SUSPEND = -1,
    FALSE = 0,
    TRUE = 1
} bool;
