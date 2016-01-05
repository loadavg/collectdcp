/*  File:    add_plugin_block.pl
    Author:  Carlo,,,
    Created: Oct 26 2015
    Purpose: browse plugins structure definition and insert text in free
    location
    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(add_plugin_block,
	  [add_plugin_block/2
	  ,create_conf_file/0
	  ,create_conf_file/1
	  ]).

:- use_module(view_ast).
:- use_module(ui_structure).
:- use_module(scrap_plugins_wiki).
:- use_module(library(xpath)).

add_plugin_block(File, _Pos) :-
	get_resource(add_plugin_treeview, Builder),
	gtk_builder_get_object(Builder, add_plugin_block, Dialog),
	%gtk_builder_get_object(Builder, add_plugin_treeview, _Treeview),
	gtk_builder_get_object(Builder, add_plugin_treestore, Treestore),
	%gtk_builder_connect_signals(Builder, Dialog),
	file_view_ast(File, _, Ast),
	%scrap_plugins_wiki(_, ManPages),
	%manpages_to_ast(ManPages, AstP),

	include(plugin_block, Ast, Plugins),
	maplist(plugins_to_store(Treestore), Plugins),
	gtk_dialog_run(Dialog, _Response),
	gtk_widget_destroy(Dialog).

	%gtk_tree_model_get_n_columns(Treestore, N_columns),
/*
	gtk_text_view_get_buffer(Tb_output, Buf),
	gtk_text_buffer_get_tag_table(Buf, TTT),
	gtk_text_tag_new(monospace, TT),
	g_object_set_property(TT, family, 'Monospace'),
	gtk_text_tag_table_add(TTT, TT),

	maplist(connect_btn(Builder, Tb_output), [validate,stats,install]),
*/

create_conf_file :-
	create_conf_file(plugins).

create_conf_file(Kind) :-
	scrap_plugins_wiki(Sections, ManPages), !,
	format(atom(File), '~s.template', [Kind]),
	setup_call_cleanup(
	    open(File, write, S),
	    with_output_to(S, dump_conf_template(Sections, ManPages)),
	    close(S)).

ds_member(Dp, Ds, D) :-
	member(D, Ds), Dp :< D.

dump_conf_template(Sections, ManPages) :-
	ds_member(_{name:plugin_options}, ManPages, Pl),
	maplist(format_plugin_options(Pl.h2s), Sections).
format_plugin_options(_H2s, Wikip) :-
	Wikip.page == yes,
	collectd_url(Wikip.href, PluginUrl),
	get_page(PluginUrl, Page),
	xpath(Page, //pre(text), Text),
	writeln(Text), !.
format_plugin_options(_H2s, _Wikip).

/*
	ds_member(_{page:yes}, Sections, SectPlo),
	maplist(format_plugin_options, Plo.h2s).

dump_conf_template(Sections, ManPages) :-
	forall(member(S, Sections), dump_conf_template_plugin(S, ManPages)).
dump_conf_template_plugin(Section, ManPages) :-
	(   Section.page == yes,
	    % plugin_url_name(Section, _Url, Name),
	    ds_member(_{name:plugin_options}, ManPages, Pl)
	->  maplist(format_plugin_options, Pl.h2s)
	;   true % format('failed ~w~n', [dump_conf_template_plugin(Plugin)])
	).
*/

%%	plugin_block(+Elem) is semidet
%
%	filter out only plugins blocks
%
plugin_block(comment(_, _,
	  xml(tag('Plugin',_,_,_,_),_,_))).
plugin_block(xml(tag('Plugin',_,_,_,_),_,_)).

plugins_to_store(Store, El) :-
	El = comment(_, _, Xml),
	Xml = xml(tag('Plugin',_,_,_,_),_,_),
	plugin_to_store(Store, {null}, Xml).
plugins_to_store(Store, Xml) :-
	Xml = xml(tag('Plugin',_,_,_,_),_,_),
	plugin_to_store(Store, {null}, Xml).

plugin_to_store(Store, Parent, xml(_, Name, Args)) :-
	gtk_tree_store_append(Store, NodeIter, Parent),
	value_to_store(Name, Store, NodeIter, 0),
	value_to_store(true, Store, NodeIter, 1),
	maplist(plugin_to_store(Store, NodeIter), Args).
plugin_to_store(Store, Parent, El) :-
	(   El = comment(_, _, pair(_, _, key(_, _, Key)-value(_, _, quoted(Quoted))))
	->  gtk_tree_store_append(Store, NodeIter, Parent),
	    value_to_store(Key, Store, NodeIter, 0),
	    value_to_store(Quoted, Store, NodeIter, 3)
	;   true
	).

value_to_store(Value, Store, NodeIter, Index) :-
	(   ( Value == true ; Value == false)
	->  g_value_init(gboolean, V),
	    g_value_set_boolean(V, Value)
	;   atom(Value)
	->  g_value_init(gchararray, V),
	    g_value_set_string(V, Value)
	;   integer(Value)
	->  g_value_init(gint, V),
	    g_value_set_int(V, Value)
	),
	gtk_tree_store_set_value(Store, NodeIter, Index, V).

