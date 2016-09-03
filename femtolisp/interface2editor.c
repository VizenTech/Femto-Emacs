/*
 * Interface functions for femtoLisp to FemtoEmacs
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include "llt/llt.h"
#include "flisp.h"
#include "llt/random.h"

#include "../public.h"

/* Interface to the editor */
extern int is_digit(char_t c);


/*
 * interface to editor functions of form
 *     void func(void)
 */
static value_t bkwrd(value_t *args, u_int32_t nargs) {
	argcount("backward", nargs, 1);
	value_t a = args[0];
	long int n = numval(a); /*Learn: Lisp num -> C int*/
	while (n-- > 0)
		left();
	return FL_T;
}

static value_t forwrd(value_t *args, u_int32_t nargs) {
	argcount("forward", nargs, 1);
	value_t a = args[0];
	long int n = numval(a); /*Learn: Lisp num -> C int*/
	while (n-- > 0)
		right();
	return FL_T;
}

static value_t backspace(value_t *args, u_int32_t nargs) {
	argcount("backwards-delete-char", nargs, 1);
	value_t a = args[0];
	long int n = numval(a); /*Learn: Lisp num -> C int*/
	while (n-- > 0)
		backsp();
	return FL_T;
}

static value_t linebegin(value_t *args, u_int32_t nargs) {
	argcount("beginning-of-line", nargs, 0);
	lnbegin();
	return FL_T;
}

static value_t lineend(value_t *args, u_int32_t nargs) {
	argcount("end-of-line", nargs, 0);
	lnend();
	return FL_T;
}

extern int nscrap;
extern char_t *scrap; 

/*
 * Lucas - we need to discuss.
 * This will cause a memory leak!
 * Hugh Barney
 */

static value_t fl_clipboard(value_t *args, u_int32_t nargs) {
  argcount("clipboard", nargs, 0);
  int i= 0;
  if ((scrap == NULL) || (nscrap < 1)) {
	  char *str= malloc(1);     /* memory leak !, how does it get freed next time ? */
    str[0] = 0;
    return (string_from_cstr(str));
  }
  char *str= malloc(nscrap + 4); /* memory leak !, how does it get freed next time ? */
  for(i=0; i<nscrap; i++) {
			str[i]= (char) scrap[i];
		}
  str[i]= '\0';
  return (string_from_cstr(str));

}

/*
 * hughbarney:  Lucas this does the same thing as region
 * so we dont need it
 *
 */
static value_t fl_region(value_t *args, u_int32_t nargs) {
  argcount("region", nargs, 0);
  int i= 0;
  copy();

  if ((scrap == NULL) || (nscrap < 1)) {
    char *str= malloc(1);
    str[0] = 0;
    return (string_from_cstr(str));
  }
  char *str= malloc(nscrap + 4);   /* this is a memory leak !as it will never get deleted */
  for(i=0; i<nscrap; i++) {
			str[i]= (char) scrap[i];
		}
  str[i]= '\0';
  return (string_from_cstr(str));

}

/*
 *  we should be using os.getenv() from femtolisp
 *
 */
static value_t fl_home(value_t *args, u_int32_t nargs) {
  argcount("home", nargs, 1);
  value_t a = args[0]; /*Learn: pick an arg */
  char *str = cptr(a); /*Learn: string Lisp -> string C  */
  if (str == NULL) {
    return (string_from_cstr(getenv("HOME")));
  }
  char *buff= malloc(400);    //  MEMORY LEAK !!!!
  sprintf(buff, "%s/%s", getenv("HOME"), str);
  return (string_from_cstr(buff));
}

/*
 * we already have kill-region()
 */
static value_t fl_cutregion(value_t *args, u_int32_t nargs) {
  argcount("cutregion", nargs, 0);
  int i= 0;
  cut();

  if ((scrap == NULL) || (nscrap < 1)) {
    char *str= malloc(1);
    str[0] = 0;
    return (string_from_cstr(str));
  }
  char *str= malloc(nscrap + 4); 
  for(i=0; i<nscrap; i++) {
			str[i]= (char) scrap[i];
		}
  str[i]= '\0';
  return (string_from_cstr(str));

}


static value_t copy_region(value_t *args, u_int32_t nargs) {
	argcount("copy-region", nargs, 0);
	copy();
	return FL_T;
}

static value_t eval_blk(value_t *args, u_int32_t nargs) {
	argcount("eval-block", nargs, 0);
	eval_block();
	return FL_T;
}

static value_t del_other_windows(value_t *args, u_int32_t nargs) {
	argcount("delete-other-windows", nargs, 0);
	delete_other_windows();
	return FL_T;
}


