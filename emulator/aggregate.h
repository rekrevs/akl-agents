/* $Id: aggregate.h,v 1.4 1994/03/30 08:46:31 bd Exp $ */

extern void initialize_aggregate();

typedef struct collectobj {
  method	*method;
  envid		*env;
  Term 		value;
} collectobj;

extern method collectobjmethod;

#define IsCollectObj(CObj) \
	(IsGEN(CObj) && (Gen(CObj)->method == &collectobjmethod))

#define CollectObj(x)	((collectobj*)Gen(x))

