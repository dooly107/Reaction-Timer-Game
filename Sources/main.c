/*
************************************************************************
 ECE 362 - Mini-Project C Source File - Spring 2017
***********************************************************************
	 	   			 		  			 		  		
 Team ID: < 17 >

 Project Name: < Music Reaction Game >

 Team Members:

   - Team/Doc Leader: < Trey Overholser >   Signature: _Trey Overholser_____
   
   - Software Leader: < Sang Hun Kim >      Signature: _Sang Hun Kim______

   - Interface Leader: < Taekkyun Kim >     Signature: _Taekkyun Kim_________

   - Peripheral Leader: < Sang Hun Kim >    Signature: _Sang Hun Kim_______


 Academic Honesty Statement:  In signing above, we hereby certify that we 
 are the individuals who created this HC(S)12 source file and that we have
 not copied the work of any other student (past or present) while completing 
 it. We understand that if we fail to honor this agreement, we will receive 
 a grade of ZERO and be subject to possible disciplinary action.

***********************************************************************
                                            
 The objective of this Mini-Project is to take in any music from the external device,
 divide the input signal into three different channels (Bass, Mid, Treble). Once the microprocessor (9S12)
 recognizes the input signal, the reaction timer starts. Based on the speed that led should light up on each column
 is controlled by the potentiometer, which 9S12 recognizes it as analog signal and converts it to digital signal.
 Once the last led on each row lits up, the player should press the pushbuttons to measure its reaction time. If the
 user pressed the button within the certain window of reaction time, then the lcd displays the "Good" sign
 with how many "Goods" that the user have. If the user presses the button when the reaction time is not within the
 certain range, then "Bad" sign shows up with how many "Bads" that user have.
 Player can manually reset the scores and start a new game with the toggle switch.
 Also, how frequent that leds should show up can also manually be controlled by three potentiometers next to leds.
 There are total of five difficulty levels that players can play, in which it is controlled by the potentiometer
 as well.


***********************************************************************

 List of project-specific success criteria (functionality that will be
 demonstrated):

 1. To implement various peripherals that are availble
    a. RTI: 
      1. Detect when the pushbuttons are pressed.
      2. Control the falling speed of leds on each row. (Levels of the difficulty)
      3. Control how frequently the leds should be appearing.
      4. Reset and start the new game by the toggle switch.
    b. SPI:
      1. LCD display for the prompts and each score displays whether
      the player has "Good" or "Bad"
    c. TIM:
      1. Set the range (window) for the reaction timer.
      2. Triggers the reaction timer when the 9S12 recognizes that input signals are
      converted successfully.
      3. Timer interrupt occurs every 1ms.
    d. ATD:
      1. Converts all the analog signal to digital signals
        a) Bass, Mid, Treble input signal from external music device
        b) Speed of the falling leds on each row.
    e. PWM:
      1. Instead of using Timer Module, used PWM to clear off any outputs that are retained
        after the game has been reset.

 2. Demonstrate successful programming on the microcontroller (9S12)
    a. Successfully Resets and starts the new game once the toggle switch has been toggled.
    b. Successfully display the scores that player has received on the LCD display.
    c. Successfully catches the reaction time based on when the user has pressed the pushbutton.
    d. Successfully control the speed of the LED lights falling on each LED rows.
    e. Successfully manage all the program in a concise and understandable manner.

 3. Demonstrate the understanding of wiring the circuit.
    a. Use PCB design to successfully derive the analog input signal to the microprocessor.
    b. Connect all the wires to correct places of the microprocessor to prevent malfunctioning.
    c. Use less wires and connections as possible so that the design looks neat and concise.

 4. To gain valuable experience in teamwork
    a. Obtain and share any knowledges or information that one of the teammates know while others do not.
    b. Collaborate and understand the importance of teamwork.
    c. Understand closer or even beyond the course related materials.

***********************************************************************

  Date code started: < 4/18/2017 >

  Update history (add an entry every time a significant change is made):

  Date: < 4/18/2017 >  Name: < Sang Hun Kim >   Update: < Initialize >

  Date: < 4/20/2017 >  Name: < Sang Hun Kim >   Update: < Finished up the rough skeleton draft >

  Date: < 4/21/2017 >  Name: < Sang Hun Kim >   Update: < Set the range for the reaction timer >
  
  Date: < 4/23/2017 >  Name: < Sang Hun Kim >   Update: < Calculate for the values of analog signals >
  
  Date: < 4/25/2017 >  Name: < Sang Hun Kim >   Update: < Change the values for RTI and TIM values >
  
  Date: < 4/26/2017 >  Name: < Sang Hun Kim >   Update: < Created count_gd_bd function >
  
  Date: < 4/27/2017 >  Name: < Sang Hun Kim >   Update: < Adjust range for the reaction timer >
  
  Date: < 4/28/2017 >  Name: < Sang Hun Kim >   Update: < Added lcddisp function >
  
  Date: < 4/29/2017 >  Name: < Sang Hun Kim >   Update: < Adjusted all the values and finalized coding >
  
  


***********************************************************************
*/
 
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>

