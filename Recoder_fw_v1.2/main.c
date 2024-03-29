// YLE-Synth-Recoder
// Recoder Firmware
// For ATmega8, 2MHz clock
// 18/01/31 Eleken rev_c1815@yahoo.co.jp

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei(), timer0 */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/eeprom.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */

#include <stdint.h>

uint8_t g_freqList[2048] PROGMEM = 
{
146, 185, 185, 138, 154, 207, 207, 123, 146, 196, 196, 130, 138, 220, 220, 110, 
146, 190, 190, 134, 143, 213, 213, 116, 156, 201, 201, 123, 130, 196, 196, 98, 
146, 187, 187, 136, 149, 210, 210, 119, 151, 198, 198, 127, 134, 207, 207, 106, 
146, 193, 193, 131, 138, 216, 216, 112, 162, 204, 204, 116, 174, 174, 174, 87, 
146, 186, 186, 137, 152, 209, 209, 121, 149, 197, 197, 128, 136, 213, 213, 108, 
146, 191, 191, 133, 141, 215, 215, 114, 159, 203, 203, 120, 151, 186, 186, 95, 
146, 189, 189, 135, 146, 212, 212, 118, 154, 200, 200, 125, 132, 201, 201, 104, 
146, 194, 194, 130, 138, 218, 218, 110, 164, 206, 206, 141, 164, 164, 164, 82, 
146, 185, 185, 138, 153, 208, 208, 122, 148, 196, 196, 129, 137, 216, 216, 109, 
146, 191, 191, 133, 142, 214, 214, 115, 158, 202, 202, 121, 140, 191, 191, 96, 
146, 188, 188, 135, 147, 211, 211, 119, 153, 199, 199, 126, 133, 204, 204, 105, 
146, 193, 193, 131, 138, 217, 217, 111, 163, 205, 205, 128, 169, 169, 169, 82, 
146, 187, 187, 136, 150, 209, 209, 120, 150, 198, 198, 128, 135, 210, 210, 107, 
146, 192, 192, 132, 139, 216, 216, 113, 160, 204, 204, 118, 162, 181, 181, 93, 
146, 189, 189, 134, 145, 213, 213, 117, 155, 201, 201, 124, 131, 198, 198, 103, 
146, 195, 195, 134, 138, 219, 219, 110, 158, 206, 206, 155, 154, 151, 151, 77, 
146, 185, 185, 138, 153, 208, 208, 122, 148, 196, 196, 129, 137, 216, 216, 109, 
146, 191, 191, 133, 142, 214, 214, 115, 158, 202, 202, 121, 140, 191, 191, 96, 
146, 188, 188, 135, 147, 211, 211, 119, 153, 199, 199, 126, 133, 204, 204, 105, 
146, 193, 193, 131, 138, 217, 217, 111, 163, 205, 205, 128, 169, 169, 169, 82, 
146, 187, 187, 136, 150, 209, 209, 120, 150, 198, 198, 128, 135, 210, 210, 107, 
146, 192, 192, 132, 139, 216, 216, 113, 160, 204, 204, 118, 162, 181, 181, 93, 
146, 189, 189, 134, 145, 213, 213, 117, 155, 201, 201, 124, 131, 198, 198, 103, 
146, 195, 195, 134, 138, 219, 219, 110, 158, 206, 206, 155, 154, 151, 151, 77, 
146, 186, 186, 137, 152, 208, 208, 122, 148, 197, 197, 129, 137, 215, 215, 108, 
146, 191, 191, 133, 141, 214, 214, 115, 158, 202, 202, 120, 145, 189, 189, 95, 
146, 188, 188, 135, 147, 211, 211, 118, 153, 200, 200, 125, 132, 203, 203, 105, 
146, 194, 194, 131, 138, 218, 218, 111, 164, 205, 205, 134, 167, 167, 167, 82, 
146, 187, 187, 136, 149, 210, 210, 120, 151, 198, 198, 127, 134, 209, 209, 106, 
146, 192, 192, 132, 139, 216, 216, 113, 161, 204, 204, 117, 168, 177, 177, 93, 
146, 190, 190, 134, 144, 213, 213, 116, 156, 201, 201, 123, 130, 197, 197, 100, 
146, 195, 195, 137, 138, 219, 219, 110, 155, 207, 207, 149, 150, 144, 144, 73, 
146, 185, 185, 138, 154, 207, 207, 123, 147, 196, 196, 130, 138, 219, 219, 109, 
146, 190, 190, 134, 143, 214, 214, 116, 157, 202, 202, 123, 133, 194, 194, 97, 
146, 187, 187, 136, 148, 210, 210, 119, 152, 199, 199, 126, 134, 206, 206, 106, 
146, 193, 193, 131, 138, 217, 217, 112, 162, 204, 204, 119, 173, 173, 173, 84, 
146, 186, 186, 137, 151, 209, 209, 121, 149, 197, 197, 128, 136, 213, 213, 108, 
146, 192, 192, 132, 140, 215, 215, 114, 159, 203, 203, 119, 153, 185, 185, 94, 
146, 189, 189, 135, 146, 212, 212, 117, 154, 200, 200, 125, 132, 201, 201, 104, 
146, 194, 194, 131, 138, 218, 218, 110, 163, 206, 206, 144, 162, 161, 161, 81, 
146, 185, 185, 137, 153, 208, 208, 122, 148, 196, 196, 129, 137, 216, 216, 108, 
146, 191, 191, 133, 142, 214, 214, 115, 158, 202, 202, 121, 143, 190, 190, 96, 
146, 188, 188, 135, 147, 211, 211, 118, 153, 199, 199, 126, 133, 204, 204, 105, 
146, 194, 194, 131, 138, 217, 217, 111, 163, 205, 205, 131, 168, 168, 168, 82, 
146, 187, 187, 136, 150, 210, 210, 120, 150, 198, 198, 127, 135, 209, 209, 107, 
146, 192, 192, 132, 139, 216, 216, 113, 161, 204, 204, 117, 165, 179, 179, 93, 
146, 190, 190, 134, 144, 213, 213, 117, 155, 201, 201, 124, 131, 198, 198, 102, 
146, 195, 195, 136, 138, 219, 219, 110, 157, 207, 207, 152, 152, 147, 147, 75, 
146, 185, 185, 137, 153, 208, 208, 122, 148, 196, 196, 129, 137, 216, 216, 108, 
146, 191, 191, 133, 142, 214, 214, 115, 158, 202, 202, 121, 143, 190, 190, 96, 
146, 188, 188, 135, 147, 211, 211, 118, 153, 199, 199, 126, 133, 204, 204, 105, 
146, 194, 194, 131, 138, 217, 217, 111, 163, 205, 205, 131, 168, 168, 168, 82, 
146, 187, 187, 136, 150, 210, 210, 120, 150, 198, 198, 127, 135, 209, 209, 107, 
146, 192, 192, 132, 139, 216, 216, 113, 161, 204, 204, 117, 165, 179, 179, 93, 
146, 190, 190, 134, 144, 213, 213, 117, 155, 201, 201, 124, 131, 198, 198, 102, 
146, 195, 195, 136, 138, 219, 219, 110, 157, 207, 207, 152, 152, 147, 147, 75, 
146, 186, 186, 137, 152, 209, 209, 121, 149, 197, 197, 129, 136, 214, 214, 108, 
146, 191, 191, 133, 141, 215, 215, 114, 159, 203, 203, 120, 148, 187, 187, 95, 
146, 188, 188, 135, 146, 212, 212, 118, 153, 200, 200, 125, 132, 202, 202, 104, 
146, 194, 194, 130, 138, 218, 218, 111, 164, 206, 206, 137, 166, 166, 166, 82, 
146, 187, 187, 136, 149, 210, 210, 120, 151, 198, 198, 127, 134, 208, 208, 106, 
146, 193, 193, 132, 138, 216, 216, 112, 161, 204, 204, 116, 171, 176, 176, 92, 
146, 190, 190, 134, 144, 213, 213, 116, 156, 201, 201, 123, 130, 196, 196, 98, 
146, 195, 195, 138, 138, 219, 219, 110, 155, 207, 207, 146, 148, 141, 141, 69, 
146, 185, 185, 138, 154, 207, 207, 123, 147, 196, 196, 130, 138, 219, 219, 109, 
146, 190, 190, 134, 143, 214, 214, 116, 157, 202, 202, 123, 133, 194, 194, 97, 
146, 187, 187, 136, 148, 210, 210, 119, 152, 199, 199, 126, 134, 206, 206, 106, 
146, 193, 193, 131, 138, 217, 217, 112, 162, 204, 204, 119, 173, 173, 173, 84, 
146, 186, 186, 137, 151, 209, 209, 121, 149, 197, 197, 128, 136, 213, 213, 108, 
146, 192, 192, 132, 140, 215, 215, 114, 159, 203, 203, 119, 153, 185, 185, 94, 
146, 189, 189, 135, 146, 212, 212, 117, 154, 200, 200, 125, 132, 201, 201, 104, 
146, 194, 194, 131, 138, 218, 218, 110, 163, 206, 206, 144, 162, 161, 161, 81, 
146, 185, 185, 137, 153, 208, 208, 122, 148, 196, 196, 129, 137, 216, 216, 108, 
146, 191, 191, 133, 142, 214, 214, 115, 158, 202, 202, 121, 143, 190, 190, 96, 
146, 188, 188, 135, 147, 211, 211, 118, 153, 199, 199, 126, 133, 204, 204, 105, 
146, 194, 194, 131, 138, 217, 217, 111, 163, 205, 205, 131, 168, 168, 168, 82, 
146, 187, 187, 136, 150, 210, 210, 120, 150, 198, 198, 127, 135, 209, 209, 107, 
146, 192, 192, 132, 139, 216, 216, 113, 161, 204, 204, 117, 165, 179, 179, 93, 
146, 190, 190, 134, 144, 213, 213, 117, 155, 201, 201, 124, 131, 198, 198, 102, 
146, 195, 195, 136, 138, 219, 219, 110, 157, 207, 207, 152, 152, 147, 147, 75, 
146, 185, 185, 137, 153, 208, 208, 122, 148, 196, 196, 129, 137, 216, 216, 108, 
146, 191, 191, 133, 142, 214, 214, 115, 158, 202, 202, 121, 143, 190, 190, 96, 
146, 188, 188, 135, 147, 211, 211, 118, 153, 199, 199, 126, 133, 204, 204, 105, 
146, 194, 194, 131, 138, 217, 217, 111, 163, 205, 205, 131, 168, 168, 168, 82, 
146, 187, 187, 136, 150, 210, 210, 120, 150, 198, 198, 127, 135, 209, 209, 107, 
146, 192, 192, 132, 139, 216, 216, 113, 161, 204, 204, 117, 165, 179, 179, 93, 
146, 190, 190, 134, 144, 213, 213, 117, 155, 201, 201, 124, 131, 198, 198, 102, 
146, 195, 195, 136, 138, 219, 219, 110, 157, 207, 207, 152, 152, 147, 147, 75, 
146, 186, 186, 137, 152, 209, 209, 121, 149, 197, 197, 129, 136, 214, 214, 108, 
146, 191, 191, 133, 141, 215, 215, 114, 159, 203, 203, 120, 148, 187, 187, 95, 
146, 188, 188, 135, 146, 212, 212, 118, 153, 200, 200, 125, 132, 202, 202, 104, 
146, 194, 194, 130, 138, 218, 218, 111, 164, 206, 206, 137, 166, 166, 166, 82, 
146, 187, 187, 136, 149, 210, 210, 120, 151, 198, 198, 127, 134, 208, 208, 106, 
146, 193, 193, 132, 138, 216, 216, 112, 161, 204, 204, 116, 171, 176, 176, 92, 
146, 190, 190, 134, 144, 213, 213, 116, 156, 201, 201, 123, 130, 196, 196, 98, 
146, 195, 195, 138, 138, 219, 219, 110, 155, 207, 207, 146, 148, 141, 141, 69, 
146, 185, 185, 138, 154, 207, 207, 123, 147, 196, 196, 130, 138, 218, 218, 109, 
146, 190, 190, 133, 143, 214, 214, 116, 157, 202, 202, 122, 134, 194, 194, 97, 
146, 188, 188, 136, 148, 211, 211, 119, 152, 199, 199, 126, 134, 206, 206, 106, 
146, 193, 193, 131, 138, 217, 217, 112, 162, 205, 205, 120, 172, 172, 172, 82, 
146, 186, 186, 137, 151, 209, 209, 121, 149, 197, 197, 128, 136, 212, 212, 107, 
146, 192, 192, 132, 140, 215, 215, 114, 160, 203, 203, 119, 155, 185, 185, 94, 
146, 189, 189, 135, 146, 212, 212, 117, 154, 200, 200, 124, 131, 200, 200, 104, 
146, 194, 194, 132, 138, 218, 218, 110, 162, 206, 206, 146, 161, 159, 159, 80, 
146, 185, 185, 137, 152, 208, 208, 122, 148, 197, 197, 129, 137, 215, 215, 108, 
146, 191, 191, 133, 142, 214, 214, 115, 158, 202, 202, 121, 144, 189, 189, 96, 
146, 188, 188, 135, 147, 211, 211, 118, 153, 199, 199, 125, 133, 203, 203, 105, 
146, 194, 194, 131, 138, 217, 217, 111, 163, 205, 205, 133, 167, 167, 167, 82, 
146, 187, 187, 136, 150, 210, 210, 120, 151, 198, 198, 127, 135, 209, 209, 107, 
146, 192, 192, 132, 139, 216, 216, 113, 161, 204, 204, 117, 166, 178, 178, 93, 
146, 190, 190, 134, 144, 213, 213, 116, 156, 201, 201, 124, 130, 197, 197, 101, 
146, 195, 195, 136, 138, 219, 219, 110, 156, 207, 207, 151, 151, 146, 146, 74, 
146, 185, 185, 137, 152, 208, 208, 122, 148, 197, 197, 129, 137, 215, 215, 108, 
146, 191, 191, 133, 142, 214, 214, 115, 158, 202, 202, 121, 144, 189, 189, 96, 
146, 188, 188, 135, 147, 211, 211, 118, 153, 199, 199, 125, 133, 203, 203, 105, 
146, 194, 194, 131, 138, 217, 217, 111, 163, 205, 205, 133, 167, 167, 167, 82, 
146, 187, 187, 136, 150, 210, 210, 120, 151, 198, 198, 127, 135, 209, 209, 107, 
146, 192, 192, 132, 139, 216, 216, 113, 161, 204, 204, 117, 166, 178, 178, 93, 
146, 190, 190, 134, 144, 213, 213, 116, 156, 201, 201, 124, 130, 197, 197, 101, 
146, 195, 195, 136, 138, 219, 219, 110, 156, 207, 207, 151, 151, 146, 146, 74, 
146, 186, 186, 137, 152, 209, 209, 121, 149, 197, 197, 129, 136, 214, 214, 108, 
146, 191, 191, 133, 141, 215, 215, 114, 159, 203, 203, 120, 149, 187, 187, 95, 
146, 189, 189, 135, 146, 212, 212, 118, 154, 200, 200, 125, 132, 202, 202, 104, 
146, 194, 194, 130, 138, 218, 218, 111, 164, 206, 206, 139, 165, 165, 165, 82, 
146, 187, 187, 136, 149, 210, 210, 120, 151, 198, 198, 127, 134, 208, 208, 106, 
146, 193, 193, 131, 138, 216, 216, 112, 161, 204, 204, 116, 173, 175, 175, 92, 
146, 190, 190, 134, 144, 213, 213, 116, 156, 201, 201, 123, 130, 196, 196, 98, 
146, 195, 195, 138, 138, 219, 219, 110, 155, 207, 207, 134, 146, 140, 140, 65, 

};

