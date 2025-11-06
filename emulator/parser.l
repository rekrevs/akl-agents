%{
/* $Id: parser.l,v 1.27 1994/05/05 09:36:53 secher Exp $ */ 

#include "parser.tab.h"

#include <stdio.h>



%}

%%
"switch_on_term"	{ return TOKEN_SWITCH_ON_TERM; }

"fd_code"               { return FD_code_T; }
"const"                 { return FD_const_T; }
"element_x"             { return FD_element_x_T; }
"element_i"             { return FD_element_i_T; }
"val"                   { return FD_val_T; }
"val_0"                 { return FD_val_0_T; }
"val_1"                 { return FD_val_1_T; }
"val_2"                 { return FD_val_2_T; }
"max_elem"              { return FD_max_elem_T; }
"add"                   { return FD_add_T; }
"sub"                   { return FD_sub_T; }
"mult"                  { return FD_mult_T; }
"divd"                  { return FD_divd_T; }
"divu"                  { return FD_divu_T; }
"mod"                   { return FD_mod_T; }
"min"                   { return FD_min_T; }
"max"                   { return FD_max_T; }
"dom_min"               { return FD_dom_min_T; }
"dom_min_0"             { return FD_dom_min_0_T; }
"dom_min_1"             { return FD_dom_min_1_T; }
"dom_min_2"             { return FD_dom_min_2_T; }
"dom_max"               { return FD_dom_max_T; }
"dom_max_0"             { return FD_dom_max_0_T; }
"dom_max_1"             { return FD_dom_max_1_T; }
"dom_max_2"             { return FD_dom_max_2_T; }
"dom"                   { return FD_dom_T; }
"dom_0"                 { return FD_dom_0_T; }
"dom_1"                 { return FD_dom_1_T; }
"dom_2"                 { return FD_dom_2_T; }
"range"                 { return FD_range_T; }
"outer_range"           { return FD_outer_range_T; }
"setadd"                { return FD_setadd_T; }
"setsub"                { return FD_setsub_T; }
"setmod"                { return FD_setmod_T; }
"compl"                 { return FD_compl_T; }
"union"                 { return FD_union_T; }
"inter"                 { return FD_inter_T; }
"fd_check"              { return FD_check_T; }
"fd_label"              { return FD_label_T; }

"try_single"		{ return TOKEN_TRY_SINGLE; }
"try"			{ return TOKEN_TRY; }	
"retry"			{ return TOKEN_RETRY; }
"trust"			{ return TOKEN_TRUST; }

"guard_unorder"		{ return TOKEN_GUARD_UNORDER; }
"guard_order"		{ return TOKEN_GUARD_ORDER; }
"guard_unit"		{ return TOKEN_GUARD_UNIT; }

"guard_cond"		{ return TOKEN_GUARD_COND; }
"guard_commit"		{ return TOKEN_GUARD_COMMIT; }
"guard_wait"		{ return TOKEN_GUARD_WAIT; }
"guard_cut"		{ return TOKEN_GUARD_CUT; }

"allocate"		{ return TOKEN_ALLOCATE; }
"deallocate"		{ return TOKEN_DEALLOCATE; }
"meta_call"		{ return TOKEN_META_CALL; }
"call"			{ return TOKEN_CALL; }
"execute"		{ return TOKEN_EXECUTE; }
"dealloc_execute"	{ return TOKEN_DEALLOC_EXECUTE; }
"extern_call"		{ return TOKEN_EXTERN_CALL; }
"extern_execute"	{ return TOKEN_EXTERN_EXECUTE; }
"proceed"		{ return TOKEN_PROCEED; }
"dealloc_proceed"	{ return TOKEN_DEALLOC_PROCEED; }
"call_apply"            { return TOKEN_CALL_APPLY; }
"execute_apply"         { return TOKEN_EXECUTE_APPLY; }
"dealloc_execute_apply" { return TOKEN_DEALLOC_EXECUTE_APPLY; }
"fail"			{ return TOKEN_FAIL; }

