#!/bin/bash
# to convert .ps files to .pdf and then append all .pdfs to a single file
#  .pdf file

for file in *.ps; do
	ps2pdf $file
done
