#include<iostream>
#include<stdlib.h>
#include<string.h>
#define maxi 50
using namespace std;

char gjz[14][maxi] = {"void", "main", "int", "char", "if", "else", "for", 
						"while", "return", "printf", "scanf", "long", "double"};

char sf[19][3] = {"+", "-", "*", "/", "=", ">", ">=", "<", "<=", "==", "<>", "++", "--", ">>", "<<", "&&", "||", "!", "**"};
 
char jf[11][3] = {"(", ")", "[", "]", "{", "}", ",", ";", "\"", "\'", "\\"}; 

int jfNumber = 11, sfNumber = 19, gjzNumber = 14, blNumber = 2;
char s[maxi*10];
int len;
int integer_binary[64], decimal_binary[64];
int integer_index, decimal_index;

void integerToBinary(int n)
{
    if(n>0)
    {
        integerToBinary(n/2);
        integer_binary[integer_index++] = n%2;
    }
}

void decimalToBinary(double m)
{
    if(m>0.000001)
    {
        m=m*2;
        decimal_binary[decimal_index++] = (long)m;
        decimalToBinary(m-(long)m);
    }
}

//二进制转换 
void toBinary(double d) {
	integer_index = 0;
	decimal_index = 0;
	int x = (int) d;
	double f = d - x;
	integerToBinary(x);
	decimalToBinary(f);
}

//字符串转十进制数 
double strToDecimal(char *s) {
	int dB = 0;
	double xj = 0.1, d = 0.0;
	int len = strlen(s);
	for(int i = 0 ; i < len ; i++) {
		if(s[i] == '.') dB = 1;
		else {
			if(dB == 0) {
				d *= 10;
				d += (s[i] - '0');
			} else {
				d += xj * (s[i] - '0') ;
				xj *= 0.1;
			}
		} 
	}
	return d;
}


//关键字 
void print1(char *str)
{  
    int gjzFlag = 0; 
    for(int i = 0 ; i < gjzNumber ; i++) {
		if(strcmp(gjz[i], str) == 0) {
			printf("(%d,%s)\n", 101+i, str);
			gjzFlag = 1;
			break;
		}
	}
		
	if(gjzFlag != 1) {
		printf("(400,%s)\n", str);
	}
	
}
//二进制输出
void print2(char *str)
{  
    int gjzFlag = 0, blFlag = 0; 
    double d = strToDecimal(str);
		toBinary(d);
		printf("(500,");
		for(int i = 0; i < integer_index; i++) {
			printf("%d",integer_binary[i]);
		}
		if(integer_index == 0) {
				printf("0");
		}
		if(decimal_index > 0) {
			printf(".");
			for(int i = 0; i < 8; i++) {
				printf("%d",decimal_binary[i]);
			}
		}
		printf(")\n");
    
    
}
//算符
void print3(char *str)
{  
    for(int i = 0 ; i < sfNumber ; i++) {
		if(strcmp(sf[i], str) == 0) {
	 		printf("(%d,%s)\n", 201+i, str);
	 		break;
		 }
	}
}
//界符
void print4(char *str)
{   
    for(int i = 0 ; i < jfNumber ; i++) {
	 	if(strcmp(jf[i], str) == 0) {
	 		printf("(%d,%s)\n", 301+i, str);
	 		break;
		 }
	 } 
}


//解析函数 
void analyzeStr(char *str, int len) {
	int flag = 0;  //1解析，0开始解析
	int index = 0;
	char t[maxi];
	char ts[maxi];
	int f = 0;

	for(int i = 0 ; i <= len ; i++) {   
		if(str[i] == ' ' || i == len) {
			if(flag != 0) {
				if(f == 1){ 
				    print1(t);
				}else if(f == 2){
					print2(t);
				}else if(f == 3){
					print3(t);
				}else{
					print4(t);
				}
				 
				memset(t, 0, sizeof(t));
				index = 0;
				flag = 0;
				i--;
			}
			continue;
		}
       
		if(flag == 0) {
			flag = 1;
			if((str[i] <= 'z' && str[i] >= 'a') || (str[i] <= 'Z' && str[i] >= 'A')) {
				f = 1; //关键字 、标识符 
				t[index++] = str[i];
			} else if(str[i] <= '9' && str[i] >= '0'){
				f = 2; //常数 
				t[index++] = str[i];
			} else {
				t[index++] = str[i];
				int isJf = 0;
				for(int j = 0; j < jfNumber; j++) {
					if(strcmp(jf[j], t) == 0) {
						isJf = 1;
					}
				}
				if(isJf == 1) {
					f = 4; //界符 
				} else {
					f = 3; //算符 
				}
				
			}
		} else {
			if(f == 1) { //关键字 
				if((str[i] <= 'z' && str[i] >= 'a') || (str[i] <= 'Z' && str[i] >= 'A') || (str[i] <= '9' && str[i] >= '0')) {
					t[index++] = str[i];
				} else {
					print1(t);
					memset(t, 0, sizeof(t));
					index = 0;
					flag = 0;
					i--;
				}
			} else if(f == 2) { //常数 
				if((str[i] <= '9' && str[i] >= '0') || str[i] == '.') {
					t[index++] = str[i];
				} else {
					print2(t);
					memset(t, 0, sizeof(t));
					index = 0;
					flag = 0;
					i--;
				}
			} else if(f == 3){ //算符 
				t[index++] = str[i];
				int isSf = 0;
				for(int j = 0 ; j < sfNumber ; j++) {
					if(strcmp(t, sf[j]) == 0) {
						isSf = 1;	
						break;
					}
				}
				if(isSf != 1) {
					print3(ts);
					memset(t, 0, sizeof(t));
					memset(ts, 0, sizeof(ts));
					index = 0;
					flag = 0;
				    i--;
				}
			} else {  //f == 4 界符 
				print4(t); 
				memset(t, 0, sizeof(t));
				index = 0;
				flag = 0;
				i--;
			}
		}
	}
} 

void analyze(char *str, int len)
{
	int i;
	int index=0;
	char t[maxi];
	for(i=0;i<=len;i++)
	{	
		if(isalpha(str[i]))
		{	
			t[index++]=str[i];
		}
		else
		{	
			t[index]='\0';
			print1(t);
			memset(t, 0, sizeof(t));
			index=0;
		}
		
		if(!isalpha(str[i]) && !isxdigit(str[i]) && str[i] != ' ')
		{
			int isJf=0;
			while(!isalpha(str[i]) && !isxdigit(str[i]) && str[i]!=' ')
			{
				t[index++]=str[i];
				i++;
			}
			t[index]='\0';
			for(int j=0;j<jfNumber;j++)
			{	
				if(strcmp(t,jf[j]) == 0)
				{
					isJf=1;
				}
			}
			if(isJf)
			{
				print4(t);
			}else
			{
				print3(t);
			}
			memset(t,0,sizeof(t));
			index=0;	
		}
		
//		if((str[i] <= '9' && str[i] >= '0') || str[i] == '.')
//		{
//			//i++;
//			t[index++]=str[i];	
//		}
//		else
//		{
//			print2(t);
//			memset(t,0,sizeof(t));
//			index = 0;
//		}
	}
}
int main()
{
	freopen("scanf.txt", "r", stdin);
	freopen("printf.txt", "w", stdout); 
	while(gets(s)) {
		//printf("%s\n",s);
		len = strlen(s);
		analyzeStr(s, len);
		//analyze(s, len);
	}
	
	fclose(stdin);
	fclose(stdout);
	return 0;
}






 