"get_x_variable"	{ return TOKEN_GET_X_VARIABLE; }
"get_y_variable"	{ return TOKEN_GET_Y_VARIABLE; }
"get2_y_variable"	{ return TOKEN_GET2_Y_VARIABLE; }
"get3_y_variable"	{ return TOKEN_GET3_Y_VARIABLE; }
"get4_y_variable"	{ return TOKEN_GET4_Y_VARIABLE; }
"get5_y_variable"	{ return TOKEN_GET5_Y_VARIABLE; }
"get_x_value"		{ return TOKEN_GET_X_VALUE; }
"get_y_value"		{ return TOKEN_GET_Y_VALUE; }
"get_constant"		{ return TOKEN_GET_CONSTANT; }
"get_nil"		{ return TOKEN_GET_NIL; }
"get_structure"		{ return TOKEN_GET_STRUCTURE; }
"get_record"           { return TOKEN_GET_RECORD; }
"get_list"		{ return TOKEN_GET_LIST; }
"get_list_x0"		{ return TOKEN_GET_LIST_X0; }
"get_nil_x0"		{ return TOKEN_GET_NIL_X0; }
"get_structure_x0"	{ return TOKEN_GET_STRUCTURE_X0; }
"get_constant_x0"	{ return TOKEN_GET_CONSTANT_X0; }
"get_abstraction"	{ return TOKEN_GET_ABSTRACTION; }

"put_x_void"		{ return TOKEN_PUT_X_VOID; }
"put_y_void"		{ return TOKEN_PUT_Y_VOID; }
"put_x_variable"	{ return TOKEN_PUT_X_VARIABLE; }
"put_y_variable"	{ return TOKEN_PUT_Y_VARIABLE; }
"put_x_value"		{ return TOKEN_PUT_X_VALUE; }
"put_y_value"		{ return TOKEN_PUT_Y_VALUE; }
"put2_y_value"		{ return TOKEN_PUT2_Y_VALUE; }
"put3_y_value"		{ return TOKEN_PUT3_Y_VALUE; }
"put4_y_value"		{ return TOKEN_PUT4_Y_VALUE; }
"put5_y_value"		{ return TOKEN_PUT5_Y_VALUE; }
"put_constant"		{ return TOKEN_PUT_CONSTANT; }
"put_nil"		{ return TOKEN_PUT_NIL; }
"put_structure"		{ return TOKEN_PUT_STRUCTURE; }
"put_record"            { return TOKEN_PUT_RECORD; }
"put_list"		{ return TOKEN_PUT_LIST; }
"put_abstraction"	{ return TOKEN_PUT_ABSTRACTION; }

"unify_void"		{ return TOKEN_UNIFY_VOID; }
"unify_y_variable"	{ return TOKEN_UNIFY_Y_VARIABLE; }
"unify_x_variable"	{ return TOKEN_UNIFY_X_VARIABLE; }
"unify_x_value"		{ return TOKEN_UNIFY_X_VALUE; }
"unify_y_value"		{ return TOKEN_UNIFY_Y_VALUE; }
"unify_constant"	{ return TOKEN_UNIFY_CONSTANT; }
"unify_nil"		{ return TOKEN_UNIFY_NIL; }
"unify_list"		{ return TOKEN_UNIFY_LIST; }
"unify_structure"       { return TOKEN_UNIFY_STRUCTURE; }

"switch_on_constant"	{ return TOKEN_SWITCH_ON_CONSTANT; }
"switch_on_structure"	{ return TOKEN_SWITCH_ON_STRUCTURE; }

"send3"			{ return TOKEN_SEND3; }

"suspend_flat"		{ return TOKEN_SUSPEND_FLAT; }

"fail_dec"		{ return TOKEN_FAIL_DEC; }
"load_nil"		{ return TOKEN_LOAD_NIL; }
"load_atom"		{ return TOKEN_LOAD_ATOM; }
"load_integer"		{ return TOKEN_LOAD_INTEGER; }
"read_car"		{ return TOKEN_READ_CAR; }
"read_cdr"		{ return TOKEN_READ_CDR; }
"read_arg"		{ return TOKEN_READ_ARG; }
"set_reg_reg"		{ return TOKEN_SET_REG_REG; }
"set_reg_yvar"		{ return TOKEN_SET_REG_YVAR; }
"set_yvar_reg"		{ return TOKEN_SET_YVAR_REG; }
"term_switch"		{ return TOKEN_TERM_SWITCH; }
"atom_switch"		{ return TOKEN_ATOM_SWITCH; }
"atom_with_else_switch"	{ return TOKEN_ATOM_WITH_ELSE_SWITCH; }
"atom_with_atom_else_switch" {
			  return TOKEN_ATOM_WITH_ATOM_ELSE_SWITCH; }