/* All funtions after main should be initialized here */
char inchar(void);
void outchar(char);
void shiftout(char);
void lcdwait(void);
void send_byte(char);
void send_i(char);
void chgline(char);
void print_c(char);
void pmsglcd(char[]);
void tdelay(void);
void tdelay_speed(int);
void lcddisp(int, int, int);

/*  Variable declarations */ 	   			 		  			 		       
int prevpbbass = 0;
int prevpbmid = 0;
int prevpbtreble = 0;
int pbbass = 0;
int pbmid = 0;
int pbtreble = 0;
int runstp = 0;
int count_good = 0;
int count_bad = 0;
int bass_push_io = 0;
int mid_push_io = 0;
int treble_push_io = 0;
int thresh = 0;
int count_time_delay = 0;
int prevpbstart = 0;
int pbstart = 0;
int cycle = 0;
int speed_pot = 0;
int speed_digital = 0;
int count_speed = 0;
int tim_start = 0;

int no_interrupt_bass = 0;
int no_interrupt_mid = 0;
int no_interrupt_treble = 0;

//reaction timer variables
int react_bass_enable = 0;
int react_mid_enable = 0;
int react_treble_enable = 0;
int react_bass = 0;
int react_mid = 0;
int react_treble = 0;
int cutoff = 0;
int actual_bass = 0;
int actual_mid = 0;
int actual_treble = 0;
int press_bass = 0;
int press_mid = 0;
int press_treble = 0;




/* LCD COMMUNICATION BIT MASKS */
#define RS 0x04		// RS pin mask (PTT[2])
#define RW 0x08		// R/W pin mask (PTT[3])
#define LCDCLK 0x10	// LCD EN/CLK pin mask (PTT[4])

/* LCD INSTRUCTION CHARACTERS */
#define LCDON 0x0F	// LCD initialization command
#define LCDCLR 0x01	// LCD clear display command
#define TWOLINE 0x38	// LCD 2-line enable command
#define CURMOV 0xFE	// LCD cursor move instruction
#define LINE1 0x80	// LCD line 1 cursor position
#define LINE2 0xC0	// LCD line 2 cursor position

/* ADDITIONAL DECLARATIONS*/
#define LEVEL1 0x2A
#define LEVEL2 0x55
#define LEVEL3 0x80
#define LEVEL4 0xAA
#define LEVEL5 0xD5


/*	 	   		
***********************************************************************
 Initializations
***********************************************************************
*/

