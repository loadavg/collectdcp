/*  File:    view_ast.pl
    Author:  Carlo,,,
    Created: Oct  6 2015
    Purpose: editable text decorated with AST info
    License  : MIT
    Copyright (c) 2015,2016 Sputnik7
*/

:- module(view_ast,
	  [view_ast/2
	  ,view_save/1
	  ,disable_block/2
	  ,enable_block/2
	  ,file_view_ast/3
	  ,disable_parse/1
	  ,file_view_ast_buf/4
	  ,buf_is_dirty/1
	  ]).

:- use_module(model).
:- use_module(parse_conf).
:- use_module(ui_structure).
:- use_module(message_box).

:- use_module(library(plgi)).
:- plgi_use_namespace('GtkSource').

:- dynamic file_view_ast/3.
:- dynamic disable_parse/1.

%%	view_ast(+File, -View)
%
%	create a view of .conf parsed file
%
view_ast(File, View) :-
	retractall(file_view_ast(File, _, _)),
	get_conf_file(File, Buffer, Ast),
	gtk_source_view_new_with_buffer(Buffer, View),
	g_object_set_property(View, 'show-line-numbers', true),
	g_signal_connect(Buffer, 'notify::cursor-position',
			 view_ast:on_cursor_position_changed/3, {null}, _C),
	g_signal_connect(Buffer, 'changed',
			 view_ast:on_modified_changed/3, {null}, _D),
	g_signal_connect(View, 'focus',
			 view_ast:on_focus/4, {null}, _V),
	assert(file_view_ast(File, View, Ast)).

%%	view_save(+File)
%
%	dump file content to configured path
%	if content needs saving
%
view_save(File) :-
	file_view_ast_buf(File, _, _, Buf),
	(   buf_is_dirty(Buf) ->
	    get_buffer_text(Buf, Text),
	    conf_file(File, Path),
	    (   setup_call_cleanup(open(Path, write, S), write(S, Text), close(S))
	    ->  status_message('saved ~w in ~w'-[File, Path])
	    ;   message_box('cannot save ~w in ~w'-[File, Path])
	    )
	;   message_box('nothing to save from ~w'-[File])
	).

%%	on_cursor_position_changed(+Buf, +_ParamInt, +_Data)
%
%	enable/disable context sensitive commands, depending on cursor
%	position
%
on_cursor_position_changed(Buf, _ParamInt, _Data) :-
	disable_parse(Buf) -> true ;
	file_view_ast_buf(File, _View, Ast, Buf),
	setup_context_commands(File, Buf, Ast).

setup_context_commands(File, Buf, Ast) :-
	buffer_cursor_offset(Buf, Cursor),
	(   ast_locate_cursor(Ast, Cursor, El)
	->  (   El = comment(_,_,_)
	    ->	(UNC,COM) = (enabled,disabled)
	    ;	( El = xml(_,_,_) ; El = pair(_,_,_) )
	    ->	(UNC,COM) = (disabled,enabled)
	    ;	(UNC,COM) = (disabled,disabled)
	    ),
	    status_message('on ~w: uncomment=~w, comment=~w'-[File,UNC,COM])
	;   (UNC,COM) = (disabled,disabled),
	    status_message('cannot locate CONF element at ~w on ~w'-[Cursor, File])
	),
	action_status(enable_block, UNC),
	action_status(disable_block, COM).

%%	on_modified_changed(+Buffer, +_ParamInt, +_Data)
%
%	on user submitted change, reparse buffer to get aligned
%	attributes
%
on_modified_changed(Buffer, _ParamInt, _Data) :-
	disable_parse(Buffer) -> true ;
	set_dirty(Buffer),
	set_save_status(Buffer).

%%	on_focus(View, _B, _C, F)
%
%	on user panel choice, setup context commands
%
on_focus(View, _B, _C, F) :-
	file_view_ast_buf(File, View, Ast, Buf),
	setup_context_commands(File, Buf, Ast),
	F = false,
	set_save_status(Buf).

set_save_status(Buf) :-
	buf_is_dirty(Buf)
	->  action_status(file_save, true)
	;   action_status(file_save, false).

view_notebook(ViewW, Notebook, ChildW, LabelW) :-
	parents(ViewW, Parents),
	append(_, [ChildW, Notebook|_], Parents),
	g_object_type(Notebook, 'GtkNotebook'),
	gtk_notebook_get_tab_label(Notebook, ChildW, LabelW).

set_dirty(Buf) :-
	file_view_ast_buf(_File, View, _Ast, Buf),
	view_notebook(View, _, _, LabelW),
	gtk_label_get_text(LabelW, Text),
	(   sub_atom(Text, 0,_,_, '* ')
	->  true
	;   atom_concat('* ', Text, Dirty),
	    gtk_label_set_text(LabelW, Dirty)
	).

