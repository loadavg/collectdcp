/** <module> parse_conf
 *
 *  Build an editable AST out of a set of markup configuration files.
 *  See  https://collectd.org/documentation/manpages/collectd.conf.5.shtml
 *
 *  This config file controls how the system statistics collection daemon
 *  collectd behaves. The most significant option is LoadPlugin, which
 *  controls which plugins to load. These plugins ultimately define
 *  collectd's behavior. If the AutoLoadPlugin option has been enabled,
 *  the explicit LoadPlugin lines may be omitted for all plugins with a
 *  configuration block, i.e. a <Plugin...> block.
 *  --------
 *
 *  source file /home/carlo/develop/work/karsten/loadavgd/configEditor/proto/pl/parse_conf.pl
 *  created at mer set 16 06:59:56 2015
 *
 * License  : MIT
 * Copyright (c) 2015,2016 Sputnik7
 */

:- module(parse_conf,
	  [parse_conf/0
	  ,parse_conf_cs/2
	  ,unquoted//1
	  ]).
:- use_module(library(dcg/basics)).

%%  parse_conf is det.
%
%   test reading & dumping default main collectd configuration file.
%
parse_conf :-
	parse_conf(AST), maplist(indented(0), AST).

%%	indented(+Off, +T) is det.
%
%	Output a readable report of parsed AST.
%
indented(Off, comment(_,_,C)) :-
	iformat(Off, '~w~n', C).
indented(Off, key_value(_,_,K-V)) :-
	iformat(Off, '~w~n', K-V).
indented(Off, xml_like(_,_,xml(Tag, Attrs, Body))) :-
	iformat(Off, '~s ~w [~n', [Tag, Attrs]),
	indented(Off + 1, Body),
	iformat(Off, ']~n', _).
indented(Off, C) :-
	iformat(Off, '~w~n', [C]).

iformat(Off, Fmt, Args) :-
	indent(Off), format(Fmt, Args).
indent(Off) :-
	N is Off,
	forall(between(0, N, _), write('  ')).

/* $ locate collectd.conf
/etc/collectd/collectd.conf
/etc/collectd/collectd.conf.d
/etc/collectd/collectd.conf.d/filters.conf
/etc/collectd/collectd.conf.d/thresholds.conf
/usr/share/doc/collectd-core/examples/collectd.conf
/usr/share/man/man5/collectd.conf.5.gz
/var/lib/dpkg/info/collectd.conffiles
*/
%config_main('/etc/collectd/collectd.conf.d/filters.conf').
%config_main('/etc/collectd/collectd.conf').
config_main('/home/carlo/develop/work/karsten/loadavgd/configEditor/proto/pl/x.conf').

