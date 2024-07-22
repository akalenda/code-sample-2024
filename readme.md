# Purpose

This is sample code produced for job interviews.

# Problem

https://leetcode.com/problems/word-ladder/

I chose this problem to create a Python code sample.

I wanted something that leaned into Python's strength: string processing!

This particular problem attracted me because it is also clearly a graph problem.
I decided to do a harder variation where the word list is arbitrary. So you could follow
a word path like, hi>hit>hot>hoh>oh

In addition, I want to provide a C++ sample, showing some multithreading and
use of pointers. So instead of just finding a single path, I will make a structure
that will memoize *all* such paths, in parallel.

A bit of NodeJS won't hurt as well, so the final step where I choose a path to
trace out using the structure built in C++ will be in Javascript!
(Typescript compilation would be a bit much for a small code sample like this.)

And finally, a little bit of BASH to pipe it all together.

# Run the solution

execute `sh ./runMe.sh` in a Linux environment

It assumes you have installed `python3`, `g++`, `nodejs`

# Thought Process

Our objective is to build a bidirectional adjacency graph that connects one word to the next:

```
  hi---hit---hot---hoh---oh    
            / | \    
            /  |  \    
          /   |   \    
        lot--dot--dog    
          \      / |    
            log--<  |    
                  \ |    
                  cog    
```

As for building it: At first blush I thought, "a trie?"

```
           ___________|__________    
          /       /      \   \   \    
        h       d        l   c   o    
        / \       \       \   \    \    
      hi  ho     do      lo   co  oh    
      /   / \    /  \    /  \   \    
  hit hoh hot dot dog lot log cog    
```

No, this doesn't really help. There's a huge jump between hoh and oh.
And even if we stuck to the constraint of 3-letter words, which puts them all on
the same level of the trie, there's no intuitive connection between dog and cog.
And even if we had both this prefix trie and a postfix trie, that wouldn't help,
because the character that needs changing can be smack-dab in the middle of the word.
But what if we had a, ah, pre-or-postfix trie, where the words can be constructed
starting from any character?

```
          _________________________________________|_________________________________________________    
          /          /      /                          \                    \          \        \     \    
        _h_         i    __t___             ____________o_____________       d          g        l     c    
      / / \        |   /      \           /      |      |    \   \   \      |          |        |     |    
    oh hi   ho    hi  it     _ot_        ho      do     lo    co  oh  og    do       __og_      lo    co    
    /  /   /  \    |  |     /  | \      / \     / \     / \   |   |   |    /  \     /  |  \     / \   |    
  hoh hit hoh hot hit hit hot dot lot hot hoh dot dog lot log cog hoh dog dot dog log dog cog lot log cog    
```

At first this may look promising, but there is no clear pattern that works in all the edges we would want to
construct on an adjacency graph. For example, hit and hot are only one character apart, but there are no connections
that illustrate this.

It looks like we can't take a string-builder approach. So let's consider a brute force approach.
Let's consider sample input [hit, hot, hi, hits].
For each character, it will look to see if it can swap or omit that to find another word.
On each character edge, it will look to see if it can insert a character to find another word.

```
hit => *h i t  => hit    
    =>  * i t  => hit    
    =>  h*i t  => hit    
    =>  h * t  => hit, hot    
    =>  h i *  => hit, hi    
    =>  h i t* => hit, hits    
hot => *h o t  => hot    
    =>  * o t  => hot    
    =>  h*o t  => hot    
    =>  h * t  => hot, hit    
    =>  h o*t  => hot    
    =>  h o *  => hot    
    =>  h o t* => hot    
hi => *h i  => hi    
    =>  * i  => hi    
    =>  h*i  => hi    
    =>  h *  => hi    
    =>  h i* => hi, hit    
hits => *h i t s  => hits    
      =>  * i t s  => hits    
      =>  h*i t s  => hits    
      =>  h * t s  => hits    
      =>  h i*t s  => hits    
      =>  h i * s  => hits    
      =>  h i t*s  => hits    
      =>  h i t *  => hits, hit    
      =>  h i t s* => hits    
```

Our sample input only has letters added or omitted at the end, but we can see that it would also
work at the beginning, e.g. [his, this], or in the middle, e.g. [hut, hurt].

This is excellent. We've got the adjacency list right there, and can either construct a graph from
it, or use it directly.

```
  hit > hit hot hi hits    
  hot > hit    
  hi > hit    
  hits > hit    
    
    hit---hot    
      | \    
    hi  hits    
```

We can see some memoization available here. hit and hot both consider h*t, resulting in [hit, hot].
Essentially, our dict/hashtable would be the middle column (prefix*suffix pairs) and the right
column (outgoing edges).

It might seem absurd to memoize, and it would certainly eat up lots of memory, but if our word
list was the most commonly used 1000 words in English, there would be a dense network of connections.
Even more so if our characters were, say, hiragana or katakana!

Once we've got the graph, we can use breadth-first search to find the shortest path. However, I am going
to move beyond this LeetCode problem and consider what the situation would be like in the wild. Chances
are, this isn't one-and-done. Chances are that this graph is going to be used multiple times. Therefore,
consider the following graph:
```
hi---hit---hot---hoh---oh    
          /  |    
         /   |    
        /    |    
      lot--dot--dog    
         \      / |    
          log--<  |    
                \ |    
                cog    
```
If we complete dot>>oh, we'll have explored a substantial portion of the graph, likely finding
the shortest paths for dot>>lot, dot>>dog, dot>>log, lot>>hot, dog>>hot, and more. In other words,
breadth-first search naturally lends itself towards memoization, by creating a minimum spanning tree.
So why not go ahead and do that for each vertex? Then we can refer to it in subsequent runs. Storing
each shortest path in the graph would explode our memory consumption, but each minimum spanning tree
isn't so bad, and that'd reduce the time complexity of a lookup to O(lg(n)) in subsequent runs.

On a backend server, we can simply cache it in memory. But I'm here to show code samples for Python,
and C++ (maybe BASH too), so I'll store it on disk.

# Summary
- In BASH, give word list via stdin
- In Python3, use prefix*suffix memoization
- Construct adjacency list
- Write as JSON to stdout
- In BASH, tee to json file
- In C++, read adjacency list
- Construct minimum spanning trees, concurrently
- Write as JSON to stdout
- In BASH, tee to json file
- In NodeJS, sample the minimum spanning trees to find a shortest path for a given source+destination
- In BASH, tee to json file

# Conclusion

It could use unit tests. It shouldn't be prone to edge case problems, but there might be some
issues with really large word sets, stupendously long words, or international characters.
It also has not been fine-tuned for performance, which was not a goal here. Documentation
could definitely use a polish pass.