%%	buf_is_dirty(+Buf)
%
%	true if Buf content has been modified
%
buf_is_dirty(Buf) :-
	file_view_ast_buf(_File, View, _Ast, Buf),
	view_notebook(View, _, _, LabelW),
	gtk_label_get_text(LabelW, Text),
	!, sub_atom(Text, 0,_,_, '* ').

file_view_ast_buf(File, View, Ast, Buf) :-
	file_view_ast(File, View, Ast),
	gtk_text_view_get_buffer(View, Buf).

disable_parse_now(Buffer) :-
	retractall(disable_parse(_)),
	assertz(disable_parse(Buffer)).
enable_parse_now :-
	retractall(disable_parse(_)).

scrollable_view_ast(File, Scrollable) :-
	view_ast(File, View),
	gtk_scrolled_window_new({null}, {null}, Scrollable),
	gtk_container_add(Scrollable, View).

%%	get_conf_file(+Name, -Buffer, -AST)
%
%	parse the file and apply AST attributes
%
get_conf_file(Name, Buffer, AST) :-
	conf_file(Name, File),
	read_file_to_codes(File, Cs, []),
	parse_conf_cs(Cs, AST),
	atom_codes(Text, Cs),

	prepare_attributes(GtkTextTagTable),

	gtk_source_buffer_new(GtkTextTagTable, Buffer),
	g_object_get_property(Buffer, 'undo-manager', GtkSourceUndoManager),
	gtk_source_undo_manager_begin_not_undoable_action(GtkSourceUndoManager),
	gtk_text_buffer_set_text(Buffer, Text, -1),
	apply_attrs(Buffer, AST),
	gtk_source_undo_manager_end_not_undoable_action(GtkSourceUndoManager).

apply_attrs(Buffer, AST) :-
	set_attribute(Buffer, monospace),
	maplist(apply_AST_attributes(Buffer), AST).

%%	apply_AST_attributes(+Buf, +Val)
%
%	decorate a 'key value' pair
%	decorate an XML like tag
%
apply_AST_attributes(Buf, pair(_X,_Y, key(Xk,Yk,_) - value(Xv,Yv,V))) :-
	set_attribute(Buf, key, Xk,Yk),
	V =.. [F|_],
	set_attribute(Buf, F, Xv,Yv).
apply_AST_attributes(Buf, xml(tag(_,X,Y,U,V), _, Xml)) :-
	set_attribute(Buf, xml_open, X,Y),
	set_attribute(Buf, xml_close, U,V),
	maplist(apply_AST_attributes(Buf), Xml).

apply_AST_attributes(Buf, comment(X,Y, C)) :-
	set_attribute(Buf, comment, X,Y),
	apply_AST_attributes(Buf,  C).
apply_AST_attributes(Buf, comment(X,Y,_Comment)) :-
	set_attribute(Buf, comment, X,Y).
apply_AST_attributes(_, _).

%%	set_attribute(+Buf, +Attr, +X,+Y)
%
%	set specified attribute on range X,Y
%
set_attribute(Buf, Attr, X,Y) :-
	gtk_text_buffer_get_iter_at_offset(Buf, I, X),
	gtk_text_buffer_get_iter_at_offset(Buf, J, Y),
	set_attribute_(Buf, Attr, I,J).

%%	set_attribute(+Buf, +Attr)
%
%	set specified attribute to entire buffer
%
set_attribute(Buf, Attr) :-
	get_start_end(Buf, I, J),
	set_attribute_(Buf, Attr, I,J).

%%	set_attribute_(+Buf, +Attr, +I,+J)
%
%	retrieve attribute by table and apply between iterators
%
set_attribute_(Buf, Attr, I,J) :-
	g_object_get_property(Buf, 'tag-table', TT),
	gtk_text_tag_table_lookup(TT, Attr, AttrTT),
	gtk_text_buffer_apply_tag(Buf, AttrTT, I, J).

%%	prepare_attributes(+TTT)
%
%	create attributes with silly default values
%	and store them in attributes table
%	TBD cache for reuse on multiple buffers
%
prepare_attributes(TTT) :-
	gtk_text_tag_table_new(TTT),
	maplist(create_attr,
		[xml_open -[style  - 'PANGO_STYLE_ITALIC'],
		 xml_close-[style  - 'PANGO_STYLE_ITALIC', strikethrough - true],
		 key      -[weight - 'PANGO_WEIGHT_BOLD'],
		 number   -[foreground - darkgreen],
		 unquoted -[foreground - blue4 /*'BlueViolet'*/ /*chocolate*/ /*cyan*/],
		 quoted   -[foreground - blue],
		 boolean  -[foreground - brown],
		 comment  -[background - lightgrey],
		 monospace-[family - 'Monospace']
		], Attrs),
	maplist(gtk_text_tag_table_add(TTT), Attrs).

