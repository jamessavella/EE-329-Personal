/*
 * FP_process.h
 *
 *  Created on: Jun 5, 2023
 *      Author: james
 */

#ifndef INC_FP_PROCESS_H_
#define INC_FP_PROCESS_H_

//function declarations
void FP_enroll(void);
int FP_search(void);
void FP_check(void);

//global variable
extern uint8_t MEM_GLOBAL;	//FP ID# LOCALATION IN FLASH MEMORY
extern uint16_t FP_ID; //FOUND FP ID# LOCATION IN FLASH MEMORY

#endif /* INC_FP_PROCESS_H_ */
