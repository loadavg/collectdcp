/*  File:    gen_glade.pl
    Author:  Carlo,,,
    Created: Dec 30 2015
    Purpose: generate glade files from collectd wiki definitions
    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(gen_glade,
	[gen_collectdcp/0
	,gen_main/0
	,show_glade/0
	,show_collectdcp/0
	,show_object_instance/2
	]).

:- use_module(library(sgml)).
:- use_module(library(xpath)).
:- use_module(library(dcg/basics)).

:- use_module(scrap_plugins_wiki).
:- use_module(parse_conf).
:- use_module(xml_ize).

%% gen_collectdcp is det.
%
%  generate main a plugins detailed configurations
%
gen_collectdcp :-
	scrap_plugins_wiki(_ToP, Man),
	gen_main(Man),
	gen_plugins(Man).

%% gen_main is det.
%
%  generate glade file for main collectd options
%
gen_main :-
	scrap_plugins_wiki(_ToP, Man),
	gen_main(Man).

%% show_glade is det.
%
%  test glade file display.
%
show_glade :-
	glade_file_full_path(glade_resource_folder, *, Path),
	expand_file_name(Path, L),
	maplist(show_glade, L).

%% show_collectdcp is det.
%
%  display some info about content of all glade files in folder
%
show_collectdcp :-
	glade_file_full_path(glade_resource_folder, collectdcp, F), % dlg_commands
	load_xml(F, X, []),
	maplist(writeln, X).

show_object_instance(Class, File) :-
	glade_file_full_path(glade_resource_folder, File, F), % dlg_commands
	load_xml(F, X, []),
	xpath(X, //object(@class=Class), Object),
	writeln(Object).

gen_plugins(Man) :-
	member(X, Man), _{name:plugin_options} :< X,
	forall(member(P, X.h2s), gen_plugin(P)).

gen_plugin(P) :-
	xpath(P, //a/code(text), Name),

	(   % append(BeforeDl, [element(dl, _,_)|_], P)
	    find_dl(P, _, BeforeDl, _)
	->  maplist(get_desc, BeforeDl, LDesc),
	    atomic_list_concat(LDesc, '\n', PDesc)
	;   PDesc = ''
	),

	dl_model(P, Dl),
	findall(FR, (nth0(R, Dl, F), gen_row(F, R, FR)), FRs),
	flatten(FRs, FLs),
	gen_grid_interface(_, Name, FLs, PDesc, XML),
	gen_glade_target_path(Name, Path),
	xml_save_file(Path, XML), !.

get_desc(X, Y) :-
	xpath(X, /self(text), Y) -> true ; Y = X.

gen_main(Man) :-
	member(X, Man),	_{name:global_options} :< X,
	findall(FR, (nth0(R, X.data_model, F), gen_row(F, R, FR)), FRs),
	flatten(FRs, FLs),
	gen_grid_interface(_, grid_main, FLs, XML),
	gen_glade_target_path(main, Path),
	xml_save_file(Path, XML).

:- meta_predicate glade_file_full_path(1, +, -).

glade_file_full_path(Folder, File, Path) :-
	call(Folder, RF),
	format(atom(Path), '~s/~s.glade', [RF, File]).

glade_target_version('3.8').

glade_resource_folder(F) :-
	glade_target_version(V),
	atom_concat('/home/carlo/develop/work/karsten/collectdcp/resources-', V, F).
gen_glade_target_folder(F) :-
	glade_resource_folder(RF),
	format(atom(F), '~s/generated', [RF]).

gen_glade_target_path(File, Path) :-
	glade_file_full_path(gen_glade_target_folder, File, Path).

show_glade(F) :-
	load_xml(F, X, []),
	time(classes_counters(X, P)),
	writeln(F:P).

classes_counters(X, P) :-
	setof(Class, xpath(X, //object(@class), Class), Classes),
	maplist({X}/[C,C-N]>>aggregate(count, xpath(X, //object(@class), C), N),
		Classes, P).

gen_grid_interface(Ver, Id, Rows, E) :- gen_grid_interface(Ver, Id, Rows, _, E).

gen_grid_interface(Ver, Id, Rows, PDesc, E) :-
	( Ver = '3.8' ; true ),
	( Id = grid1 ; true ),
	Obj = object([class='GtkGrid', id=Id],
		     [property([name=visible], ['True'])
		     ,property([name=can_focus], ['False'])
		     |Rows]),
	(  atom(PDesc)
	-> html_label(Id, PDesc, RowG),
	   Interface = [RowG, Obj]
	;  Interface = [Obj]
        ),
	xml_ize(interface([],
			  [requires([lib='gtk+', version=Ver], [])|Interface]
			 ), E).

packing(Left_attach, Top_attach, Width, Height, P) :-
	P = packing([],
	     [property([name=left_attach], [Left_attach])
	     ,property([name=top_attach], [Top_attach])
	     ,property([name=width], [Width])
	     ,property([name=height], [Height])
	     ]).
packing(Left_attach, Top_attach, P) :- packing(Left_attach, Top_attach, 1,1, P).

gen_row(Field, Row, E) :-
	_{name:Name, values:Values} :< Field,
	( _{desc:Desc} :< Field ; format(atom(Desc), 'no description available for ~w', [Name]) ),

	packing(0, Row, P1),
	properties([visible(true)
		   ,can_focus(false)
		   ,label(Name, translatable=yes)], Ps1),
	name_labelId(Name, Label),
	Lab = [object([class="GtkLabel", id=Label], Ps1), P1],

	packing(1, Row, P2),
	/*
	(   Values = [Placeholder]
	->  Class = 'GtkEntry',
	    properties([visible(true)
		       ,can_focus(true)
		       ,tooltip_text(Desc, translatable=yes) %,tooltip_markup(Desc, translatable=yes)
		       ,placeholder_text(Placeholder, translatable=yes)
		       ], Ps2)
	;   Values = []
	->  Class = 'GtkEntry',
	    properties([visible(true)
		       ,can_focus(true)
		       ,tooltip_text(Desc, translatable=yes) %,tooltip_markup(Desc, translatable=yes)
		       ,placeholder_text(?, translatable=yes)
		       ], Ps2)
	;   booleans(Values)
	->  Class = 'GtkCheckButton',
	    properties([visible(true)
		       ,can_focus(true)
		       ,tooltip_text(Desc, translatable=yes)
		       ], Ps2)
	;   items(Values, Items)
	->  Class = 'GtkComboBoxText',
	    properties([visible(true)
		       ,can_focus(false)
		       ,tooltip_text(Desc, translatable=yes)
		       ], Ps),
	    Ps2 = [Items|Ps]
	;   throw(cannot_generate_field(Name, on, Values))
	),
	*/
	(   entry(Values, Placeholder)
	->  Class = 'GtkEntry',
	    properties([visible(true)
		       ,can_focus(true)
		       ,tooltip_text(Desc, translatable=yes) %,tooltip_markup(Desc, translatable=yes)
		       ,placeholder_text(Placeholder, translatable=yes)
		       ], Ps2)
	;   booleans(Values)
	->  Class = 'GtkCheckButton',
	    properties([visible(true)
		       ,can_focus(true)
		       ,tooltip_text(Desc, translatable=yes)
		       ], Ps2)
	;   items(Values, Items)
	->  Class = 'GtkComboBoxText',
	    properties([visible(true)
		       ,can_focus(false)
		       ,tooltip_text(Desc, translatable=yes)
		       ], Ps),
	    Ps2 = [Items|Ps]
	;   throw(cannot_generate_field(Name, on, Values))
	),

	Obj = [object([class=Class, id=Name], [property([name=name],[Name])|Ps2]), P2],
	!, xml_ize([child([], Lab), child([], Obj)], E).

entry(Values, Placeholder) :-
	(   Values = [Placeholder]
	->  true
	;   Values = [Value, Optionals]
	->  sub_atom(Optionals, 0,1,_, '['),
	    sub_atom(Optionals, _,1,0, ']'),
	    format(atom(Placeholder), '~w ~w', [Value, Optionals])
	;   Values = []
	->  Placeholder = ?
	).

booleans(Vs) :- sort(Vs, [false,true]).

items(Values, items([], Items)) :-
	length(Values, NValues),
	NValues >= 2,
	maplist(value_item, Values, Items).
value_item(V, item([translatable=no],[V])).

name_labelId(Name, Label) :-
	atomic_list_concat([label,-,Name], Label).

html_label(ID, HTML, X) :-
	properties([visible(true), can_focus(false), label(HTML) /*, use_markup(true)*/], Ps),
	atom_concat(label_, ID, LabelID),
	xml_ize(object([class="GtkLabel", id=LabelID], Ps), X).
