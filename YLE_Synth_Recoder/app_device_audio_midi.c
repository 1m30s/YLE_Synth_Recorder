/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
 *******************************************************************************/

/** INCLUDES *******************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "system.h"

#include "usb.h"
#include "usb_device_midi.h"


/** VARIABLES ******************************************************/
/* Some processors have a limited range of RAM addresses where the USB module
 * is able to access.  The following section is for those devices.  This section
 * assigns the buffers that need to be used by the USB module into those
 * specific areas.
 */
#if defined(FIXED_ADDRESS_MEMORY)
#if defined(COMPILER_MPLAB_C18)
#pragma udata DEVICE_AUDIO_MIDI_RX_DATA_BUFFER=DEVCE_AUDIO_MIDI_RX_DATA_BUFFER_ADDRESS
static uint8_t ReceivedDataBuffer[64];
#pragma udata DEVICE_AUDIO_MIDI_EVENT_DATA_BUFFER=DEVCE_AUDIO_MIDI_EVENT_DATA_BUFFER_ADDRESS
static USB_AUDIO_MIDI_EVENT_PACKET midiData;
#pragma udata
#elif defined(__XC8)
static uint8_t ReceivedDataBuffer[64] @ DEVCE_AUDIO_MIDI_RX_DATA_BUFFER_ADDRESS;
static USB_AUDIO_MIDI_EVENT_PACKET midiData @ DEVCE_AUDIO_MIDI_EVENT_DATA_BUFFER_ADDRESS;
#endif
#else
static uint8_t ReceivedDataBuffer[64];
static USB_AUDIO_MIDI_EVENT_PACKET midiData;
#endif

static USB_HANDLE USBTxHandle;
static USB_HANDLE USBRxHandle;

static uint8_t pitch;
static bool sentNoteOff;

static USB_VOLATILE uint8_t msCounter;

/*********************************************************************
 * Function: void APP_DeviceAudioMIDIInitialize(void);
 *
 * Overview: Initializes the demo code
 *
 * PreCondition: None
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
void APP_DeviceAudioMIDIInitialize()
{
	USBTxHandle = NULL;
	USBRxHandle = NULL;

	pitch = 0x3C;
	sentNoteOff = true;

	msCounter = 0;

	//enable the HID endpoint
	USBEnableEndpoint(USB_DEVICE_AUDIO_MIDI_ENDPOINT, USB_OUT_ENABLED | USB_IN_ENABLED | USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);

	//Re-arm the OUT endpoint for the next packet
	USBRxHandle = USBRxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & ReceivedDataBuffer, 64);
}

/*********************************************************************
 * Function: void APP_DeviceAudioMIDIInitialize(void);
 *
 * Overview: Initializes the demo code
 *
 * PreCondition: None
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
void APP_DeviceAudioMIDISOFHandler()
{
	if (msCounter != 0) {
		msCounter--;
	}
}

/*********************************************************************
 * Function: void APP_DeviceAudioMIDITasks(void);
 *
 * Overview: Keeps the Custom HID demo running.
 *
 * PreCondition: The demo should have been initialized and started via
 *   the APP_DeviceAudioMIDIInitialize() and APP_DeviceAudioMIDIStart() demos
 *   respectively.
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
void APP_DeviceAudioMIDITasks()
{
	/* If the device is not configured yet, or the device is suspended, then
	 * we don't need to run the demo since we can't send any data.
	 */
	if ((USBGetDeviceState() < CONFIGURED_STATE) ||
			(USBIsDeviceSuspended() == true)) {
		return;
	}

	if (!USBHandleBusy(USBRxHandle)) {
		//We have received a MIDI packet from the host, process it and then
		//  prepare to receive the next packet

		//INSERT MIDI PROCESSING CODE HERE

		//Get ready for next packet (this will overwrite the old data)
		USBRxHandle = USBRxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & ReceivedDataBuffer, 64);
	}
	
	// current state
	static uint8_t curNoteNo, curNoteState, curExpression;
	static uint16_t curBend; 
	
	static uint8_t phase;
	
	struct sRecoderStatus s = g_status; // copy status
	
	// MIDI message referred to : http://www.personal.kent.edu/~sbirch/Music_Production/MP-II/MIDI/an_introduction_to_midi_contents.htm
	if(msCounter == 0){
		if(curNoteState == 0){
			if(s.expression > 32) // send note on and pitch bend
			{
				// Send Expression -> Bend -> Tone
				if(phase == 0){
					curNoteNo = s.tone;
					curBend = 0x2000 + ((int16_t)s.semitone * 43); // curSemitone dvalue is 341.3 per a single semitone
					curExpression = s.expression;


					if (!USBHandleBusy(USBTxHandle)) {
						midiData.Val = 0; //must set all unused values to 0 so go ahead
						midiData.CableNumber = 0;
						midiData.CodeIndexNumber = MIDI_CIN_CONTROL_CHANGE;
						midiData.DATA_0 = 0xB0; // Control change 
						midiData.DATA_1 = 0x0B; // Expression
						midiData.DATA_2 = curExpression; // value

						USBTxHandle = USBTxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & midiData, 4);
					}
					msCounter = 10;
					
					phase ++;
				}else if(phase == 1){
					if (!USBHandleBusy(USBTxHandle)) {
						midiData.Val = 0; //must set all unused values to 0 so go ahead
						midiData.CableNumber = 0;
						midiData.CodeIndexNumber = MIDI_CIN_PITCH_BEND_CHANGE;
						midiData.DATA_0 = 0xE0; // Bend 
						midiData.DATA_1 = curBend & 0x7F; // LSB
						midiData.DATA_2 = (curBend >> 7) & 0x7F; // MSB

						USBTxHandle = USBTxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & midiData, 4);
					}
					msCounter = 10;
					
					phase ++;
				}else{
					curNoteState = 1;
				
					if (!USBHandleBusy(USBTxHandle)) {
						midiData.Val = 0; //must set all unused values to 0 so go ahead
						midiData.CableNumber = 0;
						midiData.CodeIndexNumber = MIDI_CIN_NOTE_ON;
						midiData.DATA_0 = 0x90; // Note on
						midiData.DATA_1 = curNoteNo; // note no
						midiData.DATA_2 = 0x60; // velocity

						USBTxHandle = USBTxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & midiData, 4);
					}

					msCounter = 10;
					phase = 0;
				}
			}
		}else{ // previously note on
			if(/*1 ||*/ s.expression < 16) // send note off
			{
				if (!USBHandleBusy(USBTxHandle)) {
					midiData.Val = 0; //must set all unused values to 0 so go ahead
					midiData.CableNumber = 0;
					midiData.CodeIndexNumber = MIDI_CIN_NOTE_ON;
					midiData.DATA_0 = 0x90; // Note on
					midiData.DATA_1 = curNoteNo; // note no
					midiData.DATA_2 = 0; // velocity

					USBTxHandle = USBTxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & midiData, 4);
				}
				
				curNoteState = 0;
				// 
			}else{ // update pitch bend
				msCounter = 10;
				
				uint32_t bend = 0x2000 + 341*((int16_t)s.tone - (int16_t)curNoteNo) + ((int16_t)s.semitone) * 43; // curSemitone evalue is 341.3 per 1 semitone
				uint8_t expression = s.expression;
				
				// Send Expression -> Bend
				if ((expression != curExpression) && !USBHandleBusy(USBTxHandle)) {
					midiData.Val = 0; //must set all unused values to 0 so go ahead
					midiData.CableNumber = 0;
					midiData.CodeIndexNumber = MIDI_CIN_CONTROL_CHANGE;
					midiData.DATA_0 = 0xB0; // Control change 
					midiData.DATA_1 = 0x0B; // Expression
					midiData.DATA_2 = expression; // value

					USBTxHandle = USBTxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & midiData, 4);
				}
				
				// Gradually change picth...
				const uint8_t bendChangeability = 8; // 1-16
				bend = ((bend * bendChangeability) + ((uint32_t)curBend) * (16-bendChangeability)) / 16;
				
				if (/*(bend != curBend) &&*/ !USBHandleBusy(USBTxHandle)) {
					midiData.Val = 0; //must set all unused values to 0 so go ahead
					midiData.CableNumber = 0;
					midiData.CodeIndexNumber = MIDI_CIN_PITCH_BEND_CHANGE;
					midiData.DATA_0 = 0xE0; // Bend 
					midiData.DATA_1 = bend & 0x7F; // LSB
					midiData.DATA_2 = (bend >> 7) & 0x7F; // MSB

					USBTxHandle = USBTxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & midiData, 4);
				}
				
				curBend  = bend;
				curExpression = expression;
				
			}
			
		}
	}

