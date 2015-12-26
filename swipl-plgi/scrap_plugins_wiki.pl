/*  File:    scrap_plugins_wiki.pl
    Author:  Carlo,,,
    Created: Oct  3 2015
    Purpose: read plugins specification from wiki pages, store in local data file
    License  : MIT
    Copyright: (c) 2015 Sputnik7
*/

:- module(scrap_plugins_wiki,
	[scrap_plugins_wiki/0
	,scrap_plugins_wiki/2
	,plugin_url_name/3
	,open_page/1
	,collectd_url/2
	,get_page/2
	]).

:- use_module(library(http/http_open)).
:- use_module(library(xpath)).
:- use_module(library(sgml)).
:- use_module(library(www_browser)).

:- use_module(url_page_cache).
:- use_module(uty).

%%	scrap_plugins_wiki is semidet
%
%	cache a local DB of plugin related documentation
%
scrap_plugins_wiki :-
	scrap_plugins_wiki(_, _).

scrap_plugins_wiki(ToP, ManPages) :-
	url_page_cache_init,

	collectd_url(top, WikiToP),
	get_wiki_plugins(WikiToP, ToP),

	collectd_url(man, ManPage),
	get_manpages(ManPage, ManPages).

open_page(Kind) :-
	collectd_url(Kind, Url),
	www_open_url(Url).

get_wiki_plugins(ToP, Wiki) :-
	get_page(ToP, Html),
	xpath(Html, //div(@id=bodyContent)/div/table, Table),
	findall(P, get_plugin(Table, P), Plugins),
	maplist(get_plugin_info, Plugins, Wiki).

get_manpages(ManPage, ManPages) :-
	get_page(ManPage, Html),
	xpath(Html, //td(@id=l23), element(td, _, L)),
	capture_sections(L, h1, H1s), !,
	% this crash XPCE... length(H1s, L), writeln(L).
	%length(H1s, C), writeln(C).
	% maplist(show_h1_section, H1s),
	findall(manpage{ name:Name, desc:Desc, h2s:Sub },
		( member(G, H1s), capt(G, Name, Desc, Sub) ), ManPages).

capt(G, Name, 'Desc', Sub) :-
	xpath_chk(G, //h1/a(@name), Name),
	G=[_,_|R],
	capture_sections(R, h2, Sub), !.

/*
show_h1_section([A,B|S]) :-
	writeln((A,B)),
	capture_sections(S, h2, H2s),
	maplist(show_h2_section, H2s).
show_h2_section([A,B|_]) :-
	put(0' ), writeln((A,B)).
*/
show_h1_section([A,_|S]) :-
	writeln(A),
	capture_sections(S, h2, H2s),
	maplist(show_h2_section, H2s).
show_h2_section([A,_|_]) :-
	put(0' ), writeln(A).

capture_sections([], _Tag, []).
capture_sections([E|Es], Tag, Sections) :-
	E = element(Tag,_,_),
	(   capture_section(Es, Tag, S, R)
	->  capture_sections(R, Tag, Rs),
	    Sections = [[E|S]|Rs]
	;   Sections = []
	).
capture_sections([_|Es], Tag, Sections) :-
	capture_sections(Es, Tag, Sections).

capture_section([E|Es], Tag, [], [E|Es]) :-
	E = element(Tag,_,_).
capture_section([E|Es], Tag, [E|Ss], R) :-
	capture_section(Es, Tag, Ss, R).
capture_section([], _Tag, [], []).

/*
collect_find_dl([B|R], B, R) :-
	B = element(dl,_,_).
collect_find_dl([B|_], _, _) :-
	B = element(h2,_,_), !, fail.
collect_find_dl([_|R], B, S) :-
	collect_find_dl(R, B, S).


xpath_pos(DOM, Expr, Node, Pos) :-
	xpath(DOM, Expr, Node),
	dom_pos(DOM, Node, Pos).

dom_pos(Node, Node, []).
dom_pos(element(_, _, L), Node, [P|Ps]) :-
	nth1(P, L, E),
	dom_pos(E, Node, Ps).
*/
manpages(PluginManpages, ManPage, PluginName) :-
	atomic_list_concat([UrlPage, PluginName], #, PluginManpages),
	get_page(UrlPage, ManPage).
plugin_url_name(Plugin, UrlPage, PluginName) :-
	atomic_list_concat([UrlPage, PluginName], #, Plugin.manpages).

/*save_plugins_wiki :-
	forall(page(U, Html), add_url_page(U, Html)).
*/
collectd_url(top, Url) :- !,
	collectd_url('/wiki/index.php/Table_of_Plugins', Url).
collectd_url(man, Url) :- !,
	collectd_url('/documentation/manpages/collectd.conf.5.shtml', Url).
collectd_url(Relative, Url) :-
	atom_concat('https://collectd.org', Relative, Url).

get_plugin_info(Plugin, Plugin.put(page, yes)) :-
	plugin_has_page(Plugin),
	collectd_url(Plugin.href, PluginUrl),
	get_page(PluginUrl, _Page),
	!.
get_plugin_info(Plugin, Plugin.put(page, no)).

plugin_has_page(Plugin) :-
	replace_word(' ', '_', Plugin.title, Title),
	sub_atom(Plugin.href, _,_,0, Title).

has_wiki_page(Plugin) :-
	Plugin >:< plugin{page:yes}.

get_plugin(TBody,
	   plugin{
	       title:Title,
	       href:Href,
	       mode:Mode,
	       manpages:Manpages,
	       version:Version
	   }) :-
	xpath(TBody, //tr, Tr),
	xpath(Tr, td(1)/a(@title), Title),
	xpath(Tr, td(1)/a(@href), Href),
	xpath(Tr, td(2, text), Mode),
	xpath(Tr, td(3)/a(@href), Manpages),
	xpath(Tr, td(4)/a(text), Version).

get_page(Url, Html) :-
	url_page(Url, Html), !.
get_page(Url, Html) :-
	http_open(Url, In, []),
	load_html(stream(In), Html, []),
	close(In),
	add_url_page(Url, Html).