"atom_type"		{ return TOKEN_ATOM_TYPE; }
"atom_type_with_else"	{ return TOKEN_ATOM_TYPE_WITH_ELSE; }
"known_atom_switch"	{ return TOKEN_KNOWN_ATOM_SWITCH; }
"known_atom_with_atom_else_switch" {
			  return TOKEN_KNOWN_ATOM_WITH_ATOM_ELSE_SWITCH; }
"integer_switch"	{ return TOKEN_INTEGER_SWITCH; }
"integer_with_else_switch" {
			  return TOKEN_INTEGER_WITH_ELSE_SWITCH; }
"integer_with_integer_else_switch" {
			  return TOKEN_INTEGER_WITH_INTEGER_ELSE_SWITCH; }
"integer_type"		{ return TOKEN_INTEGER_TYPE; }
"integer_type_with_else" {
			  return TOKEN_INTEGER_TYPE_WITH_ELSE; }
"known_integer_switch"	{ return TOKEN_KNOWN_INTEGER_SWITCH; }
"known_integer_with_integer_else_switch" {
			  return TOKEN_KNOWN_INTEGER_WITH_INTEGER_ELSE_SWITCH; }
"list_switch"		{ return TOKEN_LIST_SWITCH; }
"list_type_with_else"	{ return TOKEN_LIST_TYPE_WITH_ELSE; }
"struct_type"		{ return TOKEN_STRUCT_TYPE; }
"struct_type_with_else"	{ return TOKEN_STRUCT_TYPE_WITH_ELSE; }
"known_functor_switch"	{ return TOKEN_KNOWN_FUNCTOR_SWITCH; }
"known_functor_with_functor_else_switch" {
			  return TOKEN_KNOWN_FUNCTOR_WITH_FUNCTOR_ELSE_SWITCH; }
"int_compare_reg_reg"	{ return TOKEN_INT_COMPARE_REG_REG; }
"int_compare_reg_int"	{ return TOKEN_INT_COMPARE_REG_INT; }
"eq_reg_reg"		{ return TOKEN_EQ_REG_REG; }
"match_reg_reg"		{ return TOKEN_MATCH_REG_REG; }
"match_reg_reg_off"	{ return TOKEN_MATCH_REG_REG_OFF; }
"match_reg_atom"	{ return TOKEN_MATCH_REG_ATOM; }
"match_reg_off_atom"	{ return TOKEN_MATCH_REG_OFF_ATOM; }
"match_reg_integer"	{ return TOKEN_MATCH_REG_INTEGER; }
"match_reg_off_integer"	{ return TOKEN_MATCH_REG_OFF_INTEGER; }
"match_reg_nil"		{ return TOKEN_MATCH_REG_NIL; }
"match_reg_off_nil"	{ return TOKEN_MATCH_REG_OFF_NIL; }
"match_reg_yvar"	{ return TOKEN_MATCH_REG_YVAR; }
"match_yvar_yvar"	{ return TOKEN_MATCH_YVAR_YVAR; }
"match_yvar_reg_off"	{ return TOKEN_MATCH_YVAR_REG_OFF; }
"match_reg_H_list"	{ return TOKEN_MATCH_REG_H_LIST; }
"match_yvar_H_list"	{ return TOKEN_MATCH_YVAR_H_LIST; }
"match_reg_H_struct"	{ return TOKEN_MATCH_REG_H_STRUCT; }
"match_yvar_H_struct"	{ return TOKEN_MATCH_YVAR_H_STRUCT; }
"match_reg_struct"	{ return TOKEN_MATCH_REG_STRUCT; }
"match_reg_struct_level" {
			  return TOKEN_MATCH_REG_STRUCT_LEVEL; }
