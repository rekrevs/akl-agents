% $Id: aux.pl,v 1.6 1992/11/22 19:40:48 sverker Exp $

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%
%%%	File: aux.pl
%%%	Author: olin (Peter Olin)
%%%	Date:		
%%%     Modified: 18 July, 1990 for Andorra by Johan Bevemyr
%%%	Purpose:	
%%%
%%% HISTORY
%%% Author	Date		Description
%%% PO		7/10-89		Created
%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%
%%%  compiler_version(Version).
%%%

compiler_version('v1.2b').

%%%
%%% head(Clause,Head)
%%%

head(clause(_,_,hb(head(Head,_),_)), Head).


%%%
%%% body(Clause,Guard)
%%%

body(clause(_,_,hb(_,body(Guard))), Guard).

%%%
%%% label(Clause,Label)
%%%

label(clause(_,L,_),L).


%%%
%%% permvars(Clause,Permvars)
%%%

permvars(clause(NrPermVars,_,_), NrPermVars).



% NU-Prolog
/*
:- op(970,fx,(if)).
:- op(960,xfx,(else)).
:- op(955,xfx,(then)).
:- op(950,fx,(some)).
:- op(950,fx,(all)).
:- op(920,xfy,(=>)).
:- op(920,xfy,(<=>)).
:- op(920,xfy,(<=)).

:- op(900,fy,(once)).
:- op(900,fy,(not)).
:- op(700,xfx,(~=)).
*/

% (SICStus) Prolog

xcurrent_op(200,xfy,(^)).
xcurrent_op(300,xfx,(mod)).
xcurrent_op(400,yfx,(//)).
xcurrent_op(400,yfx,(*)).
xcurrent_op(400,yfx,(/)).
xcurrent_op(400,yfx,(<<)).
xcurrent_op(400,yfx,(>>)).
xcurrent_op(500,yfx,(+)).
xcurrent_op(500,fx,(+)).
xcurrent_op(500,yfx,(-)).
xcurrent_op(500,fx,(-)).
xcurrent_op(500,yfx,(/\)).
xcurrent_op(500,yfx,(\/)).
xcurrent_op(500,yfx,(#)).
xcurrent_op(700,xfx,(=)).
xcurrent_op(700,xfx,(=:=)).
xcurrent_op(700,xfx,(=\=)).
xcurrent_op(700,xfx,(<)).
xcurrent_op(700,xfx,(>)).
xcurrent_op(700,xfx,(=<)).
xcurrent_op(700,xfx,(>=)).
xcurrent_op(700,xfx,(==)).
xcurrent_op(700,xfx,(\==)).
xcurrent_op(700,xfx,(@<)).
xcurrent_op(700,xfx,(@>)).
xcurrent_op(700,xfx,(@=<)).
xcurrent_op(700,xfx,(@>=)).
xcurrent_op(700,xfx,('=..')).
xcurrent_op(700,xfx,(is)).
xcurrent_op(900,fy,(\+)).
xcurrent_op(900,fy,(nospy)).
xcurrent_op(900,fy,(spy)).
xcurrent_op(1000,xfy,(',')).
xcurrent_op(1100,xfy,(;)).
xcurrent_op(1200,xfx,(:-)).
xcurrent_op(1200,fx,(:-)).
xcurrent_op(1200,fx,(?-)).
xcurrent_op(1200,xfx,(-->)).

% NU-Prolog

% xcurrent_op(970,fx,(if)).	% 1170
% xcurrent_op(960,xfx,(else)).	% 1160
% xcurrent_op(955,xfx,(then)).	% 1150
% xcurrent_op(950,fx,(some)).
% xcurrent_op(950,fx,(all)).
% xcurrent_op(920,xfy,(=>)).
% xcurrent_op(920,xfy,(<=>)).
% xcurrent_op(920,xfy,(<=)).
% xcurrent_op(900,fy,(once)).
% xcurrent_op(900,fy,(not)).
% xcurrent_op(700,xfx,(~=)).

% Lambda

xcurrent_op(500,xfx,(\)).
xcurrent_op(500,xfx,(\\)).

% AKL

xcurrent_op(1025,xfy,(&)).

xcurrent_op(1050,xfx,('|')).
xcurrent_op(1050,fx,('|')).
xcurrent_op(1050,xfx,(->)).
xcurrent_op(1050,fx,(->)).
xcurrent_op(1050,xfx,(?)).
xcurrent_op(1050,fx,(?)).
xcurrent_op(1050,xfx,(??)).
xcurrent_op(1050,fx,(??)).
xcurrent_op(1050,xfx,(!)).
xcurrent_op(1050,fx,(!)).

top :-	unix(argv(As)), top(As).

top([]).
top(As) :- As = [_|_], compile_files(As).

compile_files([]) :- halt.
compile_files([F|R]) :-
	format("~nCompiling ~w ...~n~n", [F]),
	compilef(F),
	compile_files(R).
