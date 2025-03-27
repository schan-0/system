#include <stdio.h>

int main(void)
{
	char name[100] = {};
	int id = 0;
	printf("이름을 입력해주세요 : ");
	scanf("%s", &name);
	printf("\n");
	printf("학번을 입력해주세요 : ");
	scanf("%d", &id);
	printf("\n");
	printf("당신의 이름과 학번은 %s_%d 입니다.\n", name, id);
	return 0;
}