volatile uint8_t g_curADCValue;

#define TOUCH_SW 1

// call this function at constant interval
#if TOUCH_SW
static uint16_t GetSWState(void)
{
	uint16_t a = PINC;
	uint8_t b2 = PIND;
	SFIOR &= ~0x04; // pull up enable
	// PIND: swap PD2 thru PD7
	uint8_t b = 0;
	if(b2 & 0x04) b |= 0x20;
	if(b2 & 0x08) b |= 0x10;
	if(b2 & 0x10) b |= 0x08;
	if(b2 & 0x20) b |= 0x04;
	if(b2 & 0x40) b |= 0x02;
	if(b2 & 0x80) b |= 0x01;
	uint16_t state = (b & 0x3F) | (a << 6);
	SFIOR |= 0x04; // pull up disable
	
	static uint16_t lastState;
	uint16_t ret = state | lastState;
	lastState = state;
	return ~ret;
}
#else
static uint16_t GetSWState(void)
{
	uint16_t a = PINC;
	uint8_t b2 = PIND;
	// PIND: swap PD2 thru PD7
	uint8_t b = 0;
	if(b2 & 0x04) b |= 0x20;
	if(b2 & 0x08) b |= 0x10;
	if(b2 & 0x10) b |= 0x08;
	if(b2 & 0x20) b |= 0x04;
	if(b2 & 0x40) b |= 0x02;
	if(b2 & 0x80) b |= 0x01;
	uint16_t state = (b & 0x3F) | (a << 6);
	
	return ~state;
}
#endif

