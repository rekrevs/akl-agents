
#define Fnk(term) ((Functor)(Gen(term)))
#define IsFUNC(Prt) (IsGEN(Prt) && ((Gen(Prt)->method == &(functormethod)) || \
				    (Gen(Prt)->method == &(functor0method))))


extern method functormethod;
extern method functor0method;

extern int  hash_val();
extern void build_hashvalue();

extern bool same_features();

extern Functor make_feat_functor();

extern bool lookup_feature_in_functor();

extern void print_table();
