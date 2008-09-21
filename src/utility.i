/*
    Cifer: Automating classical cipher cracking in C
    Copyright (C) 2008  Daniel Richman & Simrun Basuita

    Cifer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cifer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cifer.  If not, see <http://www.gnu.org/licenses/>.
*/

INSERTION_DEFINE(insertion_columnic_sort, vigenere_column_ic, column_ic_diff)
INSERTION_DEFINE(insertion_digram_sort,   digram,             digram_value)
INSERTION_DEFINE(insertion_trigram_sort,  trigram,            trigram_value)
INSERTION_DEFINE(insertion_randfreq_sort, rand_freq,          diff)
INSERTION_DEFINE(ga_natural_selection,    ga_parent_list,     parent_score)

