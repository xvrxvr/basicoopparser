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
 ParamList* pl_val;
}

%type<n_val> vNUM 
%type<s_val> vID
%type<e_val> expr
%type<pl_val> param_list params

%pure-parser

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
	| vID '(' param_list ')' {$$=new FuncOp($1,$3);}
	;

param_list: {$$=NULL;}
    | params
	;

params: expr {$$=new ParamList($1);}
    | params ',' expr {$$=$1->join(new ParamList($3));}
	;
