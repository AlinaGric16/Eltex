#include <stdio.h>

void integer_to_binary(int);
void number_of_ones(int);
int replacement_of_third_byte(int integer, int byte);

int main(){
    int num = 0, byte = 0;

    printf("Part 1. Enter a positive integer.\n");
    scanf("%d", &num);
    printf("Binary representation: ");
    integer_to_binary(num);

    printf("Part 2. Enter a negative integer.\n");
    scanf("%d", &num);
    printf("Binary representation: ");
    integer_to_binary(num);

    printf("Part 3. Enter a positive integer.\n");
    scanf("%d", &num);
    printf("Number of ones: ");
    number_of_ones(num);

    printf("Part 4. Enter a positive integer.\n");
    scanf("%d", &num);
    printf("Enter a positive integer <= 255.\n");
    scanf("%d", &byte);
    num = replacement_of_third_byte(num, byte);
    printf("Binary representation: ");
    integer_to_binary(num);

    return 0;
}

void integer_to_binary(int integer){
    int bit = 0;
    for (int i = sizeof(integer) * 8 - 1; i >= 0; i--){
        bit = (integer >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

void number_of_ones(int integer){
    int bit = 0, count = 0;
    for (int i = sizeof(integer) * 8 - 1; i >= 0; i--){
        bit = (integer >> i) & 1;
        if (bit == 1) count++;
    }
    printf("%d\n", count);
}

int replacement_of_third_byte(int integer, int byte){
    byte = byte << 2*8;
    int mask = byte | 0xFF00FFFF;
    integer = (integer & mask) | byte;
    return integer;
}