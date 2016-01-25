/*  File:    scrap_plugins_wiki.pl
    Author:  Carlo,,,
    Created: Oct  3 2015
    Purpose: read plugins specification from wiki pages, store in local data file
    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(scrap_plugins_wiki,
	[scrap_plugins_wiki/0
	,scrap_plugins_wiki/2
	,plugin_url_name/3
	,open_page/1
	,collectd_url/2
	,get_page/2
	,dl_model/2
	,find_dl/4
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

%% scrap_plugins_wiki(ToP, ManPages) is det.
%
%  get table of plugins and manpages from documentation wiki
%
%  @arg ToP table of plugins
%  @arg ManPages descriptive manpages
%
scrap_plugins_wiki(ToP, ManPages) :-
	url_page_cache_init,

	collectd_url(top, WikiToP),
	get_wiki_plugins(WikiToP, ToP),

	collectd_url(man, ManPage),
	get_manpages(ManPage, ManPages).

%% open_page(Kind) is det.
%
%  show the wiki page in system configured browser
%
%  @arg Kind describe Kind
%
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
	% length(H1s, C), writeln(C).
	% maplist(show_h1_section, H1s),
	findall(manpage{ name:Name, desc:Desc, h1s:H1, h2s:Sub, data_model:Model },
		( member(H1, H1s),
		  get_h1_section(H1, Name, Desc, Sub, Model)
		), ManPages).

get_h1_section(G, Name, Desc, Sub, Model) :-
	xpath_chk(G, //h1(text), Desc),
	xpath_chk(G, //h1/a(@name), Name),
	G=[_,_|R],
	capture_sections(R, h2, Sub), !,
	(   Name == global_options
	->  dl_model(G, Model)
	;   true
	).

field_base(E, field{name:Name, values:Values}) :-
	xpath(E, strong/a, A),
	bagof(T,  strong_em(A, T), [Name|Values]).

strong_em(A, S) :-
	( xpath(A, //strong(text), T) ; xpath(A, //em(text), T) ),
	split_alt(T, S).
split_alt(T, S) :-
	atomic_list_concat(L, '|', T), member(S, L).

find_dl(G, V, Before, After) :-
	%member(element(dl, _, V), G).
	append(Before, [element(dl, _, V)|After], G),
	xpath(V, //dt(normalize_space), DtText),
	DtText \= ''.

%%	dl_model(+G, -DlDt) is nondet
%
%	captures all variable dt+dd sections
%	some dt doesn't have dd, some pair dt+dt have shared dd
%
dl_model(G, DlDt) :-
	find_dl(G, DL, _, _),
	scan_dt(DL, DlDt), !.
dl_model(_, []).

scan_dt([], []).
/*
scan_dt([A,B,C|R], [D1,D2|Ds]) :-
	dt_dd(A,C,D1),
	dt_dd(B,C,D2),
	!, scan_dt(R, Ds).
*/
scan_dt([A,B|R], [D|Ds]) :-
	dt_dd(A,B,D),
	!, scan_dt(R, Ds).
scan_dt([A|R], [D|Ds]) :-
	A = element(dt, _A1, _V1),
	field_base(A, D),
	!, scan_dt(R, Ds).
scan_dt([_|R], Ds) :-
	scan_dt(R, Ds).

dt_dd(A,B,D) :-
	A = element(dt, _, _),
	B = element(dd, _, _),
	field_base(A, FB),
	xpath(B, /self(text), Desc),
	D = FB.put(desc, Desc).

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

manpages(PluginManpages, ManPage, PluginName) :-
	atomic_list_concat([UrlPage, PluginName], #, PluginManpages),
	get_page(UrlPage, ManPage).
plugin_url_name(Plugin, UrlPage, PluginName) :-
	atomic_list_concat([UrlPage, PluginName], #, Plugin.manpages).

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
	xpath(Tr, td(1)/a(@href), HrefD), clean_tail(HrefD, Href),
	xpath(Tr, td(2, text), Mode),
	xpath(Tr, td(3)/a(@href), Manpages),
	xpath(Tr, td(4)/a(text), Version).

clean_tail(Href, Href).

clean_tail(HrefD, What, HRef) :-
	sub_atom(HrefD, _,W,0, What),
	atom_length(HrefD, LHrefD), W1 is LHrefD - W,
	sub_atom(HrefD, 0,W1,_, HRef).

get_page(Url, Html) :-
	url_page(Url, Html), !.
get_page(Url, Html) :-
	http_open(Url, In, []),
	load_html(stream(In), Html, []),
	close(In),
	add_url_page(Url, Html).
