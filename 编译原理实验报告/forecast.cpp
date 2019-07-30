#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<iostream>
#include<stack>
#include<map> 
using namespace std;

const int maxStr = 100; //字符串最大长度 
const int maxNum = 50; //常数最大个数 

//存储产生式的结构体 
typedef struct production {
	char left[maxStr];  //产生式左部 
	char right[maxNum][maxStr];  //产生式右部 
	int rn;  //右部集合个数 
}Production;

//存储文法的结构体 
typedef struct grammar {
	char VT[maxNum]; //非终态集 
	char VN[maxNum]; //终态集 
	char S;  //开始符 
	Production P[maxNum]; //产生式 
	int pn, vtn, vnn; //各个个数 
}Grammar;


int T, N, vt_vn_tols, rows;//总数、列数 
char t[maxStr], s[maxStr];
string inputStr;
Grammar G, G1;
map<char, string> vn_first;//非终结符的FIRST集 
map<char, string>::iterator vn_first_iter;
map<string, string> cd_first;//候选式的FIRST集 
map<string, string>::iterator cd_first_iter;
map<char, string>vn_follow;//非终结符的FOLLOW集 
map<char, string>::iterator vn_follow_iter; 
string M[maxStr];//预测分析表 
stack<char> as;//分析栈
stack<char> temp;//临时栈

//判断字符串中是否含有某个字符
bool have_char(string s, char ch) {
	for(int i = 0 ; i < s.length() ; i++) {
		if(ch == s[i]) return true;
	}
	return false;
} 

//判断字符ch是否是非终结符
bool is_vn(char ch) {
	for(int i = 0 ; i < G1.vnn ; i++) {
		if(ch == G1.VN[i]) return true;
	}
	return false;
} 

//判断字符ch是否是终结符 
bool is_vt(char ch) {
	for(int i = 0 ; i < G1.vtn ; i++) {
		if(ch == G1.VT[i]) return true;
	}
	return false;
}

//替换字符串函数 
char *replaceStr(char *s1, char *s2, int pos1, int pos2) {//将s1中从pos1到pos2替换成s2 
	char s[maxStr];
	int index = 0;
	int len1 = strlen(s1);
	int len2 = strlen(s2);
	for(int i = 0 ; i < pos1 ; i++) {
		s[index++] = s1[i];
	}
	for(int i = 0 ; i < len2 ; i++) {
		s[index++] = s2[i];
	}
	for(int i = pos2 +1 ; i < len1 ; i++) {
		s[index++] = s1[i];
	}
	s[index] = '\0';
	return s;
}

//生成一个新的大写字母，不跟已有的非终结符冲突
char getUpperWord() {
	int flag;
	char x = 'A';
	int step = 1;
	while(step < 26) {
		flag = 0;
		for(int i = 0 ; i < G1.vnn ; i++) {
			if(x == G1.VN[i]) {
				flag = 1;
				x += step;
				break;
			}
		}
		if(flag == 0) {
			G1.VN[G1.vnn++] = x;//修改G1中的非终结符集 
			break;
		}
	}
	return x;
} 

