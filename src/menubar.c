/* Implements an elisp-programmable menubar.
   Copyright (C) 1993, 1994 Free Software Foundation, Inc.
   Copyright (C) 1995 Tinker Systems and INS Engineering Corp.

This file is part of SXEmacs.

SXEmacs is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

SXEmacs is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with SXEmacs; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* Synched up with: Not in FSF. */

/* Authorship:

   Created by Ben Wing as part of device-abstraction work for 19.12.
   Menu filters and many other keywords added by Stig for 19.12.
   Menu accelerators c. 1997? by ??.  Moved here from event-stream.c.
   Much other work post-1996 by ??.
*/

#include <config.h>
#include "lisp.h"

#include "buffer.h"
#include "device.h"
#include "frame.h"
#include "gui.h"
#include "keymap.h"
#include "menubar.h"
#include "redisplay.h"
#include "window.h"

int menubar_show_keybindings;
Lisp_Object Vmenubar_configuration;

Lisp_Object Qcurrent_menubar;

Lisp_Object Qactivate_menubar_hook, Vactivate_menubar_hook;

Lisp_Object Vmenubar_visible_p;

static Lisp_Object Vcurrent_menubar;	/* DO NOT ever reference this.
					   Always go through Qcurrent_menubar.
					   See below. */

Lisp_Object Vblank_menubar;

int popup_menu_titles;

Lisp_Object Vmenubar_pointer_glyph;

/* prefix key(s) that must match in order to activate menu.
   This is ugly.  fix me.
   */
Lisp_Object Vmenu_accelerator_prefix;

/* list of modifier keys to match accelerator for top level menus */
Lisp_Object Vmenu_accelerator_modifiers;

/* whether menu accelerators are enabled */
Lisp_Object Vmenu_accelerator_enabled;

/* keymap for auxiliary menu accelerator functions */
Lisp_Object Vmenu_accelerator_map;

Lisp_Object Qmenu_force;
Lisp_Object Qmenu_fallback;
Lisp_Object Qmenu_quit;
Lisp_Object Qmenu_up;
Lisp_Object Qmenu_down;
Lisp_Object Qmenu_left;
Lisp_Object Qmenu_right;
Lisp_Object Qmenu_select;
Lisp_Object Qmenu_escape;

static int
menubar_variable_changed(Lisp_Object sym, Lisp_Object * val,
			 Lisp_Object in_object, int flags)
{
	MARK_MENUBAR_CHANGED;
	return 0;
}

void update_frame_menubars(struct frame *f)
{
	if (f->menubar_changed || f->windows_changed)
		MAYBE_FRAMEMETH(f, update_frame_menubars, (f));

	f->menubar_changed = 0;
}

void free_frame_menubars(struct frame *f)
{
	/* If we had directly allocated any memory for the menubars instead
	   of using all Lisp_Objects this is where we would now free it. */

	MAYBE_FRAMEMETH(f, free_frame_menubars, (f));
}

static void
menubar_visible_p_changed(Lisp_Object specifier, struct window *w,
			  Lisp_Object oldval)
{
	MARK_MENUBAR_CHANGED;
}

static void
menubar_visible_p_changed_in_frame(Lisp_Object specifier, struct frame *f,
				   Lisp_Object oldval)
{
	update_frame_menubars(f);
}

Lisp_Object current_frame_menubar(const struct frame *f)
{
	struct window *w = XWINDOW(FRAME_LAST_NONMINIBUF_WINDOW(f));
	return symbol_value_in_buffer(Qcurrent_menubar, w->buffer);
}

Lisp_Object menu_parse_submenu_keywords(Lisp_Object desc, Lisp_Object gui_item)
{
	Lisp_Gui_Item *pgui_item = XGUI_ITEM(gui_item);

	/* Menu descriptor should be a list */
	CHECK_CONS(desc);

	/* First element may be menu name, although can be omitted.
	   Let's think that if stuff begins with anything than a keyword
	   or a list (submenu), this is a menu name, expected to be a string */
	if (!KEYWORDP(XCAR(desc)) && !CONSP(XCAR(desc))) {
		CHECK_STRING(XCAR(desc));
		pgui_item->name = XCAR(desc);
		desc = XCDR(desc);
		if (!NILP(desc))
			CHECK_CONS(desc);
	}

	/* Walk along all key-value pairs */
	while (!NILP(desc) && KEYWORDP(XCAR(desc))) {
		Lisp_Object key, val;
		key = XCAR(desc);
		desc = XCDR(desc);
		CHECK_CONS(desc);
		val = XCAR(desc);
		desc = XCDR(desc);
		if (!NILP(desc))
			CHECK_CONS(desc);
		gui_item_add_keyval_pair(gui_item, key, val, ERROR_ME);
	}

	/* Return the rest - supposed to be a list of items */
	return desc;
}

