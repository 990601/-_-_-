int a,b,c;
float m,n;
int fibo(int a){
	if(a==1 || a==2) {
		return 1;
	}
	return fibo(a-1)+fibo(a-2);
}
int main(){
	int n,i,s;
	float m;
	s=read();
	i=1;
	while(i<=s){
		n=fibo(i);
		write(n);
		i=i+1;
	}
	//下边测试自己新增的数据
	i++;
	++i;
	i--;
	--i;
	i+=6;
	i-=n;
	for(i=3;i<0;i--){
		int x;
		x=fibo(i);
		write(x);
		x+=1;
	}
	return 0;//返回类型不匹配
}
