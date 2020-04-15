/*
 * ATMega16Door.c
 *
 * Created: 14-04-2020 13:50:41
 * Author : Stephen Kingston
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 11059200UL

volatile uint8_t closed;
volatile uint8_t open;
volatile uint8_t proximity;

void ProgramInit();
void closeDoor();
void openDoor();
void stopMotor();
void readLimitSwitchStates();
void readProximity();

int main(void)
{
	ProgramInit();
	while(1)
	{
		if (proximity == 1 && open == 0)
			openDoor();
		if (proximity == 0 && closed == 0)
			closeDoor();
	}
}

ISR(INT0_vect)	//Interrupt when door fully closed - Rising edge trigger
{
	closed = 1;
	open = 0;
	stopMotor();
};

ISR(INT1_vect)  //Trigger on any change
{
	readProximity();
	readLimitSwitchStates();
};

ISR(INT2_vect) //Interrupt when door fully open - Rising edge trigger
{
	open = 1;
	closed = 0;
	stopMotor();
};

void ProgramInit()
{
	//Set Data Direction for ports B and D for outputs and inputs
	DDRB = ~(1 << PB2); 
	DDRD = (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);
	
	//Get initial values
	readLimitSwitchStates();
	readProximity();

	sei(); //Set Interrupt Enable SREG = 1
	
	//Enabling all three interrupts
	GICR |= (1 << INT1) | (1 << INT0) | (1 << INT2);
	
	//Setting interrupt modes
	
	MCUCSR |= (1 << ISC2); // Interrupt 2 is rising edge triggered
	MCUCR |= (1 << ISC00) | (1 << ISC01); // Interrupt 0 is rising edge triggered
	MCUCR |= (1 << ISC10);	//Interrupt 1 is triggered on any change in logic level
}

void closeDoor()
{
	PORTD |= (1 << PD4);
	PORTD |= (1 << PD6);
			
	PORTD &= ~(1 << PD7);
	PORTD &= ~(1 << PD5);
	readLimitSwitchStates();
}

void stopMotor()
{
	PORTD &= ~(1 << PD7);
	PORTD &= ~(1 << PD5);
			
	PORTD &= ~(1 << PD4);
	PORTD &= ~(1 << PD6);
	readLimitSwitchStates();
}

void openDoor()
{
	PORTD |= (1 << PD7);
	PORTD |= (1 << PD5);
				
	PORTD &= ~(1 << PD4);
	PORTD &= ~(1 << PD6);
	readLimitSwitchStates();
}

//Reading state of limit switches and proximity sensor
void readLimitSwitchStates()
{
		if ((PIND & (1 << PD2)))
			closed = 1;
		else
			closed = 0;
		
		if ((PINB & (1 << PB2)))
			open = 1;
		else
			open = 0;
}

//Read proximity sensor to ascertain whether a person is nearby
void readProximity()
{
	if ((PIND & (1 << PD3)))
		proximity = 1;
	else
		proximity = 0;
}