DEFUN("menu-find-real-submenu", Fmenu_find_real_submenu, 2, 2, 0,	/*
Find a submenu descriptor within DESC by following PATH.
This function finds a submenu descriptor, either from the description
DESC or generated by a filter within DESC. The function regards :config
and :included keywords in the DESC, and expands submenus along the
PATH using :filter functions. Return value is a descriptor for the
submenu, NOT expanded and NOT checked against :config and :included.
Also, individual menu items are not looked for, only submenus.

See also 'find-menu-item'.
									 */
      (desc, path))
{
	Lisp_Object path_entry, submenu_desc, submenu;
	struct gcpro gcpro1, gcpro2;
	Lisp_Object gui_item = allocate_gui_item();
	Lisp_Gui_Item *pgui_item = XGUI_ITEM(gui_item);

	GCPRO2(gui_item, desc);

	EXTERNAL_LIST_LOOP(path_entry, path) {
		/* Verify that DESC describes a menu, not single item */
		if (!CONSP(desc))
			RETURN_UNGCPRO(Qnil);

		/* Parse this menu */
		desc = menu_parse_submenu_keywords(desc, gui_item);

		/* Check that this (sub)menu is active */
		if (!gui_item_active_p(gui_item))
			RETURN_UNGCPRO(Qnil);

		/* Apply :filter */
		if (!NILP(pgui_item->filter))
			desc = call1(pgui_item->filter, desc);

		/* Find the next menu on the path inside this one */
		EXTERNAL_LIST_LOOP(submenu_desc, desc) {
			submenu = XCAR(submenu_desc);
			if (CONSP(submenu)
			    && STRINGP(XCAR(submenu))
			    &&
			    !NILP(Fstring_equal
				  (XCAR(submenu), XCAR(path_entry)))) {
				desc = submenu;
				goto descend;
			}
		}
		/* Submenu not found */
		RETURN_UNGCPRO(Qnil);

	      descend:
		/* Prepare for the next iteration */
		gui_item_init(gui_item);
	}

	/* We have successfully descended down the end of the path */
	UNGCPRO;
	return desc;
}

DEFUN("popup-menu", Fpopup_menu, 1, 2, 0,	/*
Pop up the menu described by MENU-DESCRIPTION.
A menu description is a list of menu items, strings, and submenus.

The first element of a menu must be a string, which is the name of the menu.
This is the string that will be displayed in the parent menu, if any.  For
toplevel menus, it is ignored.  This string is not displayed in the menu
itself.

If an element of a menu is a string, then that string will be presented in
the menu as unselectable text.

If an element of a menu is a string consisting solely of hyphens, then that
item will be presented as a solid horizontal line.

If an element of a menu is a list, it is treated as a submenu.  The name of
that submenu (the first element in the list) will be used as the name of the
item representing this menu on the parent.

Otherwise, the element must be a vector, which describes a menu item.
A menu item can have any of the following forms:

[ "name" callback <active-p> ]
[ "name" callback <active-p> <suffix> ]
[ "name" callback :<keyword> <value>  :<keyword> <value> ... ]

The name is the string to display on the menu; it is filtered through the
resource database, so it is possible for resources to override what string
is actually displayed.

If the `callback' of a menu item is a symbol, then it must name a command.
It will be invoked with `call-interactively'.  If it is a list, then it is
evaluated with `eval'.

The possible keywords are this:

:active   <form>    Same as <active-p> in the first two forms: the
expression is evaluated just before the menu is
displayed, and the menu will be selectable only if
the result is non-nil.

:suffix   <form>    Same as <suffix> in the second form: the expression
is evaluated just before the menu is displayed and
resulting string is appended to the displayed name,
providing a convenient way of adding the name of a
command's ``argument'' to the menu, like
``Kill Buffer NAME''.

:keys     "string"  Normally, the keyboard equivalents of commands in
menus are displayed when the `callback' is a symbol.
This can be used to specify keys for more complex menu
items.  It is passed through `substitute-command-keys'
first.

:style    <style>   Specifies what kind of object this menu item is:

nil     A normal menu item.
toggle  A toggle button.
radio   A radio button.

The only difference between toggle and radio buttons is
how they are displayed.  But for consistency, a toggle
button should be used when there is one option whose
value can be turned on or off, and radio buttons should
be used when there is a set of mutually exclusive
options.  When using a group of radio buttons, you
should arrange for no more than one to be marked as
selected at a time.

:selected <form>    Meaningful only when STYLE is `toggle' or `radio'.
This specifies whether the button will be in the
selected or unselected state.

For example:

[ "Save As..."    write-file  t ]
[ "Revert Buffer" revert-buffer (buffer-modified-p) ]
[ "Read Only"     toggle-read-only :style toggle :selected buffer-read-only ]

See menubar.el for many more examples.
						 */
      (menu_description, event)) {
	struct frame *f = decode_frame(Qnil);
	MAYBE_FRAMEMETH(f, popup_menu, (menu_description, event));
	return Qnil;
}

