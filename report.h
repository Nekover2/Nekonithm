#ifndef REPORT_H
#define REPORT_H

/*
    \brief Report keys pressed to the game (or computer) using USB HID
*/
void report_data(bool* keys, int size);
#endif // REPORT_H