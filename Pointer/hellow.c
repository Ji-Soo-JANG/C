#include <stdio.h>

int main() {
	int* p1;
	int** p2 = &p1;
	int num = 3;
	p1 = &num;
	printf("num은 %d입니다.", num);
	printf("c의 주소값은%p", &num);
	printf("p1의 값은 %d입니다.", *p1);
	printf("p1의 주소값은 %p\n", p2);
	printf("p2의 값은 %d\n", **p2);
	
	int arr[3];
	int len = sizeof(arr) / sizeof(int);
	for (int i = 0; i < len; i++) {
		arr[i] = i;
	}

	for (int i = 0; i < len; i++) {
		printf("%d", arr[i]);
	}




}
