/*  File:    editapp.pl
    Author:  Carlo,,,
    Created: Oct  9 2015
    Purpose: Main application for collectd configuration files editapp project.
             Bind commands to UI designed with glade.
    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(editapp,
	  [editapp/0
	  ,editapp_0/0
	  % event handlers must be declared public to get resolved
	  % by gtk_builder_connect_signals
	  ,on_file_save/2
	  ,on_commands/2
	  ,on_about_box/2
	  ,on_delete_event/4
	  ,on_destroy/4
	  ,on_file_quit/2
	  ,on_edit_copy/2
	  ,on_edit_cut/2
	  ,on_edit_paste/2
	  ,on_edit_delete/2
	  ,on_edit_undo/2
	  ,on_edit_redo/2
	  ,on_disable_block/2
	  ,on_enable_block/2
	  ,on_add_block/2
	  /* cannot enable actions from glade, sorry...
	  ,on_disable_block_action/2
	  ,on_enable_block_action/2
	  */
	  ]).

:- use_module(library(plgi)).
:- plgi_use_namespace('Gtk').

:- use_module(view_ast).
:- use_module(ui_structure).
:- use_module(message_box).
:- use_module(dlg_commands).
:- use_module(add_plugin_block).

%%	editapp
%
%	run in development environment
%
editapp :-
	get_application_interface(Window),
	gtk_widget_show_all(Window),
	maplist(enabled, [add_block,
		    edit_copy,edit_cut,edit_paste,edit_delete,
		    edit_undo,edit_redo]),
	%thread_create(gtk_main, _, [alias(gtk_main), detached(true)]).
	gtk_main,
	cleanup, !.

enabled(A) :- action_status(A, enabled).

%%	editapp_0
%
%	run as application (close prolog at exit)
%
editapp_0 :-
	get_application_interface(Window),
	gtk_widget_show_all(Window),
	gtk_main,
	halt.

%%	on_file_quit(+Arg1, +TopLevelWindow)
%
%	Quit the application, when no editing suspended.
%	Assume gtk_builder_connect_signals(Builder, Window).
%
%	There are problems to properly issue gtk loop stopping.
%
on_file_quit(_A, TopLevelWindow) :-
	(   check_quit
	->  gtk_widget_hide(TopLevelWindow),
	    gtk_main_quit
	;   true
	).

%%	on_destroy(+Widget, +Event, +UserData, +Flag)
%
%	quit the application
%	this handler must be attached to toplevel window
%
on_destroy(_Widget, _Event, _UserData, _Enable) :-
	   gtk_main_quit.

%%	on_delete_event(+Widget, +Event, +UserData, -Flag)
%
%	check all files for dirty
%	this handler must be attached to toplevel window
%
on_delete_event(_Widget, _Event, _UserData, Flag) :-
	check_quit ->  Flag = false ; Flag = true.

%%	check_quit is semidet
%
%	if any file dirty, ask to save
%
check_quit :-
	findall(File, ( file_view_ast_buf(File, _View, _Ast, Buf),
			buf_is_dirty(Buf)), Dirties),
	(   Dirties == []
	->  true
	;   message_box('Files ~w\nmodified and not saved\nQuit anyway?'
			-[Dirties], ok_cancel, Ans),
	    ( Ans == ok -> true ; fail )
	).

%%	on_about_box(+Handler, +UserData)
%
%	attached to help/about menu
%	get the about box from about_box.glade
%	the dialog must be named as default: aboutdialog1
%
on_about_box(_A, _B) :-
	get_resource(about_box, Builder),
	gtk_builder_get_object(Builder, aboutdialog1, Dialog),
	gtk_dialog_run(Dialog, _Response),
	gtk_widget_destroy(Dialog).

/*
on_disable_block_action(A, B) :-
	writeln(on_disable_block_action(A, B)).
on_enable_block_action(A, B) :-
	writeln(on_enable_block_action(A, B)).
*/

route_to_view(TopLevel, Signal, Args) :-
	current_file_view_buf(TopLevel, _File, View, _Buf),
	plgi:plgi_signal_emit(View, Signal, '', Args).

on_edit_copy(_A, TopLevel) :-
	route_to_view(TopLevel, 'copy-clipboard', []).
on_edit_cut(_A, TopLevel) :-
	route_to_view(TopLevel, 'cut-clipboard', []).
on_edit_paste(_A, TopLevel) :-
	route_to_view(TopLevel, 'paste-clipboard', []).
on_edit_delete(_A, TopLevel) :-
	route_to_view(TopLevel, 'delete-from-cursor', ['GTK_DELETE_CHARS', 1]).

on_edit_undo(_A, TopLevel) :-
	route_to_view(TopLevel, 'undo', []).
on_edit_redo(_A, TopLevel) :-
	route_to_view(TopLevel, 'redo', []).

on_file_save(_A, _B) :-
	forall(conf_editable(Conf),
	       view_save(Conf)).

on_commands(_A, _B) :-
	dlg_commands.

on_add_block(_M, TopLevel) :-
	current_cursor_info(TopLevel, File, Offset),
	(   File == collectd
	->  add_plugin_block(File, Offset)
	;   status_message('on_add_block ~w'-[TopLevel])
	).

on_disable_block(_M, TopLevel) :-
	current_cursor_info(TopLevel, File, Offset),
	disable_block(File, Offset),
	status_message('setting offset ~w on conf ~w'-[Offset, File]).
on_disable_block(_M, _TopLevel) :-
	status_message('cannot disable_block now'-[]).

on_enable_block(_A, TopLevel) :-
	current_cursor_info(TopLevel, File, Offset),
	enable_block(File, Offset),
	status_message('setting offset ~w on conf ~w'-[Offset, File]).
on_enable_block(_A, _TopLevel) :-
	status_message('cannot enable_block now'-[]).
