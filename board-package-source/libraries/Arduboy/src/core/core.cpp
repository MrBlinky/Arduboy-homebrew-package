#include "core.h"

// need to redeclare these here since we declare them static in .h
volatile uint8_t *ArduboyCore::mosiport, 
  /* *ArduboyCore::csport, */ *ArduboyCore::dcport;
uint8_t ArduboyCore::mosipinmask, 
  ArduboyCore::cspinmask, ArduboyCore::dcpinmask;

const uint8_t PROGMEM pinBootProgram[] = {
  // buttons
  PIN_LEFT_BUTTON, INPUT_PULLUP,
  PIN_RIGHT_BUTTON, INPUT_PULLUP,
  PIN_UP_BUTTON, INPUT_PULLUP,
  PIN_DOWN_BUTTON, INPUT_PULLUP,
  PIN_A_BUTTON, INPUT_PULLUP,
  PIN_B_BUTTON, INPUT_PULLUP,

  // OLED SPI
  DC, OUTPUT,
  CS, OUTPUT,
  RST, OUTPUT,
  0
};

#ifdef OLED_SSD1306_I2C
#define I2CADDR 0x3c

void i2c_send_byte(uint8_t data) {
  TWDR = data;
  TWCR = _BV(TWINT)  |  _BV(TWEN);
  while( !(TWCR & _BV(TWINT)));
}

void i2c_start() {
  TWCR = _BV(TWINT) | _BV(TWSTA)  | _BV(TWEN);
  while( !(TWCR & _BV(TWINT)));
  i2c_send_byte(I2CADDR<<1);
}

void i2c_stop(void) {
  TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
  while( (TWCR & _BV(TWSTO)));
}
#endif

const uint8_t PROGMEM lcdBootProgram[] = {
    
  // boot defaults are commented out but left here incase they
  // might prove useful for reference
  //
  // Further reading: https://www.adafruit.com/datasheets/SSD1306.pdf

#ifdef OLED_SSD1306_I2C
  // Sets all registers to sane defaults since i2c
  // displays usually havn't a reset input

  // Display Off
  0xAE,

  // Set Display Clock Divisor v = 0xF0
  // default is 0x80
  0xD5, 0xF0,

  // Set Multiplex Ratio v = 0x3F
  0xA8, 0x3F,

  // Set Display Offset v = 0
  0xD3, 0x00,

  // Set Start Line (0)
  0x40,

  // Charge Pump Setting v = enable (0x14)
  // default is disabled
  0x8D, 0x14,

  // Set Segment Re-map (A0) | (b0001)
  // default is (b0000)
  0xA1,

  // Set COM Output Scan Direction
  0xC8,

  // Set COM Pins v
  0xDA, 0x12,

  // Set Contrast v = 0xCF
  0x81, 0xCF,

  // Set Precharge = 0xF1
  0xD9, 0xF1,

  // Set VCom Detect
  0xDB, 0x40,

  // Entire Display ON
  0xA4,

  // Set normal/inverse display
  0xA6,

  // Display On
  0xAF,

  // set display mode = horizontal addressing mode (0x00)
  0x20, 0x00,

  // set col address range
  0x21, 0x00, WIDTH-1,

  // set page address range
  0x22, 0x00, 0x07,
#elif defined(OLED_SH1106)
  0x8D, 0x14,                   // Charge Pump Setting v = enable (0x14)
  0xA1,                         // Set Segment Re-map
  0xC8,                         // Set COM Output Scan Direction
  0x81, 0xCF,                   // Set Contrast v = 0xCF
  0xD9, 0xF1,                   // Set Precharge = 0xF1
  OLED_SET_COLUMN_ADDRESS_LO,   //Set column address for left most pixel 
  0xAF                          // Display On
#elif defined(LCD_ST7565)
  0xC8,                         //SET_COM_REVERSE
  0x28 | 0x7,                   //SET_POWER_CONTROL  | 0x7
  0x20 | 0x5,                   //SET_RESISTOR_RATIO | 0x5
  0x81,                         //SET_VOLUME_FIRST
  0x13,                         //SET_VOLUME_SECOND
  0xAF                          //DISPLAY_ON
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128) || defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
 #if defined(OLED_96X96) || defined(OLED_128X64_ON_96X96)
  0x15, 0x10, 0x3f, //left most 32 pixels are invisible
 #elif defined(OLED_96X96_ON_128X128)
  0x15, 0x08, 0x37, //center 96 pixels horizontally
 #elif defined(OLED_64X128_ON_128X128)
  0x15, 0x10, 0x2f, //center 64 pixels horizontally
 #else
  0x15, 0x00, 0x3f, //Set column start and end address
 #endif
 #if defined (OLED_96X96) 
  0x75, 0x20, 0x7f, //Set row start and end address
 #elif defined (OLED_128X64_ON_96X96) 
  0x75, 0x30, 0x6f, //Set row start and end address
 #elif defined (OLED_128X96)
  0x75, 0x00, 0x5f, //Set row start and end address
 #elif defined(OLED_128X64_ON_128X96)
  0x75, 0x10, 0x4f, //Set row start and end address
 #elif defined(OLED_96X96_ON_128X128) || defined(OLED_128X96_ON_128X128)
  0x75, 0x10, 0x6f, //Set row start and end address to centered 96 lines
 #elif defined(OLED_128X64_ON_128X128)
  0x75, 0x20, 0x5f, //Set row start and end address to centered 64 lines
 #else
  0x75, 0x00, 0x7F, //Set row start and end address to use all 128 lines
 #endif
 #if defined(OLED_64X128_ON_128X128)
  0xA0, 0x51,       //set re-map: split odd-even COM signals|COM remap|column address remap
 #else
  0xA0, 0x55,       //set re-map: split odd-even COM signals|COM remap|vertical address increment|column address remap
 #endif
  0xA1, 0x00,       //set display start line
  0xA2, 0x00,       //set display offset
  //0xA4,           //Normal display
  0xA8, 0x7F,       //Set MUX ratio 128MUX
  //0xB2, 0x23,
  //0xB3, 0xF0,     //set devider clock | oscillator frequency
  0x81, 0xCF,       //Set contrast
  //0xBC, 0x1F,     //set precharge voltage
  //0x82, 0xFE,     //set second Precharge speed
  0xB1, 0x21,       //reset and 1st precharge phase length  phase 2:2 DCLKs, Phase 1: 1 DCLKs
  //0xBB, 0x0F,     //set 2nd precharge period: 15 DCLKs
  //0xbe, 0x1F,     //output level high voltage com signal
  //0xB8, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, //set gray scale table
  0xAF              //Display on
