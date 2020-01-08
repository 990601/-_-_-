#include "def.h"
#define ASTNode node
int LEV=0;
void itoa(int i, char *string)
{
    int power = 0, j = 0;
    j = i;
    if(i<10) power=1;
    if(i>=10 && i<100) power=10;
    if(i>=100 && i<=1000) power=100;
    for (; power>0; power /= 10)
    {  
	*string++ = '0' + i / power;
	i %= power;
    }
    *string = '\0';
}

char *getname(char *s1,char *s2){
    static char result[10];
    strcpy(result,s1);
    strcat(result,s2);
    return result;
}

char *newAlias() {
    static int no=1;
    char s[10];
    itoa(no++,s);
    return getname("v",s);
}

char *newLabel() {
    static int no=1;
    char s[10];
    itoa(no++,s);
    return getname("label",s);
}

char *newTemp(){
    static int no=1;
    char s[10];
    itoa(no++,s);
    return getname("temp",s);
}

//生成一条TAC代码的结点组成的双向循环链表，返回头指针
struct codenode *genIR(int op,struct opn opn1,struct opn opn2,struct opn result){
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=op;
    h->opn1=opn1;
    h->opn2=opn2;
    h->result=result;
    h->next=h->prior=h;
    return h;
}

//生成一条标号语句，返回头指针
struct codenode *genLabel(char *label){
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=LABEL;
    strcpy(h->result.id,label);
    h->next=h->prior=h;
    return h;
}

//生成GOTO语句，返回头指针
struct codenode *genGoto(char *label){
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=GOTO;
    strcpy(h->result.id,label);
    h->next=h->prior=h;
    return h;
}

//合并多个中间代码的双向循环链表，首尾相连
struct codenode *merge(int num,...){
    struct codenode *h1,*h2,*p,*t1,*t2;
    va_list ap;
    va_start(ap,num);
    h1=va_arg(ap,struct codenode *);
    while (--num>0) {
        h2=va_arg(ap,struct codenode *);
        if (h1==NULL) h1=h2;
        else if (h2){
            t1=h1->prior;
            t2=h2->prior;
            t1->next=h2;
            t2->next=h1;
            h1->prior=t2;
            h2->prior=t1;
            }
        }
    va_end(ap);
    return h1;
}

