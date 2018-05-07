#include <stdint.h>
#include <const.h>
void putchar(uint8_t attr, char s);
void puts(uint8_t attr, char *s);
char *itoa(int num, char *str, int show_mode)
{
    int n;
    char p[32];
    char *q = str, *q1 = p;

    if (show_mode == HEX)
    {
        *q++ = '0';
        *q++ = 'x';
    }
    if (!num)
        *q++ = '0';
    else
        while (num)
        {
            *q1++ = (num % show_mode + (num % show_mode > 9 ? 'A' - 10 : '0'));
            num /= show_mode;
        }
    while (q1 != p)
        *q++ = *(--q1);
    *q++ = '\0';
    return str;
}
void NL()
{
    putchar(0, '\n');
}
void putNum(int num){
    char s[40];
    puts(0x04,itoa(num,s,HEX));
}