#include<stdio.h>
#include<stdint.h>
int main(){
    uint32_t a = 255;
    uint32_t b = (uint32_t)a<<8;
    printf("%d",b);
}