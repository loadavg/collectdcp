/*  File:    model.pl
    Author:  Carlo,,,
    Created: Oct  6 2015
    Purpose: collectd editor model data
    License  : MIT
    Copyright: (c) 2015 Sputnik7
*/

:- module(model,
	 [conf_load_file/3
	 ,base_folder/1
	 ,conf_file/2
	 ,ast_locate_cursor/3
	 ,ast_range_cursor/3
	 %,ast_enable_block/5
	 %,ast_disable_block/5
	 ]).

:- use_module(parse_conf).

%%	conf_load_file(+Conf, -Cs, -Ast) is semidet
%
%	load symbolic file to buffer Cs and parse it to Ast
%
conf_load_file(Conf, Cs, Ast) :-
	conf_file(Conf, File),
	read_file_to_codes(File, Cs, []),
	parse_conf_cs(Cs, Ast).

%%	base_folder(-FolderBase)
%
%	get the base directory of .conf files
%
:- if(false).
base_folder('/etc/collectd').
:- elif(true).
base_folder(Base) :-
	module_property(model, file(File)),
	path_list(File, List),
    append(Folder, [_], List),
    append(Folder, ['ubuntu-14.04-default'], Sample),
    path_list(Base, Sample).
:- else.
base_folder(Base) :-
	module_property(model, file(File)),
	path_list(File, List),
	append(Base1, [_,_,_], List),
	append(Base1, [collectd], Base2),
	path_list(Base, Base2).
:- endif.

path_list(Atom, List) :-
	atomic_list_concat(List, /, Atom).

%%	conf_file(+File, -Path)
%
%	apply folder naming convention to
%	get full path from a symbolic (like filters) file name
%
conf_file(File, Path) :-
	atom_concat('collectd.conf.d/', File, Sub),
	conf_file_loc(Sub, Path), exists_file(Path).
conf_file(File, Path) :-
	conf_file_loc(File, Path).

%%	conf_file_loc(+File, -Path)
%
%	patch symbolic file name with folder and extension
%
conf_file_loc(File, Path) :-
	base_folder(Base),
	(   sub_atom(File, _,_,0, '.conf')
	->  atom_concat(Base, File, Path)
	;   format(atom(Path), '~s/~s.conf', [Base, File])
	).
/*
ast_enable_block(Ast, _Cs, El, _AstUp, _CsUp) :-
	 ast_locate_cursor(Ast, 10, El).
ast_disable_block(Ast, _Cs, El, _AstUp, _CsUp) :-
	 ast_locate_cursor(Ast, 10, El).
*/

%%	ast_locate_cursor(+Ast, +Cursor, -El)
%
%	find first element El in Ast matching Cursor
%
ast_locate_cursor(Ast, Cursor, El) :-
	member(El, Ast),
	ast_range_cursor(El, X, Y),
	Cursor >= X, Cursor =< Y.

%%	ast_range_cursor(+Elem,-X,-Y)
%
%	get range covered by Elem
%
ast_range_cursor(comment(X,Y,_), X,Y) :- !.
ast_range_cursor(xml(tag(_,X,_,_,Y),_,_), X,Y) :- !.
ast_range_cursor(pair(X,Y,_), X,Y) :- !.
ast_range_cursor(skip(X,Y,_), X,Y) :- !.
ast_range_cursor(Unknown, _,_) :- throw(unknow_ast_element(Unknown)).
