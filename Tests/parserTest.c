#include "../parser.h"
#include "../scanner.h"

#define GET_NEXT_TOKEN(token) if (get_next_token(token, 0) == LEX_ERROR) return LEX_ERROR

int main(int argc, char const *argv[])
{
    Token token;
    //int i = 0;
    GET_NEXT_TOKEN(&token);
    // while(token.type != TK_EOF) {
    // 	printf("token %d: type: %d attribute: %s \n", i, token.type, token.attribute);
    // 	if (get_next_token(&token, 0) == LEX_ERROR){
    // 		printf("%d\n", token.type);
    // 		return LEX_ERROR;
    // 	}
    // 	i++;
    // }
    int returnType = prog(&token);
    printf("Return code: %d\n",  returnType);

    return 0;
}
