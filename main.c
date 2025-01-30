/*
 * LAB2.c
 *
 * Created: 30/01/2025 01:44:35
 * Author : alane
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "LCD8bits/LCD8bits.h"
#include "ADC/ADC.h"

// UART Baud rate
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

// Variables globales
uint8_t Val1 = 0, Val2 = 0, caso = 0;
char lista1[8], lista2[8], lista3[4] = {'0', '0', '0', '0'};
volatile uint8_t contador = 0;
volatile uint8_t updateLCD = 0; // Variable de estado para actualizar la LCD
volatile uint8_t adcUpdated = 0; // Variable de estado para actualizar valores ADC

// Prototipos de funciones
void setup(void);
void actualizarVoltaje(char *lista, uint8_t valor);

void actualizarLista(char *lista, int valor);
void actualizarLCD(void);

// Configuración inicial del sistema
void setup(void) {
	cli();  // Deshabilitar interrupciones globales
	DDRD = 0xFF;  // Puerto D como salida
	DDRB = 0xFF;  // Puerto B como salida
	DDRC = 0;     // Puerto C como entrada
	
	
	Lcd_Init8bits();  // Inicializar pantalla LCD
	_delay_ms(50);    // Esperar para asegurarse de que la LCD se inicialice correctamente
	Lcd_Clear();      // Limpiar pantalla LCD
	initADC();  // Inicializar ADC
	ADCSRA |= (1 << ADSC);  // Iniciar la primera conversión ADC

	sei();  // Habilitar interrupciones globales
}

// Función para convertir un valor ADC a voltaje y actualizar una cadena
void actualizarVoltaje(char *lista, uint8_t valor) {
	float voltaje = (valor * 5.0) / 255.0;
	uint16_t int_part = (uint16_t)voltaje;
	uint16_t dec_part = (uint16_t)((voltaje - int_part) * 100);  // Dos decimales

	if (int_part < 10) {
		lista[0] = '0' + int_part;
		lista[1] = '.';
		lista[2] = '0' + (dec_part / 10);
		lista[3] = '0' + (dec_part % 10);
		lista[4] = 'V';
		lista[5] = '\0';
		} else {
		lista[0] = '0' + (int_part / 10);
		lista[1] = '0' + (int_part % 10);
		lista[2] = '.';
		lista[3] = '0' + (dec_part / 10);
		lista[4] = '0' + (dec_part % 10);
		lista[5] = 'V';
		lista[6] = '\0';
	}
}

// Función para actualizar una cadena con un valor de 3 dígitos
void actualizarLista(char *lista, int valor) {
	lista[0] = '0' + (valor / 100);
	lista[1] = '0' + ((valor / 10) % 10);
	lista[2] = '0' + (valor % 10);
	lista[3] = '\0';
}


// Función para actualizar la pantalla LCD
void actualizarLCD(void) {
	Lcd_Clear();  // Limpiar pantalla
	Lcd_Set_Cursor(0, 1);
	Lcd_Write_String("S1:");  // Escribir etiqueta de Sensor 1
	Lcd_Set_Cursor(0, 7);
	Lcd_Write_String("S2:");  // Escribir etiqueta de Sensor 2
	Lcd_Set_Cursor(0, 13);
	Lcd_Write_String("S3:");  // Escribir etiqueta de Sensor 3
	// Actualizar las cadenas con los valores actuales
	actualizarVoltaje(lista1, Val1);
	actualizarVoltaje(lista2, Val2);
	actualizarLista(lista3, contador);

	// Mostrar los valores en la LCD
	Lcd_Set_Cursor(1, 1);
	Lcd_Write_String(lista1);
	Lcd_Set_Cursor(1, 7);
	Lcd_Write_String(lista2);
	Lcd_Set_Cursor(1, 13);
	Lcd_Write_String(lista3);
}

// Función principal
int main(void) {
	setup();  // Configuración inicial del sistema
	
	// Variables para guardar los valores anteriores
	uint8_t prevVal1 = 255, prevVal2 = 255, prevContador = 255;

	while (1) {
		// Verificar si hay cambios en los valores del ADC o el contador
		if ((Val1 != prevVal1) || (Val2 != prevVal2) || (contador != prevContador) || updateLCD) {
			actualizarLCD();  // Actualizar la pantalla LCD
			// Guardar los valores actuales como anteriores
			prevVal1 = Val1;
			prevVal2 = Val2;
			prevContador = contador;
			adcUpdated = 0;
			updateLCD = 0;
		}

		// Actualizar las cadenas con los valores actuales
		actualizarVoltaje(lista1, Val1);
		actualizarVoltaje(lista2, Val2);
		actualizarLista(lista3, contador);
		
		_delay_ms(100);  // Esperar 100ms
	}
}

// Interrupción del ADC
ISR(ADC_vect) {
	if (caso == 0) {
		ADMUX &= ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0)); // Seleccionar canal ADC0
		Val1 = ADCH;  // Leer valor alto del ADC
		caso = 1;  // Cambiar a caso 1
		} else {
		ADMUX = (ADMUX & ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0))) | (1 << MUX0); // Seleccionar canal ADC1
		Val2 = ADCH;  // Leer valor alto del ADC
		caso = 0;  // Cambiar a caso 0
	}
	ADCSRA |= (1 << ADSC);  // Iniciar la próxima conversión ADC
	adcUpdated = 1; // Indicar que se debe actualizar la LCD
}

