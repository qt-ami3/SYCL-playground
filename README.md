## June 23rd

Today I tried to get the logic of a double sort down in single task. something is wrong with the indexing of my sorting algorithim but it compiles on occasionally ends which I dont quite understand.

# Next steps:

Once I can actually get the algorithim to sort properely on one gpu thread I will move on to in parrelel, it seems like it should be where I am starting but learning the paradigms of SYCL has been quite challending for me.

## June 25th 

Still working on the practice in the sorting algorithim folder. Rather idiotically of me, I had my sort checking loop run inside of my sorting loop so it ended up running a check of the entire array every loop which is a begginer mistake I should not be making but that sorta just comes with the territory of working with c++ I guess.

What is completely my fualt is some incorrect indexing which I had set up as if the arrays were indexxing up, which they are not, and this is really stupid of me because I made the intentional decision of having it set up that way. Overall it still does no compile so im kinda lost but I patched out some bugs that should allow me to focus on nailing down my logic pretty soon here.

## June 25th (except like its actually day time now)

I never did end up figuring out why my code was not working, after getting many eyes and rigerously cross examing code by all account it was correct, however I ended up rewriting the sorting algorithim to count up from zero and use A_acc.size() as the target. During this rewrite I also figured out I could just have a boolean set to false once per sort per loop so that way if the sort didnt happen the loop would just know without having to count the array so overall the rewrite was probably a good thing.

./sortingAlgo/practice/main.cpp --> singleThread.cpp

```
      h.single_task([=]() {
        int bin;

        while (true) {
        bool sorted = true;
          for (int i = 0; i < (A_acc.size() - 1); i++) {
            if (A_acc[i] > A_acc[i + 1]) {
              int bin = A_acc[i];
              A_acc[i] = A_acc[i + 1];
              A_acc[i + 1] = bin;
              sorted = false;
            }
          }

          if (sorted) {break;}
        }
      });
    });
```
