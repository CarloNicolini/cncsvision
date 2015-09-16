# Cython imports
import numpy
from libcpp.string cimport string
from libcpp.map cimport map
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from libcpp cimport bool

import cython
# import both numpy and the Cython declarations for numpy
import numpy as np
cimport numpy as np

ctypedef map[string, string] params_map

cdef extern from "BalanceFactor.h":
    cdef cppclass BalanceFactor[T]:
        BalanceFactor()
        void init(unsigned int _repetitions, int _randomize, bool _randomizeWithoutConsecutive, unsigned int _maxReinsertion )
        void addFactor(string name, vector[T] curFactor)
        bool next()
        bool hasNext()
        void pyprint()
        map[string, string] getNextAsMapToString()
        map[string, string] getCurrentAsMapToString()
        map[string, T] getNext()
        map[string, T] getCurrent()
        map[string, T] getPrevious()
        deque[map[string, T]] getFullExtractions()
        void debrujinize(int subsequencelength, int ndebrujinrepetitions, int prerandomize)



cdef class balancefactor(object):
    cdef BalanceFactor[string] *thisptr # hold a C++ instance which we're wrapping
    ndebrujinrepetitions, subsequencelength, prerandomize = None, None, None
    def __cinit__(self, **kwargs):
        self.thisptr = new BalanceFactor[string]()
        nrep, randmethod, randwithoutconsecutive, maxreinsert = 1,0,0,0

        if kwargs.get('randmethod',None) is None:
            randmethod = 0
        elif kwargs.get('randmethod',None) is 'extrablock':
            randmethod = 1
        elif kwargs.get('randmethod',None) is 'intrablock':
            randmethod = 2
        elif kwargs.get('randmethod',None) is 'noconsecutive':
            randmethod = 3
        elif kwargs.get('randmethod',None) is 'debrujin':
            randmethod = 4
        else:
            raise Exception('Non supported randomization method, supported are \'extrablock\',\'intrablock\',\'noconsecutive\',\'debrujin\'')

        randwithoutconsecutive = kwargs.get('randwithoutconsecutive',0)
        try:
            maxreinsert = int(kwargs.get('maxreinsert',0))
            nrep = int(kwargs.get('repetitions',1))
        except:
            raise
        self.thisptr.init(nrep,randmethod,randwithoutconsecutive,maxreinsert)

    def __dealloc__(self):
        del self.thisptr

    def add_factor(self, name, list levels):
      self.thisptr.addFactor(name, levels)

    def __str__(self):
      self.thisptr.pyprint()
      return ''

    def next(self):
        if not self.thisptr.hasNext():
            raise StopIteration('Can\'t iterate on empty factor list.')
        else:
          return self.thisptr.getNext()

    def debrujinize(self, int subsequencelength, int ndebrujinrepetitions, bool prerandomize):
          self.thisptr.debrujinize(subsequencelength, ndebrujinrepetitions, prerandomize)

    def iteritems(self):
      if not self.thisptr.hasNext():
            raise StopIteration('Can\'t iterate on empty factor list.')
      while self.thisptr.hasNext():
          yield self.thisptr.getNext()
