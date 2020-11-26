// links para funcoes que vamos utilizar
// http://www.cplusplus.com/reference/cstring/strncpy/
// https://en.wikibooks.org/wiki/C_Programming/C_Reference/nonstandard/strcasecmp

//**********************************************************
// Código feito por: Leonardo Zoccal Longato
//**********************************************************

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // isdigit, isalpha
#include <string.h>

char *buffer; // buffer lido
char *iniBuffer; // guarda o inicio do buffer para desalocar
int contaLinha=1; // faz a contagem de linha do programa

// constantes para os atomos do mini analisador léxico
typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUMERO_INTEIRO,
    NUMERO_REAL,
    ATRIBUICAO,
    ABRE_PAR,
    FECHA_PAR,
    PONTO,
    PONTO_VIRGULA,
    VIRGULA,
    PALAVRA_RESERVADA,
    CARACTERE,
    SUBTRACAO,
	ADICAO,
	DIVISAO,
	MULTIPLICACAO,
	ME,
	MEI,
	IG,
	DI,
	MA,
	MAI,
	COMENTARIO,
    EOS,
    PROGRAM,
    AND,
	BEGIN,
	BOOLEAN,
	CHAR,
	DO,
	ELSE,
	END,
	FALSE,
	IF,
	INTEGER,
	MOD,
	NOT,
	OR,
	READ,
	REAL,
	THEN,
	TRUE,
	WHILE,
	WRITE
}TAtomo;

// vetor de mensagems para o analisador lexico
char *strAtomo[] = {
    "ERRO LEXICO",
    "IDENTIFICADOR",
    "NUMERO_INTEIRO",
    "NUMERO_REAL",
    "ATRIBUICAO",
    "ABRE_PAR",
    "FECHA_PAR",
    "PONTO",
    "PONTO_VIRGULA",
    "VIRGULA",
    "PALAVRA_RESERVADA",
    "CARACTERE",
    "SUBTRACAO",
	"ADICAO",
	"DIVISAO",
	"MULTIPLICACAO",
	"ME",
	"MEI",
	"IG",
	"DI",
	"MA",
	"MAI",
	"COMENTARIO",
    "END OF STRING"
};

// vetor de mensagems para o analisador lexico
char *palavrasReservadas[] = {
	"AND",
	"BEGIN",
	"BOOLEAN",
	"CHAR",
	"DO",
	"ELSE",
	"END",
	"FALSE",
	"IF",
	"INTEGER",
	"MOD",
	"NOT",
	"OR",
	"PROGRAM",
	"READ",
	"REAL",
	"THEN",
	"TRUE",
	"WHILE",
	"WRITE"
};

// estrutura para retornar as informações de um atomo (tokens)
typedef struct{
    TAtomo atomo;
    int linha;
    int atributo_numero;
    float atributo_num_real;
    char atributo_ID[15];
}TInfoAtomo;

TInfoAtomo obter_atomo();
TInfoAtomo info_atomo;
TAtomo lookahead;
void reconhece_num(TInfoAtomo *infoAtomo);
void reconhece_ID(TInfoAtomo *infoAtomo);
void consome(char atomo);
void programa();
void bloco();
void declaracao_de_variaveis();
void variaveis();
void comando();
void comando_composto();
void comando_atribuicao();
void comando_condicional();
void comando_enquanto();
void comando_entrada();
void comando_saida();
void expressao();
void expressao_simples();
void termo();
void fator();

int main(void){
    FILE *fp_entrada;
    TInfoAtomo infoAtomo;

    // abre o arquivo
    fp_entrada = fopen("teste.pas", "r");
    if(!fp_entrada){ // testa se abriu corretamente
        printf("erro na abertura do arquivo de entrada.\n");
        return 1;
    }
    // abertura e leitura de arquivo
    fseek(fp_entrada, 0, SEEK_END); // move ponteiro do arquivo para o final
    long tamanho = ftell(fp_entrada); // conta a quantidade de bytes deslocados na movimentacao
    fseek(fp_entrada, 0, SEEK_SET); // move novamente para o inicio do arquivo
    buffer = (char*)calloc(tamanho+1,sizeof(char)); // aloca os memoria para guardar o arquivo lido
    fread(buffer, sizeof(char),tamanho,fp_entrada); // le de uma vez so o arquivo
    fclose(fp_entrada); // fecha arquivo
	
    printf("--analisando-- \n\n%s\n\n",buffer);
    TInfoAtomo info_atomo = obter_atomo();
	lookahead = info_atomo.atomo;
	while(lookahead == COMENTARIO){
	    TInfoAtomo info_atomo = obter_atomo();
		lookahead = info_atomo.atomo;		
	}
    programa();
    if( lookahead == ERRO)// fim de buffer
        printf("\nerro sintatico.");
    else
        printf("palavra aceita.");

    printf("\nfim de programa.\n");
    return 0;
}