static value_t fe_split_window(value_t *args, u_int32_t nargs) {
	argcount("split-window-below", nargs, 0);
	split_window();
	return FL_T;
}


static value_t gotoln(value_t *args, u_int32_t nargs) {
	argcount("goto-line", nargs, 1);
	value_t a = args[0];
	goto_line(numval(a)); /*Learn: Lisp num -> C int*/
	return FL_T;
}

static value_t kill_region(value_t *args, u_int32_t nargs) {
	argcount("kill-region", nargs, 0);
	cut();
	return FL_T;
}

static value_t fe_copy_region(value_t *args, u_int32_t nargs) {
	argcount("copy-region", nargs, 0);
	copy();
	return FL_T;
}

static value_t lst_buffers(value_t *args, u_int32_t nargs) {
	argcount("list-buffers", nargs, 0);
	list_buffers();
	return FL_T;
}

static value_t next_line(value_t *args, u_int32_t nargs) {
	argcount("next-line", nargs, 0);
	down();
	return FL_T;
}

static value_t previous_line(value_t *args, u_int32_t nargs) {
	argcount("previous-line", nargs, 0);
	up();
	return FL_T;
}

static value_t set_mark(value_t *args, u_int32_t nargs) {
	argcount("set-mark", nargs, 0);
	iblock();
	return FL_T;
}

static value_t fe_update_display(value_t *args, u_int32_t nargs) {
	argcount("update-display", nargs, 0);
	update_display();
	return FL_T;
}

static value_t yank(value_t *args, u_int32_t nargs) {
	argcount("yank", nargs, 0);
	paste();
	return FL_T;
}


/*
 *  functions returning int
 */

static value_t fe_count_buffers(value_t *args, u_int32_t nargs) {
	argcount("get-buffer-count", nargs, 0);
	return (mk_uint32(count_buffers()));
}


/*
 * functions returning a char *
 */

static value_t fe_get_version_string(value_t *args, u_int32_t nargs) {
	argcount("get-version-string", nargs, 0);
	return (string_from_cstr(get_version_string()));
}

static value_t fe_get_key(value_t *args, u_int32_t nargs) {
	argcount("get-key", nargs, 0);
	return string_from_cstr(fe_get_input_key());
}

static value_t fe_get_key_name(value_t *args, u_int32_t nargs) {
	argcount("get-key-name", nargs, 0);
	return string_from_cstr(get_key_name());
}

static value_t fe_get_key_binding(value_t *args, u_int32_t nargs) {
	argcount("get-key-binding", nargs, 0);
	return string_from_cstr(get_key_binding());
}

static value_t fe_get_buffer_name(value_t *args, u_int32_t nargs) {
	argcount("get-buffer-name", nargs, 0);
	return string_from_cstr(get_current_bufname());
}

static value_t fe_get_clipboard(value_t *args, u_int32_t nargs) {
	argcount("get-clipboard", nargs, 0);
	return string_from_cstr(get_clipboard());
}


/*
 * interface to editor functions of form
 *   char *func(char *)
 */

static value_t fe_trim(value_t *args, u_int32_t nargs) {
	argcount("trim", nargs, 1);
	value_t a = args[0];
	char *str = cptr(a);
	return string_from_cstr(string_trim(str));
}


/*
 * interface to editor functions of form func(char *)
 */

static value_t msg_lisp(value_t *args, u_int32_t nargs) {
	argcount("message", nargs, 1);
	value_t a = args[0]; /*Learn: pick an arg */
	char *str = cptr(a); /*Learn: string Lisp -> string C  */
	msg(str);
	return FL_T;
}

/*
 * interface to functions of form
 *    int func(char *)
 *
 */

static value_t insrt(value_t *args, u_int32_t nargs) {
	argcount("insert", nargs, 1);
	value_t a = args[0]; /*Learn: pick an arg */
	char *str = cptr(a); /*Learn: string Lisp -> string C  */
	insert_string(str);
	return FL_T;
}

static value_t src_forward(value_t *args, u_int32_t nargs) {
	point_t founded;
	argcount("search-forward", nargs, 1);
	value_t a = args[0];
	char *str = cptr(a);
	founded = search_forward(str);
	move_to_search_result(founded);
	return (founded == -1 ? FL_F : FL_T);
}

static value_t src_backwards(value_t *args, u_int32_t nargs) {
	point_t founded;
	argcount("search-backwards", nargs, 1);
	value_t a = args[0];
	char *str = cptr(a);
	founded = search_backwards(str);
	move_to_search_result(founded);
	return (founded == -1 ? FL_NIL : FL_T);
}

