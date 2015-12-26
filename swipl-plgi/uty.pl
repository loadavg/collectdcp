/*  File:    uty.pl
    Author:  Carlo,,,
    Created: Oct 26 2015
    Purpose: simple utilities
    License  : MIT
    Copyright: (c) 2015 Sputnik7
*/
:- module(uty,
	  [replace_word/4
	  ,xml_show/1
	  ,xml_show/3
	  ]).

:- meta_predicate xml_show(+, 1, +).

%%	replace_word(+Old, +New, +Orig, -Replaced) is det.
%
%	in Replaced all occurrences of Old in Orig have changed in New.
%	note it doesn't fail if not Old is not found.
%
replace_word(Old, New, Orig, Replaced) :-
	atomic_list_concat(Split, Old, Orig),
	atomic_list_concat(Split, New, Replaced).

%%	xml_show(+E) is det
%%	xml_show(+Depth, ?ElShow, +E) is det
%
%	display xml tree indented
%
xml_show(E) :- xml_show([], xml_elshow, E).
xml_show(Depth, ElShow, E) :-
	format('~s~@~n', [Depth, call(ElShow, E)]),
	(   E = element(_, _, Xs)
	->  !, maplist(xml_show([0' |Depth], ElShow), Xs)
	;   true
	).

xml_elshow(E) :-
	E = element(D, A, _)
	-> write(D:A)
	;  write(E).
