#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<iostream>
#include<stack>
#include<map> 
using namespace std;

const int maxStr = 100; //�ַ�����󳤶� 
const int maxNum = 50; //���������� 

//�洢����ʽ�Ľṹ�� 
typedef struct production {
	char left[maxStr];  //����ʽ�� 
	char right[maxNum][maxStr];  //����ʽ�Ҳ� 
	int rn;  //�Ҳ����ϸ��� 
}Production;

//�洢�ķ��Ľṹ�� 
typedef struct grammar {
	char VT[maxNum]; //����̬�� 
	char VN[maxNum]; //��̬�� 
	char S;  //��ʼ�� 
	Production P[maxNum]; //����ʽ 
	int pn, vtn, vnn; //�������� 
}Grammar;


int T, N, vt_vn_tols, rows;//���������� 
char t[maxStr], s[maxStr];
string inputStr;
Grammar G, G1;
map<char, string> vn_first;//���ս����FIRST�� 
map<char, string>::iterator vn_first_iter;
map<string, string> cd_first;//��ѡʽ��FIRST�� 
map<string, string>::iterator cd_first_iter;
map<char, string>vn_follow;//���ս����FOLLOW�� 
map<char, string>::iterator vn_follow_iter; 
string M[maxStr];//Ԥ������� 
stack<char> as;//����ջ
stack<char> temp;//��ʱջ

//�ж��ַ������Ƿ���ĳ���ַ�
bool have_char(string s, char ch) {
	for(int i = 0 ; i < s.length() ; i++) {
		if(ch == s[i]) return true;
	}
	return false;
} 

//�ж��ַ�ch�Ƿ��Ƿ��ս��
bool is_vn(char ch) {
	for(int i = 0 ; i < G1.vnn ; i++) {
		if(ch == G1.VN[i]) return true;
	}
	return false;
} 

//�ж��ַ�ch�Ƿ����ս�� 
bool is_vt(char ch) {
	for(int i = 0 ; i < G1.vtn ; i++) {
		if(ch == G1.VT[i]) return true;
	}
	return false;
}

//�滻�ַ������� 
char *replaceStr(char *s1, char *s2, int pos1, int pos2) {//��s1�д�pos1��pos2�滻��s2 
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

//����һ���µĴ�д��ĸ���������еķ��ս����ͻ
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
			G1.VN[G1.vnn++] = x;//�޸�G1�еķ��ս���� 
			break;
		}
	}
	return x;
} 

