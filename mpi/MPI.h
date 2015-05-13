// This header file is for one purpose only:
// To include MPI without warnings.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "mpi.h"

// Re-enable warnings
#pragma GCC diagnostic pop