void consome(char atomo){
	if(lookahead == atomo){
		TInfoAtomo info_atomo = obter_atomo();
		lookahead = info_atomo.atomo;
		while(lookahead == COMENTARIO){
		    TInfoAtomo info_atomo = obter_atomo();
			lookahead = info_atomo.atomo;		
		}
	}
	else{
		lookahead = ERRO;
		printf("\nerro sintatico: esperado [%c] encontrado [%c]\n",atomo,lookahead);
		printf("Erro na linha: %d", contaLinha, info_atomo);
		exit(1);
	}
}

void programa(){
	consome(PROGRAM);
	consome(IDENTIFICADOR);
	consome(PONTO_VIRGULA);
	bloco();
	consome(PONTO);
}

void bloco(){
	declaracao_de_variaveis();
	comando_composto();
}

void declaracao_de_variaveis(){
	while(lookahead == INTEGER || lookahead == REAL || lookahead == CHAR || lookahead == BOOLEAN){
		consome(lookahead);
		variaveis();
		consome(PONTO_VIRGULA);
	}
}

void variaveis(){
	consome(IDENTIFICADOR);
	while(lookahead == VIRGULA){
		consome(VIRGULA);
		consome(IDENTIFICADOR);
	}
}

void comando_composto(){
	consome(BEGIN);
	comando();
	while(lookahead == PONTO_VIRGULA){
		consome(PONTO_VIRGULA);
		comando();
	}
	consome(END);
}

void comando(){
	if (lookahead == IDENTIFICADOR)
		comando_atribuicao();
	else if(lookahead == IF)
		comando_condicional();
	else if(lookahead == WHILE)
		comando_enquanto();
	else if(lookahead == READ)
		comando_entrada();
	else if(lookahead == WRITE)
		comando_saida();
	else if(lookahead == BEGIN)
		comando_composto();
}

void comando_atribuicao(){
	consome(IDENTIFICADOR);
	consome(ATRIBUICAO);
	expressao();
}

void comando_condicional(){
	consome(IF);
	consome(ABRE_PAR);
	expressao();
	consome(FECHA_PAR);
	consome(THEN);
	comando();
	if (lookahead == ELSE){
		consome(ELSE);
		comando();
	}
}

void comando_enquanto(){
	consome(WHILE);
	consome(ABRE_PAR);
	expressao();
	consome(FECHA_PAR);
	consome(DO);
	comando();
}

void comando_entrada(){
	consome(READ);
	consome(ABRE_PAR);
	variaveis();
	consome(FECHA_PAR);
}

void comando_saida(){
	consome(WRITE);
	consome(ABRE_PAR);
	expressao();
	while(lookahead == PONTO_VIRGULA){
		consome(PONTO_VIRGULA);
		expressao();
	}
	consome(FECHA_PAR);
}

void expressao(){
	expressao_simples();
	if (lookahead == ME || lookahead == MEI || lookahead == DI || lookahead == MA || lookahead == MAI){
		consome(lookahead);
		expressao_simples();
	}
}

void expressao_simples(){
	if (lookahead == ADICAO || lookahead == SUBTRACAO)
		consome(lookahead);
	termo();
	while (lookahead == ADICAO || lookahead == SUBTRACAO || lookahead == OR){
		consome(lookahead);
		termo();
	}
}

void termo(){
	fator();
	while(lookahead == MULTIPLICACAO || lookahead == DIVISAO || lookahead == MOD || lookahead == AND){
		consome(lookahead);
		fator();
	}
}

void fator(){
	if (lookahead == IDENTIFICADOR || lookahead == NUMERO_INTEIRO || lookahead == NUMERO_REAL || lookahead == CARACTERE || lookahead == TRUE || lookahead == FALSE){
		consome(lookahead);
	}
	else if(lookahead == NOT){
		consome(NOT);
		fator();
	}
	else{	
		consome(ABRE_PAR);
		expressao();
		consome(FECHA_PAR);
	}
}