void  initializations(void) {

/* Set the PLL speed (bus clock = 24 MHz) */
  CLKSEL = CLKSEL & 0x80; //; disengage PLL from system
  PLLCTL = PLLCTL | 0x40; //; turn on PLL
  SYNR = 0x02;            //; set PLL multiplier
  REFDV = 0;              //; set PLL divider
  while (!(CRGFLG & 0x08)){  }
  CLKSEL = CLKSEL | 0x80; //; engage PLL

  MODRR = 0x01;   //PT0 used as PWM Ch 0 output
  PWME = 0x01;    //enable PWM Ch 0
  PWMPOL = 0x01;  //set active high polarity
  PWMCTL = 0x00;  //no concatenate (8-bit)
  PWMCAE = 0x00;  //left-aligned output mode
  PWMPER0 = 0xFF; //set maximum 8-bit period
  PWMDTY0 = 0x7F; //initially set to maximum DUTY register
  PWMCLK = 0x01;  // select Clock SA for Ch 0
  PWMSCLA = 0x00; // set Clock SA scalar to 512 (max scalar)
  PWMPRCLK = 0x07;// set Clock SA to maximum
  
  

/* Disable watchdog timer (COPCTL register) */
  COPCTL = 0x40   ; //COP off; RTI and COP stopped in BDM-mode

/* Initialize asynchronous serial port (SCI) for 9600 baud, no interrupts */
  SCIBDH =  0x00; //set baud rate to 9600
  SCIBDL =  0x9C; //24,000,000 / 16 / 156 = 9600 (approx)  
  SCICR1 =  0x00; //$9C = 156
  SCICR2 =  0x0C; //initialize SCI for program-driven operation
  DDRB   =  0x10; //set PB4 for output mode
  PORTB  =  0x10; //assert DTR pin on COM port

        
/* Initialize the SPI to 6 Mbs */
  SPIBR = 0x01;
  SPICR1 = 0x50;
  SPICR2 = 0x00;
	 	   			 		  			 		  		
/* Initialize digital I/O port pins */
  DDRT = 0xFF;
  DDRM = 0x30;
  DDRAD = 0;

/* Initialize the LCD
     - pull LCDCLK high (idle)
     - pull R/W' low (write state)
     - turn on LCD (LCDON instruction)
     - enable two-line mode (TWOLINE instruction)
     - clear LCD (LCDCLR instruction)
     - wait for 2ms so that the LCD can wake up     
*/ 
  PTT_PTT4 = 1;
  PTT_PTT3 = 0;
  send_i(LCDON);
  send_i(TWOLINE);
  send_i(LCDCLR);
  chgline(LINE1);
  pmsglcd("Please Wait...");
  lcdwait();


/* Initialize RTI for 2.048 ms interrupt rate */
  RTICTL = 0x1F;
  CRGINT = CRGINT | 0x80;	

/* Initialize TIM Ch 7 (TC7) for periodic interrupts every 1.000 ms
     - enable timer subsystem
     - set channel 7 for output compare
     - set appropriate pre-scale factor and enable counter reset after OC7
     - set up channel 7 to generate 1 ms interrupt rate
     - initially disable TIM Ch 7 interrupts      
*/
  TSCR1 = 0x80;
  TSCR2 = 0x0C;
  TIOS = 0x80;
  TC7 = 1500;
  
//ATD INITIALIZATION
  ATDCTL2 = 0x80;
  ATDCTL3 = 0x20;
  ATDCTL4 = 0x85;
  //ATDDIEN = 0xB8; //Digital I/O pins (PORTAD0_PTAD3,4,5,7)       
  ATDDIEN = 0xF0;   //Digital I/O pins (PORTAD0_PTAD4,5,6,7)
//Variable Declarations
  thresh = 0x03;
  

  mid_push_io = PORTAD0_PTAD4; //PUSH BUTTON FOR MID SIGNAL
  treble_push_io = PORTAD0_PTAD5; //PUSH BUTTON FOR TREBLE SIGNAL
  bass_push_io = PORTAD0_PTAD6; //PUSH BUTTON FOR BASS SIGNAL
   
 
	      
}

/*	 		  			 		  		
***********************************************************************
 Main
***********************************************************************
*/

