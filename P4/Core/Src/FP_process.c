/*
 * FP_process.c
 *
 *  Created on: Jun 5, 2023
 *      Author: james
 */

#include "main.h"
#include "FP_process.h"
#include "fingerprint.h"
#include "delay.h"
#include "lcd.h"

#define MAX_FP_PER_SECTION 2

void FP_enroll(int section_num) {
	/* Order of Operations
	 * 1. Get user to type name associated with FP
	 * 2. Get User Input twice
	 */
	genImg();	//to get ready to take an input

	//add section functionality here

	uint8_t mem_local = 0; //initializes mem local
	mem_local = section_num * 10 + mem_local;

	uint8_t number_of_fp_per_section = 0;

	while (number_of_fp_per_section < MAX_FP_PER_SECTION) {	//takes 10 user inputs: 0-9

		for (int buff_local = 1; buff_local <= 2; buff_local++) {
			while (ConfirmationCode == 0x02) {	//No finger detected get finger
				if (buff_local == 1) {
					lcd_set_cursor_position(0, 0);
					str_write("Place finger    ");
					lcd_set_cursor_position(1, 0);
					str_write("                ");
				} else if (buff_local == 2) {
					lcd_set_cursor_position(0, 0);
					str_write("Place finger ");
					lcd_set_cursor_position(1, 0);
					str_write("again         ");
				}
				genImg();	//checks fingerprint
			}
			Img2Tz(buff_local);
			while (ConfirmationCode == 0x00) { //to wait til user removes finger
				lcd_set_cursor_position(0, 0);
				str_write("Got Finger!     ");
				lcd_set_cursor_position(1, 0);
				str_write("Remove Finger   ");
				genImg();	//checks fingerprint
			}

		}
		regMode(); //generates FP template to store to memory checks if they match as well

		if (ConfirmationCode == 0x00) { //Fingerprint matches!
			lcd_set_cursor_position(0, 0);
			str_write("FP Match!   ID=");
			write(mem_local + '0');

			store(1, mem_local); //stores ID to memory
			mem_local++; //increments mem_local
			number_of_fp_per_section++; //increments total

			ConfirmationCode = 0x02;	//to get ready for input
			while ((ConfirmationCode == 0x02) && ~(number_of_fp_per_section == MAX_FP_PER_SECTION)) {	//No finger detected get finger
				genImg(); //checks fingerprint
				lcd_set_cursor_position(1, 0);
				str_write("Place New Finger");
			}
		} else {
			lcd_set_cursor_position(0, 0);
			str_write("FP mismatch :(  ");

			ConfirmationCode = 0x02;	//to get ready for input
			while (ConfirmationCode == 0x02) {	//No finger detected get finger
				genImg(); //checks fingerprint
				lcd_set_cursor_position(1, 0);
				str_write("Redo Input      ");
			}
		}
	}
	lcd_set_cursor_position(0, 0);
	str_write("Reached Max Reg ");
	lcd_set_cursor_position(1, 0);
	str_write(" For Section = ");
	write(section_num + '0');
}