static value_t fe_select_buffer_byname(value_t *args, u_int32_t nargs) {
	argcount("select-buffer-by-name", nargs, 1);
	value_t a = args[0];
	char *str = cptr(a);
	int result = select_buffer_byname(str);
	return (result ? FL_T : FL_NIL);
}

static value_t fe_delete_buffer_byname(value_t *args, u_int32_t nargs) {
	argcount("delete-buffer-by-name", nargs, 1);
	value_t a = args[0];
	char *str = cptr(a);
	int result = delete_buffer_byname(str);
	return (result ? FL_T : FL_NIL);
}

static value_t fe_save_buffer_byname(value_t *args, u_int32_t nargs) {
	argcount("save-buffer-by-name", nargs, 1);
	value_t a = args[0];
	char *str = cptr(a);
	int result = save_buffer_byname(str);
	return (result ? FL_T : FL_NIL);
}


/*
 * code for syntax highlighting
 */

int nLangs = 0;
int thisLanguage = -1;
char LangCode[10][5];
int numWords[100];
char hiLite[10][100][30];
char line_comment[10][8];
char begin_comment[10][8];
char end_comment[10][8];

int notsep(char_t *p) {

	/* what is the purpose of this check, also p is a pointer ? */
	if (p < 1)
		return 0;

	char_t c = *p;
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9') || (c == '_'));
}


/* seq checks wether *p points to the sequence of chars given by str */
int seq(char_t *p, char *str) {
	int i;
	for (i = 0; i < strlen(str); i++) {
		if (*(p + i) != str[i])
			return 0;
	}
	return 1;
}

/* keywrd implements a finite automaton */
int negative_prefix(char_t *p) { return (*p == '-' && is_digit(*(p + 1))); }

int number_prefix(char_t *p, int *j) {
	/* Numbers start with a digit or with '-' followed by a digit */
	if (is_digit(*p)) {
		*j = *j + 1;
		return 1;
	}
	if (negative_prefix(p)) {
		*j = *j + 2;
		return 2;
	}
	return 0;
}

int exponent_prefix(char_t *p, int *j) {
	int jvalue = *j;
	if (*(p + jvalue) != 'e')
		return 0;
	if (is_digit(*(p + jvalue + 1))) {
		*j = *j + 2;
		return 1;
	}
	if (*(p + jvalue + 1) != '-')
		return 0;
	if (is_digit(*(p + jvalue + 2))) {
		*j = *j + 3;
		return 1;
	}
	return 0;
}

/*
 * check if in a class of token , return legth of token
 * 
 */
int scan_for_keywords(char_t *p, int *token_type) {

	int i; /* what is i ?, suggest rename to something more meaningful */
	int j; /* what is j ?, suggest rename to something more meaningful */
	
	/*
	 * All tokens are preceded by a separator.  Therefore, if
	 * there is not a separator at (p - 1), tokenizer fails.
	 */
	
	if (notsep(p - 1))
		return 0;

	/*
	 * In a deterministic finite automaton, a given
	 *  prefix determines the token class.
	 */
	j = 0;
	if (number_prefix(p, &j)) {
		while (is_digit(*(p + j))) {
			j = j + 1;
		}
		if (*(p + j) == '.') {
			j = j + 1;
			while (is_digit(*(p + j))) {
				j = j + 1;
			}
		}
		if (exponent_prefix(p, &j)) {
			while (is_digit(*(p + j))) {
				j = j + 1;
			}
		}
		if (!notsep(p + j)) {
			*token_type = ID_TOKEN_DIGITS;
			return j;
		}
		*token_type = ID_TOKEN_NONE;
		return 0;
	}

	/* If automaton has not number prefix, it tries to recognize a keyword */
	if (thisLanguage < 0) {
		*token_type = ID_TOKEN_NONE;
		return 0;
	}
	
	j = 0; /* Reset j */
	for (i = 0; i < numWords[thisLanguage]; i++) {
		if (seq(p, hiLite[thisLanguage][i])) {
			*token_type = ID_TOKEN_KEYWORD;
			return strlen(hiLite[thisLanguage][i]);
		}
	}
	*token_type = ID_TOKEN_NONE;
	return 0;
}

/*
 * check for comments based on previous status of comments
 */