%%	create_attr(+NamedAttr, +TextTag)
%
%	create a tag with Name and associate all properties listed
%
create_attr(Name-Attrs, TT) :-
	gtk_text_tag_new(Name, TT),
	forall(member(Prop-Value, Attrs),
	       g_object_set_property(TT, Prop, Value)).
/*
comment_lines(Buf, X,Y) :-
	get_start_lines(Buf, X,Y, Lines),
	reverse(Lines, RevLines),
	maplist(comment_line(Buf), RevLines).
comment_line(Buf, Line) :-
	gtk_text_buffer_get_iter_at_offset(Buf, Iter, Line),
	gtk_text_buffer_insert(Buf, Iter, #, -1).

uncomment_lines(Buf, X,Y) :-
	get_start_lines(Buf, X,Y, Lines),
	reverse(Lines, RevLines),
	maplist(uncomment_line(Buf), RevLines).
uncomment_line(Buf, Line) :-
	gtk_text_buffer_get_iter_at_offset(Buf, X, Line),
	Line1 is Line+1,
	gtk_text_buffer_get_iter_at_offset(Buf, Y, Line1),
	gtk_text_buffer_delete(Buf, X, Y).
*/

:- meta_predicate apply_to_lines(1,+,+,+).

apply_to_lines(Pred, Buf, X,Y) :-
	get_start_lines(Buf, X,Y, Lines),
	reverse(Lines, RevLines),
	maplist(Pred, RevLines).

comment_lines(Buf, X,Y) :-
	apply_to_lines(comment_line(Buf), Buf, X,Y).
uncomment_lines(Buf, X,Y) :-
	apply_to_lines(uncomment_line(Buf), Buf, X,Y).

comment_line(Buf, Line) :-
	gtk_text_buffer_get_iter_at_offset(Buf, Iter, Line),
	gtk_text_buffer_insert(Buf, Iter, #, -1).
uncomment_line(Buf, Line) :-
	gtk_text_buffer_get_iter_at_offset(Buf, X, Line),
	Line1 is Line+1,
	gtk_text_buffer_get_iter_at_offset(Buf, Y, Line1),
	gtk_text_buffer_delete(Buf, X, Y).

iter_offset(Iter, Off) :-
	gtk_text_iter_get_offset(Iter, Off).

get_starts(Xi, Yi, [L|Lines]) :-
	gtk_text_iter_compare(Xi,Yi,C),
	C < 0,
	iter_offset(Xi, L),
	gtk_text_iter_forward_line(Xi, _),
	!, get_starts(Xi, Yi, Lines).
get_starts(_, _, []).

get_start_lines(Buf, X,Y, Lines) :-
	gtk_text_buffer_get_iter_at_offset(Buf, Xi, X),
	gtk_text_iter_set_line_offset(Xi, 0),
	gtk_text_buffer_get_iter_at_offset(Buf, Yi, Y),
	get_starts(Xi, Yi, Lines).

get_start_end(Buf, Start, End) :-
	gtk_text_buffer_get_start_iter(Buf, Start),
	gtk_text_buffer_get_end_iter(Buf, End).
get_buffer_text(Buf, Text) :-
	get_start_end(Buf, Start, End),
	gtk_text_buffer_get_text(Buf, Start, End, true, Text).

reparse_buffer(File, Buf) :-
	get_buffer_text(Buf, Text),
	atom_codes(Text, Cs),
	parse_conf_cs(Cs, Ast),
	retract(file_view_ast(File, View, _)),
	assert(file_view_ast(File, View, Ast)),
	get_start_end(Buf, Start, End),
	gtk_text_buffer_remove_all_tags(Buf, Start, End),
	apply_attrs(Buf, Ast).

:- meta_predicate process_block(3, +,+).

process_block(AstFunc, File, Cursor) :-
	file_view_ast_buf(File, _View, Ast, Buf),
	ast_locate_cursor(Ast, Cursor, El),
	ast_range_cursor(El, X,Y),
	disable_parse_now(Buf),
	call(AstFunc, Buf, X,Y),
	reparse_buffer(File, Buf),
	enable_parse_now,
	set_dirty(Buf),
	set_save_status(Buf).

disable_block(File, Cursor) :-
	process_block(comment_lines, File, Cursor),
	action_status(enable_block, enabled),
	action_status(disable_block, disabled).
enable_block(File, Cursor) :-
	process_block(uncomment_lines, File, Cursor),
	action_status(enable_block, disabled),
	action_status(disable_block, enabled).
