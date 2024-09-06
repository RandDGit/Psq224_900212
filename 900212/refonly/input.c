/* *** include directives *** */

#include <iom16.h>
#include "Input.h"
#include "System1Data.h"
#include "simpleEventHandler.h"



/* *** type definitions *** */

typedef union 
{
  unsigned char all;
  struct 
  {
    unsigned char bit0 : 1;
    unsigned char bit1 : 1;
    unsigned char bit2 : 1;
    unsigned char bit3 : 1;
    unsigned char bit4 : 1;
    unsigned char bit5 : 1;
    unsigned char bit6 : 1;
    unsigned char bit7 : 1;
  };
} 
InputBits;



/* *** variable definitions *** */

InputBits _physicalInput;
InputBits _logicalInput;
int _inputFilterDelay_0;
int _inputFilterDelay_1;
int _inputFilterDelay_2;
int _inputFilterDelay_3;



/* *** function definitions *** */

void InputChangedOn(int no) 
{
  switch (no) 
  {
  case 1 :
    // send text message
    
    //SEQ_AddEvent(Equested_floor);
    break;
  case 2 :
    //SEQ_AddEvent(E_new_floor);
    break;
  }
}



void InitializeInput(void) 
{
  _physicalInput.all = ~PINA;
  _logicalInput = _physicalInput;
  _inputFilterDelay_0 = 5;
  _inputFilterDelay_1 = 5;
  _inputFilterDelay_2 = 5;
  _inputFilterDelay_3 = 5;
}



void HandleInput(void) 
{
  _physicalInput.all = ~PINA;
  
  if (_physicalInput.bit0 != _logicalInput.bit0) 
  {
    _inputFilterDelay_0--;
    if (_inputFilterDelay_0 == 0) 
    {
      _logicalInput.bit0 = _physicalInput.bit0;
      if (_logicalInput.bit0) 
        InputChangedOn(0);
      _inputFilterDelay_0 = 5;
    }
  } 
  else
    _inputFilterDelay_0 = 5;
  
  if (_physicalInput.bit1 != _logicalInput.bit1) 
  {
    _inputFilterDelay_1--;
    if (_inputFilterDelay_1 == 0) 
    {
      _logicalInput.bit1 = _physicalInput.bit1;
      if (_logicalInput.bit1) 
        InputChangedOn(1);
      _inputFilterDelay_1 = 5;
    }
  } 
  else 
    _inputFilterDelay_1 = 5;
  
  if (_physicalInput.bit2 != _logicalInput.bit2) 
  {
    _inputFilterDelay_2--;
    if (_inputFilterDelay_2 == 0) 
    {
      _logicalInput.bit2 = _physicalInput.bit2;
      if (_logicalInput.bit2) 
        InputChangedOn(2);
      _inputFilterDelay_2 = 5;
    }
  } 
  else 
    _inputFilterDelay_2 = 5;
  
  if (_physicalInput.bit3 != _logicalInput.bit3) 
  {
    _inputFilterDelay_3--;
    if (_inputFilterDelay_3 == 0) 
    {
      _logicalInput.bit3 = _physicalInput.bit3;
      if (_logicalInput.bit3) 
        InputChangedOn(3);
      _inputFilterDelay_3 = 5;
    }
  } 
  else
    _inputFilterDelay_3 = 5;
}
