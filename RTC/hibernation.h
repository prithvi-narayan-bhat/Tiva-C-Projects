/**
 *      @file hibernation.h
 *      @author Prithvi Bhat
 *      @brief Configurations for the Hibernation module
 *      @date 2022-09-27
 **/

#include "tm4c123gh6pm.h"
#include "stdint.h"

#ifndef HIBERNATION_H_
#define HIBERNATION_H_

void init_hibernation_module(void);
void hibernate(uint32_t seconds);
uint32_t get_hibernation_wake_mode(void);

#endif /* HIBERNATION_H_ */
