/* CSE 4130 (Fall 2021)                  Project                  ID: 17.01.04.001               */

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include <stdlib.h>

FILE *p1,*p2;


char dataTypes[10][20] = {"auto","double","float","int","short","char"};
int data_no = 6;
int keyWord_no = 32;
int operator_no = 30;
int separator_no = 4;
int parenthesis_no = 6;

char keyWords[35][20]= { "auto","const","double","float","int","short","struct","unsigned",
                         "break","continue","else","for","long","signed","switch","void",
                         "case","default","enum","goto","register","sizeof","typedef","volatile",
                         "char","do","extern","if","return","static","union","while"
                       };

char operators[35][20]= {"+","-","*","/","%","++","--","=","+=","-=","*=","/=","%=","==","->",
                         ">","<","!=","<=",">=","&&","||","!","&","|","!","^","~","<<",">>"
                        };

char separators[35][20]= {",","'","\"",";"};

char parenthesis[35][20]= {"(",")","{","}","[","]"};

char header_name_list[100][100];
int header_name_cnt=0;

char keyWord_list[200][100];
int keyWord_cnt = 0;

char id_list[200][100];
int id_cnt = 0;

char function_list[200][100];
int function_cnt = 0;

int isDataType(char lex[]);
int token_conflict(char prev[1000],char cur[1000]);

int isKeyWord(char lex[]);
int isIdentifier(char lex[]);
int isSeparator(char lex[]);
int isNumber(char lex[]);
int isOperator(char lex[]);
int isParenthesis(char lex[]);
int opType(char x);
void wrFile(char str[]);
void insert_keyWord(char lex[]);
void insert_id(char lex[]);

int take_input();
int scan_and_filter();
int header_file_separation();
int separate_lexemes();
int categorize_lexemes();

int main()
{
    if(take_input()==0)             return 0;

    if(scan_and_filter()==0)        return 0;

    if(header_file_separation()==0) return 0;

    if(separate_lexemes()==0)       return 0;

}


int isDataType(char lex[])
{
    for(int i=0; i<data_no; i++)
        if(!strcmp(lex,dataTypes[i]))return 1;
    return 0;
}

int token_conflict(char prev[1000],char cur[1000])
{
    if(isDataType(cur)==1&&(!strcmp("const",prev) ||!strcmp("signed",prev) ||!strcmp("unsigned",prev)||!strcmp("register",prev)))
        return 0;
    if(!strcmp("const",prev)&&(!strcmp("signed",prev)||!strcmp("unsigned",prev)))return 0;
    if(!strcmp("else",prev)||!strcmp("typedef",prev)||!strcmp("static",prev)) return 0;
    if(!strcmp(prev,"long") && !strcmp(cur,"long"))return 0;
    return 1;
}

int isKeyWord(char lex[])
{
    for(int i=0; i<keyWord_no; i++)
        if(!strcmp(lex,keyWords[i]))
            return 1;
    return 0;
}

int isIdentifier(char lex[])
{
    if(!(isalpha(lex[0]) || lex[0]=='_'))return 0;
    int len = strlen(lex);
    for(int i=0; i<len; i++)
        if( !(isalnum(lex[i]) || lex[i]=='_') )return 0;
    return 1;
}

int isSeparator(char lex[])
{
    for(int i=0; i<separator_no; i++)
        if(!strcmp(separators[i],lex))
            return 1;
    return 0;
}

int isNumber(char lex[])
{
    if(!strcmp(lex,"NULL") || !strcmp(lex,"nullptr")) return 1;
    int state = 0;
    int base = 10;
    int fnd = 0;
    int len = strlen(lex);
    if(base==10)
    {
        for(int i=0; i<len; i++)
        {


            if(!(isdigit(lex[i]) || lex[i]=='.'))return 0;
            if(isdigit(lex[i]) && (state==0 || state==2))state=state+1;
            else if(isdigit(lex[i]) && (state==1||state==3||state==4))continue;
            else if(lex[i]=='.' && (state==0||state==1))state=2;
            else if(lex[i]=='.' && (state==2||state==3||state==4))state=4;
        }

    }

}

int isOperator(char lex[])
{
    for(int i=0; i<operator_no; i++)
        if(!strcmp(operators[i],lex))
            return 1;
    return 0;
}

