/** <module> url_page_cache
 *
 *  cache doc info pages
 *  --------
 *
 *  source file url_page_cache.pl
 *  created at sab ott 3 17:33:06 2015
 *
 *  @author carlo
 *  @version 0.9.9
 *
 *  License  : MIT
 *  Copyright: (c) 2015 Sputnik7
 */

:- module(url_page_cache,
	[url_page_cache_init/0
	,url_page_cache_init/1
	,add_url_page/2
	,url_page/2
	]).
:- use_module(library(persistency)).

:- persistent url_page(url:atom, page:list).

url_page_cache_init :- url_page_cache_init('url_page_cache_db.pl').
url_page_cache_init(File) :-
	db_attach(File, []).

add_url_page(Url, Page) :-
	url_page(Url, Page), !.
add_url_page(Url, Page) :-
	assert_url_page(Url, Page).
