#include "lcd1602_i2c_lib.h"
/*-----------------------------------Settings----------------------------------*/

#define Adress 0x27 << 1                //Device address.
extern I2C_HandleTypeDef hi2c1;         //I2C bus.
bool backlight = true;             //Initial setting for backlight on/off.
char lcd1602_tx_buffer[40] = { 0, }; //global data buffer. Text is written to it.
uint8_t global_buffer = 0; //global byte variable sent to the display.

/*-----------------------------------Settings----------------------------------*/

/*============================Support functions============================*/
/*-------------Function for sending data when initialising the display-------------*/
/// This function is designed to send a byte of data via the i2c bus. It contains Delay. Without it, the display will not initialise.
static void lcd1602_Send_init_Data(uint8_t *init_Data) {
	if (backlight) {
		*init_Data |= 0x08; //Turn on the backlight
	} else {
		*init_Data &= ~0x08; //Turn off the backlight
	}
	*init_Data |= 0x04; // Set strobe signal E to 1
	HAL_I2C_Master_Transmit(&hi2c1, Adress, init_Data, 1, 10);
	HAL_Delay(5);
	*init_Data &= ~0x04; // Set strobe signal E to 0
	HAL_I2C_Master_Transmit(&hi2c1, Adress, init_Data, 1, 10);
	HAL_Delay(5);
}
/*-------------Function for sending data when initialising the display-------------*/

/*--------------------Function for sending a byte of information to the display---------------*/
static void lcd1602_Write_byte(uint8_t Data) {
	HAL_I2C_Master_Transmit(&hi2c1, Adress, &Data, 1, 10);
}
/*--------------------Function for sending a byte of information to the display---------------*/

/*----------------------Function for sending half a byte of information--------------------*/
/// Function for sending half a byte of information
static void lcd1602_Send_cmd(uint8_t Data) {
	Data <<= 4;
	lcd1602_Write_byte(global_buffer |= 0x04); // Set strobe signal E to 1
	lcd1602_Write_byte(global_buffer | Data); // Send the received and shifted byte to the display
	lcd1602_Write_byte(global_buffer &= ~0x04);	// Set the strobe signal E to 0.
}
/*----------------------Function for sending half a byte of information--------------------*/

/*----------------------Data byte sending function----------------------------*/
/// Function for sending a data byte to the display
static void lcd1602_Send_data_symbol(uint8_t Data, uint8_t mode) {
	if (mode == 0) {
		lcd1602_Write_byte(global_buffer &= ~0x01); // RS = 0
	} else {
		lcd1602_Write_byte(global_buffer |= 0x01); // RS = 1
	}
	uint8_t MSB_Data = 0;
	MSB_Data = Data >> 4; // Shift the resulting byte by 4 positions and write it to the variable
	lcd1602_Send_cmd(MSB_Data);	// Send the first 4 bits of the received byte
	lcd1602_Send_cmd(Data);	   // Send the last 4 bits of the received byte
}
/*----------------------Data byte sending function----------------------------*/

/*----------------------The main function for sending data---------------------*/
/// The function is designed to send a byte of data via the i2c bus.
static void lcd1602_Send_data(uint8_t *Data) {

	if (backlight) {
		*Data |= 0x08;
	} else {
		*Data &= ~0x08;
	}
	*Data |= 0x04; // set strobe signal E to 1
	HAL_I2C_Master_Transmit(&hi2c1, Adress, Data, 1, 10);
	*Data &= ~0x04; // set strobe signal E to 0
	HAL_I2C_Master_Transmit(&hi2c1, Adress, Data, 1, 10);
}

/*----------------------The main function for sending data---------------------*/
/*============================Support functions============================*/

/*-------------------------Display initialisation function-------------------------*/
/// Display initialisation function
void lcd1602_Init(void) {
	/*========Power on========*/
	uint8_t tx_buffer = 0x30;
	/*========Wait for more than 15 ms after Vcc rises to 4.5V========*/
	HAL_Delay(15);
	/*========BF can not be checked before this instruction.========*/
	/*========Function set ( Interface is 8 bits long.========*/
	lcd1602_Send_init_Data(&tx_buffer);
	/*========Wait for more 4.1 ms========*/
	HAL_Delay(5);
	/*========BF can not be checked before this instruction.========*/
	/*========Function set ( Interface is 8 bits long.========*/
	lcd1602_Send_init_Data(&tx_buffer);
	/*========Wait for more 100 microsec========*/
	HAL_Delay(1);
	/*========BF can not be checked before this instruction.========*/
	/*========Function set ( Interface is 8 bits long.========*/
	lcd1602_Send_init_Data(&tx_buffer);

	/*========Enable the 4-bit interface========*/
	tx_buffer = 0x20;
	lcd1602_Send_init_Data(&tx_buffer);
	/*========Enable the 4-bit interface========*/

	/*======2 lines, font size 5x8======*/
	tx_buffer = 0x20;
	lcd1602_Send_init_Data(&tx_buffer);
	tx_buffer = 0x80;
	lcd1602_Send_init_Data(&tx_buffer);
	/*======2 lines, font size 5x8======*/

	/*========Switch off the display========*/
	tx_buffer = 0x00;
	lcd1602_Send_init_Data(&tx_buffer);
	tx_buffer = 0x80;
	lcd1602_Send_init_Data(&tx_buffer);
	/*========Switch off the display========*/

	/*========Clear display========*/
	tx_buffer = 0x00;
	lcd1602_Send_init_Data(&tx_buffer);
	tx_buffer = 0x10;
	lcd1602_Send_init_Data(&tx_buffer);
	/*========Clear display========*/

	/*========Cursor shift mode========*/
	tx_buffer = 0x00;
	lcd1602_Send_init_Data(&tx_buffer);
	tx_buffer = 0x30;
	lcd1602_Send_init_Data(&tx_buffer);
	/*========Cursor shift mode========*/

	/*========Initialisation complete. Switch on display========*/
	tx_buffer = 0x00;
	lcd1602_Send_init_Data(&tx_buffer);
	tx_buffer = 0xC0;
	lcd1602_Send_init_Data(&tx_buffer);
	/*========Initialisation complete. Switch on display========*/
}