//输出文法G 
void printGrammar(Grammar G)
{
	Grammar G1 = G;
	//终态集 
	for(int i = 0 ; i < G1.vtn ; i++) {
		cout<<G1.VT[i]<<" ";
	}
	cout<<endl;
	//非终态集 
	for(int i = 0 ; i < G1.vnn ; i++) {
		cout<<G1.VN[i]<<" ";
	}
	cout<<endl;
	//开始符 
	cout<<G1.S<<endl;
	//产生式 
	for(int i = 0 ; i < G1.pn ; i++) {
		cout<<G1.P[i].left<<"->";
		for(int j = 0 ; j < G1.P[i].rn ; j++) {
			cout<<G1.P[i].right[j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
  } 
 
//打印产生式 
void printProduction(Production p) {
	cout<<"打印产生式====="<<endl;
	cout<<p.left<<"->"; 
	for(int i = 0 ; i < p.rn ; i++) {
		cout<<p.right[i]<<" ";
	}
	cout<<endl;
} 

//打印预测分析表
void printPredictionAnalysisTable() {
	int len, t;
	cout<<"得到预测分析表"<<endl;
	for(int i = 0 ; i <= G1.vtn ; i++) {
		printf("----------------", G1.VT[i]);
	}
	cout<<endl;
	printf("----------------");
	for(int i = 0 ; i < G1.vtn && G1.VT[i] != '@'; i++) {
		printf("       %c        ", G1.VT[i]);
	}
	printf("       #        \n");
	for(int i = 0 ; i < vt_vn_tols ; i++) {
		if(i % rows == 0) cout<<"       "<<G1.VN[i / rows]<<"        ";
		len = M[i].length();
		t = 16 - len;
		for(int j = 0 ; j < t / 2 ; j++){
			cout<<" ";
		}
		cout<<M[i];
		for(int j = 0 ; j <  t - t / 2 ; j++){
			cout<<" ";
		}
		if((i + 1) % rows == 0) cout<<endl;
	}
	for(int i = 0 ; i <= G1.vtn ; i++) {
		printf("----------------", G1.VT[i]);
	}
	cout<<endl;
	cout<<endl;
}
 
//读取文法 
void read() {
	int len;
	int rLen, rIndex;
	char rt[10];
	for(int i = 0 ; i < 2 ; i++) {
		gets(s);
		len = strlen(s);
		rIndex = 0;
		for(int j = 0 ; j < len ; j++) {
			if(s[j] != ' '){
				if(i == 0) {
					G.VT[G.vtn++] = s[j];
				} else if(i == 1) {
					G.VN[G.vnn++] = s[j];
				}
			}
		}
	}
	G.S = getchar();
	scanf("%d", &N);
//	cout<<"okokok"<<G.pn<<endl; 
	getchar();
	
	char t[maxStr];
	int sIndex, j;
	for(int i = 0 ; i < N ; i++) {
		Production p;
	    p.rn = 0;
		gets(s);
		memset(t, 0, sizeof(t));
		sIndex = 0;
		len = strlen(s);

		for(j = 0 ; j <= len ; j++) {
			if(s[j] != '-' && s[j] != '|' && s[j] != '>' && j != len) {
				t[sIndex++] = s[j];
			} else if(s[j] == '>'){
				strcpy(p.left, t);
//				p.left = t;
				memset(t, 0, sizeof(t));
				sIndex = 0;	
			} else if(s[j] == '|' || j == len) {
				strcpy(p.right[p.rn++], t);
				memset(t, 0, sizeof(t));
				sIndex = 0;
			}
		}
		G.P[G.pn++] = p;
	}
	cin>>inputStr;
	getchar();
}

//左递归-新的产生式生成第一个右部 
char* createNewFirstRight(char *s, char ch) {
	char x[maxStr];
	int len = strlen(s);
	int index = 0;
	for(int i = 1 ; i < len ; i++) {
		x[index++] = s[i];
	}
	x[index++] = ch;
	x[index] = '\0';
//	cout<<"xxxxx...."<<x<<endl;
	return x;
}

//左递归-生成原来产生式的右部 
char* createSelfRight(char *s, char ch) {
	char x[maxStr];
	int index = 0;
	int len = strlen(s);
	for(int i = 0 ; i < len ; i++) {
		x[index++] = s[i];
	}
	x[index++] = ch;
	x[index] = '\0';
	return x; 
} 

//消除左递归
void eliminateLeftRecursion() {
	//G是文法
	G1 = G;
	int pIndex = 1, len;
	//将间接左递归转化为直接左递归 
	for(int i = 0 ; i < G1.pn ; i++) {//i是当前  
		for(int j = 0 ; j < i ; j++) {//j是之前 
			char t[maxStr];//存放可能会替换的（之前） 
			strcpy(t, G1.P[j].left);
			for(int k = 0 ; k < G1.P[j].rn ; k++) {
				len = strlen(G1.P[i].right[k]);
//				for(int m = 0 ; m < len ; m++) {
					if(t[0] == G1.P[i].right[k][0]) {
						int m = 0;//**
//						printGrammar(G1);
//						cout<<"i..."<<i<<"   j...."<<j<<endl;
						char r_char[maxStr];
						strcpy(r_char, G1.P[i].right[k]);
						strcpy(G1.P[i].right[k] , replaceStr(G1.P[i].right[k], G1.P[j].right[0], m, m));
						for(int o = 1 ; o < G1.P[j].rn ; o++) {
							strcpy(G1.P[i].right[G1.P[i].rn++], replaceStr(r_char, G1.P[j].right[o], m, m));
						}
					}
//				}
			} 
		}
		
		//消除直接左递归
		char pi_l = G1.P[i].left[0];
		char l_x;
		for(int j = 0 ; j < G1.P[i].rn ; j++) {//产生式的右部  
			if(pi_l == G1.P[i].right[j][0]) { //出现 P->Pa|b 的产生式
				//构造新的P1->aP1|~ 
				//左部 ，新生成一个大写字母
				l_x = getUpperWord();
				G1.P[G1.pn++].left[0] = l_x;
				//右部
				char *temp;
				temp = createNewFirstRight(G1.P[i].right[j], l_x);
//				cout<<"新创建的产生式的右部第一项....";
//				printf("%s\n", t);   不能输出，输出就改变指针位置了 
				strcpy(G1.P[G1.pn-1].right[0], temp);
				G1.P[G1.pn-1].right[1][0] = '@';
				G1.P[G1.pn-1].rn = 2; 
//				printProduction(G1.P[G1.pn-1]); 
				
				//构造P->bP1 
				Production p = G1.P[i];
				int p_index = 0;
				for(int k = 0 ; k < G1.P[i].rn ; k++) {
					if(p.right[k][0] != pi_l) {
						temp = createSelfRight(p.right[k], l_x);
						strcpy(G1.P[i].right[p_index++], temp);						
					}
//					printf("测试本来的产生式右部...%s\n", t);  不能输出，输出就会改变指针位置 
				}
				G1.P[i].rn = p_index;
			}
		}
		
	} 
	 
	G1.VT[G1.vtn++] = '@'; 
	cout<<"消除左递归输出....."<<endl;
	printGrammar(G1); 
}

//得非终结符的FIRST集 ,四个规则 参数是 某个非终结符  
void constructVNFIRST() {
	
	//规则0：终结符的first集是本身
	string temp_char;
	for(int i = 0 ; i < G1.vtn ; i++) {
		temp_char = "";
		temp_char += G1.VT[i];
		vn_first.insert(pair<char, string>(G1.VT[i], temp_char));
	} 
	
	//规则一：有@； 规则二：有终结符
	int vn_flage;
	char vn_first_key;
	string vn_first_value = "";
	for(int i = 0 ; i < G1.pn ; i++) {  
		for(int j = 0 ; j < G1.P[i].rn ; j++) { 
			char left_vn_first = G1.P[i].left[0];//左部第一个字符 
			char right_temp_first = G1.P[i].right[j][0];//右部每个产生式一个字符 
			vn_flage = 0;//非终结符标志 
			for(int k = 0 ; k < G1.vnn ; k++) {
				if(G1.VN[k] == right_temp_first) {
					vn_flage = 1;
					break;
				}
			}
			if(vn_flage == 0) { //是终结符 
				vn_first_iter = vn_first.find(left_vn_first);
				vn_first_key = left_vn_first;
				if(vn_first_iter != vn_first.end()) {
				    vn_first_value = vn_first_iter->second;
					vn_first.erase(vn_first_iter);
				} else {
					vn_first_value = "";
				}
				vn_first_value += right_temp_first;
				vn_first.insert(pair<char, string>(vn_first_key, vn_first_value));
			}
			
		}
	}
	
	//规则二 :产生式 X->Y; X->Y1Y2Y3... 
	int fist_change_flage;
	string temp_value;
	while(1) {
		fist_change_flage = 0;
		for(int i = 0 ; i < G1.pn ; i++) {
			for(int j = 0 ; j < G1.P[i].rn ; j++) {
				vn_flage = 0;
				char left_vn_first = G1.P[i].left[0];//左部 
				char right_temp_first = G1.P[i].right[j][0];//右部某个产生式的首位置 
				for(int k = 0 ; k < G1.vnn ; k++) {
					if(G1.VN[k] == right_temp_first) {
						vn_flage = 1;   
						break;
					}
				}
				if(vn_flage == 1) { //如果是非终结符 
					vn_first_key = left_vn_first;//左部赋值给key 
					vn_first_iter = vn_first.find(left_vn_first);
					if(vn_first_iter != vn_first.end()) {  //如果已经左部在map中有值 
						temp_value = vn_first_iter->second;
//						vn_first.erase(vn_first_iter);  这里不能提前清空键值对 
					} else {
						temp_value = "";
					}
//					cout<<"查询过map的temp_value值..."<<vn_first_key<<"--"<<temp_value<<endl; 
					vn_first_iter = vn_first.find(right_temp_first);
					if(vn_first_iter == vn_first.end()) {  //找右部首字符在map中的值 
						vn_first_value = "";
					} else {
						vn_first_value = vn_first_iter->second;
					}
//					cout<<"查询过map的vn_first_value值..."<<right_temp_first<<"--"<<vn_first_value<<endl; 
					//将vn_first_value中有的temp_value没有的非直接符放进temp_value中再存到map中去 
					int update_flag = 0; 
					for(int o = 0 ; o < vn_first_value.length() ; o++) {
						char ch = vn_first_value[o];
						int temp_flag = 1;
						for(int p = 0 ; p < temp_value.length() ; p++) {
							if(ch == temp_value[p]) { 
								temp_flag = 0; 
								break;
							}
						}
						if(temp_flag == 1 && ch != '@') {
							temp_value += ch;
							update_flag = 1;//需要跟新值 
						}
					}
					
//					cout<<"需要跟新的temp_value的值...."<<temp_value<<endl;
					if(update_flag == 1) {//如果需要改变map的值 ,用temp_value的值跟新 
						vn_first_iter = vn_first.find(vn_first_key);
						if(vn_first_iter != vn_first.end()) {
							vn_first.erase(vn_first_iter);
						}
						vn_first.insert(pair<char, string>(vn_first_key, temp_value));
						cout<<endl;
						fist_change_flage = 1;
					}
				}
			}
		}
		if(fist_change_flage == 0) {//没有找到还需要改变的了 
			break;  
		}
	}
	
	//规则三 ，P->P1P2P3...   Pi中都有@才在P中加@
	for(int i = 0 ; i < G1.pn ; i++) {
		vn_first_key = G1.P[i].left[0];
		vn_first_iter = vn_first.find(vn_first_key);//左部的值 
		temp_value = vn_first_iter->second;
		char temp_flag = 0;
		for(int j = 0 ; j < G1.P[i].rn ; j++) {  //遍历右部产生式 
			vn_flage = 1;
			char right_temp_first = G1.P[i].right[j][0];//右部某个产生式的第一个位置的字符 
			for(int k = 0 ; k < G1.vnn ; k++) {
				if(G1.VN[k] == right_temp_first) {
					vn_flage = 1;   
					break;
				}
			}
			if(vn_flage == 1) { //如果是非终结符 
				//如果非终结符没有@就不可能更新
				//判断是否有@
				vn_first_iter = vn_first.find(right_temp_first);
				string right_first_value = vn_first_iter->second;
				int have_flag = 0;
				for(int o = 0 ; o < right_first_value.length() ; o++) {
					if(right_first_value[o] == '@') {
						have_flag = 1;
					}
				}
				if(have_flag == 0) temp_flag = 0;
			}
		}
		if(temp_flag == 1) { //如果需要加上@ 
			temp_value += '@';
			vn_first_iter = vn_first.find(vn_first_key);
			vn_first.erase(vn_first_iter);
			vn_first.insert(pair<char, string>(vn_first_key, temp_value));
		}
	}
	cout<<"FIRST集"<<endl;
	for(vn_first_iter = vn_first.begin() ; vn_first_iter != vn_first.end() ; vn_first_iter++) {
		if(is_vn(vn_first_iter->first)) {
			cout<<vn_first_iter->first<<"   ";
			string value = vn_first_iter->second;
			for(int id = 0 ; id < value.length() ; id++) {
				cout<<value[id]<<" ";
			}
			cout<<endl;
		}
	}
	cout<<endl;
} 

//构造非终结符的FLLOW集 
void constructVNFOLLOW() {
	string follow_vn_value;//某个非终结符的value值 
	char vn;//某个非终结符值 
	char temp_ch;//临时字符
	int vn_flag;//是否是非终结符标志 
	int vt_flag;//是否是终结符标志 
	int follow_change_flag;//follow集改变的标志 
	
	//规则0，S加#
	vn_follow.insert(pair<char, string>(G1.S, "#"));
	
	//规则一 A->aBb非终结符后面跟着终结符
	for(int i = 0 ; i < G1.pn ; i++) {
		for(int j = 0 ; j < G1.P[i].rn ; j++) {
			int temp_len = strlen(G1.P[i].right[j]);
			for(int k = 0 ; k < temp_len; k++) {
				temp_ch = G1.P[i].right[j][k];
				vn = G1.P[i].right[j][k];
				vn_flag = 0;
				if(is_vn(temp_ch)) vn_flag = 1; 
				vt_flag = 0;
				if((k + 1) < temp_len) {
					temp_ch = G1.P[i].right[j][k+1];
					if(is_vt(temp_ch)) vt_flag = 1; 
				}
				if(vn_flag == 1 && vt_flag == 1) { //符合规则一的条件 
					vn_follow_iter = vn_follow.find(vn);
					if(vn_follow_iter != vn_follow.end() ){
						follow_vn_value = vn_follow_iter->second;
					} else {
						follow_vn_value = "";
					}
					if(!have_char(follow_vn_value, temp_ch)) {
						follow_vn_value += temp_ch;
						if(vn_follow_iter != vn_follow.end()) {
							vn_follow.erase(vn_follow_iter);
						}
						vn_follow.insert(map<char, string>::value_type(vn, follow_vn_value));	
					}
				}
			} 
		}
	}
	
	//规则二 A->aB 和 规则三 A->aB@
	int update_flag;
	string temp_left_value, temp_right_value;
	while(1) {
		follow_change_flag = 0;
		
		for(int i = 0 ; i < G1.pn ; i++) {
			char temp_left_key = G1.P[i].left[0];
			for(int j = 0 ; j < G1.P[i].rn ; j++) {
				int temp_len = strlen(G1.P[i].right[j]);
				for(int k = 0 ; k < temp_len; k++) {
//					cout<<"当前判断字符:...."<<G1.P[i].right[j][k]<<endl;
					temp_ch = G1.P[i].right[j][k];
					vn = G1.P[i].right[j][k];
					update_flag = 0;//是否要更新的标志 
					if(is_vn(temp_ch)) {
						vn_follow_iter = vn_follow.find(temp_ch);
						if(vn_follow_iter != vn_follow.end()) { //如果B存在follow集 
							temp_right_value = vn_follow_iter->second;
						} else {
							temp_right_value = "";
						}
						if(k == temp_len - 1) {//如果是最后一个
							vn_follow_iter = vn_follow.find(temp_left_key);
							if(vn_follow_iter != vn_follow.end()) { //如果A存在follow集 
								temp_left_value = vn_follow_iter->second;
							} else {
								temp_left_value = "";
							}
							
//							cout<<"左部值。。"<<temp_left_value<<"右部值..."<<temp_right_value<<endl;
							for(int l = 0 ; l < temp_left_value.length() ; l++) { // 判断是不是要更新 
								char ch = temp_left_value[l];
								int have_flag = 0;
								for(int r = 0 ; r < temp_right_value.length() ; r++) {
									if(ch == temp_right_value[r]) {
										have_flag = 1;
										break;
									}
								}
								if(have_flag == 0) {
									update_flag = 1;
									temp_right_value += ch;
								}
							}
						} else { //不是最后一个字符
							string temp_value;
							//string temp_left_value, temp_right_value;
							vn_follow_iter = vn_follow.find(temp_left_key);
							if(vn_follow_iter != vn_follow.end()) { //如果A存在follow集 
								temp_left_value = vn_follow_iter->second;
							} else {
								temp_left_value = "";
							}
							char temp_next =  G1.P[i].right[j][k+1];
							if(is_vn(temp_next)) {
								vn_first_iter = vn_first.find(temp_next);
								temp_value = vn_first_iter->second;
//								cout<<"非终结符的..."<<temp_next<<" first的值是..."<<temp_value<<endl;
//								cout<<"本身的值。。。"<<temp_ch<<temp_right_value<<endl; 
								//假如没有@ 
								char have_empty = 0;
								for(int l = 0 ; l < temp_value.length() ; l++) { // 判断是不是要更新 
									char ch = temp_value[l];
									int have_flag = 0;
									for(int r = 0 ; r < temp_right_value.length() ; r++) {
										if(ch == temp_right_value[r]) {
											have_flag = 1;
										}
										if(temp_value[l] == '@') have_empty = 1;
									}
//									cout<<"update_flag..."<<update_flag<<endl;
									if(have_flag == 0 && ch != '@') {
										update_flag = 1;
										temp_right_value += ch;
									}
								}
								if(have_empty == 1) {
									vn_follow_iter = vn_follow.find(temp_left_key);
									if(vn_follow_iter != vn_follow.end()) { //如果A存在follow集 
										temp_left_value = vn_follow_iter->second;
									} else {
										temp_left_value = "";
									}
//									cout<<"在后面的非终结符的first集里有@，的前面的follow值"<<temp_left_key<<"--"<<temp_left_value<<endl;
									for(int l = 0 ; l < temp_left_value.length() ; l++) { // 判断是不是要更新 
										char ch = temp_left_value[l];
										int have_flag = 0;
										for(int r = 0 ; r < temp_right_value.length() ; r++) {
											if(ch == temp_right_value[r]) {
												have_flag = 1;
												break;
											}
										}
										if(have_flag == 0) {
											update_flag = 1;
											temp_right_value += ch;
										}
									}
								}
							}
						} 
					} 
					
					if(update_flag == 1) { //需要更新 
						//更新操作
						vn_follow_iter = vn_follow.find(vn);
						if(vn_follow_iter != vn_follow.end()) {
							vn_follow.erase(vn_follow_iter);
						}
//						cout<<"更新的键值对..."<<endl;
//						cout<<vn<<"--"<<temp_right_value<<endl;
						if(temp_right_value != "")  {
							vn_follow.insert(pair<char, string>(vn, temp_right_value));
						}
						follow_change_flag = 1; 
					}
				}
			}
		}
		
		if(follow_change_flag == 0) break;
	} 
	
	cout<<"FOLLOW集"<<endl;
	for(vn_follow_iter = vn_follow.begin() ; vn_follow_iter != vn_follow.end() ; vn_follow_iter++) {
		if(is_vn(vn_follow_iter->first)) {
			cout<<vn_follow_iter->first<<"   ";
			string value = vn_follow_iter->second;
			for(int id = 0 ; id < value.length() ; id++) {
				cout<<value[id]<<" ";
			}
			cout<<endl;
		}
		
	}
	cout<<endl; 
}

//构造各个候选式的FIRST集 
void constructCDFIRST() {
	string temp_cd, temp_value;
	char temp_ch;
	for(int i = 0 ; i < G1.pn ; i++) {
		for(int j = 0 ; j < G1.P[i].rn ; j++) {
			temp_cd = G1.P[i].right[j];
			temp_ch = G1.P[i].right[j][0];
			vn_first_iter = vn_first.find(temp_ch);
			temp_value = vn_first_iter->second;
			cd_first_iter = cd_first.find(temp_cd);
			if(cd_first_iter != cd_first.end()) {
				cd_first.erase(cd_first_iter);
			}
			cd_first.insert(pair<string, string>(temp_cd, temp_value));
		}
	}
	cout<<"候选式的FIRST集"<<endl;
	for(cd_first_iter = cd_first.begin() ; cd_first_iter != cd_first.end() ; cd_first_iter++) {
		cout<<cd_first_iter->first<<"   ";
		string value = cd_first_iter->second;
		for(int id = 0 ; id < value.length() ; id++) {
			cout<<value[id]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}

//根据字符查到对应的非终结符对应的行数 
int findVNCol(char ch) {
	for(int i = 0 ; i < G1.vnn ; i++) {
		if(ch == G1.VN[i]) return i;
	}
}

//根据字符查到对应的终结符对应的列数 
int findVTRow(char ch) {
	if(ch == '#') return G1.vtn - 1;
	for(int i = 0 ; i < G1.vtn ; i++) {
		if(ch == G1.VT[i]) return i;
	}
}

//构造预测分析表 
void constructPredictionAnalysisTable() {
	string temp_cd, temp_first_value, temp_follow_value, temp_value;
	vt_vn_tols = (G1.vnn * G1.vtn); //数组总长 
    rows = G1.vtn; //数组列数 
	int temp;
	char temp_vn, temp_vt;
	int col, row;//非终结符对应的行数，产生式的first集每个终结符对应的列数 
	for(int i = 0 ; i < G1.pn ; i++) {
		for(int j = 0 ; j < G1.P[i].rn ; j++) {
			temp_cd = G1.P[i].right[j];//右部某个产生式 
			temp_vn = G1.P[i].left[0];//左部非终结符 
			col = findVNCol(temp_vn); 
			cd_first_iter = cd_first.find(temp_cd);
			temp_first_value = cd_first_iter->second;
			vn_follow_iter = vn_follow.find(temp_vn);
			temp_follow_value = vn_follow_iter->second;
			if(temp_cd == "@") { //用FOLLOW集 
				for(int k = 0 ; k < temp_follow_value.length() ; k++) {
					temp_value = temp_vn;
					temp_vt = temp_follow_value[k]; 
					row = findVTRow(temp_vt);
					temp = rows * col + row;
					temp_value += "->@";
					M[temp] = temp_value;
				}
			} else {
				for(int k = 0 ; k < temp_first_value.length() && temp_first_value[k] != '@' ; k++) {//将产生式的FIRST集写到表中 
					temp_value = temp_vn;
					temp_vt = temp_first_value[k]; 
					row = findVTRow(temp_vt);
					temp = rows * col + row;
					temp_value += "->";
					temp_value += temp_cd;
					M[temp] = temp_value;
				}
			}
		}
	}
}

//输出分析栈
void printStack() {
	int size = as.size();
	int t = 22 - size;
	char ch;
	for(int i = 0 ; i < t / 2 ; i++) {
		cout<<" ";
	}
	while(!as.empty()) {
		ch = as.top();
		as.pop();
		temp.push(ch);
	}
	while(!temp.empty()) {
		ch = temp.top();
		cout<<ch;
		temp.pop();
		as.push(ch);
	}
	for(int i = 0 ; i < t - (t / 2) ; i++) {
		cout<<" ";
	}
	cout<<"|";
} 

//输出输入串
void printInputStr(int index) {
	int t = 22 - (inputStr.length() - index);
	for(int i = 0 ; i < t / 2  ; i++) {
		cout<<" ";
	}
	for(int i = index ; i < inputStr.length() ; i++) {
		cout<<inputStr[i];
	}
	for(int i = 0 ; i < t - (t / 2) ; i++) {
		cout<<" ";
	}
	cout<<"|";
} 

//输出动作 
void printAction(string s) {
	cout<<"       "<<s<<endl;	
}

//输入步骤
void printStep(int s) {
	if(s < 10) {
		cout<<"  "<<s<<"     |";	
	} else if(s < 100) {
		cout<<"  "<<s<<"    |";
	} else if(s < 1000) {
		cout<<"  "<<s<<"   |";
	}
	
} 

//出现错误
void error(int x) {
	printStep(x);
	printStack();
	printInputStr(0);
	printAction("匹配出错");
	printf("-------------------------------------------------------------------------\n");
	cout<<"分析句子出错了, 文法G不能推出句子"<<inputStr<<endl;
} 

//匹配成功
void success(int x) {
	printStep(x);
	printStack();
	printInputStr(inputStr.length() - 1);
	printAction("成功，分析过程终止");
	printf("-------------------------------------------------------------------------\n");
	cout<<"匹配成功，文法G可以推出句子"<<inputStr<<endl;
} 

//LL1文法执行
bool LL1() {
	int step = 1;//步骤 
	string temp_value;
	char top, temp;
	as.push('#');
	as.push(G1.S);
	int index , row, col, id;
	index = 0;
	cout<<"LL1文法匹配过程"<<endl;
	printf("-------------------------------------------------------------------------\n");
	printf("  步骤  |        分析栈        |        输入串        |        采取的动作\n");
	printStep(step++);
	printStack();
	printInputStr(index);
	printAction("初始化");
	while(!as.empty()) {
		top = as.top();
		if(top == '#' && inputStr[index] == '#') {
			success(step);
			return true; 
		}
		if(top == '#' && inputStr[index] != '#') {
			error(step);
			return false; 
		}
		temp = inputStr[index];
		if(is_vt(top)) { //如果栈顶是终结符 
			if(top == temp) {
//				cout<<"终结符匹配------栈顶---"<<top<<"  输入串---"<<inputStr[index]<<endl;;  
//				cout<<"出栈的是..."<<top<<endl;
				as.pop();
				index++;
				printStep(step++);
				printStack();
				printInputStr(index);
				printAction("匹配，读入下一个符号");
			} else {
				error(step);
				return false;
			}
		} else { //栈顶不是终结符 
			col = findVNCol(top);
			row = findVTRow(inputStr[index]);
			id = col * rows + row;
			temp_value = M[id];
			if(!temp_value.empty()) {
				if(temp_value[3] == '@') {
//					cout<<"出栈的是..."<<top<<endl;
					as.pop();
					printStep(step++);
					printStack();
					printInputStr(index);
					string ss;
					
					ss += "用";
					ss += top;
					ss += "->@自动匹配"; 
					printAction(ss);
				} else {
					as.pop();
					string s_temp;
					for(int j = temp_value.length() - 1 ; j >= 3 ; j--) { //反序入栈 
//					cout<<"入栈的是..."<<temp_value[j]<<endl;
						s_temp += temp_value[j];
						as.push(temp_value[j]);
					}
					printStep(step++);
					printStack();
					printInputStr(index);
					string ss;
					ss += "用";
					ss += top;
					ss += "->";
					for(int k = s_temp.length() - 1 ; k >= 0 ; k--) {
						ss += s_temp[k];
					}
					ss += "推导"; 
					printAction(ss);
				}
			} else {
				error(step);
				return false;
			}
		}
	}
}


int main()
{
	freopen("文法.txt", "r", stdin);
	freopen("输出.txt", "w", stdout);
	
	scanf("%d", &T);
	getchar();
	int n = T;
	while(T--) {
		if(T < n-1) cout<<endl<<endl<<endl;
		//cout<<"==================================================================="<<"第"<<n - T<<"次进行LL1分析"<<"=================================================================="<<endl; 
		vn_first.clear();
		vn_follow.clear();
		cd_first.clear();
		while(!temp.empty()) temp.pop();
		while(!as.empty()) as.pop(); 
		memset(G.VN, 0, sizeof(G.VN));
		memset(G.VT, 0, sizeof(G.VT));
		memset(G.P, 0, sizeof(G.P));
		for(int i = 0 ; i < vt_vn_tols ; i++) {  //预测分析表清空 
			M[i] = "";
		}
		G.pn = 0; 
		G.vtn = 0;
		G.vnn = 0;
		G1 = G; 
		read();
		cout<<"输出文法G.....\n";
		printGrammar(G);
		eliminateLeftRecursion();
		constructVNFIRST();
		
		constructVNFOLLOW();
		constructCDFIRST();
		constructPredictionAnalysisTable();
		printPredictionAnalysisTable(); 
		LL1();
	}
	
	fclose(stdin);
	fclose(stdout);
	return 0;
}
