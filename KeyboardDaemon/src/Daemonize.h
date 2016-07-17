/*
 * Daemonize.h
 *
 *  Created on: Jul 16, 2016
 *      Author: Nikita Wootten
 */

#ifndef DAEMONIZE_H_
#define DAEMONIZE_H_

int start_daemon(void);
int kill_deamon(void);
int unlock_pid(void);

#endif /* DAEMONIZE_H_ */