"match_reg_off_struct"	{ return TOKEN_MATCH_REG_OFF_STRUCT; }
"match_reg_off_struct_level" {
			  return TOKEN_MATCH_REG_OFF_STRUCT_LEVEL; }
"match_yvar_struct"	{ return TOKEN_MATCH_YVAR_STRUCT; }
"match_yvar_struct_level" {
			  return TOKEN_MATCH_YVAR_STRUCT_LEVEL; }
"match_reg_list_car"	{ return TOKEN_MATCH_REG_LIST_CAR; }
"match_reg_list_car_level" {
			  return TOKEN_MATCH_REG_LIST_CAR_LEVEL; }
"match_reg_off_list_car" {
			  return TOKEN_MATCH_REG_OFF_LIST_CAR; }
"match_reg_off_list_car_level" {
			  return TOKEN_MATCH_REG_OFF_LIST_CAR_LEVEL; }
"match_yvar_list_car"	{ return TOKEN_MATCH_YVAR_LIST_CAR; }
"match_yvar_list_car_level" {
			  return TOKEN_MATCH_YVAR_LIST_CAR_LEVEL; }
"match_reg_list_cdr"	{ return TOKEN_MATCH_REG_LIST_CDR; }
"match_reg_list_cdr_level" {
			  return TOKEN_MATCH_REG_LIST_CDR_LEVEL; }
"match_reg_off_list_cdr" {
		          return TOKEN_MATCH_REG_OFF_LIST_CDR; }
"match_reg_off_list_cdr_level" {
			  return TOKEN_MATCH_REG_OFF_LIST_CDR_LEVEL; }
"match_yvar_list_cdr"	{ return TOKEN_MATCH_YVAR_LIST_CDR; }
"match_yvar_list_cdr_level" {
			  return TOKEN_MATCH_YVAR_LIST_CDR_LEVEL; }
"push_list_imm"		{ return TOKEN_PUSH_LIST_IMM; }
"push_list"		{ return TOKEN_PUSH_LIST; }
"push_structure_imm"	{ return TOKEN_PUSH_STRUCTURE_IMM; }
"push_structure"	{ return TOKEN_PUSH_STRUCTURE; }
"push_nil"		{ return TOKEN_PUSH_NIL; }
"push_reg"		{ return TOKEN_PUSH_REG; }
"push_yvar"		{ return TOKEN_PUSH_YVAR; }
"push_void"		{ return TOKEN_PUSH_VOID; }
"set_reg_H_and_push_void" {
			  return TOKEN_SET_REG_H_AND_PUSH_VOID; }
"set_yvar_H_and_push_void" {
			  return TOKEN_SET_YVAR_H_AND_PUSH_VOID; }
"set_reg_and_yvar_H_and_push_void" {
			  return TOKEN_SET_REG_AND_YVAR_H_AND_PUSH_VOID; }
"push_atom"		{ return TOKEN_PUSH_ATOM; }
"push_integer"		{ return TOKEN_PUSH_INTEGER; }
"push_functor"		{ return TOKEN_PUSH_FUNCTOR; }
"set_reg_H"		{ return TOKEN_SET_REG_H; }
"set_yvar_H"		{ return TOKEN_SET_YVAR_H; }
"btry"			{ return TOKEN_BTRY; }
"btrust"		{ return TOKEN_BTRUST; }
"test_try"		{ return TOKEN_TEST_TRY; }
"test_trust"		{ return TOKEN_TEST_TRUST; }
"s_try"			{ return TOKEN_S_TRY; }
"s_trust"		{ return TOKEN_S_TRUST; }
"back"			{ return TOKEN_BACK; }
"gen_try"		{ return TOKEN_GEN_TRY; }
"gen_trust"		{ return TOKEN_GEN_TRUST; }
"bexecute_simple"	{ return TOKEN_BEXECUTE_SIMPLE; }
"bexecute"		{ return TOKEN_BEXECUTE; }
"bcall"			{ return TOKEN_BCALL; }
"jump_gc_and_dump_cont"	{ return TOKEN_JUMP_GC_AND_DUMP_CONT; }
"jump_gc"		{ return TOKEN_JUMP_GC; }
"jump"			{ return TOKEN_JUMP; }
"bproceed_simple"	{ return TOKEN_BPROCEED_SIMPLE; }
"bproceed"		{ return TOKEN_BPROCEED; }
"create_cont"		{ return TOKEN_CREATE_CONT; }
"test_level"		{ return TOKEN_TEST_LEVEL; }
"constraint_stack_test"	{ return TOKEN_CONSTRAINT_STACK_TEST; }
"jump_on_nonempty_constraint_stack" {
			  return TOKEN_JUMP_ON_NONEMPTY_CONSTRAINT_STACK; }
