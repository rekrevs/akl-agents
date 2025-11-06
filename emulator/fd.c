#define DEFINING_FD_DATA
#include "fd_akl.h"

static dep_class prop_table[2][2] = {{P_DOM,P_MAX},{P_MIN,P_MINMAX}};

int not_affected[4][4] = {
/*    Change  DOM MIN MAX MNX */
/* Dep */
/* DOM */    { 0,  0,  0,  0},
/* MIN */    { 1,  0,  1,  0},
/* MAX */    { 1,  1,  0,  0},
/* MNX */    { 1,  0,  0,  0}};

static bitmask mask[N] = {
0x80000000,
0x40000000,
0x20000000,
0x10000000,
0x08000000,
0x04000000,
0x02000000,
0x01000000,
0x00800000,
0x00400000,
0x00200000,
0x00100000,
0x00080000,
0x00040000,
0x00020000,
0x00010000,
0x00008000,
0x00004000,
0x00002000,
0x00001000,
0x00000800,
0x00000400,
0x00000200,
0x00000100,
0x00000080,
0x00000040,
0x00000020,
0x00000010,
0x00000008,
0x00000004,
0x00000002,
0x00000001
};

#if !defined(__GNUC__)
#define inline 
#endif

/* environmental tests */
inline int local(e,a)
     envid **e;
     andbox *a;
{
  DerefEnv(e);
  return LocalEnv(*e,a);
}

inline locality_info locality(e,a)
     envid **e;
     andbox *a;
{
  andbox *b;

  DerefEnv(e);
  b = Home(*e);
  if (a == b) return SELF;
  if (Ancestor(b)) return ANCESTOR;
  if (!a->tried) return SIBLING;
  do {
    if (Dead(b)) return NOMORE;
    b = b->father->father;
    if (a == b) return CHILD;
  } while (b);
  return SIBLING;
}

inline locality_info locality1(e,a)
     envid **e;
     andbox *a;
{
  andbox *b,*c;

  DerefEnv(e);
  if (LocalEnv(*e,a)) return SELF;
  b = Home(*e);
  c = a->father->father;
  while (c) {
    if (b == c) 
      return ANCESTOR;
    else
      c = c->father->father;
  }
  if (!a->tried) return SIBLING;
  do {
    if (Dead(b)) return NOMORE;
    b = b->father->father;
    if (a == b) return CHILD;
  } while (b);
  return SIBLING;
}

inline int fd_trailed(x,andb)
     findom *x;
     andbox *andb;
{
  if (!x->trailed)
    return 0;
  return Local(&(x->trailed),andb);
}

inline int fd_entailed(f)
     fd_function *f;
{
  Term V;
  Deref(V,f->ent);
  return Eq(V,ZERO);
}

int copy_fd_entailed(f)
     fd_function *f;
{
  Term V;
  uword FW = ForeWord(Ref(f->ent));
  if (IsForwMarked(FW))
    V = TagRef(ForwOf(FW));
  else
    Deref(V,f->ent);
  return Eq(V,ZERO);
}

inline int is_determined(X)
     Term X;
{
  Deref(X,X);
  return IsINT(X);
}

inline int non_empty_dom(m)
     bitmask *m;
{
  int i = MaxL;
  while (i)
    if (m[--i]) return 1;
  return 0;
}

intersect intersect_dom(tv,bv,lmt,lmb)
     bitmask *tv;
     bitmask *bv;
     int lmt;
     int lmb;
{
  int k, i;
  bitmask b;

  k = 0;
  b = 0;
  for (i=0; i<=lmt; i++) {
    b |= (tv[i] &= bv[i]);
    if (tv[i] != bv[i]) k=1;
  }
  if (!b) return EMPTY;
  if (k) return NONEMPTY;
  for (; i<=lmb; i++)
    if (bv[i]) return NONEMPTY;
  return SUBSET;
}

intersect intersect_fd(X,t)
     Term X;
     domstruct *t;
{
  int k, minx, maxx, mint, maxt;
  findom *x;

  switch (t->tag) {
  case INTERVAL:
    mint = Min0(t->d);
    maxt = Max0(t->d);
    if (IsDetermined(X)) {
      k = FdVal(X);
      if ((mint <= k) && (k <= maxt))
	return SUBSET;
      else
	return EMPTY;
    }
    x = Finite(X);
    minx = Min(x);
    maxx = Max(x);
    if ((mint > maxx) || (minx > maxt)) return EMPTY;
    if (IsInterval(x)) {
      if ((minx >= mint) && (maxx <= maxt)) return SUBSET;
      if (mint < minx)
	SetMin0(t->d,minx);
      if (maxx < maxt)
	SetMax0(t->d,maxx);
      return NONEMPTY;
    }
    t->tag = DOMAIN;
    range(mint,maxt,t->d);
    t->lm = (LastMask(x) < maxt/N ? LastMask(x) : maxt/N);
    return intersect_dom(t->d,Domain(x),t->lm,LastMask(x));
  case DOMAIN:
    if (IsDetermined(X)) {
      k = FdVal(X);
      if (t->d[k/N] & mask[k%N]) 
	return SUBSET;
      else 
	return EMPTY;
    }
    x = Finite(X);
    minx = Min(x);
    maxx = Max(x);
    if (IsInterval(x)) {
      bitmask d[MaxL];
      range(minx,maxx,d);
      t->lm = (maxx/N < t->lm ? maxx/N : t->lm);
      return intersect_dom(t->d,d,t->lm,maxx/N);
    }
    t->lm = (LastMask(x) < t->lm ? LastMask(x) : t->lm);
    return intersect_dom(t->d,Domain(x),t->lm,LastMask(x));
  }
}

inline int in_dom(i,x)
     int i;
     findom *x;
{
  if (IsInterval(x))
    return 1;
  return DomainWord(x,i/N) & mask[i%N];
}

void move_dom(x,t)
     findom *x;
     domstruct *t;
{
  int i;

  switch (t->tag) {
  case INTERVAL:
    return;
  case DOMAIN:
    if (IsInterval(x)) {
      UnSetInterval(x);
      NEWX(Domain(x),bitmask,(t->lm+1));
      for (i=0;i<=t->lm;i++)
	DomainWord(x,i) = t->d[i];
      SetLastMask(x,t->lm);
      return;
    }
    for (i=0;i<=t->lm;i++)
      DomainWord(x,i) = t->d[i];
    SetLastMask(x,t->lm);
    return;
  }
}

dep_class propagate_info(X,t,andb,exs)
     Term X;
     domstruct *t;
     andbox *andb;
     exstate *exs;
{
  long k1,k2,k3,k4;
  findom *x = Finite(X);
  
  if (t->tag == INTERVAL) {
    k2 = Min0(t->d);
    k4 = Max0(t->d);
  }
  else {
    k2 = min_bit(t->d,t->lm);
    k4 = max_bit(t->d,t->lm);
  }
  k1 = Min(x);
  k3 = Max(x);
  if (k1 < k2) 
    SetMin(x, k2);
  if (k4 < k3) 
    SetMax(x, k4);
  if (k2 == k4) {
    Determine(X,k2,andb,exs);
  }
  return prop_table[k1==k2?0:1][k3==k4?0:1];
}

int number_of_bits(l)
     bitmask l;
{
  int i=0, j;

  for (j=0; j<N; j++, l >>= 1)
    if (l%2) i++;
  return i;
}

void get_dom(dom,m)
     Term dom;
     bitmask *m;
{
  int k, lm, val;
  findom *x;

  if (IsINT(dom)) {
    val = GetInteger(dom);
    lm = val/N;
    for (k=0; k<lm; k++)
      m[k] = 0;
    m[k++] = mask[val%N];
    for (; k<MaxL; k++)
      m[k] = 0;
    return;
  }
  if (IsDetermined(dom)) {
    val = FdVal(dom);
    lm = val/N;
    for (k=0; k<lm; k++)
      m[k] = 0;
    m[k++] = mask[val%N];
    for (; k<MaxL; k++)
      m[k] = 0;
    return;
  }
  x = Finite(dom);
  if (IsInterval(x)) {
    range(Min(x),Max(x),m);
    return;
  }
  lm = LastMask(x);
  for (k=0; k<=lm; k++)
    m[k]=DomainWord(x,k);
  for (; k<MaxL; k++) 
    m[k] = 0;			/* clear rest of bits */
  return;
}