#else
  // for SSD1306 and SSD1309 displays
  //
  // Display Off
  // 0xAE,

  // Set Display Clock Divisor v = 0xF0
  // default is 0x80
  0xD5, 0xF0,

  // Set Multiplex Ratio v = 0x3F
  // 0xA8, 0x3F,

  // Set Display Offset v = 0
  // 0xD3, 0x00,

  // Set Start Line (0)
  // 0x40,
 #if defined OLED_SSD1309
  //Charge Pump command not supported, use two NOPs instead to keep same size and easy patchability
  0xE3, 0xE3,
 #else  
  // Charge Pump Setting v = enable (0x14)
  // default is disabled
  0x8D, 0x14,
 #endif

  // Set Segment Re-map (A0) | (b0001)
  // default is (b0000)
  0xA1,

  // Set COM Output Scan Direction
  0xC8,

  // Set COM Pins v
  // 0xDA, 0x12,

  // Set Contrast v = 0xCF
  0x81, 0xCF,

  // Set Precharge = 0xF1
  0xD9, 0xF1,

  // Set VCom Detect
  // 0xDB, 0x40,

  // Entire Display ON
  // 0xA4,

  // Set normal/inverse display
  // 0xA6,

  // Display On
  0xAF,

  // set display mode = horizontal addressing mode (0x00)
  0x20, 0x00,

  // set col address range
  // 0x21, 0x00, COLUMN_ADDRESS_END,

  // set page address range
  // 0x22, 0x00, PAGE_ADDRESS_END
#endif
};

ArduboyCore::ArduboyCore() {}

void ArduboyCore::boot()
{
  #if F_CPU == 8000000L
  slowCPU();
  #endif

  SPI.begin();
  bootPins();
  bootLCD();

  #ifdef SAFE_MODE
  if (buttonsState() == (LEFT_BUTTON | UP_BUTTON))
    safeMode();
  #endif

  saveMuchPower();
}

