/*  File:    wnd_commands.pl
    Author:  Carlo,,,
    Created: Oct 20 2015
    Purpose: issue and display shell commands
    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(dlg_commands,
	  [dlg_commands/0
	  %,on_validate/2
	  %,on_stats/2
	  %,on_install/2
	  ]).

:- use_module(ui_structure).
:- use_module(model).

:- use_module(library(plgi)).
% :- plgi_use_namespace('pango').

%%	dlg_commands is det
%
%	run a modal dialog with commonly used commands
%	defined as buttons, output is captured and shown in text box
%
dlg_commands :-
	get_resource(dlg_commands, Builder),
	gtk_builder_get_object(Builder, dlg_commands, Dialog),
	gtk_builder_get_object(Builder, tb_output, Tb_output),

	gtk_text_view_get_buffer(Tb_output, Buf),
	gtk_text_buffer_get_tag_table(Buf, TTT),
	gtk_text_tag_new(monospace, TT),
	g_object_set_property(TT, family, 'Monospace'),
	gtk_text_tag_table_add(TTT, TT),

	maplist(connect_btn(Builder, Tb_output), [validate,stats,install]),

	%gtk_builder_connect_signals(Builder, Dialog),
	gtk_dialog_run(Dialog, _Response),
	gtk_widget_destroy(Dialog).

set_monospace(Buf) :-
	gtk_text_buffer_get_bounds(Buf, Start, End),
	gtk_text_buffer_apply_tag_by_name(Buf, monospace, Start, End).

on_validate(_Btn, Tb_output) :-
	conf_file(collectd, _Path),
	process_create(path(collectd),
		       [/*'-C', Path, '-t',*/ '-h'],
		       [stdout(pipe(Out))]),
	read_stream_to_codes(Out, Cs),
	atom_codes(A, Cs),
	%gtk_widget_get_pango_context(Tb_output, PC),
	%pango_context_get_font_description(monospace, FD),
	%Tb_output.modify_font(FD),
	gtk_text_view_get_buffer(Tb_output, Buf),
	gtk_text_buffer_set_text(Buf, A, -1),
	set_monospace(Buf).

on_stats(_A,_B) :-
	writeln(on_stats).

on_install(_A,_B) :-
	writeln(on_install).

connect_btn(Builder, Tb_output, Cmd) :-
	atom_concat(btn_, Cmd, Btn),
	atom_concat(on_, Cmd, Pred),
	gtk_builder_get_object(Builder, Btn, Btn_validate),
	g_signal_connect(Btn_validate, clicked, dlg_commands:Pred/2, Tb_output, _).