/*-------------------------Display initialisation function-------------------------*/

/*-------------------------Function for displaying a character on the display---------------------*/
/// Function for displaying a character on the display
void lcd1602_Print_symbol(uint8_t symbol) {
	uint8_t command;
	command = ((symbol & 0xf0) | 0x09); //Forming the upper half-byte into a command for the display
	lcd1602_Send_data(&command);
	command = ((symbol & 0x0f) << 4) | 0x09; //Forming the lower half-byte into a command for the display
	lcd1602_Send_data(&command);
}
/*-------------------------Function for displaying a character on the display---------------------*/

/*-------------------------Text display function----------------------*/
/// Function for displaying a character on the display
/// The maximum length of a message is 40 characters.
void lcd1602_Print_text(char *message) {
	for (int i = 0; i < strlen(message); i++) {
		lcd1602_Print_symbol(message[i]);
	}
}
/*-------------------------Text display function----------------------*/

/*-------------------Cursor setting function for text output----------------*/
/// Cursor setting function for displaying text on the display
/// Visible area:
/// For displays 1602 max x = 15, max y = 1.
/// For displays 2004 max x = 19, max y = 3.
void lcd1602_SetCursor(uint8_t x, uint8_t y) {
	uint8_t command, adr;
	if (y > 3)
		y = 3;
	if (x > 39)
		x = 39;
	if (y == 0) {
		adr = x;
	}
	if (y == 1) {
		adr = x + 0x40;
	}
	if (y == 2) {
		adr = x + 0x14;
	}
	if (y == 3) {
		adr = x + 0x54;
	}
	command = ((adr & 0xf0) | 0x80);
	lcd1602_Send_data(&command);

	command = (adr << 4);
	lcd1602_Send_data(&command);

}
/*-------------------Cursor setting function for text output----------------*/

/*------------------------Move text to the left function-----------------------*/
/// Move text to the left function
/// If you repeat it periodically, you will get a ticker.
void lcd1602_Move_to_the_left(void) {
	uint8_t command;
	command = 0x18;
	lcd1602_Send_data(&command);

	command = 0x88;
	lcd1602_Send_data(&command);
}
/*------------------------Move text to the left function-----------------------*/

/*------------------------Move text to the right function----------------------*/
/// Move text to the right function
/// If you repeat it periodically, you will get a ticker.
void lcd1602_Move_to_the_right(void) {
	uint8_t command;
	command = 0x18;
	lcd1602_Send_data(&command);

	command = 0xC8;
	lcd1602_Send_data(&command);
}
/*------------------------Move text to the right function----------------------*/

/*---------------------Backlight on/off function--------------------*/
/// Boolean function for turning the backlight on/off
/// 1 - on, 0 - off
void lcd1602_Backlight(bool state) {
	if (state) {
		backlight = true;
	} else {
		backlight = false;
	}
}
/*---------------------Backlight on/off function--------------------*/

/*---------------------Function for creating your own symbol-------------------------- */
/// The function of creating your own symbol and saving it to memory.
void lcd1602_Create_symbol(uint8_t *my_Symbol, uint8_t memory_adress) {
	lcd1602_Send_data_symbol(((memory_adress * 8) | 0x40), 0);
	for (uint8_t i = 0; i < 8; i++) {
		lcd1602_Send_data_symbol(my_Symbol[i], 1); // We write data byte by byte to memory
	}
}
/*---------------------Function for creating your own symbol-------------------------- */

/*-------------------------Display cleaning function-------------------------------*/

void lcd1602_Clean(void) {
/// Hardware display cleaning function.
/// Deletes all text and returns the cursor to its initial position.
	uint8_t tx_buffer = 0x00;
	lcd1602_Send_init_Data(&tx_buffer);
	tx_buffer = 0x10;
	lcd1602_Send_init_Data(&tx_buffer);

}
/*-------------------------Display cleaning function-------------------------------*/

void lcd1602_Clean_Text(void) {
/// Alternative display cleaning function
/// Fills the entire memory field with spaces
/// Works faster than lcd1602_Clean, but unlike it, does not return the cursor to its initial position.
	lcd1602_SetCursor(0, 0);
	lcd1602_Print_text("                                        ");
	lcd1602_SetCursor(0, 1);
	lcd1602_Print_text("                                        ");
}
