/*
 * ReadMatrix.c
 *
 *  Created on: Jul 16, 2016
 *      Author: Nikita Wootten
 *
 *	Matrix for calculator's buttons is defined as follows:
 *	+---+-------+---------+------+-----------+------------+
 *	| ~ |   A   |    B    |  C   |     D     |     E      |
 *	+---+-------+---------+------+-----------+------------+
 *	| 0 | F1    | F2      | F3   | F4        | F5         |
 *	| 1 | 2ND   | MODE    | DEL  | LEFTARROW | UPARROW    |
 *	| 2 | ALPHA | X,t,O,n | Stat | DOWNARROW | RIGHTARROW |
 *	| 3 | MATH  | APPS    | PRGM | VARS      | CLEAR      |
 *	| 4 | X^-1  | SIN     | COS  | TAN       | ^          |
 *	| 5 | X^2   | ,       | (    | )         | /          |
 *	| 6 | LOG   | 7       | 8    | 9         | X          |
 *	| 7 | LN    | 4       | 5    | 6         | -          |
 *	| 8 | STO   | 1       | 2    | 3         | +          |
 *	| 9 | ON    | 0       | .    | (-)       | ENTER      |
 *	+---+-------+---------+------+-----------+------------+
 *
 *	All key and key combinations should be converted as follows:
 *	+---------------------+----------------------------------+
 *	| uinput ID Triggered | Key pressed                      |
 *	+---------------------+----------------------------------+
 *	| Shift               | STAT                             |
 *	| Caps Lock           | ALPHA                            |
 *	| Control             | X,T,0,n                          |
 *	| Alt                 | VARS                             |
 *	| Tab                 | 2nd + ' '                        |
 *	| Escape              | 2nd + CLEAR                      |
 *	| Backspace           | CLEAR                            |
 *	| Enter               | ENTER                            |
 *	| Space               | ' '                              |
 *	| Fn (F1-F20)         | F(n + 5 * (2nd?) + 10 * (MODE?)) |
 *	| Print Screen        | 2nd + RCL                        |
 *	| Insert              | 2nd + DEL                        |
 *	| Home                | 2nd + Left Arrow                 |
 *	| End                 | 2nd + Right Arrow                |
 *	| Page Down           | 2nd + Down Arrow                 |
 *	| Page Up             | 2nd + Up Arrow                   |
 *	| Delete              | DEL                              |
 *	| Num Lock            | 2nd + ALPHA                      |
 *	| Up Arrow            | Up Arrow                         |
 *	| Down Arrow          | Down Arrow                       |
 *	| Left Arrow          | Left Arrow                       |
 *	| Right Arrow         | Right Arrow                      |
 *	| num (0-9)           | MODE + num                       |
 *	| char (a-z)          | char                             |
 *	| char (A-Z)          | Shift + char                     |
 *	| !                   | 2nd + i (imaginary number)       |
 *	| @                   | 2nd + theta                      |
 *	| #                   | 2nd + pi                         |
 *	| $                   | 2nd + S (alphabet)               |
 *	| %                   | 2nd + / (division symbol)        |
 *	| ^                   | 2nd + ^                          |
 *	| &                   | 2nd + +                          |
 *	| *                   | MODE + x (multiplication symbol) |
 *	| (                   | MODE + (                         |
 *	| )                   | MODE + )                         |
 *	| ~                   | 2nd + MODE + -                   |
 *	| `                   | 2nd + MODE + "                   |
 *	| -                   | MODE + -                         |
 *	| _                   | MODE + (-)                       |
 *	| =                   | 2nd + (-)                        |
 *	| +                   | MODE + +                         |
 *	| [                   | 2nd + [                          |
 *	| ]                   | 2nd + ]                          |
 *	| {                   | 2nd + {                          |
 *	| }                   | 2nd + }                          |
 *	| |                   | 2nd + I (alphabet)               |
 *	| \                   | 2nd + MODE + / (division symbol) |
 *	| ;                   | Shift + :                        |
 *	| :                   | :                                |
 *	| ‘                   | Shift + "                        |
 *	| “                   | "                                |
 *	| ,                   | MODE + ,                         |
 *	| .                   | MODE + .                         |
 *	| <                   | 2nd + MODE + (                   |
 *	| >                   | 2nd + MODE + )                   |
 *	| /                   | MODE + / (division symbol)       |
 *	| ?                   | ?                                |
 *	+---------------------+----------------------------------+
 */