void main(void) {
  	DisableInterrupts;
	initializations(); 		  			 		  		
	EnableInterrupts;



  for(;;) {
   if (pbstart == 0) {
      PTT_PTT5 = 0;
      PTT_PTT6 = 0;
      PTT_PTT7 = 0;      
   }
   if (pbstart == 1) {
     pbstart = 0;
     runstp = 1;
     tim_start = 1; 
     TIE = 0x80;
     send_i(LCDCLR);
     chgline(LINE1);
     pmsglcd("Get Ready!!");
     tdelay();
     send_i(LCDCLR);
     chgline(LINE1);
     pmsglcd("Set...");
     tdelay();
     send_i(LCDCLR);
     chgline(LINE1);
     pmsglcd("GO!!!");

     tdelay();
     send_i(LCDCLR);
     chgline(LINE1);
     pmsglcd("GOOD = 000");
     chgline(LINE2);
     
     pmsglcd("BAD  = 000");
     ATDCTL5 = 0x10;
     MODRR = 0x00;
     react_bass = 0;
     react_mid = 0;
     react_treble = 0;
     count_good = 0;
     count_bad = 0;
     tim_start = 0;

    }
  
   if (runstp == 1) {
  
      //SPEED CONTROLLER  
     speed_pot = ATDDR3H;
     if ((speed_pot >= 0x00) && (speed_pot < LEVEL1)) {
      speed_digital = 1;
     } 
     if((speed_pot >= LEVEL1) && (speed_pot < LEVEL2)) {
      speed_digital = 2;
     }
     if((speed_pot >= LEVEL2) && (speed_pot < LEVEL3)) {
      speed_digital = 3;
     }  
     if((speed_pot >= LEVEL3) && (speed_pot < LEVEL4)) {
      speed_digital = 4;
     } 
     if (((speed_pot >= LEVEL4) && (speed_pot < LEVEL5)) || (speed_pot >= LEVEL5)) {
      speed_digital = 5;
     }
     
     //LED SHIFT REGISTER
    if (speed_digital) {
  
      if (ATDDR0H >= thresh) {
        PTT_PTT5 = 1;
      } 
      else { 
        PTT_PTT5 = 0;
      }
      if (ATDDR1H >= thresh) {
        PTT_PTT6 = 1;
      } 
      else { 
        PTT_PTT6 = 0;
      }
      if (ATDDR2H >= thresh) {
        PTT_PTT7 = 1;
      } 
      else { 
        PTT_PTT7 = 0;
      }


      if (no_interrupt_bass != 0) {
        no_interrupt_bass -= 1;
        PTT_PTT5 = 0;
      }
      else if (PTT_PTT5 == 1) {
        no_interrupt_bass = 4;
      } 
      else {
        no_interrupt_bass = 0;
      }
      if (no_interrupt_mid != 0) {
        no_interrupt_mid -= 1;
        PTT_PTT6 = 0; 
      }
      else if (PTT_PTT6 == 1) {
        no_interrupt_mid = 4;
      }
      else {
        no_interrupt_mid = 0;
      }
      if (no_interrupt_treble != 0) {
        no_interrupt_treble -= 1;
        PTT_PTT7 = 0;
      }
      else if (PTT_PTT7 == 1) {
        no_interrupt_treble = 4;
      }
      else {
        no_interrupt_treble = 0;
      }

      PTT_PTT0 = 1;
      tdelay_speed(speed_digital);
      PTT_PTT0 = 0;
      tdelay_speed(speed_digital);
    }
    
    if (pbbass == 1) {
      lcddisp(1,0,0);
      pbbass = 0;
      press_bass = 1;
    }
    if (pbmid == 1) {
      lcddisp(0,1,0);
      pbmid = 0;
      press_mid = 0;
    }
    if (pbtreble == 1) {
      lcddisp(0,0,1);
      pbtreble = 0;
      press_treble = 1;
    }
     
   }

 
  }/* loop forever */
   
}  /* do not leave main */




/*
***********************************************************************
 RTI interrupt service routine: RTI_ISR
***********************************************************************
*/

interrupt 7 void RTI_ISR(void)
{
  	// clear RTI interrupt flag
  	CRGFLG = CRGFLG | 0x80; 
      	
  	if ((PORTAD0_PTAD7 + prevpbstart) == 0) {
  	  prevpbstart = 1;
  	  pbstart = 1;
  	} 
  	else if (PORTAD0_PTAD7) {
  	  prevpbstart = 0;
  	}
    if ((PORTAD0_PTAD6 + prevpbbass) == 0) {
      prevpbbass = 1;
      pbbass = 1;
      actual_bass = react_bass;
    } 
    else if (PORTAD0_PTAD6) {
      prevpbbass = 0;
    }
    
    if ((PORTAD0_PTAD4 + prevpbmid) == 0) {
      prevpbmid = 1;
      pbmid = 1;
      actual_mid = react_mid;
    } 
    else if (PORTAD0_PTAD4) {
      prevpbmid = 0;
    }
    
    if ((PORTAD0_PTAD5 + prevpbtreble) == 0) {
      prevpbtreble = 1;
      pbtreble = 1;
      actual_treble = react_treble;
    } 
    else if (PORTAD0_PTAD5) {
      prevpbtreble = 0;
    }  
 
}