//输出中间代码
void prnIR(struct codenode *head){
    char opnstr1[32],opnstr2[32],resultstr[32];
    struct codenode *h=head;
    do {
        if (h->opn1.kind==INT)
             sprintf(opnstr1,"#%d",h->opn1.const_int);
        if (h->opn1.kind==FLOAT)
             sprintf(opnstr1,"#%f",h->opn1.const_float);
        if (h->opn1.kind==CHAR)
             sprintf(opnstr1,"#%f",h->opn1.const_char); 
        if (h->opn1.kind==ID)
             sprintf(opnstr1,"%s",h->opn1.id);
        if (h->opn2.kind==INT)
             sprintf(opnstr2,"#%d",h->opn2.const_int);
        if (h->opn2.kind==FLOAT)
             sprintf(opnstr2,"#%f",h->opn2.const_float);
         if (h->opn2.kind==CHAR)
             sprintf(opnstr2,"#%f",h->opn2.const_char);
        if (h->opn2.kind==ID)
             sprintf(opnstr2,"%s",h->opn2.id);
        sprintf(resultstr,"%s",h->result.id);
        switch (h->op) {
            case UMINUS:    printf("  %s := -%s\n",resultstr,opnstr1);
                            break;
            case ASSIGNOP:  printf("  %s := %s\n",resultstr,opnstr1);
                            break;
            case PLUS:
            case MINUS:
            case STAR:
            case DIV: printf("  %s := %s %c %s\n",resultstr,opnstr1, \
                      h->op==PLUS?'+':h->op==MINUS?'-':h->op==STAR?'*':'\\',opnstr2);
                      break;
            case FUNCTION: printf("\nFUNCTION %s :\n",h->result.id);
                           break;
            case PARAM:    printf("  PARAM %s\n",h->result.id);
                           break;
            case LABEL:    printf("LABEL %s :\n",h->result.id);
                           break;
            case GOTO:     printf("  GOTO %s\n",h->result.id);
                           break;
            case JLE:      printf("  IF %s <= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case JLT:      printf("  IF %s < %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case JGE:      printf("  IF %s >= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case JGT:      printf("  IF %s > %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case EQ:       printf("  IF %s == %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case NEQ:      printf("  IF %s != %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                           break;
            case ARG:      printf("  ARG %s\n",h->result.id);
                           break;
            case CALL:     printf("  %s := CALL %s\n",resultstr, opnstr1);
                           break;
            case RETURN:   if (h->result.kind)
                                printf("  RETURN %s\n",resultstr);
                           else
                                printf("  RETURN\n");
                           break;
        }
    h=h->next;
    } while (h!=head);
}
void semantic_error(int number,int line,char *msg1,char *msg2){
    printf("error %d : in line %d ---- %s %s\n",number,line,msg1,msg2);
}

void prn_symbol(){
    int i=0;
    static int countprn=1;
    printf("\n\n\nthe %d time print table:\n\n",countprn++);
    printf("%6s   %6s    %6s  %6s  %4s  %6s\n","变量名","别 名","层 号","类  型","标记","偏移量");
    for(i=0;i<symbolTable.index;i++)
        printf("%6s  %6s  %6d   %6s  %4c  %6d\n",symbolTable.symbols[i].name,\
                symbolTable.symbols[i].alias,symbolTable.symbols[i].level,\
                symbolTable.symbols[i].type==INT?"int":(symbolTable.symbols[i].type==FLOAT?"float":"char"),\
                symbolTable.symbols[i].flag,symbolTable.symbols[i].offset);
    printf("\n");
}

int searchSymbolTable(char *name) {
    int i;
    for(i=symbolTable.index-1;i>=0;i--)
        if (!strcmp(symbolTable.symbols[i].name, name))  return i;
    return -1;
}

int fillSymbolTable(char *name,char *alias,int level,int type,char flag,int offset) {
    int i;
    for(i=symbolTable.index-1; i>=0 && (symbolTable.symbols[i].level==level||level==0); i--) {
        if (level==0 && symbolTable.symbols[i].level==1) continue;
        if (!strcmp(symbolTable.symbols[i].name, name))  return -1;
    }

    strcpy(symbolTable.symbols[symbolTable.index].name,name);
    strcpy(symbolTable.symbols[symbolTable.index].alias,alias);
    symbolTable.symbols[symbolTable.index].level=level;
    symbolTable.symbols[symbolTable.index].type=type;
    symbolTable.symbols[symbolTable.index].flag=flag;
    symbolTable.symbols[symbolTable.index].offset=offset;
    return symbolTable.index++;
}

int fill_Temp(char *name,int level,int type,char flag,int offset) {
    strcpy(symbolTable.symbols[symbolTable.index].name,"");
    strcpy(symbolTable.symbols[symbolTable.index].alias,name);
    symbolTable.symbols[symbolTable.index].level=level;
    symbolTable.symbols[symbolTable.index].type=type;
    symbolTable.symbols[symbolTable.index].flag=flag;
    symbolTable.symbols[symbolTable.index].offset=offset;
    return symbolTable.index++;
}

/*处理外部变量列表-----传递信息*/
void ext_var_list(struct ASTNode *T){  
    int rtn,num=1;
    switch (T->kind){
        case EXT_DEC_LIST:
                T->ptr[0]->type=T->type;
                T->ptr[0]->offset=T->offset;
                T->ptr[1]->type=T->type;
                T->ptr[1]->offset=T->offset+T->width;
                T->ptr[1]->width=T->width;
                ext_var_list(T->ptr[0]);
                ext_var_list(T->ptr[1]);
                T->num=T->ptr[1]->num+1;
                break;
        case ID:
            rtn=fillSymbolTable(T->type_id,newAlias(),LEV,T->type,'V',T->offset);
            if (rtn==-1)
                semantic_error(18,T->pos,T->type_id, "变量重复定义");
            else T->place=rtn;
            T->num=1;
            break;
    }
}

/*函数参数检查*/
int  match_param(int posT,int i,struct ASTNode *T){
    int j,num=symbolTable.symbols[i].paramnum;
    int type1,type2;
    if (num==0 && T==NULL) return 1;
    for (j=1;j<=num;j++) {
        if (!T){
            semantic_error(1,posT,"", "函数参数数量不匹配");
            return 0;
        }
        type1=symbolTable.symbols[i+j].type;
        type2=T->ptr[0]->type;
        if (type1!=type2){
            semantic_error(2,T->pos,"", "函数参数类型不匹配");
            return 0;
        }
        T=T->ptr[1];
    }
    if (T){
        semantic_error(1,T->pos,"", "函数参数数量不匹配");
        return 0;
    }
    return 1;
}

//处理判断，循环
void boolExp(struct ASTNode *T){  
  struct opn opn1,opn2,result;
  int op;
  int rtn;
  if (T)
  {
	switch (T->kind) {
        case INT:  if (T->type_int!=0)
                        T->code=genGoto(T->Etrue);
                   else T->code=genGoto(T->Efalse);
                   T->width=0;
                   break;
        case FLOAT:  if (T->type_float!=0.0)
                        T->code=genGoto(T->Etrue);
                   else T->code=genGoto(T->Efalse);
                   T->width=0;
                   break;
        case CHAR:  if (T->type_char!='0')
                        T->code=genGoto(T->Etrue);
                   else T->code=genGoto(T->Efalse);
                   T->width=0;
                   break;
        case ID:    //查符号表，获得符号表中的位置，类型送type
                   rtn=searchSymbolTable(T->type_id);
                   if (rtn==-1)
                        semantic_error(6,T->pos,T->type_id, "引用了未定义变量");
                   if (symbolTable.symbols[rtn].flag=='F')
                        semantic_error(7,T->pos,T->type_id, "错误的引用了变量");
                   else {
                        opn1.kind=ID; strcpy(opn1.id,symbolTable.symbols[rtn].alias);
                        opn1.offset=symbolTable.symbols[rtn].offset;
                        opn2.kind=INT; opn2.const_int=0;
                        result.kind=ID; strcpy(result.id,T->Etrue);
                        T->code=genIR(NEQ,opn1,opn2,result);
                        T->code=merge(2,T->code,genGoto(T->Efalse));
                        }
                   T->width=0;
                   break;
        case RELOP: //处理关系运算表达式,2个操作数都按基本表达式处理
                    T->ptr[0]->offset=T->ptr[1]->offset=T->offset;
                    Exp(T->ptr[0]);
                    T->width=T->ptr[0]->width;
                    Exp(T->ptr[1]);
                    if (T->width<T->ptr[1]->width) T->width=T->ptr[1]->width;
                    opn1.kind=ID; strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                    opn2.kind=ID; strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                    opn2.offset=symbolTable.symbols[T->ptr[1]->place].offset;
                    result.kind=ID; strcpy(result.id,T->Etrue);
                    if (strcmp(T->type_id,"<")==0)
                            op=JLT;
                    else if (strcmp(T->type_id,"<=")==0)
                            op=JLE;
                    else if (strcmp(T->type_id,">")==0)
                            op=JGT;
                    else if (strcmp(T->type_id,">=")==0)
                            op=JGE;
                    else if (strcmp(T->type_id,"==")==0)
                            op=EQ;
                    else if (strcmp(T->type_id,"!=")==0)
                            op=NEQ;
                    T->code=genIR(op,opn1,opn2,result);
                    T->code=merge(4,T->ptr[0]->code,T->ptr[1]->code,T->code,genGoto(T->Efalse));
                    break;
        case AND:
        case OR:
                    if (T->kind==AND) {
                        strcpy(T->ptr[0]->Etrue,newLabel());
                        strcpy(T->ptr[0]->Efalse,T->Efalse);
                        }
                    else {
                        strcpy(T->ptr[0]->Etrue,T->Etrue);
                        strcpy(T->ptr[0]->Efalse,newLabel());
                        }
                    strcpy(T->ptr[1]->Etrue,T->Etrue);
                    strcpy(T->ptr[1]->Efalse,T->Efalse);
                    T->ptr[0]->offset=T->ptr[1]->offset=T->offset;
                    boolExp(T->ptr[0]);
                    T->width=T->ptr[0]->width;
                    boolExp(T->ptr[1]);
                    if (T->width<T->ptr[1]->width) T->width=T->ptr[1]->width;
                    if (T->kind==AND)
                        T->code=merge(3,T->ptr[0]->code,genLabel(T->ptr[0]->Etrue),T->ptr[1]->code);
                    else
                        T->code=merge(3,T->ptr[0]->code,genLabel(T->ptr[0]->Efalse),T->ptr[1]->code);
                    break;
        case NOT:   strcpy(T->ptr[0]->Etrue,T->Efalse);
                    strcpy(T->ptr[0]->Efalse,T->Etrue);
                    boolExp(T->ptr[0]);
                    T->code=T->ptr[0]->code;
                    break;
        }
  }
}

//处理基本表达式
void Exp(struct ASTNode *T)
{
  int rtn,num,width;
  struct ASTNode *T0;
  struct opn opn1,opn2,result;
  if (T)
	{
	switch (T->kind) {
	case ID:    //查符号表，获得符号表中的位置，类型送type
                rtn=searchSymbolTable(T->type_id);
                if (rtn==-1)
                    semantic_error(6,T->pos,T->type_id, "使用了未定义变量");
                if (symbolTable.symbols[rtn].flag=='F')
                    semantic_error(7,T->pos,T->type_id, "错误的引用了变量名");
                else {
                    T->place=rtn;       //结点保存变量在符号表中的位置
                    T->code=NULL;       //标识符不需要生成TAC
                    T->type=symbolTable.symbols[rtn].type;
                    T->offset=symbolTable.symbols[rtn].offset;
                    T->width=0;   //未再使用新单元
                    }
                break;
	case INT:
		T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset); //为整常量生成一个临时变量
                T->type=INT;
                opn1.kind=INT;opn1.const_int=T->type_int;
                result.kind=ID; strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.level =symbolTable.symbols[T->place].level;
                result.offset=symbolTable.symbols[T->place].offset;
                T->code=genIR(ASSIGNOP,opn1,opn2,result);
                T->width=4;
                break;
	case FLOAT: 
		T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset);   //为浮点常量生成一个临时变量
                T->type=FLOAT;
                opn1.kind=FLOAT; opn1.const_float=T->type_float;
                result.kind=ID; strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.level =symbolTable.symbols[T->place].level;
                result.offset=symbolTable.symbols[T->place].offset;
                T->code=genIR(ASSIGNOP,opn1,opn2,result);
                T->width=8;
                break;

	case CHAR:
		T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset);   //为浮点常量生成一个临时变量
                T->type=CHAR;
                opn1.kind=CHAR; opn1.const_char=T->type_char;
                result.kind=ID; strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.level =symbolTable.symbols[T->place].level;
                result.offset=symbolTable.symbols[T->place].offset;
                T->code=genIR(ASSIGNOP,opn1,opn2,result);
                T->width=1;
                break;

	case ASSIGNOP:
                if (T->ptr[0]->kind!=ID){
                    semantic_error(8,T->pos,"", "赋值语句左值错误");
                }
                else {
                    Exp(T->ptr[0]);   //处理左值，例中仅为变量
                    T->ptr[1]->offset=T->offset;
                    Exp(T->ptr[1]);
                    T->type=T->ptr[0]->type;
                    rtn=searchSymbolTable(T->ptr[0]->type_id);
                    if(rtn!=-1 &&((T->ptr[0]->type==INT && T->ptr[1]->type==FLOAT)||\
                        (T->ptr[0]->type==CHAR && T->ptr[1]->type==INT)||\
                        (T->ptr[0]->type==CHAR && T->ptr[1]->type==FLOAT))){
                        semantic_error(9,T->pos,"", "赋值类型不匹配");
                    }
                    T->width=T->ptr[1]->width;
                    T->code=merge(2,T->ptr[0]->code,T->ptr[1]->code);
                    opn1.kind=ID;   strcpy(opn1.id,symbolTable.symbols[T->ptr[1]->place].alias);//右值一定是个变量或临时变量
                    opn1.level = symbolTable.symbols[T->ptr[1]->place].level;
                    opn1.offset=symbolTable.symbols[T->ptr[1]->place].offset;
                    result.kind=ID; strcpy(result.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    result.level = symbolTable.symbols[T->ptr[0]->place].level;
                    result.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                    T->code=merge(2,T->code,genIR(ASSIGNOP,opn1,opn2,result));
                    T->place = T->ptr[0]->place;//因为进行过赋值运算，所以要保存该变量的位置信息
                }
                break;
	case AND:
	case OR:
                T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                Exp(T->ptr[1]);
                T->type=INT;
                T->width=T->ptr[0]->width+T->ptr[1]->width+2;
                T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset+T->ptr[0]->width+T->ptr[1]->width);
                opn1.kind=ID; strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type=T->ptr[0]->type;opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                opn2.kind=ID; strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.type=T->ptr[1]->type;opn2.offset=symbolTable.symbols[T->ptr[1]->place].offset;
                result.kind=ID; strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.type=T->type;result.offset=symbolTable.symbols[T->place].offset;
                T->code=merge(3,T->ptr[0]->code,T->ptr[1]->code,genIR(T->kind,opn1,opn2,result));
                T->width=T->ptr[0]->width+T->ptr[1]->width+4;
                break;
	case RELOP:
                T->type=INT;
                T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                Exp(T->ptr[1]);
                T->width=T->ptr[0]->width+T->ptr[1]->width+2;
                T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset+T->ptr[0]->width+T->ptr[1]->width);
                opn1.kind=ID; strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type=T->ptr[0]->type;opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                opn2.kind=ID; strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.type=T->ptr[1]->type;opn2.offset=symbolTable.symbols[T->ptr[1]->place].offset;
                result.kind=ID; strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.type=T->type;result.offset=symbolTable.symbols[T->place].offset;
                T->code=merge(3,T->ptr[0]->code,T->ptr[1]->code,genIR(T->kind,opn1,opn2,result));
                T->width=T->ptr[0]->width+T->ptr[1]->width+4;
                break;
	
	case PLUSEQ:
	case MINUSEQ:
	case STAREQ:
	case DIVEQ: 

                T->ptr[0]->offset=T->offset;
                // 要求第一个子exp是ID，需要做检查
                if (T->ptr[0]->kind!=ID)
                {
                    semantic_error(10,T->pos,T->type_id, "运算语句左值错误");
                }

                int type = (T->kind==PLUSEQ)? PLUS:(T->kind==MINUSEQ)?MINUS:(T->kind==STAREQ)?STAR:DIV;
                // 将当前的节点替换成一个ASSIGNOP节点
                // x=x+exp
                struct node * temp_op = mknode(type,T->ptr[0],T->ptr[1],NULL,T->pos);
                strcpy(temp_op->type_id,(T->kind==PLUSEQ)? "PLUS":(T->kind==MINUSEQ)?"MINUS":(T->kind==STAREQ)?"STAR":"DIV");
                //mark 这一步操作比较复杂,是当前的节点替换成一个ASSIGNOP节点的关键
                T->parent->ptr[T->seq_num]=mknode(ASSIGNOP,T->ptr[0],temp_op,NULL,T->pos);strcpy(T->type_id,"ASSIGNOP");
                T=T->parent->ptr[T->seq_num];
                T->code=NULL;
                Exp(T);
                break;


	case PLUS:
	case MINUS:
	case STAR:
	case DIV:   T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                Exp(T->ptr[1]);
                if (T->ptr[0]->type!=T->ptr[1]->type)
                {
                    semantic_error(11,T->pos,T->type_id, "运算语句类型不匹配");
                }
                if (T->ptr[0]->type==FLOAT || T->ptr[1]->type==FLOAT)
                     T->type=FLOAT,T->width=T->ptr[0]->width+T->ptr[1]->width+4;
                else T->type=INT,T->width=T->ptr[0]->width+T->ptr[1]->width+2;
                T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset+T->ptr[0]->width+T->ptr[1]->width);
                opn1.kind=ID; strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type=T->ptr[0]->type;opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                opn1.level =symbolTable.symbols[T->ptr[0]->place].level;
                opn2.kind=ID; strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.type=T->ptr[1]->type;opn2.offset=symbolTable.symbols[T->ptr[1]->place].offset;
                opn2.level = symbolTable.symbols[T->ptr[1]->place].level;
                result.kind=ID; strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.type=T->type;result.offset=symbolTable.symbols[T->place].offset;
                result.level = symbolTable.symbols[T->place].level;
                T->code=merge(3,T->ptr[0]->code,T->ptr[1]->code,genIR(T->kind,opn1,opn2,result));
                T->width=T->ptr[0]->width+T->ptr[1]->width+(T->type==INT?4:(T->type==FLOAT?8:1));
                break;
	case NOT:
                T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                T->type=INT;
                T->width=T->ptr[0]->width+2;
                break;

	case INC:
	case DEC:	
 		printf("");
		int offset = T->offset;
		if(T->ptr[0]){ //x++
    			T->ptr[0]->offset=T->offset;
    			//printf("现在是x++\n,pos=%d",T->pos);
	                // 要求子exp是ID，需要做检查
	                if (T->ptr[0]->kind!=ID)
	                {
	                    semantic_error(12,T->pos,"", "++--少对象");
	                    break;
	                }
	                Exp(T->ptr[0]); //处理ID语句，从符号表查找变量类型等 //change mark
	                if ((T->ptr[0]->type)!=INT )
	                {   
	                    semantic_error(13,T->pos,"", "++--对象类型错误");
	                    break;
	                }
	                else T->ptr[0]->type=INT,T->width=T->ptr[0]->width+4;
    		}
    		else{ //++x
    			T->ptr[1]->offset=T->offset;
    			//printf("现在是++x\n,pos=%d",T->pos);
	                if (T->ptr[1]->kind!=ID)
	                {
	                    semantic_error(12,T->pos,"", "++--少对象");
	                    break;
	                }
	                Exp(T->ptr[1]); //处理ID语句，从符号表查找变量类型等
	                if ((T->ptr[1]->type)!=INT )
	                {
	                    semantic_error(13,T->pos,"", "++--对象类型错误");
	                    break;
	                }
	                else T->ptr[1]->type=INT,T->width=T->ptr[1]->width+4;
    		}
    			 // 将当前的节点替换成一个ASSIGNOP节点
                struct node * temp_1 = mknode(INT,NULL,NULL,NULL,T->pos);temp_1->type_int=1;T->type=INT;
                struct node * temp_add = mknode((T->kind==INC)? PLUS:MINUS,(T->ptr[0] != NULL)?(T->ptr[0]):(T->ptr[1]),temp_1,NULL,T->pos);
                strcpy(temp_add->type_id,(type==PLUS)?"PLUS":"MINUS");
                //mark 这一步操作比较复杂,是当前的节点替换成一个ASSIGNOP节点的关键
                T->parent->ptr[T->seq_num]=mknode(ASSIGNOP,(T->ptr[0]!=NULL)?(T->ptr[0]):(T->ptr[1]),temp_add,NULL,T->pos);
                strcpy(T->type_id,"ASSIGNOP");
                T=T->parent->ptr[T->seq_num];
                T->offset =offset;
                T->type = INT;
                T->code=NULL;
                Exp(T);
                break;
	case UMINUS:
                T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                if (T->ptr[0]->type==FLOAT )
                     T->type=FLOAT,T->width=T->ptr[0]->width+4;
                else T->type=INT,T->width=T->ptr[0]->width+4;
                T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset+T->ptr[0]->width);
                opn1.kind=ID; 
                strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type=T->ptr[0]->type;
                opn1.level = symbolTable.symbols[T->ptr[0]->place].level;
                opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                result.kind=ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.type=T->type;
                result.level = symbolTable.symbols[T->place].level;
                result.offset=symbolTable.symbols[T->place].offset;
                T->code=merge(2,T->ptr[0]->code,genIR(UMINUS,opn1,opn2,result));
                break;
	case FUNC_CALL: //根据T->type_id查出函数的定义，如果语言中增加了实验教材的read，write需要单独处理一下
                rtn=searchSymbolTable(T->type_id);
                if (rtn==-1){
                    semantic_error(16,T->pos,T->type_id, "函数未定义");
                    break;
                    }
                if (symbolTable.symbols[rtn].flag!='F'){
                    semantic_error(17,T->pos,T->type_id, "不是一个函数");
                     break;
                    }
                T->type=symbolTable.symbols[rtn].type;
                width=T->type==INT?4:(T->type==FLOAT?8:1);   //存放函数返回值的单数字节数
                if (T->ptr[0]){
                    T->ptr[0]->offset=T->offset;
                    Exp(T->ptr[0]);       //处理所有实参表达式求值，及类型
                    T->width=T->ptr[0]->width+width; //累加上计算实参使用临时变量的单元数
                    T->code=T->ptr[0]->code;
                    }
                else {T->width=width; T->code=NULL;}
                match_param(T->pos,rtn,T->ptr[0]);   //处理所以参数的匹配，若T->ptr[0]为空，则需要T的pos
                    //处理参数列表的中间代码
                T0=T->ptr[0];
                while (T0) {
                    result.kind=ID;  strcpy(result.id,symbolTable.symbols[T0->ptr[0]->place].alias);
                    result.offset=symbolTable.symbols[T0->ptr[0]->place].offset;
                    T->code=merge(2,T->code,genIR(ARG,opn1,opn2,result));
                    T0=T0->ptr[1];
                    }
                T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset+T->width-width);
                opn1.kind=ID;     strcpy(opn1.id,T->type_id);  //保存函数名
                opn1.offset=rtn;  //这里offset用以保存函数定义入口,在目标代码生成时，能获取相应信息
                result.kind=ID;   strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.offset=symbolTable.symbols[T->place].offset;
                T->code=merge(2,T->code,genIR(CALL,opn1,opn2,result)); //生成函数调用中间代码
                break;
	case ARGS:      //此处仅处理各实参表达式的求值的代码序列，不生成ARG的实参系列
                T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                T->width=T->ptr[0]->width;
                T->code=T->ptr[0]->code;
                if (T->ptr[1]) {
                    T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                    Exp(T->ptr[1]);
                    T->width+=T->ptr[1]->width;
                    T->code=merge(2,T->code,T->ptr[1]->code);
                    }
                break;
         }
      }
}


