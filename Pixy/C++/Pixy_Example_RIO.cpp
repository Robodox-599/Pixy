#include "WPILib.h"
//#include "PixyCam.h"

#define PIXY_I2C_DEFAULT_ADDR           0x54

// Communication/misc parameters
#define PIXY_INITIAL_ARRAYSIZE      30
#define PIXY_MAXIMUM_ARRAYSIZE      130
#define PIXY_START_WORD             0xaa55
#define PIXY_START_WORD_CC          0xaa56
#define PIXY_START_WORDX            0x55aa
#define PIXY_MAX_SIGNATURE          7
#define PIXY_DEFAULT_ARGVAL         0xffff

// Pixy x-y position values
#define PIXY_MIN_X                  0L
#define PIXY_MAX_X                  319L
#define PIXY_MIN_Y                  0L
#define PIXY_MAX_Y                  199L

// RC-servo values
#define PIXY_RCS_MIN_POS            0L
#define PIXY_RCS_MAX_POS            1000L
#define PIXY_RCS_CENTER_POS         ((PIXY_RCS_MAX_POS-PIXY_RCS_MIN_POS)/2)

enum BlockType
{
	NORMAL_BLOCK,
	CC_BLOCK
};

struct Block
{
  // print block structure!
  void print()
  {
    int i, j;
    char buf[128], sig[6], d;
	bool flag;
    if (signature>PIXY_MAX_SIGNATURE) // color code! (CC)
	{
      // convert signature number to an octal string
      for (i=12, j=0, flag=false; i>=0; i-=3)
      {
        d = (signature>>i)&0x07;
        if (d>0 && !flag)
          flag = true;
        if (flag)
          sig[j++] = d + '0';
      }
      sig[j] = '\0';
      printf("CC block! sig: %s (%d decimal) x: %d y: %d width: %d height: %d angle %d\n", sig, signature, x, y, width, height, angle);
    }
	else // regular block.  Note, angle is always zero, so no need to print
      printf("sig: %d x: %d y: %d width: %d height: %d\n", signature, x, y, width, height);
    //Serial.print(buf);
  }
  uint16_t signature;
  uint16_t x; //0 - 320
  uint16_t y; //0 - 200
  uint16_t width;
  uint16_t height;
  uint16_t angle;
};

class Robot: public IterativeRobot
{
private:


	I2C* i2c;
	Servo*servo;
	SmartDashboard * dash;

	uint16_t pixy;

	BlockType blockType;
	bool  skipStart;
	uint16_t blockCount;
	uint16_t blockArraySize;
	Block blocks[100];


	//PixyCam* pixy;

	void RobotInit()
	{
		i2c = new I2C(I2C::Port::kOnboard, 0x54);
		//pixy = new PixyCam;
		servo = new Servo(0);
	}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{

	}


	bool getStart()
		{
		  uint16_t w, lastw;

		  lastw = 0xffff;

		  while(true)
		  {
		    w = getWord();
		    if (w==0 && lastw==0)
			{
		      //delayMicroseconds(10);
			  return false;
			}
		    else if (w==PIXY_START_WORD && lastw==PIXY_START_WORD)
			{
		      blockType = NORMAL_BLOCK;
		      return true;
			}
		    else if (w==PIXY_START_WORD_CC && lastw==PIXY_START_WORD)
			{
		      blockType = CC_BLOCK;
		      return true;
			}
			else if (w==PIXY_START_WORDX)
			{
			  printf("Pixy: reorder");
			  getByte(); // resync
			}
			lastw = w;
		  }
		}

	uint16_t getWord()
	{
		unsigned char buffer[2] = {0, 0};

		i2c->ReadOnly(2, buffer);
		return (buffer[1] << 8) | buffer[0];
	}

	uint8_t getByte()
	{
		unsigned char buffer[1] = {0};

		i2c->ReadOnly(1, buffer);
		return buffer[0];
	}

	uint16_t getBlocks(uint16_t maxBlocks)
	{
	  blocks[0] = {0};
	  uint8_t i;
	  uint16_t w, checksum, sum;
	  Block *block;

	  //printf("--Get Blocks");

	  if (!skipStart)
	  {
	    if (getStart()==false)
	      return 0;
	  }
	  else
		skipStart = false;

	  for(blockCount=0; blockCount<maxBlocks && blockCount<PIXY_MAXIMUM_ARRAYSIZE;)
	  {
	    checksum = getWord();
	    if (checksum==PIXY_START_WORD) // we've reached the beginning of the next frame
	    {
	      skipStart = true;
		  blockType = NORMAL_BLOCK;
		  //Serial.println("skip");
	      return blockCount;
	    }
		else if (checksum==PIXY_START_WORD_CC)
		{
		  skipStart = true;
		  blockType = CC_BLOCK;
		  return blockCount;
		}
	    else if (checksum==0)
	      return blockCount;

		//if (blockCount>blockArraySize)
			//resize();

		block = blocks + blockCount;

	    for (i=0, sum=0; i<sizeof(Block)/sizeof(uint16_t); i++)
	    {
		  if (blockType==NORMAL_BLOCK && i>=5) // skip
		  {
			block->angle = 0;
			break;
		  }
	      w = getWord();
	      sum += w;
	      *((uint16_t *)block + i) = w;
	    }
	    if (checksum==sum)
	      blockCount++;
	    else
	      printf("Pixy: cs error");

		w = getWord();
		if (w==PIXY_START_WORD)
		  blockType = NORMAL_BLOCK;
		else if (w==PIXY_START_WORD_CC)
		  blockType = CC_BLOCK;
		else
	      return blockCount;
	  }
	}


	void followBlock()
	{
		if(blocks->signature == 1)
		{
			if(blocks->x > 160)
			{
				servo->Set(0);
			}
			else if(blocks->x < 320 || blocks->x > 160)
			{
				servo->Set(180);
			}
		}
		else
		{
		 return;
		}
	}

	void TeleopPeriodic()
	{
		uint16_t blah = getBlocks(100);
		printf("blocks: ");printf("%d", blah);printf("\n");
		blocks[0].print();
		printf("\n");
		//Wait(0.5);

		followBlock();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
