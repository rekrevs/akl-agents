/* $Id: instrdef.h,v 1.3 1991/08/31 14:29:28 sverker Exp $ */

typedef enum {

    /* Basic instructions */

    SWITCH_ON_TERM,
    
    TRY_SINGLE,
    TRY,
    RETRY,
    TRUST,
    TRY_ME,
    TRY_ME_ELSE,
    RETRY_ME_ELSE,
    TRUST_ME,
    
    GUARD_NOISY_CUT,
    GUARD_NOISY_COMMIT,
    GUARD_WAIT,
    GUARD_QUIET_WAIT,

    GUARD_COLLECT,
    GUARD_UNIT,
    
    GUARD_CUT,
    GUARD_COMMIT,

    ALLOCATE0,
    ALLOCATE,
    CALL,
    META_CALL,
    EXECUTE,
    BUILTIN,
    PROCEED,
    FAIL,
    UNDEFINED,
    
    GET_X_VARIABLE,
    GET_Y_VARIABLE,
    GET_X_VALUE,
    GET_Y_VALUE,
    GET_CONSTANT,
    GET_NIL,
    GET_STRUCTURE,
    GET_LIST,
    GET_LIST_X0,
    GET_NIL_X0,
    GET_STRUCTURE_X0,
    GET_CONSTANT_X0,
    
    PUT_X_VOID,
    PUT_Y_VOID,
    PUT_X_VARIABLE,
    PUT_Y_VARIABLE,
    PUT_X_VALUE,
    PUT_Y_VALUE,
    PUT_CONSTANT,
    PUT_NIL,
    PUT_STRUCTURE,
    PUT_LIST,

    UNIFY_VOID,
    UNIFY_Y_VARIABLE,
    UNIFY_X_VARIABLE,
    UNIFY_X_VALUE,
    UNIFY_Y_VALUE,
    UNIFY_CONSTANT,
    UNIFY_NIL,
    UNIFY_LIST,
    UNIFY_STRUCTURE,
    
    /* Extras */

    HALT,
    NOOP,
    END_OF_PRED
    
} opcode;






