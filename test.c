#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

unsigned char str2hex(char *str)
{
    int i;
    unsigned char temp[2], hex;
    for (i = 2; str[i] != '\0'; i++)
    {
        if ((str[i] >= '0') && (str[i] <= '9'))
        {
            temp[i - 2] = str[i] - 0x30; // 字符0为0x30，‘0’-‘0’=0x0 ，'9'-'0'=0x9
        }
        else if ((str[i] >= 'A') && (str[i] <= 'F'))
        {
            temp[i - 2] = str[i] - 0X37; // 字符'A'为0x41，'A'-(0X37)=0xa
        }
        else if ((str[i] >= 'a') && (str[i] <= 'f'))
        {
            temp[i - 2] = str[i] - 0X57; // 字符'a'为0x61，'a'-(0X57)=0xa
        }
    }

    return hex = (temp[0] << 4) | temp[1];
}

int main()
{
    unsigned char head[] = {0xaa};        // 协议头
    short unsigned int Checksum = 0x00AA; // 16位校验码
    char str[100];
    char arr[100];
    int j = 0;
    int a1 = 0x01, a2 = 0x02, a3 = 0x03; // 代表命令类型
    FILE *pf = fopen("test.txt", "r");
    FILE *pf1 = fopen("test1.txt", "w");
    FILE *pf2 = fopen("lcd.bin", "wb");
    // 判断是否打开成功
    if (pf == NULL)
    {
        perror("fopen");
        return 1;
    }
    // 写入协议头
    fwrite(head, sizeof(char), 1, pf2);

    // 抓取命令
    while (fgets(str, 100, pf) != NULL)
    {
        memset(arr, 0, sizeof(arr));
        printf("%s", str);
        j = 0;
        if (strstr(str, "_cmd(") != NULL)
        {

            fputs("1", pf1);
            int len = strlen(str);
            for (int i = 0; i < len; i++)
            {
                if (i >= 17 && i < 21)
                {
                    arr[j++] = str[i];
                }
            }
            printf("%s\n", arr);
            unsigned char hex = str2hex(arr);
            printf("0x%x\n", hex);
            // 写入二进制bin文件
            fwrite(&a1, sizeof(char), 1, pf2);
            Checksum += a1;
            fwrite(&hex, sizeof(char), 1, pf2);
            Checksum += hex;
            fputs(arr, pf1);
            fputs("\n", pf1);
            j = 0;
        }
        // 抓取数据
        if (strstr(str, "_data(") != NULL)
        {
            memset(arr, 0, sizeof(arr));
            j = 0;
            fputs("2", pf1);
            int len = strlen(str);
            for (int i = 0; i < len; i++)
            {
                if (i >= 18 && i <= 21)
                {
                    arr[j++] = str[i];
                }
            }
            unsigned char hex = str2hex(arr);
            printf("0x%x\n", hex);
            // 写入二进制bin文件
            fwrite(&a2, sizeof(char), 1, pf2);
            Checksum += a2;
            fwrite(&hex, sizeof(char), 1, pf2);
            Checksum += hex;
            fputs(arr, pf1);
            fputs("\n", pf1);
            j = 0;
        }
        // 抓取延时时间
        if (strstr(str, "delay_ms(") != NULL)
        {
            memset(arr, 0, sizeof(arr));
            fputs("3", pf1);
            int len = strlen(str);
            printf("%d\n", len);
            for (int i = 0; i < len; i++)
            {
                if (isdigit(str[i]))
                {
                    arr[j++] = str[i];
                }
            }
            printf("%s\n", arr);
            int delay_num = atoi(arr);
            printf("%d\n", delay_num);
            // 写入二进制bin文件
            fwrite(&a3, sizeof(char), 1, pf2);
            Checksum += a3;
            if (delay_num < 255)
            {
                // 写入二进制bin文件
                fwrite(&delay_num, sizeof(char), 1, pf2);
                Checksum += delay_num;
            }
            else
            {
                printf("延时时间过长！\n");
            }
            fputs(arr, pf1);
            fputs("\n", pf1);
            j = 0;
        }
    }
    printf("校验和为0x%X\n", Checksum);

    // 关闭
    pf = NULL;
    pf1 = NULL;
    pf2 = NULL;
    fclose(pf2);
    fclose(pf1);
    fclose(pf);

    return 0;
}
