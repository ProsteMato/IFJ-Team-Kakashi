/**
 * Project: Implementace překladače imperativního jazyka IFJ19
 *
 * @brief IFJcode19 generator implementation
 *
 * @author Michal Koval <xkoval17@stud.fit.vutbr.cz>
 */

#include "generator.h"

void CL_init(){
	code_list.first = NULL;
	code_list.last = NULL;
}

int CL_add_line(Code *line){
	if (line == NULL){
		return INTERNAL_ERROR;
	}
	if (!append_char(&(line->inst), &(line->len), &(line->cap), '\0')){
		return INTERNAL_ERROR;
	}
	Code_line *new = (Code_line*) malloc (sizeof(Code_line));
	if (new == NULL){
		// chyba alokacie pamate
		return INTERNAL_ERROR;
	}
	new->code = line;
	new->next = NULL;
	if (code_list.first == NULL && code_list.last == NULL){
		// ziaden prvok v liste
		code_list.first = new;
		code_list.last = new;
	} else{
		code_list.last->next = new;
		code_list.last = new;
	}
	return OK;
}

void CL_destroy(Code_line *line){
	if (line == NULL){
		return;
	}
	Code_line *tmp;
	tmp = line;
	if (tmp->code != NULL){
		free(tmp->code);
	}
	tmp = tmp->next;
	free(line);
	CL_destroy(tmp);
}

int init_generator(){
	while_counter = 0;
	CL_init();
	if (gen_header()){
		return INTERNAL_ERROR;
	}
	return OK;
}
void print_final_code(){
	Code_line *tmp = code_list.first;
	while (tmp != NULL){
		printf("%s\n", tmp->code->inst);
		tmp = tmp->next;
	}
	CL_destroy(code_list.first);
}

Code* create_code(){
	Code *tmp = malloc(sizeof(Code));
	if (tmp == NULL){
		return NULL;
	}
	tmp->cap = DEFAULT_STR_LEN;
	tmp->len = 0;
	tmp->inst = malloc(sizeof(char) * DEFAULT_STR_LEN);
	if (tmp == NULL){
		return NULL;
	}
	return tmp;
}

int add_code(Code *code, char *inst){
	size_t len = strlen(inst);
	for (size_t i = 0; i < len; i++){
		if (!append_char(&(code->inst), &(code->len), &(code->cap), inst[i])){
			return INTERNAL_ERROR;
		}
	}
	return OK;
}

char* int_to_str(int i){
	char *str = malloc(sizeof(char) * INT2STR_SIZE);
	if (str == NULL){
		return NULL;
	}
	sprintf(str, "%d", i);
	return str;
}

char* float_to_str(char *f){
	char *tmp = malloc(sizeof(char)*FLOAT2STR_SIZE);
	if (tmp == NULL){
		return NULL;
	}
	sprintf(tmp, "%a", atof(f));
	return tmp;
}

char* transform_for_write(char *str){
	Code *code = create_code();
	if (code == NULL){
		return NULL;
	}
	size_t len = strlen(str);
	for (int i = 0; i < len; i++){
		if ((str[i] >= 0 && str[i] <= 32) ||
			str[i] == 35 || str[i] == 92){
			if (!append_char(&(code->inst), &(code->len), &(code->cap), '\\'))
				return NULL;
			if (!append_char(&(code->inst), &(code->len), &(code->cap), '0'))
				return NULL;
			int jednotky = str[i]%10;
			int desiatky = (str[i] - jednotky)/10; 
			if (!append_char(&(code->inst), &(code->len), &(code->cap), desiatky+'0'))
				return NULL;
			if (!append_char(&(code->inst), &(code->len), &(code->cap), jednotky+'0'))
				return NULL;
		} else {
			if (!append_char(&(code->inst), &(code->len), &(code->cap), str[i]))
				return NULL;
		}
	}
	char *ret = code->inst;
	free(code);
	return ret;
}

