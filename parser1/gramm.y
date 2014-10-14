%token vNUM
%token vID

%token tOR "||"
%token tAND "&&"
%token tEQ "=="
%token tNE "!="

%left tEQ tNE
%left tAND
%left tOR
%left '+' '-'
%left '*' '/' '%'

%union {
 double n_val;
 char* s_val;
 ASTNode* e_val;
}

%type<n_val> vNUM 
%type<s_val> vID
%type<e_val> expr

%pure_parser

%%

root: expr {result=$1;} 
    | vID '=' expr {set_var_value($1,result=$3);}
	;


expr: vNUM           {$$=new NodeConst($1);}
	| vID            {$$=new NodeVar($1);}
    | '(' expr ')'   {$$=$2;}
	| expr '+' expr  {$$=new NodeBinOp(OPC_Add,$1,$3);}
	| expr '-' expr  {$$=new NodeBinOp(OPC_Sub,$1,$3);}
	| expr '*' expr  {$$=new NodeBinOp(OPC_Mul,$1,$3);}
	| expr '/' expr  {$$=new NodeBinOp(OPC_Div,$1,$3);}
	| expr "||" expr {$$=new NodeBinOp(OPC_LOr,$1,$3);}
	| expr "&&" expr {$$=new NodeBinOp(OPC_LAnd,$1,$3);}
	| expr "==" expr {$$=new NodeBinOp(OPC_Eq,$1,$3);}
	| expr "!=" expr {$$=new NodeBinOp(OPC_Ne,$1,$3);}
	;