int isParenthesis(char lex[])
{
    for(int i=0; i<parenthesis_no; i++)
        if(!strcmp(parenthesis[i],lex))
            return 1;
    return 0;
}

int opType(char x)
{
    /// type 2 ops
    /// anything except alphanumeric characters, '.' , ' ' , '_' and the ones in the condition below
    if(x=='+' ||x=='-' ||x=='*' ||x=='/' ||x=='%' ||x=='=' ||x=='<' ||x=='>' ||x=='!' ||x=='&' ||x=='|' ||x=='^' ||x=='~')
        return 1;
    return 2;
}

void wrFile(char str[])
{
    int len = strlen(str);
    for(int i=0; i<len; i++)
        fputc(str[i],p2);
}

void insert_keyWord(char lex[])
{
    for(int i=0; i<keyWord_cnt; i++)
        if(!strcmp(keyWord_list[i],lex))return;
    strcpy(keyWord_list[keyWord_cnt++],lex);
}

void insert_id(char lex[])
{
    for(int i=0; i<id_cnt; i++)
        if(!strcmp(id_list[i],lex))return;
    strcpy(id_list[id_cnt++],lex);
}

int take_input()
{
    char c;
    p1 = fopen("1. Main Input.txt","w");
    while(scanf("%c",&c)!=EOF)
    {
        fputc(c,p1);
    }
    fclose(p1);
    return 1;
}

int scan_and_filter()
{
    char c, prev_char='-';
    int double_quote=0;             /// inside a double quote
    int single_quote=0;             /// inside a single quote
    int comment_start_flag=0;       /// there is a / at hand
    int escape_seq_flag=0;          /// there was \ in last turn
    int white_space=1;              /// found any whitespace

    p1 = fopen("1. Main Input.txt","r");
    p2 = fopen("2. Scan and Filter.txt","w");
    if(!p1)
    {
        printf("Error at scan and filter\n");
        return 0;
    }

    while( (c=fgetc(p1)) !=EOF)
    {
        if(c=='\n')
            fputc(c, p2);
        else if(c=='\t'||c==' ')                      /// remove all extra whitespace
        {
            if(comment_start_flag==1)
            {
                fputc('/', p2);
                comment_start_flag=0;
            }
            if(double_quote==1 || single_quote==1)              /// if inside a double quote or single quote
                fputc(c, p2);                                   /// print the whitespace
            else white_space=1;
        }
        else
        {
            if(white_space == 1)                                /// print whitespace if the previous char before the
            {
                /// whitespaces and the current one, both were alphanumeric
                if(isalnum(prev_char) && isalnum(c))
                    fputc(' ', p2);
                white_space=0;
            }
            if(double_quote==0 && single_quote==0 && c=='/')    /// not inside any single or double quote
            {
                if(comment_start_flag==1)                       /// start of a line comment
                {
                    while(comment_start_flag==1)
                    {
                        c=fgetc(p1);
                        if(c==EOF || c=='\n')
                        {
                            if(c=='\n')
                                fputc(c, p2);
                            comment_start_flag=0;
                        }

                    }
                }
                else comment_start_flag=1;
            }
            else if(c=='*' && comment_start_flag==1)            /// start of a block comment
            {
                int state=0;
                while(comment_start_flag==1)                    /// state description
                {
                    /// 0 - initial state
                    c=fgetc(p1);                                /// 1 - found a '*'
                    if(c=='*')state=1;
                    else if(c=='/'&& state==1)
                        comment_start_flag=0;
                    else state=0;
                    if(c=='\n')
                        fputc(c, p2);
                }
            }
            else
            {
                if(comment_start_flag==1)
                    fputc('/', p2);
                comment_start_flag=0;

                if(c=='"' && single_quote==0)
                    double_quote = double_quote ^ 1;
                if(double_quote==0)
                {
                    if(c=='\'')
                    {
                        if(escape_seq_flag==0)                  /// not this kind of situation -- '\''
                            single_quote = single_quote ^ 1;
                        else escape_seq_flag=0;                 /// was part of one -- '\'' -- ended it
                    }
                    else if(c=='\\')
                        escape_seq_flag=(escape_seq_flag+1)%2;  /// two consecutive '\' cancels each other out
                    else escape_seq_flag=0;
                }
                fputc(c, p2);
            }
            prev_char=c;
        }
    }
    fclose(p1);
    fclose(p2);
    return 1;
}

