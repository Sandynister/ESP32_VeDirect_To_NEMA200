/* framehandler.cpp
 *
 * Arduino library to read from Victron devices using VE.Direct protocol.
 * Derived from Victron framehandler reference implementation.
 * 
 * 2020.04.14 - 0.0 - initial
 * 2020.04.18 - 0.1 - add buffers for private and public data
 * 
 */
 
#include <Arduino.h>
#include "framehandler.h"

#define MODULE "VE.Frame"	// Victron seems to use this to find out where logging messages were generated

// The name of the record that contains the checksum.
static constexpr char checksumTagName[] = "CHECKSUM";

VeDirectFrameHandler::VeDirectFrameHandler() :
	//mStop(false),	// don't know what Victron uses this for, not using
	mState(IDLE),
	mChecksum(0),
	mTextPointer(0),
    tempName(),
    tempValue(),
	frameIndex(0),
	veName(),
	veValue(),
	veEnd(0)
{
}

/*
 *	rxData
 *  This function is called by the application which passes a byte of serial data
 *  It is unchanged from Victron's example code
 */
void VeDirectFrameHandler::rxData(uint8_t inbyte)
{
	//if (mStop) return;
	if ( (inbyte == ':') && (mState != CHECKSUM) ) {
		mState = RECORD_HEX;
	}
	if (mState != RECORD_HEX) {
		mChecksum += inbyte;
	}
	inbyte = toupper(inbyte);

	switch(mState) {
	case IDLE:
		/* wait for \n of the start of an record */
		switch(inbyte) {
		case '\n':
			mState = RECORD_BEGIN;
			break;
		case '\r': /* Skip */
		default:
			break;
		}
		break;
	case RECORD_BEGIN:
		mTextPointer = mName;
		*mTextPointer++ = inbyte;
		mState = RECORD_NAME;
		break;
	case RECORD_NAME:
		// The record name is being received, terminated by a \t
		switch(inbyte) {
		case '\t':
			// the Checksum record indicates a EOR
			if ( mTextPointer < (mName + sizeof(mName)) ) {
				*mTextPointer = 0; /* Zero terminate */
				if (strcmp(mName, checksumTagName) == 0) {
					mState = CHECKSUM;
					break;
				}
			}
			mTextPointer = mValue; /* Reset value pointer */
			mState = RECORD_VALUE;
			break;
		default:
			// add byte to name, but do no overflow
			if ( mTextPointer < (mName + sizeof(mName)) )
				*mTextPointer++ = inbyte;
			break;
		}
		break;
	case RECORD_VALUE:
		// The record value is being received.  The \r indicates a new record.
		switch(inbyte) {
		case '\n':
			// forward record, only if it could be stored completely
			if ( mTextPointer < (mValue + sizeof(mValue)) ) {
				*mTextPointer = 0; // make zero ended
				textRxEvent(mName, mValue);
			}
			mState = RECORD_BEGIN;
			break;
		case '\r': /* Skip */
			break;
		default:
			// add byte to value, but do no overflow
			if ( mTextPointer < (mValue + sizeof(mValue)) )
				*mTextPointer++ = inbyte;
			break;
		}
		break;
	case CHECKSUM:
	{
		bool valid = mChecksum == 0;
		if (!valid)
			logE(MODULE,"[CHECKSUM] Invalid frame");
		mChecksum = 0;
		mState = IDLE;
		frameEndEvent(valid);
		break;
	}
	case RECORD_HEX:
		if (hexRxEvent(inbyte)) {
			mChecksum = 0;
			mState = IDLE;
		}
		break;
	}
}

/*
 * textRxEvent
 * This function is called every time a new name/value is successfully parsed.  It writes the values to the private buffer.
 */
void VeDirectFrameHandler::textRxEvent(char * mName, char * mValue) {
    strcpy(tempName[frameIndex], mName);    // copy name to private buffer
    strcpy(tempValue[frameIndex], mValue);  // copy value to private buffer
	frameIndex++;
}

/*
 *	frameEndEvent
 *  This function is called at the end of the received frame.  If the checksum is valid, the temp buffer is read line by line.
 *  If the name exists in the public buffer, the new value is copied to the public buffer.	If not, a new name/value entry
 *  is created in the public buffer.
 */
void VeDirectFrameHandler::frameEndEvent(bool valid) {
	if ( valid ) {
		for ( int i = 0; i < frameIndex; i++ ) {				// read each name already in the temp buffer
			bool nameExists = false;
			for ( int j = 0; j <= veEnd; j++ ) {				// compare to existing names in the public buffer
				if ( strcmp(tempName[i], veName[j]) == 0 ) {	
					strcpy(veValue[j], tempValue[i]);			// overwrite tempValue in the public buffer
					nameExists = true;
					break;
				}
			}
			if ( !nameExists ) {
				strcpy(veName[veEnd], tempName[i]);				// write new Name to public buffer
				strcpy(veValue[veEnd], tempValue[i]);			// write new Value to public buffer
				veEnd++;										// increment end of public buffer
				if ( veEnd >= buffLen ) {						// stop any buffer overrun
					veEnd = buffLen - 1;
				}
			}
		}
	}
	frameIndex = 0;	// reset frame
}


/*
 *	logE
 *  This function included for continuity and possible future use.	
 */
void VeDirectFrameHandler::logE(char * module, char * error) {
	//Serial.print("MODULE: ");
    //Serial.println(module);
    //Serial.print("ERROR: ");
    //Serial.println(error);
	return;
}


/*
 *	hexRxEvent
 *  This function included for continuity and possible future use.	
 */
bool VeDirectFrameHandler::hexRxEvent(uint8_t inbyte) {
	return true;		// stubbed out for future
}