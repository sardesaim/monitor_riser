#ifndef LCD_H_
#define LCD_H_
#include<p18f2550.h>
#include<stdlib.h>
#include<string.h>
//macros
#define RS PORTBbits.RB0		//
#define RW PORTBbits.RB1		//	control lines
#define E PORTBbits.RB2			//
#define dport PORTB
//prototypes
void ms_delay(unsigned int);
void lcd_strobe(void);
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_init(void);
void lcd_gotoxy(int, int);
void lcd_string(char*);
//subroutines
void ms_delay (unsigned int time)
{
	unsigned int i, j;
	for (i = 0; i < time; i++)
		for (j = 0; j < 250; j++);/*Calibrated for a 1 ms delay in MPLAB*/
}

void lcd_strobe(void)
{
	E=1;
	ms_delay(1);
	E=0;
}
void lcd_cmd(unsigned char x)
{ 
	dport&=0x0F;	//clear the upper nibble 
	dport|=(x&0xF0); //write higher byte of command to the upper nibble of PORTB
	RS=0;	//command mode
	RW=0;	//write mode
	lcd_strobe(); //enable high to low pulse (should be at least 450ns long)
 	dport&=0x0F;
	dport|=(x<<4); //write lower byte to the upper nibble of PORTB
 	RS=0;
 	RW=0;
	lcd_strobe();
	ms_delay(5);
} 
void lcd_data(unsigned char x)
{ 
	dport&=0x0F;
	dport|=(x&0xF0);
	RS=1;	//data mode
	RW=0;
	lcd_strobe();
	dport&=0x0F;
	dport|=x<<4;
	RS=1;
	RW=0;
	lcd_strobe();
	ms_delay(5);
}
void lcd_init(void)
{  
	E=0;
	ms_delay(10);	//delay for initialization
	lcd_cmd(0x33);	//
	lcd_cmd(0x32);	//Initialization in 4 bit mode 
	lcd_cmd(0x28);	//
	lcd_cmd(0x01);	//clear screen
	lcd_cmd(0x0E);	//cursor on display on
	lcd_cmd(0x80);	//move to position 1,1
	lcd_cmd(0x06);	//Entry mode
}

void lcd_string(char *str)	//characters in string stored in string pointer and retreived one by one
{   	
	unsigned char i=0;
 	while(str[i]!='\0')
	{
		lcd_data(str[i]);
		i++;
	}
}


void lcd_gotoxy(int x, int y)	//goto position (x,y) 
{
	int i;
	if(x==1)
	{
		lcd_cmd(0x80);
		ms_delay(1);
		for(i=1;i<y;i++)
		{
			lcd_cmd(0x14);	//move cursor to right by one character
		}
	}
	if(x==2)
	{
		lcd_cmd(0xC0);
		ms_delay(1);
		for(i=1;i<y;i++)
		{
			lcd_cmd(0x14);	
		}
	}
}
#endif