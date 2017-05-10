/*
 * proof_of_work.h
 *
 *  Created on: 10/may/2017
 *      Author: Federico Lombardi
 */

#ifndef PROOF_OF_WORK_H_
#define PROOF_OF_WORK_H_

#include <string>

extern int DEFAULT_DIFFICULTY;
extern int DEFAULT_THREAD_NUM;
extern int difficulty;
extern int threadNum;
extern int id;
extern int winner;

bool isValid(std::string hash);

#endif /* PROOF_OF_WORK_H_ */
