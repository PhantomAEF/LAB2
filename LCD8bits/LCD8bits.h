/*
 * LCD8bits.h
 *
 * Created: 30/01/2025 01:46:31
 *  Author: alane
 */ 


#ifndef LCD8BITS_H_
#define LCD8BITS_H_

#include <avr/io.h>
#include <stdint.h>
#define F_CPU 16000000
#include <util/delay.h>


void Lcd_Init8bits();

void inicio(char a);    //Función de repeticion de comandos

void dato_a_mostrar(char a);

void Lcd_Clear();

void Lcd_Set_Cursor(char a, char b);

void Lcd_Write_Char(char data);

void Lcd_Write_String(char *a);

void Lcd_Shift_Right();

void Lcd_Shift_Left();



#endif /* LCD8BITS_H_ */