//����ķ�G 
void printGrammar(Grammar G)
{
	Grammar G1 = G;
	//��̬�� 
	for(int i = 0 ; i < G1.vtn ; i++) {
		cout<<G1.VT[i]<<" ";
	}
	cout<<endl;
	//����̬�� 
	for(int i = 0 ; i < G1.vnn ; i++) {
		cout<<G1.VN[i]<<" ";
	}
	cout<<endl;
	//��ʼ�� 
	cout<<G1.S<<endl;
	//����ʽ 
	for(int i = 0 ; i < G1.pn ; i++) {
		cout<<G1.P[i].left<<"->";
		for(int j = 0 ; j < G1.P[i].rn ; j++) {
			cout<<G1.P[i].right[j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
  } 
 
//��ӡ����ʽ 
void printProduction(Production p) {
	cout<<"��ӡ����ʽ====="<<endl;
	cout<<p.left<<"->"; 
	for(int i = 0 ; i < p.rn ; i++) {
		cout<<p.right[i]<<" ";
	}
	cout<<endl;
} 

//��ӡԤ�������
void printPredictionAnalysisTable() {
	int len, t;
	cout<<"�õ�Ԥ�������"<<endl;
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
 
//��ȡ�ķ� 
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

//��ݹ�-�µĲ���ʽ���ɵ�һ���Ҳ� 
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

//��ݹ�-����ԭ������ʽ���Ҳ� 
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

//������ݹ�
void eliminateLeftRecursion() {
	//G���ķ�
	G1 = G;
	int pIndex = 1, len;
	//�������ݹ�ת��Ϊֱ����ݹ� 
	for(int i = 0 ; i < G1.pn ; i++) {//i�ǵ�ǰ  
		for(int j = 0 ; j < i ; j++) {//j��֮ǰ 
			char t[maxStr];//��ſ��ܻ��滻�ģ�֮ǰ�� 
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
		
		//����ֱ����ݹ�
		char pi_l = G1.P[i].left[0];
		char l_x;
		for(int j = 0 ; j < G1.P[i].rn ; j++) {//����ʽ���Ҳ�  
			if(pi_l == G1.P[i].right[j][0]) { //���� P->Pa|b �Ĳ���ʽ
				//�����µ�P1->aP1|~ 
				//�� ��������һ����д��ĸ
				l_x = getUpperWord();
				G1.P[G1.pn++].left[0] = l_x;
				//�Ҳ�
				char *temp;
				temp = createNewFirstRight(G1.P[i].right[j], l_x);
//				cout<<"�´����Ĳ���ʽ���Ҳ���һ��....";
//				printf("%s\n", t);   �������������͸ı�ָ��λ���� 
				strcpy(G1.P[G1.pn-1].right[0], temp);
				G1.P[G1.pn-1].right[1][0] = '@';
				G1.P[G1.pn-1].rn = 2; 
//				printProduction(G1.P[G1.pn-1]); 
				
				//����P->bP1 
				Production p = G1.P[i];
				int p_index = 0;
				for(int k = 0 ; k < G1.P[i].rn ; k++) {
					if(p.right[k][0] != pi_l) {
						temp = createSelfRight(p.right[k], l_x);
						strcpy(G1.P[i].right[p_index++], temp);						
					}
//					printf("���Ա����Ĳ���ʽ�Ҳ�...%s\n", t);  �������������ͻ�ı�ָ��λ�� 
				}
				G1.P[i].rn = p_index;
			}
		}
		
	} 
	 
	G1.VT[G1.vtn++] = '@'; 
	cout<<"������ݹ����....."<<endl;
	printGrammar(G1); 
}

//�÷��ս����FIRST�� ,�ĸ����� ������ ĳ�����ս��  
void constructVNFIRST() {
	
	//����0���ս����first���Ǳ���
	string temp_char;
	for(int i = 0 ; i < G1.vtn ; i++) {
		temp_char = "";
		temp_char += G1.VT[i];
		vn_first.insert(pair<char, string>(G1.VT[i], temp_char));
	} 
	
	//����һ����@�� ����������ս��
	int vn_flage;
	char vn_first_key;
	string vn_first_value = "";
	for(int i = 0 ; i < G1.pn ; i++) {  
		for(int j = 0 ; j < G1.P[i].rn ; j++) { 
			char left_vn_first = G1.P[i].left[0];//�󲿵�һ���ַ� 
			char right_temp_first = G1.P[i].right[j][0];//�Ҳ�ÿ������ʽһ���ַ� 
			vn_flage = 0;//���ս����־ 
			for(int k = 0 ; k < G1.vnn ; k++) {
				if(G1.VN[k] == right_temp_first) {
					vn_flage = 1;
					break;
				}
			}
			if(vn_flage == 0) { //���ս�� 
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
	
	//����� :����ʽ X->Y; X->Y1Y2Y3... 
	int fist_change_flage;
	string temp_value;
	while(1) {
		fist_change_flage = 0;
		for(int i = 0 ; i < G1.pn ; i++) {
			for(int j = 0 ; j < G1.P[i].rn ; j++) {
				vn_flage = 0;
				char left_vn_first = G1.P[i].left[0];//�� 
				char right_temp_first = G1.P[i].right[j][0];//�Ҳ�ĳ������ʽ����λ�� 
				for(int k = 0 ; k < G1.vnn ; k++) {
					if(G1.VN[k] == right_temp_first) {
						vn_flage = 1;   
						break;
					}
				}
				if(vn_flage == 1) { //����Ƿ��ս�� 
					vn_first_key = left_vn_first;//�󲿸�ֵ��key 
					vn_first_iter = vn_first.find(left_vn_first);
					if(vn_first_iter != vn_first.end()) {  //����Ѿ�����map����ֵ 
						temp_value = vn_first_iter->second;
//						vn_first.erase(vn_first_iter);  ���ﲻ����ǰ��ռ�ֵ�� 
					} else {
						temp_value = "";
					}
//					cout<<"��ѯ��map��temp_valueֵ..."<<vn_first_key<<"--"<<temp_value<<endl; 
					vn_first_iter = vn_first.find(right_temp_first);
					if(vn_first_iter == vn_first.end()) {  //���Ҳ����ַ���map�е�ֵ 
						vn_first_value = "";
					} else {
						vn_first_value = vn_first_iter->second;
					}
//					cout<<"��ѯ��map��vn_first_valueֵ..."<<right_temp_first<<"--"<<vn_first_value<<endl; 
					//��vn_first_value���е�temp_valueû�еķ�ֱ�ӷ��Ž�temp_value���ٴ浽map��ȥ 
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
							update_flag = 1;//��Ҫ����ֵ 
						}
					}
					
//					cout<<"��Ҫ���µ�temp_value��ֵ...."<<temp_value<<endl;
					if(update_flag == 1) {//�����Ҫ�ı�map��ֵ ,��temp_value��ֵ���� 
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
		if(fist_change_flage == 0) {//û���ҵ�����Ҫ�ı���� 
			break;  
		}
	}
	
	//������ ��P->P1P2P3...   Pi�ж���@����P�м�@
	for(int i = 0 ; i < G1.pn ; i++) {
		vn_first_key = G1.P[i].left[0];
		vn_first_iter = vn_first.find(vn_first_key);//�󲿵�ֵ 
		temp_value = vn_first_iter->second;
		char temp_flag = 0;
		for(int j = 0 ; j < G1.P[i].rn ; j++) {  //�����Ҳ�����ʽ 
			vn_flage = 1;
			char right_temp_first = G1.P[i].right[j][0];//�Ҳ�ĳ������ʽ�ĵ�һ��λ�õ��ַ� 
			for(int k = 0 ; k < G1.vnn ; k++) {
				if(G1.VN[k] == right_temp_first) {
					vn_flage = 1;   
					break;
				}
			}
			if(vn_flage == 1) { //����Ƿ��ս�� 
				//������ս��û��@�Ͳ����ܸ���
				//�ж��Ƿ���@
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
		if(temp_flag == 1) { //�����Ҫ����@ 
			temp_value += '@';
			vn_first_iter = vn_first.find(vn_first_key);
			vn_first.erase(vn_first_iter);
			vn_first.insert(pair<char, string>(vn_first_key, temp_value));
		}
	}
	cout<<"FIRST��"<<endl;
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

//������ս����FLLOW�� 
void constructVNFOLLOW() {
	string follow_vn_value;//ĳ�����ս����valueֵ 
	char vn;//ĳ�����ս��ֵ 
	char temp_ch;//��ʱ�ַ�
	int vn_flag;//�Ƿ��Ƿ��ս����־ 
	int vt_flag;//�Ƿ����ս����־ 
	int follow_change_flag;//follow���ı�ı�־ 
	
	//����0��S��#
	vn_follow.insert(pair<char, string>(G1.S, "#"));
	
	//����һ A->aBb���ս����������ս��
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
				if(vn_flag == 1 && vt_flag == 1) { //���Ϲ���һ������ 
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
	
	//����� A->aB �� ������ A->aB@
	int update_flag;
	string temp_left_value, temp_right_value;
	while(1) {
		follow_change_flag = 0;
		
		for(int i = 0 ; i < G1.pn ; i++) {
			char temp_left_key = G1.P[i].left[0];
			for(int j = 0 ; j < G1.P[i].rn ; j++) {
				int temp_len = strlen(G1.P[i].right[j]);
				for(int k = 0 ; k < temp_len; k++) {
//					cout<<"��ǰ�ж��ַ�:...."<<G1.P[i].right[j][k]<<endl;
					temp_ch = G1.P[i].right[j][k];
					vn = G1.P[i].right[j][k];
					update_flag = 0;//�Ƿ�Ҫ���µı�־ 
					if(is_vn(temp_ch)) {
						vn_follow_iter = vn_follow.find(temp_ch);
						if(vn_follow_iter != vn_follow.end()) { //���B����follow�� 
							temp_right_value = vn_follow_iter->second;
						} else {
							temp_right_value = "";
						}
						if(k == temp_len - 1) {//��������һ��
							vn_follow_iter = vn_follow.find(temp_left_key);
							if(vn_follow_iter != vn_follow.end()) { //���A����follow�� 
								temp_left_value = vn_follow_iter->second;
							} else {
								temp_left_value = "";
							}
							
//							cout<<"��ֵ����"<<temp_left_value<<"�Ҳ�ֵ..."<<temp_right_value<<endl;
							for(int l = 0 ; l < temp_left_value.length() ; l++) { // �ж��ǲ���Ҫ���� 
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
						} else { //�������һ���ַ�
							string temp_value;
							//string temp_left_value, temp_right_value;
							vn_follow_iter = vn_follow.find(temp_left_key);
							if(vn_follow_iter != vn_follow.end()) { //���A����follow�� 
								temp_left_value = vn_follow_iter->second;
							} else {
								temp_left_value = "";
							}
							char temp_next =  G1.P[i].right[j][k+1];
							if(is_vn(temp_next)) {
								vn_first_iter = vn_first.find(temp_next);
								temp_value = vn_first_iter->second;
//								cout<<"���ս����..."<<temp_next<<" first��ֵ��..."<<temp_value<<endl;
//								cout<<"�����ֵ������"<<temp_ch<<temp_right_value<<endl; 
								//����û��@ 
								char have_empty = 0;
								for(int l = 0 ; l < temp_value.length() ; l++) { // �ж��ǲ���Ҫ���� 
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
									if(vn_follow_iter != vn_follow.end()) { //���A����follow�� 
										temp_left_value = vn_follow_iter->second;
									} else {
										temp_left_value = "";
									}
//									cout<<"�ں���ķ��ս����first������@����ǰ���followֵ"<<temp_left_key<<"--"<<temp_left_value<<endl;
									for(int l = 0 ; l < temp_left_value.length() ; l++) { // �ж��ǲ���Ҫ���� 
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
					
					if(update_flag == 1) { //��Ҫ���� 
						//���²���
						vn_follow_iter = vn_follow.find(vn);
						if(vn_follow_iter != vn_follow.end()) {
							vn_follow.erase(vn_follow_iter);
						}
//						cout<<"���µļ�ֵ��..."<<endl;
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
	
	cout<<"FOLLOW��"<<endl;
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

//���������ѡʽ��FIRST�� 
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
	cout<<"��ѡʽ��FIRST��"<<endl;
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

//�����ַ��鵽��Ӧ�ķ��ս����Ӧ������ 
int findVNCol(char ch) {
	for(int i = 0 ; i < G1.vnn ; i++) {
		if(ch == G1.VN[i]) return i;
	}
}

//�����ַ��鵽��Ӧ���ս����Ӧ������ 
int findVTRow(char ch) {
	if(ch == '#') return G1.vtn - 1;
	for(int i = 0 ; i < G1.vtn ; i++) {
		if(ch == G1.VT[i]) return i;
	}
}

//����Ԥ������� 
void constructPredictionAnalysisTable() {
	string temp_cd, temp_first_value, temp_follow_value, temp_value;
	vt_vn_tols = (G1.vnn * G1.vtn); //�����ܳ� 
    rows = G1.vtn; //�������� 
	int temp;
	char temp_vn, temp_vt;
	int col, row;//���ս����Ӧ������������ʽ��first��ÿ���ս����Ӧ������ 
	for(int i = 0 ; i < G1.pn ; i++) {
		for(int j = 0 ; j < G1.P[i].rn ; j++) {
			temp_cd = G1.P[i].right[j];//�Ҳ�ĳ������ʽ 
			temp_vn = G1.P[i].left[0];//�󲿷��ս�� 
			col = findVNCol(temp_vn); 
			cd_first_iter = cd_first.find(temp_cd);
			temp_first_value = cd_first_iter->second;
			vn_follow_iter = vn_follow.find(temp_vn);
			temp_follow_value = vn_follow_iter->second;
			if(temp_cd == "@") { //��FOLLOW�� 
				for(int k = 0 ; k < temp_follow_value.length() ; k++) {
					temp_value = temp_vn;
					temp_vt = temp_follow_value[k]; 
					row = findVTRow(temp_vt);
					temp = rows * col + row;
					temp_value += "->@";
					M[temp] = temp_value;
				}
			} else {
				for(int k = 0 ; k < temp_first_value.length() && temp_first_value[k] != '@' ; k++) {//������ʽ��FIRST��д������ 
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

//�������ջ
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

//������봮
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

//������� 
void printAction(string s) {
	cout<<"       "<<s<<endl;	
}

//���벽��
void printStep(int s) {
	if(s < 10) {
		cout<<"  "<<s<<"     |";	
	} else if(s < 100) {
		cout<<"  "<<s<<"    |";
	} else if(s < 1000) {
		cout<<"  "<<s<<"   |";
	}
	
} 

//���ִ���
void error(int x) {
	printStep(x);
	printStack();
	printInputStr(0);
	printAction("ƥ�����");
	printf("-------------------------------------------------------------------------\n");
	cout<<"�������ӳ�����, �ķ�G�����Ƴ�����"<<inputStr<<endl;
} 

//ƥ��ɹ�
void success(int x) {
	printStep(x);
	printStack();
	printInputStr(inputStr.length() - 1);
	printAction("�ɹ�������������ֹ");
	printf("-------------------------------------------------------------------------\n");
	cout<<"ƥ��ɹ����ķ�G�����Ƴ�����"<<inputStr<<endl;
} 

//LL1�ķ�ִ��
bool LL1() {
	int step = 1;//���� 
	string temp_value;
	char top, temp;
	as.push('#');
	as.push(G1.S);
	int index , row, col, id;
	index = 0;
	cout<<"LL1�ķ�ƥ�����"<<endl;
	printf("-------------------------------------------------------------------------\n");
	printf("  ����  |        ����ջ        |        ���봮        |        ��ȡ�Ķ���\n");
	printStep(step++);
	printStack();
	printInputStr(index);
	printAction("��ʼ��");
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
		if(is_vt(top)) { //���ջ�����ս�� 
			if(top == temp) {
//				cout<<"�ս��ƥ��------ջ��---"<<top<<"  ���봮---"<<inputStr[index]<<endl;;  
//				cout<<"��ջ����..."<<top<<endl;
				as.pop();
				index++;
				printStep(step++);
				printStack();
				printInputStr(index);
				printAction("ƥ�䣬������һ������");
			} else {
				error(step);
				return false;
			}
		} else { //ջ�������ս�� 
			col = findVNCol(top);
			row = findVTRow(inputStr[index]);
			id = col * rows + row;
			temp_value = M[id];
			if(!temp_value.empty()) {
				if(temp_value[3] == '@') {
//					cout<<"��ջ����..."<<top<<endl;
					as.pop();
					printStep(step++);
					printStack();
					printInputStr(index);
					string ss;
					
					ss += "��";
					ss += top;
					ss += "->@�Զ�ƥ��"; 
					printAction(ss);
				} else {
					as.pop();
					string s_temp;
					for(int j = temp_value.length() - 1 ; j >= 3 ; j--) { //������ջ 
//					cout<<"��ջ����..."<<temp_value[j]<<endl;
						s_temp += temp_value[j];
						as.push(temp_value[j]);
					}
					printStep(step++);
					printStack();
					printInputStr(index);
					string ss;
					ss += "��";
					ss += top;
					ss += "->";
					for(int k = s_temp.length() - 1 ; k >= 0 ; k--) {
						ss += s_temp[k];
					}
					ss += "�Ƶ�"; 
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
	freopen("�ķ�.txt", "r", stdin);
	freopen("���.txt", "w", stdout);
	
	scanf("%d", &T);
	getchar();
	int n = T;
	while(T--) {
		if(T < n-1) cout<<endl<<endl<<endl;
		//cout<<"==================================================================="<<"��"<<n - T<<"�ν���LL1����"<<"=================================================================="<<endl; 
		vn_first.clear();
		vn_follow.clear();
		cd_first.clear();
		while(!temp.empty()) temp.pop();
		while(!as.empty()) as.pop(); 
		memset(G.VN, 0, sizeof(G.VN));
		memset(G.VT, 0, sizeof(G.VT));
		memset(G.P, 0, sizeof(G.P));
		for(int i = 0 ; i < vt_vn_tols ; i++) {  //Ԥ���������� 
			M[i] = "";
		}
		G.pn = 0; 
		G.vtn = 0;
		G.vnn = 0;
		G1 = G; 
		read();
		cout<<"����ķ�G.....\n";
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
