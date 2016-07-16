#!/bin/bash

# Temporary file to store all files we want to delete.
toDelete=$(mktemp)

# Remove all files that do not contain the max flow result.
grep "Max flow is" -L $@ >> "$toDelete"

# Remove all the files with a max flow of zero.
grep "Max flow is 0\$" -l $@ >> "$toDelete"

# Remove the files.
sort -u "$toDelete" | xargs rm

# Remove the temporary.
rm $toDelete
