int a,b,c;
float m,n;
int fibo(int a){
	int x,y;
	y=0;
	x=1;
	while(x<=a){
		y=y+x;
		x=x+1;
	}	
	return y;
}
int main(){
	int n,i,s;
	float m;
	s=read();
	i=1;
	n=fibo(s);
	write(n);
	return 0;//返回类型不匹配
}