#if F_CPU == 8000000L
// if we're compiling for 8Mhz we need to slow the CPU down because the
// hardware clock on the Arduboy is 16MHz
void ArduboyCore::slowCPU()
{
  uint8_t oldSREG = SREG;
  cli();                // suspend interrupts
  CLKPR = _BV(CLKPCE);  // allow reprogramming clock
  CLKPR = 1;            // set clock divisor to 2 (0b0001)
  SREG = oldSREG;       // restore interrupts
}
#endif

void ArduboyCore::bootPins()
{
  uint8_t pin, mode;
  const uint8_t *i = pinBootProgram;

  while(true) {
    pin = pgm_read_byte(i++);
    mode = pgm_read_byte(i++);
    if (pin==0) break;
    pinMode(pin, mode);
  }

  digitalWrite(RST, HIGH);
  delay(1);           // VDD (3.3V) goes high at start, lets just chill for a ms
  digitalWrite(RST, LOW);   // bring reset low
  delay(10);          // wait 10ms
  digitalWrite(RST, HIGH);  // bring out of reset
}

void ArduboyCore::bootLCD()
{
  // setup the ports we need to talk to the OLED
  //csport = portOutputRegister(digitalPinToPort(CS));
  *portOutputRegister(digitalPinToPort(CS)) &= ~cspinmask;
  cspinmask = digitalPinToBitMask(CS);
  dcport = portOutputRegister(digitalPinToPort(DC));
  dcpinmask = digitalPinToBitMask(DC);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
 #if defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
  LCDDataMode();
  for (uint16_t i = 0; i < 8192; i++) SPI.transfer(0); //Clear all display ram
 #endif

  LCDCommandMode();
  // run our customized boot-up command sequence against the
  // OLED to initialize it properly for Arduboy
  for (int8_t i=0; i < sizeof(lcdBootProgram); i++) {
    SPI.transfer(pgm_read_byte(lcdBootProgram + i));
  }
  LCDDataMode();
}

void ArduboyCore::LCDDataMode()
{
  *dcport |= dcpinmask;
  // *csport &= ~cspinmask; 
}

void ArduboyCore::LCDCommandMode()
{
  // *csport |= cspinmask;
  *dcport &= ~dcpinmask;
  // *csport &= ~cspinmask; CS set once at bootLCD
}



void ArduboyCore::safeMode()
{
  blank(); // too avoid random gibberish
  while (true) {
    asm volatile("nop \n");
  }
}


/* Power Management */

void ArduboyCore::idle()
{
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();
}

void ArduboyCore::saveMuchPower()
{
  power_adc_disable();
  power_usart0_disable();
  power_twi_disable();
  // timer 0 is for millis()
  // timers 1 and 3 are for music and sounds
  power_timer2_disable();
  power_usart1_disable();
  // we need USB, for now (to allow triggered reboots to reprogram)
  // power_usb_disable()
}

uint8_t ArduboyCore::width() { return WIDTH; }

uint8_t ArduboyCore::height() { return HEIGHT; }


/* Drawing */

void ArduboyCore::paint8Pixels(uint8_t pixels)
{
  SPI.transfer(pixels);
}