void fd_element_x(I, L, m)	
     Term I, L;
     bitmask *m;		/* = {Li : i in dom(I)} */
{
  findom *fd;
  int val, min, max;

  Deref(I, I);
  if (IsDetermined(I)) {
    val = FdVal(I);
    while (--val)		/* 1-indexed */
      Deref(L,LstCdr(Lst(L)));
    Deref(I,LstCar(Lst(L)));
    val = GetInteger(I);
    range(val,val,m);
    return;
  }
  fd = Finite(I);
  min = Min(fd);
  while (--min)		/* 1-indexed */
    Deref(L,LstCdr(Lst(L)));
  range(1,0,m);		/* fill with 0 */
  min = Min(fd);
  max = Max(fd);
  for (; min<=max; min++) {
    if (in_dom(min,fd)) {
      Deref(I,LstCar(Lst(L)));
      val = GetInteger(I);
      m[val/N] |= mask[val%N];
    }
    Deref(L,LstCdr(Lst(L)));
  }
  return;
}

void fd_element_i(X, L, m)	
     Term X, L;
     bitmask *m;		/* = {i : Li in dom(X)} */
{
  findom *fd;
  int i, v, val, min, max;
  Term Li;

  Deref(X, X);
  range(1,0,m);			/* fill with 0 */
  if (IsDetermined(X)) {
    v = FdVal(X);
    i = 1;			/* 1-indexed */
    while (!Eq(L,NIL)) {
      Deref(Li,LstCar(Lst(L)));
      val = GetInteger(Li);
      if (v == val)
	m[i/N] |= mask[i%N];
      i++;
      Deref(L,LstCdr(Lst(L)));
    }
    return;
  }
  fd = Finite(X);
  min = Min(fd);
  max = Max(fd);
  i = 1;			/* 1-indexed */
  while (!Eq(L,NIL)) {
    Deref(Li,LstCar(Lst(L)));
    val = GetInteger(Li);
    if (min <= val && val <= max && in_dom(val,fd))
      m[i/N] |= mask[i%N];
    i++;
    Deref(L,LstCdr(Lst(L)));
  }
  return;
}

void range(i, j, m)
     long i;
     long j;
     bitmask *m;
{
  register bitmask one;
  int k, l;

  if ((i>j) || (j < 0)) {
    for (k=0; k<MaxL; k++)
      m[k] = 0;
    return;
  }
  if (j > (N*MaxL-1)) FatalError("Overflow in domain");
  if (i < 0) i=0;
  one = ~0;
  if (j<N) {
    m[0] = (one << ((N-1)-j)) & (one >> i);
    for (k=1; k<MaxL; k++) m[k] = 0;
    return;
  }

  for (k=0; k<(i/N); k++)
    m[k] = 0;
  m[k] = one >> (i%N);

  for (l=MaxL-1; l>(j/N); l--)
    m[l] = 0;
  if (k==l) {
    m[l] &= one << ((N-1)-j%N);
    return;
  }
  else
    m[l] = one << ((N-1)-j%N);

  for (k+=1; k<l; k++)
    m[k] = one;
  return;
}

inline void bit_add(m, k)
     bitmask *m;
     long k;
{
  int i, j;
  bitmask a,b;

  if (!k) return;

  for (i = k / N; i>0; i--) {
    for (j=MaxL-2; j>=0; j--)
      m[j+1]=m[j];
    m[0] = 0;
  }
  i = k % N;
  m[MaxL-1] >>= i;

  for (j=MaxL-2; j>=0; j--) {
    a = m[j] >> i;
    b = m[j+1] | (m[j] << (N-i));
    m[j] = a;
    m[j+1] = b;
    }
  return;
}

inline void bit_sub(m, k)
     bitmask *m;
     long k;
{
  int i, j;
  bitmask a,b;

  if (!k) return;

  for (i = k / N; i>0; i--) {
    for (j=1; j<MaxL; j++)
      m[j-1]=m[j];
    m[MaxL-1] = 0;
  }
  i = k % N;
  m[0] <<= i;

  for (j=1; j<MaxL; j++) {
    a = m[j] << i;
    b = m[j-1] | (m[j] >> (N-i));
    m[j] = a;
    m[j-1] = b;
    }
  return;
}

void bit_mod(m, i)
     bitmask *m;
     long i;
{
  int j, k, n;
  bitmask t;

  for (j=0; j<MaxL; j++) {
    t = m[j];
    m[j] = 0;
    for (k=0; k<N; k++)
      if (t & mask[k]) {
	n = (j*N + k) % i;	/* new element */
	m[n/N] |= mask[n%N];
      }
  }
  return;
}

inline void bit_compl(m)
     bitmask *m;
{
  int j;
  
  for (j=0; j<MaxL; j++)
    m[j] = ~m[j];
  return;
}

inline void bit_union(m, m0)
     bitmask *m;
     bitmask *m0;
{
  int j;
  
  for (j=0; j<MaxL; j++)
    m[j] |= m0[j];
  return;
}

inline void bit_intersect(m, m0)
     bitmask *m;
     bitmask *m0;
{
  int j;
  
  for (j=0; j<MaxL; j++)
    m[j] &= m0[j];
  return;
}

inline long max_b(x,j)
     bitmask x;
     int j;
{
  int i=0;
  if (x == (x & 0xFFFF0000)) x >>= 16, i += 16;
  if (x == (x & 0xFFFFFF00)) x >>= 8, i += 8;
  if (x == (x & 0xFFFFFFF0)) x >>= 4, i += 4;
  if (x == (x & 0xFFFFFFFC)) x >>= 2, i += 2;
  if (x == (x & 0xFFFFFFFE)) i++;
  return (N-i-1)+j*N;
}

long max_bit(m,l)
     bitmask *m;
     int l;
{
  int j;

  for (j=l; j>=0; j--) 
    if (m[j])
      return max_b(m[j],j);
  return -1;
}

inline long min_b(x,j)
     bitmask x;
     int j;
{
  int i=0;
  if (mask[0] & x) return j*N;
  if (x >= 0x10000) x >>= 16, i += 16;
  if (x >= 0x100) x >>= 8, i += 8;
  if (x >= 0x10) x >>= 4, i += 4;
  if (x >= 0x4) x >>= 2, i += 2;
  if (x >= 0x2) i++;
  return (N-i-1)+j*N;
}

long min_bit(m,l)
     bitmask *m;
     int l;
{
  int j;

  for (j=0; j<=l; j++) 
    if (m[j])
      return min_b(m[j],j);
  return N*MaxL;
}

inline dom_info monotonicity(ms)
     monstruct *ms;
{
  int s=1, g=1;
  moncons *l;

  l = ms->mon;
  while (l) {
    if (!IsDetermined(l->d)) goto cont0;
    l = l->next;
  }
  s = 2;
 cont0:
  l = ms->amon;
  while (l) {
    if (!IsDetermined(l->d)) goto cont1;
    l = l->next;
  }
  g = 3;
 cont1:
  switch (s*g) {
  case 1:
    return BOTTOM;
  case 2:
    return SHRINKING;
  case 3:
    return GROWING;
  case 6:
    return CONSTANT;
  }
}