DEFUN("normalize-menu-item-name", Fnormalize_menu_item_name, 1, 2, 0,	/*
Convert a menu item name string into normal form, and return the new string.
Menu item names should be converted to normal form before being compared.
This removes %_'s (accelerator indications) and converts %% to %.
									 */
      (name, buffer))
{
	struct buffer *buf = decode_buffer(buffer, 0);
	Lisp_String *n;
	Charcount end;
	int i;
	Bufbyte *name_data;
	Bufbyte *string_result;
	Bufbyte *string_result_ptr;
	Emchar elt;
	int expecting_underscore = 0;

	CHECK_STRING(name);

	n = XSTRING(name);
	end = string_char_length(n);
	name_data = string_data(n);

	string_result = (Bufbyte *) alloca(end * MAX_EMCHAR_LEN);
	string_result_ptr = string_result;
	for (i = 0; i < end; i++) {
		elt = charptr_emchar(name_data);
		elt = DOWNCASE(buf, elt);
		if (expecting_underscore) {
			expecting_underscore = 0;
			switch (elt) {
			case '%':
				/* Allow `%%' to mean `%'.  */
				string_result_ptr +=
				    set_charptr_emchar(string_result_ptr, '%');
				break;
			case '_':
				break;
			default:
				string_result_ptr +=
				    set_charptr_emchar(string_result_ptr, '%');
				string_result_ptr +=
				    set_charptr_emchar(string_result_ptr, elt);
			}
		} else if (elt == '%')
			expecting_underscore = 1;
		else
			string_result_ptr +=
			    set_charptr_emchar(string_result_ptr, elt);
		INC_CHARPTR(name_data);
	}

	if (string_result_ptr - string_result == XSTRING_LENGTH(name)
	    && !memcmp(string_result, XSTRING_DATA(name), XSTRING_LENGTH(name)))
		return name;

	return make_string(string_result, string_result_ptr - string_result);
}

void syms_of_menubar(void)
{
	defsymbol(&Qcurrent_menubar, "current-menubar");

	defsymbol(&Qmenu_force, "menu-force");
	defsymbol(&Qmenu_fallback, "menu-fallback");

	defsymbol(&Qmenu_quit, "menu-quit");
	defsymbol(&Qmenu_up, "menu-up");
	defsymbol(&Qmenu_down, "menu-down");
	defsymbol(&Qmenu_left, "menu-left");
	defsymbol(&Qmenu_right, "menu-right");
	defsymbol(&Qmenu_select, "menu-select");
	defsymbol(&Qmenu_escape, "menu-escape");

	DEFSUBR(Fpopup_menu);
	DEFSUBR(Fnormalize_menu_item_name);
	DEFSUBR(Fmenu_find_real_submenu);
}

