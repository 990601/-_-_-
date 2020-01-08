int a,b,c;
float m,n;
int a;//18  变量重复定义
int fibo(int a){
	if(a==-1 || a==2) {
		int x;
		return 1;
	}
	return fibo(a-1)+fibo(a-2);
}
int fibo(float s){//15  函数重复定义
	return 0;
}
int temp(float a,int a){//14  参数重复定义
	int s;
	int t;
	return 0;
}
int main(){
	int a;
	int n,i,s;
	float m;
	p=0;//6  未定义变量引用
	i=m;//9 类型不匹配
	i=1.2;//9  类型不匹配
	s=read();
	s=test();//16  函数未定义测试
	i=6;
	i+3=2;//29  赋值语句左边
	m++;//13 ++--对象类型错误
	continue;//3  continue出现在错误位置
	break;//4  break出现在错误位置
	while(i<=m){
		n=fibo(i);
		n=fibo(m);//2  参数类型不匹配
		n=fibo(i,a);//1  参数数量不匹配
		n=fibo();//1 参数数量不匹配
		n=fibo;//7  fibo是函数名，误作变量使用
		n=a();//17  不是一个函数
		write(n);
		i=i+1;
		if(i==2)break;
	}
	
	if(!i) return 0;
	return 1.2;//5  返回类型不匹配
}
