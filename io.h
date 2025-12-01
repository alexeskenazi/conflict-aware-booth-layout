#ifndef IO_H
#define IO_H

#include "model.h"
#include <string>

using namespace std;

// Read an instance from a file
Instance ReadInstance(const string& filename, const Params& params);

// Write a layout grid to a file
void WriteLayout(const string& filename, const Layout& layout);

// Print layout to stdout
void PrintLayout(const Layout& layout);

#endif
