#!/bin/sh

dot -Tpdf AST_def_node.dot -o arbre.pdf
firefox arbre.pdf
