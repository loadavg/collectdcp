/** <module> glade_utility
 *
 *  some snippet to handle glade files
 *  --------
 *
 *  source file glade_utility.pl
 *  created at mer dic 16 18:20:08 2015
 *
 *  @author carlo
 *  @version 0.9.9
 *
 *  License  : MIT
 *  Copyright (c) 2015,2016 Sputnik7
 */

:- module(glade_utility,
	[fix_version/1
	,change_elem_list/4
	,view_glade_with_css/0
	]).

:- use_module(library(sgml)).
:- use_module(library(sgml_write)).
% :- use_module(library(xpath)).
:- use_module(ui_structure).

:- use_module(library(plgi)).
:- plgi_use_namespace('Gtk').

fix_version(V) :-
	glade_files(Fs),
	maplist(fix_version(V), Fs).
fix_version(V, F) :-
	load_xml(F, X, []),
	X = [element(interface, I0, I1)],
	(	change_elem_list(I1,
				 element(requires, [A,version=B], []),
				 element(requires, [A,version=V], []), I2)
	->	Y = [element(interface,I0,I2)], writeln(changed(B,V,F))
	;	Y = X, writeln(unchanged(F))
	),
	file_base_name(F, F1),
	format(atom(Share), '/home/carlo/share/resources/~s', [F1]),
	open(Share, write, S),
	xml_write(S, Y, []),
	close(S).

% resources_folder('../../../../collectdcp/resources-3.8').

glade_files(Fs) :-
	resources_folder(F),
	atom_concat(F, '/*.glade', P),
	expand_file_name(P, Fs).

change_elem_list(Li,Old,New,Lo) :-
	append(H, [Old|T], Li),
	append(H, [New|T], Lo).

view_glade_with_css :-
	view_glade_with_css(collectdcp, collectdcp, button).

load_css(CssBaseName) :-
	get_resource_path(CssBaseName, css, Css),
	gtk_css_provider_new(CssProvider),
	gdk_display_get_default(Display),
	gdk_display_get_default_screen(Display, Screen),
	gtk_style_context_add_provider_for_screen(Screen, CssProvider, 600), %'GTK_STYLE_PROVIDER_PRIORITY_APPLICATION'),
	gtk_css_provider_load_from_path(CssProvider, Css, CssLoaded), CssLoaded == true.

view_glade_with_css(GladeBaseName, WidgetName, CssBaseName) :-
	load_css(CssBaseName),

	get_resource_path(GladeBaseName, glade, Glade),
	gtk_builder_new(Builder),
	gtk_builder_add_from_file(Builder, Glade, GLoaded), GLoaded > 0,

	gtk_builder_get_object(Builder, WidgetName, Widget), Widget \= {null},
	g_signal_connect(Widget, destroy, gtk_main_quit/0, {null}, _),

	gtk_widget_show_all(Widget),
	gtk_main.

