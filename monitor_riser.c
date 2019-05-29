#include<p18f2550.h>
#include<stdlib.h>
#include "lcd.h"
//macros
//inputs
#define enter PORTCbits.RC0
#define up PORTCbits.RC1
#define down PORTCbits.RC2
#define ack PORTCbits.RC4
#define irl PORTCbits.RC7 
#define irm PORTAbits.RA5
#define irr PORTCbits.RC6
#define ls1	PORTAbits.RA3
#define ls2 PORTAbits.RA4
//outputs
#define motorpin1 PORTAbits.RA0
#define motorpin2 PORTAbits.RA1
#define ledpin PORTAbits.RA2

//global variables
unsigned int mode; //keep a track of mode
//strings to display on LCD
char Scroll[17]="Scroll to change";
char mode1[17]= "Manual mode";
char mode2[17]="Auto mode";
char mode3[17]="Timer mode";
char sitmin[17] = "Sit period";
char standmin[17] = "Stand period";
//sit and stand durations
unsigned int sit=0;
unsigned int stand=0;
//prototypes
void mode_sel(void);
void manual_mode(void);
void auto_mode(void);
void times_mode(void);
void delay_sec(unsigned int);
void delay_min(unsigned int);
void input_sit(void);
void input_stand(void);
void check_ack(void);
//subroutines
void main() 
{
	ADCON1=0x0F;
	UCONbits.USBEN=0; /*Use RC4, RC5 as normal inputs, disable USB module*/
	UCFGbits.UTRDIS=1; /*Disable USB transceiver*/
	TRISC=0xFF;
	TRISB=0;
	TRISA=0x38;
	motorpin1=0;
	motorpin2=0;
	//TXSTAbits.TXEN=0;
	//RCSTAbits.SPEN=0;
	ledpin=0;
	lcd_init();
	mode_sel();
	ms_delay(100);
}

void delay_sec(unsigned int sec)
{
	ms_delay(sec*1000);
}
void delay_min(unsigned int min)
{
	delay_sec(min*60);
}
void mode_sel(void)
{
	char modes[];
	lcd_cmd(0x01);
	lcd_string(mode1);	
	lcd_cmd(0xc0);
	lcd_string(Scroll);		
	mode=0;
	while(enter==0)
	{
		ms_delay(100);
		if(up==1&&down==0)
		{
			ms_delay(100);
			if(mode==3)
				mode=0;
			else
				if(mode==1)
				mode=mode+2;
				else
				mode=mode+1;
			lcd_cmd(0x01);
			if(mode==0)
				lcd_string(mode1);
			else if(mode==1)
				lcd_string(mode2);
			else if(mode==2||mode==3)
				lcd_string(mode3);
			lcd_cmd(0xc0);
			lcd_string(Scroll);
		}
		ms_delay(100);
		if(up==0&&down==1)
		{
			ms_delay(100);
			if(mode==0)
				mode=3;
			else
				if(mode==3)
				mode=mode-2;
				else
				mode=mode-1;
			lcd_cmd(0x01);
			if(mode==0)
				lcd_string(mode1);
			else if(mode==1)
				lcd_string(mode2);
			else if(mode==2||mode==3)
				lcd_string(mode3);
			lcd_cmd(0xc0);
			lcd_string(Scroll);
		}
	}
	ms_delay(100);
	if(mode==0)
		manual_mode();
	else if(mode==1)
		auto_mode();
	else if(mode==3)
		times_mode();
	return;
}
void manual_mode(void)
{
	lcd_cmd(0x01);
	lcd_string(mode1);
	while(1)
	{
		while(up==1&&down==0&&ack==0&&enter==0)
		{
			if(ls1==0)
			{
				motorpin1=0;
				motorpin2=1;
			}
			else
			{
				motorpin1=0;
				motorpin2=0;
			}
		}
		while(up==0&&down==1&&ack==0&&enter==0)
		{
			if(ls2==0)
			{
				motorpin1=1;
				motorpin2=0;
			}
			else
			{
				motorpin1=0;
				motorpin2=0;
			}
		}
		while(up==0&&down==0&&ack==0&&enter==0)
		{
			motorpin1=0;
			motorpin2=0;
		}
	}
}
void auto_mode(void)
{
	lcd_cmd(0x01);
	lcd_string(mode2);
	while(1)
	{
		if(irm==1 && irl==1 && irr==1 && ls2==0)
		{
			motorpin1=1;
			motorpin2=0;
		}
		else if(irm==0 && irl==0 && irr==0 && ls1==0)
		{
			motorpin1=0;
			motorpin2=1;
		}
		else
		{
			motorpin1=0;
			motorpin2=0;
		}
	}	
}
void times_mode(void)
{
	lcd_cmd(0x01);
	ledpin=0;
	lcd_string(mode3);
	input_sit();
	ms_delay(150);
	input_stand();
	ms_delay(150);
	lcd_cmd(0x01);
	while(1)
	{
		delay_sec(sit);
		check_ack();
		while(ls1!=1)
		{
			ms_delay(50);
			motorpin1=0;
			motorpin2=1;	
		}	
		ledpin=0;
		motorpin1=0;
		motorpin2=0;	
		delay_sec(stand);
		check_ack();
		while(ls2!=1)
		{
			ms_delay(50);
			motorpin1=1;
			motorpin2=0;		
		}
		ledpin=0;
		motorpin1=0;
		motorpin2=0;	
	}
}

void input_sit(void)	//accept sitting period
{
	char sits[3];
	lcd_cmd(0x01);
	lcd_string(sitmin);
	lcd_gotoxy(2,1);
	while(enter!=1)
	{
		ms_delay(150);
		if(enter==0)
		{
			ms_delay(150);
			while((enter==0)&&(up==0)&&(down==0))
			{
				ms_delay(100);
				if(up==1)
				{
					sit+=5;
					itoa(sit,sits);
					lcd_string(sits);
				}
				else if(down==1)
				{
					sit-=5;
					itoa(sit,sits);
					lcd_string(sits);
				}
			}
			lcd_cmd(0xC0);
		}
		else if(enter==1)
		{
			ms_delay(300);
			break;
		}
	}
	return;
}

void input_stand(void)	//accept standing period
{
	char stands[3];
	lcd_cmd(0x01);
	lcd_cmd(0x80);
	lcd_string(standmin); 
	lcd_gotoxy(2,1);
	while(enter!=1)
	{
		ms_delay(150);
		if(enter==0)
		{
			ms_delay(150);
			while((enter==0)&&(up==0)&&(down==0))
			{	
				ms_delay(100);
				if(up==1)
				{
					stand+=5;
					itoa(stand,stands);
					lcd_string(stands);
				}
				else if(down==1)
				{
					sit-=5;
					itoa(stand,stands);
					lcd_string(stands);
				}
			}
			lcd_cmd(0xC0);
		}
		else if(enter==1)
		{
			ms_delay(300);
			break;
		}
	}
	return;
}

void check_ack(void)	//check if user has acknowledged movement of the setup
{
	while(ack!=1)
	{
		ms_delay(100);
		if(ack==0)
			ledpin=1;
	}	
	ledpin=0;
	return;
}