void ArduboyCore::paintScreen(const unsigned char *image)
{ 
#ifdef OLED_SSD1306_I2C
  // I2C
  for (uint8_t i=0; i<(WIDTH*HEIGHT/(16*8));) {
    // send a bunch of data in one xmission
    i2c_start();
    i2c_send_byte(0x40);
    for(uint8_t x=0;x<16;x++,i++) {
      TWDR = pgm_read_byte(image+i);
      TWCR = _BV(TWINT) |  _BV(TWEN);
      while( !(TWCR & _BV(TWINT)));
    }
    i2c_stop();
  }
#elif defined(OLED_SH1106) || defined(LCD_ST7565)
  for (uint8_t i = 0; i < HEIGHT / 8; i++)
  {
  	LCDCommandMode();
  	SPDR = (OLED_SET_PAGE_ADDRESS + i);
	while (!(SPSR & _BV(SPIF)));
  	SPDR = (OLED_SET_COLUMN_ADDRESS_HI); // we only need to reset hi nibble to 0
	while (!(SPSR & _BV(SPIF)));
  	LCDDataMode();
  	for (uint8_t j = WIDTH; j > 0; j--)
      {
  		SPDR = pgm_read_byte(*(image++));
		while (!(SPSR & _BV(SPIF)));
      }
  }
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128) || defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128) || defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128)
 #if defined(OLED_128X64_ON_96X96)
  uint16_t i = 16;
  for (uint8_t col = 0; col < 96 / 2; col++)
 #else     
  uint16_t i = 0;
  for (uint8_t col = 0; col < WIDTH / 2; col++)
 #endif     
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b1 = pgm_read_byte(image + i);
      uint8_t b2 = pgm_read_byte(image + i + 1);
      for (uint8_t shift = 0; shift < 8; shift++)
      {
        uint8_t c = 0xFF;
        if ((b1 & 1) == 0) c &= 0x0F;
        if ((b2 & 1) == 0) c &= 0xF0;
        SPDR = c;
        b1 = b1 >> 1;
        b2 = b2 >> 1;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH - 2;
  }
#elif defined(OLED_64X128_ON_128X128)
  uint16_t i = WIDTH-1;
  for (uint8_t col = 0; col < WIDTH ; col++)
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b = pgm_read_byte(image + i);
      if (clear) *(image + i) = 0;
      for (uint8_t shift = 0; shift < 4; shift++)
      {
        uint8_t c = 0xFF;
        if ((b & _BV(0)) == 0) c &= 0x0F;
        if ((b & _BV(1)) == 0) c &= 0xF0;
        SPDR = c;
        b = b >> 2;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH  + 1;
  }
#else
  for (int i = 0; i < (HEIGHT*WIDTH)/8; i++)
  {
    SPI.transfer(pgm_read_byte(image + i));
  }