TInfoAtomo obter_atomo(){
    TInfoAtomo infoAtomo;

    // descarta carateres delimitadores
    while( *buffer == '\n' || *buffer == '\r' || *buffer == '\t' || *buffer == ' '){
        if( *buffer == '\n' )
            contaLinha++;
        buffer++;
    }

    infoAtomo.atomo = ERRO;
    infoAtomo.linha = contaLinha;
    // if(buffer[0] ==':' && buffer[1] =='=')
    if(*buffer ==':' && *(buffer+1) =='='){ // reconhece atribuicao
        buffer+=2; // incrementa o buffer duas posicoes
        infoAtomo.atomo = ATRIBUICAO;
    }
    else if(*buffer == '#'){ //verifica comentario com #
    	while(*buffer != '\n'){
    		buffer++;
		}
        contaLinha++;
        buffer++;
        infoAtomo.atomo = COMENTARIO;
	}
	else if(*buffer == '{'){ //Verifica o comentario com { }
		infoAtomo.atomo = COMENTARIO;
		while (*buffer != '}' && *buffer != '\0'){
			if( *buffer == '\n' ){	// se o comentario foi em mais de uma linha, ele printa a linha atual e pula pra proxima
//				printf("\nlinha %d | %s ", infoAtomo.linha,strAtomo[infoAtomo.atomo]);			
				contaLinha++;
				infoAtomo.linha = contaLinha; //atualiza a linha do atomo para quando a funcao retornar o atomo, o valor da linha ser onde o comentario termina
			}            	
			buffer++;
		}
		if (*buffer == '}'){
			buffer++;		
			infoAtomo.atomo = COMENTARIO;			
		}
	}
	else if(*buffer == '\'' && *(buffer+2) == '\''){ // reconhece caractere
		infoAtomo.atributo_ID[0] = *(buffer+1);
		buffer+=3;
        infoAtomo.atomo = CARACTERE;
    }
    else if(*buffer =='('){ // reconhece abre parenteses
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = ABRE_PAR;
    }
    else if(*buffer ==')'){ // reconhece fecha parenteses
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = FECHA_PAR;
    }
    else if(*buffer =='.'){ // reconhece ponto
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = PONTO;
    }
    else if(*buffer ==';'){ // reconhece ponto e virgula
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = PONTO_VIRGULA;
    }
    else if(*buffer ==','){ // reconhece ponto e virgula
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = VIRGULA;
    }    
    else if(*buffer =='-'){ // reconhece subtracao
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = SUBTRACAO;
    }
    else if(*buffer =='+'){ // reconhece adicao
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = ADICAO;
    }
    else if(*buffer =='/'){ // reconhece divisao
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = DIVISAO;
    }
    else if(*buffer =='<' && *(buffer+1) =='='){ // reconhece menor igual
        buffer+=2; // incrementa o buffer duas posicoes
        infoAtomo.atomo = MEI;
    }
	else if(*buffer =='>' && *(buffer+1) =='='){ // reconhece maior igual
        buffer+=2; // incrementa o buffer duas posicoes
        infoAtomo.atomo = MAI;
	}
    else if(*buffer =='!' && *(buffer+1) =='='){ // reconhece diferente
        buffer+=2; // incrementa o buffer duas posicoes
        infoAtomo.atomo = DI;
    }
    else if(*buffer =='<'){ // reconhece menor
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = ME;
    }
    else if(*buffer =='='){ // reconhece igual
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = IG;
    }
    else if(*buffer =='>'){ // reconhece maior
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = MA;
	}
	else if(*buffer =='*'){ // reconhece multiplicacao
        buffer++; // incrementa o buffer uma posicao
        infoAtomo.atomo = MULTIPLICACAO;
	}
    else if(isdigit(*buffer)){ // reconhece numero inteiro
        reconhece_num(&infoAtomo);
    }
    else if( isalpha(*buffer)){ // reconhece identificador
        reconhece_ID(&infoAtomo);
    }
    else if(*buffer == 0) // reconhece fim de string
        infoAtomo.atomo = EOS;

    return infoAtomo;

}