void scan_for_comments(char_t *p, int *blk_comment, int *ln_comment) {
	if (thisLanguage < 0)
		return;
	if ((*ln_comment == 0) && seq(p, line_comment[thisLanguage]))
		*ln_comment = 1;
	if ((*ln_comment == 1) && (*p == '\n'))
		*ln_comment = 0;
	if ((*blk_comment== 0) && seq(p, begin_comment[thisLanguage]))
		*blk_comment= 1;
	if ((*blk_comment== 1) && seq(p, end_comment[thisLanguage]))
		*blk_comment= 0;
	return;
}

void setLanguage(char *extension) {
	int i;
	thisLanguage = -1;
	for (i = 0; i < nLangs; i++) {
		if (strcmp(LangCode[i], extension) == 0) {
			thisLanguage = i;
		}
	}
}

static value_t fe_newlanguage(value_t *args, u_int32_t nargs) {
	/* Check the number of arguments */
	argcount("newlanguage", nargs, 4);
	value_t a = args[0];
	int code = nLangs;
	char *str = cptr(a);
	int sz = strlen(str);
	int i;
	for (i = 0; i < sz; i++) {
		LangCode[code][i] = str[i];
	}
	LangCode[code][i] = '\0';
	numWords[code] = 0;
	nLangs = nLangs + 1;

	/* Lisp string for line comment prefix */
	value_t linec = args[1];
	/* Lisp string for block comment begin */
	value_t beginc = args[2];
	/* Lisp string for block comment end */
	value_t endc = args[3];

	/* femtolisp has a python style macro to
	   transform lisp-type into c-pointer */
	char *ln_cmm = cptr(linec);
	char *begin_cmm = cptr(beginc);
	char *end_cmm = cptr(endc);

	/* Copy the strings into the comment vectors
	   to avoid the  garbage collector */

	for (i = 0; i < strlen(ln_cmm); i++) {
		line_comment[code][i] = ln_cmm[i];
	}
	line_comment[code][i] = '\0';

	for (i = 0; i < strlen(begin_cmm); i++) {
		begin_comment[code][i] = begin_cmm[i];
	}
	begin_comment[code][i] = '\0';

	for (i = 0; i < strlen(end_cmm); i++) {
		end_comment[code][i] = end_cmm[i];
	}
	end_comment[code][i] = '\0';

	return FL_T;
}

static value_t fe_keyword(value_t *args, u_int32_t nargs) {
	argcount("keyword", nargs, 1);
	value_t a = args[0];
	int code = nLangs - 1;
	int nw = numWords[code];
	char *str = cptr(a);
	int sz = strlen(str);
	int i;
	for (i = 0; i < sz; i++) {
		hiLite[code][nw][i] = str[i];
	}
	hiLite[code][nw][i] = '\0';
	numWords[code] = numWords[code] + 1;
	return a;
}

/* end of interface to the editor */
extern void stringfuncs_init(void);
extern void table_init(void);
extern void iostream_init(void);

/* Femto inteface bultin functions */
static builtinspec_t builtin_info[] = {
	{"insert", insrt},
	{"backward-delete-char", backspace},
	{"backward-character", bkwrd},
	{"forward-character", forwrd},
	{"beginning-of-line", linebegin},
	{"end-of-line", lineend},
	{"copy-region", fe_copy_region},


	{"clipboard", fl_clipboard},
	{"home", fl_home},
        {"region", fl_region},
	{"cutregion", fl_cutregion},

	{"copy-region", copy_region},
	{"eval-block", eval_blk},
	{"get-key", fe_get_key},
	{"message", msg_lisp},
	{"delete-other-windows", del_other_windows},
	{"delete-buffer-by-name", fe_delete_buffer_byname},
	{"goto-line", gotoln},
	{"kill-region", kill_region},
	{"list-buffers", lst_buffers},
	{"next-line", next_line},
	{"previous-line", previous_line},
	{"save-buffer-by-name", fe_save_buffer_byname},
	{"search-forward", src_forward},
	{"search-backwards", src_backwards},
	{"set-mark", set_mark},
	{"select-buffer-by-name", fe_select_buffer_byname},
	{"split-window-below", fe_split_window},
	{"trim", fe_trim},
	{"update-display", fe_update_display},
	{"yank", yank},
	{"get-version-string", fe_get_version_string},
	{"get-buffer-count", fe_count_buffers},
	{"get-buffer-name", fe_get_buffer_name},
	{"get-key-name", fe_get_key_name},
	{"get-key-binding", fe_get_key_binding},
	{"get-clipboard", fe_get_clipboard},
	{"keyword", fe_keyword},
	{"newlanguage", fe_newlanguage},

	/*End Interface*/
	{NULL, NULL}
};

void interface_init(void)
{
	assign_global_builtins(builtin_info);
}