// -------------------
// generovacie funkcie
// -------------------
/* is_global
int gen_less_than(char *op1, char *op2){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	// LT GF@&expr&val op1 op2
	if (add_code(code, "LT GF@&expr&val "))
		return INTERNAL_ERROR;

	// op1 je premenna
	if (is_variable_defined(root,op1)){
		if (is_global_variable(root,op1){
			if (add_code(code, "GF@"))
				return INTERNAL_ERROR;
		} else {
			if (add_code(code, "LF@"))
				return INTERNAL_ERROR;
		}
		if (add_code(code, op1))
				return INTERNAL_ERROR;
	// op1 je konst
	} else {
		if (add_code(code, op1))
			return INTERNAL_ERROR;
	}

	// op2 je premenna
	if (is_variable_defined(root,op2)){
		if (is_global_variable(root,op2){
			if (add_code(code, " GF@"))
				return INTERNAL_ERROR;
		} else {
			if (add_code(code, " LF@"))
				return INTERNAL_ERROR;
		}
		if (add_code(code, op2))
				return INTERNAL_ERROR;
	// op1 je konst
	} else {
		if (add_code(code, op))
			return INTERNAL_ERROR;
	}
}

int gen_more_than(char *op1, char *op2){

}
int gen_equal(char *op1, char *op2){

}
int gen_equal_less(char *op1, char *op2){

}
int gen_equal_more(char *op1, char *op2){

}
int gen_not_equal(char *op1, char *op2){

}
*/
int gen_while_label(){  // doplnit o unique labely
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	if (add_code(code, "LABEL $while$\0"))
		return INTERNAL_ERROR;
	char *un_while_n = int_to_str(while_counter);
	if (un_while_n == NULL){
		return INTERNAL_ERROR;
	}
	if (add_code(code, un_while_n))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_while_begin(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	char *unq_while_n = int_to_str(while_counter);
	if (unq_while_n == NULL)
		return INTERNAL_ERROR;

	if (add_code(code, "JUMPIFEQ $while_end$\0"))
		return INTERNAL_ERROR;
	if (add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (add_code(code, " GF@&expr&val bool@false"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_while_end(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	char *unq_while_n = int_to_str(while_counter);
	if (unq_while_n == NULL)
		return INTERNAL_ERROR;

	if (add_code(code, "JUMP $while$\0"))
		return INTERNAL_ERROR;
	if (add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (add_code(code, "LABEL $while_end$\0"))
		return INTERNAL_ERROR;
	if (unq_while_n == NULL){
		return INTERNAL_ERROR;
	}
	if (add_code(code, unq_while_n))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	while_counter++;
	return OK;
}

int gen_int2float(char *var){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	int glob = is_global_variable(root, var);

	if (add_code(code, "INT2FLOAT \0"))
		return INTERNAL_ERROR;
	
	if (glob){
		if (add_code(code, "GF@"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, "LF@"))
			return INTERNAL_ERROR;
	}
	if (add_code(code, var))
		return INTERNAL_ERROR;

	if (glob){
		if (add_code(code, " GF@"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, " LF@"))
			return INTERNAL_ERROR;
	}
	if (add_code(code, var))
		return INTERNAL_ERROR;

	if (CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_print(char *symb){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	if (add_code(code, "WRITE \0"))
		return INTERNAL_ERROR;

	// zistenie ci je symb premenna abo konst
	if (is_variable_defined(root, local_table, params,symb)){ // premenna
		if (is_global_variable(root, symb)){ // global
			if (add_code(code, "GF@\0"))
				return INTERNAL_ERROR;
		} else { // lokal
			if (add_code(code, "LF@\0"))
				return INTERNAL_ERROR;
		}
	} else {// konst
		if (add_code(code, "string@\0"))
			return INTERNAL_ERROR;
		char *trans_str = transform_for_write(symb);
		if (trans_str == NULL)
			return INTERNAL_ERROR;
		if (add_code(code, trans_str))
			return INTERNAL_ERROR;
	}

	if (CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_f_call(char *id){
	Code *code = create_code();
	// LABEL $id
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CALL $\0"))
		return INTERNAL_ERROR;
	if (add_code(code, id))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_assing_const_to_val(char *var, Token *token){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	if (add_code(code, "DEFVAR LF@\0"))
		return INTERNAL_ERROR;
	if (add_code(code, var))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@\0"))
		return INTERNAL_ERROR;
	if (add_code(code, var))
		return INTERNAL_ERROR;

	char *converted;
	if(token->type == TK_INT){
		if (add_code(code, " int@"))
			return INTERNAL_ERROR;
		if (add_code(code, token->attribute))
			return INTERNAL_ERROR;
	} else if (token->type == TK_FLOAT){
		if (add_code(code, " float@"))
			return INTERNAL_ERROR;
		converted = float_to_str(token->attribute);
		if (converted == NULL)
			return INTERNAL_ERROR;
		if (add_code(code, converted))
			return INTERNAL_ERROR;
	} else if (token->type == TK_STRING){
		if (add_code(code, " string@"))
			return INTERNAL_ERROR;
		converted = transform_for_write(token->attribute);
		if (add_code(code, converted))
			return INTERNAL_ERROR;
	} else {
		return INTERNAL_ERROR;
	}

	if (CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_f_prep_params(){ // parametre cez TKQueue, pridavane v spravnom poradi, teda 1. vlozeny ako 1., posledny ako posledny
	Token* token = malloc(sizeof(Token));
	if(token == NULL)
		return INTERNAL_ERROR;
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	char *tmp;
	
	// CREATEFRAME
	if (add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// vytvaranie parametrov v cykle
	if (tkq_dequeue(token)){ // otestovat
		return INTERNAL_ERROR;
	}
	for (int i = 1; token != NULL; i++){
		code = create_code();
		if (!code)
			return INTERNAL_ERROR;

		tmp = int_to_str(i);
		if (tmp == NULL){
			return INTERNAL_ERROR;
		}

		if (add_code(code, "DEFVAR TF@%\0"))
			return INTERNAL_ERROR;
		if (add_code(code, tmp))
			return INTERNAL_ERROR;
		if (CL_add_line(code))
			return INTERNAL_ERROR;

		code = create_code();
		if (add_code(code, "MOVE TF@%\0"))
			return INTERNAL_ERROR;
		if (add_code(code, tmp))
			return INTERNAL_ERROR;
		if (add_code(code, " "))
			return INTERNAL_ERROR;

		free(tmp);
		
		if (token->type == TK_ID){ // parameter je premenna
			if (is_global_variable(root,token->attribute)){ // globalna premenna
				if (add_code(code, "GF@%\0"))
					return INTERNAL_ERROR;
			} else { // lokalna premenna
				if (add_code(code, "LF@%\0"))
					return INTERNAL_ERROR;
			}
			if (add_code(code, token->attribute))
				return INTERNAL_ERROR;
		} else if (token->type == TK_FLOAT){ // konst float
			if (add_code(code, "float@"))
				return INTERNAL_ERROR;
			tmp = float_to_str(token->attribute);
			if (tmp == NULL)
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
		} else if (token->type == TK_INT){ // konst int
			if (add_code(code, "int@"))
				return INTERNAL_ERROR;
			if (add_code(code, token->attribute))
				return INTERNAL_ERROR;
		} else if (token->type == TK_STRING){ // konst string
			if (add_code(code, "string@"))
				return INTERNAL_ERROR;
			tmp = transform_for_write(token->attribute);
			if (tmp == NULL)
				return INTERNAL_ERROR;
			if (add_code(code, tmp))
				return INTERNAL_ERROR;
		} else {
			return INTERNAL_ERROR;
		}

		if (CL_add_line(code))
			return INTERNAL_ERROR;

		if (tkq_dequeue(token)){
			return INTERNAL_ERROR;
		}

		free(tmp);
	}
	return OK;
}

int gen_f_start(char *id){
	Code *code = create_code();
	// LABEL $id
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $\0"))
		return INTERNAL_ERROR;
	if (add_code(code, id))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// PUSHFRAME
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// DEFVAR LF@%retval
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}
int gen_f_end(char *id){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $\0"))
		return INTERNAL_ERROR;
	if (add_code(code, id))
		return INTERNAL_ERROR;
	if (add_code(code, "_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	return OK;
}

int gen_header(){
	Code *header = create_code();
	if (!header)
		return INTERNAL_ERROR;
	if (add_code(header, ".IFJcode19\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(header))
		return INTERNAL_ERROR;

	header = create_code();
	if (add_code(header, "DEFVAR GF@&expr&val"))
		return INTERNAL_ERROR;
	if (CL_add_line(header))
		return INTERNAL_ERROR;
	return OK;
}

int gen_inputs(char *dest){
	if (dest == NULL)
		return INTERNAL_ERROR;
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "READ \0"))
		return INTERNAL_ERROR;
	
	int glob = is_global_variable(root, dest);
	if (glob){
		if (add_code(code, "GF@"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, "LF@"))
			return INTERNAL_ERROR;
	}
	if (add_code(code, dest))
		return INTERNAL_ERROR;

	if (add_code(code, " string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_inputi(char *dest){
	if (dest == NULL)
		return INTERNAL_ERROR;
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "READ \0"))
		return INTERNAL_ERROR;

	int glob = is_global_variable(root, dest);
	if (glob){
		if (add_code(code, "GF@"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, "LF@"))
			return INTERNAL_ERROR;
	}

	if (add_code(code, dest))
		return INTERNAL_ERROR;

	if (add_code(code, " int\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_inputf(char *dest){
	if (dest == NULL)
		return INTERNAL_ERROR;
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "READ \0"))
		return INTERNAL_ERROR;

	int glob = is_global_variable(root, dest);
	if (glob){
		if (add_code(code, "GF@"))
			return INTERNAL_ERROR;
	} else {
		if (add_code(code, "LF@"))
			return INTERNAL_ERROR;
	}

	if (add_code(code, dest))
		return INTERNAL_ERROR;

	if (add_code(code, " float\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_len(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "# Built-in function len - returns lenght of string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "STRLEN LF@$retval LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_ord(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "# Built-in function ord - returns ordinal value of char in s string on i index\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $ord\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// kontrola i -> musi byt v rozmedzi retazca inak return None
	// zikasnie dlzky retazca
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR TF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE TF@%1 LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CALL $len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@len TF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@cond_val\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// len < 0
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@cond_val LF@len int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord_return_none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// i > len-1
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "SUB LF@len LF@len int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@cond_val LF@%2 LF@len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $ord_return_none LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "STRI2INT LF@%retval LF@%1 LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %ord_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $ord_return_none\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@%retval nil@nil\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP $ord_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;


	return OK;
}

int gen_chr(){
// i mimo 0:255 vedie na behovu chybu pri praci so stringom
// INT2CHAR ju generuje sam nie je potrebna kontrola
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "# Built-in function chr - returns char whose ASCII value is i\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $chr\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "INT2CHAR LF@%retval LF@%1 LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}

int gen_substr(){
	Code *code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "# Built-in function substr - returns substring of string\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "PUSHFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@%retval string@\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	// -----vytvoreny prazdny string v retval
	
	// kontroly dlzky substr a dlzky str
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@len_str\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CREATEFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR TF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE TF@%1 LF@%1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CALL $len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// ziskanie retval -> dlzky stringu s
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@len_str LF@%retval\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@cond_val\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// len(s) < 0 -> prazdny substr  ?????? mozno hlasit chybu !!!!!
	/*
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@cond_val LF@len_str int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	*/

	// len(s) == 0 -> prazdny substr
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EQ LF@cond_val LF@len_str int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// dlzka subst - n < 0 -> prazdny substr
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@cond_val LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// dlzka subst - n == 0 -> prazdny substr
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "EQ LF@cond_val LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// index zac subsrt i > len(s)
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@ret_cond LF@%2 LF@len_str\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// i < 0
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LT LF@ret_cond LF@%2 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_end LF@cond_val bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	// ------ kontroly vstupov hotove

	// ak je n > len(s) -> n = len(s) - i -> id indexu po koniec stringu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@max_len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@max_len LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "SUB LF@max_len LF@max_len LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@max_cond\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@max_cond LF@%3 LF@max_n\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_setmax LF@max_cond bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMP %substr_concating\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr_setmax\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@%3 LF@max_len\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL %substr_concating\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@idx\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "MOVE LF@idx LF@%2\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// premenne pre cyklus
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@loop_cond\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "DEFVAR LF@char\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// zaciatok cyklu pridavania pismen
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr_loop\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GETCHAR LF@char LF@%1 LF@idx\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "CONCAT LF@%retval LF@%retval LF@char\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "ADD LF@idx LF@idx int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "SUB LF@%3 LF@%3 int@1\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// ak je n uz nula koniec cyklu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "GT LF@loop_cond LF@%3 int@0\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	// skok na zac cyklu
	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "JUMPIFEQ $substr_loop LF@loop_cond bool@true\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "LABEL $substr_end\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "POPFRAME\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;

	code = create_code();
	if (!code)
		return INTERNAL_ERROR;
	if (add_code(code, "RETURN\0"))
		return INTERNAL_ERROR;
	if (CL_add_line(code))
		return INTERNAL_ERROR;
	return OK;
}