#Breakdown of the C++ Code

**ATTENTION**: Majority of the code is directly from the arduino code provided by Charmed Labs. Few codes are derived from the arduino code due to incompatabilty to C++ libarary. Also, the code still needs some refinement - we could not separate the code into .h and .cpp

Tried our best to comment things on the .cpp file to makes things easier for you to understand :)

---
##Block Struct 

- Includes: 
  
  - Just predefined things for Pixy (specifics are commented on the actual code)

- Variables & Declaration: 
  
  - I2C declaration goes like this: 
    - i2c = new I2C(I2C::Port::kOnboard, PIXY_ADDRESS)
    - i2c = new I2C(I2C::Port::kMXP, PIXY_ADDRESS)

---
##Class Robot

- GetStart(): 
  
  - checks for Pixy to start new frame

- GetWord() / GetByte(): 
  
  - gets information from Pixy - either 2 bytes or a byte (respectively)

- **GetBlocks()**

  - **This Piece of code is extremely important!**
  - Make sure to understand this function in conjuction of GetStart() function :)

- FollowBlock()
  
  - Makes Servo goes left and right based on x-coordinate of an object

**LAST UPDATED: 3/30/2016**
