/*  File:    ui_structure.pl
    Author:  Carlo,,,
    Created: Oct 12 2015
    Purpose: interface GUI
    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(ui_structure,
	  [conf_editable/1
	  ,cleanup/0
	  ,get_application_interface/1
	  ,get_resource/2
	  ,container_structure/2
	  ,get_notebook/2
	  ,get_statusbar/2
	  ,parents/2
	  ,named_parents/2
	  ,status_message/1
	  ,status_message/2
	  ,action_status/2
	  ,buffer_cursor_offset/2
	  ,current_cursor_info/3
	  ,current_file_view_buf/4
	  ]).

:- use_module(library(plgi)).
:- plgi_use_namespace('Gtk').

:- use_module(view_ast).

:- dynamic ui_elements/2.

%%	cleanup is det
%
%	remove all stored objects
%
cleanup :-
	aggregate_all(count, file_view_ast(_, _, _), Views),
	aggregate_all(count, ui_elements(_, _), Elems),
	writeln(cleanup(Views, Elems)),
	retractall(file_view_ast(_, _, _)),
	retractall(disable_parse(_)),
	retractall(ui_elements(_, _)).

%%	conf_editable(+ConfTab)
%
%	elements enabled to conf specific editing
%
conf_editable(collectd).
conf_editable(filters).
conf_editable(thresholds).

:- meta_predicate apply_to_action(1,+).

%%	get_application_interface(-TopLevelWindow)
%
%	build the main toplevel interface, must be named as default:
%	'window1'. Associate default .conf files to editors.
%
get_application_interface(Window) :-
	retractall(ui_elements(_, _)),

	get_resource(editor, Builder),
	gtk_builder_get_object(Builder, window1, Window),
	gtk_builder_connect_signals(Builder, Window),
	forall(conf_editable(Conf),
	       (   gtk_builder_get_object(Builder, Conf, Scrollable),
		   time(view_ast(Conf, View)),
		   %view_ast(Conf, View),
		   gtk_container_add(Scrollable, View)
		   % gtk_widget_get_path(View, Path), writeln(Path)
	       )),
	assert(ui_elements(top_level, Window)),
	Actions =
		[file_save
		,edit_cut
		,edit_copy
		,edit_paste
		,edit_delete
		,edit_undo
		,edit_redo
		,add_block
		,enable_block
		,disable_block
		],
	maplist(apply_to_action(store_action(Builder)), Actions),
	maplist(apply_to_action(status_action(disabled)), Actions).

apply_to_action(Pred, Sym) :-
	atom_concat(mn_, Sym, MnSym), call(Pred, MnSym),
	atom_concat(tb_, Sym, TbSym), call(Pred, TbSym).

store_action(Builder, Name) :-
	(   gtk_builder_get_object(Builder, Name, Elem)
	->  assertz(ui_elements(Name, Elem))
	;   writeln(no_action(Name))
	).

%%	get_resource(+What, -Builder)
%
%	patch the glade base file name with folder and extension
%	get the GtkBuilder ready for object retrieval
%
get_resource(What, Builder) :-
	context_module(M),
	module_property(M, file(File)),
	file_directory_name(File, Directory),
	format(atom(GladeFile), '~s/../resources-3.10/~s.glade', [Directory, What]),
	gtk_builder_new(Builder),
	gtk_builder_add_from_file(Builder, GladeFile, _).

%%	container_structure(+Widget, -Structure)
%
%	recover the container given structure
%
container_structure(Widget, Structure) :-
	% unable to check type hierarchy: g_type seems to be a string...
	(   catch(gtk_container_get_children(Widget, Children), _, fail)
	->  maplist(container_structure, Children, Sub),
	    Structure = Widget-Sub
	;   Structure = Widget
	).

/*
get_structure({null}, _Deep, _Structure).
get_structure(Widget, Deep, Structure) :-
	g_object_type(Widget, Type),
	g_type_from_name(Type, GType),
	g_type_parent(GType, TParent),

	object_name(Widget, Name),
	forall(between(0, Deep, _), put(0' )), writeln(Name:Widget),
	parents(Widget, Parents),
	maplist(g_object_type, Parents, Types),
	(   memberchk('GtkContainer', Types)
	->  gtk_container_get_children(Widget, Children),
	    Deep1 is Deep+1,
	    forall(member(C, Children), get_structure(C, Deep1, Structure))
	).
*/