void evaluate_range(c,args,t)
     long c;			/* list of instructions */
     argvec args;
     domstruct *t;		/* domain to be computed */
{
  register long fd_operand;
  register fd_instr fd_op;
  static long v_stack[VStack];  /* value stack */
  register int v=0;
  static bitmask b_heap[BHeap];	/* interpreter heap */
  register int b=0;
  int i;
  register long r1, r2;
  register Term a_0, a_1, a_2, T;

#ifdef THREADED_CODE
  static address label_table[] = {
    &&CaseLabel(FD_MAXELEM),
    &&CaseLabel(FD_RANGE),
    &&CaseLabel(FD_OUTER_RANGE),
    &&CaseLabel(FD_SETADD),
    &&CaseLabel(FD_SETSUB),
    &&CaseLabel(FD_SETMOD),
    &&CaseLabel(FD_COMPL),
    &&CaseLabel(FD_UNION),
    &&CaseLabel(FD_INTER),
    &&CaseLabel(FD_CHECK),
    &&CaseLabel(FD_ADD),
    &&CaseLabel(FD_MULT),
    &&CaseLabel(FD_SUB),
    &&CaseLabel(FD_DIVD),
    &&CaseLabel(FD_DIVU),
    &&CaseLabel(FD_MIN),
    &&CaseLabel(FD_MAX),
    &&CaseLabel(FD_MOD),
    &&CaseLabel(FD_VAL),
    &&CaseLabel(FD_VAL_0),
    &&CaseLabel(FD_VAL_1),
    &&CaseLabel(FD_VAL_2),
    &&CaseLabel(FD_DOM),
    &&CaseLabel(FD_DOM_0),
    &&CaseLabel(FD_DOM_1),
    &&CaseLabel(FD_DOM_2),
    &&CaseLabel(FD_DOMMIN),
    &&CaseLabel(FD_DOMMIN_0),
    &&CaseLabel(FD_DOMMIN_1),
    &&CaseLabel(FD_DOMMIN_2),
    &&CaseLabel(FD_DOMMAX),
    &&CaseLabel(FD_DOMMAX_0),
    &&CaseLabel(FD_DOMMAX_1),
    &&CaseLabel(FD_DOMMAX_2),
    &&CaseLabel(FD_CONST),
    &&CaseLabel(FD_ELEMENT_X),
    &&CaseLabel(FD_ELEMENT_I),
    &&CaseLabel(FD_HALT)
    };
  if (!args) {
    fd_instr_label_table = label_table;
    fd_instr_label_table_length = sizeof(label_table)/sizeof(label_table[0]);
    return;
  }
#endif

  fd_time++;
  if (EnvSize(args) > 0) Deref(a_0,EnvArg(0,args));
  if (EnvSize(args) > 1) Deref(a_1,EnvArg(1,args));
  if (EnvSize(args) > 2) Deref(a_2,EnvArg(2,args));
  fd_op = (fd_instr)NextInstr(c);
  while (1) {
    assert(v<VStack);
    assert(b<BHeap);
    SwitchTo(fd_op) {
    CaseLabel(FD_HALT):
      goto rest;
    CaseLabel(FD_CONST):
      fd_operand = NextInstr(c);
      v_stack[v++] = fd_operand;
      NextFdInstr;
    CaseLabel(FD_ELEMENT_X):
      /* U(a_1(i) : i in a_0) */
      fd_element_x(a_0, a_1, (b_heap+b));
      v_stack[v++] = b;
      b += MaxL;
      NextFdInstr;
    CaseLabel(FD_ELEMENT_I):
      /* U(i : a_0(i) in a_1) */
      fd_element_i(a_0, a_1, (b_heap+b));
      v_stack[v++] = b;
      b += MaxL;
      NextFdInstr;
    CaseLabel(FD_VAL):
      fd_operand = NextInstr(c);
      Deref(T,EnvArg(fd_operand,args));
      v_stack[v++] = GetInteger(T);
      NextFdInstr;
    CaseLabel(FD_VAL_0):
      v_stack[v++] = GetInteger(a_0);
      NextFdInstr;
    CaseLabel(FD_VAL_1):
      v_stack[v++] = GetInteger(a_1);
      NextFdInstr;
    CaseLabel(FD_VAL_2):
      v_stack[v++] = GetInteger(a_2);
      NextFdInstr;
    CaseLabel(FD_DOM):
      fd_operand = NextInstr(c);
      Deref(T,EnvArg(fd_operand,args));
      get_dom(T,(b_heap+b));
      v_stack[v++] = b; 
      b += MaxL;
      NextFdInstr;
    CaseLabel(FD_DOM_0):
      get_dom(a_0,(b_heap+b));
      v_stack[v++] = b; 
      b += MaxL;
      NextFdInstr;
    CaseLabel(FD_DOM_1):
      get_dom(a_1,(b_heap+b));
      v_stack[v++] = b; 
      b += MaxL;
      NextFdInstr;
    CaseLabel(FD_DOM_2):
      get_dom(a_2,(b_heap+b));
      v_stack[v++] = b; 
      b += MaxL;
      NextFdInstr;
    CaseLabel(FD_DOMMIN):
      fd_operand = NextInstr(c);
      Deref(T,EnvArg(fd_operand,args));
      v_stack[v++] = MinVal(T);
      NextFdInstr;      
    CaseLabel(FD_DOMMIN_0):
      v_stack[v++] = MinVal(a_0);
      NextFdInstr;      
    CaseLabel(FD_DOMMIN_1):
      v_stack[v++] = MinVal(a_1);
      NextFdInstr;      
    CaseLabel(FD_DOMMIN_2):
      v_stack[v++] = MinVal(a_2);
      NextFdInstr;      
    CaseLabel(FD_DOMMAX):
      fd_operand = NextInstr(c);
      Deref(T,EnvArg(fd_operand,args));
      v_stack[v++] = MaxVal(T);
      NextFdInstr;      
    CaseLabel(FD_DOMMAX_0):
      v_stack[v++] = MaxVal(a_0);
      NextFdInstr;      
    CaseLabel(FD_DOMMAX_1):
      v_stack[v++] = MaxVal(a_1);
      NextFdInstr;      
    CaseLabel(FD_DOMMAX_2):
      v_stack[v++] = MaxVal(a_2);
      NextFdInstr;      
    CaseLabel(FD_RANGE):		/* X..Y */
      r1 = v_stack[--v];	/* X */
      r2 = v_stack[--v];	/* Y */
      range(r1, r2, (b_heap+b));
      v_stack[v++] = b;
      b += MaxL;
      NextFdInstr;
    CaseLabel(FD_OUTER_RANGE):		/* X..Y */
      {
	r1 = v_stack[--v];	/* X */
	r2 = v_stack[--v];	/* Y */
	if ((r1>r2) || (r2<0)) {
	  r1 = 1;
	  r2 = 0;
	}
	else {
	  if (r1<0) r1=0;
	}
	t->tag = INTERVAL;
	t->d = b_heap;
	SetMin0(t->d,r1);
	SetMax0(t->d,r2);
	t->lm = 0;
	return;
      }
    CaseLabel(FD_SETADD):		/* R+X */
      r1 = v_stack[--v];	/* R */
      r2 = v_stack[v-1], v_stack[v-1] = r1;
      bit_add((b_heap+r1), r2);
      NextFdInstr;
    CaseLabel(FD_SETSUB):		/* R-X */
      r1 = v_stack[--v];	/* R */
      r2 = v_stack[v-1], v_stack[v-1] = r1;
      bit_sub((b_heap+r1), r2);
      NextFdInstr;
    CaseLabel(FD_SETMOD):		/* R mod X */
      r1 = v_stack[--v];	/* R */
      r2 = v_stack[v-1], v_stack[v-1] = r1;
      bit_mod((b_heap+r1), r2);
      NextFdInstr;
    CaseLabel(FD_COMPL):		/* -R */
      r1 = v_stack[v-1];	/* R */
      bit_compl((b_heap+r1));
      NextFdInstr;
    CaseLabel(FD_UNION):		/* R1 : R2 */
      r1 = v_stack[--v];	/* R1 */
      r2 = v_stack[v-1];        /* R2 */
      b -= MaxL;	
      bit_union((b_heap+r2), (b_heap+r1));
      NextFdInstr;
    CaseLabel(FD_INTER):		/* R1 & R2 */
      r1 = v_stack[--v];	/* R1 */
      r2 = v_stack[v-1];	/* R2 */
      b -= MaxL;
      bit_intersect((b_heap+r2), (b_heap+r1));
      NextFdInstr;
    CaseLabel(FD_CHECK):		/* S => R */
      r1 = v_stack[--v];	/* S */
      if (non_empty_dom(b_heap+r1)) {
	b -= MaxL;              /* pop non-empty domain */
	NextInstr(c);
      }
      else {
	v_stack[v++] = r1;	/* push empty domain */
	c = byte_code[c];
      }
      NextFdInstr;
    CaseLabel(FD_ADD):		/* X + Y */
      r1 = v_stack[--v];	/* X */
      r2 = v_stack[--v];	/* Y */
      v_stack[v++] = r1 + r2;
      NextFdInstr;
    CaseLabel(FD_MULT):		/* X * Y */
      r1 = v_stack[--v];
      r2 = v_stack[--v];
      v_stack[v++] = r1 * r2;
      NextFdInstr;
    CaseLabel(FD_SUB):		/* X - Y */
      r1 = v_stack[--v];
      r2 = v_stack[--v];
      v_stack[v++] = r1 - r2;
      NextFdInstr;
    CaseLabel(FD_DIVD):		/* X /d N */
      r1 = v_stack[--v];
      r2 = v_stack[--v];
      v_stack[v++] = r1 / r2;
      NextFdInstr;
    CaseLabel(FD_DIVU):		/* X /u N */
      r1 = v_stack[--v];
      r2 = v_stack[--v];
      v_stack[v++] = (r1+r2-1) / r2;
      NextFdInstr;
    CaseLabel(FD_MOD):		/* X mod N */
      r1 = v_stack[--v];
      r2 = v_stack[--v];
      v_stack[v++] = r1 % r2;
      NextFdInstr;
    CaseLabel(FD_MIN):		/* min(R) */
      r1 = v_stack[--v];	/* R */
      b -= MaxL;
      v_stack[v++] = min_bit((b_heap+r1),MaxL-1);
      NextFdInstr;
    CaseLabel(FD_MAX):		/* max(R) */
      r1 = v_stack[--v];	/* R */
      b -= MaxL;
      v_stack[v++] = max_bit((b_heap+r1),MaxL-1);
      NextFdInstr;
    CaseLabel(FD_MAXELEM):
      v_stack[v++] = MaxL*N-1;
      NextFdInstr;
    DefaultLabel:
      FatalError("Unimplemented FD-instruction!");
    }
  }
 rest:
  t->tag = DOMAIN;
  r1 = v_stack[v-1];		/* pop the address to the resulting domain */
  for (i=MaxL-1; i>0; i--)	/* find last bitmask */
    if ((b_heap+r1)[i] != 0) break;

  t->d = b_heap+r1;
  t->lm = i;
  return;
}

