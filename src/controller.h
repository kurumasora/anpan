#ifndef CONTROLLER_H
#define CONTROLLER_H

/*
 * Produces exactly target_count anpan. A target_count of zero means continuous
 * operation until an error or external interruption. Returns the total number
 * of completed anpan, or -1 on error.
 */
int RunProduction(int target_count);

#endif
