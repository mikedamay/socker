//
// Created by mike on 3/3/17.
//

#ifndef SOCKER_USAGE_H_H
#define SOCKER_USAGE_H_H
/**
 * checks on whether the command line action relates to the operation implemented by the caller
 * @param argc number of command line parameters typically
 * @param argv command line parameters "<progname> <action> <host name> <port>"
 * @param action e.g. "connect", "bind" etc.
 * @param usageText usage message to be shown if usage test fails.  This does not include the word
 *        "usage" e.g. "socker connect localhost 80"
 * @param usageStr location in which pass out the usage text here
 * @param usageLen size of buffer to pass out the usage
 * @return true means that the usage is correct for this operation
 */
bool usage(int argc, char ** argv, char * action, char * usageText, char * usageStr, size_t usageLen);

#endif //SOCKER_USAGE_H_H