void print_dom(file,x)
     FILE *file;
     findom *x;
{
  int i,j;

  if (IsInterval(x)) {
    i = Min(x);
    j = Max(x);
    for (; i<=j; i++)
      fprintf(file, "%d ", i);
  }
  else {
    bitmask *dom = Domain(x);
    short l = LastMask(x);
    for (i=0; i<=l; i++)
      for (j=0; j<N; j++)
	if (dom[i] & mask[j]) 
	  fprintf(file, "%d ", N*i+j);
  }
  return;
}

void p_fin(x)
     findom *x;
{
  print_finite(stdout,x);
  printf("\n");
}

void copy_args(args,ap)
     argvec args;
     argvec *ap;
{
  *ap = args;
  copy_location(ap);
  return;
}

void copy_mon(ms,msp)
     monstruct *ms;
     monstruct **msp;
{
  moncons *l, **lp;

  NEW(*msp,monstruct);
  if (!ms->mon) {
    (*msp)->mon = NULL;
    if (!ms->amon) {
      (*msp)->amon = NULL;
      return;
    }
    goto amlbl;
  }
  l = ms->mon;
  lp = &((*msp)->mon);
  while (l) {
    NEW(*lp,moncons);
    (*lp)->d = l->d;
    copy_location(&((*lp)->d));
    lp = &((*lp)->next);
    l = l->next;
  }
  *lp = NULL;
 amlbl:
  l = ms->amon;
  lp = &((*msp)->amon);
  while (l) {
    NEW(*lp,moncons);
    (*lp)->d = l->d;
    copy_location(&((*lp)->d));
    lp = &((*lp)->next);
    l = l->next;
  }
  *lp = NULL;
  return;
}

void split_fd_susp(s)
     fd_suspension *s;
{
  fd_function *fdc, *nfdc;
  fd_suspension *t;

  for (; s; s=s->next) {
    fdc = s->f;
    DerefEnv(&(fdc->env));
    if (InCopy(fdc->env)) {
      NEW(t,fd_suspension);
      t->dep = s->dep;
      t->next = s->next;
      s->next = t;
      s = t;
      if (!Forwarded(fdc)) {
	NEW(nfdc,fd_function);
	t->f = nfdc;
	nfdc->info = fdc->info;	
	nfdc->env = NewEnv(fdc->env);
	nfdc->d = fdc->d;
	copy_location(&(nfdc->d));
	copy_forward(fdc,nfdc);	/* destroys fdc->d */
	nfdc->c = fdc->c;
	nfdc->ent = fdc->ent;
	copy_location(&(nfdc->ent));
	copy_args(fdc->args,&(nfdc->args));
	copy_mon(fdc->moninfo,&(nfdc->moninfo));
	nfdc->constr = copy_referenced_constraint(fdc->constr);
      }
      else
	t->f = DerefForward(fdc);
    }
  }
  return;
}

void copy_fd_susp(sp,tp)
     fd_suspension **sp;
     fd_suspension **tp;
{
  fd_function *fdc, *nfdc;
  fd_suspension *t, *s;

  for (s=*sp; s; s=s->next) {
    fdc = s->f;
    if (NoMore(fdc)) {
      *sp = s->next;
      continue;
    }
    if (CopyEntailed(fdc)) {
      *sp = s->next;
      continue;
    }
    NEW(t,fd_suspension);
    t->dep = s->dep;
    *tp = t;
    tp = &(t->next);
    DerefEnv(&(fdc->env));
    if (InCopy(fdc->env)) {
      if (!Forwarded(fdc)) {
	NEW(nfdc,fd_function);
	t->f = nfdc;
	nfdc->info = fdc->info;	
	nfdc->env = NewEnv(fdc->env);
	nfdc->d = fdc->d;
	copy_location(&(nfdc->d));
	copy_forward(fdc,nfdc);	/* destroys fdc->d */
	nfdc->c = fdc->c;
	nfdc->ent = fdc->ent;
	copy_location(&(nfdc->ent));
	copy_args(fdc->args,&(nfdc->args));
	copy_mon(fdc->moninfo,&(nfdc->moninfo));
	nfdc->constr = copy_referenced_constraint(fdc->constr);
      }
      else
	t->f = DerefForward(fdc);
    }
    else
      t->f = fdc;
    sp = &(s->next);
  }
  *tp = NULL;
  return;
}

void gc_args(args,ap,gcst)
     argvec args;
     argvec *ap;
     gcstatep gcst;
{
  *ap = args;
  gc_location(ap,gcst);
  return;
}

void gc_mon(ms,msp,gcst)
     monstruct *ms;
     monstruct **msp;
     gcstatep gcst;
{
  moncons *l, **lp;

  NEW(*msp,monstruct);
  if (!ms->mon) {
    (*msp)->mon = NULL;
    if (!ms->amon) {
      (*msp)->amon = NULL;
      return;
    }
    goto amlbl;
  }
  l = ms->mon;
  lp = &((*msp)->mon);
  while (l) {
    NEW(*lp,moncons);
    (*lp)->d = l->d;
    gc_location(&((*lp)->d),gcst);
    lp = &((*lp)->next);
    l = l->next;
  }
  *lp = NULL;
 amlbl:
  l = ms->amon;
  lp = &((*msp)->amon);
  while (l) {
    NEW(*lp,moncons);
    (*lp)->d = l->d;
    gc_location(&((*lp)->d),gcst);
    lp = &((*lp)->next);
    l = l->next;
  }
  *lp = NULL;
  return;
}

void gc_fd_susp(sp,tp,gcst)
     fd_suspension **sp;
     fd_suspension **tp;
     gcstatep gcst;
{
  fd_function *fdc, *nfdc;
  fd_suspension *t, *s;

  for (s=*sp; s; s=s->next) {
    fdc = s->f;
    if (NoMore(fdc)) {
      *sp = s->next;
      continue;
    }
    if (GcEntailed(fdc)) {
      *sp = s->next;
      continue;
    }
    DerefEnv(&(fdc->env));
    if (InGc(fdc->env)) {
      NEW(t,fd_suspension);
      t->dep = s->dep;
      *tp = t;
      tp = &(t->next);
      if (!Forwarded(fdc)) {
	NEW(nfdc,fd_function);
	t->f = nfdc;
	nfdc->info = fdc->info;	
	nfdc->env = NewEnv(fdc->env);
	nfdc->d = fdc->d;
	gc_location(&(nfdc->d),gcst);
	gc_forward(fdc,nfdc);	/* destroys fdc->d */
	nfdc->c = fdc->c;
	nfdc->ent = fdc->ent;
	gc_location(&(nfdc->ent),gcst);
	gc_args(fdc->args,&(nfdc->args),gcst);
	gc_mon(fdc->moninfo,&(nfdc->moninfo),gcst);
	nfdc->constr = gc_referenced_constraint(fdc->constr,gcst);
      }
      else
	t->f = DerefForward(fdc);
    }
    sp = &(s->next);
  }
  *tp = NULL;
  return;
}

