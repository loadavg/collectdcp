/*  File:    message_box.pl
    Author:  Carlo,,,
    Created: Oct 16 2015
    Purpose: display a modal message box
    License  : MIT
    Copyright: (c) 2015 Sputnik7
*/

:- module(message_box,
	  [message_box/1
	  ,message_box/3
	  ]).

:- use_module(library(plgi)).
:- plgi_use_namespace('Gtk').

message_box(Message) :-
	message_box(Message, ok, _).

message_box(Message-Args, Btns, Response) :-
	format(atom(Text), Message, Args),
	!, message_box(Text, Btns, Response).
message_box(Message, Btns, Resp) :-
	(   memberchk(Btns-Buttons,
		  [none	     - 'GTK_BUTTONS_NONE'
		  ,ok        - 'GTK_BUTTONS_OK'
		  ,close     - 'GTK_BUTTONS_CLOSE'
		  ,cancel    - 'GTK_BUTTONS_CANCEL'
		  ,yes_no    - 'GTK_BUTTONS_YES_NO'
		  ,ok_cancel - 'GTK_BUTTONS_OK_CANCEL'
		  ])
	->  true
	;   throw('unhandled button type'(Btns))
	),
	g_object_new('GtkMessageDialog',
		     ['message-type' = 'GTK_MESSAGE_INFO',
		      'buttons'      = Buttons
		     ], Dialog),
	gtk_message_dialog_set_markup(Dialog, Message),
	gtk_dialog_run(Dialog, Response),
	member(V-Resp,
		['GTK_RESPONSE_NONE'         - ok
		,'GTK_RESPONSE_REJECT'       - ok
		,'GTK_RESPONSE_ACCEPT'       - ok
		,'GTK_RESPONSE_DELETE_EVENT' - cancel
		,'GTK_RESPONSE_OK'           - ok
		,'GTK_RESPONSE_CANCEL'       - cancel
		,'GTK_RESPONSE_CLOSE'        - cancel
		,'GTK_RESPONSE_YES'	     - ok
		,'GTK_RESPONSE_NO'           - cancel
		,'GTK_RESPONSE_APPLY'	     - ok
		,'GTK_RESPONSE_HELP'         - cancel
		]),
	plgi_enum_value(V, Response),
	gtk_widget_destroy(Dialog).