/*
***********************************************************************
  TIM interrupt service routine
  used to initiate ATD samples (on Ch 0 and Ch 1)	 		  			 		  		
***********************************************************************
*/

interrupt 15 void TIM_ISR(void)
{

        // clear TIM CH 7 interrupt flag 
 	TFLG1 = TFLG1 | 0x80;
 	ATDCTL5 = 0x10;
 	count_time_delay++;
 	count_speed++;
 	
 	while (!ATDSTAT0_SCF) {
 	}
  
  if (speed_digital == 1) {
    cutoff = 5001;
  } 
  else if (speed_digital == 2) {
    cutoff = 2501; 
  }
  else if (speed_digital == 3) {
    cutoff = 1001; 
  } 
  else if (speed_digital == 4) {
    cutoff = 501; 
  } 
  else if (speed_digital == 5) {
    cutoff = 101;
  }
  
  
 	if (ATDDR2H >= thresh) {
    react_treble_enable = 1;
 	}
 	if (ATDDR1H >= thresh) {
 	  react_mid_enable = 1;
 	}
 	if (ATDDR0H >= thresh) {
 	  react_bass_enable = 1;
 	}
 	if (react_treble_enable == 1) {
 	  react_treble++;
 	  if ((react_treble > cutoff) || (press_treble == 1)) {
 	    react_treble = 0;
 	  } 
 	}
 	if (react_mid_enable == 1) {
 	  react_mid++;
 	 	if ((react_mid > cutoff)) {
 	    react_mid = 0;
 	  }
 	}
 	if (react_bass_enable == 1) {
 	  react_bass++;
 	 	if ((react_bass > cutoff)) {
 	    react_bass = 0;
 	  }
 	}
 
}

/*
***********************************************************************  
DISP COUNT GOOD, COUNT BAD		 		  		
***********************************************************************
*/

void count_gd_bd(int good, int bad) {
  if (good == 1) {
    count_good++;
    send_i(LCDCLR);
    chgline(LINE1);
    send_i(LINE1);
    pmsglcd("GOOD = ");
    print_c(((count_good / 100) % 10) + 0x30);//Hundreds
    print_c(((count_good / 10) % 10) + 0x30); //Tenths
    print_c((count_good % 10) + 0x30);        //Oneths
    chgline(LINE2);
    send_i(LINE2);
    pmsglcd("BAD  = ");
    print_c(((count_bad / 100) % 10) + 0x30);//Hundreds
    print_c(((count_bad / 10) % 10) + 0x30); //Tenths
    print_c((count_bad % 10) + 0x30);        //Oneths
  }
  if (bad == 1) {
    count_bad++;
    send_i(LCDCLR);   
    chgline(LINE1);
    send_i(LINE1);
    pmsglcd("GOOD = ");
    print_c((count_good / 100) + 0x30);       //Hundreds
    print_c(((count_good / 10) % 10) + 0x30); //Tenths
    print_c((count_good % 10) + 0x30);        //Oneths
    chgline(LINE2);
    send_i(LINE2);
    pmsglcd("BAD  = ");
    print_c((count_bad / 100) + 0x30);       //Hundreds
    print_c(((count_bad / 10) % 10) + 0x30); //Tenths
    print_c((count_bad % 10) + 0x30);        //Oneths
  }
}

/*
***********************************************************************  
LCD DISPLAY		 		  		
***********************************************************************
*/