#endif
}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void ArduboyCore::paintScreen(unsigned char image[])
{
#ifdef OLED_SSD1306_I2C
  // I2C
  for (uint8_t i=0; i<(WIDTH*HEIGHT/(16*8));) {
    // send a bunch of data in one xmission
    i2c_start();
    i2c_send_byte(0x40);
    for(uint8_t x=0;x<16;x++,i++) {
      TWDR = pgm_read_byte(image+i);
      TWCR = _BV(TWINT) |  _BV(TWEN);
      while( !(TWCR & _BV(TWINT)));
    }
    i2c_stop();
  }
#elif defined(OLED_SH1106) || defined(LCD_ST7565)
  for (uint8_t i = 0; i < HEIGHT / 8; i++)
  {
  	LCDCommandMode();
  	SPDR = (OLED_SET_PAGE_ADDRESS + i);
	while (!(SPSR & _BV(SPIF)));
  	SPDR = (OLED_SET_COLUMN_ADDRESS_HI); // we only need to reset hi nibble to 0
	while (!(SPSR & _BV(SPIF)));
  	LCDDataMode();
  	for (uint8_t j = WIDTH; j > 0; j--)
      {
  		SPDR = *(image++);
		while (!(SPSR & _BV(SPIF)));
      }
  }
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128)|| defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128)
  // 1 bit to 4-bit expander display code with clear support.
  // Each transfer takes 18 cycles with additional 4 cycles for a column change.
  asm volatile(
   #if defined(OLED_128X64_ON_96X96)
    "  adiw   r30, 16                           \n\t"          
   #endif
    "  ldi   r25, %[col]                        \n\t"          
    ".lcolumn:                                  \n\t"         
    "   ldi  r24, %[row]            ;1          \n\t"
    ".lrow:                                     \n\t"
    "   ldi  r21, 7                 ;1          \n\t"
    "   ld   r22, z                 ;2          \n\t"
    "   ldd  r23, z+1               ;2          \n\t"
    ".lshiftstart:                              \n\t"
    "   ldi  r20, 0xFF              ;1          \n\t"
    "   sbrs r22, 0                 ;1          \n\t"
    "   andi r20, 0x0f              ;1          \n\t"
    "   sbrs r23, 0                 ;1          \n\t"
    "   andi r20,0xf0               ;1          \n\t"
    "   out  %[spdr], r20           ;1          \n\t"
    "                                           \n\t"
    "   cp   %[clear], __zero_reg__ ;1          \n\t"
    "   brne .lclear1               ;1/2        \n\t"
    ".lshiftothers:                             \n\t"
    "   movw r18, %A[ptr]           ;1          \n\t"
    "   rjmp .+0                    ;2          \n\t"
    "   rjmp .lshiftnext            ;2          \n\t"
    ".lclear1:                                  \n\t"
    "   st   z, __zero_reg__        ;2          \n\t" 
    "   std  z+1, __zero_reg__      ;2          \n\t"
    ".lshiftnext:                               \n\t"
    "                                           \n\t"
    "   lsr  r22                    ;1          \n\t"
    "   lsr  r23                    ;1          \n\t"
    "                                           \n\t"
    "   ldi  r20, 0xFF              ;1          \n\t"
    "   sbrs r22, 0                 ;1/2        \n\t"
    "   andi r20, 0x0f              ;1          \n\t"
    "   sbrs r23, 0                 ;1/2        \n\t"
    "   andi r20,0xf0               ;1          \n\t"
    "                                           \n\t"
    "   subi r18, %[top_lsb]        ;1          \n\t" //image - (WIDTH * ((HEIGHT / 8) - 1) - 2)
    "   sbci r19, %[top_msb]        ;1          \n\t"
    "   subi r21, 1                 ;1          \n\t"
    "   out  %[spdr], r20           ;1          \n\t"
    "   brne .lshiftothers          ;1/2        \n\t"
    "                                           \n\t"
    "   nop                         ;1          \n\t"
    "   subi %A[ptr], %[width]      ;1          \n\t" //image + width (negated addition)
    "   sbci %B[ptr], -1            ;1          \n\t"
    "   subi r24, 1                 ;1          \n\t"
    "   brne .lrow                  ;1/2        \n\t"
    "                                           \n\t"
    "   movw %A[ptr], r18           ;1          \n\t"
    "   subi r25, 1                 ;1          \n\t"
    "   brne .lcolumn               ;1/2        \n\t"
    "   in    __tmp_reg__, %[spsr]              \n\t" //read SPSR to clear SPIF
    : [ptr]     "+&z" (image)
    : [spdr]    "I" (_SFR_IO_ADDR(SPDR)),
      [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
      [row]     "M" (HEIGHT / 8),
     #if defined(OLED_128X64_ON_96X96)
      [col]     "M" (96 / 2),
     #else
      [col]     "M" (WIDTH / 2),
     #endif
      [width]   "M" (256 - WIDTH),
      [top_lsb] "M" ((WIDTH * ((HEIGHT / 8) - 1) - 2) & 0xFF),
      [top_msb] "M" ((WIDTH * ((HEIGHT / 8) - 1) - 2) >> 8),
      [clear]   "r" (0)
    : "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25"
  );
#elif defined(OLED_64X128_ON_128X128)
  uint16_t i = WIDTH-1;
  for (uint8_t col = 0; col < WIDTH ; col++)
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b = *(image + i);
      if (clear) *(image + i) = 0;
      for (uint8_t shift = 0; shift < 4; shift++)
      {
        uint8_t c = 0xFF;
        if ((b & _BV(0)) == 0) c &= 0x0F;
        if ((b & _BV(1)) == 0) c &= 0xF0;
        SPDR = c;
        b = b >> 2;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH  + 1;
  }
#else
  uint8_t c;
  int i = 0;

  SPDR = image[i++]; // set the first SPI data byte to get things started

  // the code to iterate the loop and get the next byte from the buffer is
  // executed while the previous byte is being sent out by the SPI controller
  while (i < (HEIGHT * WIDTH) / 8)
  {
    // get the next byte. It's put in a local variable so it can be sent as
    // as soon as possible after the sending of the previous byte has completed
    c = image[i++];

    while (!(SPSR & _BV(SPIF))) { } // wait for the previous byte to be sent

    // put the next byte in the SPI data register. The SPI controller will
    // clock it out while the loop continues and gets the next byte ready
    SPDR = c;
  }
  while (!(SPSR & _BV(SPIF))) { } // wait for the last byte to be sent
#endif
}

