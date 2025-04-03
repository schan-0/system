#include <stdio.h>
#include <ctype.h>


int main()
{
	char input;
	char tmp;

	while(1)
	{
			
		printf("문자 입력 : ");
		input = getchar();
		while ((tmp = getchar()) != '\n');
		if (input == '0')
			return 1;
		else if (isupper(input))
			printf("%c의 소문자는 %c입니다.\n", input, tolower(input));	
		else if (islower(input))
			printf("%c의 대문자는 %c입니다.\n", input, toupper(input));
		else
			printf("유효한 입력이 아닙니다.\n");
	}
	return 0;
}

