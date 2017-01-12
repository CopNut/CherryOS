#include <stdio.h>

size_t strlen(const char *str){

    const char *char_ptr;
    const ulong *longword_ptr;
    register ulong longword, himagic, lomagic;

    for (char_ptr = str; ((ulong)char_ptr & (sizeof(ulong) - 1)) != 0; ++char_ptr) {
        if (*char_ptr == '\0')
            return char_ptr - str;
    }

    longword_ptr = (ulong*)char_ptr;

    himagic = 0x80808080L;
    lomagic = 0x01010101L;

    while (1) {
        longword = *longword_ptr++;
        if (((longword - lomagic) & himagic) != 0) {
            const char *cp = (const char*)(longword_ptr - 1);    
            if (cp[0] == 0)
                return cp - str;
            if (cp[1] == 0)
                return cp - str + 1;
            if (cp[2] == 0)
                return cp - str + 2;
            if (cp[3] == 0)
                return cp - str + 3;
        }
    }
}

char *strcpy(char *dst,const char *src){  
 
    char *ret = dst;  
    memcpy(dst, src, strlen(src)+1);  
    return ret;  
}

void memset(void *dst, char src, size_t count){
    while(count--){
        *(char *)dst = src;
        dst = (char *)dst + 1;
    }
}

void *memcpy(void *dst, const void *src, size_t count){

     void * ret = dst;  
     if (dst <= src || (char *)dst >= ((char *)src + count))//源地址和目的地址不重叠，低字节向高字节拷贝  
     {  
         while(count--)  
         {  
             *(char *)dst = *(char *)src;  
             dst = (char *)dst + 1;  
             src = (char *)src + 1;  
         }  
     }  
     else                       //源地址和目的地址重叠，高字节向低字节拷贝  
     {   
         dst = (char *)dst + count - 1;  
         src = (char *)src + count - 1;   
         while(count--)   
         {  
             *(char *)dst = *(char *)src;  
             dst = (char *)dst - 1;  
             src = (char *)src - 1;  
         }  
    }  
    return ret;  
}  


size_t sprintf(char* buffer,const char* format,...)
{
    va_list arg_ptr;
    unsigned int buffer_length=strlen(buffer);
    unsigned int format_length=strlen(format);
    unsigned int index=0;
     
    int temp;
    char ctemp;
    char* stemp;
     
    va_start(arg_ptr,format);
    for(index=0;index<format_length;++index)
    {
        if(format[index]!='%')
            (*buffer++)=format[index];
        else
        {
            switch(format[index+1])
            {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':


                case 'd':
                    temp=va_arg(arg_ptr,int);
                    buffer=buffer+write_dec(buffer,temp);
                break;

                case 'x':
                    temp=va_arg(arg_ptr,int);
                    buffer=buffer+write_hex(buffer,temp);
                break;

                case 's':
                     stemp=(char*)va_arg(arg_ptr,char*);
                     strcpy(buffer,stemp);
                     buffer+=strlen(stemp);
                break;

                case 'c':
                    ctemp=va_arg(arg_ptr,int);
                    *(buffer++)=ctemp;
                break;
            }
            index++;
        }
    }
    *buffer=0;
    va_end(arg_ptr);
    return --index;
}

unsigned int write_dec(char* buffer,int value){
    unsigned char stack[10];
    unsigned char length=0;
    unsigned int _temp = 0;
    if(value<0){
        *(buffer++)='-';
        value*=-1;
        _temp++;
    }
    do
    {
        stack[length]='0'+(value%10);
        value/=10;
        length++;
    }while(value);
     
    _temp+=length;
    while(length)
    {
        *buffer=stack[length-1];
        length--;
        buffer++;
    }
    return _temp;
}

unsigned int write_hex(char* buffer,int value){
    unsigned char stack[8];
    unsigned char length=0;
    unsigned int mask = 0x0000000f;
    unsigned int _temp = 0;
    if(value<0){
        *(buffer++)='-';
        value*=-1;
        _temp++;
    }
    do
    {
        stack[length] = (value & mask) < 10 ? (value & mask) + '0' : (value & mask) - 10 + 'A';
        value = value >> 4;
        length++;
    }while(value);
     
    _temp+=length;
    while(length)
    {
        *buffer=stack[length-1];
        length--;
        buffer++;
    }
    return _temp;
}

int atoi_dec(const uchar *string){

    int x = 0;
    int sign;
    uchar *p;

    if (string[0] == '-')
    {
        sign = -1;
        p = string + 1;
    }
    else if (string[0] == '+')
    {
        sign = 1;
        p = string + 1;
    }
    else
    {
        sign = 1;
        p = string;
    }

    for (; p[0] != 0; ++p)
    {
        //debug
        if(p[0] < '0' || p[0] > '9'){
            //invalide input
            return -1;
        }
        //debug
        
        x *= 10;
        x += p[0] - '0';
    }

    return sign * x;
}

int atoi_hex(const uchar *string){

    int x = 0;
    int sign;
    uchar *p;

    if (string[0] == '-')
    {
        sign = -1;
        p = string + 1;
    }
    else if (string[0] == '+')
    {
        sign = 1;
        p = string + 1;
    }
    else
    {
        sign = 1;
        p = string;
    }

    for (; p[0] != 0; ++p)
    {
        
        if (p[0] >= '0' && p[0] <= '9')
        {
            x <<= 4;
            x += p[0] - '0';
        }
        else if (p[0] >= 'a' && p[0] <= 'f')
        {
            x <<= 4;
            x += p[0] - 'a' + 10;
        }
        else if (p[0] >= 'A' && p[0] <= 'F')
        {
            x <<= 4;
            x += p[0] - 'A' + 10;
        }
        else
        {
            return -1;
        }
        
    }

    return sign * x;
}