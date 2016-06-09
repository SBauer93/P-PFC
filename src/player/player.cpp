/* -----------------------------------------------------------------
 * player.cpp
 *
 * Simple software player template.
 *
 * Author:  Jens Rudolf (Uni Rostock)
 * Date:    2015-29-05
 *
 *
 * Changelog:
 *      2015-29-05 - Initial version
 *
 *
 * Copyright (c) 2015 by Jens Rudolf (Uni Rostock). All rights
 * reserved. For educational use only. NO WARRANTY!
 *
 * Report bugs to:
 *      jens.rudolf@uni-rostock.de
 *
 * -----------------------------------------------------------------
 */

#include <fstream>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

typedef char sample_t;

int main(int argc, char *argv[]) {

    const sample_t Tokens[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmonpqrstuvwxyz"
                              "0123456789";

    if (argc < 3) {
        cerr << "ERROR: input and output files must be supplied!\n"
             << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
        return -1;
    }

    ofstream toFilter(argv[2]);
    ifstream fromFilter(argv[1]);

    for (int i = 0; Tokens[i] != '\0'; ++i) {
        // Write sample to filter
        toFilter.put(Tokens[i]);
        toFilter.flush();
        cout << "Player: >> " << Tokens[i] << endl;
    } 
    
    sample_t token;
    while (token != '8' && token != '9') { 
        fromFilter.get(token);
        cout << "Player: << " << token << endl;
    }

    return 0;
}