void lcddisp(int pbbass, int pbmid, int pbtreble) {
  if (pbbass == 1) {
   if (speed_digital == 1) {
     if (((1000 >= actual_bass - 4000) && (0 <= actual_bass - 4000)) || ((-1000 <= actual_bass - 4000) && (0 >= actual_bass - 4000))) {
        count_gd_bd(1,0);     
     } 
     else if (actual_bass == 5000) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }  
   }
   if (speed_digital == 2) {
     if (((500 >= actual_bass - 2000) && (0 <= actual_bass - 2000)) || ((-500 <= actual_bass - 2000) && (0 >= actual_bass - 2000))) {
        count_gd_bd(1,0);
     }
     else if (actual_bass == 2500) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 3) {
     if (((200 >= actual_bass - 800) && (0 <= actual_bass - 800)) || ((-200 <= actual_bass - 800) && (0 >= actual_bass - 800))) {
        count_gd_bd(1,0);
     }
     else if (actual_bass == 1000) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 4) {
     if (((100 >= actual_bass - 400) && (0 <= actual_bass - 400)) || ((-100 <= actual_bass - 400) && (0 >= actual_bass - 400))) {
        count_gd_bd(1,0);                                     
     }
     else if (actual_bass == 500) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 5) {
     if (((20 >= actual_bass - 80) && (0 <= actual_bass - 80)) || ((-20 <= actual_bass - 80) && (0 >= actual_bass - 80))) {
        count_gd_bd(1,0);
     }
     else if (actual_bass == 100) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   actual_bass = 0;
  }
  
  if (pbmid == 1) {
   if (speed_digital == 1) {
     if (((1000 >= actual_mid - 4000) && (0 <= actual_mid - 4000)) || ((-1000 <= actual_mid - 4000) && (0 >= actual_mid - 4000))) {
        count_gd_bd(1,0);     
     }
     else if (actual_mid == 5000) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }  
   }
   if (speed_digital == 2) {
     if (((500 >= actual_mid - 2000) && (0 <= actual_mid - 2000)) || ((-500 <= actual_mid - 2000) && (0 >= actual_mid - 2000))) {
        count_gd_bd(1,0);
     }
     else if (actual_mid == 2500) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 3) {
     if (((200 >= actual_mid - 800) && (0 <= actual_mid - 800)) || ((-200 <= actual_mid - 800) && (0 >= actual_mid - 800))) {
        count_gd_bd(1,0);
     }
     else if (actual_mid == 1000) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 4) {
     if (((100 >= actual_mid - 400) && (0 <= actual_mid - 400)) || ((-100 <= actual_mid - 400) && (0 >= actual_mid - 400))) {
        count_gd_bd(1,0);                                     
     }
     else if (actual_mid == 500) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 5) {
     if (((20 >= actual_mid - 80) && (0 <= actual_mid - 80)) || ((-20 <= actual_mid - 80) && (0 >= actual_mid - 80))) {
        count_gd_bd(1,0);
     }
     else if (actual_mid == 100) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   actual_mid = 0;
  }
 
 if (pbtreble == 1) {
   if (speed_digital == 1) {
     if (((1000 >= actual_treble - 4000) && (0 <= actual_treble - 4000)) || ((-1000 <= actual_treble - 4000) && (0 >= actual_treble - 4000))) {
        count_gd_bd(1,0);     
     }
     else if (actual_mid == 5000) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }  
   }
   if (speed_digital == 2) {
     if (((500 >= actual_treble - 2000) && (0 <= actual_treble - 2000)) || ((-500 <= actual_treble - 2000) && (0 >= actual_treble - 2000))) {
        count_gd_bd(1,0);
     }
     else if (actual_mid == 2500) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 3) {
     if (((200 >= actual_treble - 800) && (0 <= actual_treble - 800)) || ((-200 <= actual_treble - 800) && (0 >= actual_treble - 800))) {
        count_gd_bd(1,0);
     }
     else if (actual_mid == 1000) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 4) {
     if (((100 >= actual_treble - 400) && (0 <= actual_treble - 400)) || ((-100 <= actual_treble - 400) && (0 >= actual_treble - 400))) {
        count_gd_bd(1,0);                                     
     }
     else if (actual_mid == 500) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   if (speed_digital == 5) {
     if (((20 >= actual_treble - 80) && (0 <= actual_treble - 80)) || ((-20 <= actual_treble - 80) && (0 >= actual_treble - 80))) {
        count_gd_bd(1,0);
     }
     else if (actual_mid == 100) {
      count_gd_bd(0,1);
     }
     else {
        count_gd_bd(0,1);
     }
   }
   actual_treble = 0;
  }
}



