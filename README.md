# mult_-_singel-C
multithreading c vs single thread performance

This project implements and evaluates the performance of multithreaded
sorting and searching algorithms on a large dataset of student records in C.
Performance was tested on a resource-constrained system :
• Sorting by average grade using a multithreaded Merge Sort.
• Sorting by name using a multithreaded Bitonic Sort.
• Searching for students by name using a multithreaded linear search.
• Merge Sort is a stable, divide-and-cotime complexity with guaranteed
 O(n log n) time complexity in all cases.
ped

• The top-level array is split into two halves.
• Each half is processed by a separate thread, but thread creation
 is capped at MAX_T * 2 to avoid overhead.
• After both threads complete, the sorted halves are merged.
Multithreading Strategy
 O(log n)
 O( n)
 O(n log n)
Advantages: Stable, predictable performance, cache-friendly (with buffer optimization).
Sorting by Name: Multithreaded Bitonic Sort
• Bitonic Sort is a comparison-based sorting network designed
 for parallel hardware (e.g., GPUs, FPGAs).
• It constructs bitonic sequences (monotonically increasing then decreasing)
 and merges them into a sorted sequence.
• Its complexity is O(n log² n).
Multithreading Strategy
• The innermost loop (over indices i) is split between 2 threads.
• Each thread processes independent index ranges
 to avoid race conditions.
• Threads synchronize via pthread_join after each sorting stage.
int cmp_name (const void *a, const void *b) {
return strcmp((( struct student*) a ->name, ) (( ) ) ); struct student* b ->name
}
 


Multithreaded Linear Search
Average O(n / p) (p = thread count)
Worst O(n) (element not found)
Bitonic Sort is not stable and should only be used for educational purposes
 on CPUs.
• A linear scan through the array to find all occurrences of a target name.
• The array is split into num_threads
• contiguous chunks, each processed by a dedicated thread.
• A global mutex (result_mutex) protects concurrent writes to the
 result list.
• The search stops early if results are found (optional, not fully
 implemented).
• Results are stored in a dynamically resized array (realloc).
Time Complexity
 Note:
Searching by Name:
Multithreading Strategy
Best O(1) (first element)
3. Performance Analysis
Test Environment
After testing, we observed that Multi_Thread was two times faster than Single_Thread,
especially when the number of students increased.
We conclude that using multiple threads is excellent for saving time and accomplishing
more than one task,
unlike programs that use only a main thread,which will be slower,especially when dealing
with large amounts of data.