void vars_of_menubar(void)
{
	/* put in Vblank_menubar a menubar value which has no visible
	 * items.  This is a bit tricky due to various quirks.  We
	 * could use '(["" nil nil]), but this is apparently equivalent
	 * to '(nil), and a new frame created with this menubar will
	 * get a vertically-squished menubar.  If we use " " as the
	 * button title instead of "", we get an etched button border.
	 * So we use
	 *  '(("No active menubar" ["" nil nil]))
	 * which creates a menu whose title is "No active menubar",
	 * and this works fine.
	 */

	Vblank_menubar = list1(list2(build_string("No active menubar"),
				     vector3(build_string(""), Qnil, Qnil)));
	staticpro(&Vblank_menubar);

	DEFVAR_BOOL("popup-menu-titles", &popup_menu_titles	/*
If true, popup menus will have title bars at the top.
								 */ );
	popup_menu_titles = 1;

	/* #### Replace current menubar with a specifier. */

	/* All C code must access the menubar via Qcurrent_menubar
	   because it can be buffer-local.  Note that Vcurrent_menubar
	   doesn't need to exist at all, except for the magic function. */

	DEFVAR_LISP_MAGIC("current-menubar", &Vcurrent_menubar	/*
The current menubar.  This may be buffer-local.

When the menubar is changed, the function `set-menubar-dirty-flag' has to
be called for the menubar to be updated on the frame.  See `set-menubar'
and `set-buffer-menubar'.

A menubar is a list of menus and menu-items.
A menu is a list of menu items, keyword-value pairs, strings, and submenus.

The first element of a menu must be a string, which is the name of the menu.
This is the string that will be displayed in the parent menu, if any.  For
toplevel menus, it is ignored.  This string is not displayed in the menu
itself.

Menu accelerators can be indicated in the string by putting the
sequence "%_" before the character corresponding to the key that will
invoke the menu or menu item.  Uppercase and lowercase accelerators
are equivalent.  The sequence "%%" is also special, and is translated
into a single %.

If no menu accelerator is present in the string, SXEmacs will act as if
the first character has been tagged as an accelerator.

Immediately following the name string of the menu, various optional
keyword-value pairs are permitted: currently, :filter, :active, :included,
and :config. (See below.)

If an element of a menu (or menubar) is a string, then that string will be
presented as unselectable text.

If an element of a menu is a string consisting solely of hyphens, then that
item will be presented as a solid horizontal line.

If an element of a menu is a string beginning with "--:", it will be
presented as a line whose appearance is controlled by the rest of the
text in the string.  The allowed line specs are system-dependent, and
currently work only under X Windows (with Lucid and Motif menubars);
otherwise, a solid horizontal line is presented, as if the string were
all hyphens.

The possibilities are:

"--:singleLine"
"--:doubleLine"
"--:singleDashedLine"
"--:doubleDashedLine"
"--:noLine"
"--:shadowEtchedIn"
"--:shadowEtchedOut"
"--:shadowEtchedInDash"
"--:shadowEtchedOutDash"
"--:shadowDoubleEtchedIn" (Lucid menubars only)
"--:shadowDoubleEtchedOut" (Lucid menubars only)
"--:shadowDoubleEtchedInDash" (Lucid menubars only)
"--:shadowDoubleEtchedOutDash" (Lucid menubars only)

If an element of a menu is a list, it is treated as a submenu.  The name of
that submenu (the first element in the list) will be used as the name of the
item representing this menu on the parent.

If an element of a menubar is `nil', then it is used to represent the
division between the set of menubar-items which are flushleft and those
which are flushright.

Otherwise, the element must be a vector, which describes a menu item.
A menu item is of the following form:

[ "name" callback :<keyword> <value> :<keyword> <value> ... ]

The following forms are also accepted for compatibility, but deprecated:

[ "name" callback <active-p> ]
[ "name" callback <active-p> <suffix> ]

The name is the string to display on the menu; it is filtered through the
resource database, so it is possible for resources to override what string
is actually displayed.  Menu accelerator indicators (the sequence `%_') are
also processed; see above.  If the name is not a string, it will be
evaluated with `eval', and the result should be a string.

If the `callback' of a menu item is a symbol, then it must name a command.
It will be invoked with `call-interactively'.  If it is a list, then it is
evaluated with `eval'.

In the deprecated forms, <active-p> is equivalent to using the :active
keyword, and <suffix> is equivalent to using the :suffix keyword.

The possible keywords are:

:active   <form>    The expression is evaluated just before the menu is
displayed, and the menu will be selectable only if
the result is non-nil.

:suffix   <form>    The expression is evaluated just before the menu is
displayed and the resulting string is appended to
the displayed name, providing a convenient way of
adding the name of a command's ``argument'' to the
menu, like ``Kill Buffer NAME''.

:keys     "string"  Normally, the keyboard equivalents of commands in
menus are displayed when the `callback' is a symbol.
This can be used to specify keys for more complex menu
items.  It is passed through `substitute-command-keys'
first.

:style    <style>   Specifies what kind of object this menu item is:

nil     A normal menu item.
toggle  A toggle button.
radio   A radio button.
button  A menubar button.

The only difference between toggle and radio buttons is
how they are displayed.  But for consistency, a toggle
button should be used when there is one option whose
value can be turned on or off, and radio buttons should
be used when there is a set of mutually exclusive
options.  When using a group of radio buttons, you
should arrange for no more than one to be marked as
selected at a time.

:selected <form>    Meaningful only when STYLE is `toggle', `radio' or
`button'.  This specifies whether the button will be in
the selected or unselected state.

:included <form>    This can be used to control the visibility of a menu or
menu item.  The form is evaluated and the menu or menu
item is only displayed if the result is non-nil.

:config  <symbol>   This is an efficient shorthand for
:included (memq symbol menubar-configuration)
See the variable `menubar-configuration'.

:filter <function>  A menu filter can only be used at the beginning of a
submenu description (i.e. not in a menu item itself).
(Remember that most of the keywords can take evaluated
expressions as well as constants.)  The filter is used to
incrementally create a submenu only when it is selected
by the user and not every time the menubar is activated.
The filter function is passed the list of menu items in
the submenu and must return the modified list to be
actually used.  The filter MUST NOT destructively modify
the list of menu items passed to it.  It is called only
when the menu is about to be displayed, so other menus
may already be displayed.  Vile and terrible things will
happen if a menu filter function changes the current
buffer, window, or frame.  It also should not raise,
lower, or iconify any frames.  Basically, the filter
function should have no side-effects.

:key-sequence keys  Used in FSF Emacs as an hint to an equivalent keybinding.
Ignored by SXEmacs for easymenu.el compatibility.
(SXEmacs computes this information automatically.)

For example:

("%_File"
:filter file-menu-filter   ; file-menu-filter is a function that takes
; one argument (a list of menu items) and
; returns a list of menu items
[ "Save %_As..."    write-file  t ]
[ "%_Revert Buffer" revert-buffer (buffer-modified-p) ]
[ "R%_ead Only"     toggle-read-only :style toggle
:selected buffer-read-only ]
)

See menubar-items.el for many more examples.

After the menubar is clicked upon, but before any menus are popped up,
the functions on the `activate-menubar-hook' are invoked to make top-level
changes to the menus and menubar.  Note, however, that the use of menu
filters (using the :filter keyword) is usually a more efficient way to
dynamically alter or sensitize menus.  
*/ ,
			  menubar_variable_changed);

	Vcurrent_menubar = Qnil;

	DEFVAR_LISP("activate-menubar-hook", &Vactivate_menubar_hook	/*
Function or functions called before a menubar menu is pulled down.
These functions are called with no arguments, and should interrogate and
modify the value of `current-menubar' as desired.

The functions on this hook are invoked after the mouse goes down, but before
the menu is mapped, and may be used to activate, deactivate, add, or delete
items from the menus.  However, it is probably the case that using a :filter
keyword in a submenu would be a more efficient way of updating menus.  See
the documentation of `current-menubar'.

These functions may return the symbol `t' to assert that they have made
no changes to the menubar.  If any other value is returned, the menubar is
recomputed.  If `t' is returned but the menubar has been changed, then the
changes may not show up right away.  Returning `nil' when the menubar has
not changed is not so bad; more computation will be done, but redisplay of
the menubar will still be performed optimally.
									 */ );
	Vactivate_menubar_hook = Qnil;
	defsymbol(&Qactivate_menubar_hook, "activate-menubar-hook");

	DEFVAR_BOOL("menubar-show-keybindings", &menubar_show_keybindings	/*
If true, the menubar will display keyboard equivalents.
If false, only the command names will be displayed.
										 */ );
	menubar_show_keybindings = 1;

	DEFVAR_LISP_MAGIC("menubar-configuration", &Vmenubar_configuration	/*
A list of symbols, against which the value of the :config tag for each
menubar item will be compared.  If a menubar item has a :config tag, then
it is omitted from the menubar if that tag is not a member of the
`menubar-configuration' list.
										 */ , menubar_variable_changed);
	Vmenubar_configuration = Qnil;

	DEFVAR_LISP("menubar-pointer-glyph", &Vmenubar_pointer_glyph	/*
*The shape of the mouse-pointer when over the menubar.
This is a glyph; use `set-glyph-image' to change it.
If unspecified in a particular domain, the window-system-provided
default pointer is used.
									 */ );

	DEFVAR_LISP("menu-accelerator-prefix", &Vmenu_accelerator_prefix	/*
Prefix key(s) that must be typed before menu accelerators will be activated.
Set this to a value acceptable by define-key.

NOTE: This currently only has any effect under X Windows.
										 */ );
	Vmenu_accelerator_prefix = Qnil;

	DEFVAR_LISP("menu-accelerator-modifiers", &Vmenu_accelerator_modifiers	/*
Modifier keys which must be pressed to get to the top level menu accelerators.
This is a list of modifier key symbols.  All modifier keys must be held down
while a valid menu accelerator key is pressed in order for the top level
menu to become active.

NOTE: This currently only has any effect under X Windows.

See also menu-accelerator-enabled and menu-accelerator-prefix.
										 */ );
	Vmenu_accelerator_modifiers = list1(Qmeta);

	DEFVAR_LISP("menu-accelerator-enabled", &Vmenu_accelerator_enabled	/*
Whether menu accelerator keys can cause the menubar to become active.
If 'menu-force or 'menu-fallback, then menu accelerator keys can
be used to activate the top level menu.  Once the menubar becomes active, the
accelerator keys can be used regardless of the value of this variable.

menu-force is used to indicate that the menu accelerator key takes
precedence over bindings in the current keymap(s).  menu-fallback means
that bindings in the current keymap take precedence over menu accelerator keys.
Thus a top level menu with an accelerator of "T" would be activated on a
keypress of Meta-t if menu-accelerator-enabled is menu-force.
However, if menu-accelerator-enabled is menu-fallback, then
Meta-t will not activate the menubar and will instead run the function
transpose-words, to which it is normally bound.

See also menu-accelerator-modifiers and menu-accelerator-prefix.
										 */ );
	Vmenu_accelerator_enabled = Qnil;

	DEFVAR_LISP("menu-accelerator-map", &Vmenu_accelerator_map	/*
Keymap for use when the menubar is active.
The actions menu-quit, menu-up, menu-down, menu-left, menu-right,
menu-select and menu-escape can be mapped to keys in this map.
NOTE: This currently only has any effect under X Windows.

menu-quit    Immediately deactivate the menubar and any open submenus without
selecting an item.
menu-up      Move the menu cursor up one row in the current menu.  If the
move extends past the top of the menu, wrap around to the bottom.
menu-down    Move the menu cursor down one row in the current menu.  If the
move extends past the bottom of the menu, wrap around to the top.
If executed while the cursor is in the top level menu, move down
into the selected menu.
menu-left    Move the cursor from a submenu into the parent menu.  If executed
while the cursor is in the top level menu, move the cursor to the
left.  If the move extends past the left edge of the menu, wrap
around to the right edge.
menu-right   Move the cursor into a submenu.  If the cursor is located in the
top level menu or is not currently on a submenu heading, then move
the cursor to the next top level menu entry.  If the move extends
past the right edge of the menu, wrap around to the left edge.
menu-select  Activate the item under the cursor.  If the cursor is located on
a submenu heading, then move the cursor into the submenu.
menu-escape  Pop up to the next level of menus.  Moves from a submenu into its
parent menu.  From the top level menu, this deactivates the
menubar.

This keymap can also contain normal key-command bindings, in which case the
menubar is deactivated and the corresponding command is executed.

The action bindings used by the menu accelerator code are designed to mimic
the actions of menu traversal keys in a commonly used PC operating system.
									 */ );

	Fprovide(intern("menubar"));
	Fprovide(intern("menu-accelerator-support"));
}

void specifier_vars_of_menubar(void)
{
	DEFVAR_SPECIFIER("menubar-visible-p", &Vmenubar_visible_p	/*
*Whether the menubar is visible.
This is a specifier; use `set-specifier' to change it.
									 */ );
	Vmenubar_visible_p = Fmake_specifier(Qboolean);

	set_specifier_fallback(Vmenubar_visible_p, list1(Fcons(Qnil, Qt)));
	set_specifier_caching(Vmenubar_visible_p,
			      offsetof(struct window, menubar_visible_p),
			      menubar_visible_p_changed,
			      offsetof(struct frame, menubar_visible_p),
			      menubar_visible_p_changed_in_frame, 0);
}

void complex_vars_of_menubar(void)
{
	Vmenubar_pointer_glyph = Fmake_glyph_internal(Qpointer);

	Vmenu_accelerator_map = Fmake_keymap(Qnil);
}