/*
***********************************************************************   
Time delay for approximately 1 second		 		  		
***********************************************************************
*/

 void tdelay() {
  count_time_delay = 0;
  if (tim_start == 1) {
    while (count_time_delay < 1000) {
    }
  } 
  else {
    
    if (speed_digital == 1) {
     while (count_time_delay < 1000) {
      }
   }
   
   else if (speed_digital == 2) {
     while (count_time_delay < 500) {
     }
   }
   else if (speed_digital == 3) {
     while (count_time_delay < 200) {
     }
   }
   else if (speed_digital == 4) {
     while (count_time_delay < 100) {
     }
   }
   else if (speed_digital == 5) {
     while (count_time_delay < 20) {
     }
   }
  }
 }
 
/*
***********************************************************************   
Time delay for each corresponding speed		 		  		
***********************************************************************
*/

 void tdelay_speed(int speed_digital) {
  count_speed = 0;
   if (speed_digital == 1) {    
     while (count_speed < 500) {
     }
   } 
   else if (speed_digital == 2) {
     while (count_speed < 250) {
     }
   }
   else if (speed_digital == 3) {
     while (count_speed < 100) {
     }
   } 
   else if (speed_digital == 4) {
     while (count_speed < 50) {
     }
   }
   else if (speed_digital == 5) {
     while (count_speed < 10) {
     }
   }
 }
/*
***********************************************************************
  shiftout: Transmits the character x to external shift 
            register using the SPI.  It should shift MSB first.  
             
            MISO = PM[4]
            SCK  = PM[5]
***********************************************************************
*/
 
void shiftout(char x)

{
 
  // read the SPTEF bit, continue if bit is 1
  // write data to SPI data register
  // wait for 30 cycles for SPI data to shift out 
  
  while (!SPISR_SPTEF) {
  }
  
  SPIDR = x;
  cycle = 30;
  
  while (cycle) {
    cycle--;
  }
}

/*
***********************************************************************
  lcdwait: Delay for approx 2 ms
***********************************************************************
*/

void lcdwait()
{
  int a = 8000;
  while (a) {
    a--;
  }
}

/*
*********************************************************************** 
  send_byte: writes character x to the LCD
***********************************************************************
*/

void send_byte(char x)
{
     // shift out character
     // pulse LCD clock line low->high->low
     // wait 2 ms for LCD to process data
     shiftout(x);
     PTT_PTT4 = 0;
     PTT_PTT4 = 1;
     PTT_PTT4 = 0;
     lcdwait();
}

/*
***********************************************************************
  send_i: Sends instruction byte x to LCD  
***********************************************************************
*/

void send_i(char x)
{
        // set the register select line low (instruction data)
        // send byte
  PTT_PTT2 = 0;
  send_byte(x);
        
}

/*
***********************************************************************
  chgline: Move LCD cursor to position x
  NOTE: Cursor positions are encoded in the LINE1/LINE2 variables
***********************************************************************
*/

void chgline(char x)
{
  send_i(CURMOV);
  send_i(x);
}

/*
***********************************************************************
  print_c: Print (single) character x on LCD            
***********************************************************************
*/
 
void print_c(char x)
{
  PTT_PTT2 = 1;
  send_byte(x);
}

/*
***********************************************************************
  pmsglcd: print character string str[] on LCD
***********************************************************************
*/

void pmsglcd(char str[])
{
  int i = 0;
  
  while (str[i] != '\0') {
    print_c(str[i++]);    
  }
}


/*
***********************************************************************
 Character I/O Library Routines for 9S12C32 (for debugging only)
***********************************************************************
 Name:         inchar
 Description:  inputs ASCII character from SCI serial port and returns it
 Example:      char ch1 = inchar();
***********************************************************************
*/

char inchar(void) {
  /* receives character from the terminal channel */
        while (!(SCISR1 & 0x20)); /* wait for input */
    return SCIDRL;
}

/*
***********************************************************************
 Name:         outchar
 Description:  outputs ASCII character x to SCI serial port
 Example:      outchar('x');
***********************************************************************
*/

void outchar(char x) {
  /* sends a character to the terminal channel */
    while (!(SCISR1 & 0x80));  /* wait for output buffer empty */
    SCIDRL = x;
}