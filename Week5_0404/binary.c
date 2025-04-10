#include <stdio.h>
#include <stdlib.h>

// 정수를 2진수 문자열로 변환하는 함수
char* decToBinary(int n) {
    char* binaryString = (char*)malloc(9 * sizeof(char)); // 8비트 + 널 문자
    if (binaryString == NULL) {
        perror("메모리 할당 오류");
        exit(EXIT_FAILURE);
    }
    for (int i = 7; i >= 0; i--) {
        binaryString[7 - i] = (n & (1 << i)) ? '1' : '0';
    }
    binaryString[8] = '\0';
    return binaryString;
}

// 2진수 문자열에서 1의 개수를 세는 함수
int countSetBits(const char* binaryString) {
    int count = 0;
    for (int i = 0; binaryString[i] != '\0'; i++) {
        if (binaryString[i] == '1') {
            count++;
        }
    }
    return count;
}

// 2진수 문자열에서 상위 4비트를 추출하는 함수
char* getUpper4Bits(const char* binaryString) {
    char* upper4 = (char*)malloc(5 * sizeof(char)); // 4비트 + 널 문자
    if (upper4 == NULL) {
        perror("메모리 할당 오류");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 4; i++) {
        upper4[i] = binaryString[i];
    }
    upper4[4] = '\0';
    return upper4;
}

int main() {
    int num;

    printf("0부터 255 사이의 정수를 입력하세요: ");
    if (scanf("%d", &num) != 1) {
        printf("잘못된 입력입니다.\n");
        return 1;
    }

    if (num < 0 || num > 255) {
        printf("입력 값의 범위가 벗어났습니다.\n");
        return 1;
    }

    // 1. 입력 받은 값의 2진수 값 A
    char* binaryA = decToBinary(num);
    printf("입력 값의 2진수 값 A: %s\n", binaryA);

    // 2. A가 가지는 1의 개수
    int oneCount = countSetBits(binaryA);
    printf("A가 가지는 1의 개수: %d\n", oneCount);

    // 3. A의 상위 4비트
    char* upper4Bits = getUpper4Bits(binaryA);
    printf("A의 상위 4비트: %s\n", upper4Bits);

    // 동적 할당된 메모리 해제
    free(binaryA);
    free(upper4Bits);

    return 0;
}