"alias_check"		{ return TOKEN_ALIAS_CHECK; }
"alias_check_reg"	{ return TOKEN_ALIAS_CHECK_REG; }
"fail_on_alias"		{ return TOKEN_FAIL_ON_ALIAS; }
"choice_quiet_cut"	{ return TOKEN_CHOICE_QUIET_CUT; }
"suspend_lone_var"	{ return TOKEN_SUSPEND_LONE_VAR; }
"bsuspend"		{ return TOKEN_BSUSPEND; }
"suspend_without_reg"	{ return TOKEN_SUSPEND_WITHOUT_REG; }
"create_guard"		{ return TOKEN_CREATE_GUARD; }
"flat_quiet_cut"	{ return TOKEN_FLAT_QUIET_CUT; }
"flat_quiet_commit"	{ return TOKEN_FLAT_QUIET_COMMIT; }
"flat_noisy_wait"	{ return TOKEN_FLAT_NOISY_WAIT; }
"shortcut_quiet_cut"	{ return TOKEN_SHORTCUT_QUIET_CUT; }
"shortcut_quiet_commit"	{ return TOKEN_SHORTCUT_QUIET_COMMIT; }
"shortcut_noisy_wait"	{ return TOKEN_SHORTCUT_NOISY_WAIT; }
"deep_quiet_cut"	{ return TOKEN_DEEP_QUIET_CUT; }
"deep_quiet_commit"	{ return TOKEN_DEEP_QUIET_COMMIT; }
"deep_noisy_wait"	{ return TOKEN_DEEP_NOISY_WAIT; }

"label"			{ return LABEL; }
"plabel"		{ return PLABEL; }
"predicate"		{ return PREDICATE; }

"l"			{ return LABELTAG; }

"constant"		{ return CONSTANT; }
"list"			{ return LIST; }
"struct"		{ return STRUCT; }
"typed"			{ return TYPED; }
"integer"		{ return INTEGER; }
"atom"			{ return ATOM; }
"functor"		{ return FUNCTOR; }
"reg"			{ return REG; }
"void"			{ return VOID; }
"labeled"		{ return LABELED; }

[a-zA-Z_$=\\][[a-zA-Z_0-9$=\\]* { return ATOMNAME; }

"'"([\01-\46\50-\377]|"''")*"'" {
  /* shift the characters one step to the left to avoid the quote */
  int i=0;
  int j=1;

  parseatom:
    if(yytext[j] == 39)
      /* j is pointing to a quote */
      if(yytext[j+1] == 39)
        /* it was a quoted quote, continue */
	j++;
      else {
        /* place a end-of-string at the end */
	yytext[i]=0;
	return ATOMNAME;
      }
    yytext[i]=yytext[j];
    i++; j++;
    goto parseatom;
  }

"("			{ return LPAR; }
")"			{ return RPAR; }
"["			{ return LHPAR; }
"]"			{ return RHPAR; }
","			{ return COMMA; }
"/"			{ return SLASH; }
"-"?[0-9]+		{ return NUMBER; }
"-"?[0-9]+"."[0-9]+([Ee][+-]?[0-9]+)?	{ return FLOAT; }
"-"			{ return DASH; }
[\n\t ]			;
%.*$			;
<<EOF>>			{ return END; }

%%
#ifndef yywrap
int yywrap()	/* Expected to be a function in flex 2.4.1 and later */
{
  return 1;
}
#endif

