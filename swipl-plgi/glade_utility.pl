/** <module> glade_utility
 *
 *  some snippet to handle glade files
 *  --------
 *
 *  source file glade_utility.pl
 *  created at mer dic 16 18:20:08 2015
 *
 *  @author carlo
 *  @version 0.9.9
 *  License  : MIT
 *  Copyright (c) 2015,2016 Sputnik7
 */

:- module(glade_utility,
	[fix_version/1
	,change_elem_list/4
	]).

:- use_module(library(sgml)).
:- use_module(library(sgml_write)).
% :- use_module(library(xpath)).

fix_version(V) :-
	glade_files(Fs),
	maplist(fix_version(V), Fs).
fix_version(V, F) :-
	load_xml(F, X, []),
	X = [element(interface, I0, I1)],
	(	change_elem_list(I1, element(requires, [A,version=B], []), element(requires, [A,version=V],[]), I2)
	->	Y = [element(interface,I0,I2)], writeln(changed(B,V,F))
	;	Y = X, writeln(unchanged(F))
	),
	file_base_name(F, F1),
	format(atom(Share), '/home/carlo/share/resources/~s', [F1]),
	open(Share, write, S),
	xml_write(S, Y, []),
	close(S).

resources_folder('../../../../collectdcp/resources').

glade_files(Fs) :-
	resources_folder(F),
	atom_concat(F, '/*.glade', P),
	expand_file_name(P, Fs).

change_elem_list(Li,Old,New,Lo) :-
	append(H, [Old|T], Li),
	append(H, [New|T], Lo).
