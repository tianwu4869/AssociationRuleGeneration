# AssociationRuleGeneration

## Objective
The project generates all association rules whose support is greater than a user-supplied minimum support and whose confidence is greater than a user supplied minimum confidence. The program should take as command line option five parameters: (i) the minimum support, (ii) the minimum confidence, (iii) the name of the input file, (iiii) the name of the output file, and (iv) options. The specific parameter sequence along with the name of the executable are as follows:

`hcrminer minsup minconf inputfile outputfile options`

The options parameter will be a numerical value whose meaning is as follows:

options = 1
  The numbering of the items coming from the input file is used as the lexicographical ordering of the items.

options = 2
  The lexicographical ordering of the items is determined by sorting the items in increasing frequency order in each projected database.

options = 3
    The lexicographical ordering of the items is determined by sorting the items in decreasing frequency order in each projected database.