void ArduboyCore::blank()
{
#ifdef OLED_SSD1306_I2C
  TWSR = 0;
  TWBR = F_CPU/(2*100000)-8;

  i2c_start();
  i2c_send_byte(0x00);
  for (uint8_t i = 0; i < sizeof(lcdBootProgram); i++) 
    i2c_send_byte(pgm_read_byte(lcdBootProgram + i));
  i2c_stop();
  
  //  TWBR = F_CPU/(2*400000)-8;
  TWBR = 1; // 12 = 400kHz
#elif OLED_SH1106 
  for (int i = 0; i < (HEIGHT * 132) / 8; i++)
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
  for (int i = 0; i < (HEIGHT * WIDTH) / 2; i++)
#else //OLED SSD1306 and compatibles
  for (int i = 0; i < (HEIGHT * WIDTH) / 8; i++)
#endif

#ifndef OLED_SSD1306_I2C
    SPItransfer(0x00);
#endif
}

void ArduboyCore::sendLCDCommand(uint8_t command)
{
  LCDCommandMode();
  SPI.transfer(command);
  LCDDataMode();
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void ArduboyCore::invert(boolean inverse)
{
  sendLCDCommand(inverse ? OLED_PIXELS_INVERTED : OLED_PIXELS_NORMAL);
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void ArduboyCore::allPixelsOn(boolean on)
{
  sendLCDCommand(on ? OLED_ALL_PIXELS_ON : OLED_PIXELS_FROM_RAM);
}

// flip the display vertically or set to normal
void ArduboyCore::flipVertical(boolean flipped)
{
  sendLCDCommand(flipped ? OLED_VERTICAL_FLIPPED : OLED_VERTICAL_NORMAL);
}

#define OLED_HORIZ_FLIPPED 0xA0 // reversed segment re-map
#define OLED_HORIZ_NORMAL 0xA1 // normal segment re-map

// flip the display horizontally or set to normal
void ArduboyCore::flipHorizontal(boolean flipped)
{
  sendLCDCommand(flipped ? OLED_HORIZ_FLIPPED : OLED_HORIZ_NORMAL);
}

/* RGB LED */

void ArduboyCore::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ARDUBOY_10 // RGB, all the pretty colors
 #if defined(LCD_ST7565)
  if ((red | green | blue) == 0) //prevent backlight off 
  {
    red   = 255;
    green = 255;
    blue  = 255;
  }
  analogWrite(RED_LED, red);
  analogWrite(GREEN_LED, green);
  analogWrite(BLUE_LED, blue);
 #else
  // inversion is necessary because these are common annode LEDs
  analogWrite(RED_LED, 255 - red);
  analogWrite(GREEN_LED, 255 - green);
  analogWrite(BLUE_LED, 255 - blue);
 #endif
#elif defined(AB_DEVKIT)
  // only blue on devkit
  digitalWrite(BLUE_LED, ~blue);
#endif
}

/* Buttons */

uint8_t ArduboyCore::getInput()
{
  return buttonsState();
}


uint8_t ArduboyCore::buttonsState()
{
  uint8_t buttons;
  
  // using ports here is ~100 bytes smaller than digitalRead()
#ifdef AB_DEVKIT
  // down, left, up
  buttons = ((~PINB) & B01110000);
  // right button
  //buttons = buttons | (((~PINC) & B01000000) >> 4);
  if ((PINC & B01000000) == 0) buttons |= 0x04; //compiles to shorter and faster code
  // A and B
  //buttons = buttons | (((~PINF) & B11000000) >> 6);
  if ((PINF & B10000000) == 0) buttons |= 0x02; //compiles to shorter and faster code
  if ((PINF & B01000000) == 0) buttons |= 0x01; 
#elif defined(ARDUBOY_10)
  // down, up, left right
  buttons = ((~PINF) & B11110000);
  // A (left)
  //buttons = buttons | (((~PINE) & B01000000) >> 3);
  if ((PINE & B01000000) == 0) {buttons |= 0x08;} //compiles to shorter and faster code
  // B (right)
  //buttons = buttons | (((~PINB) & B00010000) >> 2);
  if ((PINB & B00010000) == 0) {buttons |= 0x04;} //compiles to shorter and faster code
#endif
  
  return buttons;
}