void suspend_indexical(X,c,V,args,ms,propv,at,andb,dv)
     Term X;
     long c;
     Term V;
     argvec args;
     monstruct *ms;
     Term propv;
     int  at;
     andbox *andb;
     Term dv;
{
  int i, g_c=1, l_c=1, p;
  fd_constraint *cns;
  fd_function *fdc;
  findom *y;
  Term Y, P;

  for (i=0; i<DVSize(dv); i++) {
    Deref(Y,StrArgument(Str(dv),i));
    if (IsDetermined(Y)) continue;
    y = Finite(Y);
    if (l_c) {
      MakeInC(fdc,X,c,V,args,ms,at,andb);
      l_c = 0;
    }
    if (g_c) {
      MakeGlobalIn(cns,fdc);
      LinkConstr(andb,(constraint *)cns);
      g_c = 0;
    }
    Deref(P,StrArgument(Str(propv),i));
    p = GetInteger(P);
    PushInSuspension(fdc,p,y);
    if (ExternalObj(y,andb)) {
      Mark(andb,Home(y->env));
    }
  }
}

bool opt_ok(f)
     fd_function *f;
{
  domstruct t;

  evaluate_range(f->c,f->args,&t);
  switch (intersect_fd(f->d,&t)) {
  case SUBSET:
    return TRUE;
  case EMPTY:
  case NONEMPTY:
    return FALSE;
  }
}

bool fd_propagate(x,prop,andb,exs)
     findom *x;
     dep_class prop;
     andbox *andb;
     exstate *exs;
{
  domstruct t;
  findom *fd;
  fd_function *fdc;
  dep_class pr;
  fd_suspension *l;
  dom_info di;
  QueueDecl;
  locality_info loc;
  Term X;
  int local_indexical;

  InitQueue(x,prop,fd_time);
  do {
    for(l=x->isusp; l; l=l->next) {
      fdc = l->f;
      local_indexical = 0;
      if (Entailed(fdc))
	continue;
      if (LastExec(fdc) > Time(x))
	continue;
      if (not_affected[l->dep][Change(x)])
	continue;
      di = monotonicity(fdc->moninfo);
      if (Bottom(di))
	continue;
      if (Tell(fdc) && Growing(di))
	continue;
      if (NoMore(fdc))
	continue;
      loc = Locality(&(fdc->env),andb);
      switch (loc) {
      case NOMORE:
	SetNoMore(fdc);
	continue;
      case CHILD:
	GlobalWakeIn(fdc,exs);
	continue;
      case SIBLING:
	continue;
      case SELF:
	local_indexical = 1;
      case ANCESTOR:
	evaluate_range(fdc->c,fdc->args,&t);
	SetLastExec(fdc,fd_time);
	Deref(X,fdc->d);
	switch (intersect_fd(X,&t)) {
	case EMPTY:
	  if (Growing(di)) {
	    continue;
	  }
	  return FALSE;
	case SUBSET:
	  if (Shrinking(di))
	    continue;
	  if (local_indexical)
	    IndexicalEntailed(fdc);
	  SetEntailed(fdc->ent,andb,exs);
	  continue;
	case NONEMPTY:
	  if (Growing(di))
	    continue;
	  fd = Finite(X);
	  if (MustTrail(fd,andb))
	    trail(X,andb);
	  pr = propagate_info(X,&t,andb,exs);
	  if (!IsDetermined(X)) 
	    move_dom(fd,&t);
	  else
	    wake_agent_suspensions(fd,andb,exs);
	  wake_trail_suspensions(fd,andb,exs);
	  if (Queued(fd)) {
	    UpdateQueued(fd,pr,fd_time);
	  }
	  else {
	    Enqueue(fd,fdc,pr,fd_time);
	  }
	  if (Constant(di)) {
	    if (local_indexical)
	      IndexicalEntailed(fdc);
	    SetEntailed(fdc->ent,andb,exs);
	  }
	  continue;
	}
      }
    }
    Dequeue(x);
  } while (x);
  return TRUE;
}

bool dom_check(X,c,V,args,propv,ms,at,dv,andb,exs)
     Term X;			/* Number or finite domain */
     long c;
     Term V;
     argvec args;
     Term propv;
     monstruct *ms;
     int  at;
     Term dv;
     andbox *andb;
     exstate *exs;
{
  domstruct t;
  dep_class prop;
  dom_info di = monotonicity(ms);
  findom *x;

  if (Bottom(di)) {
    suspend_indexical(X,c,V,args,ms,propv,at,andb,dv);
    return TRUE;
  }
  if (Growing(di) && at) {
    suspend_indexical(X,c,V,args,ms,propv,at,andb,dv);
    return TRUE;
  }
  evaluate_range(c,args,&t);
  switch (intersect_fd(X,&t)) {
  case EMPTY:
    if (Shrinking(di) || Constant(di)) {
      return FALSE;
      }
    suspend_indexical(X,c,V,args,ms,propv,at,andb,dv);
    return TRUE;
  case SUBSET:
    if (Growing(di) || Constant(di)) {
      return TRUE;
    }
    suspend_indexical(X,c,V,args,ms,propv,at,andb,dv);
    return TRUE;
  case NONEMPTY:
    if Growing(di) {
      suspend_indexical(X,c,V,args,ms,propv,at,andb,dv);
      return TRUE;
    }
    x = Finite(X);
    if (MustTrail(x,andb))
      trail(X,andb);
    prop = propagate_info(X,&t,andb,exs);
    if (!IsDetermined(X)) 
      move_dom(x,&t);
    else
      wake_agent_suspensions(x,andb,exs);
    wake_trail_suspensions(x,andb,exs);
    if (fd_propagate(x,prop,andb,exs)) {
      if (Constant(di)) {
	SetEntailed(V,andb,exs);
	return TRUE;
      }
      else {
	suspend_indexical(X,c,V,args,ms,propv,at,andb,dv);
	return TRUE;
      }
    }
    else
      return FALSE;
  }
}

bool construct_fd(X0,c,V,args,propv,ms,at,dv,andb,exs)
     Term X0;
     long c;
     Term V;
     argvec args;
     Term propv;
     monstruct *ms;
     int  at;
     Term dv;
     andbox *andb;
     exstate *exs;
{
  findom *x;
  Term X;
  bitmask b=0;
  int i,min,max;
  domstruct t;
  dom_info di = monotonicity(ms);

  if (Shrinking(di) || Constant(di)) {
    evaluate_range(c,args,&t);
    if (t.tag == INTERVAL) {
      if (Min0(t.d) > Max0(t.d))
	return FALSE;
    }
    else {
      for (i=0; i<=t.lm; i++)
	b |= t.d[i];
      if (!b) {
	return FALSE;
      }
    }
    MakeGvainfo(x,findom,&finitemethod,andb);
    MakeCvaTerm(X,(Gvainfo)x);
    Domain(x) = NULL;
    x->asusp = NULL;
    x->tsusp = NULL;
    x->isusp = NULL;
    x->next = NULL;
    x->info = 0;
    x->trailed = NULL;
    if (t.tag == INTERVAL) {
      min = Min0(t.d);
      max = Max0(t.d);
    }
    else {
      min = min_bit(t.d,t.lm);
      max = max_bit(t.d,t.lm);
    }
    SetMin(x,min);
    SetMax(x,max);
    SetLastMask(x,t.lm);
    if (t.tag == INTERVAL)
      SetInterval(x);
    else {
      NEWX(Domain(x),bitmask,(t.lm+1));
      move_dom(x,&t);
    }
    if (Constant(di)) {
      SetEntailed(V,andb,exs);
    }
    else
      suspend_indexical(X,c,V,args,ms,propv,at,andb,dv);
    if ((Constant(di) || Shrinking(di)) && min == max) {
      Determine(X,min,andb,exs);
    }
    unify(X0,X,andb,exs);
  }
  else {
    MakeGvainfo(x,findom,&finitemethod,andb);
    MakeCvaTerm(X,(Gvainfo)x);
    Domain(x) = NULL;
    x->asusp = NULL;
    x->tsusp = NULL;
    x->isusp = NULL;
    x->next = NULL;
    x->info = 0;
    x->trailed = NULL;
    SetMin(x,0);
    SetMax(x,MaxL*N-1);
    SetInterval(x);
    suspend_indexical(X,c,V,args,ms,propv,at,andb,dv);
    unify(X0,X,andb,exs);
  }
  return TRUE;
}

