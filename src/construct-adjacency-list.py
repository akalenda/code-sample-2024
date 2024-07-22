
import sys
import re
import json

def compile_stdin_to_word_set():
  # Time complexity O(w) where w is the number of words piped in
  word_set = set()
  for line in sys.stdin:
    if line.rstrip() == 'Exit':
        break
    word_set.update(filter(None, re.split('\W+', line)))
  return word_set

"""
  Outputs a dictionary of prefix-suffix pairs that, by adding/omitting a single character,
  can produce a list of words. A couple sample entries would look like:
    pref_suff_dict["h*"] == ["hi", "h", "ha"]
    pref_suff_dict["h*a"] == ["ha"]
"""
def construct_pref_suff_dict(word_set):
  # Time complexity O(c) where c is the number of characters in word_set
  pref_suff_dict = dict()
  for word in word_set:
    for i in range(0, len(word)+1):
      for pref_suff in [word[:i] + '*' + word[i:], word[:i] + '*' + word[i+1:]]:
        if not pref_suff in pref_suff_dict:
          pref_suff_dict[pref_suff] = []
        if not word in pref_suff_dict[pref_suff]:
          # TODO: We can probably construct the adjacency list here instead to optimize things
          pref_suff_dict[pref_suff].append(word)
  return pref_suff_dict

def construct_adjacency_list(pref_suff_dict):
  # Time complexity O(w^2) where w is the number of words in word_set
  adjacency_list = dict();
  for adjacent_words in pref_suff_dict.values():
    for i in range(0, len(adjacent_words)):
      a = adjacent_words[i]
      if not a in adjacency_list:
        adjacency_list[a] = []
      for j in range(i+1, len(adjacent_words)):
        b = adjacent_words[j]
        adjacency_list[a].append(b)
        if not b in adjacency_list:
          adjacency_list[b] = [] 
        adjacency_list[b].append(a)
  return adjacency_list

def main():
  word_set = compile_stdin_to_word_set()
  pref_suff_dict = construct_pref_suff_dict(word_set)
  adjacency_list = construct_adjacency_list(pref_suff_dict)
  print(json.dumps(adjacency_list, indent=4))

if __name__ == "__main__":
    main()