#define ADC_BUF_SIZE 4
// ADC Interrput
ISR(ADC_vect)
{
	static uint16_t adcBuf[ADC_BUF_SIZE];
	uint16_t d = ADC;
	if(d>=512) d = 511; // 2.5V f.s.
	
	uint8_t i;
	for(i=0; i<ADC_BUF_SIZE-1; i++){
		adcBuf[i] = adcBuf[i+1];
	}
	adcBuf[ADC_BUF_SIZE-1] = d;
	
	uint16_t ret = 0;
	for(i=0; i<ADC_BUF_SIZE; i++){
		ret += adcBuf[i];
	}
	ret /= ADC_BUF_SIZE;
	
	g_curADCValue = ret/2;
	
}

static void SendUART(uint8_t data)
{
	while((UCSRA & 0x20) == 0);
	UDR = data;
	while((UCSRA & 0x20) == 0);
}

static void Init(void)
{
	//OSCCAL = 176; // [Note] this value should be adjusted by reading CAL value,,, 176 is an example value
	
	DDRC = 0;
	PORTC = 0b00011111; // Sensor, SW10-6
	
	DDRD  = 0b00000010; // SW0-5, TX, RX
	PORTD = 0b11111101;
	
	DDRB  = 0b00001010;
	PORTB = 0b11110101;
	
	// UART
	UCSRA = 0b00000010; // U2X = 1
	UCSRB = 0b00001000;
	UCSRC = 0b10000110; 
	
	UBRRH = 0;
	UBRRL = 12; // 19200 bps (19200bps x 10 x 5 = 2.6ms)
	// ADC
	ADMUX = 0b01000101; // ADC5
	ADCSRA = 0b10101101; // ADPS = 1/32 (63 kHz)
	ADCSRA |= 0x40; // start adc
	
	// Timer1: CTC, output OC1A
	TCCR1A = 0b01000000; // Toggle OC1A on compare match
	TCCR1B = 0b00001000; // stop timer here
	OCR1A  = 0; // OCR1A = OSC/freq
	
	// Timer2: PWM (7.8kHz)
	TCCR2 = 0b01101001;
	OCR2 = 255;
}