%%	get_notebook(+TopLevel, -Notebook)
%%	get_statusbar(+TopLevel, -StatusBar)
%
%	these calls resolve the *hardcoded* application structure
%	keep aligned with glade design
%
get_notebook(TopLevel, Notebook) :-
	get_main_iitem(TopLevel, 2, Notebook).
get_statusbar(TopLevel, StatusBar) :-
	get_main_iitem(TopLevel, 3, StatusBar).

get_main_iitem(TopLevel, Index, Item) :-
	gtk_container_get_children(TopLevel, [VBox]),
	gtk_container_get_children(VBox, Items), % [Menu,Toolbar,Notebook,StatusBar]
	nth0(Index, Items, Item).

%%	parents(+Widget, -Parents:list)
%
%	go back in parents relationship
%
parents({null}, []).
parents(G, [G|Ps]) :-
	gtk_widget_get_parent(G, P),
	parents(P, Ps).

%%	named_parents(+Widget, -NamedParents:list)
%
%	get an assoc list of widgets to names
%
named_parents(W, NPs) :-
	parents(W, Ps),
	maplist(object_name, Ps, TNPs),
	pairs_keys_values(NPs, Ps, TNPs).

%%	object_name(+Widget, -Name)
%
%	get name property
%	pretty useless right now
%
object_name(W, N) :-
	g_object_get_property(W, name, N).

%%	status_message(+SpecArgs)
%%	status_message(+TopLevel, +SpecArgs)
%
%	display a formatted message
%
status_message(Spec-Args) :-
	ui_elements(top_level, TopLevel),
	status_message(TopLevel, Spec-Args).
status_message(TopLevel, Spec-Args) :-
	get_statusbar(TopLevel, Statusbar),
	gtk_statusbar_remove_all(Statusbar, 0),
	format(atom(Message), Spec, Args),
	gtk_statusbar_push(Statusbar, 0, Message, _).

%%	action_status(+Action, ?Status)
%
%	change status to an action - a pair of menu/toolbar commands
%
action_status(Action, Status) :-
	apply_to_action(status_action(Status), Action).

status_action(Status, Action) :-
	ui_elements(Action, Object),
	(   ( Status == enabled ; Status == true )
	->  gtk_widget_set_sensitive(Object, true)
	;   ( Status == disabled ; Status == false )
	->  gtk_widget_set_sensitive(Object, false)
	).
status_action(Status, Action) :-
	throw(failed(status_action(Status, Action))).

%%	buffer_cursor_offset(+Buffer, -Offset)
%
%	get cursor offset for buffer
%
buffer_cursor_offset(Buffer, Offset) :-
	gtk_text_buffer_get_insert(Buffer, TextMark),
	gtk_text_buffer_get_iter_at_mark(Buffer, Iter, TextMark),
	gtk_text_iter_get_offset(Iter, Offset).

%%	current_cursor_info(+TopLevel, +File, -Offset)
%
%	retrieve current notebook page cursor offset
%
current_cursor_info(TopLevel, File, Offset) :-
	current_file_view_buf(TopLevel, File, _View, Buf),
	buffer_cursor_offset(Buf, Offset).
/*
	get_notebook(TopLevel, Notebook),
	gtk_notebook_get_current_page(Notebook, CurrentPage),
	gtk_notebook_get_nth_page(Notebook, CurrentPage, Scrollable),
	gtk_container_get_children(Scrollable, [SourceView]),
	file_view_ast(File, SourceView, _AST),
	gtk_text_view_get_buffer(SourceView, SourceBuffer),
	buffer_cursor_offset(SourceBuffer, Offset).
*/

%%	current_file_view_buf(+TopLevel, -File, -View, -Buf)
%
%	retrieve current notebook page cursor offset

current_file_view_buf(TopLevel, File, View, Buf) :-
	get_notebook(TopLevel, Notebook),
	gtk_notebook_get_current_page(Notebook, CurrentPage),
	gtk_notebook_get_nth_page(Notebook, CurrentPage, Scrollable),
	gtk_container_get_children(Scrollable, [View]),
	file_view_ast_buf(File, View, _AST, Buf).