void reconhece_num(TInfoAtomo *infoAtomo){
    char *iniNum = buffer; int isFloat = 0;

    while( isdigit(*buffer) || *buffer == '.' || *buffer == 'e' || *buffer == 'E' || *buffer == '-' || *buffer == '+'){
    	if(*buffer == '.' && (*(buffer+1) == 'e' || *(buffer+1) == 'E') ){ //reconhece a notacao de numero real, ex: NUMERO.e
    		if (isdigit(*(buffer+2))) //se a notacao for: NUMERO.eNUMERO
    			isFloat = 1;
    		else if( *(buffer+2) == '+' && isdigit(*(buffer+3)) ) //se a notacao for: NUMERO.e+NUMERO
    			isFloat = 1;
    		else if( *(buffer+2) == '-' && isdigit(*(buffer+3)) ) //se a notacao for: NUMERO.e-NUMERO
    			isFloat = 1;    
			else //caso a notacao estiver errada
				return;
		}
		else if( *buffer == '.' && (*(buffer+1) != 'e' || *(buffer+1) != 'E') ){ //se o numero real estiver escrito da maneira errada
			return;
		}
    	buffer++;	
	}        

    if( isalpha(*buffer))
        return;
	
    strncpy(infoAtomo->atributo_ID,iniNum,buffer-iniNum);
    infoAtomo->atributo_ID[buffer-iniNum]=0; // finalizador de string
    if(isFloat == 1){
    	infoAtomo->atributo_num_real = atof(infoAtomo->atributo_ID);
    	infoAtomo->atomo = NUMERO_REAL;
	}
	else{
	    infoAtomo->atributo_numero = atoi(infoAtomo->atributo_ID);
    	infoAtomo->atomo = NUMERO_INTEIRO;	
	}

}

void reconhece_ID(TInfoAtomo *infoAtomo){
    char *iniID = buffer;

    while(isalpha(*buffer) || isdigit(*buffer) || *buffer == '_')
        buffer++;

    strncpy(infoAtomo->atributo_ID,iniID,buffer-iniID);
    infoAtomo->atributo_ID[buffer-iniID]=0; // finalizador de string
    int pRes;
    int palavrasReservadasLength = sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]);
    for (pRes = 0; pRes < palavrasReservadasLength; pRes++){
    	if (strlen(infoAtomo->atributo_ID) > 16){
    		infoAtomo->atomo = ERRO;
       		return;
		}
		if( strcasecmp(infoAtomo->atributo_ID,palavrasReservadas[pRes])==0 ){
			if (palavrasReservadas[pRes] == "PROGRAM")
				infoAtomo->atomo = PROGRAM;					
			else if(palavrasReservadas[pRes] == "AND")
				infoAtomo->atomo = AND;	
			else if(palavrasReservadas[pRes] == "BEGIN")
				infoAtomo->atomo = BEGIN;	
			else if(palavrasReservadas[pRes] == "BOOLEAN")
				infoAtomo->atomo = BOOLEAN;	
			else if(palavrasReservadas[pRes] == "CHAR")
				infoAtomo->atomo = CHAR;	
			else if(palavrasReservadas[pRes] == "DO")
				infoAtomo->atomo = DO;	
			else if(palavrasReservadas[pRes] == "ELSE")
				infoAtomo->atomo = ELSE;	
			else if(palavrasReservadas[pRes] == "END")
				infoAtomo->atomo = END;	
			else if(palavrasReservadas[pRes] == "FALSE")
				infoAtomo->atomo = FALSE;	
			else if(palavrasReservadas[pRes] == "IF")
				infoAtomo->atomo = IF;	
			else if(palavrasReservadas[pRes] == "INTEGER")
				infoAtomo->atomo = INTEGER;	
			else if(palavrasReservadas[pRes] == "MOD")
				infoAtomo->atomo = MOD;	
			else if(palavrasReservadas[pRes] == "NOT")
				infoAtomo->atomo = NOT;	
			else if(palavrasReservadas[pRes] == "OR")
				infoAtomo->atomo = OR;	
			else if(palavrasReservadas[pRes] == "READ")
				infoAtomo->atomo = READ;	
			else if(palavrasReservadas[pRes] == "REAL")
				infoAtomo->atomo = REAL;	
			else if(palavrasReservadas[pRes] == "THEN")
				infoAtomo->atomo = THEN;	
			else if(palavrasReservadas[pRes] == "TRUE")
				infoAtomo->atomo = TRUE;	
			else if(palavrasReservadas[pRes] == "WHILE")
				infoAtomo->atomo = WHILE;	
			else if(palavrasReservadas[pRes] == "WRITE")
				infoAtomo->atomo = WRITE;	

       		return;
		}
	}

    infoAtomo->atomo = IDENTIFICADOR;

}