#if 0
	/* If the user button is pressed... */
	if (BUTTON_IsPressed(BUTTON_DEVICE_AUDIO_MIDI) == true) {
		/* and we haven't sent a transmission in the past 100ms... */
		if (msCounter == 0) {
			/* and we have sent the NOTE_OFF for the last note... */
			if (sentNoteOff == true) {
				/* and we aren't currently trying to transmit data... */
				if (!USBHandleBusy(USBTxHandle)) {
					//Then reset the 100ms counter
					msCounter = 100;

					midiData.Val = 0; //must set all unused values to 0 so go ahead
					//  and set them all to 0

					midiData.CableNumber = 0;
					midiData.CodeIndexNumber = MIDI_CIN_NOTE_ON;
					midiData.DATA_0 = 0x90; //Note on
					midiData.DATA_1 = pitch; //pitch
					midiData.DATA_2 = 0x7F; //velocity

					USBTxHandle = USBTxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & midiData, 4);

					/* we now need to send the NOTE_OFF for this note. */
					sentNoteOff = false;
				}
			}
		}
	} else {
		if (msCounter == 0) {
			if (sentNoteOff == false) {
				if (!USBHandleBusy(USBTxHandle)) {
					//Debounce counter for 100ms
					msCounter = 100;

					midiData.Val = 0; //must set all unused values to 0 so go ahead
					//  and set them all to 0

					midiData.CableNumber = 0;
					midiData.CodeIndexNumber = MIDI_CIN_NOTE_ON;
					midiData.DATA_0 = 0x90; //Note off
					midiData.DATA_1 = pitch++; //pitch
					midiData.DATA_2 = 0x00; //velocity

					if (pitch == 0x49) {
						pitch = 0x3C;
					}

					USBTxHandle = USBTxOnePacket(USB_DEVICE_AUDIO_MIDI_ENDPOINT, (uint8_t*) & midiData, 4);
					sentNoteOff = true;
				}
			}
		}
	}
#endif
}