void semantic_Analysis(struct ASTNode *T)
{
  int rtn,num,width;
  struct ASTNode *T0;
  struct opn opn1,opn2,result;
  if (T){
	switch (T->kind) {
	case EXT_DEF_LIST://外部声明
            if (!T->ptr[0]) break;
            T->ptr[0]->offset=T->offset;
            semantic_Analysis(T->ptr[0]);
            T->code=T->ptr[0]->code;
            if (T->ptr[1]){
                T->ptr[1]->offset=T->ptr[0]->offset+T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]);
                T->code=merge(2,T->code,T->ptr[1]->code);
            }
            break;
	case EXT_VAR_DEF: //定义全局变量
            T->type=T->ptr[1]->type=!strcmp(T->ptr[0]->type_id,"int")?INT:(!strcmp(T->ptr[0]->type_id,"float")?FLOAT:CHAR);
            T->ptr[1]->offset=T->offset;
            T->ptr[1]->width=T->type==INT?4:(T->type==FLOAT?8:1);
            ext_var_list(T->ptr[1]);
            T->width=(T->type==INT?4:(T->type==FLOAT?8:1))* T->ptr[1]->num;
            T->code=NULL;
            break;
	case FUNC_DEF:      //函数定义
            T->ptr[1]->type=!strcmp(T->ptr[0]->type_id,"int")?INT:(!strcmp(T->ptr[0]->type_id,"float")?FLOAT:CHAR);
            T->width=0;
            T->offset=DX;
            semantic_Analysis(T->ptr[1]);
            T->offset+=T->ptr[1]->width;
            T->ptr[2]->offset=T->offset;
            strcpy(T->ptr[2]->Snext,newLabel());
            semantic_Analysis(T->ptr[2]);
            symbolTable.symbols[T->ptr[1]->place].offset=T->offset+T->ptr[2]->width;
            T->code=merge(3,T->ptr[1]->code,T->ptr[2]->code,genLabel(T->ptr[2]->Snext));
            break;
	case FUNC_DEC: //根据返回类型，函数名填写符号表

            symbol_scope_TX.TY[symbol_scope_TX.top]='F';//进入函数作用域

            rtn=fillSymbolTable(T->type_id,newAlias(),LEV,T->type,'F',0);//函数不在数据区中分配单元，偏移量为0
            if (rtn==-1){
                semantic_error(15,T->pos,T->type_id, "函数重复定义");
                break;
            }
            else T->place=rtn;

            result.kind=ID;   strcpy(result.id,T->type_id);
            result.offset=rtn;	result.level=LEV;
            T->code=genIR(FUNCTION,opn1,opn2,result);     //genIR FUNCTION---------------------------------------
            T->offset=DX;   //设置形式参数在活动记录中的偏移量初值
            if (T->ptr[0]) { //参数处理
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);
                T->width=T->ptr[0]->width;
                symbolTable.symbols[rtn].paramnum=T->ptr[0]->num;
                T->code=merge(2,T->code,T->ptr[0]->code);
            }
            else symbolTable.symbols[rtn].paramnum=0,T->width=0;
            break;
	case PARAM_LIST:    //处理函数形式参数列表
            T->ptr[0]->offset=T->offset;
            semantic_Analysis(T->ptr[0]);
            if (T->ptr[1]){
                T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]);
                T->num=T->ptr[0]->num+T->ptr[1]->num;        //统计参数个数
                T->width=T->ptr[0]->width+T->ptr[1]->width;  //累加参数单元宽度
                T->code=merge(2,T->ptr[0]->code,T->ptr[1]->code);  //连接参数代码
                }
            else {
                T->num=T->ptr[0]->num;
                T->width=T->ptr[0]->width;
                T->code=T->ptr[0]->code;
                }
            break;
	case  PARAM_DEC:
            rtn=fillSymbolTable(T->ptr[1]->type_id,newAlias(),1,T->ptr[0]->type,'P',T->offset);
            if (rtn==-1){
                semantic_error(14,T->ptr[1]->pos,T->ptr[1]->type_id, "参数名重复定义");
            }
            else T->ptr[1]->place=rtn;
            T->num=1;       //参数个数计算的初始值
            T->width=T->ptr[0]->type==INT?4:(T->ptr[0]->type==FLOAT?8:1);  //参数宽度
            result.kind=ID;   strcpy(result.id, symbolTable.symbols[rtn].alias);
            result.offset=T->offset;
            result.level = 0;
            T->code=genIR(PARAM,opn1,opn2,result);     //genIR FUNCTION---------------------------------------
            break;
	case COMP_STM:
            LEV++;
            symbol_scope_TX.TX[symbol_scope_TX.top++]=symbolTable.index;
            T->width=0;
            T->code=NULL;
            if (T->ptr[0]) {//查看变量定义情况
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);  //处理该层的局部变量DEF_LIST
                T->width+=T->ptr[0]->width;
                T->code=T->ptr[0]->code;
            }
            if (T->ptr[1]){//查看语句执行情况
                T->ptr[1]->offset=T->offset+T->width;
                strcpy(T->ptr[1]->Snext,T->Snext);  //S.next属性向下传递
                semantic_Analysis(T->ptr[1]);       //处理复合语句的语句序列
                T->width+=T->ptr[1]->width;
                T->code=merge(2,T->code,T->ptr[1]->code);
             }
             prn_symbol();
             LEV--;
             symbolTable.index=symbol_scope_TX.TX[--symbol_scope_TX.top]; //删除该作用域中的符号
             break;
	case DEF_LIST:
            T->code=NULL;
            if (T->ptr[0]){
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);   //处理一个局部变量定义
                T->code=T->ptr[0]->code;
                T->width=T->ptr[0]->width;
                }
            if (T->ptr[1]) {
                T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]);   //处理剩下的局部变量定义
                T->code=merge(2,T->code,T->ptr[1]->code);
                T->width+=T->ptr[1]->width;
                }
                break;
	case VAR_DEF://处理一个局部变量定义,将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
                T->code=NULL;
                T->ptr[1]->type=!strcmp(T->ptr[0]->type_id,"int")?INT:(!strcmp(T->ptr[0]->type_id,"float")?FLOAT:CHAR);  //确定变量序列各变量类型
                T0=T->ptr[1]; //T0为变量名列表子树根指针，对ID、ASSIGNOP类结点在登记到符号表，作为局部变量
                num=0;
                T0->offset=T->offset;
                T->width=0;
                width=T->ptr[1]->type==INT?4:(T->ptr[1]->type==FLOAT?8:1);  //一个变量宽度
                while (T0) {  //处理所以DEC_LIST结点
                    num++;
                    T0->ptr[0]->type=T0->type;  //类型属性向下传递
                    if (T0->ptr[1]) T0->ptr[1]->type=T0->type;
                    T0->ptr[0]->offset=T0->offset;  //类型属性向下传递
                    if (T0->ptr[1]) T0->ptr[1]->offset=T0->offset+width;
                    if (T0->ptr[0]->kind==ID){
                        rtn=fillSymbolTable(T0->ptr[0]->type_id,newAlias(),LEV,T0->ptr[0]->type,'V',T->offset+T->width);
                        if (rtn==-1)
                            semantic_error(18,T0->ptr[0]->pos,T0->ptr[0]->type_id, "变量重复定义");
                        else T0->ptr[0]->place=rtn;
                        T->width+=width;
                        }
                    else if (T0->ptr[0]->kind==ASSIGNOP){
                            rtn=fillSymbolTable(T0->ptr[0]->ptr[0]->type_id,newAlias(),LEV,T0->ptr[0]->type,'V',T->offset+T->width);
                            if (rtn==-1)
                                semantic_error(18,T0->ptr[0]->ptr[0]->pos,T0->ptr[0]->ptr[0]->type_id, "变量重复定义");
                            else {
                                T0->ptr[0]->place=rtn;
                                T0->ptr[0]->ptr[1]->offset=T->offset+T->width+width;
                                Exp(T0->ptr[0]->ptr[1]);
                                opn1.kind=ID; strcpy(opn1.id,symbolTable.symbols[T0->ptr[0]->ptr[1]->place].alias);
                                opn1.offset = symbolTable.symbols[T0->ptr[0]->ptr[1]->place].offset; 
                                opn1.level =symbolTable.symbols[T0->ptr[0]->ptr[1]->place].level;
                                result.kind=ID; strcpy(result.id,symbolTable.symbols[T0->ptr[0]->place].alias);
                                result.offset = symbolTable.symbols[T0->ptr[0]->ptr[0]->place].offset;
                                result.level = symbolTable.symbols[T0->ptr[0]->place].level;
                                T->code=merge(3,T->code,T0->ptr[0]->ptr[1]->code,genIR(ASSIGNOP,opn1,opn2,result));//genIR ASSIGNOP------------------------------------------------------------
                            }
                            T->width+=width+T0->ptr[0]->ptr[1]->width;
                     }
                    T0=T0->ptr[1];
              	}
              	break;
	case STM_LIST:
                if (!T->ptr[0]) { T->code=NULL; T->width=0; break;}   //空语句序列
                if (T->ptr[1]) //2条以上语句连接，生成新标号作为第一条语句结束后到达的位置
                    strcpy(T->ptr[0]->Snext,newLabel());
                else     //语句序列仅有一条语句，Snext属性向下传递
                    strcpy(T->ptr[0]->Snext,T->Snext);
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);
                T->code=T->ptr[0]->code;
                T->width=T->ptr[0]->width;
                if (T->ptr[1]){     //2条以上语句连接,Snext属性向下传递
                    strcpy(T->ptr[1]->Snext,T->Snext);
                    T->ptr[1]->offset=T->offset;  //顺序结构共享单元方式
                    semantic_Analysis(T->ptr[1]);
                    //序列中第1条为表达式语句，返回语句，复合语句时，第2条前不需要标号
                    if (T->ptr[0]->kind==RETURN ||T->ptr[0]->kind==EXP_STMT ||T->ptr[0]->kind==COMP_STM)
                        T->code=merge(2,T->code,T->ptr[1]->code);
                    else
                        T->code=merge(3,T->code,genLabel(T->ptr[0]->Snext),T->ptr[1]->code);
                    if (T->ptr[1]->width>T->width) T->width=T->ptr[1]->width; //顺序结构共享单元方式
                    }
                break;
	case IF_THEN:
                strcpy(T->ptr[0]->Etrue,newLabel());
                strcpy(T->ptr[0]->Efalse,T->Snext);
                T->ptr[0]->offset=T->ptr[1]->offset=T->offset;
                boolExp(T->ptr[0]);
                T->width=T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext,T->Snext);
                semantic_Analysis(T->ptr[1]);      //if
                if (T->width<T->ptr[1]->width) T->width=T->ptr[1]->width;
                T->code=merge(3,T->ptr[0]->code, genLabel(T->ptr[0]->Etrue),T->ptr[1]->code);
                break;
	case IF_THEN_ELSE:
                strcpy(T->ptr[0]->Etrue,newLabel());
                strcpy(T->ptr[0]->Efalse,newLabel());
                T->ptr[0]->offset=T->ptr[1]->offset=T->ptr[2]->offset=T->offset;
                boolExp(T->ptr[0]);
                T->width=T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext,T->Snext);
                semantic_Analysis(T->ptr[1]);      //if
                if (T->width<T->ptr[1]->width) T->width=T->ptr[1]->width;
                strcpy(T->ptr[2]->Snext,T->Snext);
                semantic_Analysis(T->ptr[2]);      //else
                if (T->width<T->ptr[2]->width) T->width=T->ptr[2]->width;
                T->code=merge(6,T->ptr[0]->code,genLabel(T->ptr[0]->Etrue),T->ptr[1]->code,genGoto(T->Snext),genLabel(T->ptr[0]->Efalse),T->ptr[2]->code);
                break;
	case WHILE: 
                symbol_scope_TX.TY[symbol_scope_TX.top]='W';
                strcpy(T->ptr[0]->Etrue,newLabel());
                strcpy(T->ptr[0]->Efalse,T->Snext);
                T->ptr[0]->offset=T->ptr[1]->offset=T->offset;
                boolExp(T->ptr[0]);      //循环条件
                T->width=T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext,newLabel());
                semantic_Analysis(T->ptr[1]);      //循环体
                if (T->width<T->ptr[1]->width) T->width=T->ptr[1]->width;
                T->code=merge(5,genLabel(T->ptr[1]->Snext),T->ptr[0]->code,genLabel(T->ptr[0]->Etrue),T->ptr[1]->code,genGoto(T->ptr[1]->Snext));
                break;
	case FOR:
                symbol_scope_TX.TY[symbol_scope_TX.top]='W';
                T->ptr[0]->offset=T->ptr[1]->offset=T->ptr[2]->ptr[0]->offset=T->ptr[2]->ptr[1]->offset=T->offset; 
                strcpy(T->ptr[0]->Snext,newLabel());
 		strcpy(T->ptr[1]->Etrue,newLabel());
                strcpy(T->ptr[1]->Efalse,T->Snext);
		strcpy(T->ptr[2]->ptr[1]->Snext,T->Snext);
                Exp(T->ptr[0]);     
                if (T->width<T->ptr[0]->width) T->width=T->ptr[0]->width;
                boolExp(T->ptr[1]);      //循环条件                
                semantic_Analysis(T->ptr[2]->ptr[1]);
		if (T->width<T->ptr[2]->ptr[1]->width) T->width=T->ptr[2]->ptr[1]->width;                
                Exp(T->ptr[2]->ptr[0]);//循环变量控制语句                
                T->code=merge(7,T->ptr[0]->code,genLabel(T->ptr[0]->Snext),T->ptr[1]->code,genLabel(T->ptr[1]->Etrue),T->ptr[2]->ptr[1]->code,T->ptr[2]->ptr[0]->code,genGoto(T->ptr[0]->Snext));
                break; 
	case EXP_STMT:
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);
                T->code=T->ptr[0]->code;
                T->width=T->ptr[0]->width;
                break;
	case RETURN:if (T->ptr[0]){
                T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                num=symbolTable.index;
                do num--; while (symbolTable.symbols[num].flag!='F');
                if (T->ptr[0]->type!=symbolTable.symbols[num].type) {
                    semantic_error(5,T->pos, "","返回值类型错误");
                    T->width=0;T->code=NULL;
                    break;
                }
                T->width=T->ptr[0]->width;
                result.kind=ID; strcpy(result.id,symbolTable.symbols[T->ptr[0]->place].alias);
                result.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                T->code=merge(2,T->ptr[0]->code,genIR(RETURN,opn1,opn2,result));
                }
                else{
                    T->width=0;
                    result.kind=0;
                    T->code=genIR(RETURN,opn1,opn2,result);//genIR return---------------------------------------
                    }
                break;
	case BREAK: 
                //如果当前作用域不是While作用域，则报错
                printf("");
                int i=symbol_scope_TX.top-1;
                while( i>0 && symbol_scope_TX.TY[i]!='W' && symbol_scope_TX.TY[i]!='F'){
                    i--;
                }
                if(symbol_scope_TX.TY[i]!='W'){
                    semantic_error(4,T->pos,"","break位置错误");
                    break;
                }
                break;

	case CONTINUE: 
                printf("");
                int t=symbol_scope_TX.top-1;
                while( t>0 && symbol_scope_TX.TY[t]!='W' && symbol_scope_TX.TY[t]!='F'){
                    t--;
                }
                if(symbol_scope_TX.TY[t]!='W'){
                    semantic_error(3,T->pos,"","continue位置错误");
                    break;
                }
                break;
	case ID:
	case INT:
	case FLOAT:
	case CHAR:
	case ASSIGNOP:
	case INC:
	case DEC:
	case PLUSEQ:
	case MINUSEQ:
	case STAREQ:
	case DIVEQ:
	case AND:
	case OR:
	case RELOP:
	case PLUS:
	case MINUS:
	case STAR:
	case DIV:
	case NOT:
	case UMINUS:
	case FUNC_CALL:
		Exp(T);
		break;
    }
    }
}

extern void objectCode(struct codenode *head);

void semantic_Analysis0(struct ASTNode *T) {
    symbolTable.index=0;
    fillSymbolTable("read","",0,INT,'F',4);
    symbolTable.symbols[0].paramnum=0;
    fillSymbolTable("write","",0,INT,'F',4);
    symbolTable.symbols[1].paramnum=1;
    fillSymbolTable("x","",1,INT,'P',12);
    symbol_scope_TX.TX[0]=0;
    symbol_scope_TX.top=1;
    T->offset=0;
    semantic_Analysis(T);
    prnIR(T->code);
    objectCode(T->code);
 }