// freq: actual freq x 2 [Hz]
#define FOSC 2000000UL
void UpdateBeepFreq(uint16_t freq)
{
	if(freq == 0){
		TCCR1A = 0b00000000;
		TCCR1B &= ~0x01;
	}else{
		TCCR1A = 0b01000000;
		TCCR1B |= 0x01;
		OCR1A = FOSC/freq - 1;
	}
}

void CalibrateSensor()
{
	uint8_t calibrationValue = 0;
	for(calibrationValue=200; calibrationValue>20; calibrationValue--)
	{
		OCR2 = calibrationValue;
		_delay_ms(100);
		if(g_curADCValue > 10) {
			OCR2 = calibrationValue+1;
			break;
		}
	}
}

int main(void)
{
	uint8_t i;
	// wait for power up
	for(i=0; i<100; i++) _delay_ms(2);
	
	Init();
	sei();
	
	CalibrateSensor();
	
	uint8_t beepState = 0;
	
	uint8_t newTone = 0;
	
	// transpose
	uint8_t refFreq = 131;
	if((GetSWState() % 2048) != 0){
		refFreq = pgm_read_byte(&g_freqList[GetSWState() % 2048]);
	}
	
	while(1) // Poll RX pin state
	{
		if((PIND & 0x01) == 0){
			_delay_us(10); // wait 10 us
			while((PIND & 0x01) == 0); // wait until H
			_delay_us(10); // wait 10 us
			
			uint8_t adcValue = g_curADCValue;
			
			// Send 5 bytes
			uint8_t sendBytes[5];
			uint16_t curSwState = GetSWState();
			sendBytes[0] = adcValue;
			sendBytes[1] = curSwState;
			sendBytes[2] = curSwState >> 8;
			sendBytes[3] = 0;
			sendBytes[4] = 0;
			for(i=0; i<5; i++){
				SendUART(sendBytes[i]);
			}
			
			if((beepState && (adcValue > 24)) || (adcValue > 32)){
				if(beepState == 0) newTone = 1;
				else newTone = 0;
				static uint16_t lastFreq;
				
				uint16_t freq_d = pgm_read_byte(&g_freqList[curSwState % 2048]);
				freq_d = ((uint32_t)(freq_d)) * 131 / refFreq; // Transpose
				
				uint8_t  freq_mod = 255;
				if(adcValue < 64) freq_mod = 255 + adcValue/2 - 32; // lower freq with weaker breath
				
				uint16_t freq = ((uint32_t)freq_d) * freq_mod / 32; // 8 = 256 / 32
				
				if(newTone == 0) freq = (lastFreq * 3 + freq) / 4;
				
				
				UpdateBeepFreq(freq);
				//UpdateBeepFreq(440);
				lastFreq = freq;
				beepState = 1;
			}else{
				UpdateBeepFreq(0);
				beepState = 0;
			}
			
			_delay_ms(1); // wait 1 ms
			// test
			//SendUART(0x55);
		}
	}
	
	return 0;
}
