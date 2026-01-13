#define _POSIX_C_SOURCE 200809L

#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../../src/lexer/lexer.h"

const char *get_type(enum types type)
{
	switch (type)
	{
		case IF:
			return "IF";
		case THEN:
			return "THEN";
		case ELIF:
			return "ELIF";
		case ELSE:
			return "ELSE";
		case FI:
			return "FI";

		case SEMICOLON:
			return "SEMICOLON";
		case NEWLINE:
			return "NEWLINE";

		case S_QUOTE:
			return "S_QUOTE";
		case D_QUOTE:
			return "D_QUOTE";

		case REDIR:
			return "REDIR";
		case PIPE:
			return "PIPE";
		case OR:
			return "OR";
		case AND:
			return "AND";
		case NEG:
			return "NEG";

		case WHILE:
			return "WHILE";
		case UNTIL:
			return "UNTIL";
		case DO:
			return "DO";
		case DONE:
			return "DONE";

		case FOR:
			return "FOR";
		case IN:
			return "IN";

		case WORDS:
			return "WORDS";
		default:
			return "UNKNOWN";
	}
}

void print_lexing(const char *input)
{
	if (!input) return;

	FILE *stream = fmemopen((void *)input, strlen(input), "r");
	if (!stream) return;

	struct token *tok = get_token(stream);

	while (tok != NULL)
	{
		printf("[%s](%s)\n", tok->content, get_type(tok->type));
		free_token(tok);
		tok = get_token(stream);
	}

	fclose(stream);
}

void setup_stdout(void)
{
	cr_redirect_stdout();
}

Test(edge_case, empty_input, .init = setup_stdout)
{
	char *input = "";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("");
}

// Tests to check if lexer handles correctly single tokens
// One test for each type of token
TestSuite(single_token, .init = setup_stdout);

Test(single_token, word_token, .init = setup_stdout)
{
	char *input = "bien";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[bien](WORDS)\n");
}

Test(single_token, if_token, .init = setup_stdout)
{
	char *input = "if";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[if](IF)\n");
}

Test(single_token, then_token, .init = setup_stdout)
{
	char *input = "then";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[then](THEN)\n");
}

Test(single_token, elif_token, .init = setup_stdout)
{
	char *input = "elif";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[elif](ELIF)\n");
}

Test(single_token, else_token, .init = setup_stdout)
{
	char *input = "else";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[else](ELSE)\n");
}

Test(single_token, fi_token, .init = setup_stdout)
{
	char *input = "fi";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[fi](FI)\n");
}

Test(single_token, semicolon_token, .init = setup_stdout)
{
	char *input = ";";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[;](SEMICOLON)\n");
}

Test(single_token, newline_token, .init = setup_stdout)
{
	char *input = "\n";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[\n](NEWLINE)\n");
}

Test(single_token, single_quote_token, .init = setup_stdout)
{
	char *input = "'";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("['](S_QUOTE)\n");
}

Test(single_token, double_quote_token, .init = setup_stdout)
{
	char *input = "\"";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[\"](D_QUOTE)\n");
}

Test(single_token, and_token, .init = setup_stdout)
{
	char *input = "&&";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[&&](AND)\n");
}

Test(single_token, or_token, .init = setup_stdout)
{
	char *input = "||";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[||](OR)\n");
}

Test(single_token, pipe_token, .init = setup_stdout)
{
	char *input = "|";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[|](PIPE)\n");
}

Test(single_token, while_token, .init = setup_stdout)
{
	char *input = "while";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[while](WHILE)\n");
}

Test(single_token, until_token, .init = setup_stdout)
{
	char *input = "until";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[until](UNTIL)\n");
}

Test(single_token, do_token, .init = setup_stdout)
{
	char *input = "do";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[do](DO)\n");
}

Test(single_token, done_token, .init = setup_stdout)
{
	char *input = "done";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[done](DONE)\n");
}

Test(single_token, for_token, .init = setup_stdout)
{
	char *input = "for";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[for](FOR)\n");
}

Test(single_token, in_token, .init = setup_stdout)
{
	char *input = "in";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[in](IN)\n");
}

Test(single_token, redir_token_1, .init = setup_stdout)
{
	char *input = ">";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[>](REDIR)\n");
}

Test(single_token, redir_token_2, .init = setup_stdout)
{
	char *input = "<";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[<](REDIR)\n");
}

Test(single_token, redir_token_3, .init = setup_stdout)
{
	char *input = ">>";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[>>](REDIR)\n");
}

Test(single_token, redir_token_4, .init = setup_stdout)
{
	char *input = ">&";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[>&](REDIR)\n");
}

Test(single_token, redir_token_5, .init = setup_stdout)
{
	char *input = "<&";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[<&](REDIR)\n");
}

Test(single_token, redir_token_6, .init = setup_stdout)
{
	char *input = ">|";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[>|](REDIR)\n");
}

Test(single_token, redir_token_7, .init = setup_stdout)
{
	char *input = "<>";

	print_lexing(input);

	fflush(stdout);

	cr_assert_stdout_eq_str("[<>](REDIR)\n");
}

// Tests for if clausures
TestSuite(if_clausure, .init = setup_stdout);

Test(if_clausure, simple_if, .init = setup_stdout)
{
	char *input = "if true; then false; fi";

	print_lexing(input);
	fflush(stdout);

	cr_assert_stdout_eq_str("[if](IF)\n"
			"[true](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[then](THEN)\n"
			"[false](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[fi](FI)\n");
}

Test(if_clausure, if_else, .init = setup_stdout)
{
	char *input = "if true; then false; else true; fi";

	print_lexing(input);
	fflush(stdout);

	cr_assert_stdout_eq_str("[if](IF)\n"
			"[true](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[then](THEN)\n"
			"[false](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[else](ELSE)\n"
			"[true](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[fi](FI)\n");
}

Test(if_clausure, if_elif, .init = setup_stdout)
{
	char *input = "if true; then false; elif true; then true; fi";

	print_lexing(input);
	fflush(stdout);

	cr_assert_stdout_eq_str("[if](IF)\n"
			"[true](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[then](THEN)\n"
			"[false](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[elif](ELIF)\n"
			"[true](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[then](THEN)\n"
			"[true](WORDS)\n"
			"[;](SEMICOLON)\n"
			"[fi](FI)\n");
}
