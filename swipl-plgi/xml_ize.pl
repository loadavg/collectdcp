/*  File:    xml_ize.pl
    Author:  Carlo,,,
    Created: Jan  9 2016

    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(xml_ize,
	  [xml_ize/2
	  ,xml_save_file/2
	  ,properties/2
	  ,property/2
	  ,add_missing_key_attr/3
	  ]).

xml_ize(S, XML) :-
	    is_list(S)
	->  maplist(xml_ize, S, XML)
	;   S =.. [T, As, Es]
	->  attr_normalize_key(As, Zs),
	    XML = element(T, Zs, Xs),
	    maplist(xml_ize, Es, Xs)
	;   XML = S
	.

xml_save_file(Path, XML) :-
	open(Path, write, S),
	xml_write(S, XML, []),
	close(S).

attr_normalize_key(As, Bs) :-
	maplist([A, B]>>(
		A=(K1=V), B=(K2=V), with_output_to(atom(K2), write(K1))), As, Bs).

add_missing_key_attr(As, K=V, At) :-
	memberchk(K=_, As) -> At = As ; At = [K=V|As].

properties(Ps, Ts) :-
	maplist(property, Ps, Ts), !.
property(P, property([name=Name|Rest], [ValueT])) :-
	P =.. [Name, Value|Rest],
	( memberchk(Value-ValueT, [true-'True', false-'False']) ; ValueT = Value ).
