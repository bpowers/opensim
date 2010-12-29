#===--- parser for boosd models ------------------------------------------===#
#
# Copyright 2010 Bobby Powers
# Licensed under the GPLv2 license
#
# see COPYING for details.
#===----------------------------------------------------------------------===#
import logging
import constants as sim
from errors import report_eqn_error

import antlr3
from boosdLexer import boosdLexer
from boosdParser import boosdParser

log = logging.getLogger('opensim.parse')



class Parser:
    '''
    Class to parse variable expressions and create well formed ASTs.
    '''

    def __init__(self, file_name):
        '''
        Initialize a parser for a compilation unit (typically a file)
        '''
        stream = antlr3.ANTLRFileStream(file_name)
        self._lex = boosdLexer(stream)
        tokens = antlr3.CommonTokenStream(self._lex)
        self._parser = boosdParser(tokens)

        self.kind = sim.UNDEF
        self.valid = False
        self.refs = []


    def parse(self):
        '''
        Parse our variables equation, making available pertinent information.

        Information includes:
        * a well-formed AST, if the equation is valid
        * any errors or warnings that were encountered
        * the lookup table, if applicable
        '''
        self._parser.compilation_unit()

if __name__ == '__main__':
    from sys import argv

    if len(argv) is not 2:
        print 'usage: %s filename' % argv[0]
        exit(1)

    parser = Parser(argv[1])
    parser.parse()
