/*
 * fingerprint.h
 *
 *  Created on: May 31, 2023
 *      Author: james
 */

#ifndef INC_FINGERPRINT_H_
#define INC_FINGERPRINT_H_

void FP_init(void);

//global variable
extern uint32_t ACK_LENGTH;


//instruction function declarations
void handshake(void);
void setAddr(uint32_t og_adr, uint32_t new_adr);
void SetSysPara(char param, char setting);
void templateNum(void);
void regMode(void);
void upChar(char buffID);
void downChar(char buffID);
void store(char buffID, uint16_t pageID);
void loadChar(char buffID, uint16_t pageID);
void deletChar(uint16_t pageID, uint16_t N);
void empty(void);
void match(void);
void search(char buffID, uint16_t startPage, uint16_t pageNum);
void genImg(void);
void Img2Tz(char buffID);


#endif /* INC_FINGERPRINT_H_ */
