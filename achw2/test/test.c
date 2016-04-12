#include <stdio.h>
#include <stdlib.h>
int main(){
	int a , b , c , d,e,f;
	a = 5;
	b = 6;
	c = a + b;
	d = a + b;
	e = c + c;
	d = 7;
	f = d + d;
	if( c != d ){
		return 1;
	}
	if(5!=5 ){
		return -1;
	}
	
	return 0;
}