bool fd_in(X,c,V,args,propv,ms,at,dv,andb,exs)
     Term X;
     long c;
     Term V;
     argvec args;
     Term propv;
     monstruct *ms;
     int  at;
     Term dv;
     andbox *andb;
     exstate *exs;
{
  if (IsINT(X))
    return dom_check(X,c,V,args,propv,ms,at,dv,andb,exs);
  if (IsFinite(X))
    return dom_check(X,c,V,args,propv,ms,at,dv,andb,exs);
  if (IsVar(X)) 
    return construct_fd(X,c,V,args,propv,ms,at,dv,andb,exs);
  return FALSE;
}

long *fd_disassemble(cp,args)
     long *cp;
     argvec args;
{
  switch (*cp) {
  case FD_CONST:
    printf("%d",cp[1]);
    return cp+2;
  case FD_ELEMENT_X:
    printf("element_x(");
    display_term(EnvArg(0,args));
    printf(",");
    display_term(EnvArg(1,args));
    printf(")");
    return cp+2;
  case FD_ELEMENT_I:
    printf("element_i(");
    display_term(EnvArg(0,args));
    printf(",");
    display_term(EnvArg(1,args));
    printf(")");
    return cp+2;
  case FD_VAL:			/* N */
    display_term(EnvArg(cp[1],args));
    return cp+2;
  case FD_VAL_0:
    display_term(EnvArg(0,args));
    return cp+1;
  case FD_VAL_1:
    display_term(EnvArg(1,args));
    return cp+1;
  case FD_VAL_2:
    display_term(EnvArg(2,args));
    return cp+1;
  case FD_DOM:
    printf("dom(");
    display_term(EnvArg(cp[1],args));
    printf(")");
    return cp+2;
  case FD_DOM_0:
    printf("dom(");
    display_term(EnvArg(0,args));
    printf(")");
    return cp+1;
  case FD_DOM_1:
    printf("dom(");
    display_term(EnvArg(1,args));
    printf(")");
    return cp+1;
  case FD_DOM_2:
    printf("dom(");
    display_term(EnvArg(2,args));
    printf(")");
    return cp+1;
  case FD_DOMMAX:
    printf("max(");
    display_term(EnvArg(cp[1],args));
    printf(")");
    return cp+2;
  case FD_DOMMAX_0:
    printf("max(");
    display_term(EnvArg(0,args));
    printf(")");
    return cp+1;
  case FD_DOMMAX_1:
    printf("max(");
    display_term(EnvArg(1,args));
    printf(")");
    return cp+1;
  case FD_DOMMAX_2:
    printf("max(");
    display_term(EnvArg(2,args));
    printf(")");
    return cp+1;
  case FD_DOMMIN:
    printf("min(");
    display_term(EnvArg(cp[1],args));
    printf(")");
    return cp+2;
  case FD_DOMMIN_0:
    printf("min(");
    display_term(EnvArg(0,args));
    printf(")");
    return cp+1;
  case FD_DOMMIN_1:
    printf("min(");
    display_term(EnvArg(1,args));
    printf(")");
    return cp+1;
  case FD_DOMMIN_2:
    printf("min(");
    display_term(EnvArg(2,args));
    printf(")");
    return cp+1;
  case FD_RANGE:		/* X..Y */
    printf("..(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_OUTER_RANGE:		/* X..Y */
    printf("..(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_SETADD:		/* R+X */
  case FD_ADD:
    printf("+(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_SETSUB:		/* R-X */
  case FD_SUB:
    printf("-(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_SETMOD:		/* R mod X */
  case FD_MOD:
    printf("mod(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_COMPL:		/* -R */
    printf("-(");
    cp = fd_disassemble(cp+1,args);
    printf(")");
    return cp;
  case FD_UNION:		/* R1 : R2 */
    printf("\\/(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_INTER:		/* R1 & R2 */
    printf("/\\(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_MULT:			/* X * Y */
    printf("*(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_DIVD:			/* X / N */
    printf("/(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_DIVU:			/* (X+N-1) / N */
    printf("//(");
    cp = fd_disassemble(cp+1,args);
    printf(",");
    cp = fd_disassemble(cp, args);
    printf(")");
    return cp;
  case FD_MIN:			/* min(R) */
    printf("min(");
    cp = fd_disassemble(cp+1,args);
    printf(")");
    return cp;
  case FD_MAX:			/* max(R) */
    printf("max(");
    cp = fd_disassemble(cp+1,args);
    printf(")");
    return cp;
  case FD_MAXELEM:
    printf("max");
    return cp+1;
  case FD_HALT:
    return NULL;
  }
}

void fd_das(cp,args)
     long *cp;
     argvec args;
{
  while (1) {
    cp = fd_disassemble(cp,args);
    if (*cp == FD_CHECK) {
      printf(" <= ");
      cp += 2;
    }
    else
      return;
  }
}

int print_lin(c)
     fd_function *c;
{
  long d = c->c;
  long b_code[200], *cp;
  enum_fd_instr instr;

  cp = b_code+200;
  *(--cp) = FD_HALT;
  while (cp-b_code > 0) {
    instr = FdInstrToEnum(byte_code[d]);
    switch(instr) {
    case FD_HALT:
      goto rest;
    case FD_CONST:
    case FD_VAL:
    case FD_DOMMIN:
    case FD_DOMMAX:
    case FD_DOM:
    case FD_CHECK:
      *(--cp) = byte_code[d+1];
      d++;
    default:
      *(--cp) = instr;
      d++;
    }
  }
  FatalError("Overflow when printing in-constraint!");
 rest:
  display_term(c->d);
  printf(" in ");
  fd_das(cp,c->args);
  return 1;
}

int p_gin(c)
     fd_constraint *c;
{
  print_in(c);
  printf("\n");
  return 0;
}

int p_gwake(c)
     fd_constraint *c;
{
  printf("Global wake:\n");
  return p_gin(c);
}

int p_gsusp(c)
     fd_constraint *c;
{
  printf("Global suspension:\n");
  return p_gin(c);
}

int p_lin(c)
     fd_function *c;
{
  print_lin(c);
  printf("\n");
  return 0;
}

int p_lwake(c)
     fd_function *c;
{
  printf("Local wake:\n");
  return p_lin(c);
}

int p_lsusp(c)
     fd_function *c;
{
  printf("Local suspension:\n");
  return p_lin(c);
}

void d_t(X)
     Term X;
{
  display_term(X);
  printf("\n");
}

intersect constant_in_trail(v,c)
     int v;
     trail_constraint *c;
{
  if (IsInterval(c)) {
    if (v >= Min(c) && v <= Max(c))
      return SUBSET;
    else
      return EMPTY;
  }
  if (c->vec[v/N] & mask[v%N])
    return SUBSET;
  else
    return EMPTY;
}

intersect interval_in_trail(min,max,c)
     int min;
     int max;
     trail_constraint *c;
{
  bitmask d[MaxL];
  int lmc;

  if (IsInterval(c)) {
    if (min >= Min(c) && max <= Max(c))
      return SUBSET;
    if (max < Min(c) || min > Max(c))
      return EMPTY;
    if (min > Min(c))
      Min(c) = min;
    if (max < Max(c))
      Max(c) = max;
    return NONEMPTY;
  }
  range(min,max,d);
  lmc = (max/N < LastMask(c) ? max/N : LastMask(c));
  SetLastMask(c,lmc);
  return intersect_dom(c->vec,d,LastMask(c),max/N);
}

intersect domain_in_trail(d,lm,c)
     bitmask *d;
     int lm;
     trail_constraint *c;
{
  int lmc;

  if (IsInterval(c)) {
    UnSetInterval(c);
    NEWX(c->vec,bitmask,1+Max(c)/N);
    range(Min(c),Max(c),c->vec);
    lmc = (lm < Max(c)/N ? lm : Max(c)/N);
    SetLastMask(c,lmc);
    return intersect_dom(c->vec,d,lmc,lm);
  }
  lmc = (lm < LastMask(c) ? lm : LastMask(c));
  SetLastMask(c,lmc);
  return intersect_dom(c->vec,d,lmc,lm);
}

intersect intersect_trail(X,c)
     Term X;
     trail_constraint *c;
{
  findom *x;

  if (IsDetermined(X))
    return constant_in_trail(FdVal(X),c);
  x = Finite(X);
  if (IsInterval(x))
    return interval_in_trail(Min(x),Max(x),c);
  return domain_in_trail(Domain(x),LastMask(x),c);
}

void trail(X,andb)
     Term X;
     andbox *andb;
{
  findom *x = Finite(X);
  int i;
  trail_constraint *c;

  NEW(c,trail_constraint);
  c->next = NULL;
  c->method = &(trail_method);
  c->d = X;
  c->dom = x;
  c->min = x->min;
  c->max = x->max;
  c->info = x->info;
  c->trailed = x->trailed;
  c->vec = Domain(x);		/* save old domain of x */
  if (!IsInterval(x)) {         /* create local copy of the domain of x */
    NEWX(Domain(x),bitmask,(LastMask(x)+1));
    for (i=0; i<=LastMask(x); i++) 
      DomainWord(x,i) = c->vec[i];
  }
  x->trailed = (envid *)&(andb->env);
  SuspendAndb(x,andb);
  LinkConstr(andb,(constraint *)c);
  Mark(andb,Home(x->env));
  return;
}

void retrail(c,andb,exs)
     trail_constraint *c;
     andbox *andb;
     exstate *exs;
{
  if (!IsInterval(c)) {
    SetMin(c,min_bit(c->vec,LastMask(c)));
    SetMax(c,max_bit(c->vec,LastMask(c)));
  }
  if (Min(c) == Max(c))
    Determine(c->d,Min(c),andb,exs);
  deinstall_trail(c);
  return;
}

void bind_and_trail_uva(exs, andb, var, val)
     exstate *exs;
     andbox *andb;
     Reference var;
     Term val;
{
  register envid *I = Env(RefTerm(var));

  if (I->env != NULL) {
    /* Handle promoted environments */
    do {
      I = I->env;
    } while (I->env != NULL);
    if (IsLocalEnv(I, andb)) {
      /* It dereferenced to the local environment */
      BindVar(var, val);
      return;
    }
  }
  TrailValue(exs, TagRef(var), TagUva(I));
  BindVar(var, val);
  return;
}

void bind_and_trail_gva(exs, andb, var, val)
     exstate *exs;
     andbox *andb;
     Reference var;
     Term val;
{
  register envid *I = (envid*)(RefGva(var)->env);

  if (I->env != NULL) {
    /* Handle promoted environments */
    do {
      I = I->env;
    } while (I->env != NULL);

    RefGva(var)->env = I;

    if (IsLocalEnv(I, andb)) {
      /* It dereferenced to the local environment */
      BindVar(var, val);
      return;
    }
  }
/* for now
  TrailRef(exs, var);
  BindVar(var, val);
*/
  return;
}

void bind_variable(exs, andb, V, val)
     exstate *exs;
     andbox *andb;
     Term V;
     Term val;
{
  Reference var;
  Deref(V,V);
  if (!IsREF(V))
    return;
  var = Ref(V);
  if(VarIsUVA(var)) {
    if(IsLocalUVA(var,andb)) {
      BindVar(var,val);
    } else {
      bind_and_trail_uva(exs,andb,var,val);
    }
  }
  else {
    if(IsLocalGVA(var,andb)) {
      BindVar(var,val);
    } else {
      bind_and_trail_gva(exs,andb,var,val);
    }
  }
}

void wake_fd_suspensions(x,andb,exs)
     findom *x;
     andbox *andb;
     exstate *exs;
{
  fd_suspension *k, *l, **lp;
  fd_function *fdc;

  if (x->isusp) k=x->isusp->next;
  for(l=x->isusp, lp=&(x->isusp); l; l=l->next, k=(k ? k->next : k)) {
    fdc = l->f;
    switch(Locality(&(fdc->env),andb)) {
    case NOMORE:
      *lp = k;
      continue;
    case CHILD:
      GlobalWakeIn(fdc,exs);
    case SELF:
    case ANCESTOR:
    case SIBLING:
      lp = &((*lp)->next);
      continue;
    }
  }
  return;
}

void wake_agent_suspensions(x,andb,exs)
     findom *x;
     andbox *andb;
     exstate *exs;
{
  suspension *s, **sp;

  sp = &(x->asusp);
  for (s = x->asusp; s!=NULL; s=s->next)
    switch (s->type) {
    case ANDB:
      if (isunder(s->suspended.andb,andb)) {
	Wake(exs,s->suspended.andb);
	*sp = s->next;
      }
      else
	sp = &(s->next);
      break;
    case CHB:
      if(isunder(s->suspended.chb->father,andb)) {
	Recall(exs,s->suspended.chb);
	*sp = s->next;
      }
      else
	sp = &(s->next);
      break;
    }
  return;
}

void wake_trail_suspensions(x,andb,exs)
     findom *x;
     andbox *andb;
     exstate *exs;
{
  suspension *s, **sp;

  sp = &(x->tsusp);
  for (s = x->tsusp; s!=NULL; s=s->next)
    if (s->suspended.andb != andb && isunder(s->suspended.andb,andb)) {
      Wake(exs,s->suspended.andb);
      *sp = s->next;
    }
    else
      sp = &(s->next);
  return;
}

/* C-version of labeling, labeling_ff, one, one_ff.
*/

void fd_trail_value(x,cp)
     findom *x;
     int cp;
{
  PushFdTrail(&(x->next));
  PushFdTrail(&(x->min));
  PushFdTrail(&(x->max));
  PushFdTrail(&(x->info));
  PushFdTrail(&(x->trailed));
  x->trailed = (envid*)cp;
}

inline void reset_trail(cp)
     int cp;
{
  uword l;
  while (fd_cp != cp) {
    l = fd_trail[--fd_cp]; 
    *(uword *)fd_trail[--fd_cp] = l;
  }
}

void fd_bind_var(V,val)
     Term V;
     Term val;
{
  Reference var;
  if (!IsREF(V))
    return;
  var = Ref(V);
  PushFdTrail(var);
  BindVar(var,val);
}

void fd_set_value(X,x,i,cp)
     Term X;
     findom *x;
     int i;
     int cp;
{
  Term n;
  if (FdTrailP(x,cp))
    fd_trail_value(x,cp);
  MakeIntegerTerm(n,i);
  fd_bind_var(X,n);
}

void set_dom(x,t)
     findom *x;
     domstruct *t;
{
  int i;

  switch (t->tag) {
  case INTERVAL:
    return;
  case DOMAIN:
    if (IsInterval(x)) {
      UnSetInterval(x);
      PushFdTrail(&(Domain(x)));
      NEWX(Domain(x),bitmask,(t->lm+1));
      for (i=0;i<=t->lm;i++)
	DomainWord(x,i) = t->d[i];
      SetLastMask(x,t->lm);
      return;
    }
    for (i=0;i<=t->lm;i++) {
      PushFdTrail(&(DomainWord(x,i)));
      DomainWord(x,i) = t->d[i];
    }
    SetLastMask(x,t->lm);
    return;
  }
}

dom_info monoton(ms)
     monstruct *ms;
{
  int s=1, g=1;
  moncons *l;

  l = ms->mon;
  while (l) {
    if (!IsDetermined(l->d)) goto cont0;
    l = l->next;
  }
  s = 2;
 cont0:
  if (ms->mon != l) {
    PushFdTrail(&(ms->mon));
    ms->mon = l;
  }
  l = ms->amon;
  while (l) {
    if (!IsDetermined(l->d)) goto cont1;
    l = l->next;
  }
  g = 3;
 cont1:
  if (ms->amon != l) {
    PushFdTrail(&(ms->amon));
    ms->amon = l;
  }
  switch (s*g) {
  case 1:
    return BOTTOM;
  case 2:
    return SHRINKING;
  case 3:
    return GROWING;
  case 6:
    return CONSTANT;
  }
}

dep_class prop_info(X,t)
     Term X;
     domstruct *t;
{
  long k1,k2,k3,k4;
  Term n;
  findom *x = Finite(X);
  
  if (t->tag == INTERVAL) {
    k2 = Min0(t->d);
    k4 = Max0(t->d);
  }
  else {
    k2 = min_bit(t->d,t->lm);
    k4 = max_bit(t->d,t->lm);
  }
  k1 = Min(x);
  k3 = Max(x);
  if (k1 < k2) 
    SetMin(x, k2);
  if (k4 < k3) 
    SetMax(x, k4);
  if (k2 == k4) {
    MakeIntegerTerm(n,k2);
    fd_bind_var(X,n);
  }
  return prop_table[k1==k2?0:1][k3==k4?0:1];
}

bool fd_prop(x,prop,cp)
     findom *x;
     dep_class prop;
     int cp;
{
  domstruct t;
  findom *fd;
  fd_function *fdc;
  dep_class pr;
  fd_suspension *l;
  dom_info di;
  QueueDecl;
  Term X;

  InitQueue(x,prop,fd_time);
  do {
    for(l=x->isusp; l; l=l->next) {
      fdc = l->f;
      if (Ent(fdc))
	continue;
      if (LastExec(fdc) > Time(x))
	continue;
      if (not_affected[l->dep][Change(x)])
	continue;
      di = monoton(fdc->moninfo);
      if (Bottom(di))
	continue;
      if (Tell(fdc) && Growing(di))
	continue;
      evaluate_range(fdc->c,fdc->args,&t);
      SetLastExec(fdc,fd_time);
      Deref(X,fdc->d);
      switch (intersect_fd(X,&t)) {
      case EMPTY:
	if (Growing(di)) {
	  continue;
	}
	return FALSE;
      case SUBSET:
	if (Shrinking(di))
	  continue;
	IndexicalEnt(fdc);
	SetEnt(fdc->ent);
	continue;
      case NONEMPTY:
	if (Growing(di))
	  continue;
	fd = Finite(X);
	if (FdTrailP(fd,cp))
	  fd_trail_value(fd,cp);
	pr = prop_info(X,&t);
	if (!IsDetermined(X)) 
	  set_dom(fd,&t);
	if (Queued(fd)) {
	  UpdateQueued(fd,pr,fd_time);
	}
	else {
	  Enqueue(fd,fdc,pr,fd_time);
	}
	if (Constant(di)) {
	  IndexicalEnt(fdc);
	  SetEnt(fdc->ent);
	}
	continue;
      }
    }
    Dequeue(x);
  } while (x);
  return TRUE;
}

bool fd_one_solution(L)
     Term L;
{
  Term X;
  findom *x;
  int min, max, m, cp;
  
  while (1) {
    if (Eq(L,NIL))
      return TRUE;
    Deref(X,LstCar(Lst(L)));
    Deref(L,LstCdr(Lst(L)));
    if (IsDetermined(X))
      continue;
    else
      break;
  }
  x = Finite(X);
  min = Min(x);
  max = Max(x);
  cp = fd_cp;
  for (m=min; m<=max; m++) {
    if (!in_dom(m,x)) 
      continue;
    fd_set_value(X,x,m,cp);
    if (fd_prop(x,PropV(m,min,max),cp) == FALSE) {
      reset_trail(cp);
      continue;
    }
    if (fd_one_solution(L) == FALSE) {
      reset_trail(cp);
      continue;
    }
    return TRUE;
  }
  return FALSE;
}

bool fd_one_solution_ff(L)
     Term L;
{
  Term Car, Cdr, X;
  findom *x, *y;
  int min, max, m, cp, sz, szm, k, lm;
  
  while (1) {
    if (Eq(L,NIL))
      return TRUE;
    Deref(X,LstCar(Lst(L)));
    if (IsDetermined(X)) {
      Deref(L,LstCdr(Lst(L)));
      continue;
    }
    else
      break;
  }
  x = Finite(X);
  if  (IsInterval(x))
    szm = (Max(x) - Min(x)) + 1;
  else {
    lm=LastMask(x);
    for (szm=0, k=0; k<=lm; k++)
      szm += number_of_bits(DomainWord(x,k));
  }
  Deref(Cdr,LstCdr(Lst(L)));
  while (!Eq(Cdr,NIL)) {		/* find smallest domain */
    Deref(Car,LstCar(Lst(Cdr)));
    if (!IsDetermined(Car)) {
      y = Finite(Car);
      if  (IsInterval(y))
	sz = (Max(y) - Min(y)) + 1;
      else {
	lm=LastMask(y);
	for (sz=0, k=0; k<=lm; k++)
	  sz += number_of_bits(DomainWord(y,k));
      }
      if (sz < szm) {
	X = Car;
	x = y;
	szm = sz;
      }
    }
    Deref(Cdr,LstCdr(Lst(Cdr)));
  }
  min = Min(x);
  max = Max(x);
  cp = fd_cp;
  for (m=min; m<=max; m++) {
    if (!in_dom(m,x)) 
      continue;
    fd_set_value(X,x,m,cp);
    if (fd_prop(x,PropV(m,min,max),cp) == FALSE) {
      reset_trail(cp);
      continue;
    }
    if (fd_one_solution_ff(L) == FALSE) {
      reset_trail(cp);
      continue;
    }
    return TRUE;
  }
  return FALSE;
}

void fd_numb_of(L)
     Term L;
{
  Term X;
  findom *x;
  int min, max, m, cp;
  
  while (1) {
    if (Eq(L,NIL)) {
      fd_number_of_solutions++;
      return;
    }
    Deref(X,LstCar(Lst(L)));
    Deref(L,LstCdr(Lst(L)));
    if (IsDetermined(X))
      continue;
    else
      break;
  }
  x = Finite(X);
  min = Min(x);
  max = Max(x);
  cp = fd_cp;
  for (m=min; m<=max; m++) {
    if (!in_dom(m,x)) 
      continue;
    fd_set_value(X,x,m,cp);
    if (fd_prop(x,PropV(m,min,max),cp) == FALSE) {
      reset_trail(cp);
      continue;
    }
    fd_numb_of(L);
    reset_trail(cp);
  }
}

void fd_numb_of_ff(L)
     Term L;
{
  Term Car, Cdr, X;
  findom *x, *y;
  int min, max, m, cp, sz, szm, k, lm;
    
  while (1) {
    if (Eq(L,NIL)) {
      fd_number_of_solutions++;
      return;
    }
    Deref(X,LstCar(Lst(L)));
    if (IsDetermined(X)) {
      Deref(L,LstCdr(Lst(L)));
      continue;
    }
    else
      break;
  }
  x = Finite(X);
  if  (IsInterval(x))
    szm = (Max(x) - Min(x)) + 1;
  else {
    lm=LastMask(x);
    for (szm=0, k=0; k<=lm; k++)
      szm += number_of_bits(DomainWord(x,k));
  }
  Deref(Cdr,LstCdr(Lst(L)));
  while (!Eq(Cdr,NIL)) {	/* find smallest domain */
    Deref(Car,LstCar(Lst(Cdr)));
    if (!IsDetermined(Car)) {
      y = Finite(Car);
      if  (IsInterval(y))
	sz = (Max(y) - Min(y)) + 1;
      else {
	lm=LastMask(y);
	for (sz=0, k=0; k<=lm; k++)
	  sz += number_of_bits(DomainWord(y,k));
      }
      if (sz < szm) {
	X = Car;
	x = y;
	szm = sz;
      }
    }
    Deref(Cdr,LstCdr(Lst(Cdr)));
  }
  min = Min(x);
  max = Max(x);
  cp = fd_cp;
  for (m=min; m<=max; m++) {
    if (!in_dom(m,x)) 
      continue;
    fd_set_value(X,x,m,cp);
    if (fd_prop(x,PropV(m,min,max),cp) == FALSE) {
      reset_trail(cp);
      continue;
    }
    fd_numb_of_ff(L);
    reset_trail(cp);
  }
}