int header_file_separation()
{
    p1 = fopen("2. Scan and Filter.txt","r");
    p2 = fopen("3. Header File Separation.txt","w");

    if(!p1)
    {
        printf("Error at heaqder file separation\n");
        return 0;
    }
    char c;
    char prev_char = '-';
    int first_char = 1;

    while( (c=fgetc(p1)) !=EOF)
    {
        if(c=='\n')
        {
            first_char = 1;
            prev_char ='#';
            fputc(c,p2);
        }
        else
        {
            if(first_char == 1)
            {
                if(c=='#')
                {
                    int len = 0;
                    prev_char ='#';
                    while(1)
                    {
                        c=fgetc(p1);
                        if(c=='\n'||c=='<'||c==EOF)
                            break;
                    }
                    if(c==EOF)
                        break;
                    if(c=='\n')
                    {
                        fputc(c,p2);
                        continue;
                    }
                    while(1)
                    {
                        c=fgetc(p1);
                        if(c=='\n'||c=='>'||c==EOF)
                            break;
                        header_name_list[header_name_cnt][len++]=c;
                    }
                    header_name_list[header_name_cnt][len]='\0';
                    header_name_cnt++;

                    if(c==EOF)
                        break;
                    if(c=='\n')
                        fputc(c,p2);
                    continue;
                }
                else if(isalnum(prev_char) && isalnum(c))
                    fputc(' ',p2);
            }
            first_char = 0;
            prev_char = c;
            fputc(c,p2);
        }
    }
    fclose(p1);
    fclose(p2);
    return 1;
}

int separate_lexemes()
{
    p1 = fopen("3. Header File Separation.txt","r");
    p2 = fopen("4. Separated Lexemes.txt","w");

    int flag=0;         /// 1 - alphanumeric characters or '.' , ' ' , '_'
    /// 2 - anything else
    int flag2=0;        /// keep track of the return value of opType()
    char c;

    if(!p1)
    {
        printf("Error at lexemes separation\n!");
        return 0;
    }

    while( (c=fgetc(p1)) !=EOF)
    {
        if(c=='\n')
        {
            flag=-1;
            flag2=-1;
            fputc(c, p2);
        }
        else if(isalnum(c) ||c=='.'||c==' '||c=='_')
        {
            if(flag==2)
                fputc(' ', p2);
            flag2=0;
            flag=1;
            fputc(c, p2);
        }
        else
        {
            if((flag==1 || (opType(c)==1 && flag2==2) || opType(c)==2) && flag2!=-1)
                fputc(' ', p2);
            flag=2;
            fputc(c, p2);
            flag2=opType(c);
        }
    }
    fclose(p1);
    fclose(p2);
    return 1;
}

int categorize_lexemes()
{
    p1 = fopen("4. Separated Lexemes.txt","r");
    p2 = fopen("5. Categorized Lexemes.txt","w");

    char c;
    char ch[1000];
    int len=0;

    if(!p1)
    {
        printf("\nError in lexems categorizing\n");
        return 0;
    }

    while( (c=fgetc(p1)) !=EOF)
    {
        if(c==' ' || c==EOF || c=='\n')
        {
            ch[len]='\0';
            if(isKeyWord(ch) && len!=0)
            {
                insert_keyWord(ch);
                wrFile("kw ");
            }

            else if(isIdentifier(ch) && len!=0)
            {
                insert_id(ch);
                wrFile("id ");
            }

            else if(isSeparator(ch) && len!=0)
                wrFile("sep ");
            else if(isNumber(ch) && len!=0)
                wrFile("num ");
            else if(isOperator(ch) && len!=0)
                wrFile("op ");
            else if(isParenthesis(ch) && len!=0)
                wrFile("par ");
            else if(len!=0)wrFile("unkn ");

            if(len!=0)
            {
                ///ch[len++]=']';
                if(c!=EOF && c!='\n')ch[len++]=' ';
                ch[len]='\0';
                wrFile(ch);
            }
            if(c=='\n')
                fputc(c,p2);
            if(c==EOF)break;
            strcpy(ch,"");
            len=0;
        }
        else ch[len++]=c;
    }
    fclose(p1);
    fclose(p2);
    return 1;
}

