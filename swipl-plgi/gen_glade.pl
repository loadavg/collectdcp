/*  File:    gen_glade.pl
    Author:  Carlo,,,
    Created: Dec 30 2015
    Purpose: generation of Glade editable user interface from collectd wiki documentation
    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(gen_glade,
	  [gen_glade/0
	  ,gen_main/0
	  ,show_collectdcp/0
	  ]).

:- use_module(library(sgml)).
:- use_module(library(xpath)).
:- use_module(scrap_plugins_wiki).
:- use_module(library(dcg/basics)).

gen_glade :-
	glade_file_full_path(glade_resource_folder, *, Path),
	expand_file_name(Path, L),
	maplist(show_glade, L).

gen_main :-
	scrap_plugins_wiki(_ToP, Man),
	member(X, Man), _{name:global_options} :< X,
	findall(FR, (nth0(R, X.data_model, F), gen_row(F, R, FR)), FRs),

	gen_glade_target_path(main, Path),
	open(Path, write, S),
	flatten(FRs, FLs),
	gen_grid_interface(_, FLs, GI),
	xml_write(S, GI, []),
	close(S).

show_collectdcp :-
	glade_file_full_path(glade_resource_folder, dlg_commands, F), % collectdcp
	load_xml(F, X, []),
	maplist(writeln, X).

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

gen_grid_interface(Id, Rows, E) :-
	( Id = grid1 ; true ),
	E = element(interface, [],
		    [element(requires,
			    [lib='gtk+', version='3.8'],
			    [])
		    ,element(object,
			     [class='GtkGrid', id=Id],
			     [element(property, [name=visible], ['True'])
			     ,element(property, [name=can_focus], ['False'])
			     | Rows])
		    ]).

gen_row(Field, Row, E) :-
	_{name:NameValue} :< Field,
	atom_codes(NameValue, Cs),
	phrase((parse_conf:unquoted(NameCs),whites,string(PlaceholderCs)), Cs),
	atom_codes(Name, NameCs),
	atom_codes(Placeholder, PlaceholderCs),
	( _{desc:Desc} :< Field ; format(atom(Desc), 'no description available for ~w', [Name]) ),
	!, E = [element(child,
		    [],
		    [element(object,
			     [class="GtkLabel", id="label1"],
			     [element(property, [name=visible], ['True'])
			     ,element(property, [name=can_focus], ['False'])
			     ,element(property, [name=label, translatable=yes], [Name])
			     ])
		    ,element(packing,
			     [],
			     [element(property, [name=left_attach], [0])
			     ,element(property, [name=top_attach], [Row])
			     ,element(property, [name=width], [1])
			     ,element(property, [name=height], [1])
			     ])
		    ])
	    ,element(child,
		    [],
		    [element(object,
			     [class="GtkEntry", id=Name],
			     [element(property, [name=visible], ['True'])
			     ,element(property, [name=can_focus], ['True'])
			     ,element(property, [name=tooltip_markup, translatable=yes], [Desc])
			     ,element(property, [name=placeholder_text, translatable=yes], [Placeholder])
			     ])
		    ,element(packing,
			     [],
			     [element(property, [name=left_attach], [1])
			     ,element(property, [name=top_attach], [Row])
			     ,element(property, [name=width], [1])
			     ,element(property, [name=height], [1])
			     ])
		    ])
	    ].

