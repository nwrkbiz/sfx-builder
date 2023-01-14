#ifndef PRIV_ESCALATE_H
#define PRIV_ESCALATE_H

/**
 * @brief Allows checking wether executable runs as admin/root or not.
 * @return true if executable runs as root, false otherwhise
 */
bool isElevated();

/**
 * @brief Execute with admin privileges (and ask for password).
 * @param argc argc used to run the executable
 * @param argv argv used to run the executable
 */
void elevatePrivilege(int argc, char **argv);

#endif //PRIV_ESCALATE_H