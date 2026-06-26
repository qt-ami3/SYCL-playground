      h.parallel_for(range<1>(size), [=](id<1> i) { // is the same as main.cpp's for loop.
          A_acc[i] = i;
      });

      is just a for loop that divides tasks among cores/threads evenly

## Current issue

Your sort check algorithim only check for the next element so it only checs for sorted pairs.