%%	parse_conf(+AST) is det.
%
% The syntax of this config file is similar to the config file of the
% famous Apache webserver. Each line contains either an option (a key and
% a list of one or more values) or a section-start or -end. Empty lines
% and everything after a non-quoted hash-symbol (#) is ignored. Keys are
% unquoted strings, consisting only of alphanumeric characters and the
% underscore (_) character. Keys are handled case insensitive by collectd
% itself and all plugins included with it. Values can either be an
% unquoted string, a quoted string (enclosed in double-quotes) a number
% or a boolean expression. Unquoted strings consist of only alphanumeric
% characters and underscores (_) and do not need to be quoted. Quoted
% strings are enclosed in double quotes ("). You can use the backslash
% character (\) to include double quotes as part of the string. Numbers
% can be specified in decimal and floating point format (using a dot . as
% decimal separator), hexadecimal when using the 0x prefix and octal with
% a leading zero (0). Boolean values are either true or false.
%
parse_conf(AST) :-
	config_main(CFG),
	read_file_to_codes(CFG, Cs, []),
	parse_conf_cs(Cs, AST).

:- dynamic text_length/1.

parse_conf_cs(Cs, AST)	 :-
	length(Cs, TL),
	retractall(text_length(_)),
	assert(text_length(TL)),
	phrase(cfg(AST), Cs).

%%	cfg(-List)// is det.
%
%	Build a list of nested elements with positions.
%	Parsing is deterministic, committed after each element.
%

cfg([]) --> [].
cfg([Comment|Ls]) -->
	comment(Comment), !, cfg(Ls).
cfg([KeyValue|Ls]) -->
	key_value(KeyValue), !, cfg(Ls).
cfg([XmlTree|Ls]) -->
	xml_like(false, XmlTree), !, cfg(Ls).
cfg([Skip|Ls]) -->
	skip(Skip), !, cfg(Ls).
cfg(Ls) -->
	[_], !, cfg(Ls).

:- meta_predicate xml_like(0, +, +,-).

comment(comment(X, Y, C)) -->
	commline(X), xml_like(true, C), pos(Y), !.
comment(comment(X, Y, C)) -->
	commline(X), key_value(C), pos(Y), !.
comment(comment(X, Y, C)) -->
	commline(X), string(S), eol(Y), !,
	{atom_codes(C, S)}.

commline(X) -->
	pos(X), whites, "#", whites.

key_value(pair(X, Y, K-value(Xv, Yv, V))) -->
	pos(X), whites,
	key(K), whites,
	pos(Xv), value(V), pos(Yv),
	whites, eol(Y).

key(key(Xk, Yk, K)) -->
	pos(Xk), unquoted([C|Cs]), pos(Yk), {atom_codes(K, [C|Cs])}.

skip(skip(X, Y, C)) -->
	pos(X), string(S), eol(Y), {atom_codes(C, S)}.

%%	xml_like(+InComment, -XmlLike)// is det.
%
%	Multiline describe complex configuration options.
%	Parse markup with an XML like syntax.
%
xml_like(InComment, xml(tag(Tag,X,Y,U,V), Attrs, Body)) -->
	whites, "<", tag(TagC,X,Y), whites, string(AttrsC), whites, ">", !,
	body(InComment, TagC, Body, U,V),
	{maplist(atom_codes, [Tag, Attrs], [TagC, AttrsC])}.

body(InComment, Tag, [], U,V) -->
	( {InComment} ->  blanks, "#" ; [] ), blanks, "</", tag(Tag, U,V), whites, ">".
body(InComment, Tag, [], U,V) -->
	% there is a bug in my collectd.conf
	( {InComment} ->  blanks, "#" ; [] ), blanks, "<", tag(Tag, U,V), whites, ">".
body(InComment, Tag, [C|Cs], U,V) -->
	comment(C),
	body(InComment, Tag, Cs, U,V).
body(InComment, Tag, [C|Cs], U,V) -->
	key_value(C),
	body(InComment, Tag, Cs, U,V).
body(InComment, Tag, [C|Cs], U,V) -->
	xml_like(false, C),
	body(InComment, Tag, Cs, U,V).
body(InComment, Tag, Cs, U,V) -->
	blank,
	body(InComment, Tag, Cs, U,V).

value(quoted(V)) --> quoted(V).
value(number(int, V)) --> integer(V).
value(number(float, V)) --> float(V).
value(number(hex, V)) --> xinteger(V).
value(number(oct, V)) --> [0'0], octals(Ds), {digits_octal(Ds, V)}.
value(boolean(true)) --> "true", !.
value(boolean(false)) --> "false", !.
value(unquoted(V)) --> unquoted(V).

octals([D|Ds]) -->
	[C], {code_type(C, digit(D)), D >= 0, D < 8},
	octals(Ds).
octals([]) --> [].

%%	unquoted(-Symbol)// is det.
%
%
unquoted([C|Cs]) -->
	[C], {C = 0'_ ; code_type(C, alnum)}, unquoted(Cs).
unquoted([]) --> [].

%%	quoted(-V)// is det.
%
%	Get all text between quotes.
%	Handle quotes inside and multiline (line continuation)
%
quoted(V) -->
	"\"", quoted_(Cs), {atom_codes(V, Cs)}.

quoted_([]) -->
	"\"".
quoted_([0'\n|Cs]) -->
	"\\\n", quoted_(Cs).
quoted_([0'"|Cs]) -->
	"\\\"", quoted_(Cs).
quoted_([C|Cs]) -->
	[C], quoted_(Cs).

%%	tag(?T, -X, -Y)// is det.
%
%	Start/Stop tokens for XML like entries.
%	Maybe this should restrict somewhat the allowed text.
%
tag(T, X, Y) -->
	pos(X), unquoted(T), pos(Y).

%%	pos(-C, +P, -P) is det.
%
%	capture offset from end of stream
%
%pos(C, P, P) :- length(P, C).
pos(C, P, P) :- text_length(L), length(P, Q), C is L - Q.

eol(P) --> pos(P), eol.
eol --> "\n" ; eos.

%%	digits_octal(+Ds, -Oct) is det
%
%	sequence of numbers in range 0..7 (octal) converted to integer
%
digits_octal(Ds, Oct) :-
	digits_number(Ds, 8, Oct, _).

%%	digits_number(+Digits, +Base, -Number, -Factor) is det.
%
%	convert a sequence of numbers applying a base
%
digits_number([Num], _Base, Num, 0).
digits_number([D|Ds], Base, Num, F1) :-
	digits_number(Ds, Base, Right, F),
	F1 is F + 1,
	Num is Right + D * Base ** F1.

show_dcg(P, _) :-
	length(S, 20),
	append(S, _, P),
	format('[~s]~n', [S]),
	fail.
