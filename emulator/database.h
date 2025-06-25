/* $Id: database.h,v 1.7 1992/01/29 13:35:13 sverker Exp $ */


  /* In this implementation all objects in the code
   * area must be of a fixed size u32
   */

#define Code(PC)		*(PC++)

#define Label(PC)		PC = *((code **)PC)

#define Switch_HVA(PC)		PC = *((code **)PC)
#define Switch_ATM(PC)		PC = *((code **)(PC+1))
#define Switch_INT(PC)		PC = *((code **)(PC+2))
#define Switch_LST(PC)		PC = *((code **)(PC+3))
#define Switch_STR(PC)		PC = *((code **)(PC+4))

#define UnCode(PC)  		(--PC)

#define Get_Code(PC,C)		C = *(PC++)

#define Get_Index(PC,I) 	I = (indx)*(PC++)

#define Get_Label(PC,L)	        L = (code *)*(PC++)

#define Get_Tagged(PC, T) 	T = (TAGGED)*(PC++)

#define Get_Functor(PC,F)  	F = (functor)*(PC++)

#define Get_Definition(PC,D)  	D = (definition*)*(PC++)

#define Set_Code(PC,C)		*(PC++) = (code)(C)

#define Set_Index(PC,I) 	*(PC++) = (code)(I)

#define Set_Label(PC,L)         *(PC++) = (code)(L)

#define Set_Tagged(PC, T) 	*(PC++) = (code)(T)

#define Set_Functor(PC,F)  	*(PC++) = (code)(F)

#define Set_Definition(PC,D)  	*(PC++) = (code)(D)



typedef u32 code;
typedef u32 indx;


typedef enum {
    ENTER_EMULATED,
    ENTER_C,
    ENTER_UNDEFINED
} enter_instruction;


typedef enum {
    EMULATED_PREDICATE,
    C_PREDICATE,
    UNDEFINED_PREDICATE
} predicate_type;

union definfo {
  code *incoreinfo;
  BOOL (*cinfo)();
};


typedef struct definition {
    enter_instruction enter_instruction;
    u16 arity;
    TAGGED name;
    predicate_type predicate_type;
    union definfo code;
    BOOL protected;
    struct definition *spypoint;
} definition;


extern void	init_atomtable();
extern void	init_predtable();
extern void	init_functortable();

extern int 	hash_atom();

extern atom	store_atom();
extern functor	store_functor();
extern void 	store_predicate();
extern void 	store_emulated_predicate();
extern void 	store_c_predicate();
extern void 	store_undefined();


extern definition *get_definition();

extern BOOL pred_protection;
extern BOOL pred_protect;



typedef struct predlist {
    struct definition *def;
    struct predlist *next;
} predlist;

typedef struct storeatomlist {
    atom a;
    struct storeatomlist *next;
} storeatomlist;

typedef struct functorlist {
    functor functor;
    struct functorlist *next;
} functorlist;


extern predlist *get_predlist();
